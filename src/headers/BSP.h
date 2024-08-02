# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __BSP_H
# define __BSP_H

# include "utils.h"

#define LENGTH_HEADER 0x10

void get_BSP_payload_format(enum PAYLOAD_TYPE[], int);
void generate_BSP_Payload(uint8_t *, size_t, int);

uint32_t generate_tunneled_BS_packet_header();
uint32_t generate_BS_packet_header(uint8_t, uint8_t, uint32_t);
void generate_BS_packet(uint32_t, uint32_t, int, FILE*);
void BSP_GEN(const char*, const char*, const char*, const char*, FILE*);


# endif