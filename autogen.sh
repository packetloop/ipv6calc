#!/bin/sh
#
# Project    : ipv6calc
# File       : autogen.sh
# Version    : $Id: autogen.sh,v 1.47 2014/07/31 19:02:50 ds6peter Exp $
# Copyright  : 2003-2014 by Peter Bieringer <pb (at) bieringer.de>
#
# Information: autogeneration of projects with optional features

OPTIONS_CONFIGURE=""

LAST=""
while [ "$1" != "$LAST" ]; do
	LAST="$1"
	case $1 in
	    '--no-make'|'-n')
		shift
		flag_no_make=1
		;;
	    '--all'|'-a')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip --enable-ip2location"
		SKIP_STATIC=1
		use_geoip=1
		use_ip2location=1
		;;
	    '--geoip'|'-g')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip"
		SKIP_STATIC=1
		use_geoip=1
		;;
	    '--geoip-dyn'|'-G')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-geoip --with-geoip-dynamic"
		SKIP_STATIC=1
		use_geoip=1
		use_geoip_dyn=1
		;;
	    '--ip2location'|'-i')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location"
		SKIP_STATIC=1
		use_ip2location=1
		;;
	    '--ip2location-dyn'|'-I')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-ip2location --with-ip2location-dynamic"
		SKIP_STATIC=1
		use_ip2location=1
		use_ip2location_dyn=1
		;;
	    '--disable-db-ieee')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ieee"
		;;
	    '--disable-db-ipv4')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ipv4"
		;;
	    '--disable-db-ipv6')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --disable-db-ipv6"
		;;
	    '--geoip-ipv6-compat')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --with-geoip-ipv6-compat"
		SKIP_STATIC=1
		;;
	    '-S')
		shift
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE --enable-shared"
		SKIP_STATIC=1
		;;
	    '--no-static-build')
		shift
		SKIP_STATIC=1
		;;
	    '--no-test')
		shift
		SKIP_TEST=1
		;;
	    '--enable-bundled-md5'|'--enable-bundled-getopt')
		# whitelisted
		OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE $1"
		shift
		;;
	    '-?'|'-h'|'--help')
		echo "Supported options:"
		echo "   -?|-h|--help        : this help"
		echo "   -n|--no-make        : stop before running 'make'"
		echo "   -a|--all            : enable GeoIP and IP2Location support"
		echo "   -g|--geoip          : enable GeoIP support"
		echo "   --geoip-dyn|-G      : switch to dynamic library loading of GeoIP"
		echo "   -i|--ip2location    : enable IP2Location support"
		echo "   --ip2location-dyn|-I: switch to dynamic library loading of IP2Location"
		echo "   --disable-db-ieee   : disable built-in IEEE database"
		echo "   --disable-db-ipv4   : disable built-in IPv4 database"
		echo "   --disable-db-ipv6   : disable built-in IPv6 database"
		echo "   --geoip-ipv6-compat : enable GeoIP IPv6 compatibility mode"
		echo "   -S                  : enable shared library mode"
		echo "   --no-static-build   : skip static build"
		echo "   --no-test           : skip 'make test'"
		exit 1
	esac
done

source ./autogen-support.sh "source"

if [ "$use_geoip" = "1" ]; then
	OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE $(fallback_options_from_name GeoIP)"
fi

if [ "$use_ip2location" = "1" ]; then
	OPTIONS_CONFIGURE="$OPTIONS_CONFIGURE $(fallback_options_from_name IP2Location)"
fi

if [ -f Makefile ]; then
	echo "*** cleanup"
	make autoclean
fi

echo "*** run: autoheader"
autoheader || exit 1

echo "*** run: autoconf"
autoconf || exit 1

echo "*** run: configure, options: $OPTIONS_CONFIGURE $*"
LDFLAGS="$LDFLAGS -Wl,--as-needed" ./configure --bindir=/usr/bin --mandir=/usr/share/man $OPTIONS_CONFIGURE $* || exit 1

if [ "$flag_no_make" = "1" ]; then
	echo
	echo "Stop before running 'make'"
	exit
fi

echo "*** run: make clean"
make clean || exit 1

if [ "$use_ip2location_dyn" = "1" ]; then
	if ldd ./ipv6calc/ipv6calc | grep -i IPLocation; then
		echo "ERROR : dynamic library mode enabled, but ldd of binary still reports reference to IP2Location"
		exit 1
	fi
fi

if [ "$use_geoip_dyn" = "1" ]; then
	if ldd ./ipv6calc/ipv6calc | grep -i GeoIP; then
		echo "ERROR : dynamic library mode enabled, but ldd of binary still reports reference to GeoIP"
		exit 1
	fi
fi

make
if [ $? -ne 0 ]; then
	echo "ERROR : 'make' was not successful with configure options: $OPTIONS_CONFIGURE $*"
	exit 1
fi

if [ "$SKIP_TEST" = "1" ]; then
	echo "*** skip: make test"
else
	echo "*** run: make test"
	make test
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make test' was not successful with configure options: $OPTIONS_CONFIGURE $*"
		exit 1
	fi
fi

if [ "$SKIP_STATIC" != "1" -a ! -e /etc/redhat-release ]; then
	# skip static on non Fedora/RedHat/CentOS systems
	SKIP_STATIC="1"
	echo "NOTICE: 'make static' skipped on non Fedora/RedHat/CentOS systems"
fi

if [ "$SKIP_STATIC" != "1" ]; then
	echo "*** run: make static"
	make static
	if [ $? -ne 0 ]; then
		echo "ERROR : 'make static' reports an error (perhaps glibc-static/openssl-static/zlib-static is missing)"
		exit 1
	fi
fi

echo "*** run: make -n install (dummy install test)"
make -n install
if [ $? -ne 0 ]; then
        echo "ERROR : 'make -n install' reports an error"
        exit 1
fi

echo
echo "For installing the binaries, type: make install"
