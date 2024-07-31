# include "headers/MSA.h"
# include "headers/BSP.h"
# include "headers/VDP.h"
# include "headers/SDP.h"

int main()
{
    FILE *file_MSA = fopen("results/MSA.txt", "w");
    FILE *file_BSP = fopen("results/BSP.txt", "w");
    FILE *file_VDP = fopen("results/VDP.txt", "w");
    FILE *file_SDP = fopen("results/SDP.txt", "w");

    char *argv_VDP[] = {"VDP", "0", "1", "1", "100", "50", "0", "1", "1", "100", "50"};
    char *argv_SDP[] = {"SDP", "1", "0", "1", "1", "100", "50", "1", "0", "1", "1", "100", "50"};

    printf("Generating Main Stream Attributes Packet...\n");
    MSAP_GEN("100", file_MSA);

    printf("Generating Blank Start Packet...\n");
    BSP_GEN("1", "0", "100", file_BSP);

    printf("Generating Video Data Packet...\n");
    VDP_GEN(11, argv_VDP, file_VDP);

    printf("Generating Secondary Data Packet...\n");
    SDP_GEN(13, argv_SDP, file_SDP);
    return 0;
}