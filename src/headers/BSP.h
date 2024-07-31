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

#define PDF_BLANK_START_PACKET 0x2
#define SUPPID_TUNNELED_PACKET 0x0  
#define RESERVED_VALUE 0x0
#define HOPID 0x01 // This is a default value, can be modified based on the context
#define LENGTH_HEADER 0x0C // Length of Blank Start Packet Header is 12 bytes (0x0C)

// enum PAYLOAD_TYPE BSP_PAYLOAD_FORMAT_LANE_1[] = {VBID, VBID, VBID, VBID, MVID, MVID, MVID, MVID, MAUD, MAUD, MAUD, MAUD};
// enum PAYLOAD_TYPE BSP_PAYLOAD_FORMAT_LANE_2[] = {VBID, VBID, MVID, MVID, MAUD, MAUD, VBID, VBID, MVID, MVID, MAUD, MAUD};
// enum PAYLOAD_TYPE BSP_PAYLOAD_FORMAT_LANE_4[] = {VBID, MVID, MAUD, VBID, MVID, MAUD, VBID, MVID, MAUD, VBID, MVID, MAUD};

void get_BSP_payload_format(enum PAYLOAD_TYPE[], int);

void generatePayload(uint8_t *, size_t);
uint32_t generate_tunneled_BS_packet_header();
uint32_t generate_BS_packet_header(uint8_t, uint8_t, uint32_t);
void generate_BS_packet(uint32_t, uint32_t, FILE*);
void BSP_GEN(const char*, const char*, const char*, FILE*);

# endif