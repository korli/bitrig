# $FreeBSD: src/tools/regression/fstest/tests/truncate/08.t,v 1.1 2007/01/17 01:42:12 pjd Exp $

desc="truncate returns EPERM if the named file has its immutable or append-only flag set"

n0=`namegen`

expect 0 create ${n0} 0644
expect 0 chflags ${n0} SF_IMMUTABLE
expect EPERM truncate ${n0} 123
expect 0 stat ${n0} size
expect 0 chflags ${n0} none
expect 0 truncate ${n0} 123
expect 0 unlink ${n0}

expect 0 create ${n0} 0644
expect 0 chflags ${n0} UF_IMMUTABLE
expect EPERM truncate ${n0} 123
expect 0 stat ${n0} size
expect 0 chflags ${n0} none
expect 0 truncate ${n0} 123
expect 0 unlink ${n0}

expect 0 create ${n0} 0644
expect 0 chflags ${n0} SF_APPEND
expect EPERM truncate ${n0} 123
expect 0 stat ${n0} size
expect 0 chflags ${n0} none
expect 0 truncate ${n0} 123
expect 0 unlink ${n0}

expect 0 create ${n0} 0644
expect 0 chflags ${n0} UF_APPEND
expect EPERM truncate ${n0} 123
expect 0 stat ${n0} size
expect 0 chflags ${n0} none
expect 0 truncate ${n0} 123
expect 0 unlink ${n0}
