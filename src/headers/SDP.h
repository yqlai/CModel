# ifndef __GENERAL_H
# define __GENERAL_H

# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>

# endif

# ifndef __SDP_H
# define __SDP_H

# include <stdint.h>


int extractSecondaryCount(const unsigned char*);
int calculateTotalSecondaryDataLength(int, const unsigned char**);
uint32_t generate_TU_set_Header(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t *generate_TU_set_Headers(int, char*[]);
uint32_t generate_Tunneled_SDP_Header(uint32_t);
void generate_Tunneled_SD_Packet(uint32_t, uint32_t*, size_t, FILE*);
int SDP_GEN(int, char*[], FILE*);


# endif