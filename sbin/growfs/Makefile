#	@(#)Makefile	8.8 (Berkeley) 6/21/2000
#
# $TSHeader: src/sbin/growfs/Makefile,v 1.4 2000/12/05 19:45:24 tomsoft Exp $
# $FreeBSD: src/sbin/growfs/Makefile,v 1.4 2001/12/04 02:19:47 obrien Exp $
# $OpenBSD: Makefile,v 1.8 2015/11/23 18:35:18 mmcc Exp $
#

PROG=   growfs
SRCS=   growfs.c
MAN=	growfs.8

#CFLAGS+=-Wall

DPADD=	${LIBUTIL}
LDADD=	-lutil

.include <bsd.prog.mk>      
