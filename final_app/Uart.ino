/******************************************************************
file: UART Functions Code
author: Tarek Mahmoud Younes
brief: includes definitions of the only used UART functions in the project 
*******************************************************************/

/*************Include Section Start**********/

#if !defined(__AVR_ATmega328P__)// Check if the target MCU is not ATmega328P
#include <avr/iom328p.h>// Include ATmega328P register definitions manually if not defined
#endif
#include <avr/io.h>// Include standard AVR I/O definitions
#include <avr/interrupt.h>// Include interrupt handling functions 
#include <string.h> // Include string handling functions
#include "Uart.h"// Include custom UART function declarations

/*************Include Section End**********/

/*************Definition Section Start**********/

#define CmdBufferSize 20 // Define the maximum UART command buffer size
volatile char UartBuffer[CmdBufferSize]; // Global buffer to store received UART data
volatile unsigned char UartIndex = 0; // Index to track the position in the buffer
volatile unsigned char UartCommandReady = 0;// Flag to indicate when a full command has been received

/*************Definition Section End**********/

/*************Defined Functions Section Start**********/

void UartInit(void) // Function to initialize UART communication
{
    UartSetBaudRate(9600); // Set UART baud rate to 9600 bps
    UCSR0B = (1 << TXEN0) | (1 << RXEN0)| (1 << RXCIE0);// Enable TX, RX, and RX interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Enable 8-bit mode
    sei(); // Enable global interrupts
}

void UartSetBaudRate(unsigned short BuadRate) // Function to set UART baud rate dynamically
{
    unsigned short UBBR = ( (F_CPU / 16) /  BuadRate ) - 1; // Calculate baud rate register value
    UBRR0L = (UBBR & 0xFF); // Load lower 8 bits of UBRR value
    UBRR0H = (( UBBR >> 8 ) & 0xFF); // Load higher 8 bits of UBRR value
}

void UartSendChar(unsigned char DataByte) // Function to send a single byte via UART
{
    while ( ! (UCSR0A & ( 1 << UDRE0)) ); // Wait until transmit buffer (UDR0) is empty
    UDR0 = DataByte; // Load data byte into UART data register for transmission
}

unsigned char UartReadData(void)  // Function to read a received byte from UART
{
    while ( ! (UCSR0A & ( 1 << RXC0)) ); // Wait until a byte is received
    return UDR0; // Return the received data from UART data register
}

void UartSendString(char DataString[], unsigned char Size) // Function to send a string via UART
{
    int i;
    for (i=0; DataString[i]; i++) // Loop through each character in the string
    {
        UartSendChar(DataString[i]); // Send each character individually
    }
}

ISR(USART_RX_vect) // Interrupt Service Routine for UART Receive Complete
{
    char Data = UDR0; // Read received byte from UART data register

    if (Data == 'x') // Check if the character 'x' indicates end of command
    {
        UartBuffer[UartIndex] = '\0'; // Terminate the command string with null character
        UartIndex = 0; // Reset buffer index to start storing new data
        UartCommandReady = 1; // Set flag to indicate a complete command has been received
    }
    else if (UartIndex < CmdBufferSize - 1) // Ensure buffer doesn't overflow
    {
        UartBuffer[UartIndex++] = Data; // Store received character and increment index
    }
}

/*************Defined Functions Section End**********/
