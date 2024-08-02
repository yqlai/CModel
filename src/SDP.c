#include "headers/SDP.h"
#include "headers/utils.h"

/**
 * Extracts the secondary count from the TU set header.
 *
 * @param tuSetHeader The TU set header from which to extract the secondary count.
 * @return The extracted secondary count.
 */
int extractSecondaryCount(const unsigned char* tuSetHeader) {
    int secondaryCount = (tuSetHeader[2] & 0x3F);  // Extract bits [13:8]
    if (secondaryCount == 0) secondaryCount = 64;
    return secondaryCount;
}


/**
 * Calculates the total length of secondary video data.
 *
 * @param totalTuSets The total number of TU sets.
 * @param tuSetHeaders An array of TU set headers.
 * @return The total length of secondary video data.
 */
int calculateTotalSecondaryDataLength(int totalTuSets, const unsigned char** tuSetHeaders) {
    int totalSecondaryDataLength = 0;
    for (int i = 0; i < totalTuSets; i++) {
        totalSecondaryDataLength += extractSecondaryCount(tuSetHeaders[i]);
    }
    return totalSecondaryDataLength;
}


/**
 * Generates the TU set Header based on the given parameters.
 *
 * @param EFC_ND The EFC_ND value.
 * @param NSS The NSS value.
 * @param NSE The NSE value.
 * @param L The L value.
 * @param Fill_Count The Fill_Count value.
 * @param Secondary_Count The Secondary_Count value.
 * @return The generated TU set Header.
 */
uint32_t generate_TU_set_Header(uint32_t EFC_ND, uint32_t NSS, uint32_t NSE, uint32_t L, uint32_t Fill_Count, uint32_t Secondary_Count)
{
    // Construct TU set Header
    uint32_t TU_set_header = ((Secondary_Count & 0x3F) << 8) |
                             ((Fill_Count & 0x3FFF) << 14) |
                             ((L & 0x1) << 28) |
                             ((NSE & 0x1) << 29) |
                             ((NSS & 0x1) << 30) |
                             ((EFC_ND & 0x1) << 31);
    TU_set_header |= calculateECC(TU_set_header);

    return TU_set_header;
}


/**
 * Generates an array of TU set headers based on the command line arguments.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @return A pointer to the array of TU set headers.
 */
uint32_t *generate_TU_set_Headers(int argc, char *argv[])
{
    size_t num_TU_sets = (argc - 1) / 6;
    uint32_t *TU_set_headers = (uint32_t *)malloc(num_TU_sets * sizeof(uint32_t));
    for(int i=0 ; i<num_TU_sets ; i++)
    {
        uint32_t EFC_ND = atoi(argv[1 + i*6]);
        uint32_t NSS = atoi(argv[2 + i*6]);
        uint32_t NSE = atoi(argv[3 + i*6]);
        uint32_t L = atoi(argv[4 + i*6]);
        uint32_t Fill_Count = atoi(argv[5 + i*6]);
        uint32_t Secondary_Count = atoi(argv[6 + i*6]);

        TU_set_headers[i] = generate_TU_set_Header(EFC_ND, NSS, NSE, L, Fill_Count, Secondary_Count);
    }
    return TU_set_headers;
}


/**
 * Generates a tunneled SDP header.
 *
 * This function takes the length of the SDP header as input and generates a tunneled SDP header
 * by combining various fields such as HEC, Length, HopID, and other default values.
 *
 * @param Length The length of the SDP header.
 * @return The generated tunneled SDP header.
 */
uint32_t generate_Tunneled_SDP_Header(uint32_t Length)
{
    uint8_t HopID = HOPID_DEFAULT;
    uint32_t USB4_header = (HEC_INIT) |
                           (Length << 8) |
                           (HopID << 16) |
                           (RESERVED_DEFAULT << 23) |
                           (SUPP_ID_DEFAULT << 27) |
                           (PDF_SECONDARY_DATA_PACKET << 28);
    uint8_t HEC = calculateHEC(USB4_header);
    USB4_header |= HEC;
    return USB4_header;
}


/**
 * Generates a tunneled SD packet.
 *
 * @param USB4_header The USB4 header value.
 * @param TU_set_headers An array of TU set headers.
 * @param num_TU_sets The number of TU sets.
 * @param file The file to write the packet to.
 */
void generate_Tunneled_SD_Packet(uint32_t USB4_header, uint32_t *TU_set_headers, size_t num_TU_sets, FILE *file)
{
    uint8_t tunHeader[4];
    uint8_t **tuSetHeaders;
    uint8_t **payload;
    uint32_t *payloadLengths;
    
    // Extract the bytes from the USB4 header
    tunHeader[0] = (USB4_header >> 24) & 0xFF;
    tunHeader[1] = (USB4_header >> 16) & 0xFF;
    tunHeader[2] = (USB4_header >> 8) & 0xFF;
    tunHeader[3] = USB4_header & 0xFF;

    // Allocate memory for TU set headers
    tuSetHeaders = (uint8_t **)malloc(num_TU_sets * sizeof(uint8_t *));
    for(int i=0 ; i<num_TU_sets ; i++)
    {
        tuSetHeaders[i] = (uint8_t *)malloc(4 * sizeof(uint8_t));
        // Extract the bytes from each TU set header
        tuSetHeaders[i][0] = (TU_set_headers[i] >> 24) & 0xFF;
        tuSetHeaders[i][1] = (TU_set_headers[i] >> 16) & 0xFF;
        tuSetHeaders[i][2] = (TU_set_headers[i] >> 8) & 0xFF;
        tuSetHeaders[i][3] = TU_set_headers[i] & 0xFF;
    }

    // Calculate the total number of bytes for the payload
    int totalVideoDataLength = calculateTotalSecondaryDataLength(num_TU_sets, (const unsigned char**)tuSetHeaders);
    int totalPacketLength = 4 + (num_TU_sets * 4) + totalVideoDataLength;  // 4 bytes for Tunneled Packet Header + TU headers + Video data

    // Update the length field in the tunneled packet header
    tunHeader[2] = (totalPacketLength - 4) == 256 ? (totalPacketLength - 4) : 0;  // Exclude the Tunneled Packet Header length
    tunHeader[3] = calculateHEC((uint32_t)((uint32_t)(tunHeader[0] << 24) | (uint32_t)(tunHeader[1] << 16) | (uint32_t)(tunHeader[2] << 8) | (uint32_t)(0)));

    // Write the tunneled packet header to the file
    bytesToHexString(tunHeader, 4, file, 1);
    int bytes = 4;

    // Generate the payload for each TU set
    payload = (uint8_t **)malloc(num_TU_sets * sizeof(uint8_t *));
    payloadLengths = (uint32_t *)malloc(num_TU_sets * sizeof(uint32_t));
    for(int i=0 ; i<num_TU_sets ; i++)
    {
        // Print TU set header
        payloadLengths[i] = extractSecondaryCount(tuSetHeaders[i]);

        bytesToHexString(tuSetHeaders[i], 4, file, 0);
        bytes += 4;
        for(int j=0 ; j<payloadLengths[i] ; j++)
        {
            unsigned char dummy[1] = { (unsigned char)j };
            bytesToHexString(dummy, 1, file, 0);
            bytes += 1;
        }
        
        // Align the payload to 4 bytes
        for(int j=0 ; j<(((payloadLengths[i] + 3) & ~3) - payloadLengths[i]) ; j++)
        {
            unsigned char dummy[1] = { 0 };
            bytesToHexString(dummy, 1, file, 0);
            bytes += 1;
        }
        free(tuSetHeaders[i]);
    }

    free(tuSetHeaders);  
    free(payload);
    free(payloadLengths);

    // Print the final packet
}

/**
 * Generate a Video Data Packet
 * @param argc Number of arguments
 * @param argv Strings: SDP <EFC/ND_1> <NSS_1> <NSE_1> <L_1> <Fill_Count_1> <Secondary_Count_1> ... <EFC/ND_n> <NSS_n> <NSE_n> <L_n> <Fill_Count_n> <Secondary_Count_n>
 * @param file File to save the packet to
 * 
*/
int SDP_GEN(int argc, char *argv[], FILE* file)
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <HopID> <EFC/ND_1> <NSS_1> <NSE_1> <L_1> <Fill_Count_1> <Secondary_Count_1> ... <EFC/ND_n> <NSS_n> <NSE_n> <L_n> <Fill_Count_n> <Secondary_Count_n>\n", argv[0]);
        return 1;
    }

    size_t num_TU_sets = 0;
    if ((argc - 1) % 6 != 0)
    {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        fprintf(stderr, "Usage: %s <HopID> <EFC/ND_1> <NSS_1> <NSE_1> <L_1> <Fill_Count_1> <Secondary_Count_1> ... <EFC/ND_n> <NSS_n> <NSE_n> <L_n> <Fill_Count_n> <Secondary_Count_n>\n", argv[0]);
        return 1;
    }
    else num_TU_sets = (argc - 1) / 6;

    // ---------------------------------------------------
    // --------------- TU Set Header ---------------------
    // ---------------------------------------------------

    uint32_t *TU_set_headers = generate_TU_set_Headers(argc, argv);

    // ---------------------------------------------------
    // ----------- USB4 Tunneled Packet Header -----------
    // ---------------------------------------------------
    
    uint32_t Length = 0; // Calculated in the payload generation
    uint32_t USB4_header = generate_Tunneled_SDP_Header(Length);

    // ---------------------------------------------------
    // --------------- Generate Packet -------------------
    // ---------------------------------------------------
    generate_Tunneled_SD_Packet(USB4_header, TU_set_headers, num_TU_sets, file);

    return 0;
}