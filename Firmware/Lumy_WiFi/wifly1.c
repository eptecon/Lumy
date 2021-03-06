/************************************************************************
Title:    Wifly module (RN-VX) library
Author:   EP
File:     $Id: wifly.c,v 1.0.0.0 2012/11/16 20:14:38  $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR, tested on ATmega328P at 8 Mhz
License:  GNU General Public License 
Usage:    see Wifly manual

LICENSE:
    Copyright (C) 2006 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
************************************************************************/

/************************************************************************
Changelog for modifications. 

Date        Description
=========================================================================
16/11/2012  Created

************************************************************************/

#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
#include "uart.h"
#include "wifly1.h"

/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* 9600 baud */
#define UART_BAUD_RATE	9600      


// Strings stored in Program space
const char s_WIFLYDEVICE_LIBRARY_VERSION[] PROGMEM = "WiFlySerial v1.00";   
const char s_WIFLYDEVICE_JOIN[] PROGMEM = "join ";   
const char s_WIFLYDEVICE_OPEN[] PROGMEM = "open ";   
const char s_WIFLYDEVICE_CLOSE[] PROGMEM = "close";   
const char s_WIFLYDEVICE_ASSOCIATED[] PROGMEM = "associated";   
const char s_WIFLYDEVICE_ATTN[] PROGMEM = "$$$";   
const char s_WIFLYDEVICE_VER[] PROGMEM = "ver";
const char s_WIFLYDEVICE_LEAVE_CMD_MODE[] PROGMEM ="exit";
const char s_WIFLYDEVICE_REBOOT[] PROGMEM ="reboot";
const char s_WIFLYDEVICE_SAVE[] PROGMEM ="save";
const char s_WIFLYDEVICE_GET_MAC[] PROGMEM =" get mac";
const char s_WIFLYDEVICE_GET_MAC_ADDR[] PROGMEM ="Addr=";
const char s_WIFLYDEVICE_GET_IP[] PROGMEM =" get ip";
const char s_WIFLYDEVICE_GET_GW[] PROGMEM = " "; // "GW=";
const char s_WIFLYDEVICE_GET_NM[] PROGMEM = " "; // "NM=";
const char s_WIFLYDEVICE_LEAVE[] PROGMEM ="leave";
const char s_WIFLYDEVICE_SET_SSID[] PROGMEM =" set wlan s ";
const char s_WIFLYDEVICE_SET_CHANNEL[] PROGMEM =" set wlan c ";
const char s_WIFLYDEVICE_SET_WIFI_AUTH[] PROGMEM =" set wlan a ";
const char s_WIFLYDEVICE_SET_WIFI_JOIN[] PROGMEM =" set wlan j ";
const char s_WIFLYDEVICE_SET_PASSPHRASE[] PROGMEM =" set w p ";
const char s_WIFLYDEVICE_NETWORK_SCAN[] PROGMEM ="scan ";
const char s_WIFLYDEVICE_AOK[] PROGMEM ="";
const char s_WIFLYDEVICE_SET_UART_BAUD[] PROGMEM ="set u b 9600 ";
const char s_WIFLYDEVICE_DEAUTH[] PROGMEM ="Deauth";
const char s_WIFLYDEVICE_SET_NTP[] PROGMEM =" set time a ";
const char s_WIFLYDEVICE_SET_NTP_ENABLE[] PROGMEM ="set time e ";
const char s_WIFLYDEVICE_SET_DEVICEID[] PROGMEM ="set opt deviceid ";
const char s_WIFLYDEVICE_IP_DETAILS[] PROGMEM ="get ip";
const char s_WIFLYDEVICE_GET_DNS_DETAILS[] PROGMEM ="get dns";
const char s_WIFLYDEVICE_GET_TIME[] PROGMEM ="show t t";
const char s_WIFLYDEVICE_SET_DHCP[] PROGMEM ="set ip dhcp ";
const char s_WIFLYDEVICE_SET_IP[] PROGMEM ="set ip a ";
const char s_WIFLYDEVICE_SET_NETMASK[] PROGMEM ="set ip n ";
const char s_WIFLYDEVICE_SET_GATEWAY[] PROGMEM ="set ip g ";
const char s_WIFLYDEVICE_SET_DNS[] PROGMEM ="set dns addr ";
const char s_WIFLYDEVICE_SET_LOCAL_PORT[] PROGMEM ="set ip local ";
const char s_WIFLYDEVICE_SET_REMOTE_PORT[] PROGMEM ="set ip remote  ";
const char s_WIFLYDEVICE_SET_PROTOCOL[] PROGMEM ="set ip proto ";
const char s_WIFLYDEVICE_ERR_REBOOOT[] PROGMEM ="Attempting reboot";
const char s_WIFLYDEVICE_ERR_START_FAIL[] PROGMEM ="Failed to get cmd prompt:Halted.";
const char s_WIFLYDEVICE_SET_UART_MODE[] PROGMEM ="set u m 1 ";
const char s_WIFLYDEVICE_GET_WLAN[] PROGMEM ="get wlan ";
const char s_WIFLYDEVICE_GET_RSSI[] PROGMEM ="show rssi ";
const char s_WIFLYDEVICE_GET_BATTERY[] PROGMEM ="show batt ";
const char s_WIFLYDEVICE_GET_STATUS[] PROGMEM ="show conn ";
const char s_WIFLYDEVICE_RETURN[] PROGMEM ="\r";
const char s_WIFLYDEVICE_GET_IP_IND[] PROGMEM ="IP=";
const char s_WIFLYDEVICE_GET_NM_IND[] PROGMEM ="NM=";
const char s_WIFLYDEVICE_GET_GW_IND[] PROGMEM ="GW=";
const char s_WIFLYDEVICE_GET_DNS_IND[] PROGMEM ="DNS=";
const char s_WIFLYDEVICE_GET_WLAN_SSID_IND[] PROGMEM ="SSID=";
const char s_WIFLYDEVICE_GET_RSSI_IND[] PROGMEM ="RSSI=";
const char s_WIFLYDEVICE_GET_WLAN_DEV_IND[] PROGMEM ="DeviceID=";
const char s_WIFLYDEVICE_GET_BATTERY_IND[] PROGMEM ="Batt=";
const char s_WIFLYDEVICE_GET_TIME_IND[] PROGMEM ="RTC=";
const char s_WIFLYDEVICE_GET_STATUS_IND[] PROGMEM ="8";
const char s_WIFLYDEVICE_GET_IP_UP_IND[] PROGMEM ="F=";

// Index of strings
#define STI_WIFLYDEVICE_INDEX_JOIN        0
#define STI_WIFLYDEVICE_INDEX_ASSOCIATED  1
#define STI_WIFLYDEVICE_ATTN              2
#define STI_WIFLYDEVICE_VER               3 
#define STI_WIFLYDEVICE_GET_MAC           4 
#define STI_WIFLYDEVICE_GET_IP            5 
#define STI_WIFLYDEVICE_GET_GW            6 
#define STI_WIFLYDEVICE_GET_NM            7 
#define STI_WIFLYDEVICE_LEAVE             8 
#define STI_WIFLYDEVICE_SET_SSID          9
#define STI_WIFLYDEVICE_SET_PASSPHRASE    10
#define STI_WIFLYDEVICE_NETWORK_SCAN      11
#define STI_WIFLYDEVICE_AOK               12
#define STI_WIFLYDEVICE_SET_UART_BAUD     13
#define STI_WIFLYDEVICE_DEAUTH            14
#define STI_WIFLYDEVICE_SET_NTP           15
#define STI_WIFLYDEVICE_SET_NTP_ENABLE    16
#define STI_WIFLYDEVICE_SET_DEVICEID      17
#define STI_WIFLYDEVICE_GET_IP_DETAILS    18
#define STI_WIFLYDEVICE_LEAVE_CMD_MODE    19
#define STI_WIFLYDEVICE_GET_DNS_DETAILS   20
#define STI_WIFLYDEVICE_GET_TIME          21
#define STI_WIFLYDEVICE_SET_DHCP          22
#define STI_WIFLYDEVICE_SET_IP            23
#define STI_WIFLYDEVICE_SET_NETMASK       24
#define STI_WIFLYDEVICE_SET_GATEWAY       25
#define STI_WIFLYDEVICE_SET_DNS           26
#define STI_WIFLYDEVICE_ERR_REBOOT        27
#define STI_WIFLYDEVICE_ERR_START_FAIL    28
#define STI_WIFLYDEVICE_SET_UART_MODE     29
#define STI_WIFLYDEVICE_GET_WLAN          30
#define STI_WIFLYDEVICE_GET_RSSI          31
#define STI_WIFLYDEVICE_GET_BATTERY       32
#define STI_WIFLYDEVICE_LIBRARY_VERSION   33
#define STI_WIFLYDEVICE_SET_CHANNEL       34
#define STI_WIFLYDEVICE_SET_WIFI_AUTH     35
#define STI_WIFLYDEVICE_SET_WIFI_JOIN     36
#define STI_WIFLYDEVICE_GET_STATUS        37
#define STI_WIFLYDEVICE_GET_MAC_ADDR      38
#define STI_WIFLYDEVICE_RETURN            39
#define STI_WIFLYDEVICE_GET_IP_IND        40
#define STI_WIFLYDEVICE_GET_NM_IND        41
#define STI_WIFLYDEVICE_GET_GW_IND        42
#define STI_WIFLYDEVICE_GET_DNS_IND       43
#define STI_WIFLYDEVICE_GET_WLAN_SSID_IND 44
#define STI_WIFLYDEVICE_GET_RSSI_IND      45
#define STI_WIFLYDEVICE_GET_BATTERY_IND   46
#define STI_WIFLYDEVICE_GET_WLAN_DEV_IND  47
#define STI_WIFLYDEVICE_GET_TIME_IND      48
#define STI_WIFLYDEVICE_GET_STATUS_IND    49
#define STI_WIFLYDEVICE_GET_IP_UP_IND     50
#define STI_WIFLYDEVICE_OPEN              51
#define STI_WIFLYDEVICE_REBOOT            52
#define STI_WIFLYDEVICE_CLOSE             53
#define STI_WIFLYDEVICE_SET_LOCAL_PORT    54
#define STI_WIFLYDEVICE_SET_REMOTE_PORT   55
#define STI_WIFLYDEVICE_SET_PROTOCOL      56
#define STI_WIFLYDEVICE_SAVE              57


// String Table in Program space
const char * const WiFlyDevice_string_table[] PROGMEM= 	   
{ 
  // 0-based index, see STI_WIFLY_DEVICE_ list above.  
  s_WIFLYDEVICE_JOIN,
  s_WIFLYDEVICE_ASSOCIATED,
  s_WIFLYDEVICE_ATTN,
  s_WIFLYDEVICE_VER,
  s_WIFLYDEVICE_GET_MAC,
  s_WIFLYDEVICE_GET_IP,
  s_WIFLYDEVICE_GET_GW,
  s_WIFLYDEVICE_GET_NM,
  s_WIFLYDEVICE_LEAVE,
  s_WIFLYDEVICE_SET_SSID,
  // 10 follows
  s_WIFLYDEVICE_SET_PASSPHRASE,
  s_WIFLYDEVICE_NETWORK_SCAN,
  s_WIFLYDEVICE_AOK,
  s_WIFLYDEVICE_SET_UART_BAUD,
  s_WIFLYDEVICE_DEAUTH,
  s_WIFLYDEVICE_SET_NTP,
  s_WIFLYDEVICE_SET_NTP_ENABLE,
  s_WIFLYDEVICE_SET_DEVICEID,
  s_WIFLYDEVICE_IP_DETAILS,
  s_WIFLYDEVICE_LEAVE_CMD_MODE,  
  // 20 follows
  s_WIFLYDEVICE_GET_DNS_DETAILS,
  s_WIFLYDEVICE_GET_TIME,
  s_WIFLYDEVICE_SET_DHCP,
  s_WIFLYDEVICE_SET_IP,
  s_WIFLYDEVICE_SET_NETMASK,
  s_WIFLYDEVICE_SET_GATEWAY,
  s_WIFLYDEVICE_SET_DNS,
  s_WIFLYDEVICE_ERR_REBOOOT,
  s_WIFLYDEVICE_ERR_START_FAIL,
  s_WIFLYDEVICE_SET_UART_MODE,
  // 30 follows
  s_WIFLYDEVICE_GET_WLAN,
  s_WIFLYDEVICE_GET_RSSI,
  s_WIFLYDEVICE_GET_BATTERY,
  s_WIFLYDEVICE_LIBRARY_VERSION,
  s_WIFLYDEVICE_SET_CHANNEL,
  s_WIFLYDEVICE_SET_WIFI_AUTH,
  s_WIFLYDEVICE_SET_WIFI_JOIN,
  s_WIFLYDEVICE_GET_STATUS,
  s_WIFLYDEVICE_GET_MAC_ADDR,
  s_WIFLYDEVICE_RETURN,
  // 40 follows
  s_WIFLYDEVICE_GET_IP_IND,
  s_WIFLYDEVICE_GET_NM_IND,
  s_WIFLYDEVICE_GET_GW_IND,
  s_WIFLYDEVICE_GET_DNS_IND,
  s_WIFLYDEVICE_GET_WLAN_SSID_IND,
  s_WIFLYDEVICE_GET_RSSI_IND,
  s_WIFLYDEVICE_GET_BATTERY_IND,
  s_WIFLYDEVICE_GET_WLAN_DEV_IND,
  s_WIFLYDEVICE_GET_TIME_IND,
  s_WIFLYDEVICE_GET_STATUS_IND,
  // 50 follows
  s_WIFLYDEVICE_GET_IP_UP_IND,
  s_WIFLYDEVICE_OPEN,
  s_WIFLYDEVICE_REBOOT,
  s_WIFLYDEVICE_CLOSE,
  s_WIFLYDEVICE_SET_LOCAL_PORT,
  s_WIFLYDEVICE_SET_REMOTE_PORT,
  s_WIFLYDEVICE_SET_PROTOCOL,
  s_WIFLYDEVICE_SAVE
};




/* WiFly-specific prompt codes */
const char* WiFlyFixedPrompts[] = { "","AOK", "CMD", "ERR: ?", "",">","*CLOS*","*OPEN*","ERR:Connected" };
static unsigned int WiFlyFixedFlags[N_PROMPTS] = {PROMPT_EXPECTED_TOKEN_FOUND, PROMPT_AOK, PROMPT_CMD_MODE, PROMPT_CMD_ERR, PROMPT_READY, PROMPT_READY, PROMPT_CLOSE, PROMPT_OPEN, PROMPT_OPEN_ALREADY};

/* Internal status flags */
unsigned char bWiFlyInCommandMode;
unsigned char bWiFlyConnectionOpen;
unsigned long    fStatus;
char*   pControl;


/* internal buffer for command-prompt */
char    szWiFlyPrompt[INDICATOR_BUFFER_SIZE ];


/* Ports for connections */
unsigned int     iRemotePort;
unsigned int     iLocalPort;
unsigned long 	 lUTC_Offset_seconds;




/*************************************************************************
 Utility Functions
**************************************************************************/

/* WFSIPArrayToStr*/
/* Converts IPArray to a character string representation for WiFlySerial*/
/*																					*/
/* pIP    pointer to array of 4 ints representing ip address e.g. {192,168,1,3}*/
/* pStr   Destination buffer of at least 16 characters "192.168.1.3"*/
/* returns 0 on success*/
/*													*/
/* Note: could be enhanced to support IPv6*/
/*															*/
/* Convert a Buffer holding an IP address to a byte array.*/
/* Minimal safety checks - be careful!*/
/*char* BufferToIP_Array(char* pBuffer, uint8_t* pIP) {

  char* posStart=0;
  char* posEnd=0;
  char alphabuf[IP_ADDR_WIDTH];

  posStart = pBuffer;
  for (int i = 0; i<UC_N_IP_BYTES ; i++) {
    memset(alphabuf,'\0',IP_ADDR_WIDTH);
    posEnd = strchr(posStart,'.');
    if (posEnd == NULL) {
      posEnd = strchr(posStart,'\0');
    }
    strncpy(alphabuf, posStart, posEnd-posStart);
    
    pIP[i] =  atoi( alphabuf );
    // Start looking one after last dot.
    posStart = posEnd +1;
  }
  return pIP;
}*/


/* Convert a Buffer holding an IP address to a byte array.*/
/* Minimal safety checks - be careful!*/
/*char* IP_ArrayToBuffer( const uint8_t* pIP, char* pBuffer, int buflen) {

  memset (pBuffer,'\0',buflen);
 
    for (int i =0; i< UC_N_IP_BYTES; i++) {
      itoa( (int) pIP[i], strchr(pBuffer,'\0'), 10);
      if (i < UC_N_IP_BYTES -1 ) {
        strcat(pBuffer, ".");
      }
    }
    
  return pBuffer;
}*/


/*************************************************************************
Command and Response
WiFly provides one of three results from commands:
1) ERR: Bad Args  , from malformed commands.
2) AOK , from accepted commands
3) nothing, after an inquiries' response.

Some commands will provide specific messages
e.g. join  has a possible result of
 mode=WPA1 SCAN OK followed by 'Associated!' and by ip values.
(bad SSID)  mode=NONE FAILED
(bad pwd)  mode=WPA1 SCAN OK followed by 'Disconn ...  AUTH-ERR'
           and followed by 'Disconn from <SSID>'
           
after a successful join, a 'cr' is needed to get the prompt

The 'command prompt' is currently the version number in angle-brackets e.g. <2.21>
**************************************************************************/


/*************************************************************************
 WiFlySerial
 Initializer for WiFlySerial library
										
 Parameters:
 pinReceive      receive pin to WiFly's TX pin
 pinSend         send pin to WiFly's RX pin
																
 Returns:  N/A
**************************************************************************/
void WiFlySerial(unsigned char pinReceive, unsigned char pinSend) 
{
  /* Set initial values for flags. */ 
  /* On startup, WiFly state not known. */
  bWiFlyInCommandMode = FALSE;
  bWiFlyConnectionOpen = FALSE;
  fStatus = WIFLY_STATUS_OFFLINE ;
  strcpy(szWiFlyPrompt, WiFlyFixedPrompts[WIFLY_MSG_PROMPT2] );  /* ">" */
  
  iLocalPort = WIFLY_DEFAULT_LOCAL_PORT;
  iRemotePort = WIFLY_DEFAULT_REMOTE_PORT;
  
  /* default is UTC timezone */
  lUTC_Offset_seconds = 0;
  
  /* ensure a default sink. */
//pDebugChannel = NULL;
  
  pControl = (char*) WiFlyFixedPrompts[WIFLY_MSG_CLOSE];		/* "*CLOS*" */
  
  /* set default uart transmission speed to same as WiFly default speed. */
  uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(WIFLY_DEFAULT_BAUD_RATE,F_CPU) );
  uart_getc();
  uart_flush(); 
}/* WiFlySerial */


/*************************************************************************
 begin
 Initializes WiFly interface and starts communication with WiFly device.
																
 Parameters: none.
 Returns: TRUE on initialize success, FALSE on failure.
**************************************************************************/
unsigned char begin() 
{
  unsigned char bStart = FALSE;
  char szCmd[SMALL_COMMAND_BUFFER_SIZE];
  char szResponse[COMMAND_BUFFER_SIZE];
  //char szIndicator[INDICATOR_BUFFER_SIZE];  

  /* Device may or may not be:
     - awake / asleep
     - net-connected / connection lost
     - IP assigned / no IP
     - in command mode / data mode
     - in known state / confused */
  
  /* Start by setting command prompt. */
  bWiFlyInCommandMode = FALSE;
  StartCommandMode(szCmd, SMALL_COMMAND_BUFFER_SIZE);
  
  /* turn off echo */
  /* set baud rate */
  bStart = SendCommand( GetBuffer_P(STI_WIFLYDEVICE_SET_UART_MODE, szCmd, SMALL_COMMAND_BUFFER_SIZE), 
                         WiFlyFixedPrompts[WIFLY_MSG_AOK],
                         szResponse, 
                         COMMAND_BUFFER_SIZE,
						 TRUE,
						 DEFAULT_WAIT_TIME,
						 TRUE,
						 TRUE );

  bStart = SendCommand( GetBuffer_P(STI_WIFLYDEVICE_SET_UART_BAUD, szCmd, SMALL_COMMAND_BUFFER_SIZE), 
                        WiFlyFixedPrompts[WIFLY_MSG_AOK],
                        szResponse,
                        COMMAND_BUFFER_SIZE,
						TRUE,
						DEFAULT_WAIT_TIME,
						TRUE,
						TRUE );

  GetCmdPrompt();

  //DebugPrint("GotPrompt:");
  //DebugPrint(szWiFlyPrompt);
  
  getDeviceStatus();
  
  /* try, then try again after reboot. */
  if (strlen(szWiFlyPrompt) < 1 ) 
  {
    /* got a problem */
    //DebugPrint(GetBuffer_P(STI_WIFLYDEVICE_ERR_REBOOT, szCmd, SMALL_COMMAND_BUFFER_SIZE));
    reboot();
    
    _delay_ms(WIFLY_RESTART_WAIT_TIME);
    /* try again */
    
    GetCmdPrompt();
    if (strlen(szWiFlyPrompt) < 1 ) 
	{
      //DebugPrint(GetBuffer_P(STI_WIFLYDEVICE_ERR_START_FAIL, szCmd, SMALL_COMMAND_BUFFER_SIZE));
      bStart = FALSE;
    }
  }

  return bStart;
}/* begin() */


/*************************************************************************
 ScanForPattern
						
 General-purpose stream watcher.
 Monitors incoming stream until given prompt is detected, or error conditions, or until timer expired
																					
 Parameters
 ResponseBuffer    buffer for WiFly response
 bufsize           size of buffer
 pExpectedPrompt   Marker to find
 bCollecting       true: collect chars in buffer UNTIL marker found, false: discard UNTIL marker found
 WaitTime          Timeout duration to wait for response 
 bPromptAfterResult true: version prompt after result, false: version prompt precedes results (scan, join).
																																									
 Returns: (see .h file)  OR-ed flags of the following
 WiFly Responses:
 #define PROMPT_NONE 0
 #define PROMPT_EXPECTED_TOKEN_FOUND 1
 #define PROMPT_READY 2
 #define PROMPT_CMD_MODE 4
 #define PROMPT_AOK 8
 #define PROMPT_OTHER 16
 #define PROMPT_CMD_ERR 32
 #define PROMPT_TIMEOUT 64
 #define PROMPT_OPEN 128
 #define PROMPT_CLOSE 256
**************************************************************************/
int ScanForPattern( char* responseBuffer, const int buflen,  const char *pExpectedPrompt, const unsigned char bCollecting,  const unsigned long WaitTime, const unsigned char bPromptAfterResult) 
{  
  unsigned char iPromptFound = PROMPT_NONE;  
  unsigned char chResponse = 'A';
  unsigned int  bufpos = 0;
  unsigned int  bufsize = buflen -1;  /*terminating null for bufsize */
  //int  iPromptIndex = 0;
  unsigned char bWaiting = TRUE;
  //unsigned char bReceivedCR = FALSE;
  
 WiFlyFixedPrompts[WIFLY_MSG_EXPECTED] = (char*) pExpectedPrompt;
 WiFlyFixedPrompts[WIFLY_MSG_PROMPT] = (char*) szWiFlyPrompt;

 char* pFixedCurrent[N_PROMPTS];
 int iFixedPrompt = 0;
   
 for (int i=0; i< N_PROMPTS; i++) 
 {
  pFixedCurrent[i] = (char*) WiFlyFixedPrompts[i];
 }
    
  memset (responseBuffer, '\0', bufsize);
  //unsigned long TimeAtStart = millis()  ;  /* capture current time */
      
  while (bWaiting ) 
  {    
    if ( uart_available() > 0 ) 
	{
      chResponse = uart_getc(); 
      //DebugPrint(chResponse);
      
      if ( bCollecting ) 
	  {     
        responseBuffer[bufpos] = chResponse;
        if ( ++bufpos == bufsize ) 
		{     
          bufpos = 0;
        } /* if buffer wrapped */
      } /* if capturing */
            
      for ( iFixedPrompt = 0; iFixedPrompt< N_PROMPTS; iFixedPrompt++ ) 
	  {
        if ( chResponse == *pFixedCurrent[iFixedPrompt] ) 
		{
          /* deal with 'open' and 'scan' version-prompt appearing BEFORE result; ignore it */
          if ( (!bPromptAfterResult) && (iFixedPrompt == WIFLY_MSG_PROMPT || iFixedPrompt == WIFLY_MSG_PROMPT2) /* standard version-prompt */  ) 
		  {
            bWaiting = TRUE;
            iPromptFound |= PROMPT_READY;
          } 
		  else 
		  {
            bWaiting = ( *(++pFixedCurrent[iFixedPrompt]) == '\0' ? FALSE : TRUE ) ; /* done when end-of-string encountered. */           
            if (!bWaiting) 
			{				
              iPromptFound |= WiFlyFixedFlags[iFixedPrompt];  /* if a prompt found then grab its flag. */
            }
           } /* handle prompt-BEFORE-result case */
        } 
	    else 
	    {
          pFixedCurrent[iFixedPrompt] = (char*) WiFlyFixedPrompts[iFixedPrompt];  /* not next char expected; reset to beginning of string. */
        } /* if tracking expected response */
      }
       
	   /* If the *OPEN* signal caught then a connection was opened. */
       if (iPromptFound & (PROMPT_OPEN | PROMPT_OPEN_ALREADY)  ) 
	   {
         bWiFlyConnectionOpen = TRUE;
         bWiFlyInCommandMode = FALSE;
         iPromptFound &= (!WiFlyFixedFlags[WIFLY_MSG_CLOSE]);  /* clear prior close */
       }
       	   
       /* If the *CLOS* signal caught then a connection was closed  */
       /* and we dropped into command mode */
       if (iPromptFound & PROMPT_CLOSE ) 
	   {
         bWiFlyConnectionOpen = FALSE;
         bWiFlyInCommandMode = TRUE;
         iPromptFound &= (!WiFlyFixedFlags[WIFLY_MSG_OPEN]);  /* clear prior open */
       }
                       
    } /* if anything in uart */
      
    //if ( (millis() - TimeAtStart) >= WaitTime) 
	//{
    //    bWaiting = FALSE;        
    //}/* did we time-out? */
 }  /* while waiting for a line*/
    /* could capture and compare with known prompt */
 if ( bCollecting ) 
 {   
   responseBuffer[bufpos]='\0'; 
 }

  return (int) iPromptFound;
} /* ScanForPattern */


/*************************************************************************
 Start Command Mode
 					
 Attempt up to 5 times
 test is "Get a command prompt matching results of 'ver' command".
 if InCommand mode, try a 'cr'.  
 If no useful result, assume not actually in command mode, force with $$$
 																				
 Returns true for Command mode entered, false if not (something weird afoot).
**************************************************************************/
unsigned char StartCommandMode(char* pBuffer, const int bufSize) 
{
  unsigned char iPromptResult = 0;
  char* responseBuffer;
  unsigned char bWaiting = TRUE;
  int nTries = 0;
  
  if (pBuffer == NULL) 
  {
    responseBuffer = (char*) malloc(bufSize); /* defaults to COMMAND_BUFFER_SIZE */
  } 
  else 
  {
    responseBuffer = pBuffer;
  }

//  unsigned long TimeOutTime = millis() + ATTN_WAIT_TIME;
  /* check if actually in command mode: */
  while  (!bWiFlyInCommandMode || bWaiting ) 
  {            
       /* if not effectively in command mode, try $$$ */
       if ( !bWiFlyInCommandMode) 
	   { 
         uart_flush();
         /* Send $$$ , wait a moment, look for CMD */
         _delay_ms(COMMAND_MODE_GUARD_TIME );
         uart_puts(GetBuffer_P(STI_WIFLYDEVICE_ATTN, responseBuffer, bufSize) );
         uart_flush();
         _delay_ms(COMMAND_MODE_GUARD_TIME  );
         if (nTries >= 2)  {
            uart_puts("\r");
            uart_flush();
          }
         /* expect CMD without a cr */
         /* WiFlyFixedPrompts[WIFLY_MSG_CMD] */
         iPromptResult = ScanForPattern( responseBuffer, bufSize, "CMD", TRUE, ATTN_WAIT_TIME, TRUE);
         
         if ( iPromptResult & ( PROMPT_EXPECTED_TOKEN_FOUND | PROMPT_READY |PROMPT_CMD_MODE |PROMPT_CMD_ERR ) ) 
		 {
            bWiFlyInCommandMode = TRUE;
            bWaiting = FALSE;
         } 
		 else 
		 {
            bWiFlyInCommandMode = FALSE;
         }      /* if one of several indicators of command-mode received. */    
          
       } 
	   else 
	   { 

        /* think we are in a command-mode - try a cr, then add a version command to get through. */
       /* send a ver + cr, should see a prompt. */
          if (nTries > 2)  {
            uart_puts(GetBuffer_P(STI_WIFLYDEVICE_VER, responseBuffer, bufSize));
            /* DebugPrint("ver="); */
            // DebugPrint(responseBuffer);
          }
          
          // DebugPrint("***scm:InCommandMode***");
          uart_puts( "\r\r");
          /* bring in a cr-terminated line */
          uart_flush();
          /*/ wait for up to time limit for a cr to flow by */
          iPromptResult = ScanForPattern( responseBuffer, bufSize, szWiFlyPrompt, FALSE, DEFAULT_WAIT_TIME, TRUE);
          /* could have timed out, or have *READY*, CMD or have a nice CR. */
                    
         if ( iPromptResult & ( PROMPT_EXPECTED_TOKEN_FOUND | PROMPT_AOK | PROMPT_READY |PROMPT_CMD_MODE |PROMPT_CMD_ERR ) ) 
		 {
               bWiFlyInCommandMode = TRUE;
               bWaiting = FALSE;
         } 
		 else 
		 {
               bWiFlyInCommandMode = FALSE;
         }      /* if one of several indicators of command-mode received.     */

       } /*  else in in command command mode */
  
      //if (  millis() >= TimeOutTime) {
      //  bWaiting = FALSE;
      //}
      nTries++;
  } /* while trying to get into command mode */
  
  /* clean up as needed */
  if (pBuffer == NULL) {
    free (responseBuffer);
  }

  return bWiFlyInCommandMode;  
}


/* GetCmdPrompt*/
/* Obtains the WiFly command prompt string for use by other command functions.*/
/* Parameters: None*/
/* Sets global szWiFlyPrompt*/
/* Returns command prompt on success or empty string on failure*/
unsigned char GetCmdPrompt () {
 
  unsigned char bOk = FALSE;
  char responseBuffer[RESPONSE_BUFFER_SIZE];
    
  if ( StartCommandMode(responseBuffer, RESPONSE_BUFFER_SIZE)  ) {
    //uart << GetBuffer_P(STI_WIFLYDEVICE_VER, responseBuffer, RESPONSE_BUFFER_SIZE )  << "\r";
    uart_flush();
    ScanForPattern(responseBuffer, RESPONSE_BUFFER_SIZE, WiFlyFixedPrompts[WIFLY_MSG_PROMPT2], TRUE, COMMAND_MODE_GUARD_TIME, TRUE);
    char* pPromptStart = strrchr(responseBuffer, '<') ;
    char* pPromptEnd = strrchr (responseBuffer, '>');
        
    if ( (pPromptStart < pPromptEnd ) && pPromptStart && pPromptEnd) {
      strncpy(szWiFlyPrompt, pPromptStart , (size_t) (pPromptEnd - pPromptStart)+1 );
      szWiFlyPrompt[(pPromptEnd - pPromptStart)+1] = '\0';
    
    }
  }
 
  if ( strlen (szWiFlyPrompt) > 1 ) {
      bOk = TRUE;
      // DebugPrint( F("CmdPrompt:") );
      // DebugPrint(szWiFlyPrompt);
  } else {
    bOk = FALSE;
  }

  return bOk;  
}


/* SendCommand*/
/* Issues a command to the WiFly device*/
/* Captures results in Returned result*/
/* 										*/		
/*										*/
/* Parameters: */
/* Command          The inquiry-command to send*/
/* SuccessIndicator String to indicate success */
/* pResultBuffer    A place to put results of the command*/
/* bufsize          Length of the pResultBuffer*/
/* bCollecting      true = collect results, false=ignore results.*/
/* iWaitTime        Time in milliseconds to wait for a result.*/
/* bClear           true = drain any preceeding and subsequent characters, false=ignore*/
/* bPromptAfterResult true=commands end with a version-prompt, false=version-prompt precedes results.*/
/*																					*/
/* Returns true on SuccessIndicator presence, false if absent.*/
unsigned char SendCommand( char *pCmd, const char *SuccessIndicator, char* pResultBuffer, const int bufsize, 
              const unsigned char bCollecting, const  unsigned long iWaitTime, const unsigned char bClear, const unsigned char bPromptAfterResult) {
  
  unsigned char bCommandOK = FALSE;
  //char ch;
  int iResponse = 0;
  int iTry = 0;
    
  char* Command  = pCmd;
  if (pCmd == pResultBuffer ) {
    Command = (char*) malloc( sizeof(pCmd) +1 );
    strcpy( Command, pCmd);
  }
  //      
  // clear out leftover characters coming in
  
  if ( bClear ) {
	 // DebugPrint("Clearing:"); 
	 //while ( available() ) {
		//ch = (char) read();
		// DebugPrint( ch);
	 //} 
  } 

 
//  DebugPrint( "Cmd:");
//  DebugPrint( Command );
//  DebugPrint( " Ind:" );
//  DebugPrint( SuccessIndicator);
    
 
  if ( StartCommandMode(pResultBuffer, bufsize) ) {
	  uart_flush();
    
      while ( ((iResponse & PROMPT_EXPECTED_TOKEN_FOUND) != PROMPT_EXPECTED_TOKEN_FOUND) && iTry < COMMAND_RETRY_ATTEMPTS ) {
        uart_puts( Command);
		uart_puts( "\r" );
        uart_flush();
        iResponse = ScanForPattern( pResultBuffer, bufsize, SuccessIndicator, bCollecting, iWaitTime, bPromptAfterResult );   
        
 //       DebugPrint("Try#:");
 //       DebugPrint( iTry ); 
 //       DebugPrint(" Res:");
 //       DebugPrint(iResponse);
        iTry++;
      }

  }
  if ( pCmd == pResultBuffer ) {
    free (Command);
  }
  
  if ( bClear ) {
     ScanForPattern(strchr(pResultBuffer, '\0') +1, bufsize - strlen(pResultBuffer) -1, WiFlyFixedPrompts[WIFLY_MSG_CLOSE], FALSE, DEFAULT_WAIT_TIME, TRUE);
//     while ( (ch = uart.read() ) > -1 ) {
//       DebugPrint(ch);
//     }
  } // clear out leftover characters
  
  bCommandOK = ( ((iResponse & PROMPT_EXPECTED_TOKEN_FOUND) == PROMPT_EXPECTED_TOKEN_FOUND) ? TRUE : FALSE );

  return bCommandOK;
}


/* convenient and version with own small ignored response buffer.*/
unsigned char SendCommandSimple( char* pCommand, const char* pSuccessIndicator) {
 
  char bufResponse[INDICATOR_BUFFER_SIZE];

  return SendCommand(  pCommand, pSuccessIndicator, bufResponse, INDICATOR_BUFFER_SIZE, FALSE, DEFAULT_WAIT_TIME, TRUE, TRUE );
}


/* SendInquiry*/
/* Inquiries provide a device setting result, terminated with a command prompt.*/
/* No specific 'ok/fail' result shown, only ERR or requested response.*/
/* Results placed into global responsebuffer*/
/*												*/
/* Parameters: */
/* Command        The inquiry-command to send*/
/* pBuffer        pointer to a buffer for the response*/
/* bufsize        size of the buffer*/
/*										*/
/* Returns true on command success, false on failure.*/
unsigned char SendInquiry( char *Command, char* pBuffer, const int bufsize) {

  return  SendCommand(Command,  szWiFlyPrompt, pBuffer, bufsize, TRUE, DEFAULT_WAIT_TIME, TRUE, TRUE);
    
}


/* SendInquiry*/
/* Inquiries provide a device setting result, terminated with a command prompt.*/
/* No specific 'ok/fail' result shown, only ERR or requested response.*/
/* Results placed into global responsebuffer*/
/*											*/
/* Parameters: */
/* Command        The inquiry-command to send*/
/*											*/
/* Returns true on command success, false on failure.*/
unsigned char SendInquirySimple( char *Command ) {
  
  char InquiryBuffer[RESPONSE_BUFFER_SIZE];
  unsigned char bSendInquiry = FALSE;

  bSendInquiry = SendCommand(Command,  szWiFlyPrompt, InquiryBuffer, RESPONSE_BUFFER_SIZE, TRUE, DEFAULT_WAIT_TIME, TRUE, TRUE);
  // should trim to returned result less ExpectedPrompt
  
  return bSendInquiry;
}


/* exitCommandMode*/
/* Exits from WiFly command mode.*/
/*									*/
/* Watch the NSS for further traffic.*/
/*										*/
/* Parameters: */
/* None			*/
/* Returns true on command success, false on failure.*/
unsigned char exitCommandMode() {

  char szCmd[INDICATOR_BUFFER_SIZE]; // exit command is short
  char szPrompt[INDICATOR_BUFFER_SIZE]; // exit Prompt is short:  EXIT (which looks like 'exit' but in upper case).
  char szResponse[INDICATOR_BUFFER_SIZE]; // small buffer for result

  bWiFlyInCommandMode = !SendCommand( GetBuffer_P(STI_WIFLYDEVICE_LEAVE_CMD_MODE, szCmd, INDICATOR_BUFFER_SIZE),
                      strupr(GetBuffer_P(STI_WIFLYDEVICE_LEAVE_CMD_MODE, szPrompt, INDICATOR_BUFFER_SIZE)),
                      szResponse, INDICATOR_BUFFER_SIZE, FALSE, DEFAULT_WAIT_TIME, TRUE, TRUE );    
                      
  bWiFlyInCommandMode = FALSE;
  
  return bWiFlyInCommandMode;
}


/* showNetworkScan*/
/* Displays list of available WiFi networks.*/
/*													*/
/* Parameters: */
/* pNetScan    Buffer for scan results (should be large)*/
/* buflen      length of buffer*/
char* showNetworkScan( char* pNetScan, const int buflen) {
  
  SendCommand("scan","'", pNetScan, buflen, TRUE, JOIN_WAIT_TIME, TRUE, FALSE) ;
  
  return pNetScan;  
}


/* setProtocol*/
/* Sets WiFly's communication protocol*/
/* TCP or UDP*/
/*										*/
/* Parameters:*/							
/* iProtocol      Hex value for protocol - bit mapped values.*/ 
/* Returns true for success, false for failure or error in call*/
/*							*/
/* Note:*/
/* Switching from one to the other requires a reboot of the WiFly.*/
/* For UDP, set all other settings first (unlike example in manual) ; UDP traffic will start upon WiFly reboot.*/
/* This version does not attempt to determine current mode. Reboot forced.*/
unsigned char setProtocol( unsigned int iProtocol) {
  
  unsigned char bOk = FALSE;
  unsigned int iMode = 0, iTmp; 
  char bufMode[10];  
  
  iTmp = iProtocol &  WIFLY_IPMODE_TCP;
  
  if (  iTmp != 0x00 ) {
      iMode |= 0x02; 
  } 
  
  //iTmp = WIFLY_IPMODE_UDP & iProtocol;
  //if (iTmp ) {
  //    iMode |= 0x01;
  //}
  
  itoa( iMode, bufMode, 10);

  bOk =  issueSetting( STI_WIFLYDEVICE_SET_PROTOCOL, bufMode );
  
  // Save settings
  
  char szCommand[SMALL_COMMAND_BUFFER_SIZE];

  GetBuffer_P(STI_WIFLYDEVICE_SAVE, szCommand, SMALL_COMMAND_BUFFER_SIZE);
  
  bOk = SendCommandSimple(szCommand , WiFlyFixedPrompts[WIFLY_MSG_AOK]);
  reboot();
  
  // Allow WiFly to restart
  _delay_ms (WIFLY_RESTART_WAIT_TIME);
  
  return bOk;
}
    

/* openConnection*/
/* Opens a TCP connection to the provided URL and port (defaults to 80)*/
/*																		*/		
/* Parameters:*/
/* pURL      IP or dns name of server to connect to.*/
/* iWaitTime Time to wait for connection*/
/*												*/
/* Returns: true on success, false on failure.*/
/* Remote Port number is set through SetRemotePort*/
/* Note that opened ports can be closed externally / lost connection at any time.*/
/* Opening a connection switches to Data mode from Command mode.*/
/*																				*/
/* Note: Open and Scan each generate a version-prompt BEFORE results, not after.*/
unsigned char openConnection(const char* pURL, const unsigned long iWaitTime) {
  char bufOpen[INDICATOR_BUFFER_SIZE];
  char bufCommand[COMMAND_BUFFER_SIZE];
  
  memset (bufCommand, '\0', COMMAND_BUFFER_SIZE);
  GetBuffer_P(STI_WIFLYDEVICE_OPEN, bufCommand, COMMAND_BUFFER_SIZE);
  strcat (bufCommand, pURL);
  strcat (bufCommand, " ");
  itoa( iRemotePort, strchr(bufCommand, '\0'), 10);
  //DebugPrint("openConnection:");
  //DebugPrint(bufCommand);
  bWiFlyConnectionOpen = SendCommand(bufCommand,WiFlyFixedPrompts[WIFLY_MSG_OPEN], bufOpen, INDICATOR_BUFFER_SIZE, FALSE, iWaitTime , TRUE, FALSE); 
  if( bWiFlyConnectionOpen) {
	  bWiFlyInCommandMode = FALSE;
  }
  
  return bWiFlyConnectionOpen;
}


/* closeConnection*/
/* closes an open connection*/
/*								*/
/* Parameters:					*/
/* bSafeClose	   (default) Use slow 'safe close'*/
/*                 false if high confidence connection is in fact open,*/ 
/*				   and can tolerate occasional errors if connection*/
/*                 closes sooner than expected.*/
/*                 											*/
/* returns   true on close, false on failure.*/
/* Side effects: bWiFlyConnectionOpen should become false*/
/*          and  bWiFlyInCommandMode should become true.*/
/*														*/
/* Notes:																		*/
/* 1. When closed via a command the WiFly state is in command mode.*/
/* 2. External connection close leaves WiFly in data mode*/
/* 3. Closing a closed connection results in an error.*/
/* 4. External connection close could happen at any time.*/
/* 5. getDeviceStatus() gives actual WiFly status at the moment of calling.*/
/* 6. When opened via listening, info from previous getDeviceStatus() is out-of-date.*/
/* 7. The 'close' command results with a command-prompt, no 'AOK' or similar.*/
/*     then, a *CLOS* signal appears.*/
/*										*/
/* returns true on closure, false on failure to close.*/ 
unsigned char closeConnection(unsigned char bSafeClose) {
  // if a connection is open then close it.
  //char chDrain;
  
  if ( bWiFlyConnectionOpen ) {
      // first see if connection is *STILL* open. 
      //unsigned char bClosed = FALSE;
      unsigned char bTrySafeClose = bSafeClose;
      unsigned char bDoClose = TRUE;
      
      // repeat until closed...
      while ( bWiFlyConnectionOpen ) {
		  if (bTrySafeClose) {
			getDeviceStatus();
			if ( isTCPConnected() && bWiFlyConnectionOpen ) {
				bDoClose = TRUE;
			} else {
				bDoClose = FALSE;
				bWiFlyConnectionOpen = FALSE; // should be redundant as a caught *CLOS* will set this to false.
			}
		  } // if doing safe close
		  if ( bDoClose ) {
			  //drain();
			  char bufCmd[INDICATOR_BUFFER_SIZE];
			  char bufClose[INDICATOR_BUFFER_SIZE];
			  memset( bufCmd, '\0', INDICATOR_BUFFER_SIZE);
			  memset( bufClose, '\0', INDICATOR_BUFFER_SIZE);
			  
			  // close command response is a prompt, then a *CLOS* signal after.
			  SendCommand(GetBuffer_P(STI_WIFLYDEVICE_CLOSE, bufCmd, INDICATOR_BUFFER_SIZE), WiFlyFixedPrompts[WIFLY_MSG_PROMPT],    bufClose,    INDICATOR_BUFFER_SIZE, TRUE, DEFAULT_WAIT_TIME , FALSE, TRUE);
			  //DebugPrint( bufCmd );
			  //DebugPrint( bufClose );

			  //drain();
			  
			  bWiFlyInCommandMode = TRUE;
		  }
	  } // while
   } else {
	  // closed externally already.
	  // No change to bWiFlyCommandMode. 
      return TRUE;
   }
   return TRUE;
}


/* drain*/
/* Empties incoming buffer */
/*int drain() {
	char chDrain;
	//unsigned long TimeOutTime = millis() + DEFAULT_WAIT_TIME;
	// DebugPrint("Waiting for signal");
	while ( bWiFlyConnectionOpen && available() > 0 && millis() < TimeOutTime  ) {
		chDrain = read();
		DebugPrint( chDrain );
	}
	// DebugPrint("Drained.");
}*/


/* serveConnection*/
/* Waits for a client to connect on the given port.*/
/*													*/
/* Parameters:*/
/* reconnectWaitTime  Duration to wait before verifying wlan and reconnecting if needed.*/
/*																		*/
/* returns            true on connection, false on internal failure.*/
unsigned char serveConnection( const unsigned long reconnectWaitTime )
{
  char bufRequest[COMMAND_BUFFER_SIZE];
  int iRequest;
  unsigned char bReturn = FALSE;
   
   
  iRequest = ScanForPattern( bufRequest, COMMAND_BUFFER_SIZE, WiFlyFixedPrompts[WIFLY_MSG_OPEN], FALSE, reconnectWaitTime, TRUE );
  if ( ( iRequest &  PROMPT_EXPECTED_TOKEN_FOUND) == PROMPT_EXPECTED_TOKEN_FOUND ) {
    //memset (bufRequest,'\0',COMMAND_BUFFER_SIZE);
    bWiFlyInCommandMode = FALSE;
    bReturn = TRUE;
  } else {
    // No connection within timeout.
    bReturn = FALSE;
    // reconnection logic
  }

  // TODO: decision on timeout/reconnection status
  
  return bReturn;
}


/* getMAC*/
/* Returns WiFly MAC address*/
/*							*/
/* Parameters: */
/* bufMAC    buffer for MAC address*/
/* buflen    length of buffer (should be at least 18 chars)*/
/* Returns:  pointer to supplied buffer MAC address, or empty string on failure.*/
/* Format expected: Mac Addr=xx:xx:xx:xx:xx:xx*/
char* getMAC(char* pbuf, int buflen) {

  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_MAC, pbuf, buflen, STI_WIFLYDEVICE_GET_MAC_ADDR, STI_WIFLYDEVICE_RETURN ) ;
}


/* getIP*/
/* Returns WiFly IP address*/
/*							*/
/* Parameters*/
/* bufIP     buffer for IP address*/
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with IP address, will be empty string on failure.*/ 
char* getIP(char* pbuf, int buflen) {
 
 return ExtractDetailIdx( STI_WIFLYDEVICE_GET_IP_DETAILS, pbuf, buflen, STI_WIFLYDEVICE_GET_IP_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getNetMask*/
/* Returns WiFly Netmask*/
/*						*/
/* Parameters*/
/* buf     buffer for Netmask */
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with GW address, will be empty string on failure.*/
char* getNetMask(char* pbuf, int buflen) {
  
  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_IP_DETAILS, pbuf, buflen, STI_WIFLYDEVICE_GET_NM_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getGateway*/
/* Returns WiFly Gateway address*/
/*						*/
/* Parameters*/
/* bufGW     buffer for IP address*/
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with GW address, will be empty string on failure.*/
char* getGateway(char* pbuf, int buflen) {
  
  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_IP_DETAILS, pbuf, buflen, STI_WIFLYDEVICE_GET_GW_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getDNS*/
/* Returns WiFly DNS address*/
/*							*/
/* Parameters*/
/* bufDNS    buffer for IP address*/
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with DNS address, will be empty string on failure.*/
char* getDNS(char* pbuf, int buflen) {
  
  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_DNS_DETAILS, pbuf, buflen, STI_WIFLYDEVICE_GET_DNS_IND, STI_WIFLYDEVICE_RETURN ) ;
  
}

/* getSSID*/
/* Returns current SSID*/
/*					*/
/* Parameters*/
/* bufSSID   buffer for SSID*/
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with SSID, will be empty string on failure.*/
char* getSSID(char* pbuf, int buflen) {

  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_WLAN, pbuf, buflen, STI_WIFLYDEVICE_GET_WLAN_SSID_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getRSSI*/
/* Returns current RSSI */
/*				*/
/* Parameters*/
/* pbuf      buffer for RSSI*/
/* buflen    length of buffer */
/* Returns:  pointer to supplied buffer with RSSI, will be empty string on failure. */
char* getRSSI(char* pbuf, int buflen) {
  
  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_RSSI, pbuf, buflen, STI_WIFLYDEVICE_GET_RSSI_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getBattery*/
/* Returns current Battery voltage*/
/*								*/
/* Parameters*/
/* pbuf      buffer for voltage*/
/* buflen    length of buffer */
/* Returns:  pointer to supplied buffer with battery voltage, will be empty string on failure.*/
char* getBattery(char* pbuf, int buflen) {

  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_BATTERY, pbuf, buflen, STI_WIFLYDEVICE_GET_BATTERY_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getDeviceID*/
/* Returns current DeviceID*/
/*					*/
/* Parameters*/
/* bufSSID   buffer for DeviceID*/
/* buflen    length of buffer (should be at least 18 chars for IPv4 and longer for IPv6)*/
/* Returns:  pointer to supplied buffer with DeviceID, will be empty string on failure.*/
char* getDeviceID(char* pbuf, int buflen) {

  return ExtractDetailIdx( STI_WIFLYDEVICE_GET_WLAN, pbuf, buflen, STI_WIFLYDEVICE_GET_WLAN_DEV_IND, STI_WIFLYDEVICE_RETURN ) ;
}


/* getTime*/
/* Returns (in milliseconds) time since bootup or Unix epoch if NTP server updated.*/
/*					*/
/* Parameters: */
/* Returns:  unsigned long representing seconds since Unix Epoch or power-on.*/
/* Format expected: RTC=tttttttt ms*/
unsigned long getTime() {
  char bufTimeSerial[COMMAND_BUFFER_SIZE];
  
  return ( atol(ExtractDetailIdx( STI_WIFLYDEVICE_GET_TIME, bufTimeSerial, COMMAND_BUFFER_SIZE, STI_WIFLYDEVICE_GET_TIME_IND, STI_WIFLYDEVICE_RETURN ) ) + lUTC_Offset_seconds);
}

// TODO: add GetTimeStatus


/* getDeviceStatus*/
/* Refreshes device status flags*/
/*								*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.*/
/*							*/
/* Parameters*/
/* None.*/
/* Returns true on success, false on failure.*/
long getDeviceStatus() {
  
  char bufStatus[INDICATOR_BUFFER_SIZE];
  memset(bufStatus,'\0',INDICATOR_BUFFER_SIZE);
  // place leading fixed '8' into first position
  bufStatus[0]='8';
 
  ExtractDetailIdx( STI_WIFLYDEVICE_GET_STATUS, &bufStatus[1], INDICATOR_BUFFER_SIZE - 1, STI_WIFLYDEVICE_GET_STATUS_IND, STI_WIFLYDEVICE_RETURN ) ;
    
  fStatus = strtol(bufStatus, (char**)0, 16);
  
  return fStatus;
}


/* isTCPConnected*/
/* 					*/
/* Returns true if TCP Connected, false if not.*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.*/
/* bit 1*/
/*			*/
/* Parameters*/
/* None. */
/* Returns true on success, false on failure.*/
unsigned char isTCPConnected() { 
  
  if ( (fStatus & 0x8000) == 0 ) {
    
    getDeviceStatus();
  }
  
  return ( fStatus &&  0x01 > 0 ? TRUE : FALSE);
}


/* isAssociated*/
/* 				*/
/* Returns true if associated with a wifi SSID, false if not.*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.*/
/* bit 4*/
/*			*/
/* Parametersv*/
/* None. */
/* Returns true on success, false on failure.*/
unsigned char isAssociated() {
  
  if ( (fStatus & 0x8000) == 0 ) {
    
    getDeviceStatus();
  }
  
  return ( (fStatus & 0x10) > 0 ? TRUE : FALSE); // if (fStatus & 0x10) > 0 { return TRUE; } else { return FALSE;}
}


/* isAuthenticated*/
/* 					*/
/* Returns true if authenticated with a wifi SSID, false if not.*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.	*/
/* bit 5*/	
/*				*/
/* Parameters*/
/* None. */
unsigned char isAuthenticated() {
  
  if ( (fStatus & 0x8000) == 0 ) {
    
    getDeviceStatus();
  }
  
  return ( (fStatus & 0x20) > 0 ? TRUE : FALSE);
}


/* isDNSfound*/
/* 								*/
/* Returns true if DNS is found, false if not.*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.	*/
/* bit 6	*/
/*				*/
/* Parameters*/
/* None. */
unsigned char isDNSfound() {
  
  if ( (fStatus & 0x8000) == 0 ) {
    
    getDeviceStatus();
  }
  
  return ( (fStatus & 0x40) > 0 ? TRUE : FALSE);
}


/* isDNScontacted*/
/* 					*/
/* Returns true if DNS is contacted, false if not.*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.*/
/* bit 7*/
/*		*/
/* Parameters*/
/* None. */
unsigned char isDNScontacted() {
if ( (fStatus & 0x8000) == 0 ) {
			    
    getDeviceStatus();
  }
  
  return ( (fStatus & 0x80) > 0 ? TRUE : FALSE); //if then else
}


/* getChannel*/
/* 				*/
/* Returns wifi channel number .*/
/* Dependent on prior call to getDeviceStatus();*/
/* WiFly 'show connection' command gives a result in hex 8XYZ as a string.*/
/* bits 9-*/
/*			*/
/* Parameters*/
/* None. */
/* Returns true on success, false on failure.*/
int getChannel() {
  
  if ( (fStatus & 0x8000) == 0 ) {
    
    getDeviceStatus();
  }
  
  return ( (fStatus >> 8 ) - 0x80);
}


/* isInCommandMode*/
/* 						*/
/* Returns true if WiFly in Command Mode false if not.*/
/*														*/
/* Parameters*/
/* None. */
unsigned char isInCommandMode() {
  
  return  bWiFlyInCommandMode;
}


/* isConnectionOpen*/
/* 					*/
/* Returns true if an IP connection is open, false if not.*/
/*														*/
/* Parameters*/
/* None. */ 
unsigned char isConnectionOpen() {
  
  return  bWiFlyConnectionOpen;
}


/* ExtractDetailIdx*/
/* Returns substring from a response.  Indexes point to command, search string and terminator strings.*/
/*																						*/
/* Parameters: */
/* idxCommand     StringID of command*/
/* pDetail        pointer to destination buffer*/
/* buflen         length of destination buffer*/
/* idxSearch      StringID to extract AFTER*/
/* idxStop        StringID to extract UNTIL*/
/*											*/
/* Returns:       pointer to destination buffer*/
char* ExtractDetailIdx(const int idxCommand, char* pDetail, int buflen, const int idxSearch, const int idxStop) {
  char bufCmd[COMMAND_BUFFER_SIZE];
  char bufSearch[INDICATOR_BUFFER_SIZE];
  char bufStop[INDICATOR_BUFFER_SIZE];
  
  GetBuffer_P(idxCommand, bufCmd, COMMAND_BUFFER_SIZE);
  GetBuffer_P(idxSearch, bufSearch, INDICATOR_BUFFER_SIZE);
  GetBuffer_P(idxStop, bufStop, INDICATOR_BUFFER_SIZE);
  
  return ExtractDetail( bufCmd, pDetail, buflen, bufSearch, bufStop);
}


/*
   ExtractDetail
   Returns substring from a response.

   Parameters: 
   idxCommand     StringID of command
   pDetail        pointer to destination buffer
   buflen         length of destination buffer
   pFrom          String to extract AFTER
   pTo            String to extract UNTIL

   Returns:       pointer to destination buffer
  char* WiFlySerial::ExtractDetail(const int idxCommand, char* pDetail, int buflen, const char* pFrom, const char* pTo) {
    char bufCmd[COMMAND_BUFFER_SIZE];
    GetBuffer_P(idxCommand, bufCmd, COMMAND_BUFFER_SIZE);
    return ExtractDetail( bufCmd, pDetail, buflen, pFrom, pTo);
     
 }*/									


/* ExtractDetail*/
/* Returns substring from a response.*/
/*								*/
/* Parameters: */
/* pCommand       pointer to command*/
/* pDetail        pointer to destination buffer*/
/* buflen         length of destination buffer*/
/* pFrom          String to extract AFTER*/
/* pTo            String to extract UNTIL*/
/*												*/
/* Returns:       pointer to destination buffer*/
char* ExtractDetail(char* pCommand, char* pDetail, int buflen, const char* pFrom, const char* pTo) {
  
  char* pEndToken = NULL;
  //char ch;
    
  //send command and ignore results up to after pFrom
  SendCommand(pCommand,  
             (char*) pFrom,
             pDetail,
             buflen,
             FALSE,  DEFAULT_WAIT_TIME, FALSE, TRUE  );
   // then collect results up to after pTo.
  ScanForPattern(pDetail, buflen, pTo, TRUE,  DEFAULT_WAIT_TIME, TRUE  );                           
  
  // trim result to not include end token.
  
  if ( (pEndToken = strstr(pDetail, pTo) ) != NULL) {
     *pEndToken = '\0';
  }
  
  // clear buffer of remaining characters
  ScanForPattern(strchr(pDetail,'\0')+1, buflen - strlen(pDetail) -1 , "\0\0", FALSE,  DEFAULT_WAIT_TIME , TRUE );                           
//   while ( uart.available() > 0 ) {
//     ch = uart.read();
//     DebugPrint(ch);
//   }
//
    
  return pDetail;
}


/* getLibraryVersion*/
/* Returns WiFly Driver Library version*/
/*						*/
/* Parameters */
/* pDetail        pointer to destination buffer*/
/* buflen         length of destination buffer*/
/* Returns:  pointer to supplied buffer with library version; empty string on failure.*/
char* getLibraryVersion(char* pbuf, int buflen) {

  return GetBuffer_P( STI_WIFLYDEVICE_LIBRARY_VERSION ,pbuf, buflen) ;
}


/* isifUp*/
/* Returns true if currently connected to Access Point.*/
/*						*/
/* Parameters: None*/
/* Returns:  IP netmask, or empty string on failure.*/
unsigned char isifUp() {
  
  unsigned char bReturn = FALSE;
  char buf[COMMAND_BUFFER_SIZE];
  
  ExtractDetailIdx( STI_WIFLYDEVICE_GET_IP_DETAILS, buf, COMMAND_BUFFER_SIZE, STI_WIFLYDEVICE_GET_IP_UP_IND, STI_WIFLYDEVICE_RETURN ) ;
  bReturn = ( strcmp(buf,"UP" ) == 0 ? TRUE : FALSE ) ;
  
  return bReturn;     
}


/* ExtractLineFromBuffer*/
/* Returns string extracted from provided buffer.*/
/*													*/
/*												*/			
/* Parameters: */
/* idString         StringID of command to send */
/* pBuffer          pointer to provided buffer*/
/* bufsize          expected size of required buffer*/
/* pStartPattern    pointer to null-terminated string identifying the start of desired line*/
/* chTerminator     character to terminate desired line.*/
/* Returns:  pointer to within provided buffer, with result or empty string on failure.*/
char* ExtractLineFromBuffer(const int idString,  char* pBuffer, const int bufsize, const char* pStartPattern, const char* pTerminator) {

  char* pStart;
  char* pTerm;
  unsigned char bOk = FALSE;
  char szCommand[SMALL_COMMAND_BUFFER_SIZE];
  char* pResponse = pBuffer;
   
  if ( !SendInquiry( GetBuffer_P(idString, szCommand, SMALL_COMMAND_BUFFER_SIZE) , pBuffer, bufsize) ) {
    bOk = FALSE;
  } else {
    pStart= strstr(pBuffer, pStartPattern);
    if (pStart != NULL ) {
      // Move pointer past the start pattern
      pStart += strlen(pStartPattern);
      pResponse = pStart;
    }
    pTerm = strstr( pStart, pTerminator);
    if (pTerm == NULL ) {
      bOk=FALSE;
    } else {
      *(pTerm) = '\0';
      bOk = TRUE;
    } // if end-of-line found
  
  }
  if (!bOk) {
    *(pResponse) = '\0';
  }

  return pResponse;
}


/* leave*/
/* Disconnects from current WIFI lan*/
/*										*/
/* Parameters: */
/*											*/
/* Returns true on command success, false on failure.*/
unsigned char leave() {

  unsigned char bSendLeave = FALSE;
  char szCmd[COMMAND_BUFFER_SIZE];
  char szReply[INDICATOR_BUFFER_SIZE];
  
  bSendLeave = SendCommand(GetBuffer_P(STI_WIFLYDEVICE_LEAVE, szCmd, COMMAND_BUFFER_SIZE),  
                           GetBuffer_P(STI_WIFLYDEVICE_DEAUTH, szReply, INDICATOR_BUFFER_SIZE),
                           szCmd,  
                           COMMAND_BUFFER_SIZE, 
                           FALSE, DEFAULT_WAIT_TIME, TRUE, TRUE );
  
  return bSendLeave;    
}


/* Sets SSID*/
unsigned char setSSID( const char* pSSID){

  return issueSetting( STI_WIFLYDEVICE_SET_SSID, pSSID );
}


/* Sets Passphrase*/
unsigned char setPassphrase( const char* pPassphrase) {
 
  return issueSetting( STI_WIFLYDEVICE_SET_PASSPHRASE, pPassphrase );
}


/* Sets NTP server address*/
unsigned char setNTP(const char* pNTP) {
  
  return issueSetting( STI_WIFLYDEVICE_SET_NTP, pNTP );
}


/* Sets frequency for NTP updates*/
unsigned char setNTP_Update_Frequency(const char* pNTP_Update) {
  
  return issueSetting( STI_WIFLYDEVICE_SET_NTP_ENABLE, pNTP_Update );
}


/* sets offset from UTC.*/
/* Parameters:*/
/* fltUTC_Offset_Hours  Number of hours from UTC  (e.g. PST = -8 )*/
/* Setting is used during setTime().*/
/* returns true always.*/
unsigned char setNTP_UTC_Offset(float fltUTC_Offset_hours) {

	lUTC_Offset_seconds = (long) (fltUTC_Offset_hours * 60 * 60);

	return TRUE;
}


/* Sets WiFly DNS name?*/
unsigned char setDeviceID( const char* pDeviceID) {
 
  return issueSetting( STI_WIFLYDEVICE_SET_DEVICEID, pDeviceID );
}


/* issueSetting*/
/* Issues a WiFly setting command*/
/* Parameters:						*/
/* IdxCommand    Index into string table of command*/
/* pParam        null-terminated string of parameter. */
/*               Command and parameter must be less than COMMAND_BUFFER_SIZE*/
/*														*/
/* Returns - true on Command success, false on fail.*/
unsigned char issueSetting( int idxCommand, const char* pParam) {
 
  char szReply[INDICATOR_BUFFER_SIZE];
//  char szIndicator[INDICATOR_BUFFER_SIZE];
  char szBuffer[COMMAND_BUFFER_SIZE];
  
  GetBuffer_P(idxCommand,szBuffer, COMMAND_BUFFER_SIZE );
  strncat( szBuffer, pParam, COMMAND_BUFFER_SIZE - strlen(szBuffer) );
    
  return SendCommand( szBuffer, WiFlyFixedPrompts[WIFLY_MSG_AOK], szReply,  INDICATOR_BUFFER_SIZE, TRUE, DEFAULT_WAIT_TIME, TRUE, TRUE );
}


/* SetUseDCHP*/
/* Sets DHCP to requested mode */
unsigned char setDHCPMode(const int iDHCPMode) {
  
  char bufMode[10];
  
  itoa( iDHCPMode, bufMode, 10);

  return issueSetting( STI_WIFLYDEVICE_SET_DHCP, bufMode );
}


/* SetIP*/
/* Sets static IP address*/
/* Parameters:*/
/* pIP      null-terminated character string of the IP address e.g. '192.168.1.3'*/
unsigned char setIP( const char* pIP) {

  return issueSetting( STI_WIFLYDEVICE_SET_IP, pIP );
}


/* SetNetMask*/
/* Sets static IP netmask*/
/* Parameters:*/
/* pNM      null-terminated character string of the netmask e.g. '255.255.255.0'*/
unsigned char setNetMask( const char* pNM) {

  return issueSetting( STI_WIFLYDEVICE_SET_NETMASK, pNM );
}


/* SetGateway*/
/* Sets static Gateway address*/
/* Parameters:*/
/* pGW      null-terminated character string of the Gateway address e.g. '192.168.1.254'*/
unsigned char setGateway( const char* pGW) {

  return issueSetting( STI_WIFLYDEVICE_SET_GATEWAY, pGW );
}


/* SetDNS*/
/* Sets static DNS address*/
/* Parameters:*/
/* pDNS      null-terminated character string of the DNS address e.g. '192.168.1.1'*/
unsigned char setDNS( const char* pDNS) {

  return issueSetting( STI_WIFLYDEVICE_SET_DNS, pDNS );
}


/* SetChannel*/
/* Sets wifi Channel*/ 
/* Parameters:*/
/* pChannel      null-terminated character string of the channel e.g. '6'*/
unsigned char setChannel( const char* pChannel) {
  

  return issueSetting( STI_WIFLYDEVICE_SET_CHANNEL, pChannel );
}


/* SetAuthMode*/
/* Sets wifi Authentication mode*/ 
/* Parameters:*/
/* iAuthMode      Authentication mode of type WIFLY_AUTH_XXXX*/
unsigned char setAuthMode( int iAuthMode) {
  
  char bufMode[10];
  
  itoa( iAuthMode, bufMode, 10);

  return issueSetting( STI_WIFLYDEVICE_SET_WIFI_AUTH, bufMode );
}


/* SetJoinMode*/
/* Sets wifi Network Join mode */
/* Parameters:*/
/* iJoinMode     join mode of type WIFLY_JOIN_XXXXXX*/
unsigned char setJoinMode( int iJoinMode) {
  
  char bufMode[10];
  
  itoa( iJoinMode, bufMode, 10);

  return issueSetting( STI_WIFLYDEVICE_SET_WIFI_JOIN, bufMode );
}


/* SetLocalPort*/
/* Sets Local port for listening*/ 
/* Parameters:*/
/* iLocalPort    Port to listen to.*/
unsigned char setLocalPort( int iNewLocalPort) {

  char bufPort[10];
  
  itoa( iNewLocalPort, bufPort, 10);
  iLocalPort = iNewLocalPort;

  return issueSetting( STI_WIFLYDEVICE_SET_LOCAL_PORT, bufPort );
}


/* SetRemotePort*/
/* Sets Remote port for connections */
/* Parameters:*/
/* iRemotePort    Port to contact on remote server.*/
unsigned char setRemotePort( int iNewRemotePort) {

  char bufPort[10];
  
  itoa( iNewRemotePort, bufPort, 10);
  iRemotePort = iNewRemotePort;

  return issueSetting( STI_WIFLYDEVICE_SET_REMOTE_PORT, bufPort );
}



// reboot
// Forces reboot of WiFly
// 
// Parameters: none
// Returns: none
#define SOFTWARE_REBOOT_RETRY_ATTEMPTS 5

void reboot() {
    char szCommand[SMALL_COMMAND_BUFFER_SIZE];

  GetBuffer_P(STI_WIFLYDEVICE_REBOOT, szCommand, SMALL_COMMAND_BUFFER_SIZE);
  // DebugPrint(szCommand);

  if (!SendCommandSimple( szCommand ,   WiFlyFixedPrompts[WIFLY_MSG_AOK] )) {
    //DebugPrint( GetBuffer_P(STI_WIFLYDEVICE_ERR_START_FAIL, szCommand, SMALL_COMMAND_BUFFER_SIZE));
    while (1) {}; // Hang. TODO: Handle differently?
  }
}



/* join*/
/* Parameters: None*/
/* Joins a network with previously supplied setSSID and passphrase.*/
/*												*/
/* returns true on success, false on failure*/
unsigned char joinSSID() {

  char szSSID[COMMAND_BUFFER_SIZE];

  getSSID( szSSID, COMMAND_BUFFER_SIZE );

  return joinSSID( szSSID );    
}


/* join*/
/* Parameters: None*/
/* Joins a network with given SSID and previously-provided passphrase. */
/*											 */	
/* returns true on success, false on failure.*/
/* Todo: support spaces in passphrase.*/
unsigned char join(char* pSSID) {

  unsigned char bJoined = FALSE;
  char szCmd[COMMAND_BUFFER_SIZE];

  setSSID(pSSID);
  GetBuffer_P(STI_WIFLYDEVICE_INDEX_JOIN, szCmd, COMMAND_BUFFER_SIZE);
  strncat( szCmd, pSSID, COMMAND_BUFFER_SIZE - strlen(szCmd) );
  
  char bufIndicator[INDICATOR_BUFFER_SIZE];
  char bufResponse [RESPONSE_BUFFER_SIZE];
    
  bJoined = SendCommand( szCmd,
                       GetBuffer_P(STI_WIFLYDEVICE_INDEX_ASSOCIATED, bufIndicator, INDICATOR_BUFFER_SIZE),
                       bufResponse, 
                       RESPONSE_BUFFER_SIZE, 
                       TRUE, 
                       JOIN_WAIT_TIME, FALSE, FALSE);
    
  return bJoined;    
}


/* GetBuffer_P*/
/* Returns pointer to a supplied Buffer, from PROGMEM based on StringIndex provided.*/
char* GetBuffer_P(const int StringIndex, char* pBuffer, int bufSize) {
  
  memset(pBuffer, '\0', bufSize);
  strncpy_P(pBuffer, (char*)pgm_read_word(&(WiFlyDevice_string_table[StringIndex])), bufSize);
  
  return pBuffer; 
}



/*
int WiFlySerial::peek() {
   return uart.peek();
}
size_t WiFlySerial::write(uint8_t byte) {
   return uart.write(byte);
}

// read
// Returns every character from WiFly via SoftwareSerial, and
// watches for WiFly in-stream control messages e.g. '*CLOS*'.
// WiFly note: externally-closed connections leave WiFly in data mode.

int WiFlySerial::read() {
   
   int iIn = uart.read();
   
   if (iIn == *pControl ) {
      if ( *(++pControl) == '\0') {
        bWiFlyConnectionOpen = false;
        
        // DebugPrint("***Caught Close in read()***" );
	    pControl = WiFlyFixedPrompts[WIFLY_MSG_CLOSE];
      }
   } else {
      pControl = WiFlyFixedPrompts[WIFLY_MSG_CLOSE];
   }
   return iIn;
}

int WiFlySerial::available() {
   return uart.available();
}
void WiFlySerial::flush() {
   uart.flush();
}
  
//using Print::write;



// setDebugChannel
// Conduit for debug output
// must not be a NewSoftSerial instance as incoming interrupts conflicts with outgoing data.
void WiFlySerial::setDebugChannel( Print* pChannel) {
  pDebugChannel = pChannel; 
}
void WiFlySerial::clearDebugChannel() {
 pDebugChannel = NULL; 
}

void WiFlySerial::DebugPrint( const char* pMessage) {
  if ( pDebugChannel )
    *pDebugChannel << (char*) pMessage;
}
void WiFlySerial::DebugPrint( const int iNumber) {
  if ( pDebugChannel )
    *pDebugChannel << iNumber;
}
void WiFlySerial::DebugPrint( const char ch) {
  if ( pDebugChannel )
    *pDebugChannel << (ch);
}
*/
