#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

run()
{
    $@
    if test $? -ne 0; then
	echo "Failed $@"
	exit 1
    fi
}

run ${ACLOCAL:-aclocal} $ACLOCAL_OPTIONS
run ${LIBTOOLIZE:-libtoolize} --copy --force
run ${INTLTOOLIZE:-intltoolize} --force --copy --automake
run ${AUTOHEADER:-autoheader}
run ${AUTOMAKE:-automake} --add-missing --foreign --copy
run ${AUTOCONF:-autoconf}
