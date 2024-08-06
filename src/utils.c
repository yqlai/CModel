# include "headers/utils.h"

uint8_t calculateHEC(uint32_t header){
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = (header >> (24 - i * 8)) & 0xFF;
    }

    uint8_t crc = (uint8_t)HEC_INIT;
    for (int i = 0; i < 3; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ HEC_POLY;
            else
                crc <<= 1;
        }
    }
    crc ^= HEC_XOROUT;
    return crc;
}

uint8_t calculateECC(uint32_t header) {
    uint8_t data[4];
    for (int i = 0; i < 4; i++) {
        data[i] = (header >> (24 - i * 8)) & 0xFF;
    }

    uint8_t crc = ECC_INIT;
    for (int i = 0; i < 3; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ ECC_POLY;
            else
                crc <<= 1;
        }
    }
    crc ^= ECC_XOROUT; // XOROUT value is defined as 0x00 for the ECC calculation.
    return crc;
}

// Utility function to convert byte array back to hex string
void bytesToHexString(const unsigned char* byteArray, int length, FILE* file, int isHeader) {
    static int ind_SDP = 0;
    for (int i = 0; i < length; i++) {
        if(file)
        {
            if (ind_SDP % 4 == 0)
            {
                fprintf(file, "  %02X", 1);
                fprintf(file, "  %02X", isHeader);
            }

            fprintf(file, "  %02X", byteArray[i]);
            ind_SDP++;

            if (ind_SDP % 4 == 0)
                fprintf(file, "\n");
        }
    }
    if (isHeader == 1) ind_SDP = 0;
}

// Fill the payload with incremental values (00, 01, 02, etc.)
void fillPayload(unsigned char* payload, int length) {
    for (int i = 0; i < length; i++) {
        payload[i] = i & 0xFF;
    }
}

void print_value_of_PAYLOAD_TYPE()
{
    printf("ZERO: %d\t", ZERO);
    printf("VBID: %d\t", VBID);
    printf("MVID_23_16: %d\t", MVID_23_16);
    printf("MVID_15_8: %d\t", MVID_15_8);
    printf("MVID_7_0: %d\n", MVID_7_0);
    printf("MAUD_23_16: %d\t", MAUD_23_16);
    printf("MAUD_15_8: %d\t", MAUD_15_8);
    printf("MAUD_7_0: %d\t", MAUD_7_0);
    printf("NVID_23_16: %d\t", NVID_23_16);
    printf("NVID_15_8: %d\n", NVID_15_8);
    printf("NVID_7_0: %d\t", NVID_7_0);
    printf("HTOTAL_15_8: %d\t", HTOTAL_15_8);
    printf("HTOTAL_7_0: %d\t", HTOTAL_7_0);
    printf("HSTART_15_8: %d\t", HSTART_15_8);
    printf("HSTART_7_0: %d\t", HSTART_7_0);
    printf("HWIDTH_15_8: %d\n", HWIDTH_15_8);
    printf("HWIDTH_7_0: %d\t", HWIDTH_7_0);
    printf("VTOTAL_15_8: %d\t", VTOTAL_15_8);
    printf("VTOTAL_7_0: %d\t", VTOTAL_7_0);
    printf("VSTART_15_8: %d\t", VSTART_15_8);
    printf("VSTART_7_0: %d\n", VSTART_7_0);
    printf("VHEIGHT_15_8: %d\t", VHEIGHT_15_8);
    printf("VHEIGHT_7_0: %d\t", VHEIGHT_7_0);
    printf("MISC0_7_0: %d\t", MISC0_7_0);
    printf("MISC1_7_0: %d\t", MISC1_7_0);
    printf("HSP_HSW_14_8: %d\n", HSP_HSW_14_8);
    printf("HSW_7_0: %d\t", HSW_7_0);
    printf("VSP_VSW_14_8: %d\t", VSP_VSW_14_8);
    printf("VSW_7_0: %d\n", VSW_7_0);
}