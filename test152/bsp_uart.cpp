#include "bsp_uart.h"
#include "bsp_delay.h"

volatile uint16_t rx1_len=0;                                                                    //Received data length
volatile unsigned char  rx1_buf[USART1_BUF_SIZE]= {0}, usart1_recv_end_flag=0;                  //Receive buffer//Receive flag
volatile unsigned char  rx2_buf[USART2_BUF_SIZE]= {0}, usart2_recv_end_flag=1, rx2_len=0;

void bsp_UART1_Init(int baud) {
    Serial.begin(baud);
    delay_ms(50);
}
void UART1_EnRCV(void) {                    //Start serial port 1 to receive
    memset((char *)rx1_buf, '0', sizeof(char) * (USART1_BUF_SIZE - 1));
}
int UART1_getRcvFlag(void) {                //Determine the receiving flag
    return (Serial.available());
}
int UART1_dataPreProcess(void) {            //Data preprocessing
    int i = 0;
    memset((char *)rx1_buf, '0', sizeof(char) * (USART1_BUF_SIZE - 1));
    do {
        *(rx1_buf+i) = Serial.read();
        i++;
    } while (Serial.available());
    return i;
}


void bsp_UART2_Init(int baud) {
    Serial1.begin(baud);
}

void UART1_Put_Char(unsigned char ch) {
    Serial.write(ch);
}

void UART2_Put_Char(unsigned char ch) {
    Serial1.write(ch);
}
/////////////////////////////////////////////////////////////////////////////////////////////
void UART1_Init(void) {
    bsp_UART1_Init(115200);
    D_printf("Serial0 Init\n");
}
void UART1_Put_String(unsigned char s[]) {
    int i=0;
    while (s[i]!='\0') {
        UART1_Put_Char(s[i]);
        i++;
    }
}
void UART1_Send_Message(char s[], int size) {
    for (int i=0; i<size; i++) {
        UART1_Put_Char(s[i]);
    }
}

void UART2_Put_String(unsigned char s[]) {
    int i=0;
    while (s[i]!='\0') {
        UART2_Put_Char(s[i]);
        i++;
    }
}
void UART2_Send_Message(char s[], int size) {
    for (int i=0; i<size; i++) {
        UART2_Put_Char(s[i]);
    }
}

/*******************Module related functions*****************/
//Set the main sending and receiving parameters
void Set_A20(CHAN_ARV set, unsigned char sq) {
    unsigned char a002_send_buff[47]="AT+DMOSETGROUP=1,436.025,436.025,000,1,001,1\r\n";
    int i=0;
    D_printf("\nSET A20: TX:%f, RX:%f, TS:%d, RS:%d, SQL:%d, GBW:%d, POWER:%d\n",set.TX_FREQ, set.RX_FREQ, set.TS, set.RS, sq, set.GBW, set.POWER);
    if (set.GBW) {
        a002_send_buff[15]='1';
    } else {
        a002_send_buff[15]='0';
    }

    if (sq>8) {
        sq=1;
    }
    if (set.POWER) {
        set.POWER=1;
    }

    sprintf((char *)a002_send_buff+17,"%3.3f,%3.3f,%03d,%d,%03d,%d\r\n", set.TX_FREQ, set.RX_FREQ, set.RS, sq, set.TS, set.POWER);
    for (i=0; i<=45; i++) {
        UART2_Put_Char(a002_send_buff[i]);
    }
    delay_ms(100);
//  printf("%s\n", a002_send_buff);
//  printf("\nSET TIME: %d\n", key_timer_cnt1);

}

//Set control parameters
void Set_A20_MIC(unsigned char miclvl,unsigned char scramlvl,unsigned char tot) {
    unsigned char i=0,a002_send_buff[21]="AT+DMOSETMIC=1,0,0\r\n";
    //  printf("SETA002MIC: MIC:%d, SCRAM:%d, TOT:%d\n\n", miclvl, scramlvl, tot);
    a002_send_buff[13]=miclvl+'0';
    a002_send_buff[15]=scramlvl+'0';
    a002_send_buff[17]=tot+'0';

    for (i=0; i<=19; i++) {
        UART2_Put_Char(a002_send_buff[i]);
    }
}

//Get signal strength
int Get_A20_RSSI(void) {
    unsigned char i=0, a002_send_buff[17]="AT+DMOREADRSSI\r\n";
    int rssi=0, len=0;

    for (i=0; i<16; i++) {
        UART2_Put_Char(a002_send_buff[i]);
    }
    // delay_ms(20);

    while (!Serial1.available() && i++<200) {
        delay_ms(1);
    }
    D_printf("等待时间:%dms\n", i-16);        //Waiting time
    if (Serial1.available()) {
        i = 0;
        do {
            *(rx2_buf+i) = Serial1.read();
            i++;
        } while (Serial1.available());

        len = strlen((char *)rx2_buf) - 1;
// D_printf("Receive A20length:%d, %s\n", len, rx2_buf);          //receive A20 length:
// D_printf("Receive RSSI:%s\n", rx2_buf+13);                     //receive RSSI:

        if (len==15) {
            rssi = rx2_buf[13]-'0';
        } else if (len==16) {
            rssi = (rx2_buf[13]-'0')*10+(rx2_buf[14]-'0');
        } else if (len==17) {
            rssi = (rx2_buf[13]-'0')*100+(rx2_buf[14]-'0')*10+(rx2_buf[15]-'0');
        }

// D_printf("%dRSSI:%d\n", __LINE__,rssi);

        rssi=rssi*100/0xff;
        delay_ms(1);                                          //Can't calculate without delay！！！
        memset((char *)rx2_buf, 0, 255);
    }

    if (rssi<0) {
        rssi=0;
    }
    if (rssi>100) {
        rssi=100;
    }
    return rssi;
}

void A002_CALLBACK(void) {                                     //A20 Data reception and processing
    //if (usart2_recv_end_flag)
    if (Serial1.available()) {
        int i = 0;
        do {
            *(rx2_buf+i) = Serial1.read();
            i++;
        } while (Serial1.available());

        D_printf("length:%d, Serial1:%s\n", i, rx2_buf);

        if (strstr((const char *)rx2_buf, "+DMOCONNECT:0")) { //Handshake successfully
            D_printf("Connected!!\n");
        }
        if (strstr((const char *)rx2_buf, "+DMOSETGROUP:0")) { //Working parameters were written successfully
            D_printf("Successfully Write!\n");
        }
        if (strstr((const char *)rx2_buf, "+DMOSETMIC:0")) { //MIC Parameter setting is successful
            D_printf("Successfully Set MIC!\n");
        }
        if (strstr((const char *)rx2_buf, "+DMOSETVOX:0")) { //Voice control is set up successfully
            D_printf("Successfully Set VOX!\n");
        }
        if (strstr((const char *)rx2_buf, "+DMOAUTOPOWCONTR:0")) { //Power saving mode is successful
            D_printf("Successfully Set AUTOPOWCONTR!\n");
        }
        if (strstr((const char *)rx2_buf, "+ DMOSETVOLUME:0")) { //Volume settings is successful
            D_printf("Successfully Set VOLUME!\n");
        }

        // usart2_recv_end_flag = 0;
        memset((char *)rx2_buf, 0, sizeof(char) * 255);
        // UART2_Send_Message((char *)rx2_buf, USART2_BUF_SIZE);
    }
}
//


void disposeRx1(void) {
    usart1_recv_end_flag=1;
}

void diseposeRx2(void) {
    usart2_recv_end_flag=1;
}
