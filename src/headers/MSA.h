# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __MSA_H
# define __MSA_H

# include "utils.h"

void get_MSA_payload_format(enum PAYLOAD_TYPE[], int);

uint32_t generate_tunneled_MSA_packet_header();
uint32_t generate_MSA_packet_header(uint32_t);
void generate_MSA_packet(uint32_t, uint32_t, FILE*);

void MSAP_GEN(const char*, FILE *file);

# endif