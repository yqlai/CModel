#include "headers/VDP.h"
#include "headers/utils.h"


/**
 * Extracts the video count from the given TU set header.
 *
 * @param tuSetHeader The TU set header containing the video count.
 * @return The extracted video count.
 */
int extractVideoCount(const unsigned char* tuSetHeader) {
    int videoCount = (tuSetHeader[2] & 0x3F);  // Extract bits [13:8]
    if (videoCount == 0) videoCount = 64;
    return videoCount;
}


/**
 * Calculates the total video data length by parsing the tuSetHeaders array.
 *
 * @param totalTuSets The total number of tuSets in the tuSetHeaders array.
 * @param tuSetHeaders An array of pointers to unsigned char arrays representing tuSet headers.
 * @return The total video data length calculated from the tuSetHeaders array.
 */
int calculateTotalVideoDataLength(int totalTuSets, const unsigned char** tuSetHeaders) {
    int totalVideoDataLength = 0;
    for (int i = 0; i < totalTuSets; i++) {
        totalVideoDataLength += extractVideoCount(tuSetHeaders[i]);
    }
    return totalVideoDataLength;
}


/**
 * Generates the TU set Header based on the provided parameters.
 *
 * @param EOC         End of Conversion flag (0 or 1)
 * @param TU_type     Type of TU (0, 1, or 2)
 * @param L           L value (0 or 1)
 * @param Fill_Count  Fill Count value (0 to 16383)
 * @param Video_Count Video Count value (0 to 63)
 *
 * @return The generated TU set Header.
 */
uint32_t generate_VDP_TU_set_Header(uint32_t EOC, uint32_t TU_type, uint32_t L, uint32_t Fill_Count, uint32_t Video_Count)
{
    // Construct TU set Header
    uint32_t TU_set_header = ((Video_Count & 0x3F) << 8) |
                             ((Fill_Count & 0x3FFF) << 14) |
                             ((L & 0x1) << 28) |
                             ((TU_type & 0x3) << 29) |
                             ((EOC & 0x1) << 31);
    TU_set_header |= calculateECC(TU_set_header);

    return TU_set_header;
}


/**
 * Generates an array of TU set headers based on the command line arguments.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of command line arguments.
 * @return A pointer to the array of TU set headers.
 */
uint32_t *generate_VDP_TU_set_Headers(int argc, char *argv[])
{
    size_t num_TU_sets = (argc - 1) / 5;
    uint32_t *TU_set_headers = (uint32_t *)malloc(num_TU_sets * sizeof(uint32_t));
    for(int i=0 ; i<num_TU_sets ; i++)
    {
        uint32_t EOC = atoi(argv[1 + i*5]);
        uint32_t TU_type = atoi(argv[2 + i*5]);
        uint32_t L = atoi(argv[3 + i*5]);
        uint32_t Fill_Count = atoi(argv[4 + i*5]);
        uint32_t Video_Count = atoi(argv[5 + i*5]);

        TU_set_headers[i] = generate_VDP_TU_set_Header(EOC, TU_type, L, Fill_Count, Video_Count);
    }
    return TU_set_headers;
}

/**
 * Generates a tunneled VDP (Video Data Packet) header.
 *
 * @param Length The length of the VDP packet.
 * @return The generated VDP header.
 */
uint32_t generate_Tunneled_VDP_Header(uint32_t Length)
{
    uint8_t HopID = HOPID_DEFAULT;
    uint32_t USB4_header = (Length << 8) |
                           (HopID << 16) |
                           (RESERVED_DEFAULT << 23) |
                           (SUPP_ID_DEFAULT << 27) |
                           (PDF_VIDEO_DATA_PACKET << 28);
    uint8_t HEC = calculateHEC(USB4_header);
    USB4_header |= HEC;
    return USB4_header;
}


/**
 * Generates a tunneled video data packet.
 *
 * @param USB4_header The USB4 header value.
 * @param TU_set_headers An array of TU set headers.
 * @param num_TU_sets The number of TU sets.
 * @param file The file to write the packet to.
 */
void generate_Tunneled_VD_Packet(uint32_t USB4_header, uint32_t *TU_set_headers, size_t num_TU_sets, FILE *file)
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

    // Allocate memory for the TU set headers
    tuSetHeaders = (uint8_t **)malloc(num_TU_sets * sizeof(uint8_t *));
    for(int i=0 ; i<num_TU_sets ; i++)
    {
        tuSetHeaders[i] = (uint8_t *)malloc(4 * sizeof(uint8_t));
        tuSetHeaders[i][0] = (TU_set_headers[i] >> 24) & 0xFF;
        tuSetHeaders[i][1] = (TU_set_headers[i] >> 16) & 0xFF;
        tuSetHeaders[i][2] = (TU_set_headers[i] >> 8) & 0xFF;
        tuSetHeaders[i][3] = TU_set_headers[i] & 0xFF;
    }

    // Calculate the total number of bytes for the payload
    int totalVideoDataLength = calculateTotalVideoDataLength(num_TU_sets, (const unsigned char**)tuSetHeaders);
    int totalPacketLength = 4 + (num_TU_sets * 4) + totalVideoDataLength;  // 4 bytes for Tunneled Packet Header + TU headers + Video data
    
    // Update the total packet length in the Tunneled Packet Header
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
        payloadLengths[i] = extractVideoCount(tuSetHeaders[i]);

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
}


/**
 * Generate a Video Data Packet
 * @param argc Number of arguments
 * @param argv Strings: VDP <EOC_1> <TU_type_1> <L_1> <Fill_Count_1> <Video_Count_1> ... <EOC_n> <TU_type_n> <L_n> <Fill_Count_n> <Video_Count_n>
 * @param file File to save the packet to
 * 
*/
int VDP_GEN(int argc, char *argv[], FILE* file)
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <EOC_1> <TU_type_1> <L_1> <Fill_Count_1> <Video_Count_1> ... <EOC_n> <TU_type_n> <L_n> <Fill_Count_n> <Video_Count_n>\n", argv[0]);
        return 1;
    }

    size_t num_TU_sets = 0;
    if ((argc - 1) % 5 != 0)
    {
        fprintf(stderr, "Error: Invalid number of arguments.\n");
        fprintf(stderr, "Usage: %s <EOC_1> <TU_type_1> <L_1> <Fill_Count_1> <Video_Count_1> ... <EOC_n> <TU_type_n> <L_n> <Fill_Count_n> <Video_Count_n>\n", argv[0]);

        return 1;
    }
    else num_TU_sets = (argc - 1) / 5;

    // ---------------------------------------------------
    // --------------- TU Set Header ---------------------
    // ---------------------------------------------------

    uint32_t *TU_set_headers = generate_VDP_TU_set_Headers(argc, argv);

    // ---------------------------------------------------
    // ----------- USB4 Tunneled Packet Header -----------
    // ---------------------------------------------------
    
    uint32_t Length = 0; // Calculated in the payload generation
    uint32_t USB4_header = generate_Tunneled_VDP_Header(Length);

    // ---------------------------------------------------
    // --------------- Generate Packet -------------------
    // ---------------------------------------------------
    generate_Tunneled_VD_Packet(USB4_header, TU_set_headers, num_TU_sets, file);

    return 0;
}