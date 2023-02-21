#include "hex_parser.h"
#include "flash.h"
#include "boot_config.h"

uint32_t extented_linear_adress = 0;//дополнительный адрес
uint8_t size_data, type_data, check_sum;//размер, тип данных и чек сумма
uint16_t address_data;//младшие 16 бит адреса
uint32_t program_data;//слово которое пишется во флеш
uint8_t calculation_check_sum = 0;//чек-сумма
uint8_t tempBuf[8];

void asciiToHex(uint8_t* buff, uint8_t count) {
	uint8_t i;
	for(i=0; i<count;i++) {
		if(buff[i] <= '9' && buff[i] >= '0' ) {
			buff[i] -= 0x30;
		}
		else {
			buff[i] = buff[i] - 0x41 + 10;
		}
	}
}

void fillBuffer(uint8_t* destination, uint8_t* source, uint16_t* startPtr, uint16_t count) {
	for (int16_t i = 0; i < count; i++) {
		destination[i] = source[*startPtr + i];
	}
	*startPtr += count;
}

uint8_t flashHex(uint8_t* flashBuf, uint16_t size) {
	uint16_t ptr = 0;
	while (ptr < size) {
		if(flashBuf[ptr] == ':') {
			ptr++;
			fillBuffer(tempBuf, flashBuf, &ptr, 8);
			asciiToHex(tempBuf, 8);

			size_data = 2*(tempBuf[1] + 16*tempBuf[0]);//находим размер данных
			address_data = tempBuf[5] + 16*tempBuf[4] + 256*tempBuf[3] + 4096*tempBuf[2];//адрес
			type_data = tempBuf[7] + 16*tempBuf[6];//и тип

			calculation_check_sum = size_data/2 + (uint8_t)address_data + (uint8_t)(address_data>>8) + type_data;//считаем чек сумму

			if(type_data == 0x00) {
				while(size_data > 0) {
					fillBuffer(tempBuf, flashBuf, &ptr, 8);
					asciiToHex(tempBuf, 8);


					for(uint8_t i = 0; i < 8; i = i + 2) { //формируем 32-битное слово для записи
						tempBuf[i] <<= 4;
						tempBuf[i] = tempBuf[i] | tempBuf[i+1];
						program_data |= tempBuf[i] <<(i*4);
					}

					FlashWriteWord(extented_linear_adress + address_data, program_data);
					calculation_check_sum += (uint8_t)program_data + (uint8_t)(program_data>>8) + (uint8_t)(program_data>>16) + (uint8_t)(program_data>>24);
					size_data -= 8;
					address_data += 4;
					program_data = 0;
				}
				calculation_check_sum =  ~(calculation_check_sum) + 1;

				fillBuffer(tempBuf, flashBuf, &ptr, 2);
				asciiToHex(tempBuf, 2);


				check_sum = tempBuf[1] + 16*tempBuf[0];
				if(calculation_check_sum != check_sum ) {
					return 0;//uartTransmit("\n\rchecksum error 1\n\r", 20);
				}
				calculation_check_sum = 0;//обнуляем чек сумму

			} else if(type_data == 0x04) {//дополнительный адрес
				fillBuffer(tempBuf, flashBuf, &ptr, 4);
				asciiToHex(tempBuf, 4);

				extented_linear_adress = (uint32_t)(tempBuf[0]<<28 | tempBuf[1]<<24 | tempBuf[2]<<20 | tempBuf[3]<<16 );//считаем адрес
				if ((getLatestApplicationAddress() & 0xFFFF0000) != extented_linear_adress) return 2;//TODO

				calculation_check_sum +=  16*tempBuf[0] + tempBuf[1]+ 16*tempBuf[2] + tempBuf[3];
				calculation_check_sum =  ~(calculation_check_sum) + 1;

				fillBuffer(tempBuf, flashBuf, &ptr, 2);
				asciiToHex(tempBuf, 2);


				check_sum = tempBuf[1] + 16*tempBuf[0];
				if(calculation_check_sum != check_sum ) {
					return 0;//uartTransmit("\n\rchecksum error 2\n\r", 20);
				}
				calculation_check_sum = 0;//обнуляем чек сумму
			} else if(type_data == 0x01) {//конец файла
				break;
			}
		}
		ptr++;
	}
	return 1;
}
