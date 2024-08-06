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

int is_comment(const char *line)
{
    if(line[0] == '#') return 1;
    if(line[0] == '\n') return 1;
    return 0;
}

int main()
{
    const char *filename = "sample.txt";
    const char *inputfolder = "data/input/";
    const char *outputfolder = "data/output/";
    char inputfilepath[100];
    char outputfilepath[100];

    sprintf(inputfilepath, "%s%s", inputfolder, filename);
    sprintf(outputfilepath, "%s%s", outputfolder, filename);

    FILE *inputfile = fopen(inputfilepath, "r");
    if(inputfile == NULL)
    {
        fprintf(stderr, "Error reading input file.\n");
        exit(1);
    }

    FILE *outputfile = fopen(outputfilepath, "w");
    if(outputfile == NULL)
    {
        fprintf(stderr, "Error opening output file.\n");
        exit(1);
    }

    char lane[500];
    while(fgets(lane, 500, inputfile) && is_comment(lane));
    // if lane number is not equal to 1, 2, or 4, set it as 1
    printf("Lane: %s", lane);
    if(lane[0] != '1' && lane[0] != '2' && lane[0] != '4')
    {
        fprintf(stderr, "Invalid lane number. Setting lane number to 1.\n");
        strcpy(lane, "1");
    }
    else lane[1] = '\0';

    char line[500];
    while(fgets(line, 500, inputfile))
    {
        if (is_comment(line)) continue;
        struct Packet packet = parse(line);
        printf("Generate a packet of type: %s\n", PACKETTYPES[packet.type]);
        switch(packet.type)
        {
            case PACKET_TYPE_MSA:
                MSAP_GEN(packet.argv[1], lane, outputfile);
                break;
            case PACKET_TYPE_BSP:
                BSP_GEN(packet.argv[1], packet.argv[2], packet.argv[3], lane, outputfile);
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
    return 0;
}