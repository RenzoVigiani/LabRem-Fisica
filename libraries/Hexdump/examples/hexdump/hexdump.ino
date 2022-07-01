#include "Hexdump.h"

Stream& stream = Serial;


void hexdumpTest()
{
	stream.println("This test prints several hexdump variations of the same data.\n");

	uint8_t data[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";
	size_t dataSize = sizeof(data);

	stream.printf("The following data is used for the hexdumps:\n\"%s\"\n\n", data);
	stream.printf("Data size is 0x%X (%u) bytes.\n\n", dataSize, dataSize);

	hexdump(stream, data, dataSize, true, "of data", 2);
	hexdump(stream, data, dataSize, true, "of data with small offset and dash on first line", 2, 5);
	hexdump(stream, data, dataSize, true, "of data with small offset without dash on first line", 2, 9);
	hexdump(stream, data, dataSize, true, "of data with larger, rounded offset", 2, 0x200);
	hexdump(stream, data, dataSize, true, "of data with large offset", 2, 0x5104);	
	hexdump(stream, data, dataSize, true, "of data with very large offset and 16-bit overflow", 2, 0xFFE4);		
}


void setup() 
{
	Serial.begin(115200);

	stream.println("\n\nhexdump\n");
	
	hexdumpTest();
}


void loop() 
{
}