/* mbed Example Program
 * Copyright (c) 2006-2014 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"

DigitalOut led1(LED1);
DigitalIn  SW_Read(SW0);
static void watchdog_init(void);
Serial pc(USBTX, USBRX);

//WWDG_HandleTypeDef WwdgHandle;
IWDG_HandleTypeDef  IwdgHandle;
int main() {
	pc.printf("WWD timer demo started ...\r\n");
	led1 = 1;
    int a = 0;
    
    watchdog_init();
    while(1)
    {
        if(SW_Read == 1)
        {
            led1 = 0;
            HAL_IWDG_Refresh(&IwdgHandle);
            a =1;
        }
        else{
            a=2;
        }
    }
}

static void watchdog_init(void)
{
    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
    IwdgHandle.Init.Reload = 0x0FFF;

    IwdgHandle.Instance = IWDG;
    
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
     /* Clear reset flags */
      __HAL_RCC_CLEAR_RESET_FLAGS();

    if(HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
    {
      /* Initialization Error */
       // Error_Handler();  
    }

}
// static void watchdog_init(void)
// {
     // /*##-1- Check if the system has resumed from WWDG reset ####################*/
    // if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
         // /* Clear reset flags */
          // __HAL_RCC_CLEAR_RESET_FLAGS();

    // /*##-2- Configure the WWDG peripheral ######################################*/
    // /* WWDG clock counter = (PCLK1 (2MHz)/4096)/1) = 488 Hz (~2049 us)
     // WWDG Window value = 80 means that the WWDG counter should be refreshed only
     // when the counter is below 80 (and greater than 64) otherwise a reset will
     // be generated.
     // WWDG Counter value = 127, WWDG timeout = ~2049 us * 64 = 131.13 ms
     // In this case the refresh window is: ~2049 * (127-80) = 96.303 ms < refresh window < ~2049 * 64 = 131.13ms */
    // WwdgHandle.Instance = WWDG;

    // WwdgHandle.Init.Prescaler = WWDG_PRESCALER_1;
    // WwdgHandle.Init.Window    = 100;
    // WwdgHandle.Init.Counter   = 127;

    // if(HAL_WWDG_Init(&WwdgHandle) != HAL_OK)
    // {
        // /* Initialization Error */
        // //Error_Handler();
    // }

    // /*##-3- Start the WWDG #####################################################*/
    // //if(HAL_WWDG_Start(&WwdgHandle) != HAL_OK)
    // //{
    // //    //Error_Handler();
    // //}
// }
