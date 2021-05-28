#ifndef WIFLY_H
#define WIFLY_H

/************************************************************************
Title:    Wifly module (RN-VX) library
Author:   EP
File:     $Id: wifly.h,v 1.0.0.0 2012/11/16 20:14:38  $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR, tested on AATmega328P at 8 Mhz
License:  GNU General Public License 
Usage:    see Wifly manual

LICENSE:
    Copyright (C) 2012

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


/*
** constants and macros
*/


//#define RX_PIN  2
//#define TX_PIN  3

#define COMMAND_BUFFER_SIZE 64
#define SMALL_COMMAND_BUFFER_SIZE 20
#define RESPONSE_BUFFER_SIZE 80
#define INDICATOR_BUFFER_SIZE 16
#define IP_ADDR_WIDTH  18
#define UC_N_IP_BYTES  4      /* number of bytes per IP Address (need test for IPv6) */

/*default ports as set in WiFly */
#define WIFLY_DEFAULT_REMOTE_PORT 80    
#define WIFLY_DEFAULT_LOCAL_PORT 2000  
#define WIFLY_DEFAULT_BAUD_RATE 9600

#define COMMAND_MODE_GUARD_TIME 	300 		/* in milliseconds.  Must be at least 250ms. */
#define DEFAULT_WAIT_TIME			1000UL  	/* waiting time for a command */
#define ATTN_WAIT_TIME				1000UL  	/* waiting time for a reponse after a $$$ attention signal */
#define JOIN_WAIT_TIME 				10000UL  	/* joining a network could take longer */
#define SERVING_WAIT_TIME			300000UL    /* 5 minutes default wait time for serving a connection in milliseconds */
#define WIFLY_RESTART_WAIT_TIME		2000UL  	/* wait time after issuing reboot command. */
#define WIFLY_TIME_SYNC_INTERVAL 	100UL   	/* 15 minute interval between clock resyncs */
#define WIFLY_NTP_SYNC_INTERVAL     "60"    	/* sync with NTP server every hour */

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 5
#define COMMAND_RETRY_ATTEMPTS 3

/*WiFly Responses */
#define PROMPT_NONE					0x0
#define PROMPT_EXPECTED_TOKEN_FOUND	0x1
#define PROMPT_READY 			   	0x2
#define PROMPT_CMD_MODE 	   		0x4
#define PROMPT_AOK 			    	0x8
#define PROMPT_OTHER 				0x10
#define PROMPT_CMD_ERR 				0x20
#define PROMPT_TIMEOUT 				0x40
#define PROMPT_OPEN 				0x80
#define PROMPT_CLOSE 				0x100
#define PROMPT_OPEN_ALREADY			0x200

#define N_PROMPTS 				9
#define WIFLY_MSG_EXPECTED  	0
#define WIFLY_MSG_AOK  			1
#define WIFLY_MSG_CMD  			2
#define WIFLY_MSG_ERR  			3
#define WIFLY_MSG_PROMPT  		4
#define WIFLY_MSG_PROMPT2  		5
#define WIFLY_MSG_CLOSE  		6
#define WIFLY_MSG_OPEN  		7
#define WIFLY_MSG_OPEN_ALREADY  8

/*Auth Modes for Network Authentication*/
#define WIFLY_AUTH_OPEN        0    /* Open (default) */ 
#define WIFLY_AUTH_WEP         1    /* WEP-128 */
#define WIFLY_AUTH_WPA1        2    /* WPA1 */
#define WIFLY_AUTH_WPA1_2      3    /* Mixed-mode WPA1 and WPA2-PSK */
#define WIFLY_AUTH_WPA2_PSK    4    /* WPA2-PSK */
#define WIFLY_AUTH_ADHOC       6    /* Ad-hoc, join any Ad-hoc network */

/*Join modes for joining networks*/
#define WIFLY_JOIN_MANUAL      0    /* Manual, do not try to join automatically */
#define WIFLY_JOIN_AUTO        1    /* Try to join the access point that matches the stored SSID, passkey and channel. (Default) */
#define WIFLY_JOIN_ANY         2    /* Join ANY access point with security matching the stored authentication mode. Ignores stored SSID and searches for strongest signal. */
#define WIFLY_JOIN_MAKE_ADHOC  4    /* Create an Ad-Hoc network using stored SSID, IP and netmask.  Channel MUST be set. DHCP should be 0. */

/*DHCP modes*/
#define WIFLY_DHCP_OFF         0    /* DHCP OFF, use static IP address */
#define WIFLY_DHCP_ON          1    /* DHCP ON, get IP, gateway from Access Point. */
#define WIFLY_DHCP_AUTO        2    /* DHCP AUTO-IP, generally used with Adhoc networks */
#define WIFLY_DHCP_CACHE       3    /* DHCP CACHE, use previous IP address if lease is not expired. */

/*IP modes: UDP and TCP*/
#define WIFLY_IPMODE_UDP       0x02;    /* Same as W5100_defn's SmMR::UDP */
#define WIFLY_IPMODE_TCP       0x01;    /* Same as W5100_defn's SmMR::TCP */

#define WIFLY_STATUS_OFFLINE    0
#define WIFLY_STATUS_ONLINE     1

#define TRUE	1
#define FALSE	0




/*************************************************************************
 Utility functions
**************************************************************************/

//char* IP_ArrayToBuffer( const uint8_t* pIP, char* pBuffer, int buflen);
//char* BufferToIP_Array(char* pBuffer, uint8_t* pIP)  ;


void WiFlySerial(unsigned char pinReceive, unsigned char pinSend);    
    
/* Initialization */
unsigned char begin();  /* Initialises WiFly interface */

/* utilities for collecting results or scanning for indicators. */
int ScanForPattern(char* responseBuffer, const int bufsize, const char *pExpectedPrompt, 
                   const unsigned char bCapturing, 
				   const unsigned long WaitTime, 
				   const unsigned char bPromptAfterResult ); //= TRUE, = DEFAULT_WAIT_TIME, = TRUE );


    
char* ExtractDetail(char* pCommand, char* pDetail, const int buflen, const char* pFrom, const char* pTo);
  
char* ExtractDetailIdx(const int idxCommand, char* pDetail, int buflen, const int idxSearch, const int idxStop);
    
int CaptureUntilPrompt(char* responseBuffer, const int bufsize, const char *pExpectedPrompt, 
					   const unsigned long WaitTime ); //= DEFAULT_WAIT_TIME  );

int peek();

//virtual size_t write(uint8_t byte);
//virtual int read();
//virtual int available();
//virtual void flush();
    
int drain ();
  
//using Print::write;


    
// Status
    
// Obtain current device status flags
long    getDeviceStatus();  // refreshes device status flags.
unsigned char isAssociated();
unsigned char isTCPConnected();
unsigned char isAuthenticated();
unsigned char isifUp();
int     getChannel();
unsigned char isDNSfound();
unsigned char isDNScontacted();
unsigned char isInCommandMode();
unsigned char isConnectionOpen();
            
// Information
// Device Info
char* showNetworkScan( char* pNetScan, const int buflen);
char* getLibraryVersion(char* buf, int buflen);

// IP info
char* getSSID(char* buf, int buflen);
char* getDeviceID(char* buf, int buflen);    
char* getIP(char* buf, int buflen);
char* getNetMask(char* buf, int buflen);
char* getGateway(char* buf, int buflen);
char* getDNS(char* buf, int buflen);
char* getMAC(char* buf, int buflen);
char* getNTP(char* buf, int buflen);
char* getNTP_Update_Frequency(char* buf, int buflen);
unsigned long getTime();
char* getRSSI(char* pBuf, int buflen);
char* getBattery(char* pBuf, int buflen);

// Transmit / Receive / available through exposed SoftwareSerial
//    SoftwareSerial uart;    
    
// Configuration Generic Wifi methods
    
// Generic Wifi methods
unsigned char setSSID( const char* pSSID);    
unsigned char setPassphrase( const char* pPassphrase);
unsigned char setDeviceID( const char* pHostname);
unsigned char setNTP(const char* pNTP_IP);
unsigned char setNTP_Update_Frequency(const char* pFreq);
unsigned char setNTP_UTC_Offset(float fltUTC_Offset_hours);
unsigned char setIP( const char* pIP);
unsigned char setNetMask( const char* pNM);
unsigned char setGateway( const char* pGW);
unsigned char setDNS( const char* pDNS);
unsigned char setChannel( const char* pChannel);
unsigned char setLocalPort( int iNewLocalPort); // = WIFLY_DEFAULT_LOCAL_PORT);
unsigned char setRemotePort( int iNewRemotePort); // = WIFLY_DEFAULT_REMOTE_PORT);
unsigned char setProtocol( unsigned int iProtocol);
unsigned char setAuthMode( int iAuthMode);
unsigned char setJoinMode( int iJoinMode);        
unsigned char setDHCPMode(const int iModeDHCP);
    
// wifi network Association
    
// Joins ssid set with setSSID
unsigned char joinSSID();
unsigned char join( char* pSSID);
    
    // Leaves current SSID.
unsigned char leave();
                 
    // Generic utility
unsigned char StartCommandMode(char* pBuffer, const int bufSize); //= NULL, = COMMAND_BUFFER_SIZE);
unsigned char exitCommandMode();     
void    reboot();
    
    // Client Connection
unsigned char openConnection(const char* pURL, const unsigned long WaitTime); // = JOIN_WAIT_TIME  );
unsigned char closeConnection(unsigned char bSafeClose); // = TRUE);
    
    // Server Connection - waits for a client to connect
unsigned char serveConnection(  const unsigned long reconnectWaitTIme ); //= SERVING_WAIT_TIME );
    
    // Open-format for RN 131C/G commands
unsigned char SendInquiry(char *Command, char* pBuffer, const int bufsize ); // = RESPONSE_BUFFER_SIZE );

unsigned char SendInquirySimple(char *Command);

unsigned char SendCommand( char *Command,  const char *SuccessIndicator, char* pBuffer, const int bufsize, 
                const unsigned char bCollecting, const unsigned long WaitTime, 
                const unsigned char bClear, const unsigned char bPromptAfterResult); // = TRUE, = DEFAULT_WAIT_TIME  = TRUE,  = TRUE );

unsigned char SendCommandSimple( char *Command, const  char *SuccessIndicator);

    

// debug utilities - use Serial : not NewSoftSerial as it will affect incoming stream.
// should change these to use stream <<
//    void    setDebugChannel( Print* pDebug);
//    Print*  getDebugChannel( )  { return pDebugChannel; };
//    void    clearDebugChannel();
//    void    DebugPrint( const char* pMessage);
//    void    DebugPrint( const int iNumber);
//    void    DebugPrint( const char ch);

    
    

    unsigned char GetCmdPrompt();
    char*   GetBuffer_P(const int StringIndex, char* pBuffer, int bufSize);
    char*   ExtractLineFromBuffer(const int idString,  char* pBuffer, const int bufsize, const char* pStartPattern, const char* chTerminator);
    unsigned char issueSetting( int idxCommand, const char* pParam);

    // Internal debug channel.
//    Print*  pDebugChannel;


// static  WiFlySerialv2 wifi ( ARDUINO_RX_PIN, ARDUINO_TX_PIN);

#endif




