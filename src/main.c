# include "headers/MSA.h"
# include "headers/BSP.h"
# include "headers/VDP.h"
# include "headers/SDP.h"

enum PacketType
{
    PACKET_TYPE_UNKNOWN,
    PACKET_TYPE_MSA,
    PACKET_TYPE_BSP,
    PACKET_TYPE_VDP,
    PACKET_TYPE_SDP
};

const char *PACKETTYPES[] = {
    "UNKNOWN",
    "MSA",
    "BSP",
    "VDP",
    "SDP"
};

struct Packet
{
    enum PacketType type;
    int argc;
    char **argv;
};

enum PacketType identify_packet_type(const char *str)
{
    if(strcmp(str, "MSA") == 0) return PACKET_TYPE_MSA;
    if(strcmp(str, "BSP") == 0) return PACKET_TYPE_BSP;
    if(strcmp(str, "VDP") == 0) return PACKET_TYPE_VDP;
    if(strcmp(str, "SDP") == 0) return PACKET_TYPE_SDP;
    return PACKET_TYPE_UNKNOWN;
}

struct Packet parse(const char* input)
{
    struct Packet packet;

    char *inputCopy = strdup(input);

    char *token = strtok(inputCopy, " ");
    packet.type = identify_packet_type(token);
    free(inputCopy);
    inputCopy = strdup(input);

    int argc = 1;
    char *tmp = inputCopy;
    while(*(tmp++) != '\0')
        if(*tmp == ' ') argc++;


    packet.argv = (char**)malloc((argc+1) * sizeof(char*));
    if(packet.argv == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strcpy(inputCopy, input);
    token = strtok(inputCopy, " ");
    for(int i=0 ; i<argc ; i++)
    {
        packet.argv[i] = strdup(token);
        token = strtok(NULL, " \n");
        if(packet.argv[i] == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    packet.argc = argc;

    free(inputCopy);

    return packet;
}

int main()
{
    FILE *inputfile = fopen("data/input/sample.txt", "r");
    if(inputfile == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    FILE *outputfile = fopen("data/output/output.txt", "w");
    char line[150];
    while(fgets(line, 150, inputfile))
    {
        if (line[0] == '#') continue;
        struct Packet packet = parse(line);
        printf("Generate a packet of type: %s\n", PACKETTYPES[packet.type]);
        switch(packet.type)
        {
            case PACKET_TYPE_MSA:
                MSAP_GEN(packet.argv[1], outputfile);
                break;
            case PACKET_TYPE_BSP:
                BSP_GEN(packet.argv[1], packet.argv[2], packet.argv[3], outputfile);
                break;
            case PACKET_TYPE_VDP:
                VDP_GEN(packet.argc, packet.argv, outputfile);
                break;
            case PACKET_TYPE_SDP:
                SDP_GEN(packet.argc, packet.argv, outputfile);
                break;
            case PACKET_TYPE_UNKNOWN:
                fprintf(stderr, "Unknown packet type\n");
                break;
        }
    }

    // char *test = "MSA 100\n";
    // struct Packet packet = parse(test);
    // printf("Packet type: %d, argc: %d, argv: ", packet.type, packet.argc);
    // for(int i=0 ; i<packet.argc ; i++) printf("%s ", packet.argv[i]);
    // printf("\n\n");

    // char *test2 = "BSP 1 0 100\n";
    // struct Packet packet2 = parse(test2);
    // printf("Packet type: %d, argc: %d, argv: ", packet2.type, packet2.argc);
    // for(int i=0 ; i<packet2.argc ; i++) printf("%s ", packet2.argv[i]);
    // printf("\n\n");

    // char *test3 = "VDP 0 1 1 100 50 0 1 1 100 50\n";
    // struct Packet packet3 = parse(test3);
    // printf("Packet type: %d, argc: %d, argv: ", packet3.type, packet3.argc);
    // for(int i=0 ; i<packet3.argc ; i++) printf("%s ", packet3.argv[i]);
    // printf("\n\n");

    // char *test4 = "SDP 1 0 1 1 100 50 1 0 1 1 100 50\n";
    // struct Packet packet4 = parse(test4);
    // printf("Packet type: %d, argc: %d, argv: ", packet4.type, packet4.argc);
    // for(int i=0 ; i<packet4.argc ; i++) printf("%s ", packet4.argv[i]);
    // printf("\n\n");

    // char *folder = "data/output/";
    // char filepath_MSA[100];
    // char filepath_BSP[100];
    // char filepath_VDP[100];
    // char filepath_SDP[100];

    // sprintf(filepath_MSA, "%s%s", folder, "MSA.txt");
    // sprintf(filepath_BSP, "%s%s", folder, "BSP.txt");
    // sprintf(filepath_VDP, "%s%s", folder, "VDP.txt");
    // sprintf(filepath_SDP, "%s%s", folder, "SDP.txt");

    // FILE *file_MSA = fopen(filepath_MSA, "w");
    // FILE *file_BSP = fopen(filepath_BSP, "w");
    // FILE *file_VDP = fopen(filepath_VDP, "w");
    // FILE *file_SDP = fopen(filepath_SDP, "w");

    // char *argv_VDP[] = {"VDP", "0", "1", "1", "100", "50", "0", "1", "1", "100", "50"};
    // char *argv_SDP[] = {"SDP", "1", "0", "1", "1", "100", "50", "1", "0", "1", "1", "100", "50"};


    // printf("Generating Main Stream Attributes Packet...\n");
    // MSAP_GEN("100", file_MSA);

    // printf("Generating Blank Start Packet...\n");
    // BSP_GEN("1", "0", "100", file_BSP);

    // printf("Generating Video Data Packet...\n");
    // VDP_GEN(11, argv_VDP, file_VDP);

    // printf("Generating Secondary Data Packet...\n");
    // SDP_GEN(13, argv_SDP, file_SDP);
    return 0;
}