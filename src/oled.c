/*****************************************************************************
* | File      	:   OLED_1in3_test.c
* | Author      :   
* | Function    :   
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#include "oled.h"
#include "OLED_1in3_c.h"
#include "GUI_Paint.h"
#include <math.h>
#include <stdlib.h>

time sleeptime =
{
    .hour = 0,
    .minutes = 0
};

int init_display(void)
{
    DEV_Delay_ms(100);
    
    printf("OELD_init\r\n");
    if(DEV_Module_Init()!=0){
        while(1){
            printf("END\r\n");
        }
    }
    
    /* Init */
    OLED_1in3_C_Init();
    OLED_1in3_C_Clear();

    /*Init buttons*/
    DEV_KEY_Config(key0);
    DEV_KEY_Config(key1);
    Paint_Clear(BLACK);

    return 0;
}

time set_time(void)
{   
    UBYTE *BlackImage;
    UWORD Imagesize = ((OLED_1in3_C_WIDTH%8==0)? (OLED_1in3_C_WIDTH/8): (OLED_1in3_C_WIDTH/8+1)) * OLED_1in3_C_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        while(1){
            printf("Failed to apply for black memory...\r\n");
        }
    }

    printf("Näyttö päällä\r\n");
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 180, BLACK);	

    // Intro display
    Paint_DrawString_EN(10, 20, intro_str, &Font24, BLACK, WHITE);
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(2000);

    // Time set display
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 180, WHITE);	
    Paint_Clear(BLACK);

    // TODO: Make forloops one
    int iter = 0;
    int i = 0;
    for(i; i<2000; i++){
        if(DEV_Digital_Read(key1 ) == 0){
            Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
            iter = iter+1;
            i = 0;
        }else {
            Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
        }
            
        if(DEV_Digital_Read(key0 ) == 0){
            Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
            iter = iter-1;
            i = 0;
        }else {
            Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
        }

        sleeptime.hour = iter/40; // Adjust selection speed
        if(sleeptime.hour > 23 || sleeptime.hour < -23){sleeptime.hour = 0; iter = 0;}
        else if(sleeptime.hour < 0){sleeptime.hour = 24+sleeptime.hour;}

        // Blink numbers to be selected
        if(i/100 == 0 || i/100 == 1 || i/100 == 4 || i/100 == 5 || i/100 == 8 || i/100 == 9 || i/100 == 12 || i/100 == 13 || i/100 == 16 || i/100 == 17)
        {
            Paint_DrawString_EN(15, 20, format_h[sleeptime.hour], &Font24, WHITE, BLACK);
        }
        Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
        Paint_DrawString_EN(60, 20, format_m[0], &Font24, WHITE, BLACK);

        OLED_1in3_C_Display(BlackImage);
        Paint_Clear(BLACK);
    }
    iter = 0;
    i = 0;
    for(i; i<2000; i++){
        if(DEV_Digital_Read(key1 ) == 0){
            Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
            iter = iter+1;
            i = 0;
        }else {
            Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
        }
            
        if(DEV_Digital_Read(key0 ) == 0){
            Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
            iter = iter-1;
            i = 0;
        }else {
            Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
        }

        sleeptime.minutes = iter/40; // Adjust selection speed
        if(sleeptime.minutes > 5 || sleeptime.minutes < -5){sleeptime.minutes = 0; iter = 0;}
        else if(sleeptime.minutes < 0){sleeptime.minutes = 6+sleeptime.minutes;}

        // Blink numbers to be selected
        if(i/100 == 0 || i/100 == 1 || i/100 == 4 || i/100 == 5 || i/100 == 8 || i/100 == 9 || i/100 == 12 || i/100 == 13 || i/100 == 16 || i/100 == 17)
        {
            Paint_DrawString_EN(60, 20, format_m[sleeptime.minutes], &Font24, WHITE, BLACK);
        }
        Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
        Paint_DrawString_EN(15, 20, format_h[sleeptime.hour], &Font24, WHITE, BLACK);

        OLED_1in3_C_Display(BlackImage);
        Paint_Clear(BLACK);
    }

    Paint_DrawString_EN(60, 20, format_m[sleeptime.minutes], &Font24, WHITE, BLACK);
    Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
    Paint_DrawString_EN(15, 20, format_h[sleeptime.hour], &Font24, WHITE, BLACK);
    OLED_1in3_C_Display(BlackImage);
    Paint_Clear(BLACK);
    sleep_ms(2000);

    // Back to black
    OLED_1in3_C_Clear();

    return sleeptime;
}