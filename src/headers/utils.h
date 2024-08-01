#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef __UTILS_H
#define __UTILS_H

#define HEC_POLY 0x07
#define HEC_INIT 0x00
#define HEC_XOROUT 0x55

#define ECC_POLY 0x07
#define ECC_INIT 0x00
#define ECC_XOROUT 0x00

#define PDF_MAIN_STREAM_ATTRIBUTE_PACKET 3
#define PDF_BLANK_START_PACKET 2
#define PDF_SECONDARY_DATA_PACKET 4
#define PDF_VIDEO_DATA_PACKET 1

#define HOPID_DEFAULT 0
#define RESERVED_DEFAULT 0
#define SUPP_ID_DEFAULT 9

uint8_t calculateHEC(uint32_t);
uint8_t calculateECC(uint32_t);

void bytesToHexString(const unsigned char*, int, FILE*, int);

void fillPayload(unsigned char*, int);

enum PAYLOAD_TYPE {
    ZERO,
    VBID,
    MVID_23_16,     MVID_15_8,      MVID_7_0,
    MAUD_23_16,     MAUD_15_8,      MAUD_7_0, 
    NVID_23_16,     NVID_15_8,      NVID_7_0,
    HTOTAL_15_8,    HTOTAL_7_0,
    HSTART_15_8,    HSTART_7_0,
    HWIDTH_15_8,    HWIDTH_7_0,
    VTOTAL_15_8,    VTOTAL_7_0,
    VSTART_15_8,    VSTART_7_0,
    VHEIGHT_15_8,   VHEIGHT_7_0,
    MISC0_7_0,      MISC1_7_0,
    HSP_HSW_14_8,   HSW_7_0,
    VSP_VSW_14_8,   VSW_7_0,
};

#endif