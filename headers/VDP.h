# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __VDP_H
# define __VDP_H

# include <stdint.h>

#define PDF_VIDEO_DATA 1
#define SUPP_ID 0
#define RESERVED 0
#define HEC_INIT 0x00
#define HEC_XOR_OUT 0x55
#define ECC_INIT 0x00

void hexStringToBytes(const char*, unsigned char*);
void bytesToHexString(const unsigned char*, int, char*, FILE*, int);
int extractVideoCount(const unsigned char*);
int calculateTotalVideoDataLength(int, const unsigned char**);
void fillPayload(unsigned char*, int);
uint8_t calculate_HEC(uint32_t);
uint8_t calculate_ECC(uint32_t);
uint32_t generate_TU_set_Header(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t *generate_TU_set_Headers(int, char*[]);
uint32_t generate_Tunneled_VDP_Header(uint32_t, uint8_t);
void generate_Tunneled_VD_Packet(uint32_t, uint32_t*, size_t, FILE*);
int VDP_GEN(int, char*[]);


# endif