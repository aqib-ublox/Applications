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

static int a = 0;
DigitalOut led0(LED0);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut led5(LED5);
DigitalOut led6(LED6);

Serial pc(UART1_TX, UART1_RX);

Ticker flipper;

void gpio_led_toggle(void) {
	led0 = !led0;
	led1 = !led1;
	led2 = !led2;

	led4 = !led4;
	led5 = !led5;
	led6 = !led6;
	a++;

}


int main() {
	pc.printf("MicroSecond ticker demo started ...\r\n");
	led0 = 1;
	led1 = 0;
	led2 = 1;

	led3 = 0;

	led4 = 1;
	led5 = 0;
	led6 = 1;
	while(1)
	{
		flipper.attach(&gpio_led_toggle, 2.0); // the address of the function to be attached (flip) and the interval (2 seconds)
	}
    // spin in a main loop. flipper will interrupt it to call flip
    //while(1);
}
