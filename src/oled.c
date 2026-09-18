#include "oled.h"
#include "OLED_1in3_c.h"
#include "GUI_Paint.h"
#include <math.h>
#include <stdlib.h>

time sleeptime =
{
    .sleephour = 0,
    .sleepminutes = 0,
    .wakehour = 0,
    .wakeminutes = 0,
    .durhour = 0,
    .durminutes = 0,
    .total = 0
};

int init_display(void)
{
    DEV_Delay_ms(100);
    
    printf("OELD_init\r\n");
    if(DEV_Module_Init()!=0){
        return 1;
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

char *formatted(int res)
{
    char *form = (char *)malloc(12 * sizeof(char));
    if(form == NULL){return NULL;}

    if(res < 10)
    {sprintf(form, "0%d", res);}
    else
    {sprintf(form, "%d", res);}

    return form;
}

int set_time(void)
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

    volatile int *hour= NULL;
    volatile int *minute= NULL;
    char *info=NULL;
    // TODO: Make forloops one
    for(int setup=0; setup<2; setup++)  /*Whether we're setting sleep or wake time*/
    {
        info = (setup == 0) ? "Current time:" : "Wakeup time:";
        hour = (setup == 0) ? &sleeptime.sleephour : &sleeptime.wakehour;
        minute = (setup == 0) ? &sleeptime.sleepminutes : &sleeptime.wakeminutes;
        
        int iter = 0;
        int i = 0;
        for(i; i<2000; i++){
            if(DEV_Digital_Read(key1 ) == 0){
                Paint_DrawChar(115, 5, '+', &Font12, BLACK, WHITE);
                iter = iter+1;
                i = 0;
            }else {;
                Paint_DrawChar(110, 0, '+', &Font24, BLACK, WHITE);
            }
                
            if(DEV_Digital_Read(key0 ) == 0){
                Paint_DrawChar(115, 50, '-', &Font12, BLACK, WHITE);
                iter = iter-1;
                i = 0;
            }else {
                Paint_DrawChar(110, 45, '-', &Font24, BLACK, WHITE);
            }

            *hour = iter/40; // Adjust selection speed
            if(*hour > 23 || *hour < -23){*hour = 0; iter = 0;}
            else if(*hour < 0){*hour = 24+*hour;}

            // Blink numbers to be selected
            if(i/100 == 0 || i/100 == 1 || i/100 == 4 || i/100 == 5 || i/100 == 8 || i/100 == 9 || i/100 == 12 || i/100 == 13 || i/100 == 16 || i/100 == 17)
            {
                Paint_DrawString_EN(15, 20, formatted(*hour), &Font24, WHITE, BLACK);
            }
            Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
            Paint_DrawString_EN(60, 20, format_m[0], &Font24, WHITE, BLACK);
            Paint_DrawString_EN(15, 5, info, &Font12, WHITE, BLACK);

            OLED_1in3_C_Display(BlackImage);
            Paint_Clear(BLACK);
        }
        iter = 0;
        i = 0;
        for(i; i<2000; i++){
            if(DEV_Digital_Read(key1 ) == 0){
                Paint_DrawChar(115, 5, '+', &Font12, BLACK, WHITE);
                iter = iter+1;
                i = 0;
            }else {
                Paint_DrawChar(110, 0, '+', &Font24, BLACK, WHITE);
            }
                
            if(DEV_Digital_Read(key0 ) == 0){
                Paint_DrawChar(115, 50, '-', &Font12, BLACK, WHITE);
                iter = iter-1;
                i = 0;
            }else {
                Paint_DrawChar(110, 45, '-', &Font24, BLACK, WHITE);
            }

            *minute = iter/40; // Adjust selection speed
            if(*minute > 59 || *minute < -59){*minute = 0; iter = 0;}
            else if(*minute < 0){*minute = 60+*minute;}

            // Blink numbers to be selected
            if(i/100 == 0 || i/100 == 1 || i/100 == 4 || i/100 == 5 || i/100 == 8 || i/100 == 9 || i/100 == 12 || i/100 == 13 || i/100 == 16 || i/100 == 17)
            {
                Paint_DrawString_EN(60, 20, formatted(*minute), &Font24, WHITE, BLACK);
            }
            Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
            Paint_DrawString_EN(15, 20, formatted(*hour), &Font24, WHITE, BLACK);
            Paint_DrawString_EN(15, 5, info, &Font12, WHITE, BLACK);

            OLED_1in3_C_Display(BlackImage);
            Paint_Clear(BLACK);
        }

        // Show result for a bit
        Paint_DrawString_EN(60, 20, formatted(*minute), &Font24, WHITE, BLACK);
        Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
        Paint_DrawString_EN(15, 20, formatted(*hour), &Font24, WHITE, BLACK);
        OLED_1in3_C_Display(BlackImage);
        Paint_Clear(BLACK);
        sleep_ms(1000);
    }
    sleeptime.durhour = sleeptime.wakehour - sleeptime.wakehour;
    if(sleeptime.durhour < 0){sleeptime.durhour = 0;}
    sleeptime.durminutes = sleeptime.wakeminutes - sleeptime.wakeminutes;
    if(sleeptime.durminutes < 0){sleeptime.durminutes = 0;}

    sleeptime.total = 60*sleeptime.durhour + sleeptime.durminutes;

    // Show time to sleep
    Paint_DrawString_EN(60, 20, formatted(sleeptime.durminutes), &Font24, WHITE, BLACK);
    Paint_DrawChar(45, 20, del_char, &Font24, BLACK, WHITE);
    Paint_DrawString_EN(15, 20, formatted(sleeptime.durhour), &Font24, WHITE, BLACK);
    Paint_DrawString_EN(15, 5, "Time to sleep:", &Font12, WHITE, BLACK);
    OLED_1in3_C_Display(BlackImage);
    Paint_Clear(BLACK);
    sleep_ms(2000);

    // Back to black
    OLED_1in3_C_Clear();

    return sleeptime.total;
}