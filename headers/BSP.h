# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __BSP_H
# define __BSP_H

#define PDF_BLANK_START_PACKET 0x2
#define SUPPID_TUNNELED_PACKET 0x0
#define RESERVED_VALUE 0x0
#define HOPID 0x01 // This is a default value, can be modified based on the context
#define LENGTH_HEADER 0x0C // Length of Blank Start Packet Header is 12 bytes (0x0C)
#define ECC_POLY 0x07
#define ECC_XOROUT 0x55

uint8_t calculateHEC(uint32_t);
uint8_t calculateECC(uint32_t);

void hexStringToByteArray(const char *, uint8_t *, size_t);
void printByteArrayToFile_BSP(unsigned char *, int, FILE*, int);

void generatePayload(uint8_t *, size_t);
uint32_t generate_tunneled_BS_packet_header();
uint32_t generate_BS_packet_header(uint8_t, uint8_t, uint32_t);
void generate_BS_packet(uint32_t, uint32_t, const char*);
void BSP_GEN(const char*, const char*, const char*, const char*);

# endif