/******************************************************************
file: final integrated code
author: Tarek Mahmoud Younes
brief: includes the final code for controlling both led and button
*******************************************************************/

/*************Include Section Start**********/

#include <avr/io.h> // Include AVR I/O register definitions
#include <util/delay.h> // Include delay functions
#include "Uart.h" // Include UART header file
#include "dio.h" // Include digital input/output header file

/*************Include Section End**********/

/*************Definition Section Start**********/

#define F_CPU 16000000UL // Define CPU frequency as 16 MHz for baud rate calculations
unsigned char LastButtonState = 0; // Variable to store the previous button state (pressed or released)
extern volatile unsigned char UartCommandReady; // External flag indicating if UART received a complete command
extern volatile char UartBuffer[]; // External UART buffer that stores received characters

/*************Definition Section End**********/

/*************main Funciton Start**********/

int main(void)
{
    UartInit(); // Initialize UART for serial communication
    DDRD = 0b10000000; // Set PD7 as output (LED) and all others as input
    
    unsigned char LedPosition = 7; // Define LED pin position (PD7)/D7
    unsigned char ButtonPosition = 4; //Define button pin position (PD4)/D4
    
    while (1)
    {
        if (UartCommandReady) // Check if a UART command has been received
        {
          LedControl(LedPosition); // Execute LED control based on received command
        }

        unsigned char CurrentButtonState = (PIND & (1 << ButtonPosition)) ? 1 : 0; // Read current button state (1 = pressed)


        if (CurrentButtonState != LastButtonState) // Check if button state has changed (press/release)
        {
            PushButtonSignal(ButtonPosition); // Send current button state via UART
            LastButtonState = CurrentButtonState; // Update last known button state
            _delay_ms(100); // Add debounce delay to apply debounce
        }
    }
}
/*************main Funciton End**********/