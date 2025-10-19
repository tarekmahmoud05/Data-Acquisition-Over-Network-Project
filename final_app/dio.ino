/******************************************************************
file: Digital input output code
author: Tarek Mahmoud Younes
brief: includes the defined functions of the led and button control
*******************************************************************/

/*************Include Section Start**********/

#include <avr/io.h> // Include AVR i/o definitions
#include <string.h> // Include string functions
#include "Uart.h" //Iclude UART header file
#include "dio.h" //Include dio header file

/*************Include Section End**********/

/*************Definition Section Start**********/

extern volatile char UartBuffer[]; // External UART buffer to store received data
extern volatile unsigned char UartCommandReady;// Flag indicating a complete UART command has been received

/*************Definition Section End**********/


/*************Defined Functions Section Start**********/

void LedControl(unsigned char LedPosition)// Function to control LED based on UART command
{
  if (UartCommandReady)// Check if a UART command has been received
  {
    UartCommandReady = 0; // Reset the command ready flag

    if (strcmp((char*)UartBuffer, "led_on") == 0)// Compare received command with "led_on"
    {
      PORTD |= (1 << LedPosition);// Turn ON LED by setting bit in PORTD
      UartSendString("led_on\n", 6);// Send confirmation message via UART
    }
    else if (strcmp((char*)UartBuffer, "led_off") == 0)// Compare received command with "led_off"
    {
      PORTD &= ~(1 << LedPosition);// Turn OFF LED by clearing bit in PORTD
      UartSendString("led_off\n", 7);// Send confirmation message via UART
    }
    else
    {
      UartSendString("Invalid_Input\n", 14);// Send invalid input message via UART
    } 
    for (int i = 0; i < CmdBufferSize; i++)// Loop through UART buffer
      UartBuffer[i] = '\0'; // Clear buffer contents
    UartIndex = 0;// Reset UART buffer index

  }
}

void PushButtonSignal(unsigned char ButtonPosition) // Function to monitor push button state
{ 
  if(PIND & (1<<ButtonPosition))// Check if button input pin is HIGH
  {
    UartSendString("button_pressed\n",0);// Send "button pressed" message via UART
  }
  else // If button input pin is LOW
  {
    UartSendString("button_released\n",0);   // Send "button released" message via UART
  }
}

/*************Defined Functions Section End**********/

