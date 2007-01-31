/*
 * Project    : ipv6calc
 * File       : ipv6loganon.h
 * Version    : $Id: ipv6loganon.h,v 1.2 2007/01/31 16:27:32 peter Exp $
 * Copyright  : 2007 by Peter Bieringer <pb (at) bieringer.de>
 * License    : GNU GPL v2
 *
 * Information:
 *  Main header file
 */ 

/* global program related definitions */
#define PROGRAM_NAME "ipv6loganon"
#define PROGRAM_COPYRIGHT "(P) & (C) 2007 by Peter Bieringer <pb (at) bieringer.de>"

/* LRU cache maximum size */
#define CACHE_LRU_SIZE 200

/* prototyping */
extern int cache_lru_limit;

extern int mask_ipv4;
extern int mask_iid;
