# include "headers/MSA.h"
# include "headers/utils.h"

void get_MSA_payload_format(enum PAYLOAD_TYPE payloadFormat[], int lane)
{
    switch(lane)
    {
        case 1:
        {
            enum PAYLOAD_TYPE MSA_PAYLOAD_FORMAT_LANE_1[36] = { MVID_23_16,     MVID_15_8,      MVID_7_0,       HTOTAL_15_8,
                                                                HTOTAL_7_0,     VTOTAL_15_8,    VTOTAL_7_0,     HSP_HSW_14_8,
                                                                HSW_7_0,        MVID_23_16,     MVID_15_8,      MVID_7_0,
                                                                HSTART_15_8,    HSTART_7_0,     VSTART_15_8,    VSTART_7_0,
                                                                VSP_VSW_14_8,   VSW_7_0,        MVID_23_16,     MVID_15_8,
                                                                MVID_7_0,       HWIDTH_15_8,    HWIDTH_7_0,     VHEIGHT_15_8,
                                                                VHEIGHT_7_0,    ZERO,           ZERO,           MVID_23_16,
                                                                MVID_15_8,      MVID_7_0,       NVID_23_16,     NVID_15_8,
                                                                NVID_7_0,       MISC0_7_0,      MISC1_7_0,      ZERO};
            
            for(int i=0 ; i<36 ; i++)
                payloadFormat[i] = MSA_PAYLOAD_FORMAT_LANE_1[i];
            return;
        }
        case 2:
        {
            enum PAYLOAD_TYPE MSA_PAYLOAD_FORMAT_LANE_2[36] = { MVID_23_16,     MVID_23_16,     MVID_15_8,      MVID_15_8,
                                                                MVID_7_0,       MVID_7_0,       HTOTAL_15_8,    HSTART_15_8,
                                                                HTOTAL_7_0,     HSTART_7_0,     VTOTAL_15_8,    VSTART_15_8, 
                                                                VTOTAL_7_0,     VSTART_7_0,     HSP_HSW_14_8,   VSP_VSW_14_8,
                                                                HSW_7_0,        VSW_7_0,        MVID_23_16,     MVID_23_16, 
                                                                MVID_15_8,      MVID_15_8,      MVID_7_0,       MVID_7_0, 
                                                                HWIDTH_15_8,    NVID_23_16,     HWIDTH_7_0,     NVID_15_8,
                                                                VHEIGHT_15_8,   NVID_7_0,       VHEIGHT_7_0,    MISC0_7_0,
                                                                ZERO,           MISC1_7_0,      ZERO,           ZERO };
            for(int i=0 ; i<36 ; i++)
                payloadFormat[i] = MSA_PAYLOAD_FORMAT_LANE_2[i];
            return;
        }
        case 4:
        {
            enum PAYLOAD_TYPE MSA_PAYLOAD_FORMAT_LANE_4[36] = { MVID_23_16,     MVID_23_16,     MVID_23_16,     MVID_23_16,
                                                                MVID_15_8,      MVID_15_8,      MVID_15_8,      MVID_15_8, 
                                                                MVID_7_0,       MVID_7_0,       MVID_7_0,       MVID_7_0, 
                                                                HTOTAL_15_8,    HSTART_15_8,    HWIDTH_15_8,    NVID_23_16, 
                                                                HTOTAL_7_0,     HSTART_7_0,     HWIDTH_7_0,     NVID_15_8, 
                                                                VTOTAL_15_8,    VSTART_15_8,    VHEIGHT_15_8,   NVID_7_0,
                                                                VTOTAL_7_0,     VSTART_7_0,     VHEIGHT_7_0,    MISC0_7_0, 
                                                                HSP_HSW_14_8,   VSP_VSW_14_8,   ZERO,           MISC1_7_0, 
                                                                HSW_7_0,        VSW_7_0,        ZERO,           ZERO };
            for(int i=0 ; i<36 ; i++)
                payloadFormat[i] = MSA_PAYLOAD_FORMAT_LANE_4[i];
            return;
        }
        default:
            payloadFormat = NULL;
            return; 
    }
}

// Generate a payload sequentially starting from 00
void generate_MSA_Payload(uint8_t *payload, size_t payloadLength, int lane) {
    enum PAYLOAD_TYPE *payloadFormat = (enum PAYLOAD_TYPE *)malloc(payloadLength * sizeof(enum PAYLOAD_TYPE));
    get_MSA_payload_format(payloadFormat, lane);
    if(payloadFormat == NULL) {
        printf("Invalid lane configuration.\n");
        return;
    }

    for (size_t i = 0; i < payloadLength; i++)
        payload[i] = payloadFormat[i] & 0xFF;
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
    uint8_t pdf = PDF_MAIN_STREAM_ATTRIBUTE_PACKET;                     // Protocol Defined Field for Main Stream Attribute Packet
    uint8_t hopID = HOPID_DEFAULT;       // Hop ID: Default value
    uint8_t suppID = SUPP_ID_DEFAULT;    // Supplemental ID: Shall be set to 0b
    uint8_t reserved = RESERVED_DEFAULT; // Reserved: Fixed to 0
    uint8_t length = 0x28;               // Length: Fixed to 0x28 for a Main Stream Attribute Packet
    uint32_t tunneledPacketHeader = 0;   // Placeholder for Tunnled Packet Header

    // Set fields in the Tunneled Packet Header
    tunneledPacketHeader |= pdf << 28;
    tunneledPacketHeader |= suppID << 27;
    tunneledPacketHeader |= reserved << 23;
    tunneledPacketHeader |= hopID << 16;
    tunneledPacketHeader |= length << 8;

    // Calculate the HEC (Header Error Control)
    uint8_t hec = calculateHEC(tunneledPacketHeader);
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

void generate_MSA_packet(uint32_t tunneledPacketHeader, uint32_t msaHeader, int lane, FILE *file)
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

    generate_MSA_Payload(payload, MSA_PAYLOAD_SIZE, lane);
    
    bytesToHexString(tunneledPacketHeaderArr, TUNNELED_PACKET_HEADER_SIZE, file, 1);
    bytesToHexString(msaHeaderArr, MSA_HEADER_SIZE, file, 0);
    bytesToHexString(payload, MSA_PAYLOAD_SIZE, file, 0);
}

/**
 * Generates an MSA packet with the given fill count and hop ID, and saves it to a file.
 *
 * @param fillCountString The fill count as a string.
 * @param file The file to save the MSA packet to.
 * @param lane The lane number as a string.
 */
void MSAP_GEN(const char* fillCountString, const char* LaneString, FILE* file)
{
    uint32_t fillCount = (uint32_t)strtol(fillCountString, NULL, 10);
    int lane = (int)strtol(LaneString, NULL, 10);
    
    uint32_t Tunneled_MSA_Packet_Header = generate_tunneled_MSA_packet_header();
    uint32_t MSA_Packet_Header = generate_MSA_packet_header(fillCount);
    
    generate_MSA_packet(Tunneled_MSA_Packet_Header, MSA_Packet_Header, lane, file);
}