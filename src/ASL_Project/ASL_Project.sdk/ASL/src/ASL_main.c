/*
 * ASL_main.c
 *
 *  Created on: Feb 29, 2016
 *      Author: Emily
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xparameters.h"
#include "xio.h"
#include "xtft.h"

#define DISPLAY_COLUMNS  640
#define DISPLAY_ROWS     480

//slvreg0[20:16] sw debugging; slvreg0[0] start
//slvreg1[11:0] sensor1; slvreg1[27:16] sensor2
//slvreg2[11:0] sensor3; slvreg2[27:16] sensor4
//slvreg3[11:0] sensor5
//slvreg4[8:0] x coord; slvreg4[18:10] y coord
//slvreg6[11] unknown, [10] valid, [6:0] letter
//slvreg7 intermediate letter output for debugging
volatile unsigned int * ASL_BASE_ADDR = (unsigned int *) 0x44A30000;
//slvreg0[2:0] address selection
//slvreg1[15:0] sensor data
volatile unsigned int * SENSOR_BASE_ADDR = (unsigned int *) 0x44A10000;
//slvreg0[8:0] x coord
//slvreg1[8:0] y coord
volatile unsigned int * ACCEL_BASE_ADDR = (unsigned int *) 0x44A40000;
//slvreg0 32 bit decimal number to display on hex
volatile unsigned int * HEX_BASE_ADDR = (unsigned int *) 0x44A20000;
//sw 0,1 resets
//sw 2 start
//sw 10 buttons
//sw 11,12 accel debugging; 1-x, 2-y
//sw 13,14,15 sensor debugging; 1-thumb, 2-index, 3-middle, 4-ring, 5-pinky
volatile unsigned int * SW_BASE_ADDR = (unsigned int *) 0x40000000;
volatile unsigned int * BUTTON_BASE_ADDR = (unsigned int *) 0x40010000;

//global vars
unsigned int sensor[5];
unsigned int x_coord;
unsigned int y_coord;
unsigned int button;
static XTft TftInstance;

/************************* SUBFUNCTIONS ***************************/
void small_delay (int number) {
	//input is number of seconds to delay
	int i;
	for (i = 0; i < number * 1000; i++) {
		;
	}
}

void delay (int number) {
	//input is number of seconds to delay
	int i;
	for (i = 0; i < number * 100000; i++) {
		;
	}
}

void writeSensor (int number) {
	print("DEBUG write sensor\n");
	int decimal;
	number = number >> 4;
	if (number >= 4093)
		decimal = 4096;
	else
		decimal = (number * 250000) >> 10;
	*HEX_BASE_ADDR = decimal;
	delay(1);
}

void writeHex (int number) {
	*HEX_BASE_ADDR = number;
	delay(1);
}

void getAccel() {
	x_coord = *(ACCEL_BASE_ADDR);
	y_coord = *(ACCEL_BASE_ADDR + 1);
	//xil_printf("x coord %0d  y coord %0d\n",x_coord,y_coord);
}

void getButton() {
	button = *(BUTTON_BASE_ADDR);
	//xil_printf("button %0d\n",button);
}

void getSensor() {
	int j, i;
	int sample = 50;
	for (j = 0; j < 4; j++) {
		for (i = 0; i < sample; i++) {

			//check each sensor value
			*SENSOR_BASE_ADDR = j;
			if (i == 0)
				sensor[j] = *(SENSOR_BASE_ADDR + 1);
			else
				sensor[j] += *(SENSOR_BASE_ADDR + 1);

			small_delay(1);
		}

		sensor[j] = sensor[j] / sample;

		xil_printf("sensor %0d val %0d  ",j,sensor[j]);
	}
	sensor[4] = 0;

	/*for (i = 0; i < sample; i++) {
		for (j = 0; j < 4; j++) {
			//check each sensor value
			*SENSOR_BASE_ADDR = j;
			if (i == 0)
				sensor[j] = *(SENSOR_BASE_ADDR + 1);
			else
				sensor[j] += *(SENSOR_BASE_ADDR + 1);
		}
		small_delay(1);
	}

	for (j = 0; j < 4; j++) {
		sensor[j] = sensor[j] / sample;
	}

	sensor[4] = 0;
	for (j = 0; j < 5; j++) {
		xil_printf("sensor %0d val %0d  ",j,sensor[j]);
	}*/
	xil_printf("\n");

	sensor[0] = sensor[0] - 15;
	sensor[1] = sensor[1] - 50;
	sensor[2] = sensor[2];
	sensor[3] = sensor[3] - 20;
	/*sensor[0] = sensor[0] * 2 - 188;
	sensor[1] = sensor[1] * 4 - 656;
	sensor[2] = sensor[2] * 6 - 792;
	sensor[3] = sensor[3] * 4.6 - 657;*/

	for (j = 0; j < 5; j++) {
		xil_printf("sensor %0d val %0d  ",j,sensor[j]);
	}
	xil_printf("\n\n");
}

void checkSwitches() {
	//print("DEBUG checking switches\n");
	//check switches
	//ON BOARD DEBUGGING
	int sw, sw_sensor, sw_accel, sw_button, read_value;
	sw = *(SW_BASE_ADDR);
	sw_sensor = (sw >> 4) & ((1 << 3) - 1); //bits 7-5 sw 15-13
	sw_accel = (sw >> 2) & ((1 << 2) - 1); //bits 4-3 sw 12-11
	sw_button = (sw >> 1) & 1; //bit 2 sw 10
	//DEBUG
	/*writeHex(sw * 4);
	writeHex(sw_button);
	writeHEX(11111);
	writeHex(sw_sensor);
	writeHEX(11111);
	writeHex(sw_accel);
	writeHEX(11111);
	*/
	if (sw_button != 0) {
		//write to slvreg0, keep start value
		read_value = *(BUTTON_BASE_ADDR);
		xil_printf("Read from button is %0d\n",read_value);
		writeHex(11111);
		writeHex(read_value);
	} else if (sw_sensor != 0) {
		if (sw_sensor <= 5) {
			int addr = sw_sensor - 1;
			xil_printf("addr is %0d\n",addr);
			*SENSOR_BASE_ADDR = addr;
			read_value = *(SENSOR_BASE_ADDR + 1);
			xil_printf("Read from ASL %0d is %0d\n",sw_sensor,read_value);
			writeHex(11111);
			writeHex(read_value);
		}
	} else if (sw_accel != 0) {
		if (sw_accel == 1) {
			read_value = *(ACCEL_BASE_ADDR);
			xil_printf("Read from accel x is %0d\n",read_value);
			writeHex(11111);
			writeHex(read_value);
		} else {
			read_value = *(ACCEL_BASE_ADDR + 1);
			xil_printf("Read from accel y is %0d\n",read_value);
			writeHex(11111);
			writeHex(read_value);
		}
	}
}

void init_vga() {
	int Status;
	XTft_Config *TftConfigPtr;

	TftConfigPtr = XTft_LookupConfig(XPAR_AXI_TFT_0_DEVICE_ID);
	if (TftConfigPtr == (XTft_Config *)NULL) {
		return XST_FAILURE;
	}

	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr,
				 	TftConfigPtr->BaseAddress);
	//xil_printf("BaseAddr = %x", TftConfigPtr->BaseAddress);
	//xil_printf("instance = %x", TftInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XTft_SetPosChar(&TftInstance, 50, 50);
	xil_printf("  Display color ");
	xil_printf("\r\n");
}

void clear_screen() {
	//XTft_SetColor(&TftInstance, 0, 0);
    XTft_ClearScreen(&TftInstance); //clear screen
	XTft_SetPosChar(&TftInstance, 50, 50);
	xil_printf("clear_screen\n");
}

void vga_write(int num) {
	//num is ascii of character to write
	XTft_Write(&TftInstance,num);
	xil_printf("write letter %c \n",num);
}

void vga_test() {
	init_vga();
	vga_write(65);
	vga_write(66);
	vga_write(67);
	vga_write(68);
	vga_write(69);
	vga_write(70);
	delay(2000);
	clear_screen();
	vga_write(71);
	vga_write(73);
	vga_write(75);
	vga_write(32);
	vga_write(77);
	vga_write(79);
	vga_write(80);
}

/************************* MAINFUNCTION ***************************/

int main() {
	//variables

	int check_sensor, ASL_ctrl, wait_counter, letter;
	int i, j;

	check_sensor = 0;
//	vga_test();

	init_vga();

	while(1) {

		//check sw
		check_sensor = *(SW_BASE_ADDR) & 1;

		if (check_sensor) {

			//initialize sensor to be stable
			//xil_printf("checking sensor; start calculation\n");

			//get data
			getAccel();
			getSensor();
			getButton();

			//write to ASL
			for (j = 1; j <= 5; j++) //slv regs 1-5
			*(ASL_BASE_ADDR + 1) = (sensor[1] * 65536) + (sensor[0] % 65536); //write sensor1,2 data
			*(ASL_BASE_ADDR + 2) = (sensor[3] * 65536) + (sensor[2] % 65536); //write sensor3,4 data
			*(ASL_BASE_ADDR + 3) = (sensor[4] % 65536); //write sensor5 data
			*(ASL_BASE_ADDR + 4) = (x_coord % 512) + (y_coord * 1024); //write accel data
			*(ASL_BASE_ADDR + 5) = button; //write button data
			*ASL_BASE_ADDR = 1; //start = 1

			xil_printf("write data to ASL IP\n");

			//read from ASL
			wait_counter = 0;
			while (*(ASL_BASE_ADDR + 6) == 0) {
				//wait for valid
				wait_counter++;
				if (wait_counter > 10000) {
					xil_printf("ERROR Hardware not responding\n");
					break;
				}
			} // while wait for valid

			ASL_ctrl = *(ASL_BASE_ADDR + 6); // get outputs from ASL
			//writeHex(77777);//DEBUG
			//delay(3);//DEBUG
			//writeHex(ASL_ctrl);//DEBUG
			if ((ASL_ctrl / 2048) >= 1) //letter unknown
				xil_printf("Letter unknown\n");
			else if ((ASL_ctrl / 1024) >= 1) { //letter valid
				letter = ASL_ctrl % 1024; //extract last 10 bits of ASL control register
				writeHex(77777777);//DEBUG
				writeHex(letter);
				xil_printf("Letter is %0d\n",letter);
				//WRITE VGA
				if (letter == 0) //unknown
					xil_printf("Letter unknown\n");
				else if (letter == 1) //clear
					clear_screen();
				else
					vga_write(letter);
			}
			else {
				xil_printf("ERROR return value %0d invalid\n",ASL_ctrl);
				writeHex(7777);//DEBUG
			}

			//reset ASL start to 0
			*ASL_BASE_ADDR = 0; //start = 0

			//make sure sensor becomes unstable first before checking for next letter
			//use switches for start
			while (check_sensor) {
				check_sensor = *(SW_BASE_ADDR) & 1;
				checkSwitches();
			}
		} // if check sensor
		checkSwitches();
	} // while 1
	return 0;
}

/*int sensorStable() {
	int j, sensor_stable = 1;
	unsigned int sensor_new;
	int sensor_diff;
	//for (j = 0; j < 5; j++) {
		for (j = 0; j < 4; j++) {
		//check each sensor value
		*SENSOR_BASE_ADDR = j;
		sensor_new = *(SENSOR_BASE_ADDR + 1);
		writeHex(22222222);//DEBUG
		writeHex(j);//DEBUG
		writeSensor(sensor[j]); //DEBUG
		delay(2);//DEBUG
		writeSensor(sensor_new); //DEBUG
		delay(2);//DEBUG

		sensor_diff = sensor[j] - sensor_new;
		if (sensor_diff < 0)
			sensor_diff *= -1;
		writeHex(44444);//DEBUG
		writeHex(sensor_diff);//DEBUG
		//delay(2);//DEBUG
		//writeHex(22222);//DEBUG
		if (sensor_diff > SENSOR_THRESHOLD) {
			//sensor movement too big
			sensor_stable = 0;
		}
		sensor[j] = sensor_new;
	}
	return sensor_stable;
}*/
