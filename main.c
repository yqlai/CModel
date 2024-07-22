# include "headers/MSA.h"
# include "headers/BSP.h"

int main()
{
    MSAP_GEN("100", "3", "MSA.txt");
    BSP_GEN("1", "0", "100", "BSP.txt");
    return 0;
}