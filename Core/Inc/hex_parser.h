#ifndef INC_HEX_PARSER_H_
#define INC_HEX_PARSER_H_

#include <inttypes.h>

void asciiToHex(uint8_t* buff, uint8_t count);
void fillBuffer(uint8_t* destination, uint16_t* startPtr, uint16_t count);
void flashHex(uint16_t size);
#endif /* INC_HEX_PARSER_H_ */
