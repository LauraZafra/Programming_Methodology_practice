/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "MPTools.h"
#include "Byte.h"

using namespace std;


#ifdef __INTEGRATION__

TEST_F(_01_Basics, INTEGRATION_Byte)  { 
    DEF_EXECUTION_ENVIRONMENT(INTEGRATION_Byte);
    FROM_KEYBOARD("");
    EXPECTED_OUTPUT(
            00000000
10100000
10100001
10100011
10100111
0,1,2,5,7,
11111111
00000000
11111111
01111111
10111111
11011111
11101111
11110111
11111011
11111101
11111110
11111111
01111110
00111100
00011000
00000000
00011000
00111100
01111110
11111111
);
    CALL_FROM_FILE( "")<< "The output of the main program must match that of the assignment" << endl;
}


#endif