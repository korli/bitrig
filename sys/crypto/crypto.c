/*	$OpenBSD: crypto.c,v 1.75 2015/08/28 00:03:53 deraadt Exp $	*/
/*
 * The author of this code is Angelos D. Keromytis (angelos@cis.upenn.edu)
 *
 * This code was written by Angelos D. Keromytis in Athens, Greece, in
 * February 2000. Network Security Technologies Inc. (NSTI) kindly
 * supported the development of this code.
 *
 * Copyright (c) 2000, 2001 Angelos D. Keromytis
 *
 * Permission to use, copy, and modify this software with or without fee
 * is hereby granted, provided that this entire notice is included in
 * all source code copies of any software which is or includes a copy or
 * modification of this software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTY. IN PARTICULAR, NONE OF THE AUTHORS MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE
 * MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/pool.h>

#include <crypto/cryptodev.h>

void crypto_init(void);

struct cryptocap *crypto_drivers = NULL;
int crypto_drivers_num = 0;

struct pool cryptop_pool;
struct pool cryptodesc_pool;

struct taskq *crypto_taskq;

/*
 * Create a new session.
 */
int
crypto_newsession(u_int64_t *sid, struct cryptoini *cri, int hard)
{
	u_int32_t hid, lid, hid2 = -1;
	struct cryptocap *cpc;
	struct cryptoini *cr;
	int err, s, turn = 0;

	if (crypto_drivers == NULL)
		return EINVAL;

	s = splvm();

	/*
	 * The algorithm we use here is pretty stupid; just use the
	 * first driver that supports all the algorithms we need. Do
	 * a double-pass over all the drivers, ignoring software ones
	 * at first, to deal with cases of drivers that register after
	 * the software one(s) --- e.g., PCMCIA crypto cards.
	 *
	 * XXX We need more smarts here (in real life too, but that's
	 * XXX another story altogether).
	 */
	do {
		for (hid = 0; hid < crypto_drivers_num; hid++) {
			cpc = &crypto_drivers[hid];

			/*
			 * If it's not initialized or has remaining sessions
			 * referencing it, skip.
			 */
			if (cpc->cc_newsession == NULL ||
			    (cpc->cc_flags & CRYPTOCAP_F_CLEANUP))
				continue;

			if (cpc->cc_flags & CRYPTOCAP_F_SOFTWARE) {
				/*
				 * First round of search, ignore
				 * software drivers.
				 */
				if (turn == 0)
					continue;
			} else { /* !CRYPTOCAP_F_SOFTWARE */
				/* Second round of search, only software. */
				if (turn == 1)
					continue;
			}

			/* See if all the algorithms are supported. */
			for (cr = cri; cr; cr = cr->cri_next) {
				if (cpc->cc_alg[cr->cri_alg] == 0)
					break;
			}

			/*
			 * If even one algorithm is not supported,
			 * keep searching.
			 */
			if (cr != NULL)
				continue;

			/*
			 * If we had a previous match, see how it compares
			 * to this one. Keep "remembering" whichever is
			 * the best of the two.
			 */
			if (hid2 != -1) {
				/*
				 * Compare session numbers, pick the one
				 * with the lowest.
				 * XXX Need better metrics, this will
				 * XXX just do un-weighted round-robin.
				 */
				if (crypto_drivers[hid].cc_sessions <=
				    crypto_drivers[hid2].cc_sessions)
					hid2 = hid;
			} else {
				/*
				 * Remember this one, for future
                                 * comparisons.
				 */
				hid2 = hid;
			}
		}

		/*
		 * If we found something worth remembering, leave. The
		 * side-effect is that we will always prefer a hardware
		 * driver over the software one.
		 */
		if (hid2 != -1)
			break;

		turn++;

		/* If we only want hardware drivers, don't do second pass. */
	} while (turn <= 2 && hard == 0);

	hid = hid2;

	/*
	 * Can't do everything in one session.
	 *
	 * XXX Fix this. We need to inject a "virtual" session
	 * XXX layer right about here.
	 */

	if (hid == -1) {
		splx(s);
		return EINVAL;
	}

	/* Call the driver initialization routine. */
	lid = hid; /* Pass the driver ID. */
	err = crypto_drivers[hid].cc_newsession(&lid, cri);
	if (err == 0) {
		(*sid) = hid;
		(*sid) <<= 32;
		(*sid) |= (lid & 0xffffffff);
		crypto_drivers[hid].cc_sessions++;
	}

	splx(s);
	return err;
}

/*
 * Delete an existing session (or a reserved session on an unregistered
 * driver).
 */
int
crypto_freesession(u_int64_t sid)
{
	int err = 0, s;
	u_int32_t hid;

	if (crypto_drivers == NULL)
		return EINVAL;

	/* Determine two IDs. */
	hid = (sid >> 32) & 0xffffffff;

	if (hid >= crypto_drivers_num)
		return ENOENT;

	s = splvm();

	if (crypto_drivers[hid].cc_sessions)
		crypto_drivers[hid].cc_sessions--;

	/* Call the driver cleanup routine, if available. */
	if (crypto_drivers[hid].cc_freesession)
		err = crypto_drivers[hid].cc_freesession(sid);

	/*
	 * If this was the last session of a driver marked as invalid,
	 * make the entry available for reuse.
	 */
	if ((crypto_drivers[hid].cc_flags & CRYPTOCAP_F_CLEANUP) &&
	    crypto_drivers[hid].cc_sessions == 0)
		explicit_bzero(&crypto_drivers[hid], sizeof(struct cryptocap));

	splx(s);
	return err;
}

/*
 * Find an empty slot.
 */
int32_t
crypto_get_driverid(u_int8_t flags)
{
	struct cryptocap *newdrv;
	int i, s;
	
	s = splvm();

	if (crypto_drivers_num == 0) {
		crypto_drivers_num = CRYPTO_DRIVERS_INITIAL;
		crypto_drivers = mallocarray(crypto_drivers_num,
		    sizeof(struct cryptocap), M_CRYPTO_DATA, M_NOWAIT | M_ZERO);
		if (crypto_drivers == NULL) {
			crypto_drivers_num = 0;
			splx(s);
			return -1;
		}
	}

	for (i = 0; i < crypto_drivers_num; i++) {
		if (crypto_drivers[i].cc_process == NULL &&
		    !(crypto_drivers[i].cc_flags & CRYPTOCAP_F_CLEANUP) &&
		    crypto_drivers[i].cc_sessions == 0) {
			crypto_drivers[i].cc_sessions = 1; /* Mark */
			crypto_drivers[i].cc_flags = flags;
			splx(s);
			return i;
		}
	}

	/* Out of entries, allocate some more. */
	if (i == crypto_drivers_num) {
		if (crypto_drivers_num >= CRYPTO_DRIVERS_MAX) {
			splx(s);
			return -1;
		}

		newdrv = mallocarray(crypto_drivers_num,
		    2 * sizeof(struct cryptocap), M_CRYPTO_DATA, M_NOWAIT);
		if (newdrv == NULL) {
			splx(s);
			return -1;
		}

		bcopy(crypto_drivers, newdrv,
		    crypto_drivers_num * sizeof(struct cryptocap));
		bzero(&newdrv[crypto_drivers_num],
		    crypto_drivers_num * sizeof(struct cryptocap));

		newdrv[i].cc_sessions = 1; /* Mark */
		newdrv[i].cc_flags = flags;

		free(crypto_drivers, M_CRYPTO_DATA,
		    crypto_drivers_num * sizeof(struct cryptocap));

		crypto_drivers_num *= 2;
		crypto_drivers = newdrv;
		splx(s);
		return i;
	}

	/* Shouldn't really get here... */
	splx(s);
	return -1;
}

/*
 * Register a crypto driver. It should be called once for each algorithm
 * supported by the driver.
 */
int
crypto_register(u_int32_t driverid, int *alg,
    int (*newses)(u_int32_t *, struct cryptoini *),
    int (*freeses)(u_int64_t), int (*process)(struct cryptop *))
{
	int s, i;


	if (driverid >= crypto_drivers_num || alg == NULL ||
	    crypto_drivers == NULL)
		return EINVAL;
	
	s = splvm();

	for (i = 0; i <= CRYPTO_ALGORITHM_MAX; i++) {
		/*
		 * XXX Do some performance testing to determine
		 * placing.  We probably need an auxiliary data
		 * structure that describes relative performances.
		 */

		crypto_drivers[driverid].cc_alg[i] = alg[i];
	}


	crypto_drivers[driverid].cc_newsession = newses;
	crypto_drivers[driverid].cc_process = process;
	crypto_drivers[driverid].cc_freesession = freeses;
	crypto_drivers[driverid].cc_sessions = 0; /* Unmark */

	splx(s);

	return 0;
}

/*
 * Unregister a crypto driver. If there are pending sessions using it,
 * leave enough information around so that subsequent calls using those
 * sessions will correctly detect the driver being unregistered and reroute
 * the request.
 */
int
crypto_unregister(u_int32_t driverid, int alg)
{
	int i = CRYPTO_ALGORITHM_MAX + 1, s;
	u_int32_t ses;

	s = splvm();

	/* Sanity checks. */
	if (driverid >= crypto_drivers_num || crypto_drivers == NULL ||
	    ((alg <= 0 || alg > CRYPTO_ALGORITHM_MAX) &&
		alg != CRYPTO_ALGORITHM_MAX + 1) ||
	    crypto_drivers[driverid].cc_alg[alg] == 0) {
		splx(s);
		return EINVAL;
	}

	if (alg != CRYPTO_ALGORITHM_MAX + 1) {
		crypto_drivers[driverid].cc_alg[alg] = 0;

		/* Was this the last algorithm ? */
		for (i = 1; i <= CRYPTO_ALGORITHM_MAX; i++)
			if (crypto_drivers[driverid].cc_alg[i] != 0)
				break;
	}

	/*
	 * If a driver unregistered its last algorithm or all of them
	 * (alg == CRYPTO_ALGORITHM_MAX + 1), cleanup its entry.
	 */
	if (i == CRYPTO_ALGORITHM_MAX + 1 || alg == CRYPTO_ALGORITHM_MAX + 1) {
		ses = crypto_drivers[driverid].cc_sessions;
		bzero(&crypto_drivers[driverid], sizeof(struct cryptocap));
		if (ses != 0) {
			/*
			 * If there are pending sessions, just mark as invalid.
			 */
			crypto_drivers[driverid].cc_flags |= CRYPTOCAP_F_CLEANUP;
			crypto_drivers[driverid].cc_sessions = ses;
		}
	}
	splx(s);
	return 0;
}

/*
 * Add crypto request to a queue, to be processed by a kernel thread.
 */
int
crypto_dispatch(struct cryptop *crp)
{
	if (crypto_taskq && !(crp->crp_flags & CRYPTO_F_NOQUEUE)) {
		task_set(&crp->crp_task, (void (*))crypto_invoke, crp);
		task_add(crypto_taskq, &crp->crp_task);
	} else {
		crypto_invoke(crp);
	}

	return 0;
}

/*
 * Dispatch a crypto request to the appropriate crypto devices.
 */
int
crypto_invoke(struct cryptop *crp)
{
	struct cryptodesc *crd;
	u_int64_t nid;
	u_int32_t hid;
	int error;
	int s;

	/* Sanity checks. */
	if (crp == NULL || crp->crp_callback == NULL)
		return EINVAL;

	s = splvm();
	if (crp->crp_desc == NULL || crypto_drivers == NULL) {
		crp->crp_etype = EINVAL;
		crypto_done(crp);
		splx(s);
		return 0;
	}

	hid = (crp->crp_sid >> 32) & 0xffffffff;
	if (hid >= crypto_drivers_num)
		goto migrate;

	if (crypto_drivers[hid].cc_flags & CRYPTOCAP_F_CLEANUP) {
		crypto_freesession(crp->crp_sid);
		goto migrate;
	}

	if (crypto_drivers[hid].cc_process == NULL)
		goto migrate;

	crypto_drivers[hid].cc_operations++;
	crypto_drivers[hid].cc_bytes += crp->crp_ilen;

	error = crypto_drivers[hid].cc_process(crp);
	if (error) {
		if (error == ERESTART) {
			/* Unregister driver and migrate session. */
			crypto_unregister(hid, CRYPTO_ALGORITHM_MAX + 1);
			goto migrate;
		} else {
			crp->crp_etype = error;
		}
	}

	splx(s);
	return 0;

 migrate:
	/* Migrate session. */
	for (crd = crp->crp_desc; crd->crd_next; crd = crd->crd_next)
		crd->CRD_INI.cri_next = &(crd->crd_next->CRD_INI);

	if (crypto_newsession(&nid, &(crp->crp_desc->CRD_INI), 0) == 0)
		crp->crp_sid = nid;

	crp->crp_etype = EAGAIN;
	crypto_done(crp);
	splx(s);
	return 0;
}

/*
 * Release a set of crypto descriptors.
 */
void
crypto_freereq(struct cryptop *crp)
{
	struct cryptodesc *crd;

	if (crp == NULL)
		return;

	while ((crd = crp->crp_desc) != NULL) {
		crp->crp_desc = crd->crd_next;
		pool_put(&cryptodesc_pool, crd);
	}

	pool_put(&cryptop_pool, crp);
}

/*
 * Acquire a set of crypto descriptors.
 */
struct cryptop *
crypto_getreq(int num)
{
	struct cryptodesc *crd;
	struct cryptop *crp;
	
	crp = pool_get(&cryptop_pool, PR_NOWAIT | PR_ZERO);
	if (crp == NULL)
		return NULL;

	while (num--) {
		crd = pool_get(&cryptodesc_pool, PR_NOWAIT | PR_ZERO);
		if (crd == NULL) {
			crypto_freereq(crp);
			return NULL;
		}

		crd->crd_next = crp->crp_desc;
		crp->crp_desc = crd;
	}

	return crp;
}

void
crypto_init(void)
{
	crypto_taskq = taskq_create("crypto", 1, IPL_VM, 0);

	pool_init(&cryptop_pool, sizeof(struct cryptop), 0, 0,
	    0, "cryptop", NULL);
	pool_setipl(&cryptop_pool, IPL_VM);
	pool_init(&cryptodesc_pool, sizeof(struct cryptodesc), 0, 0,
	    0, "cryptodesc", NULL);
	pool_setipl(&cryptodesc_pool, IPL_VM);
}

/*
 * Invoke the callback on behalf of the driver.
 */
void
crypto_done(struct cryptop *crp)
{
	crp->crp_flags |= CRYPTO_F_DONE;
	if (crp->crp_flags & CRYPTO_F_NOQUEUE) {
		/* not from the crypto queue, wakeup the userland process */
		crp->crp_callback(crp);
	} else {
		task_set(&crp->crp_task, (void (*))crp->crp_callback, crp);
		task_add(crypto_taskq, &crp->crp_task);
	}
}
