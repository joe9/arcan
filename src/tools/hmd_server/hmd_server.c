#include "arcan_math.h"
#include "hmd_platform.h"
#include "platform.h"

#include <hidapi/hidapi.h>

/*
 * vive version:
 *  enumerate on vid/pid
 *  -> send feature report (hand, buf, 17)
 *
 *  -> read 128 bytes,
 *  	byte[4] is packet type:
 *  	 - if TRIGGER: [5] iss buttons
 *     - if CLICK: ...
 *     if ANALOG:
 *     if TRACKPAD:
 *     if TRACKPAD_TAP: [buf 6 = x, buf 8 = y],
 *     if AXISv: pitch[9], roll[7]
 */

int main(int argc, char** argv)
{
/* map connection,
 * scan devices,
 * update state fields
 * enter poll loop
 * decode protocol
 */
}
