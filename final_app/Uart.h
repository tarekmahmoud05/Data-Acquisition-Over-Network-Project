#ifndef __UART__
#define __UART__

void UartInit(void);

void UartSetBaudRate(unsigned short BuadRate);

void UartSendChar(unsigned char DataByte);

unsigned char UartIsDataAvailable(void);

unsigned char UartReadData();

void UartSendString(char DataString[], unsigned char Size);

char* UARTReceiveString(void);


extern volatile char UartBuffer[];
extern volatile unsigned char UartCommandReady;

#endif