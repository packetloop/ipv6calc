/*
 * Project    : ipv6calc
 * File       : libipv6addr.c
 * Version    : $Id: libipv6addr.c,v 1.10 2003/04/04 20:23:31 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de> except the parts taken from kernel source
 *
 * Information:
 *  Function library for IPv6 address handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "libipv6addr.h"
#include "librfc1884.h"
#include "libipv6addr.h"
#include "ipv6calctypes.h"
#include "libipv6calc.h"
#include "libipv6calcdebug.h"


/*
 * function returns an octett of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numoctett  = number of octett (0 = MSB, 15 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getoctett"
uint8_t ipv6addr_getoctett(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctett) {
	uint8_t retval;
	
	if ( numoctett > 15 ) {
		fprintf(stderr, "%s: given ocett number '%u' is out of range!\n", DEBUG_function_name, numoctett);
		exit(EXIT_FAILURE);
	};

	retval = ipv6addrp->in6_addr.s6_addr[numoctett];

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a word of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getword"
uint16_t ipv6addr_getword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword) {
	uint16_t retval;
	
	if ( numword > 7 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numword * 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] );

	return (retval);
};
#undef DEBUG_function_name


/*
 * function returns a dword of an IPv6 address
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_getdword"
uint32_t ipv6addr_getdword(const ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword) {
	uint32_t retval;
	
	if ( numdword > 3 ) {
		fprintf(stderr, "%s: given dword number '%u' is out of range!\n", DEBUG_function_name, numdword);
		exit(EXIT_FAILURE);
	};

	retval = ( ipv6addrp->in6_addr.s6_addr[numdword * 4] << 24 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] << 16 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] << 8 ) \
		| ( ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] ); 

	return (retval);
};
#undef DEBUG_function_name


/*
 * function sets an octett of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numoctett   = number of word (0 = MSB, 15 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setoctett"
void ipv6addr_setoctett(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numoctett, const unsigned int value) {
	
	if ( numoctett > 15 ) {
		fprintf(stderr, "%s: given octett number '%u' is out of range!\n", DEBUG_function_name, numoctett);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000000ff ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numoctett] = (uint8_t) value;

	return;
};
#undef DEBUG_function_name


/*
 * function sets a word of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numword   = number of word (0 = MSB, 7 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setword"
void ipv6addr_setword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numword, const unsigned int value) {
	if ( numword > 7 ) {
		fprintf(stderr, "%s: given word number '%u' is out of range!\n", DEBUG_function_name, numword);
		exit(EXIT_FAILURE);
	};
	
	if ( value > 0x0000ffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numword * 2    ] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numword * 2 + 1] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};
#undef DEBUG_function_name


/*
 * function sets a dword of an IPv6 address
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 * in: numdword  = number of word (0 = MSB, 3 = LSB)
 * in: value     = value to set
 * additional: calls exit on out of range
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_setdword"
void ipv6addr_setdword(ipv6calc_ipv6addr *ipv6addrp, const unsigned int numdword, const unsigned int value) {
	
	if ( numdword > 3 ) {
		fprintf(stderr, "%s: given dword number '%u' is out of range!\n", DEBUG_function_name, numdword);
		exit(EXIT_FAILURE);
	};

	if ( value > 0xffffffffu ) {
		fprintf(stderr, "%s: given value '%x' is out of range!\n", DEBUG_function_name, value);
		exit(EXIT_FAILURE);
	}; 

	ipv6addrp->in6_addr.s6_addr[numdword * 4    ] = (uint8_t) ( ( value & 0xff000000 ) >> 24 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 1] = (uint8_t) ( ( value & 0x00ff0000 ) >> 16 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 2] = (uint8_t) ( ( value & 0x0000ff00 ) >>  8 );
	ipv6addrp->in6_addr.s6_addr[numdword * 4 + 3] = (uint8_t) ( ( value & 0x000000ff )       );

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_clear"
void ipv6addr_clear(ipv6calc_ipv6addr *ipv6addrp) {
	int i;

	for (i = 0; i < (int) (sizeof(ipv6addrp->in6_addr.s6_addr) / sizeof(ipv6addrp->in6_addr.s6_addr[0])); i++) {
		ipv6addrp->in6_addr.s6_addr[i] = 0;
	};
	
	/* Clear IPv6 address scope */
	ipv6addrp->scope = 0;

	/* Clear valid flag */
	ipv6addrp->flag_valid = 0;

	return;
};
#undef DEBUG_function_name


/*
 * function clears the IPv6 structure
 *
 * mod: ipv6addrp = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_clearall"
void ipv6addr_clearall(ipv6calc_ipv6addr *ipv6addrp) {
	ipv6addr_clear(ipv6addrp);

	/* Clear other field */
	ipv6addrp->bit_start = 1;
	ipv6addrp->bit_end = 128;
	ipv6addrp->flag_startend_use = 0;
	ipv6addrp->flag_prefixuse = 0;
	ipv6addrp->prefixlength = 0;
	ipv6addrp->flag_valid = 0;
	
	return;
};
#undef DEBUG_function_name


/*
 * function copies the IPv6 structure
 *
 * in:  ipv6addrp  = pointer to IPv6 address structure
 * mod: ipv6addrp2 = pointer to IPv6 address structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addr_copy"
void ipv6addr_copy(ipv6calc_ipv6addr *ipv6addrp_dst, const ipv6calc_ipv6addr *ipv6addrp_src) {
	int i;

	for (i = 0; i < (int) sizeof(ipv6calc_ipv6addr); i++) {
		*(ipv6addrp_dst + i) = *(ipv6addrp_src + i);
	};
	
	return;
};
#undef DEBUG_function_name


/*
 * Get type of an IPv6 address
 *
 * with credits to kernel and USAGI developer team
 * basic code was taken from "kernel/net/ipv6/addrconf.c"
 *
 * in: ipv6addrp = pointer to IPv6 address structure
 */

uint32_t ipv6addr_gettype(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t type = 0;
	uint32_t st, st1, st2, st3;

	st =  ipv6addr_getdword(ipv6addrp, 0); /* 32 MSB */
	st1 = ipv6addr_getdword(ipv6addrp, 1);
	st2 = ipv6addr_getdword(ipv6addrp, 2);
	st3 = ipv6addr_getdword(ipv6addrp, 3); /* 32 LSB */

	/* unspecified address */
	if ( (st == 0) && (st1 == 0) && (st2 == 0) && (st3 == 0) ) {
		type |= IPV6_NEW_ADDR_UNSPECIFIED;
		return (type);
	};

	/* address space information  */
	if ((st & 0xE0000000u) == 0x20000000u) {
		/* 2000::/3 -> global unicast */
		type |= IPV6_NEW_ADDR_AGU;
	};
	
	/* address space information  */
	if ((st & 0xFFFF0000u) == 0x3FFE0000u) {
		/* 3ffe::/16 -> experimental 6bone */
		type |= IPV6_NEW_ADDR_6BONE;
	};

	if ((st & 0xFFFF0000u) == 0x20020000u) {
		/* 2002::/16 -> 6to4 tunneling */
		type |= IPV6_NEW_ADDR_6TO4;
	};
	
	if ((st & 0xFFFF0000u) == 0x20010000u) {
		/* 2001::/16 -> productive IPv6 address space */
		type |= IPV6_NEW_ADDR_PRODUCTIVE;
	};
	
	if ((st2 == (uint32_t) 0x00000001u) && (st3 & 0xFF000000u) == 0xFF000000u) {
		/* ..:0000:0001:ffxx:xxxx solicited node suffix */
		type |= IPV6_NEW_ADDR_SOLICITED_NODE;
	};

	if (st2 == (uint32_t) 0x00005EFEu) {
		/* ..:0000:5EFE:xx.xx.xx.xx ISATAP suffix */
		type |= IPV6_NEW_ADDR_ISATAP;
	};

	/* Consider all addresses with the first three bits different of
	   000 and 111 as unicasts.
	 */

	/* original from kernel, only | and return changed */	
	if ( ((st & 0xE0000000u) != 0x00000000u) && (st & 0xE0000000u) != 0xE0000000u) {
		type |= IPV6_ADDR_UNICAST;
		return (type);
	};

	if ((st & (0xFF000000u)) == (0xFF000000u)) {
		type |= IPV6_ADDR_MULTICAST;

		switch((st & 0x00FF0000u)) {
			case (0x00010000u):
				type |= IPV6_ADDR_LOOPBACK;
				break;

			case (0x00020000u):
				type |= IPV6_ADDR_LINKLOCAL;
				break;

			case (0x00050000u):
				type |= IPV6_ADDR_SITELOCAL;
				break;
		};
		return (type);
	}
	
	if ((st & 0xFFC00000u) == 0xFE800000u) {
		type |=  IPV6_ADDR_LINKLOCAL | IPV6_ADDR_UNICAST;
		return (type);
	};

	if ((st & 0xFFC00000u) == 0xFEC00000u) {
		type |= IPV6_ADDR_SITELOCAL | IPV6_ADDR_UNICAST;
		return (type);
	}; 

	if ((st | st1) == 0) {
		if (st2 == 0) {
			if (st3 == 0) {
				type |= IPV6_ADDR_ANY;
				return (type);
			};

			if (st3 == (uint32_t) 0x00000001u) {
				type |= IPV6_ADDR_LOOPBACK | IPV6_ADDR_UNICAST;
				return (type);
			};

			type |= IPV6_ADDR_COMPATv4 | IPV6_ADDR_UNICAST;
			return (type);
		}

		if (st2 == (uint32_t) 0x0000ffffu)
			type |= IPV6_ADDR_MAPPED;
			return (type);
	};

	type |= IPV6_ADDR_RESERVED;
	return (type);
};


/*
 * Get IPv6 address assignement information
 *
 * See also: http://www.iana.org/assignments/ipv6-tla-assignments
 *
 * in : ipv6addrp = pointer to IPv6 address structure
 * out: assignment number (-1 = no result)
 */
int ipv6addr_getregistry(const ipv6calc_ipv6addr *ipv6addrp) {
	uint32_t st, st1, st2, st3;

	st =  ipv6addr_getdword(ipv6addrp, 0); /* 32 MSB */
	st1 = ipv6addr_getdword(ipv6addrp, 1);
	st2 = ipv6addr_getdword(ipv6addrp, 2);
	st3 = ipv6addr_getdword(ipv6addrp, 3); /* 32 LSB */
	
	if ((st & 0xFFFF0000u) == 0x3FFE0000u) {
		/* 3ffe::/16 -> 6bone space */
		return(IPV6_ADDR_REGISTRY_6BONE);
	};
		
	if ((st & 0xFFFF0000u) == 0x20010000u) {
		/* 2001::/16 -> productive IPv6 address space */
		
		if ( (st & 0xFFFFFE00u) == 0x20010000u) {
			/* 2001:0000::/23 -> IANA */
			return(IPV6_ADDR_REGISTRY_IANA);
		};
		
		if ( (st & 0xFFFFFE00u) == 0x20010200u ||
		     (st & 0xFFFFFE00u) == 0x20010C00u ) {
			/* 2001:0200::/23 -> APNIC */
			/* 2001:0C00::/23 -> APNIC */
			return(IPV6_ADDR_REGISTRY_APNIC);
		};
		
		if ( (st & 0xFFFFFE00u) == 0x20010400u) {
			/* 2001:0400::/23 -> ARIN */
			return(IPV6_ADDR_REGISTRY_ARIN);
		};
		
		if ( (st & 0xFFFFFE00u) == 0x20010600u ||
		     (st & 0xFFFFFE00u) == 0x20010800u || 
		     (st & 0xFFFFFE00u) == 0x20010A00u )  {
			/* 2001:0600::/23 -> RIPE NCC */
			/* 2001:0800::/23 -> RIPE NCC */
			/* 2001:0A00::/23 -> RIPE NCC */
			return(IPV6_ADDR_REGISTRY_RIPE);
		};

		return (IPV6_ADDR_REGISTRY_RESERVED);
	};

	return(-1);
};


/*
 * Get IPv6 address assignement information as string
 *
 * in : ipv6addrp = pointer to IPv6 address structure
 * mod: resultstring
 * ret: 1=not found, 0=ok
 */
int libipv6addr_get_registry_string(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring) {
	int i, registry;

	registry = ipv6addr_getregistry(ipv6addrp);

	if ( registry != -1 ) {
		for ( i = 0; i < (int) (sizeof(ipv6calc_ipv6addrregistry) / sizeof(ipv6calc_ipv6addrregistry[0])); i++ ) {
			if ( ipv6calc_ipv6addrregistry[i].number == registry ) {
				snprintf(resultstring, NI_MAXHOST, "%s", ipv6calc_ipv6addrregistry[i].tokensimple);
				return (0);
			};
		};
	};

	return (1);
};


/*
 * function stores an IPv6 address string into a structure
 *
 * in : *addrstring = IPv6 address
 * out: *resultstring = error message
 * out: ipv6addrp = changed IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/addr_to_ipv6addrstruct"
int addr_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result, i, cpoints = 0, ccolons = 0;
	char *addronlystring, *cp, tempstring[NI_MAXHOST], *cptr, **ptrptr;
	int expecteditems = 0;
	int temp[8];
	unsigned int compat[4];
	uint32_t scope = 0;

	ptrptr = &cptr;

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got input: %s\n", DEBUG_function_name, addrstring);
	};

	if (strlen(addrstring) > sizeof(tempstring) - 1) {
		fprintf(stderr, "Input too long: %s\n", addrstring);
		return (1);
	};

	snprintf(tempstring, sizeof(tempstring), "%s", addrstring);
	
	ipv6addr_clearall(ipv6addrp);

	/* save prefix length first, if available */
	ipv6addrp->flag_prefixuse = 0; /* reset flag first */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Call strtok_r\n", DEBUG_function_name);
	};

	addronlystring = strtok_r(tempstring, "/", ptrptr);
	
	if ( addronlystring == NULL ) {
		fprintf(stderr, "Strange input: %s\n", addrstring);
		return (1);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got address only string: %s\n", DEBUG_function_name, addronlystring);
	};

	cp = strtok_r (NULL, "/", ptrptr);
	if ( cp != NULL ) {
		i = atoi(cp);
		if (i < 0 || i > 128 ) {
			snprintf(resultstring, NI_MAXHOST, "Illegal prefix length: '%s'", cp);
			retval = 1;
			return (retval);
		};
		ipv6addrp->flag_prefixuse = 1;
		ipv6addrp->prefixlength = (uint8_t) i;
		
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: prefix length %u\n", DEBUG_function_name, (unsigned int) ipv6addrp->prefixlength);
			fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
		};
	};

	/* uncompress string, if necessary */
	if (strstr(addronlystring, "::") != NULL) {
		result = compaddr_to_uncompaddr(addronlystring, tempstring);
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Result of uncompressed string: '%s'\n", DEBUG_function_name, tempstring);
		};
		if ( result != 0 ) {
			snprintf(resultstring, NI_MAXHOST, "%s", tempstring);
			retval = 1;
			return (retval);
		};
	} else {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Copy string: '%s'\n", DEBUG_function_name, addronlystring);
		};
		strncpy(tempstring, addronlystring, sizeof(tempstring) - 1);
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Check string: '%s'\n", DEBUG_function_name, tempstring);
	};

	/* count ":", must be 6 (compat) or 7 (other) */
	for (i = 0; i < (int) strlen(tempstring); i++) {
		if (tempstring[i] == ':') {
			ccolons++;
		};
		if (tempstring[i] == '.') {
			cpoints++;
		};
	};
	if ( ! ( ( ( ccolons == 7 ) && ( cpoints == 0 ) ) ||  ( ( ccolons == 6 ) && ( cpoints == 3 ) ) ) ) {
		if (strstr(addronlystring, "::")) {
			snprintf(resultstring, NI_MAXHOST, "Error, given address expanded to '%s' is not valid!", tempstring);
		} else {
			snprintf(resultstring, NI_MAXHOST, "Error, given address '%s' is not valid!", addrstring);
		};
		retval = 1;
		return (retval);
	};

	/* clear variables */
	for ( i = 0; i <= 3; i++ ) {
		compat[i] = 0;
	};

	ipv6addr_clear(ipv6addrp);
	
	if ( ccolons == 6 ) {
		/* compatv4/mapped format */
		expecteditems = 10;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &compat[0], &compat[1], &compat[2], &compat[3]);
		/* check compat */
		for ( i = 0; i <= 3; i++ ) {
			if ( compat[i] > 255 )	{
				snprintf(resultstring, NI_MAXHOST, "Error, given compatv4/mapped address '%s' is not valid (%d)!", addrstring, compat[i]);
				retval = 1;
				return (retval);
			};
		};
		temp[6] = (int) (( compat[0] << 8 ) | compat[1]);
		temp[7] = (int) (( compat[2] << 8 ) | compat[3]);
		scope = IPV6_ADDR_COMPATv4;
	} else {
		/* normal format */
		expecteditems = 8;
		result = sscanf(tempstring, "%x:%x:%x:%x:%x:%x:%x:%x", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5], &temp[6], &temp[7]);
	};
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: reading into array, got items: %d\n", DEBUG_function_name, result);
	};

	if ( result != expecteditems ) {
		snprintf(resultstring, NI_MAXHOST, "Error splitting address %s, got %d items instead of %d!", addronlystring, result, expecteditems);
		retval = 1;
		return (retval);
	};

	/* check address words range */
	for ( i = 0; i <= 7; i++ ) {
		if ( (temp[i] < 0) || (temp[i] > 0xffff) )	{
			snprintf(resultstring, NI_MAXHOST, "Error, given address '%s' is not valid on position %d (%x)!", addronlystring, i, temp[i]);
			retval = 1;
			return (retval);
		};
	};
	
	/* copy into structure */
	for ( i = 0; i <= 7; i++ ) {
		if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
			fprintf(stderr, "%s: Push word %u: %04x\n", DEBUG_function_name, (unsigned int) i, (unsigned int) temp[i]);
		};
		ipv6addr_setword(ipv6addrp, (unsigned int) i, (unsigned int) temp[i]);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: In structure %08x %08x %08x %08x\n", DEBUG_function_name, (unsigned int) ipv6addr_getdword(ipv6addrp, 0), (unsigned int) ipv6addr_getdword(ipv6addrp, 1), (unsigned int) ipv6addr_getdword(ipv6addrp, 2), (unsigned int) ipv6addr_getdword(ipv6addrp, 3));
		fprintf(stderr, "%s: In structure %04x %04x %04x %04x %04x %04x %04x %04x\n", DEBUG_function_name, (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) ipv6addr_getword(ipv6addrp, 1), (unsigned int) ipv6addr_getword(ipv6addrp, 2), (unsigned int) ipv6addr_getword(ipv6addrp, 3), (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
	};
	
	scope = ipv6addr_gettype(ipv6addrp); 

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got scope %08x\n", DEBUG_function_name, (unsigned int) scope);
	};

	ipv6addrp->scope = scope;
	
	/* currently unused code - forgotten why there... :-(
	if ( scope != 0 ) { */
		/* test, whether compatv4/mapped/ISATAP is really one */
	/*
		if ( (ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED | IPV6_NEW_ADDR_ISATAP)) == 0 ) {
			snprintf(resultstring, NI_MAXHOST, "Error, given address '%s' is not valid compatv4/mapped/ISATAP one!", addrstring);
			retval = 1;
			return (retval);
		};
	};*/

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: First word is: %04x, address info value: %08x\n", DEBUG_function_name, (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) scope);
		fprintf(stderr, "%s: flag_prefixuse %d\n", DEBUG_function_name, ipv6addrp->flag_prefixuse);
	};
	
	ipv6addrp->flag_valid = 1;
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
static int ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];
	
	/* print array */
	if ( (ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) {
		if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "%04x:%04x:%04x:%04x:%04x:%04x:%u.%u.%u.%u", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
		} else {
			snprintf(tempstring, sizeof(tempstring), "%x:%x:%x:%x:%x:%x:%u.%u.%u.%u", (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) ipv6addr_getword(ipv6addrp, 1), (unsigned int) ipv6addr_getword(ipv6addrp, 2), (unsigned int) ipv6addr_getword(ipv6addrp, 3), (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addrp->in6_addr.s6_addr[12], (unsigned int) ipv6addrp->in6_addr.s6_addr[13], (unsigned int) ipv6addrp->in6_addr.s6_addr[14], (unsigned int) ipv6addrp->in6_addr.s6_addr[15]);
		};
	} else {
		if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
			snprintf(tempstring, sizeof(tempstring), "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", ipv6addr_getword(ipv6addrp, 0), ipv6addr_getword(ipv6addrp, 1), ipv6addr_getword(ipv6addrp, 2), ipv6addr_getword(ipv6addrp, 3), ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));
		} else {
			snprintf(tempstring, sizeof(tempstring), "%x:%x:%x:%x:%x:%x:%x:%x", (unsigned int) ipv6addr_getword(ipv6addrp, 0), (unsigned int) ipv6addr_getword(ipv6addrp, 1), (unsigned int) ipv6addr_getword(ipv6addrp, 2), (unsigned int) ipv6addr_getword(ipv6addrp, 3), (unsigned int) ipv6addr_getword(ipv6addrp, 4), (unsigned int) ipv6addr_getword(ipv6addrp, 5), (unsigned int) ipv6addr_getword(ipv6addrp, 6), (unsigned int) ipv6addr_getword(ipv6addrp, 7));
		};
	};

	if (ipv6addrp->flag_prefixuse == 1) {
		/* append prefix length */
		snprintf(resultstring, NI_MAXHOST, "%s/%u", tempstring, ipv6addrp->prefixlength);
	} else {
		snprintf(resultstring, NI_MAXHOST, "%s", tempstring);
	};

	retval = 0;	
	return (retval);
};


/*
 * stores the prefix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure, formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddrprefix"
static int ipv6addrstruct_to_uncompaddrprefix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* test for misuse */
	if ( ((ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && (ipv6addrp->prefixlength > 96) ) {
		snprintf(resultstring, NI_MAXHOST, "Error, cannot print prefix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 0 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, cannot print prefix of a address with prefix length 0!");
		retval = 1;
		return (retval);
	};

	max = ( ipv6addrp->prefixlength - 1 ) / 16;
	i = 0;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x", tempstring1, ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x", tempstring1, ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1), "%s", tempstring2);
	};
	snprintf(resultstring, NI_MAXHOST, "%s", tempstring1);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};

	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


/*
 * function stores the suffix of an ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddrsuffix"
static int ipv6addrstruct_to_uncompaddrsuffix(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	unsigned int max, i;
	char tempstring1[NI_MAXHOST], tempstring2[NI_MAXHOST];

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};

	/* test for misuse */
	if ( ( (ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0) && ( ipv6addrp->prefixlength > 96 ) ) {
		snprintf(resultstring, NI_MAXHOST, "Error, cannot print suffix of a compatv4/mapped address with prefix length bigger than 96!");
		retval = 1;
		return (retval);
	};
	if ( ipv6addrp->prefixlength == 128 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, cannot print suffix of a address with prefix length 128!");
		retval = 1;
		return (retval);
	};

	max = 7;
	i   = ipv6addrp->prefixlength / 16;
	tempstring1[0] = '\0';
	while (i <= max ) {
		if ( ( ( ipv6addrp->scope & (IPV6_ADDR_COMPATv4 | IPV6_ADDR_MAPPED)) != 0 ) && ( i == 6 ) ) {
			snprintf(tempstring2, sizeof(tempstring2), "%s%u.%u.%u.%u", tempstring1, ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);
			i = max;
		} else if ( i < max ) {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x:", tempstring1, ipv6addr_getword(ipv6addrp, i));
			};
		} else {
			if ( (formatoptions & FORMATOPTION_printfulluncompressed) != 0 ) {
				snprintf(tempstring2, sizeof(tempstring2), "%s%04x", tempstring1, ipv6addr_getword(ipv6addrp, i));
			} else {
				snprintf(tempstring2, sizeof(tempstring2), "%s%x", tempstring1, ipv6addr_getword(ipv6addrp, i));
			};
		};
		i++;
		snprintf(tempstring1, sizeof(tempstring1), "%s", tempstring2);
	};
	snprintf(resultstring, NI_MAXHOST, "%s", tempstring1);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;
	return (retval);
};
#undef DEBUG_function_name


#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_uncompaddr"
int libipv6addr_ipv6addrstruct_to_uncompaddr(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	
	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: get format option: %08x\n", DEBUG_function_name, (unsigned int) formatoptions);
	};

	if ( (formatoptions & FORMATOPTION_printprefix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrprefix(ipv6addrp, resultstring, formatoptions);
	} else if ( (formatoptions & FORMATOPTION_printsuffix) != 0 ) {
		retval = ipv6addrstruct_to_uncompaddrsuffix(ipv6addrp, resultstring, formatoptions);		
	} else {
		retval = ipv6addrstruct_to_uncompaddr(ipv6addrp, resultstring, formatoptions);		
	};

	if (retval == 0) {
		/* don't modify case on error messages */
		if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
			/* nothing to do */
		} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
			string_to_upcase(resultstring);
		};
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string: %s\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name


/*
 * mask prefix bits (set suffix bits to 0)
 * 
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_masksuffix"
void ipv6addrstruct_maskprefix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 127; i >= 0; i--) {
		nbit = (unsigned int) i;
		if (nbit >= (unsigned int) ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = ((uint16_t) 0x8000u) >> (( ((uint16_t) nbit) & ((uint16_t) 0x0fu)));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
				fprintf(stderr, "%s: bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x\n", DEBUG_function_name, nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);
			};

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};
#undef DEBUG_function_name


/*
 * mask suffix bits (set prefix bits to 0) 
 *
 * in:  structure via reference
 * out: modified structure
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_masksuffix"
void ipv6addrstruct_masksuffix(ipv6calc_ipv6addr *ipv6addrp) {
	unsigned int nbit, nword;
	uint16_t mask, newword;
	int i;

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: called\n", DEBUG_function_name);
	};
   
	if (ipv6addrp->flag_prefixuse != 1) {
		/* hmm, no prefix specified. skip */
		return;
	};

	for (i = 127; i >= 0; i--) {
		nbit = (unsigned int) i;

		if (nbit < (unsigned int) ipv6addrp->prefixlength) {
			/* set bit to zero */
			
			/* calculate word (16 bit) - matches with addr6p[]*/
			nword = (nbit & 0x70) >> 4;
				 
			/* calculate mask */
			mask = ((uint32_t) 0x8000u) >> (((uint32_t) nbit) & ((uint32_t) 0x0fu ));
			newword = ipv6addr_getword(ipv6addrp, nword) & (~ mask );
			
			if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
				fprintf(stderr, "libipv6addr/ipv6calc_ipv6addr_masksuffix: bit: %u = nword: %u, mask: %04x, word: %04x newword: %04x\n", nbit, nword, (unsigned int) mask, (unsigned int) ipv6addr_getword(ipv6addrp, nword), (unsigned int) newword);
			};

			ipv6addr_setword(ipv6addrp, nword, (unsigned int) newword);
		};
	};
};
#undef DEBUG_function_name


/*
 * function stores an 16 char token into a structure
 *
 * in : *addrstring = 16 char token
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/tokenlsb64_to_ipv6addrstruct"
int tokenlsb64_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1, result;
	int temp[4];
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	if ( strlen(addrstring) != 16 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, given /token '%s' is not valid (length != 16!", addrstring);
		retval = 1;
		return (retval);
	};

	/* scan address into array */
	result = sscanf(addrstring, "%04x%04x%04x%04x", &temp[0], &temp[1], &temp[2], &temp[3]);
	if ( result != 4 ) {
		snprintf(resultstring, NI_MAXHOST, "Error splitting address '%s', got %d items instead of 4!", addrstring, result);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring),  "0:0:0:0:%04x:%04x:%04x:%04x", temp[0], temp[1], temp[2], temp[3]);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores an interface identifier into a structure
 *
 * in : *addrstring = interface identifier
 * out: *resultstring = error message
 * out: ipv6addr = IPv6 address structure
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/identifier_to_ipv6addrstruct"
int identifier_to_ipv6addrstruct(const char *addrstring, char *resultstring, ipv6calc_ipv6addr *ipv6addrp) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	resultstring[0] = '\0'; /* clear result string */

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: Got input '%s'\n", DEBUG_function_name, addrstring);
	};
	
	if ( strlen(addrstring) > 19 ) {
		snprintf(resultstring, NI_MAXHOST, "Error, given identifier identifier '%s' is too long (length > 16!", addrstring);
		retval = 1;
		return (retval);
	};

	/* set prefix */
	snprintf(tempstring, sizeof(tempstring), "0:0:0:0:%s", addrstring);

	/* store into structure */
	retval = addr_to_ipv6addrstruct(tempstring, resultstring, ipv6addrp);

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string '%s'\n", DEBUG_function_name, resultstring);
	};

	return (retval);
};
#undef DEBUG_function_name

/*
 * function stores the ipv6addr structure in an uncompressed IPv6 format string
 *
 * in:  ipv6addr = IPv6 address structure
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_tokenlsb64"
int libipv6addr_ipv6addrstruct_to_tokenlsb64(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, uint32_t formatoptions) {
	int retval = 1;
	
	/* print array */
	snprintf(resultstring, NI_MAXHOST, "%04x%04x%04x%04x", ipv6addr_getword(ipv6addrp, 4), ipv6addr_getword(ipv6addrp, 5), ipv6addr_getword(ipv6addrp, 6), ipv6addr_getword(ipv6addrp, 7));

	if ( (formatoptions & FORMATOPTION_printlowercase) != 0 ) {
		/* nothing to do */
	} else if ( (formatoptions & FORMATOPTION_printuppercase) != 0 ) {
		string_to_upcase(resultstring);
	};

	if ( (ipv6calc_debug & DEBUG_libipv6addr) != 0 ) {
		fprintf(stderr, "%s: result string '%s'\n", DEBUG_function_name, resultstring);
	};
	
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name

/*
 * function prints an IPv6 address in native octal format
 *
 * in:  ipv6addr = IPv6 address structure
 * formatoptions
 * out: *resultstring = IPv6 address (modified)
 * ret: ==0: ok, !=0: error
 */
#define DEBUG_function_name "libipv6addr/ipv6addrstruct_to_octal"
int libipv6addr_to_octal(const ipv6calc_ipv6addr *ipv6addrp, char *resultstring, const uint32_t formatoptions) {
	int retval = 1;
	char tempstring[NI_MAXHOST];

	snprintf(tempstring, sizeof(tempstring), "\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o\\%o", ipv6addrp->in6_addr.s6_addr[0], ipv6addrp->in6_addr.s6_addr[1], ipv6addrp->in6_addr.s6_addr[2], ipv6addrp->in6_addr.s6_addr[3], ipv6addrp->in6_addr.s6_addr[4], ipv6addrp->in6_addr.s6_addr[5], ipv6addrp->in6_addr.s6_addr[6], ipv6addrp->in6_addr.s6_addr[7], ipv6addrp->in6_addr.s6_addr[8], ipv6addrp->in6_addr.s6_addr[9], ipv6addrp->in6_addr.s6_addr[10], ipv6addrp->in6_addr.s6_addr[11], ipv6addrp->in6_addr.s6_addr[12], ipv6addrp->in6_addr.s6_addr[13], ipv6addrp->in6_addr.s6_addr[14], ipv6addrp->in6_addr.s6_addr[15]);

	snprintf(resultstring, NI_MAXHOST, "%s", tempstring);
	retval = 0;	
	return (retval);
};
#undef DEBUG_function_name
