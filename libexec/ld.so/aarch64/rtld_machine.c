/*	$OpenBSD: rtld_machine.c,v 1.19 2013/06/13 04:13:47 brad Exp $ */

/*
 * Copyright (c) 2004 Dale Rahn
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#define _DYN_LOADER

#include <sys/types.h>
#include <sys/mman.h>

#include <nlist.h>
#include <link.h>
#include <signal.h>

#include "syscall.h"
#include "archdep.h"
#include "resolve.h"
#include "tls.h"

void _dl_bind_start(void); /* XXX */
Elf_Addr _dl_bind(elf_object_t *object, int index);
#define _RF_S		0x80000000		/* Resolve symbol */
#define _RF_A		0x40000000		/* Use addend */
#define _RF_P		0x20000000		/* Location relative */
#define _RF_G		0x10000000		/* GOT offset */
#define _RF_B		0x08000000		/* Load address relative */
#define _RF_U		0x04000000		/* Unaligned */
#define _RF_V		0x02000000		/* ERROR */
#define _RF_SZ(s)	(((s) & 0xff) << 8)	/* memory target size */
#define _RF_RS(s)	((s) & 0xff)		/* right shift */
static int reloc_target_flags[] = {
	0,						/*  0 NONE */
	[ R_AARCH64_ABS64 ] = 
	  _RF_V|_RF_S|_RF_A|		_RF_SZ(64) | _RF_RS(0),	/* ABS64 */
	[ R_AARCH64_GLOB_DAT ] = 
	  _RF_V|_RF_S|_RF_A|		_RF_SZ(64) | _RF_RS(0),	/* GLOB_DAT */
	[ R_AARCH64_JUMP_SLOT ] = 
	  _RF_V|_RF_S|			_RF_SZ(64) | _RF_RS(0),	/* JUMP_SLOT */
	[ R_AARCH64_RELATIVE ] = 
	  _RF_V|_RF_B|_RF_A|		_RF_SZ(64) | _RF_RS(0),	/* REL64 */
	[ R_AARCH64_TLSDESC ] = 
	  _RF_V|_RF_S,
	[ R_AARCH64_TLS_TPREL64 ] = 
	  _RF_V|_RF_S,
	[ R_AARCH64_COPY ] = 
	  _RF_V|_RF_S|			_RF_SZ(32) | _RF_RS(0),	/* 20 COPY */

};

#define RELOC_RESOLVE_SYMBOL(t)		((reloc_target_flags[t] & _RF_S) != 0)
#define RELOC_PC_RELATIVE(t)		((reloc_target_flags[t] & _RF_P) != 0)
#define RELOC_BASE_RELATIVE(t)		((reloc_target_flags[t] & _RF_B) != 0)
#define RELOC_UNALIGNED(t)		((reloc_target_flags[t] & _RF_U) != 0)
#define RELOC_USE_ADDEND(t)		((reloc_target_flags[t] & _RF_A) != 0)
#define RELOC_TARGET_SIZE(t)		((reloc_target_flags[t] >> 8) & 0xff)
#define RELOC_VALUE_RIGHTSHIFT(t)	(reloc_target_flags[t] & 0xff)
static Elf_Addr reloc_target_bitmask[] = {
#define _BM(x)  (~(Elf_Addr)0 >> ((8*sizeof(reloc_target_bitmask[0])) - (x)))
	0,		/*  0 NONE */
	[ R_AARCH64_ABS64 ] = _BM(64),
	[ R_AARCH64_GLOB_DAT ] = _BM(64),
	[ R_AARCH64_JUMP_SLOT ] = _BM(64),
	[ R_AARCH64_RELATIVE ] = _BM(64),
	[ R_AARCH64_TLSDESC ] = _BM(64),
	[ R_AARCH64_TLS_TPREL64 ] = _BM(64),
	[ R_AARCH64_COPY ] = _BM(64),
#undef _BM
};
#define RELOC_VALUE_BITMASK(t)	(reloc_target_bitmask[t])

#define R_TYPE(x) R_AARCH64_ ## x

void _dl_reloc_plt(Elf_Word *where, Elf_Addr value, Elf_RelA *rel);

#define nitems(_a)     (sizeof((_a)) / sizeof((_a)[0]))

long _rtld_tlsdesc(long);

int
_dl_md_reloc(elf_object_t *object, int rel, int relsz)
{
	long	i;
	long	numrel;
	long	relrel;
	int	fails = 0;
	Elf_Addr loff;
	Elf_Addr prev_value = 0;
	const Elf_Sym *prev_sym = NULL;
	Elf_RelA *rels;
	struct load_list *llist;

	loff = object->obj_base;
	numrel = object->Dyn.info[relsz] / sizeof(Elf_RelA);
	relrel = rel == DT_RELA ? object->relcount : 0;
	rels = (Elf_RelA *)(object->Dyn.info[rel]);

	if (rels == NULL)
		return(0);

	if (relrel > numrel) {
		_dl_printf("relcount > numrel: %ld > %ld\n", relrel, numrel);
		_dl_exit(20);
	}

	/*
	 * unprotect some segments if we need it.
	 */
	if ((object->dyn.textrel == 1) && (rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list;
		    llist != NULL;
		    llist = llist->next) {
			if (!(llist->prot & PROT_WRITE))
				_dl_mprotect(llist->start, llist->size,
				    llist->prot|PROT_WRITE);
		}
	}

	/* tight loop for leading RELATIVE relocs */
	for (i = 0; i < relrel; i++, rels++) {
		Elf_Addr *where;

#ifdef DEBUG
		if (ELF_R_TYPE(rels->r_info) != R_TYPE(RELATIVE)) {
			_dl_printf("RELCOUNT wrong\n");
			_dl_exit(20);
		}
#endif
		where = (Elf_Addr *)(rels->r_offset + loff);
		*where += loff;
	}
	for (; i < numrel; i++, rels++) {
		Elf_Addr *where, value, ooff, mask;
		Elf_Word type;
		const Elf_Sym *sym, *this;
		const char *symn;
		const elf_object_t *refobj;

		type = ELF_R_TYPE(rels->r_info);

		if (type >= nitems(reloc_target_flags)) {
			_dl_printf(" bad relocation %d %d\n", i, type);
			_dl_exit(1);
		}
		if ((reloc_target_flags[type] & _RF_V)==0) {
			_dl_printf(" bad relocation %d %d\n", i, type);
			_dl_exit(1);
		}
		if (type == R_TYPE(NONE))
			continue;

		if (type == R_TYPE(JUMP_SLOT) && rel != DT_JMPREL)
			continue;

		where = (Elf_Addr *)(rels->r_offset + loff);

		if (RELOC_USE_ADDEND(type))
//#ifdef LDSO_ARCH_IS_RELA_
			value = rels->r_addend;
//#else
//			value = *where & RELOC_VALUE_BITMASK(type);
//#endif
		else
			value = 0;

		sym = NULL;
		symn = NULL;
		if (RELOC_RESOLVE_SYMBOL(type)) {
			sym = object->dyn.symtab;
			sym += ELF_R_SYM(rels->r_info);
			symn = object->dyn.strtab + sym->st_name;

			if (sym->st_shndx != SHN_UNDEF &&
			    ELF_ST_BIND(sym->st_info) == STB_LOCAL) {
				value += loff;
				refobj = object;
			} else if (sym == prev_sym) {
				value += prev_value;
				refobj = object;
			} else {
				this = NULL;
				ooff = _dl_find_symbol_bysym(object,
				    ELF_R_SYM(rels->r_info), &this,
				    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|
				    ((type == R_TYPE(JUMP_SLOT)) ?
					SYM_PLT : SYM_NOTPLT),
				    sym, &refobj);
				if (this == NULL) {
resolve_failed:
					if (ELF_ST_BIND(sym->st_info) !=
					    STB_WEAK)
						fails++;
					continue;
				}
				prev_sym = sym;
				prev_value = (Elf_Addr)(ooff + this->st_value);
				value += prev_value;
			}
		}

		if (type == R_TYPE(JUMP_SLOT)) {
			/*
			_dl_reloc_plt((Elf_Word *)where, value, rels);
			*/
			*where = value;
			continue;
		}

		if (type == R_TYPE(COPY)) {
			void *dstaddr = where;
			const void *srcaddr;
			const Elf_Sym *dstsym = sym, *srcsym = NULL;
			Elf_Addr soff;

			soff = _dl_find_symbol(symn, &srcsym,
			    SYM_SEARCH_OTHER|SYM_WARNNOTFOUND|SYM_NOTPLT,
			    dstsym, object, NULL);
			if (srcsym == NULL)
				goto resolve_failed;

			srcaddr = (void *)(soff + srcsym->st_value);
			_dl_bcopy(srcaddr, dstaddr, dstsym->st_size);
			continue;
		}
		if (type == R_TYPE(TLS_DTPMOD64)) {
			if (value == 0)
				goto resolve_failed;
			*where = (Elf_Addr) refobj->tls_index + rels->r_addend;
			continue;
		}
		if (type == R_TYPE(TLS_TPREL64)) {
			if (value == 0)
				goto resolve_failed;
			*where = (Elf_Addr) this->st_value +
			    (Elf_Addr) refobj->tls_offset +
			    sizeof(struct thread_control_block);
			continue;
		}
		if (type == R_TYPE(TLSDESC)) {
			if (value == 0)
				goto resolve_failed;
			if (refobj->tls_done == 0) {
				_dl_printf("shared object not intialized %s\n",
				  refobj->load_name);
				_dl_exit(21);
			}
			if (refobj->initial_module == 1) {
				where[0] = (Elf_Addr)_rtld_tlsdesc;
				where[1] = (Elf_Addr) this->st_value +
				    (Elf_Addr) refobj->tls_offset + 
				    (Elf_Addr) rels->r_addend +
				    sizeof(struct thread_control_block);
			} else {
				_dl_exit(23);
			}

			continue;
		}

		if (RELOC_PC_RELATIVE(type))
			value -= (Elf_Addr)where;
		if (RELOC_BASE_RELATIVE(type))
			value += loff;

		mask = RELOC_VALUE_BITMASK(type);
		value >>= RELOC_VALUE_RIGHTSHIFT(type);
		value &= mask;

		if (RELOC_UNALIGNED(type)) {
			/* Handle unaligned relocations. */
			Elf_Addr tmp = 0;
			char *ptr = (char *)where;
			int i, size = RELOC_TARGET_SIZE(type)/8;

			/* Read it in one byte at a time. */
			for (i=0; i<size; i++)
				tmp = (tmp << 8) | ptr[i];

			tmp &= ~mask;
			tmp |= value;

			/* Write it back out. */
			for (i=0; i<size; i++)
				ptr[i] = ((tmp >> (8*i)) & 0xff);
		} else {
			*where &= ~mask;
			*where |= value;
		}
	}

	/* reprotect the unprotected segments */
	if ((object->dyn.textrel == 1) && (rel == DT_REL || rel == DT_RELA)) {
		for (llist = object->load_list;
		    llist != NULL;
		    llist = llist->next) {
			if (!(llist->prot & PROT_WRITE))
				_dl_mprotect(llist->start, llist->size,
				    llist->prot);
		}
	}

	return (fails);
}

/*
 *	Relocate the Global Offset Table (GOT).
 *	This is done by calling _dl_md_reloc on DT_JUMPREL for DL_BIND_NOW,
 *	otherwise the lazy binding plt initialization is performed.
 */
int
_dl_md_reloc_got(elf_object_t *object, int lazy)
{
	int	fails = 0;
	Elf_Addr *pltgot = (Elf_Addr *)object->Dyn.info[DT_PLTGOT];
	Elf_Addr ooff;
	const Elf_Sym *this;
	int i, num;
	Elf_RelA *rel;

	if (object->Dyn.info[DT_PLTREL] != DT_RELA)
		return (0);

	object->got_addr = 0;
	object->got_size = 0;
	this = NULL;
	ooff = _dl_find_symbol("__got_start", &this,
	    SYM_SEARCH_OBJ|SYM_NOWARNNOTFOUND|SYM_PLT, NULL, object, NULL);
	if (this != NULL)
		object->got_addr = ooff + this->st_value;

	this = NULL;
	ooff = _dl_find_symbol("__got_end", &this,
	    SYM_SEARCH_OBJ|SYM_NOWARNNOTFOUND|SYM_PLT, NULL, object, NULL);
	if (this != NULL)
		object->got_size = ooff + this->st_value  - object->got_addr;

	object->plt_size = 0;	/* Text PLT on ARM */

	if (object->got_addr == 0)
		object->got_start = 0;
	else {
		object->got_start = ELF_TRUNC(object->got_addr, _dl_pagesz);
		object->got_size += object->got_addr - object->got_start;
		object->got_size = ELF_ROUND(object->got_size, _dl_pagesz);
	}
	object->plt_start = 0;

	if (object->traced)
		lazy = 1;

	lazy = 0; // until support is written.

	if (!lazy) {
		fails = _dl_md_reloc(object, DT_JMPREL, DT_PLTRELSZ);
	} else {
		rel = (Elf_RelA *)(object->Dyn.info[DT_JMPREL]);
		num = (object->Dyn.info[DT_PLTRELSZ]);

		for (i = 0; i < num/sizeof(Elf_RelA); i++, rel++) {
			Elf_Addr *where;
			where = (Elf_Addr *)(rel->r_offset + object->obj_base);
			*where += object->obj_base;
		}

		pltgot[1] = (Elf_Addr)object;
		pltgot[2] = (Elf_Addr)_dl_bind_start;
	}
	if (object->got_size != 0)
		_dl_mprotect((void*)object->got_start, object->got_size,
		    PROT_READ);
	if (object->plt_size != 0)
		_dl_mprotect((void*)object->plt_start, object->plt_size,
		    PROT_READ|PROT_EXEC);

	return (fails);
}

Elf_Addr
_dl_bind(elf_object_t *object, int relidx)
{
	Elf_RelA *rel;
	Elf_Word *addr;
	const Elf_Sym *sym, *this;
	const char *symn;
	const elf_object_t *sobj;
	Elf_Addr ooff, newval;
	sigset_t savedmask;

	rel = ((Elf_RelA *)object->Dyn.info[DT_JMPREL]) + (relidx);

	sym = object->dyn.symtab;
	sym += ELF_R_SYM(rel->r_info);
	symn = object->dyn.strtab + sym->st_name;

	this = NULL;
	ooff = _dl_find_symbol(symn,  &this,
	    SYM_SEARCH_ALL|SYM_WARNNOTFOUND|SYM_PLT, sym, object, &sobj);
	if (this == NULL) {
		_dl_printf("lazy binding failed!\n");
		*(volatile int *)0 = 0;		/* XXX */
	}

	addr = (Elf_Word *)(object->obj_base + rel->r_offset);
	newval = ooff + this->st_value;

	if (sobj->traced && _dl_trace_plt(sobj, symn))
		return newval;

	/* if GOT is protected, allow the write */
	if (object->got_size != 0) {
		_dl_thread_bind_lock(0, &savedmask);
		_dl_mprotect((void*)object->got_start, object->got_size,
		    PROT_READ|PROT_WRITE);
	}

	if (*addr != newval)
		*addr = newval;

	/* put the GOT back to RO */
	if (object->got_size != 0) {
		_dl_mprotect((void*)object->got_start, object->got_size,
		    PROT_READ);
		_dl_thread_bind_lock(1, &savedmask);
	}
	return newval;
}

void
_dl_allocate_first_tls()
{
	void *tls;

	if (_dl_tls_first_done)
		return;
	_dl_tls_first_done = 1;
	_dl_tls_static_space = _dl_tls_free_idx + RTLD_STATIC_TLS_EXTRA;
	tls = _dl_allocate_tls(NULL, _dl_objects,
	    sizeof(struct thread_control_block), sizeof(Elf_Addr));
	_dl___set_tcb(tls);
}

void *__tls_get_addr(tls_index *ti)
{
	struct thread_control_block *tcb;
	// XXX - fetch from register 
	tcb =  _dl___get_tcb();
	Elf_Addr *dtv;
	dtv = tcb->tcb_dtv;
	return (void *)(dtv[1+ti->ti_module] + ti->ti_offset);
}
