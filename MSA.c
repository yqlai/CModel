# include "headers/MSA.h"

// Function to calculate HEC (Header Error Control)
uint8_t calculate_hec(uint32_t header) {
    uint8_t hec = 0x00;
    uint8_t poly = 0x07; 
    for (int bit = 31; bit >= 8; bit--) {
        hec ^= ((header >> bit) & 1) << 7;
        for (int i = 7; i >= 1; i--) {
            hec = (hec >> 1) ^ ((hec & 0x01) ? poly : 0x00);
        }
    }
    hec ^= 0x55;  // XorOut: 0x55
    return hec;
}

// Function to calculate ECC (Error Correction Code)
uint8_t calculate_ecc(uint32_t header) {
    uint8_t ecc = 0x00;
    uint8_t poly = 0x07;
    for (int bit = 31; bit >= 8; bit--) {
        ecc ^= ((header >> bit) & 1) << 7;
        for (int i = 7; i >= 1; i--) {
            ecc = (ecc >> 1) ^ ((ecc & 0x01) ? poly : 0x00);
        }
    }
    return ecc;
}

void hexStringToByteArray_MSA(const char *hexString, unsigned char *byteArray, int byteArraySize) {
    int hexStringLength = strlen(hexString);
    int i;
    for (i = 0; i < hexStringLength / 2 && i < byteArraySize; i++) {
        sscanf(hexString + 2*i, "%2hhx", &byteArray[i]);
    }
}

void printByteArrayToFile(unsigned char *byteArray, int byteArraySize, FILE* file, int isHeader) {
    static int ind_MSA = 0;
    for (int i = 0; i < byteArraySize; i++) {
        if(ind_MSA % 4 == 0)
        {
            fprintf(file, "  %02X", 1);
            fprintf(file, "  %02X", isHeader);
        }

        fprintf(file, "  %02x ", byteArray[i]);
        ind_MSA++;

        if(ind_MSA % 4 == 0)
            fprintf(file, "\n");
    }
}

/**
 * Generates a tunneled MSA (Main Stream Attribute) packet header.
 *
 * @param hopID The hop ID to be set in the tunneled packet header.
 * @return The generated tunneled packet header.
 */
uint32_t generate_tunneled_MSA_packet_header(uint8_t hopID)
{
    // Tunneled Packet Header Fields
    uint8_t pdf = 3;                     // Protocol Defined Field for Main Stream Attribute Packet
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
    uint8_t hec = calculate_hec(tunneledPacketHeader);
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
    uint8_t ecc = calculate_ecc(msapHeader);
    msapHeader |= ecc;

    return msapHeader;
}

void generate_MSA_packet(uint32_t tunneledPacketHeader, uint32_t msaHeader, const char* filename)
{
    const int TUNNELED_PACKET_HEADER_SIZE = 4;
    const int MSA_HEADER_SIZE = 4;
    const int MSA_PAYLOAD_SIZE = 36;
    const int TOTAL_PACKET_SIZE = TUNNELED_PACKET_HEADER_SIZE + MSA_HEADER_SIZE + MSA_PAYLOAD_SIZE;

    uint8_t tunneledPacketHeaderArr[4] = {0};
    uint8_t msaHeaderArr[4] = {0};
    uint8_t payload[36] = {0};

    printf("Tunneled Packet Header [0]: %02x\n", ((tunneledPacketHeader >> 24) & 0xff));
    printf("Tunneled Packet Header [1]: %02x\n", ((tunneledPacketHeader >> 16) & 0xff));
    printf("Tunneled Packet Header [2]: %02x\n", ((tunneledPacketHeader >> 8) & 0xff));
    printf("Tunneled Packet Header [3]: %02x\n", (tunneledPacketHeader & 0xff));

    printf("MSA Header [0]: %02x\n", ((msaHeader >> 24) & 0xff));
    printf("MSA Header [1]: %02x\n", ((msaHeader >> 16) & 0xff));
    printf("MSA Header [2]: %02x\n", ((msaHeader >> 8) & 0xff));
    printf("MSA Header [3]: %02x\n", (msaHeader & 0xff));

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
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    printByteArrayToFile(tunneledPacketHeaderArr, TUNNELED_PACKET_HEADER_SIZE, file, 1);
    printByteArrayToFile(msaHeaderArr, MSA_HEADER_SIZE, file, 1);
    printByteArrayToFile(payload, MSA_PAYLOAD_SIZE, file, 0);
    fclose(file);
}

/**
 * Generates an MSA packet with the given fill count and hop ID, and saves it to a file.
 *
 * @param fillCountString The fill count as a string.
 * @param hopIDString The hop ID as a string.
 * @param filename The name of the file to save the generated packet.
 */
void MSAP_GEN(const char* fillCountString, const char* hopIDString, const char* filename)
{
    uint32_t fillCount = (uint32_t)strtol(fillCountString, NULL, 10);
    uint32_t hopID = (uint32_t)strtol(hopIDString, NULL, 10);
    
    uint32_t Tunneled_MSA_Packet_Header = generate_tunneled_MSA_packet_header(hopID);
    uint32_t MSA_Packet_Header = generate_MSA_packet_header(fillCount);
    
    generate_MSA_packet(Tunneled_MSA_Packet_Header, MSA_Packet_Header, filename);
}

int main()
{
    MSAP_GEN("100", "3", "results/MSA.txt");
    return 0;
}