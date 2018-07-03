
#if 0
#include "mbed.h"

Serial BT(D1, D0); //tx, rx
Serial pc(USBTX, USBRX);

DigitalOut RST(D10);
DigitalOut RTS(D9);

#define BUFFER_SIZE 256
#define URC_RECEIVED 8
#define BLE_ROLE  2
#define BAUD_RATE  115200

int Index;
bool urc_received_flag;
char REC_BUFFER[BUFFER_SIZE];

EventFlags urc_event;

void ble_callback_ex(void);
void ble_setup_up(int role,int baud);
void ble_setup_gatt_server(void);
void ble_connect(void);
void ble_send(int value);


int main(void) {

	int data = 65;

	Index = 0;
	urc_received_flag = false;
    pc.baud(BAUD_RATE);

    ble_setup_up(BLE_ROLE,BAUD_RATE);
    ble_setup_gatt_server();
	ble_connect();

    while(1)
    {
    	ble_send(data++);
        if(data == 91) {
        	data = 65;
        }
        wait_ms(2000);
    }
}

void ble_callback_ex(void) {
	REC_BUFFER[Index] = BT._getc();
	Index++;
	if(REC_BUFFER[Index-1] == '\n' && urc_received_flag == false) {
		REC_BUFFER[Index] = '\0';
		urc_received_flag = true;
		Index = 0;
		urc_event.set(URC_RECEIVED);
	}
}


/*
 * Configure baud rate
 * Set BLE sensor name
 * Enable peripheral role. 1: central, 2: peripheral, 3: both
 * Store configuration
 * Restart
 *
 */

void ble_setup_up(int role,int baud) {

	char  ATCmd[25] = {0};
	BT.attach(&ble_callback_ex);

    printf("Setup BLE Baud Rate = %d\n\n",baud);
    BT.baud(baud);

    printf("RTS low, RST high\n\n");
    RTS.write(0); // RTS to low
    RST = 1; //Reset pin to high
    wait_ms(2000);

    printf("Set name: mikroE\n");
    BT.puts("AT+UBTLN=\"mikroE\"\r"); //Set name to mikroE
    wait_ms(2000);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);

 	Index = 0;
    printf("Echo OFF\n");
    BT.puts("ATE0\r");
    wait_ms(200);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);


 	Index = 0;
    printf("Enable peripheral role\n");
	sprintf(ATCmd,"AT+UBTLE=%d\r",role);
    BT.puts(ATCmd);
    wait_ms(200);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);
    memset(ATCmd,0,sizeof(ATCmd));

    Index = 0;
    printf("Save configuration\n");
    BT.puts("AT&W\r");
    wait_ms(200);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);

 	Index = 0;
    printf("Restart\n");
    BT.puts("AT+CPWROFF\r");
    wait_ms(2000);
    wait_ms(2000);
    wait_ms(2000);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);
}


 //Setup Heart Rate service

void ble_setup_gatt_server(void) {

	Index = 0;
    printf("Heart Rate Service Started\n");
    BT.puts("AT+UBTGSER=180D\r");
    wait_ms(2000);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);


	Index = 0;
    printf("Heart Rate Characteristics Init\n");
    BT.puts("AT+UBTGCHA=2A37,10,1,1\r");
    wait_ms(2000);
    urc_event.wait_all(URC_RECEIVED);
    if (strstr(REC_BUFFER,"OK")) {
    	urc_received_flag = false;
        printf("Response: %s\n",REC_BUFFER);
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
    }
    urc_event.clear(URC_RECEIVED);
}


 //Wait for BLE device(Client) to connect

void ble_connect(void) {

    printf("Waiting for BLE device to connect ...\n");
    while(1){
    	Index = 0;
        urc_event.wait_all(URC_RECEIVED);
    	urc_received_flag = false;

        if (strstr(REC_BUFFER,"+UUBTACLC:")) {
            printf("Device Connected: %s\n",REC_BUFFER);
            memset(REC_BUFFER,0,sizeof(REC_BUFFER));
            urc_event.clear(URC_RECEIVED);
        	break;
        }
        memset(REC_BUFFER,0,sizeof(REC_BUFFER));
        urc_event.clear(URC_RECEIVED);
    }
}


 //Send data to BLE device

void ble_send(int value) {

	char  ATCmd[25] = {0};

	Index = 0;
	printf("Sending Data = %c\n\n",value);
	sprintf(ATCmd,"AT+UBTGSN=0,32,%x",value);
	strcat(ATCmd,"\r");

	BT.puts(ATCmd);
	urc_received_flag = false;
	memset(REC_BUFFER,0,sizeof(REC_BUFFER));
}

#else


//----------------- Using ATCmdParcer -----------------//

#include "mbed.h"
#include "ATCmdParser.h"



Serial pc(USBTX, USBRX);
FileHandle *_fh;
ATCmdParser *_at;

DigitalOut RST(D10);
DigitalOut RTS(D9);
DigitalOut led(LED1);

DigitalIn SW(SW0);

#define BUFFER_SIZE 256
#define URC_RECEIVED 8
#define BLE_ROLE  2
#define BAUD_RATE  115200

int Index;
bool urc_received_flag;
char REC_BUFFER[BUFFER_SIZE];

EventFlags urc_event;

void ble_callback(void);
void ble_setup_up(int role,int baud);
void ble_setup_gatt_server(void);
void ble_connect(void);
void ble_send(uint16_t value);
int read_at_to_char(char * buf, int size, char end);
int connect_wait=0;

int data = 89;

int main()
{
    pc.baud(BAUD_RATE);
	_fh = new UARTSerial(D1, D0, 115200);
	_at = new ATCmdParser(_fh, "\r", BUFFER_SIZE,8000,true);
    _at->oob("+UUBTACLC:", callback(&ble_callback));

	Index = 0;

	ble_setup_up(BLE_ROLE,BAUD_RATE);
	ble_setup_gatt_server();
	ble_connect();

	while(1);
}


void ble_setup_up(int role,int baud) {

    RTS.write(0); // RTS to low
    RST = 1; // Reset pin to high
    wait_ms(2000);

	printf("Configuring ...\n\n");

	if(_at->send("AT") && _at->recv("+STARTUP")) {
		printf("AT\n\n");
	}

//    wait_ms(4000);

	/*if (_at->send("ATS3=13\n") && _at->recv("OK")) {
		printf("ATS3\n\n");
	}
    wait_ms(2000);*/

//	if (_at->send("ATS4=10") && _at->recv("OK")) {
//		printf("ATS4\n\n");
//	}
//    wait_ms(2000);

	if (_at->send("AT+UBTLN=%s", "mikroE") && _at->recv("OK")) {
		printf("changed name to mikroE\n\n");
	}
    wait_ms(2000);

	if ( _at->send("ATE0") &&_at->recv("OK")) {
        printf("Echo OFF\n\n");
    }

    if (_at->send("AT+UBTLE=%d",role) && _at->recv("OK")) {
        printf("Enabled central role\n\n");
    }
    wait_ms(200);


    if (_at->send("AT&W") && _at->recv("OK")) {
        printf("Stored configuration\n\n");
    }
    wait_ms(200);

    if (_at->send("AT+CPWROFF") && _at->recv("OK")) {
        printf("Restarted\n\n");
    }
    wait_ms(6000);
}


void ble_setup_gatt_server() {

	if (_at->send("AT+UBTGSER=1815") && _at->recv("OK")) {
		printf("Automation_IO Service Started\n\n");
	}
    wait_ms(2000);

	if (_at->send("AT+UBTGCHA=2A56,10,1,1") && _at->recv("OK")) {
		printf("Digital Characteristics Init\n\n");
	}
    wait_ms(2000);
}


void ble_connect() {

    printf("Waiting for BLE device to connect ...\n");
	urc_received_flag = false;
    connect_wait=1;

    while(urc_received_flag == false){
    	_at->process_oob();
    }

    while(1){
    	if(SW==0)
    		data = 78;
    	else
    		data = 89;
    	ble_send(data);

//    	if(data >49)
//    		data = 48;
    	led = !led;
//    	data = !data;
//    	if(data == 91)
//    		data = 65;
    }
}

void ble_send(uint16_t value) {

	if (_at->send("AT+UBTGSN=0,32,%x",value) && _at->recv("OK")) {
		printf("Send Value = %d\n\n",value);
	}
    wait_ms(1000);
}


void ble_callback() {

	if(connect_wait==1){
		urc_received_flag = true;
    }
}

int read_at_to_char(char * buf, int size, char end)
{
    int count = 0;
    int x = 0;

    if (size > 0) {
        for (count = 0; (count < size) && (x >= 0) && (x != end); count++) {
            x = _at->getc();
            *(buf + count) = (char) x;
        }

        count--;
        *(buf + count) = 0;

        // Convert line endings:
        // If end was '\n' (0x0a) and the preceding character was 0x0d, then
        // overwrite that with null as well.
        if ((count > 0) && (end == '\n') && (*(buf + count - 1) == '\x0d')) {
            count--;
            *(buf + count) = 0;
        }
    }

    return count;
}

#endif
