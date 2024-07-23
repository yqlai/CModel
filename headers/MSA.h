# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __MSA_H
# define __MSA_H

uint8_t calculate_hec(uint32_t);
uint8_t calculate_ecc(uint32_t);
void hexStringToByteArray(const char *, unsigned char *, int);
void printByteArrayToFile(unsigned char *, int, FILE* ); 


uint32_t generate_tunneled_MSA_packet_header(uint8_t);
uint32_t generate_MSA_packet_header(uint32_t);
void generate_MSA_packet(uint32_t, uint32_t, const char*);

void MSAP_GEN(const char*, const char*, const char*);

# endif