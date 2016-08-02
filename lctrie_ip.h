#ifndef __LC_TRIE_IP_H__
#define __LC_TRIE_IP_H__
// begin #ifndef guard

#include <stdlib.h>
#include <stdint.h>

#define IP_SUBNET_UNUSED      0
#define IP_SUBNET_BGP         1
#define IP_SUBNET_PRIVATE     2
#define IP_SUBNET_LINKLOCAL   3
#define IP_SUBNET_MULTICAST   4
#define IP_SUBNET_BROADCAST   5
#define IP_SUBNET_LOOPBACK    6
#define IP_SUBNET_RESERVED    7
#define IP_SUBNET_BOGON       8
#define IP_SUBNET_USER        9

#define LCT_IP_DEBUG_PREFIXES 0

// Bit manipulation macros
//
// Extract num bits from 32 bit string starting at pos bit
// requires bit strings to be in host endian byte order
// for the bit manipulation to work properly
#define EXTRACT(pos, num, str) ((str)<<(pos)>>(32-(num)))

// link local, multicast, loopback, and reserved have no additional
// information and thereforce only have a type

// subnet bgp has a 32-bit AS number
typedef struct lct_subnet_bgp_t {
  uint32_t type;
  uint32_t asn;
} lct_subnet_bgp_t;

// RFC1918 private IP subnets have a
typedef struct lct_subnet_private_t {
  uint32_t type;
  char class;
} lct_subnet_private_t;

// RFC5735 reserved IP subnets
typedef struct lct_subnet_reserved_t {
  uint32_t type;
  const char *desc;
} lct_subnet_reserved_t;

// User customized IP subnets (host groupings)
typedef struct lct_subnet_usr_t {
  uint32_t type;
  void *data;
} lct_subnet_usr_t;

// union representing all possible subnet info types
typedef union lct_subnet_info {
  uint32_t type;
  lct_subnet_bgp_t bgp;
  lct_subnet_private_t priv;
  lct_subnet_reserved_t rsv;
  lct_subnet_usr_t usr;
} lct_subnet_info_t;

// subnet types
#define IP_BASE         0
#define IP_PREFIX       1
#define IP_PREFIX_FULL  2 // prefix full exhausted by its subprefixes

// nil prefix index canary
#define IP_PREFIX_NIL   UINT32_MAX

// the actual IP subnet structure
typedef struct lct_subnet {
  uint8_t type;         // prefix type
  uint8_t len;          // CIDR address prefix length
  uint32_t addr;        // subnet address

  // index to our next highest prefix, .  this limits us to about 4 billion entries,
  // and we're not going to get close because the number of subnets is always going
  // to be less than the number of theoretical IP addresses.
  //
  // this also means that the structure cannot be
  uint32_t prefix;

  lct_subnet_info_t info;
} lct_subnet_t;

typedef struct lct_ip_stats {
  uint32_t size;  // size of the subnet
  uint32_t used;  // size of the subprefixed address space
} lct_ip_stats_t;

// fill in user array with reserved IP subnets
// according to RFC 5735
extern int init_reserved_subnets(lct_subnet_t *subnets, size_t size);

// three-way subnet comparison for qsort
extern int subnet_cmp(const void *di, const void *dj);

// apply netmasks to entries, should be done prior to sorting
// the array
extern void subnet_mask(lct_subnet_t *subnets, size_t size);

// de-duplicates subnets, should be run after applying netmasks
// and sorting the array.
// returns the number of duplicates removed
extern size_t subnet_dedup(lct_subnet_t *subnets, size_t size);

// calculates subnets that are prefixes of other subnets
// and returns the number found
extern size_t subnet_prefix(lct_subnet_t *subnets, lct_ip_stats_t *stats, size_t size);

// is subnet s a prefix of the subnet t?
// requires the two elements to be sorted and in order according
// to subnet_cmp
extern int subnet_isprefix(lct_subnet_t *s, lct_subnet_t *t);

// end #ifndef guard
#endif
