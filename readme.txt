**********   Input (.txt file)   **********
Description: This file contains lines that describe packet parameters.

Format: <Packet type> <Param 1> <Param 2> ... <Param n>

Packet types:
	1. MSA <Fill Count> <HopID>
	2. BSP <SR> <CP> <Fill Count>
	3. VDP 	<HopID>	    <EOC_1>  <TU_type_1>  <L_1>   <Fill_Count_1> <Video_Count_1> ... <EOC_n> <TU_type_n> <L_n> <Fill_Count_n> <Video_Count_n>
	4. SDP 	<HopID>  <EFC/ND_1>  <NSS_1>     <NSE_1> <L_1> <Fill_Count_1> <Secondary_Count_1> ... <EFC/ND_n> <NSS_n> <NSE_n> <L_n> <Fill_Count_n> <Secondary_Count_n>
###### Example ######
	MSA 982 2
	MSA 189 7
	BSP 1 0 666
	BSP 1 1 881
...

**********   Output(.txt file)   **********
Description: This file contains the generated packet contents represented in hexadecimal format, with two numbers grouped together.
Format: <Valid> <Header> <B0> <B1> <B2> <B3>


###### Example ######
  1  1 12 34 56 78
  1  0 90 AB CD EF
  1  0 12 34 56 78
  ...

Note:
1. Input Limitation: If the input exceeds the number of bits that can e represented, the exceeding bits will be discarded. e.g. If a byte can only hold a value from 0 to 255 and the value is 300, the resulting value saved will be 300 % 256 = 44.
2. Valid Byte is always 1 in our case.
3. Payload Generation Method: The payload is generated sequentially starting from 00, 01, 02.

TBD:
1. Need another payload generation method?
2. What if input is not correspond to the format? Terminated or just set the missed value as 0 or ...

