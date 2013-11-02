/*
 * Project    : ipv6calc
 * File       : ipv6calchelp.c
 * Version    : $Id: ipv6calchelp.c,v 1.50 2013/11/02 17:05:11 ds6peter Exp $
 * Copyright  : 2002-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Help library
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libipv6calc.h"
#include "libipv6calcdebug.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"
#include "config.h"

#include "libieee.h"
#include "databases/ieee-oui/dbieee_oui.h"
#include "databases/ieee-iab/dbieee_iab.h"
#include "databases/ieee-oui36/dbieee_oui36.h"
#include "databases/ipv4-assignment/dbipv4addr_assignment.h"
#include "databases/ipv6-assignment/dbipv6addr_assignment.h"
#include "databases/lib/libipv6calc_db_wrapper.h"
#include "databases/lib/libipv6calc_db_wrapper_GeoIP.h"
#include "databases/lib/libipv6calc_db_wrapper_IP2Location.h"
#include "databases/lib/libipv6calc_db_wrapper_BuiltIn.h"


#ifdef SUPPORT_IP2LOCATION
/* 
 *  * API_VERSION is defined as a bareword in IP2Location.h, 
 *   * we need this trick to stringify it. Blah.
 *    */
#define makestr(x) #x
#define xmakestr(x) makestr(x)

extern char* file_ip2location_ipv4;
extern char* file_ip2location_ipv6;
#endif

/* to be defined in each application */
extern void printversion(void);
extern void printcopyright(void);


/* format option arguments */
void printhelp_print(void) {
	fprintf(stderr, "   --printprefix        : print only prefix of IPv6 address\n");
        fprintf(stderr, "   --printsuffix        : print only suffix of IPv6 address\n");
};

void printhelp_mask(void) {
	fprintf(stderr, "   --maskprefix         : mask IPv6 address with prefix length (clears suffix bits)\n");
        fprintf(stderr, "   --masksuffix         : mask IPv6 address with suffix length (clears prefix bits)\n");
};

void printhelp_case(void) {
	fprintf(stderr, "   --uppercase|-u       : print chars of IPv6 address in upper case\n");
        fprintf(stderr, "   --lowercase|-l       : print chars of IPv6 address in lower case [default]\n");
};

void printhelp_printstartend(void) {
	fprintf(stderr, "   --printstart <1-128> : print part of IPv6 address start from given number\n");
	fprintf(stderr, "   --printend   <1-128> : print part of IPv6 address end at given number\n");
};

void printhelp_doublecommands(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Only one command may be specified!\n");
};

void printhelp_missinginputdata(void) {
	printversion();
	printcopyright();
	fprintf(stderr, "   Missing or to few input data given!\n");
};

/* list of input types */
void printhelp_inputtypes(const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	}

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available input types:\n");

	/* run through matrix */
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatmatrix); i++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Row %d: %08x - %08x\n", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);
		};
		if (ipv6calc_formatmatrix[i][1] != 0) {
			/* available for input, look for name now */
			for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				};
				if (ipv6calc_formatstrings[j].number == ipv6calc_formatmatrix[i][0]) {
					if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
						fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
					} else {
						fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token);
					};
				};
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* list of output types */
void printhelp_outputtypes(const uint32_t inputtype, const uint32_t formatoptions) {
	int i, j;
	size_t maxlen = 0;
	char printformatstring[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
		if (strlen(ipv6calc_formatstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_formatstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_formatstrings'!\n");
		exit(EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
			fprintf(stderr, "\n Available output types filtered by input type:\n");
		} else {
			fprintf(stderr, "\n Available output types:\n");
		};
	};

	/* run through matrix */
	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatstrings); j++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_formatstrings[j].number, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
		};

		for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_formatmatrix); i++) {
			if ( (inputtype & ~ (FORMAT_auto | FORMAT_any) ) != 0 ) {
				if (ipv6calc_formatmatrix[i][0] != inputtype) {
					/* skip */
					continue;
				};
			};
		
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "Row %d: %08x - %08x\n", i, (unsigned int) ipv6calc_formatmatrix[i][0], (unsigned int) ipv6calc_formatmatrix[i][1]);
			};

			if ((ipv6calc_formatmatrix[i][1] & ipv6calc_formatstrings[j].number) != 0) {
				/* available for output, look for name now */
				if (strlen(ipv6calc_formatstrings[j].explanation) > 0) {
					fprintf(stderr, printformatstring, ipv6calc_formatstrings[j].token, ipv6calc_formatstrings[j].explanation);
				} else {
					fprintf(stdout, printformatstring, ipv6calc_formatstrings[j].token, "(empty)");
				};
				break;
			};
		};
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		fprintf(stderr, "\n For examples and available format options use:\n");
		fprintf(stderr, "    --out <type> --examples\n");
		fprintf(stderr, "\n");
	};
};


/* list of action types */
void printhelp_actiontypes(const uint32_t formatoptions, const struct option longopts[]) {
	int i, j, o, test = 2, has_options = 0;
	size_t maxlen = 0;
	char printformatstring[20], printformatstring2[20], printformatstring3[20];
	
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {	
		printversion();
		printcopyright();
	};

	/* look for longest type definition */
	for (j = 0; j < (int) (sizeof(ipv6calc_actionstrings) / sizeof(ipv6calc_actionstrings[0])); j++) {
		if (strlen(ipv6calc_actionstrings[j].token) > maxlen) {
			maxlen = strlen(ipv6calc_actionstrings[j].token);
		};
	};

	if (maxlen > 999) {
		fprintf(stderr, " Something going wrong with array 'ipv6calc_actionstrings'!\n");
		exit (EXIT_FAILURE);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "  %%-%ds : %%s\n", (int) maxlen);
	} else {
		snprintf(printformatstring, sizeof(printformatstring) - 1, "%%-%ds\n", (int) maxlen);
	};

	snprintf(printformatstring2, sizeof(printformatstring2) - 1, "  %%-%ds%%s\n", (int) maxlen + 4);
	snprintf(printformatstring3, sizeof(printformatstring3) - 1, "  %%-%ds--%%s", (int) maxlen + 5);

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Format string: %s\n", printformatstring);
	};

	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n Available action types:\n");

	for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_actionstrings); j++) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "Format-Row %d: %08x - %s - %s\n", j, (unsigned int) ipv6calc_actionstrings[j].number, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);
		};

		if ((formatoptions & FORMATOPTION_machinereadable) == 0) {
			fprintf(stderr, printformatstring, ipv6calc_actionstrings[j].token, ipv6calc_actionstrings[j].explanation);

			while (test != 0) {
				if (test == 1) {	
					fprintf(stderr, printformatstring2, "", "Required options:");
				};

				/* search for defined options */
				for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_actionoptionmap); i++) {
					if (ipv6calc_debug != 0) {
						fprintf(stderr, "Option %d\n", i);
					};

					if (ipv6calc_actionstrings[j].number == ipv6calc_actionoptionmap[i][0]) {
						if (ipv6calc_actionoptionmap[i][1] == 0) {
							/* no options supported */
							break;
						};
						
						if (ipv6calc_debug != 0) {
							fprintf(stderr, "Option value: %08x\n", (unsigned int) ipv6calc_actionoptionmap[i][1]);
						};

						/* run through options */
						o = 0;
						while(longopts[o].name != NULL) {
							if ((ipv6calc_actionoptionmap[i][1] == longopts[o].val)) {
								has_options = 1;
								if (test == 1) {	
									fprintf(stderr, printformatstring3, "", longopts[o].name);
									if (longopts[o].has_arg > 0) {
										fprintf(stderr, " ...");
									};
									fprintf(stderr, "\n");
								};
							};
							o++;
						};
					};
				};
				if (has_options == 0) {
					break;
				};
				test--;
			};
		} else {
			fprintf(stdout, printformatstring, ipv6calc_actionstrings[j].token);
		};
	};
	if ((formatoptions & FORMATOPTION_machinereadable) == 0) fprintf(stderr, "\n");
};


/* print global common help */
void printhelp_common(void) {
	fprintf(stderr, "  [-d|--debug <debug value>] : debug value (bitwise like)\n");
	fprintf(stderr, "                                can also be set IPV6CALC_DEBUG environment value\n");
	fprintf(stderr, "  [-v|--version [-v [-v]]]   : version information (2 optional verbose levels)\n");
	fprintf(stderr, "  [-h|--help|-?]             : this online help\n");

#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, "\n");
	fprintf(stderr, "  [--db-ip2location-disable        ] : IP2Location support disabled\n");
	fprintf(stderr, "  [--db-ip2location-dir <directory>] : IP2Location database directory (default: %s)\n", ip2location_db_dir);
#ifdef SUPPORT_IP2LOCATION_DYN
	fprintf(stderr, "  [--db-ip2location-lib <file>]      : IP2Location library file (default: %s)\n", ip2location_lib_file);
#endif
#endif

#ifdef SUPPORT_GEOIP
	fprintf(stderr, "\n");
	fprintf(stderr, "  [--db-geoip-disable              ] : GeoIP support disabled\n");
	fprintf(stderr, "  [--db-geoip-dir       <directory>] : GeoIP database directory (default: %s)\n", geoip_db_dir);
#ifdef SUPPORT_GEOIP_DYN
	fprintf(stderr, "  [--db-geoip-lib       <file>]      : GeoIP library file (default: %s)\n", geoip_lib_file);
#endif
#endif
	return;
};

void printhelp_oldoptions(const struct option longopts[]) {
	int i = 0;
	
	printversion();
	printcopyright();
	fprintf(stderr, " Usage with old style (shortcut) options (going obsolete):\n");
	fprintf(stderr, "  <shortcut option> [<format option> ...] <input data> [...]\n");
	fprintf(stderr, "\n");

	while(longopts[i].name != NULL) {
		if (longopts[i].val >= CMD_shortcut_start && longopts[i].val <= CMD_shortcut_end) {
			fprintf(stderr, "  --%s\n", longopts[i].name);
		};
		i++;
	};
	
	fprintf(stderr, "\n");
	return;
};


/* print help for output type examples */


static void printhelp_output_base85(void) {
	fprintf(stderr, " Print a given IPv6 address in base85 format (RFC 1924), e.g.\n");
	fprintf(stderr, "  1080:0:0:0:8:800:200c:417a -> 4)+k&C#VzJ4br>0wv%%Yp\n");
};

static void printhelp_output_bitstring(void) {
	fprintf(stderr, " Print a given IPv6 address as a bitstring label for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff::1    -> \\[x3ffeffff000000000000000000000001/128].ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff::1/64 -> \\[x3ffeffff000000000000000000000001/64].ip6.arpa.\n");
	fprintf(stderr, "  --printsuffix 3ffe:ffff::1/64 -> \\[x0000000000000001/64]\n");
	fprintf(stderr, "  --printprefix 3ffe:ffff::1/64 -> \\[x3ffeffff00000000/64]\n");
};

static void printhelp_output_ipv6addr(void) {
	fprintf(stderr, " Print a given IPv6 address depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1    -> 3ffe:ffff:100:f101:0:0:0:1\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1/64 -> 3ffe:ffff:100:f101:0:0:0:1/64\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:100:f101::1 -> 3ffe:ffff:0100:f101:0000:0000:0000:0001\n");
	fprintf(stderr, "  Compressed, e.g.\n");
	fprintf(stderr, "   3ffe:ffff:0100:f101:0000:0000:0000:0001 -> 3ffe:ffff:100:f101::1\n");
};

static void printhelp_output_ipv6literal(void) {
	fprintf(stderr, " Print a given IPv6 address in literal format depending on format options:\n");
	fprintf(stderr, "  Uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8-0-0-0-0-0-1.ipv6-literal.net\n");
	fprintf(stderr, "  Full uncompressed, e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net\n");
	fprintf(stderr, "  Compressed (default), e.g.\n");
	fprintf(stderr, "   2001:db8::1 -> 2001-db8--1.ipv6-literal.net\n");
	fprintf(stderr, "  With Scope ID, e.g.\n");
	fprintf(stderr, "   fe80::1%%0    -> fe80--1s0.ipv6-literal.net\n");
};

static void printhelp_output_eui64(void) {
	fprintf(stderr, " Print a generated EUI-64 identifier, e.g.:\n");
	fprintf(stderr, "  00:50:BF:06:B4:F5 -> 0250:bfff:fe06:b4f5\n");
};

static void printhelp_output_iid_token(void) {
	fprintf(stderr, " Print generated interface identifier and token, e.g.:\n");
	fprintf(stderr, "  -> 4462:bdea:8654:776d 486072ff7074945e\n");
};

static void printhelp_output_revnibble_int(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.int.\n");
};

static void printhelp_output_revnibble_arpa(void) {
	fprintf(stderr, " Print a given IPv6 address in dot separated reverse nibble format for use with DNS, e.g.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1\n    -> 1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64\n    -> 1.0.1.f.0.0.1.0.0.0.4.0.e.f.f.3.ip6.arpa.\n");
};

static void printhelp_output_ifinet6void(void) {
	fprintf(stderr, " Print a given IPv6 address to same format shown in Linux /proc/net/if_inet6:\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1    -> 3ffeffff0100f1010000000000000001 00\n");
	fprintf(stderr, "  3ffe:ffff:100:f101::1/64 -> 3ffeffff0100f1010000000000000001 00 40\n");
};

static void printhelp_output_ipv4addr(void) {
	fprintf(stderr, " Print an IPv4 address\n");
};

static void printhelp_output_revipv4(void) {
	fprintf(stderr, " Print an IPv4 address in reverse format for PTR/DNS\n");
	fprintf(stderr, "  1.2.3.4  -> 4.3.2.1.in-addr.arpa\n");
};

static void printhelp_output_addrtype(void) {
	fprintf(stderr, " Print type of a given IPv4/IPv6 address:\n");
	fprintf(stderr, "  IPv4 address  -> ipv4-addr.addrtype.ipv6calc\n");
	fprintf(stderr, "  IPv6 address  -> ipv6-addr.addrtype.ipv6calc\n");
};

static void printhelp_output_ipv6addrtype(void) {
	fprintf(stderr, " Print type of a given IPv6 address:\n");
	fprintf(stderr, "  3ffe::/16         -> 6bone-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2002::/16         -> 6to4-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  2001::/16         -> productive-global.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fe80::/10         -> link-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  fec0::/10         -> site-local.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::ffff:0:0:0:0/96 -> mapped-ipv4.ipv6addrtype.ipv6calc\n");
	fprintf(stderr, "  ::0:0:0:0/96      -> compat-ipv4.ipv6addrtype.ipv6calc\n");
};

static void printhelp_output_ouitype(void) {
	fprintf(stderr, " Print OUI name of a given IPv6 address:\n");
	fprintf(stderr, "  IID local scope   -> local-scoppe.ouitype.ipv6calc\n");
	fprintf(stderr, "  IID global scope  -> vendorname.ouitype.ipv6calc\n");
};

static void printhelp_output_octal(void) {
	fprintf(stderr, " Print octal representation of a given IPv6 address:\n");
	fprintf(stderr, "  (useful for djbdns/tinydns)\n");
	fprintf(stderr, "  3ffe:ffff::1 ->\n    \\77\\376\\377\\377\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\0\\1\n");
	fprintf(stderr, "  --printfulluncompressed 3ffe:ffff::1 ->\n    \\077\\376\\377\\377\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\000\\001\n");
};

void printhelp_output_dispatcher(const uint32_t outputtype) {
	int i, j;

	printversion();
	
	fprintf(stderr, "\n");

	switch (outputtype) {
		case FORMAT_base85:
			printhelp_output_base85();
			break;

		case FORMAT_bitstring:
			printhelp_output_bitstring();
			break;
			
		case FORMAT_ipv6addr:
			printhelp_output_ipv6addr();
			break;
			
		case FORMAT_ipv6literal:
			printhelp_output_ipv6literal();
			break;
			
		case FORMAT_eui64:
			printhelp_output_eui64();
			break;

		case FORMAT_revnibbles_int:
			printhelp_output_revnibble_int();
			break;

		case FORMAT_revnibbles_arpa:
			printhelp_output_revnibble_arpa();
			break;

		case FORMAT_ifinet6:
			printhelp_output_ifinet6void();
			break;
			
		case FORMAT_iid_token:
			printhelp_output_iid_token();
			break;

		case FORMAT_ipv4addr:
			printhelp_output_ipv4addr();
			break;

		case FORMAT_addrtype:
			printhelp_output_addrtype();
			break;

		case FORMAT_ipv6addrtype:
			printhelp_output_ipv6addrtype();
			break;
			
		case FORMAT_ouitype:
			printhelp_output_ouitype();
			break;
			
		case FORMAT_revipv4:
			printhelp_output_revipv4();
			break;
			
		case FORMAT_octal:
			printhelp_output_octal();
			break;
			
		default:
			fprintf(stderr, " Examples currently missing...!\n");
			break;
	};
	
	/* looking for outtype */
	for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_outputformatoptionmap); i++) {
		if (outputtype == ipv6calc_outputformatoptionmap[i][0]) {
			if (ipv6calc_outputformatoptionmap[i][1] == 0) {
				fprintf(stderr, " No format options supported\n");
				break;
			};
			
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "Format value: %08x\n", (unsigned int) ipv6calc_outputformatoptionmap[i][1]);
			};
			
			fprintf(stderr, "\n");
			fprintf(stderr, " Available format options:\n");

			/* run through format options */
			for (j = 0; j < MAXENTRIES_ARRAY(ipv6calc_formatoptionstrings); j++) {
				if ((ipv6calc_outputformatoptionmap[i][1] & ipv6calc_formatoptionstrings[j].number) != 0) {
					fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
				};
			};
			break;
		};
	};
	fprintf(stderr, "\n");
};

/* help regarding action command */
/* in: embedded = 1 : do not show version and command */
void printhelp_action_dispatcher(const uint32_t action, const int embedded) {
	int i, j;
	char method_name[32];

	if (embedded != 1) {
		printversion();
	};
	
	fprintf(stderr, "\n");

	switch (action) {
		case ACTION_mac_to_eui64:
			fprintf(stderr, " help still missing - sorry.\n");
			break;
			
		case ACTION_ipv4_to_6to4addr:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_anonymize:
			if (embedded != 1) {
				fprintf(stderr, " Anonymize given address according to preset or custom values, e.g.\n");
				fprintf(stderr, "  ipv6calc -A anonymize 2001:db8:2280:6901:224:21ff:fe01:2345 --anonymize-preset zeroize-standard\n");
				fprintf(stderr, "   2001:db8:2280:6900:224:21ff:fe00:0\n");
				fprintf(stderr, "  ipv6calc -A anonymize 2001:db8:2280:6901:224:21ff:fe01:2345 --anonymize-preset anonymize-standard\n");
				fprintf(stderr, "   2001:db8:2280:6909:a929:4291:4022:4217\n");
				fprintf(stderr, "\n");
			};

			fprintf(stderr, "  Shortcut for anonymization presets:\n");
			fprintf(stderr, "   --anonymize-standard (default)\n");
			fprintf(stderr, "   --anonymize-careful\n");
			fprintf(stderr, "   --anonymize-paranoid\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  Supported methods [--anonymize-method METHOD]:\n");
			for (i = 0; i < sizeof(ipv6calc_anon_methods) / sizeof(s_ipv6calc_anon_methods); i++) {
				fprintf(stderr, "   %-10s: %s\n", ipv6calc_anon_methods[i].name, ipv6calc_anon_methods[i].description);
			};
			fprintf(stderr, "\n");

			fprintf(stderr, "  Available presets (shortcut names) [--anonymize-preset PRESET-NAME]:\n");

			for (i = 0; i < MAXENTRIES_ARRAY(ipv6calc_anon_set_list); i++) {
				snprintf(method_name, sizeof(method_name) - 1, "%s", "unknown"); // default

				for (j = 0; j < sizeof(ipv6calc_anon_methods) / sizeof(s_ipv6calc_anon_methods); j++) {
					if (ipv6calc_anon_methods[j].method == ipv6calc_anon_set_list[i].method) {
						snprintf(method_name, sizeof(method_name) - 1, "%s", ipv6calc_anon_methods[j].name);
						break;
					};
				};

				fprintf(stderr, "   %-20s (%2s): mask-ipv6=%3d mask-ipv4=%2d mask-eui64=%2d mask-mac=%2d mask-autoadjust=%-3s method=%s\n", ipv6calc_anon_set_list[i].name, ipv6calc_anon_set_list[i].name_short, ipv6calc_anon_set_list[i].mask_ipv6, ipv6calc_anon_set_list[i].mask_ipv4, ipv6calc_anon_set_list[i].mask_eui64, ipv6calc_anon_set_list[i].mask_mac, (ipv6calc_anon_set_list[i].mask_autoadjust == 1) ? "yes" : "no", method_name);
			};
			fprintf(stderr, "\n");

			fprintf(stderr, "  Custom control:\n");
			fprintf(stderr, "  --mask-ipv4  <bits>     : mask IPv4 address [0-32] (even if occurs in IPv6 address)\n");
			fprintf(stderr, "  --mask-ipv6  <bits>     : mask IPv6 prefix [0-64] (only applied to related address types)\n");
			fprintf(stderr, "  --mask-eui64 <bits>     : mask EUI-64 address or IPv6 interface identifier [0-64]\n");
			fprintf(stderr, "  --mask-mac   <bits>     : mask MAC address [0-48]\n");
			fprintf(stderr, "  --mask-autoadjust yes|no: autoadjust mask to keep type/vendor information regardless of less given mask\n");

			break;
			
		case ACTION_iid_token_to_privacy:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_prefix_mac_to_ipv6:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_6rd_local_prefix:
			fprintf(stderr, " help still missing - sorry.\n");
			break;

		case ACTION_filter:
			fprintf(stderr, " Filter given addresses from stdout by filter expression, e.g.\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E iid-local\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E iid-local,global-unicast\n");
			fprintf(stderr, "  echo '2001:db8::1' | ipv6calc [-A filter] -E ^iid-randomy\n");
			fprintf(stderr, "\n");
			fprintf(stderr, " (note: since version 0.95.0 '-A filter' is autoselected if option '-E <filter expression>' is given)\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv6 address filter tokens:\n");
			fprintf(stderr, "   ipv6 ");
			for (i = 0; i < (int) (sizeof(ipv6calc_ipv6addrtypestrings) / sizeof(ipv6calc_ipv6addrtypestrings[0])); i++ ) {
				fprintf(stderr, " %s", ipv6calc_ipv6addrtypestrings[i].token);
			};
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  IPv4 address filter tokens:\n");
			fprintf(stderr, "   ipv4 ");
			for (i = 0; i < (int) (sizeof(ipv6calc_ipv4addrtypestrings) / sizeof(ipv6calc_ipv4addrtypestrings[0])); i++ ) {
				fprintf(stderr, " %s", ipv6calc_ipv4addrtypestrings[i].token);
			};
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  EUI-48/MAC address filter tokens:\n");
			fprintf(stderr, "   ");
			fprintf(stderr, " IMPLEMENTATION MISSING");
			fprintf(stderr, "\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "  EUI-64 address filter tokens:\n");
			fprintf(stderr, "   ");
			fprintf(stderr, " IMPLEMENTATION MISSING");
			fprintf(stderr, "\n");
			break;
	};
};


/***************************
 * version information
 * *************************/

void ipv6calc_print_features(void) {
#ifdef SUPPORT_IP2LOCATION
	fprintf(stderr, " IP2Location");
#endif
#ifdef SUPPORT_GEOIP
	fprintf(stderr, " GeoIP");
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, " GeoIPv6");
#endif
#endif
#ifdef SUPPORT_DB_IEEE
	fprintf(stderr, " DB_IEEE");
#endif
#ifdef SUPPORT_DB_IPV4
	fprintf(stderr, " DB_IPV4");
#endif
#ifdef SUPPORT_DB_IPV6
	fprintf(stderr, " DB_IPV6");
#endif
};


/* display features in verbose mode */
void ipv6calc_print_features_verbose(const int level_verbose) {
	char string[NI_MAXHOST];

#ifdef SUPPORT_GEOIP
#ifndef SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_V6
#if defined (SUPPORT_GEOIP_COUNTRY_CODE_BY_ADDR_V6) && defined (SUPPORT_GEOIP_COUNTRY_NAME_BY_ADDR_V6)
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support\n");
#else
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 & IPv6 support (in compatibility mode)\n");
#endif
#else
	fprintf(stderr, "GeoIP support enabled, compiled with IPv4 support only\n");
#endif
#ifdef SUPPORT_GEOIP_LIB_VERSION
	fprintf(stderr, "GeoIP dynamic library version (on this system): %s\n", libipv6calc_db_wrapper_GeoIP_lib_version());
#else
	fprintf(stderr, "GeoIP dynamic library version (on this system): compiled without detection\n");
#endif
#else // SUPPORT_GEOIP_DYN
#ifdef SUPPORT_GEOIP_V6
	fprintf(stderr, "GeoIP support by dynamic library load, compiled with IPv4 & IPv6 support\n");
#else
	fprintf(stderr, "GeoIP support by dynamic library load, compiled with IPv4 support only\n");
#endif // SUPPORT_GEOIP_DYN
	fprintf(stderr, "GeoIP configured dynamic library file and version: %s %s\n", geoip_lib_file, libipv6calc_db_wrapper_GeoIP_lib_version());
#endif

	//TODO: list of GeoIP files
#ifdef SUPPORT_GEOIP_V6
	//TODO: list of GeoIP files
#endif
	libipv6calc_db_wrapper_GeoIP_wrapper_info(string, sizeof(string));
	fprintf(stderr, "%s\n", string);
#else
	fprintf(stderr, "GeoIP support not enabled\n");
#endif

#ifdef SUPPORT_IP2LOCATION
#ifndef SUPPORT_IP2LOCATION_DYN
	fprintf(stderr, "IP2Location support enabled, compiled with API version: %s\n", xmakestr(API_VERSION));
#else
	fprintf(stderr, "IP2Location support by dynamic library load\n");
	fprintf(stderr, "IP2Location configured dynamic library file and version: %s %s\n", ip2location_lib_file, libipv6calc_db_wrapper_IP2Location_lib_version());
#endif
	// TODO: show base directory
	/*
	if (file_ip2location_ipv4 != NULL && strlen(file_ip2location_ipv4) > 0) {
		fprintf(stderr, "IP2Location IPv4 default file: %s\n", file_ip2location_ipv4);
	} else {
		fprintf(stderr, "IP2Location IPv4 default file: not configured\n");
	};
	if (file_ip2location_ipv6 != NULL && strlen(file_ip2location_ipv6) > 0) {
		fprintf(stderr, "IP2Location IPv6 default file: %s\n", file_ip2location_ipv6);
	} else {
		fprintf(stderr, "IP2Location IPv6 default file: not configured\n");
	};
	*/
#else
	fprintf(stderr, "IP2Location support not enabled\n");
#endif

#ifdef SUPPORT_BUILTIN
	libipv6calc_db_wrapper_BuiltIn_wrapper_info(string, sizeof(string));
	fprintf(stderr, "%s\n", string);
#else
	fprintf(stderr, "BuiltIn support not enabled\n");
#endif

	libipv6calc_db_wrapper_print_db_info(level_verbose, "");
};

