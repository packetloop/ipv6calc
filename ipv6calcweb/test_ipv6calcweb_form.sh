#!/bin/sh
#
# Project    : ipv6calc/ipv6calcweb
# File       : test_ipv6calcweb_form.sh
# Version    : $Id$
# Copyright  : 2012-2018 by Peter Bieringer <pb (at) bieringer.de>
#
# Information:
#  Test script for form mode of ipv6calcweb
#

verbose=0
while getopts "Vh\?" opt; do
	case $opt in
	    V)
		verbose=1
		;;
	    *)
		echo "$0 [-V]"
		echo "    -V   verbose"
		exit 1
		;;
	esac
done

# Test Scenarios for autodetection "good case"
source ../ipv6calc/test_scenarios.sh

if [ ! -x ipv6calcweb.cgi ]; then
	echo "ERROR : run make"
	exit 1
fi

REMOTE_ADDR="127.0.0.1"
REMOTE_HOST="client.domain.example"
HTTP_USER_AGENT="test_ipv6calcweb"

SERVER_ADDR="192.0.2.1"
SERVER_NAME="server.domain.example"

HTTP_IPV6CALCWEB_MODE="form"
HTTP_IPV6CALCWEB_DEBUG="0x5000" # no sleeps and no Anti-DoS

export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_MODE HTTP_IPV6CALCWEB_DEBUG

# extract tokenhash & tokentime
OUTPUT="`./ipv6calcweb.cgi`"
result=$?
if [ $result -ne 0 ]; then
	echo "TEST FAILED"
	exit 1
else
	# check output
	if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
		echo "TEST FAILED"
		exit 1
	fi
fi 

tokenhash="`echo "$OUTPUT" | grep 'name="tokenhash"' | sed 's/.* name="tokenhash" value="\([0-9a-f]*\)" .*/\1/'`"
tokentime="`echo "$OUTPUT" | grep 'name="tokentime"' | sed 's/.* name="tokentime" value="\([0-9]*\)" .*/\1/'`"
if [ ${#tokenhash} -ne 72 ]; then
	echo "TEST FAILED (tokenhash extraction)"
	echo "$OUTPUT"
	exit 1
fi
if [ ${#tokentime} -ne 10 ]; then
	echo "TEST FAILED (tokentime extraction)"
	echo "$OUTPUT"
	exit 1
fi

[ "$verbose" = "1" ] && echo "DEBUG: extracted tokenhash: $tokenhash"
[ "$verbose" = "1" ] && echo "DEBUG: extracted tokentime: $tokentime"

test="run 'ipv6calcweb.cgi ' good tests"
echo "INFO  : $test"
testscenarios_auto_good | grep -v "^#" | egrep -vw "(bitstring|base85)" | grep -v "%" | while read input type; do
	input_escaped="$(perl -MURI::Escape -e 'print uri_escape($ARGV[0]);' "$input")"
	QUERY_STRING="input=$input_escaped&tokenhash=$tokenhash&tokentime=$tokentime"

	[ "$verbose" = "1" ] && echo "Test: $input ($input_escaped) ($type)"
	[ "$verbose" = "1" ] && echo "QUERY_STRING=$QUERY_STRING"

	export REMOTE_ADDR REMOTE_HOST HTTP_USER_AGENT SERVER_ADDR SERVER_NAME QUERY_STRING HTTP_IPV6CALCWEB_MODE

	if [ "$verbose" = "1" ]; then 
		OUTPUT="`./ipv6calcweb.cgi`"
		result=$?
	else
		OUTPUT=$(./ipv6calcweb.cgi 2>/dev/null)
		result=$?
	fi

	[ "$verbose" = "1" ] && echo "Result: $result"

	if [ $result -ne 0 ]; then
		echo "TEST FAILED"
		exit 1
	else
		# check output
		if echo "$OUTPUT" | egrep -q "(ERROR|problem)"; then
			echo "TEST FAILED"
			exit 1
		fi
	fi
	[ "$verbose" = "1" ] || echo -n "."
done || exit $?
[ "$verbose" = "1" ] || echo
echo "INFO  : $test successful"
