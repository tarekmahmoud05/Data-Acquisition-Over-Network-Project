*** Settings ***
Library    SerialLibrary
Library    OperatingSystem
Library    String             
Library    BuiltIn            
Library    Dialogs            
Suite Setup    Initialize Port    
Suite Teardown    Cleanup Port    

*** Variables ***
${PORT}    COM8   
${BAUDRATE}    9600
${TERMINATOR}    x           
${SPACE}    ${EMPTY} ${EMPTY}    

*** Keywords ***
Decode Serial Response
    [Arguments]    ${response_bytes}
    [Documentation]    Decodes the raw hex string response from the library into a readable string.
    # Fix for SyntaxError: Convert hex string with spaces to raw bytes object
    ${hex_string_clean}=    Replace String    ${response_bytes}    ${SPACE}    ${EMPTY}
    ${decoded_response}=    Evaluate    bytes.fromhex('${hex_string_clean}').decode('ascii').strip()
    [Return]    ${decoded_response}

Initialize Port
    [Documentation]    Adds the port configuration and opens the connection once per test suite.
    Add Port    ${PORT}    baudrate=${BAUDRATE}    encoding=ascii
    Open Port
    Log    Opened port ${PORT} at ${BAUDRATE}.
    Sleep    3s    # Give the AVR time to reset and initialize the UART

Cleanup Port
    [Documentation]    Closes the connection and attempts to remove the port configuration safely.
    Close Port
    # This safely attempts Remove Port. If the keyword is missing in your library,
    # the teardown will now succeed instead of failing the entire suite.
    Run Keyword And Ignore Error    Remove Port    ${PORT}

Run Serial Command
    [Arguments]    ${command_base}    ${expected_response}
    ${full_command}=    Set Variable    ${command_base}${TERMINATOR}

    # 1. WRITE: Convert the command string (e.g., "led_onx") to bytes and send it.
    ${command_bytes}=    Evaluate    '${full_command}'.encode('ascii')
    Write Data    ${command_bytes}
    Log    Sent Command: ${full_command}

    # 2. WAIT: Wait for the AVR to process the command
    Sleep    1s

    # 3. READ & DECODE: Get response, handle hex string format, decode, and strip newline.
    ${response_bytes}=    Read All Data
    ${decoded_response}=    Decode Serial Response    ${response_bytes}
    Log    Received Response: ${decoded_response}

    # 4. ASSERT: Verify the decoded response matches the expected string
    Should Be Equal    ${decoded_response}    ${expected_response}

Read Button Status Manually
    [Arguments]    ${action}    ${expected_status}
    [Documentation]    Reads the spontaneous output from the button polling loop, requiring user intervention.
    
    # 1. Read/flush any previous data before the manual step
    Read All Data
    
    # 2. PAUSE for manual action
    Pause Execution    *** MANUAL STEP REQUIRED ***\nPlease ${action} the button now and press Enter to continue.
    
    # 3. Wait briefly for the Arduino loop to detect the change and send the message
    Sleep    1s
    
    # 4. Read the new, spontaneous output
    ${response_bytes}=    Read All Data
    
    # Check if we received any data before trying to decode
    Run Keyword If    '${response_bytes}' == ''    Fail    No button status change detected after manual action.
    
    ${decoded_response}=    Decode Serial Response    ${response_bytes}
    Log    Received Button Status: ${decoded_response}
    
    # 5. ASSERT
    Should Contain    ${decoded_response}    ${expected_status}

*** Test Cases ***
Verify Led On Command
    [Documentation]    Sends 'led_onx' and expects 'led_on' response.
    Run Serial Command    led_on    led_on

Verify Led Off Command
    [Documentation]    Sends 'led_offx' and expects 'led_off' response.
    Run Serial Command    led_off    led_off

Verify Invalid Command
    [Documentation]    Sends an invalid command (ending in 'x') and expects 'Invalid_Input' response.
    Run Serial Command    garbage    Invalid_Input

Verify Button Pressed Status (MANUAL)
    [Documentation]    Requires pressing the physical button to trigger 'button_pressed'.
    Read Button Status Manually    **PRESS** the button and hold it    button_pressed

Verify Button Released Status (MANUAL)
    [Documentation]    Requires releasing the physical button to trigger 'button_released'.
    Read Button Status Manually    **RELEASE** the button    button_released
