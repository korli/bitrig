/*	$OpenBSD: sh_machdep.c,v 1.3 2006/10/06 23:15:12 mickey Exp $	*/
/*	$NetBSD: sh3_machdep.c,v 1.59 2006/03/04 01:13:36 uwe Exp $	*/

/*-
 * Copyright (c) 1996, 1997, 1998, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum and by Jason R. Thorpe of the Numerical Aerospace
 * Simulation Facility, NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*-
 * Copyright (c) 1982, 1987, 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)machdep.c	7.4 (Berkeley) 6/3/91
 */

#include <sys/param.h>
#include <sys/systm.h>

#include <sys/buf.h>
#include <sys/exec.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/signalvar.h>
#include <sys/syscallargs.h>
#include <sys/user.h>
#include <sys/sched.h>
#include <sys/msg.h>

#include <uvm/uvm_extern.h>

#include <dev/cons.h>

#include <sh/cache.h>
#include <sh/clock.h>
#include <sh/locore.h>
#include <sh/mmu.h>
#include <sh/trap.h>
#include <sh/intr.h>

#ifdef  NBUF
int	nbuf = NBUF;
#else
int	nbuf = 0;
#endif

#ifndef BUFCACHEPERCENT
#define BUFCACHEPERCENT 5
#endif

#ifdef  BUFPAGES
int	bufpages = BUFPAGES;
#else
int	bufpages = 0;
#endif
int	bufcachepercent = BUFCACHEPERCENT;

/* Our exported CPU info; we can have only one. */
int cpu_arch;
int cpu_product;
char cpu_model[120];

struct vm_map *exec_map;
struct vm_map *phys_map;

int physmem;
struct user *proc0paddr;	/* init_main.c use this. */
struct pcb *curpcb;
struct md_upte *curupte;	/* SH3 wired u-area hack */

#define	VBR	(uint8_t *)SH3_PHYS_TO_P1SEG(IOM_RAM_BEGIN)
vaddr_t ram_start = SH3_PHYS_TO_P1SEG(IOM_RAM_BEGIN);
/* exception handler holder (sh/sh/vectors.S) */
extern char sh_vector_generic[], sh_vector_generic_end[];
extern char sh_vector_interrupt[], sh_vector_interrupt_end[];
#ifdef SH3
extern char sh3_vector_tlbmiss[], sh3_vector_tlbmiss_end[];
#endif
#ifdef SH4
extern char sh4_vector_tlbmiss[], sh4_vector_tlbmiss_end[];
#endif

caddr_t allocsys(caddr_t);

/*
 * These variables are needed by /sbin/savecore
 */
uint32_t dumpmag = 0x8fca0101;	/* magic number */
int dumpsize;			/* pages */
long dumplo;	 		/* blocks */

void
sh_cpu_init(int arch, int product)
{
	int i;

	/* CPU type */
	cpu_arch = arch;
	cpu_product = product;

#if defined(SH3) && defined(SH4)
	/* Set register addresses */
	sh_devreg_init();
#endif
	/* Cache access ops. */
	sh_cache_init();

	/* MMU access ops. */
	sh_mmu_init();

	/* Hardclock, RTC initialize. */
	machine_clock_init();

	/* ICU initiailze. */
	intc_init();

	/* Exception vector. */
	memcpy(VBR + 0x100, sh_vector_generic,
	    sh_vector_generic_end - sh_vector_generic);
#ifdef SH3
	if (CPU_IS_SH3)
		memcpy(VBR + 0x400, sh3_vector_tlbmiss,
		    sh3_vector_tlbmiss_end - sh3_vector_tlbmiss);
#endif
#ifdef SH4
	if (CPU_IS_SH4)
		memcpy(VBR + 0x400, sh4_vector_tlbmiss,
		    sh4_vector_tlbmiss_end - sh4_vector_tlbmiss);
#endif
	memcpy(VBR + 0x600, sh_vector_interrupt,
	    sh_vector_interrupt_end - sh_vector_interrupt);

	if (!SH_HAS_UNIFIED_CACHE)
		sh_icache_sync_all();

	__asm volatile("ldc %0, vbr" :: "r"(VBR));

	/* kernel stack setup */
	__sh_switch_resume = CPU_IS_SH3 ? sh3_switch_resume : sh4_switch_resume;

	/* Set page size (4KB) */
	uvm_setpagesize();
}

/*
 * void sh_proc0_init(void):
 *	Setup proc0 u-area.
 */
void
sh_proc0_init()
{
	struct switchframe *sf;
	vaddr_t u;

	/* Steal process0 u-area */
	u = uvm_pageboot_alloc(USPACE);
	memset((void *)u, 0, USPACE);

	/* Setup proc0 */
	proc0paddr = (struct user *)u;
	proc0.p_addr = proc0paddr;
	/*
	 * u-area map:
	 * |user| .... | .................. |
	 * | PAGE_SIZE | USPACE - PAGE_SIZE |
         *        frame top        stack top
	 * current frame ... r6_bank
	 * stack top     ... r7_bank
	 * current stack ... r15
	 */
	curpcb = proc0.p_md.md_pcb = &proc0.p_addr->u_pcb;
	curupte = proc0.p_md.md_upte;

	sf = &curpcb->pcb_sf;
	sf->sf_r6_bank = u + PAGE_SIZE;
	sf->sf_r7_bank = sf->sf_r15	= u + USPACE;
	__asm volatile("ldc %0, r6_bank" :: "r"(sf->sf_r6_bank));
	__asm volatile("ldc %0, r7_bank" :: "r"(sf->sf_r7_bank));

	proc0.p_md.md_regs = (struct trapframe *)sf->sf_r6_bank - 1;
#ifdef KSTACK_DEBUG
	memset((char *)(u + sizeof(struct user)), 0x5a,
	    PAGE_SIZE - sizeof(struct user));
	memset((char *)(u + PAGE_SIZE), 0xa5, USPACE - PAGE_SIZE);
#endif /* KSTACK_DEBUG */
}

void
sh_startup()
{
	u_int loop;
	vaddr_t minaddr, maxaddr;
	caddr_t sysbase;
	caddr_t size;
	vsize_t bufsize;
	int base, residual;

	printf("%s", version);
	if (*cpu_model != '\0')
		printf("%s", cpu_model);
#ifdef DEBUG
	printf("general exception handler:\t%d byte\n",
	    sh_vector_generic_end - sh_vector_generic);
	printf("TLB miss exception handler:\t%d byte\n",
#if defined(SH3) && defined(SH4)
	    CPU_IS_SH3 ? sh3_vector_tlbmiss_end - sh3_vector_tlbmiss :
	    sh4_vector_tlbmiss_end - sh4_vector_tlbmiss
#elif defined(SH3)
	    sh3_vector_tlbmiss_end - sh3_vector_tlbmiss
#elif defined(SH4)
	    sh4_vector_tlbmiss_end - sh4_vector_tlbmiss
#endif
	    );
	printf("interrupt exception handler:\t%d byte\n",
	    sh_vector_interrupt_end - sh_vector_interrupt);
#endif /* DEBUG */

	printf("real mem = %u (%uK)\n", ctob(physmem), ctob(physmem) / 1024);

	/*
	 * Find out how much space we need, allocate it,
	 * and then give everything true virtual addresses.
	 */
	size = allocsys(NULL);
	sysbase = (caddr_t)uvm_km_zalloc(kernel_map, round_page((vaddr_t)size));
	if (sysbase == 0)
		panic("sh_startup: no room for system tables; %d required",
		    (u_int)size);
	if ((caddr_t)((allocsys(sysbase) - sysbase)) != size)
		panic("cpu_startup: system table size inconsistency");

	/*
	 * Now allocate buffers proper.  They are different than the above
	 * in that they usually occupy more virtual memory than physical.
	 */
	bufsize = MAXBSIZE * nbuf;
	if (uvm_map(kernel_map, (vaddr_t *)&buffers, round_page(bufsize),
	    NULL, UVM_UNKNOWN_OFFSET, 0,
	    UVM_MAPFLAG(UVM_PROT_NONE, UVM_PROT_NONE, UVM_INH_NONE,
	    UVM_ADV_NORMAL, 0)) != 0)
		panic("sh_startup: cannot allocate UVM space for buffers");
	minaddr = (vaddr_t)buffers;
	/* don't want to alloc more physical mem than needed */
	if ((bufpages / nbuf) >= btoc(MAXBSIZE))
		bufpages = btoc(MAXBSIZE) * nbuf;

	base = bufpages / nbuf;
	residual = bufpages % nbuf;
	for (loop = 0; loop < nbuf; ++loop) {
		vsize_t curbufsize;
		vaddr_t curbuf;
		struct vm_page *pg;

		/*
		 * Each buffer has MAXBSIZE bytes of VM space allocated.  Of
		 * that MAXBSIZE space, we allocate and map (base+1) pages
		 * for the first "residual" buffers, and then we allocate
		 * "base" pages for the rest.
		 */
		curbuf = (vaddr_t) buffers + (loop * MAXBSIZE);
		curbufsize = NBPG * ((loop < residual) ? (base+1) : base);

		while (curbufsize) {
			pg = uvm_pagealloc(NULL, 0, NULL, 0);
			if (pg == NULL)
				panic("sh_startup: not enough memory for buffer cache");

			pmap_kenter_pa(curbuf, VM_PAGE_TO_PHYS(pg),
			    VM_PROT_READ|VM_PROT_WRITE);
			curbuf += PAGE_SIZE;
			curbufsize -= PAGE_SIZE;
		}
	}
	pmap_update(pmap_kernel());

	/*
	 * Allocate a submap for exec arguments.  This map effectively
	 * limits the number of processes exec'ing at any time.
	 */
	exec_map = uvm_km_suballoc(kernel_map, &minaddr, &maxaddr,
	    16 * NCARGS, VM_MAP_PAGEABLE, FALSE, NULL);

	/*
	 * Allocate a submap for physio
	 */
	phys_map = uvm_km_suballoc(kernel_map, &minaddr, &maxaddr,
	    VM_PHYS_SIZE, 0, FALSE, NULL);

	/*
	 * Set up buffers, so they can be used to read disk labels.
	 */
	bufinit();

	printf("avail mem = %u (%uK)\n", ptoa(uvmexp.free),
	    ptoa(uvmexp.free) / 1024);
	printf("using %d buffers containing %u bytes (%uK) of memory\n",
	    nbuf, bufpages * PAGE_SIZE, bufpages * PAGE_SIZE / 1024);
}

/*
 * Allocate space for system data structures.  We are given
 * a starting virtual address and we return a final virtual
 * address; along the way we set each data structure pointer.
 *
 * We call allocsys() with 0 to find out how much space we want,
 * allocate that much and fill it with zeroes, and then call
 * allocsys() again with the correct base virtual address.
 */
caddr_t
allocsys(caddr_t v)
{
#define	valloc(name, type, num)	v = (caddr_t)(((name) = (type *)v) + (num))

#ifdef SYSVMSG
	valloc(msgpool, char, msginfo.msgmax);
	valloc(msgmaps, struct msgmap, msginfo.msgseg);
	valloc(msghdrs, struct msg, msginfo.msgtql);
	valloc(msqids, struct msqid_ds, msginfo.msgmni);
#endif
	/*
	 * Determine how many buffers to allocate.  We use 10% of the
	 * first 2MB of memory, and 5% of the rest, with a minimum of 16
	 * buffers.  We allocate 1/2 as many swap buffer headers as file
	 * i/o buffers.
	 */
	if (bufpages == 0)
		bufpages = (btoc(2 * 1024 * 1024) + physmem) *
		    bufcachepercent / 100;

	if (nbuf == 0) {
		nbuf = bufpages;
		if (nbuf < 16)
			nbuf = 16;
	}

	/* Restrict to at most 35% filled kvm */
	/* XXX - This needs UBC... */
	if (nbuf >
	    (VM_MAX_KERNEL_ADDRESS-VM_MIN_KERNEL_ADDRESS) / MAXBSIZE * 35 / 100)
		nbuf = (VM_MAX_KERNEL_ADDRESS-VM_MIN_KERNEL_ADDRESS) /
		    MAXBSIZE * 35 / 100;

	/* More buffer pages than fits into the buffers is senseless.  */ 
	if (bufpages > nbuf * MAXBSIZE / PAGE_SIZE)
		bufpages = nbuf * MAXBSIZE / PAGE_SIZE;

	valloc(buf, struct buf, nbuf);
	return v;
}

void
dumpsys()
{
	/* TODO */
}

/*
 * Signal frame.
 */
struct sigframe {
#if 0 /* in registers on entry to signal trampoline */
	int		sf_signum;	/* r4 - "signum" argument for handler */
	siginfo_t	*sf_sip;	/* r5 - "sip" argument for handler */
	struct sigcontext *sf_ucp;	/* r6 - "ucp" argument for handler */
#endif
	struct sigcontext sf_uc;	/* actual context */		
	siginfo_t	sf_si;
};

/*
 * Send an interrupt to process.
 */
void
sendsig(sig_t catcher, int sig, int mask, u_long code, int type,
    union sigval val)
{
	struct proc *p = curproc;
	struct sigframe *fp, frame;
	struct trapframe *tf = p->p_md.md_regs;
	struct sigacts *ps = p->p_sigacts;
	siginfo_t *sip;
	int onstack;

	onstack = ps->ps_sigstk.ss_flags & SS_ONSTACK;
	if ((ps->ps_flags & SAS_ALTSTACK) && onstack == 0 &&
	    (ps->ps_sigonstack & sigmask(sig))) {
		fp = (struct sigframe *)((vaddr_t)ps->ps_sigstk.ss_sp +
		    ps->ps_sigstk.ss_size);
		ps->ps_sigstk.ss_flags |= SS_ONSTACK;
	} else
		fp = (void *)p->p_md.md_regs->tf_r15;
	--fp;


	bzero(&frame, sizeof(frame));

	if (ps->ps_siginfo & sigmask(sig)) {
		initsiginfo(&frame.sf_si, sig, code, type, val);
		sip = &fp->sf_si;
	} else
		sip = NULL;

	/* Save register context. */
	frame.sf_uc.sc_spc = tf->tf_spc;
	frame.sf_uc.sc_ssr = tf->tf_ssr;
	frame.sf_uc.sc_pr = tf->tf_pr;
	frame.sf_uc.sc_r14 = tf->tf_r14;
	frame.sf_uc.sc_r13 = tf->tf_r13;
	frame.sf_uc.sc_r12 = tf->tf_r12;
	frame.sf_uc.sc_r11 = tf->tf_r11;
	frame.sf_uc.sc_r10 = tf->tf_r10;
	frame.sf_uc.sc_r9 = tf->tf_r9;
	frame.sf_uc.sc_r8 = tf->tf_r8;
	frame.sf_uc.sc_r7 = tf->tf_r7;
	frame.sf_uc.sc_r6 = tf->tf_r6;
	frame.sf_uc.sc_r5 = tf->tf_r5;
	frame.sf_uc.sc_r4 = tf->tf_r4;
	frame.sf_uc.sc_r3 = tf->tf_r3;
	frame.sf_uc.sc_r2 = tf->tf_r2;
	frame.sf_uc.sc_r1 = tf->tf_r1;
	frame.sf_uc.sc_r0 = tf->tf_r0;
	frame.sf_uc.sc_r15 = tf->tf_r15;
	frame.sf_uc.sc_onstack = onstack;
	frame.sf_uc.sc_expevt = tf->tf_expevt;
	/* frame.sf_uc.sc_err = 0; */
	frame.sf_uc.sc_mask = p->p_sigmask;
	/* XXX tf_macl, tf_mach not saved */

	if (copyout(&frame, fp, sizeof(frame)) != 0) {
		/*
		 * Process has trashed its stack; give it an illegal
		 * instruction to halt it in its tracks.
		 */
		sigexit(p, SIGILL);
		/* NOTREACHED */
	}

	tf->tf_r4 = sig;		/* "signum" argument for handler */
	tf->tf_r5 = (int)sip;		/* "sip" argument for handler */
	tf->tf_r6 = (int)&fp->sf_uc;	/* "ucp" argument for handler */
 	tf->tf_spc = (int)catcher;
	tf->tf_r15 = (int)fp;
	tf->tf_pr = (int)p->p_sigcode;
}

/*
 * System call to cleanup state after a signal
 * has been taken.  Reset signal mask and
 * stack state from context left by sendsig (above).
 * Return to previous pc and psl as specified by
 * context left by sendsig. Check carefully to
 * make sure that the user has not modified the
 * psl to gain improper privileges or to cause
 * a machine fault.
 */
int
sys_sigreturn(struct proc *p, void *v, register_t *retval)
{
	struct sys_sigreturn_args /* {
		syscallarg(struct sigcontext *) sigcntxp;
	} */ *uap = v;
	struct sigcontext *scp, context;
	struct trapframe *tf;
	int error;

	/*
	 * The trampoline code hands us the context.
	 * It is unsafe to keep track of it ourselves, in the event that a
	 * program jumps out of a signal handler.
	 */
	scp = SCARG(uap, sigcntxp);
	if ((error = copyin((caddr_t)scp, &context, sizeof(*scp))) != 0)
		return (error);

	/* Restore signal context. */
	tf = p->p_md.md_regs;

	/* Check for security violations. */
	if (((context.sc_ssr ^ tf->tf_ssr) & PSL_USERSTATIC) != 0)
		return (EINVAL);

	tf->tf_ssr = context.sc_ssr;

	tf->tf_r0 = context.sc_r0;
	tf->tf_r1 = context.sc_r1;
	tf->tf_r2 = context.sc_r2;
	tf->tf_r3 = context.sc_r3;
	tf->tf_r4 = context.sc_r4;
	tf->tf_r5 = context.sc_r5;
	tf->tf_r6 = context.sc_r6;
	tf->tf_r7 = context.sc_r7;
	tf->tf_r8 = context.sc_r8;
	tf->tf_r9 = context.sc_r9;
	tf->tf_r10 = context.sc_r10;
	tf->tf_r11 = context.sc_r11;
	tf->tf_r12 = context.sc_r12;
	tf->tf_r13 = context.sc_r13;
	tf->tf_r14 = context.sc_r14;
	tf->tf_spc = context.sc_spc;
	tf->tf_r15 = context.sc_r15;
	tf->tf_pr = context.sc_pr;

	/* Restore signal stack. */
	if (context.sc_onstack)
		p->p_sigacts->ps_sigstk.ss_flags |= SS_ONSTACK;
	else
		p->p_sigacts->ps_sigstk.ss_flags &= ~SS_ONSTACK;
	/* Restore signal mask. */
	p->p_sigmask = context.sc_mask & ~sigcantmask;

	return (EJUSTRETURN);
}

/*
 * Clear registers on exec
 */
void
setregs(struct proc *p, struct exec_package *pack, u_long stack,
    register_t rval[2])
{
	struct trapframe *tf;

	p->p_md.md_flags &= ~MDP_USEDFPU;

	tf = p->p_md.md_regs;

	tf->tf_r0 = 0;
	tf->tf_r1 = 0;
	tf->tf_r2 = 0;
	tf->tf_r3 = 0;
	copyin((caddr_t)stack, &tf->tf_r4, sizeof(register_t));	/* argc */
	tf->tf_r5 = stack + 4;			/* argv */
	tf->tf_r6 = stack + 4 * tf->tf_r4 + 8;	/* envp */
	tf->tf_r7 = 0;
	tf->tf_r8 = 0;
	tf->tf_r9 = (int)PS_STRINGS;
	tf->tf_r10 = 0;
	tf->tf_r11 = 0;
	tf->tf_r12 = 0;
	tf->tf_r13 = 0;
	tf->tf_r14 = 0;
	tf->tf_spc = pack->ep_entry;
	tf->tf_ssr = PSL_USERSET;
	tf->tf_r15 = stack;
}

void
setrunqueue(struct proc *p)
{
	int whichq = p->p_priority / PPQ;
	struct prochd *q;
	struct proc *prev;

#ifdef DIAGNOSTIC
	if (p->p_back != NULL || p->p_wchan != NULL || p->p_stat != SRUN)
		panic("setrunqueue");
#endif
	q = &qs[whichq];
	prev = q->ph_rlink;
	p->p_forw = (struct proc *)q;
	q->ph_rlink = p;
	prev->p_forw = p;
	p->p_back = prev;
	whichqs |= 1 << whichq;
}

void
remrunqueue(struct proc *p)
{
	struct proc *prev, *next;
	int whichq = p->p_priority / PPQ;

#ifdef DIAGNOSTIC
       if (((whichqs & (1 << whichq)) == 0))
		panic("remrunqueue: bit %d not set", whichq);
#endif
	prev = p->p_back;
	p->p_back = NULL;
	next = p->p_forw;
	prev->p_forw = next;
	next->p_back = prev;
	if (prev == next)
		whichqs &= ~(1 << whichq);
}

/*
 * Jump to reset vector.
 */
void
cpu_reset()
{
	_cpu_exception_suspend();
	_reg_write_4(SH_(EXPEVT), EXPEVT_RESET_MANUAL);

#ifndef __lint__
	goto *(void *)0xa0000000;
#endif
	/* NOTREACHED */
}

int
cpu_sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp,
    size_t newlen, struct proc *p)
{

	/* all sysctl names at this level are terminal */
	if (namelen != 1)
		return (ENOTDIR);		/* overloaded */

	switch (name[0]) {
	case CPU_CONSDEV: {
		dev_t consdev;
		if (cn_tab != NULL)
			consdev = cn_tab->cn_dev;
		else
			consdev = NODEV;
		return (sysctl_rdstruct(oldp, oldlenp, newp, &consdev,
		    sizeof consdev));
	}

	default:
		return (EOPNOTSUPP);
	}
	/* NOTREACHED */
}
