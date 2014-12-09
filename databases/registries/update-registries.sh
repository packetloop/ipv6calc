#!/bin/sh
#
# Project    : ipv6calc/databases/registries
# File       : update-registries.sh
# Version    : $Id: update-registries.sh,v 1.12 2014/12/09 21:03:51 ds6peter Exp $
# Copyright  : 2002-2014 by Peter Bieringer <pb (at) bieringer.de>
#               replaces ../ipv4-assignment/update-ipv4-assignment.sh
#               replaces ../ipv6-assignment/update-ipv6-assignment.sh
#
# Information:
#  Shell script to update registry data

#set -x

get_urls() {
	cat <<END | grep -v "^#"
iana	http://www.iana.org/assignments/ipv4-address-space/			ipv4-address-space.xml			xml
iana	http://www.iana.org/assignments/ipv6-unicast-address-assignments/	ipv6-unicast-address-assignments.xml	xml
ripencc	http://ftp.ripe.net/pub/stats/ripencc/		delegated-ripencc-latest		txt
arin	http://ftp.arin.net/pub/stats/arin/		delegated-arin-extended-latest		txt
apnic	http://ftp.apnic.net/stats/apnic/		delegated-apnic-latest			txt
lacnic	http://ftp.lacnic.net/pub/stats/lacnic/		delegated-lacnic-latest			txt
afrinic	http://ftp.afrinic.net/pub/stats/afrinic/	delegated-afrinic-latest		txt
iana    https://www.iana.org/assignments/as-numbers/	as-numbers.txt				txt
END
}

help() {
	cat <<END
Usage: $(basename "$0") [-D <DST-DIR>] [-h|?] [-d]
	-D <DST-DIR>	destination directory (default: internal sub-directories)

	-d		dry-run/debug
	-h|?		this online help
END
}

## default
dir_dst=""
dry_run=0

## parse options
while getopts "\?hdD:" opt; do 
	case $opt in
	    D)
		if [ -d "$OPTARG" ]; then
			dir_dst="$OPTARG"
		else
			echo "ERROR : given destination directory doesn't exist: $OPTARG"
			exit 1
		fi
		;;
	    d)
		dry_run=1
		;;
	    \?|h)
		help
		exit 1
		;;
	    *)
		echo "Invalid option: -$OPTARG" >&2
		exit 0
		;;
	esac
done

if [ -z "$dir_dst" ]; then
	echo "INFO  : download new version of files to defined sub-directories"
else
	echo "INFO  : download new version of files to: $dir_dst"
fi

get_urls | while read subdir url filename format flag; do
	if [ -z "$dir_dst" ]; then
		echo "DEBUG : check for sub-directory: $subdir"
		if [ ! -d "$subdir" ]; then
			mkdir "$subdir" || exit 1
		fi
		pushd "$subdir" || exit 1
	else
		echo "DEBUG : change to download directory: $dir_dst"
		pushd "$dir_dst" >/dev/null || exit 1
	fi

	if [ "$flag" = "out" ]; then
		[ $dry_run -ne 1 ] && wget $url$filename -O $filename
		retval=$?
	else
		[ $dry_run -ne 1 ] && wget $url$filename --timestamping --retr-symlinks
		retval=$?
	fi
	popd >/dev/null
	if [ $retval -ne 0 ]; then
		echo "ERROR : can't download: $filename ($url)"
		exit 1
	else
		echo "INFO  : successfully downloaded: $filename ($url)"
	fi

	if [ -z "$dir_dst" ]; then
		pushd "$subdir" >/dev/null || exit 1
	else
		pushd "$dir_dst" >/dev/null || exit 1
	fi

	case $format in
            'txt'|'csv')
		# nothing to do
		;;
            'xml')
		# fix buggy encoding
		perl -pi -e "s/^(.*encoding=')ASCII('.*)$/\1US-ASCII\2/" $filename || exit 1
		;;
	    'bz2')
		# decompress
		bzip2 -f -d -k $filename || exit 1
		;;
	    *)
		echo "ERROR: unsupported format: $format - fix code"
		exit 1
		;;
	esac
	popd >/dev/null
done
