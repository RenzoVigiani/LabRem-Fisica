/* Hexdump.h
 *
 * Description:  Arduino library for creating hexdumps from program memory.
 * 
 * Note:         Initially designed for ESP8266 therefore uses printf.
 *               To be replaced with standard Arduino print and println in a future version.
 * 
 * Author:       Leonel Lopes Parente
 * License:      MIT (see LICENSE file in repository root)
 * 
 */

#ifndef HEXDUMP_H_
#define HEXDUMP_H_

#include <Arduino.h>


void hexdump(
    Stream& stream, 
    const uint8_t* const data, 
    const uint32_t byteCount, 
    const bool header = true, 
    const char* const message = nullptr, 
    const uint8_t linefeeds = 1, 
    const uint16_t offset = 0)
{
    // Actual memory address where data points to is unknown so first byte is shown as address 0x0000.
    // Optionally an offset for the address labeling can be specified. This imitates as if memory starts 
    // at the offset. The offset feature can be useful when having to compare a hexdump with another hexdump.

    if (header)
    {
        if (message == nullptr || strlen(message) == 0)
        {
            stream.printf("Memory hexdump - 0x%X (%u) bytes\n", byteCount, byteCount);
        }
        else
        {
            stream.printf("Memory hexdump %s - 0x%X (%u) bytes\n", message, byteCount, byteCount);
        }
    }
    
    if (byteCount == 0)
    {
        stream.println("Byte count is 0 (hexdump).");
        for (uint8_t i = 0; i < linefeeds; ++i)
        {
            stream.println();
        }
        return;
    }     

    if (data == nullptr)
    {
        stream.println("Data nullpointer (hexdump).");
        for (uint8_t i = 0; i < linefeeds; ++i)
        {
            stream.println();
        }
        return;
    }              

    if (header)
    {
        if (offset > 0)
        {
            stream.printf("Showing addresses with simulated offset 0x%02X (%u).\n", offset, offset);
        }
        stream.println();

        // Output header
        stream.print("     ");
        for (uint8_t i = 0; i < 16; ++i)
        {
            if (i == 8)
            {
                stream.print("  ");
            }
            stream.printf(" %02X", i);
        }
        stream.print("\n     ");
        for (uint8_t i = 0; i < 16; ++i)
        {
            if (i == 8)
            {
                stream.print("  ");
            }
            stream.printf(" --");
        }
        stream.println();
    }

    // Output data

    uint16_t lineNr = 0;
    uint16_t address = offset;
    if ((address & 0xF) > 0)
    {
        ++lineNr;
        // Output first adreslabel and indent
        stream.printf("%04X:", address & 0xFFF0);
        for (uint8_t i = 0; i < (offset & 0xF); ++i)		
        {
            if (i == 8 && (offset & 0xF) >= 8)
            {
                stream.print("  ");
            } 
            stream.print("   ");
        }
    }

    // Output byte values
    for (uint16_t i = 0; i < byteCount; ++i)
    {
        // stream.printf("[i=%u, address=0x%04X, lineNr=%u]", i, address, lineNr); //debug
        if ((address & 0xF) == 0)
        {
#if defined(ESP8266)
            // If ESP8266 MCU yield() once every 16 bytes to prevent WDT resets.
            yield();
#endif						
            ++lineNr;
            if (lineNr > 1)
            {
                stream.println();
            }
            // Write address label
            stream.printf("%04X:", address & 0xFFF0);
        }
        if ((address & 0xF) ==  8) 
        {
            stream.print(" -");
        }		
        stream.printf(" %02X", data[i]);
        ++address;
    }
    stream.println();
    for (uint8_t i = 0; i < linefeeds; ++i)
    {
        stream.println();
    }
}

#endif  //HEXDUMP_H_