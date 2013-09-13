/*
 * From boot-132.
 */

#include "genboot.h"

unsigned long Adler32(unsigned char* buffer, long length) {
    long cnt;
    unsigned long result, lowHalf, highHalf;

    lowHalf = 1;
    highHalf = 0;

    for(cnt = 0; cnt < length; cnt++) {
        if((cnt % 5000) == 0) {
            lowHalf %= 65521L;
            highHalf %= 65521L;
        }
        lowHalf += buffer[cnt];
        highHalf += lowHalf;
    }

    lowHalf %= 65521L;
    highHalf %= 65521L;

    result = (highHalf << 16) | lowHalf;
    return result;
}
