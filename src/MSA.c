# include "headers/MSA.h"
# include "headers/utils.h"

void printByteArrayToFile(unsigned char *byteArray, int byteArraySize, FILE* file, int isHeader) {
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

/**
 * Generates a tunneled MSA (Main Stream Attribute) packet header.
 * No parameters needed.
 *
 * @return The generated tunneled packet header.
 */
uint32_t generate_tunneled_MSA_packet_header()
{
    // Tunneled Packet Header Fields
    uint8_t pdf = 3;                     // Protocol Defined Field for Main Stream Attribute Packet
    uint8_t hopID = HOPID_DEFAULT;       // Hop ID: Default value
    uint8_t suppID = 0;                  // Supplemental ID: Shall be set to 0b
    uint8_t reserved = 0;                // Reserved: Fixed to 0
    uint8_t length = 0x28;               // Length: Fixed to 0x28 for a Main Stream Attribute Packet
    uint32_t tunneledPacketHeader = 0;   // Placeholder for Tunnled Packet Header

    // Set fields in the Tunneled Packet Header
    tunneledPacketHeader |= pdf << 28;
    tunneledPacketHeader |= suppID << 27;
    tunneledPacketHeader |= reserved << 23;
    tunneledPacketHeader |= hopID << 16;
    tunneledPacketHeader |= length << 8;

    // Calculate the HEC (Header Error Control)
    uint8_t hec = calculateECC(tunneledPacketHeader);
    tunneledPacketHeader |= hec;

    return tunneledPacketHeader;
}

uint32_t generate_MSA_packet_header(uint32_t fillCount)
{
    // Main Stream Attribute Packet Fields
    uint32_t msapHeader = 0;
    uint32_t reserved = 0;  // Reserved: Fixed to 0
    msapHeader |= reserved << 25;
    msapHeader |= fillCount << 8;

    // Calculate the ECC (Error Correction Code)
    uint8_t ecc = calculateECC(msapHeader);
    msapHeader |= ecc;

    return msapHeader;
}

void generate_MSA_packet(uint32_t tunneledPacketHeader, uint32_t msaHeader, FILE *file)
{
    const int TUNNELED_PACKET_HEADER_SIZE = 4;
    const int MSA_HEADER_SIZE = 4;
    const int MSA_PAYLOAD_SIZE = 36;
    const int TOTAL_PACKET_SIZE = TUNNELED_PACKET_HEADER_SIZE + MSA_HEADER_SIZE + MSA_PAYLOAD_SIZE;

    uint8_t tunneledPacketHeaderArr[4] = {0};
    uint8_t msaHeaderArr[4] = {0};
    uint8_t payload[36] = {0};

    tunneledPacketHeaderArr[0] = (tunneledPacketHeader >> 24) & 0xFF;
    tunneledPacketHeaderArr[1] = (tunneledPacketHeader >> 16) & 0xFF;
    tunneledPacketHeaderArr[2] = (tunneledPacketHeader >> 8) & 0xFF;
    tunneledPacketHeaderArr[3] = tunneledPacketHeader & 0xFF;

    msaHeaderArr[0] = (msaHeader >> 24) & 0xFF;
    msaHeaderArr[1] = (msaHeader >> 16) & 0xFF;
    msaHeaderArr[2] = (msaHeader >> 8) & 0xFF;
    msaHeaderArr[3] = msaHeader & 0xFF;

    for (int i = 0; i < MSA_PAYLOAD_SIZE; i++) {
        payload[i] = i;
    }
    
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    bytesToHexString(tunneledPacketHeaderArr, TUNNELED_PACKET_HEADER_SIZE, file, 1);
    bytesToHexString(msaHeaderArr, MSA_HEADER_SIZE, file, 0);
    bytesToHexString(payload, MSA_PAYLOAD_SIZE, file, 0);
}

/**
 * Generates an MSA packet with the given fill count and hop ID, and saves it to a file.
 *
 * @param fillCountString The fill count as a string.
 * @param file The file to save the MSA packet to.
 */
void MSAP_GEN(const char* fillCountString, FILE* file)
{
    uint32_t fillCount = (uint32_t)strtol(fillCountString, NULL, 10);
    
    uint32_t Tunneled_MSA_Packet_Header = generate_tunneled_MSA_packet_header();
    uint32_t MSA_Packet_Header = generate_MSA_packet_header(fillCount);
    
    generate_MSA_packet(Tunneled_MSA_Packet_Header, MSA_Packet_Header, file);
}

// int main()
// {
//     MSAP_GEN("100", file);
//     return 0;
// }