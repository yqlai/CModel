# include "headers/BSP.h"
# include "headers/utils.h"

void printByteArrayToFile_BSP(unsigned char *byteArray, int byteArraySize, FILE* file, int isHeader) {
    static int ind_MSA = 0;
    for (int i = 0; i < byteArraySize; i++) {
        if(ind_MSA % 4 == 0)
        {
            fprintf(file, "  %02X", 1);
            fprintf(file, "  %02X", isHeader);
        }

        fprintf(file, "  %02x", byteArray[i]);
        ind_MSA++;

        if(ind_MSA % 4 == 0)
            fprintf(file, "\n");
    }
} 

// Generate a payload sequentially starting from 00
void generatePayload(uint8_t *payload, size_t payloadLength) {
    for (size_t i = 0; i < payloadLength; i++) {
        payload[i] = i & 0xFF; // Keep incrementing from 00
    }
}

uint32_t generate_tunneled_BS_packet_header() {
    uint32_t tunneledPacketHeader = 0;
    tunneledPacketHeader |= (LENGTH_HEADER & 0xFF) << 8;
    tunneledPacketHeader |= (HOPID & 0x7F) << 16;
    tunneledPacketHeader |= (RESERVED_VALUE & 0xF) << 23;
    tunneledPacketHeader |= (SUPPID_TUNNELED_PACKET & 0x1) << 27;
    tunneledPacketHeader |= (PDF_BLANK_START_PACKET & 0xF) << 28;
    tunneledPacketHeader |= (calculateHEC(tunneledPacketHeader) & 0xFF);

    return tunneledPacketHeader;
}

uint32_t generate_BS_packet_header(uint8_t SR, uint8_t CP, uint32_t FillCount)
{
    uint32_t BlankStartHeader = 0;
    BlankStartHeader |= (CP & 0x1) << 30;
    BlankStartHeader |= (SR & 0x1) << 31;
    BlankStartHeader |= (FillCount & 0xFF) << 8;

    // Calculate ECC for Blank Start Header
    uint8_t BlankStartECC = calculateECC(BlankStartHeader);
    BlankStartHeader |= (BlankStartECC & 0xFF);

    return BlankStartHeader;
}

void generate_BS_packet(uint32_t tunHeader, uint32_t blankStartHeader, FILE *file)
{
    uint32_t tunHeader_t;
    uint32_t blankStartHeader_t;

    // Allocate memory for headers and payload
    uint8_t tunHeaderArr[4];
    uint8_t blankStartHeaderArr[4];
    size_t payloadLength = 12;
    uint8_t payload[payloadLength];

    tunHeaderArr[0] = (tunHeader >> 24) & 0xFF;
    tunHeaderArr[1] = (tunHeader >> 16) & 0xFF;
    tunHeaderArr[2] = (tunHeader >> 8) & 0xFF;
    tunHeaderArr[3] = tunHeader & 0xFF;

    blankStartHeaderArr[0] = (blankStartHeader >> 24) & 0xFF;
    blankStartHeaderArr[1] = (blankStartHeader >> 16) & 0xFF;
    blankStartHeaderArr[2] = (blankStartHeader >> 8) & 0xFF;
    blankStartHeaderArr[3] = blankStartHeader & 0xFF;
    
    // Generate the payload sequentially
    generatePayload(payload, payloadLength);


    if(file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    // Output the Tunneled Packet
    // for (size_t i = 0; i < 4; i++) {
    //     fprintf(file, "%02X ", tunHeaderArr[i]);
    // }
    // for (size_t i = 0; i < 4; i++) {
    //     fprintf(file, "%02X ", blankStartHeaderArr[i]);
    // }
    // for (size_t i = 0; i < payloadLength; i++) {
    //     fprintf(file, "%02X ", payload[i]);
    // }

    printByteArrayToFile_BSP(tunHeaderArr, 4, file, 1);
    printByteArrayToFile_BSP(blankStartHeaderArr, 4, file, 0);
    printByteArrayToFile_BSP(payload, payloadLength, file, 0);

    fclose(file);
}

/**
 * 
 * Generate a Blank Start Packet
 * @param SRString SR as a string
 * @param CPString CP as a string
 * @param FillCountString Fill Count as a string
 * @param file File to save the packet to
 * 
*/
void BSP_GEN(const char* SRString, const char* CPString, const char* FillCountString, FILE* file)
{
    uint8_t SR = (uint8_t) strtol(SRString, NULL, 10);
    uint8_t CP = (uint8_t) strtol(CPString, NULL, 10);
    uint32_t FillCount = (uint32_t) strtol(FillCountString, NULL, 10);

    uint32_t tunHeader = generate_tunneled_BS_packet_header();
    uint32_t blankStartHeader = generate_BS_packet_header(SR, CP, FillCount);

    generate_BS_packet(tunHeader, blankStartHeader, file);
}

// int main()
// {
//     BSP_GEN("1", "0", "100", "results/BSP.txt");
//     return 0;
// }