# $FreeBSD$

.include <bsd.own.mk>

LIB=	llvmmipsdisassembler

SRCDIR=	lib/Target/Mips/Disassembler
INCDIR=	lib/Target/Mips
SRCS=	MipsDisassembler.cpp

TGHDRS= Attributes \
	Intrinsics \
	MipsGenDisassemblerTables \
	MipsGenInstrInfo \
	MipsGenRegisterInfo \
	MipsGenSubtargetInfo

.include "../clang.lib.mk"
