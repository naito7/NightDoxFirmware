/*
 * SeeedOLED.cpp
 * SSD130x OLED Driver Library
 *
 * Copyright (c) 2011 seeed technology inc.
 * Author        :   Visweswara R
 * Create Time   :   Dec 2011
 * Change Log    :
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* #include "Wire.h" */
#include "SeeedOLED.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <util/twi.h>
#include "../../../lib/twi.h"  // `TWI_FREQ` defined in "teensy-2-0.c"
#include "../../../main.h"
#include "../../../lib-other/pjrc/usb_keyboard/usb_keyboard.h"

#include <avr/pgmspace.h>

/* #include <avr/pgmspace.h> */

// 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters(glyphs)
// BasicFont is placed in code memory.

// This font be freely used without any restriction(It is placed in public domain)
const unsigned char BasicFont[][8] PROGMEM=
{
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00},
  {0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00},
  {0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00},
  {0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00},
  {0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00},
  {0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00},
  {0x00,0x1C,0x22,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x22,0x1C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x2A,0x1C,0x2A,0x08,0x00,0x00},
  {0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00},
  {0x00,0xA0,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
  {0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00},
  {0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00},
  {0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00},
  {0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x00},
  {0x00,0x22,0x41,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00},
  {0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00},
  {0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00},
  {0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x00},
  {0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00},
  {0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00},
  {0x00,0x00,0xAC,0x6C,0x00,0x00,0x00,0x00},
  {0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00},
  {0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00},
  {0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00},
  {0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00},
  {0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x22,0x1C,0x00,0x00},
  {0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x51,0x72,0x00,0x00},
  {0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00},
  {0x00,0x41,0x7F,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00},
  {0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00},
  {0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00},
  {0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00},
  {0x00,0x7F,0x04,0x08,0x10,0x7F,0x00,0x00},
  {0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00},
  {0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00},
  {0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00},
  {0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00},
  {0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00},
  {0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00},
  {0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00},
  {0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00},
  {0x00,0x3F,0x40,0x38,0x40,0x3F,0x00,0x00},
  {0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00},
  {0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x00},
  {0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00},
  {0x00,0x7F,0x41,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00},
  {0x00,0x41,0x41,0x7F,0x00,0x00,0x00,0x00},
  {0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00},
  {0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00},
  {0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00},
  {0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00},
  {0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x28,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00},
  {0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00},
  {0x00,0x08,0x7E,0x09,0x02,0x00,0x00,0x00},
  {0x00,0x18,0xA4,0xA4,0xA4,0x7C,0x00,0x00},
  {0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00},
  {0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x80,0x84,0x7D,0x00,0x00,0x00,0x00},
  {0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00},
  {0x00,0x41,0x7F,0x40,0x00,0x00,0x00,0x00},
  {0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x00},
  {0x00,0x7C,0x08,0x04,0x7C,0x00,0x00,0x00},
  {0x00,0x38,0x44,0x44,0x38,0x00,0x00,0x00},
  {0x00,0xFC,0x24,0x24,0x18,0x00,0x00,0x00},
  {0x00,0x18,0x24,0x24,0xFC,0x00,0x00,0x00},
  {0x00,0x00,0x7C,0x08,0x04,0x00,0x00,0x00},
  {0x00,0x48,0x54,0x54,0x24,0x00,0x00,0x00},
  {0x00,0x04,0x7F,0x44,0x00,0x00,0x00,0x00},
  {0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00},
  {0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00},
  {0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00},
  {0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00},
  {0x00,0x1C,0xA0,0xA0,0x7C,0x00,0x00,0x00},
  {0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00},
  {0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x41,0x36,0x08,0x00,0x00,0x00,0x00},
  {0x00,0x02,0x01,0x01,0x02,0x01,0x00,0x00},
  {0x00,0x02,0x05,0x05,0x02,0x00,0x00,0x00} 
};

char addressingMode;

uint8_t SeeedOLED_sendCommand(unsigned char command)
{
  uint8_t ret = 0;
  twi_start();
  ret = twi_send((SeeedOLED_Address<<1)|0); // begin I2C communication & write
  if(ret) goto out;
  ret = twi_send(SeeedOLED_Command_Mode);        // Set OLED Command mode
  if(ret) goto out;
  ret = twi_send(command); 
 out:  
  twi_stop(); // End I2C communication
  return ret;
}

uint8_t SeeedOLED_sendData(unsigned char Data)
{
  uint8_t ret = 0;
  twi_start();
  ret = twi_send((SeeedOLED_Address<<1)|0); // begin I2C communication
  if(ret) goto out;
  ret = twi_send(SeeedOLED_Data_Mode);            // data mode
  if(ret) goto out;
  ret = twi_send(Data);
  if(ret) goto out;
 out:
  twi_stop(); // stop I2C transmission
  return ret;
}

uint8_t SeeedOLED_init(void)
{
  if(SeeedOLED_sendCommand(SeeedOLED_Display_Off_Cmd))     //display off
    return 1;
  SeeedOLED_clearDisplay();
  /* delay(5);  */
  if(SeeedOLED_sendCommand(SeeedOLED_Display_On_Cmd))  //display on
    return 1;
  /* delay(5);  */
  if(SeeedOLED_sendCommand(SeeedOLED_Normal_Display_Cmd))  //Set Normal Display (default)
    return 1;
  SeeedOLED_sendCommand(SeeedOLED_Segment_Remap_Cmd);
  SeeedOLED_sendCommand(SeeedOLED_Com_Remap_Cmd);
  return 0;
}

void SeeedOLED_setBrightness(unsigned char Brightness)
{
   SeeedOLED_sendCommand(SeeedOLED_Set_Brightness_Cmd);
   SeeedOLED_sendCommand(Brightness);
}

void SeeedOLED_setHorizontalMode()
{
    addressingMode = HORIZONTAL_MODE;
    SeeedOLED_sendCommand(0x20);          //set addressing mode
    SeeedOLED_sendCommand(0x00);          //set horizontal addressing mode
}

void SeeedOLED_setPageMode()
{
    addressingMode = PAGE_MODE;
    SeeedOLED_sendCommand(0x20);          //set addressing mode
    SeeedOLED_sendCommand(0x02);          //set page addressing mode
}


void SeeedOLED_setTextXY(unsigned char Row, unsigned char Column)
{
    SeeedOLED_sendCommand(0xB0 + Row);            //set page address
    SeeedOLED_sendCommand(0x00 + (8*Column & 0x0F));  //set column lower address
    SeeedOLED_sendCommand(0x10 + ((8*Column>>4)&0x0F));   //set column higher address
}


void SeeedOLED_clearDisplay()
{
  unsigned char i,j;
  SeeedOLED_sendCommand(SeeedOLED_Display_Off_Cmd);   //display off
  for(j=0;j<8;j++)
  { 
    SeeedOLED_setTextXY(j,0);    
    {
      for(i=0;i<16;i++)  //clear all columns
      {
        SeeedOLED_putChar(' ');    
      }
    }
  }
  SeeedOLED_sendCommand(SeeedOLED_Display_On_Cmd);    //display on
  SeeedOLED_setTextXY(0,0);    
}

void SeeedOLED_putChar(unsigned char C)
{
  if(C < 32 || C > 127) //Ignore non-printable ASCII characters. This can be modified for multilingual font.
    {
      C=' '; //Space
    }   
  unsigned char i=0;
  for(i=0;i<8;i++)
    {
      //read bytes from code memory
      SeeedOLED_sendData(pgm_read_byte(&BasicFont[C-32][i])); //font array starts at 0, ASCII starts at 32. Hence the translation
    }
}

void SeeedOLED_putString(const char *String)
{
    unsigned char i=0;
    while(String[i])
    {
        SeeedOLED_putChar(String[i]);     
        i++;
    }
}


void SeeedOLED_putHalfByteAsHex(unsigned char c){
  uint8_t cc = c&0x0F;
  if(cc<=9)
    SeeedOLED_putChar('0'+cc);
  else
    SeeedOLED_putChar('A'+cc-10);
}
void SeeedOLED_putByteAsHex(unsigned char c){
  SeeedOLED_putHalfByteAsHex(c>>4);
  SeeedOLED_putHalfByteAsHex(c);
}

unsigned char SeeedOLED_putNumber(long long_num)
{
  unsigned char char_buffer[10]="";
  unsigned char i = 0;
  unsigned char f = 0;

  if (long_num < 0) 
  {
    f=1;
    SeeedOLED_putChar('-');
    long_num = -long_num;
  } 
  else if (long_num == 0) 
  {
    f=1;
    SeeedOLED_putChar('0');
    return f;
  } 

  while (long_num > 0) 
  {
    char_buffer[i++] = long_num % 10;
    long_num /= 10;
  }

  f=f+i;
  for(; i > 0; i--)
  {
    SeeedOLED_putChar('0'+ char_buffer[i - 1]);
  }
  return f;

}

unsigned char SeeedOLED_putFloatD(float floatNumber,unsigned char decimal)
{
  unsigned int temp=0;
  float decy=0.0;
  float rounding = 0.5;
  unsigned char f=0;
  if(floatNumber<0.0)
  {
    SeeedOLED_putString("-");
    floatNumber = -floatNumber;
    f +=1;
  }
  for (unsigned char i=0; i<decimal; ++i)
  {
    rounding /= 10.0;
  }
    floatNumber += rounding;
  
  temp = floatNumber;
  f += SeeedOLED_putNumber(temp);
  if(decimal>0)
  {
    SeeedOLED_putChar('.');
    f +=1;
 }
  decy = floatNumber-temp;//decimal part, 
  for(unsigned char i=0;i<decimal;i++)//4 
  {
    decy *=10;// for the next decimal
    temp = decy;//get the decimal
    SeeedOLED_putNumber(temp);
    decy -= temp;
  }
  f +=decimal;
  return f;
}
unsigned char SeeedOLED_putFloat(float floatNumber)
{
  unsigned char decimal=2;
  unsigned int temp=0;
  float decy=0.0;
  float rounding = 0.5;
  unsigned char f=0;
  if(floatNumber<0.0)
    {
      SeeedOLED_putString("-");
      floatNumber = -floatNumber;
      f +=1;
    }
  for (unsigned char i=0; i<decimal; ++i)
    {
      rounding /= 10.0;
    }
  floatNumber += rounding;
  
  temp = floatNumber;
  f += SeeedOLED_putNumber(temp);
  if(decimal>0)
    {
      SeeedOLED_putChar('.');
      f +=1;
    }
  decy = floatNumber-temp;//decimal part, 
  for(unsigned char i=0;i<decimal;i++)//4 
    {
      decy *=10;// for the next decimal
      temp = decy;//get the decimal
      SeeedOLED_putNumber(temp);
      decy -= temp;
    }
  f +=decimal;
  return f;
}

void SeeedOLED_drawBitmap(unsigned char *bitmaparray,int bytes)
{
  char localAddressMode = addressingMode;
  if(addressingMode != HORIZONTAL_MODE)
  {
      //Bitmap is drawn in horizontal mode  
      SeeedOLED_setHorizontalMode();
  }

  for(int i=0;i<bytes;i++)
  {
      SeeedOLED_sendData(pgm_read_byte(&bitmaparray[i]));
  }

  if(localAddressMode == PAGE_MODE)
  {
     //If pageMode was used earlier, restore it.
     SeeedOLED_setPageMode(); 
  }
  
}

void SeeedOLED_setHorizontalScrollProperties(bool direction,unsigned char startPage, unsigned char endPage, unsigned char scrollSpeed)
{
/*
Use the following defines for 'direction' :

 Scroll_Left            
 Scroll_Right           

Use the following defines for 'scrollSpeed' :

 Scroll_2Frames     
 Scroll_3Frames
 Scroll_4Frames
 Scroll_5Frames 
 Scroll_25Frames
 Scroll_64Frames
 Scroll_128Frames
 Scroll_256Frames

*/

   if(Scroll_Right == direction)
   {
        //Scroll Right
        SeeedOLED_sendCommand(0x26);
   }
   else
   {
        //Scroll Left  
        SeeedOLED_sendCommand(0x27);

   }
    SeeedOLED_sendCommand(0x00);
    SeeedOLED_sendCommand(startPage);
    SeeedOLED_sendCommand(scrollSpeed);
    SeeedOLED_sendCommand(endPage);
    SeeedOLED_sendCommand(0x00);
    SeeedOLED_sendCommand(0xFF);
}

void SeeedOLED_activateScroll()
{
    SeeedOLED_sendCommand(SeeedOLED_Activate_Scroll_Cmd);
}

void SeeedOLED_deactivateScroll()
{
    SeeedOLED_sendCommand(SeeedOLED_Dectivate_Scroll_Cmd);
}

void SeeedOLED_setNormalDisplay()
{
    SeeedOLED_sendCommand(SeeedOLED_Normal_Display_Cmd);
}

void SeeedOLED_setInverseDisplay()
{
    SeeedOLED_sendCommand(SeeedOLED_Inverse_Display_Cmd);
}


/* SeeedOLED SeeedOled;  // Preinstantiate Objects */

int oled_counter;
uint8_t prev_keyboard_leds;
uint8_t prev_keyboard_modifier_keys;
uint8_t prev_keyboard_keys[6];
uint8_t prev_layers_state[16];
uint8_t oled_init(void){
  oled_counter = 0;
  prev_keyboard_leds = 0;
  prev_keyboard_modifier_keys = -1;
  for(int i=0; i<16; ++i)
    prev_layers_state[i] = -1;
  for(int i=0; i<6; ++i)
    prev_keyboard_keys[i] = -1;
    
  SeeedOLED_init();
  SeeedOLED_setTextXY(7, 0);
  SeeedOLED_putString("Num");
  SeeedOLED_setTextXY(7, 5);
  SeeedOLED_putString("Caps");
  
  return 0;
}
unsigned char water_wheels[] = {
  '-', '/', '|', '\\',
};
uint8_t oled_update(void){
  {
    uint8_t layers_state[16];
    for(int i=0; i<8; ++i){
      layers_state[i*2+0] = main_layers_peek(i);
      layers_state[i*2+1] = main_layers_peek_sticky(i);
    }
    if(memcmp(layers_state, prev_layers_state, 16)!=0){
      uint8_t layers_head = main_layers_get_head();
      uint8_t i=0;
      for(; i<=layers_head; ++i){
        SeeedOLED_setTextXY(0, i*2);
        SeeedOLED_putNumber(main_layers_peek(i));
        SeeedOLED_setTextXY(0, i*2+1);
        SeeedOLED_putNumber(main_layers_peek_sticky(i));
      }
      for(; i<8; ++i){
        SeeedOLED_setTextXY(0, i*2);
        SeeedOLED_putString("  ");
      }
      memcpy(prev_layers_state, layers_state, 16);
    }
  }
  if(memcmp(keyboard_keys, prev_keyboard_keys, 6)!=0 ||
     prev_keyboard_modifier_keys != keyboard_modifier_keys){
    SeeedOLED_setTextXY(1, 0);

    SeeedOLED_putByteAsHex(keyboard_modifier_keys);
    for(int i=0; i<6; ++i)
      SeeedOLED_putByteAsHex(keyboard_keys[i]);
    
    // backup prev state
    memcpy(prev_keyboard_keys, keyboard_keys, 6);
    prev_keyboard_modifier_keys = keyboard_modifier_keys;
  }

  if((prev_keyboard_leds^keyboard_leds)&(1<<0)){
    SeeedOLED_setTextXY(7, 3);
    SeeedOLED_putChar(keyboard_leds & (1<<0) ? 'O' : 'X');
  }
  if((prev_keyboard_leds^keyboard_leds)&(1<<1)){
    SeeedOLED_setTextXY(7, 9);
    SeeedOLED_putChar(keyboard_leds & (1<<1) ? 'O' : 'X');
  }
  prev_keyboard_leds = keyboard_leds;
  
  SeeedOLED_setTextXY(7, 15);
  SeeedOLED_putChar(water_wheels[oled_counter++]);
  if(oled_counter>=4)
    oled_counter=0;
  return 0;
}
