/*
 * Project    : ipv6calc
 * File       : ipv6calc.h
 * Version    : $Id$
 * Copyright  : 2001-2016 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Main header file
 */ 


#ifndef _ipv6calc_h_

#define _ipv6calc_h_

/* global program related definitions */
#define PROGRAM_NAME "ipv6calc"
#define PROGRAM_COPYRIGHT "(P) & (C) 2001-" COPYRIGHT_YEAR " by Peter Bieringer <pb (at) bieringer.de>"

//#define ANONPRESET_DEFAULT "keep-type-asn-cc"
#define ANONPRESET_DEFAULT "anonymize-standard"

#define DEBUG_ipv6calc_general      0x00000001l

#endif

extern int feature_zeroize;
extern int feature_anon;
extern int feature_kp;

extern char showinfo_machine_readable_filter[];
extern int showinfo_machine_readable_filter_used;
