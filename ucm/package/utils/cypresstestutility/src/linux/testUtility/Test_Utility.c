/*
 * Command line utility for downloading FW
 * Copyright (C) 2013 Cypress Semiconductor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>

#include "../../common/header/CyUSBSerial.h"
#include "../../common/header/CyUSBBootloader.h"

#define CY_MAX_DEVICES 30
#define CY_MAX_INTERFACES 4

typedef struct _CY_DEVICE_STRUCT {
    int deviceNumber;
    int interfaceFunctionality[CY_MAX_INTERFACES];
    bool isI2c;
    bool isSpi;
    int numInterface; 
}CY_DEVICE_STRUCT;

CY_DEVICE_STRUCT *glDevice;
int i2cDeviceIndex[CY_MAX_DEVICES][CY_MAX_INTERFACES];
unsigned char *deviceNumber = NULL;
int cyDevices, i2cDevices = 0; 
static char numDevices = 0;
int selectedDeviceNum = -1, selectedInterfaceNum = -1;
bool exitApp = false;
unsigned short pageAddress = -1;
short readWriteLength = -1;
bool deviceAddedRemoved = false;
unsigned char read_buffer[512];
int getUserInput()
{
    char userInput[6], x;
    int output,i = 0;
    bool isDigit = true;
    x = getchar();
    while (x != '\n'){
        if (i < 5){
            userInput[i] = x;
            i++;
        }
        if (!isdigit(x))
            isDigit = false;

        x = getchar();
    }
    userInput[i] = '\0';
    if (isDigit == false)
        return -1;
    output = atoi(userInput);
    return output;
}


unsigned int GetCheckSum(unsigned int *buff, unsigned int length)
{
    unsigned int i,val;
    unsigned int checkSum =0;

char temp[4];
       char *p; 
       unsigned int *q;

    for (i = 0; i < length; i++) // start at 12th byte
    {
        printf("checkSum[%d] %x\n",i, buff[i]);
	p = (char *)&buff[i];
                temp[3] = *p++;
                temp[2] = *p++;
                temp[1] = *p++;
                temp[0] = *p;
#if 0
  	q = (char *)buff[i];
                *q++ = temp[3];
                *q++ = temp[2];
                *q++ = temp[1];
                *q = temp[0];
 #endif
       checkSum += buff[i];
	//q = (unsigned int *)&temp[0];
	//checkSum += *q;	
    }

    return checkSum;
}      


void deviceHotPlug () {

    CY_RETURN_STATUS rStatus;
    deviceAddedRemoved = true;
    selectedDeviceNum = -1;
    selectedInterfaceNum = -1;
    printf ("Device of interest Removed/Added \n");
    rStatus = CyGetListofDevices (&numDevices);
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    printListOfDevices (false);
}

int main (int argc, char **argv)
{
    int index = 0, i, j, userInput;
    int output;
    int count=0,h=0,cnt;
    int size_buffer,size_checksum;
    FILE *fp=NULL;
    CY_HANDLE handle;
    unsigned char buff[516];
    int silicon_id;
    int tempSelectedDeviceNum, tempSelectedInterfaceNum;
    CY_RETURN_STATUS rStatus;
    signal (SIGUSR1, deviceHotPlug);
    char src_file[100];
    char id[4];

    memset(buff,0,sizeof(buff));
    

    glDevice = (CY_DEVICE_STRUCT *)malloc (CY_MAX_DEVICES *sizeof (CY_DEVICE_STRUCT));
    if (glDevice == NULL){
        printf ("Memory allocation failed ...!! \n");
        return -1;
    }
    rStatus = CyLibraryInit ();
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Doing library init Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    rStatus = CyGetListofDevices (&numDevices);
    if (rStatus != CY_SUCCESS) {
        printf ("CY:Error in Getting List of Devices: Error NO:<%d> \n", rStatus);
        return rStatus;
    }
    printListOfDevices(true);
    do {

        printf ("-------------------------------------------------------------------\n");
        printf ("1: Print list of devices \n");
        if (selectedDeviceNum != -1 && selectedInterfaceNum != -1){
            printf ("2: Change device selection--selected device: [Device number %d] : [Interface No %d]",\
                    selectedDeviceNum, selectedInterfaceNum);
            if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_I2C)
                printf (" : I2C\n");
            else if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_SPI)
                printf (" : SPI\n");
            else if (glDevice[selectedDeviceNum].interfaceFunctionality[selectedInterfaceNum] == CY_TYPE_UART)
               printf ("UART : \n");
            else
                printf (" : NA\n");

        }
        else    
            printf ("2: Select device...No device selected !!\n");


        userInput = getUserInput();
        if (userInput < 1 || userInput > 5){
            printf ("Wrong selection code ... Enter again \n");
            continue;
        }

        switch (userInput){

            case 1:
                printListOfDevices(true);
                break;
            case 2:
                if (cyDevices == 0) {
                    printf ("No device of interest connected ...!!\n");
                    continue;
                }
                printf ("Enter Device number to be selected.. \n");
                tempSelectedDeviceNum = getUserInput();
                if (tempSelectedDeviceNum >= cyDevices || tempSelectedDeviceNum == -1){
                    printf ("Wrong device selection \n");
                    continue;
                }
                printf ("Enter interface number..\n");
                tempSelectedInterfaceNum = getUserInput();
               
                if (tempSelectedInterfaceNum >= glDevice[tempSelectedDeviceNum].numInterface || 
                        tempSelectedInterfaceNum == -1) {
                    printf ("Wrong interface Selection selection \n");
                    continue;
                }

                if (deviceAddedRemoved == true) {
                    printf ("Device of interest was added/removed .... Print and select from new list\n");
                    continue;
                }
                selectedDeviceNum = tempSelectedDeviceNum;
                selectedInterfaceNum = tempSelectedInterfaceNum;
                

                rStatus = CyOpen (selectedDeviceNum,selectedInterfaceNum , &handle);
             
                if (rStatus == CY_SUCCESS){
                   
                }
                else
                {
                    printf("fail \n");
                }
            
                fp = fopen(argv[1],"rb+");
        
                rStatus = CyFlashConfigEnable(handle,1);

                if (rStatus == CY_SUCCESS){
                                  }
                else
                {
                    printf("Manufacturing mode of FLASH is not configured");
                }
             

                if(fp == NULL)
                {
                    printf("\n fopen() Error!!!\n");
                    return 1;
                }
                printf("\n File opened successfully\n");

		//char temp_buff[520] = {0};

		
                if(sizeof(buff) != fread(buff,1,516,fp))
                {
                    printf("\n fread() failed\n");
                    return 1;
                }

#if 1
		//memcpy(buff, &temp_buff[4], 516);
 		int i=0;
		for(i=0; i<516; i++)
		{
			printf("%x ", buff[i]);
			if((i+1)%4 == 0)
				printf("\n");
		}	
#endif
                printf("\n Bytes successfully read \n");
                rStatus=CyGetSiliconID(handle,&silicon_id);
                if (rStatus == CY_SUCCESS){
                }
                else
                {
                    printf("Not correct ID");
                } 

          
                rStatus=CyReadDeviceConfig(handle,&read_buffer);
                if (rStatus == CY_SUCCESS){

                }
                else
                {
                    printf("Not done");
                }  
#if 0
		char temp[4] = {0};
                /* Reversing the byte for big endian */
                for(i=0; i<516; i++)
                {
                        if(i!=0)
                        {
                                if(i%4 == 0)
                                {
                                        temp[0] = buff[i-1];
                                        temp[1] = buff[i-2];
                                        temp[2] = buff[i-3];
                                        temp[3] = buff[i-4];

                                        buff[i-1] = temp[3];
                                        buff[i-2] = temp[2];
                                        buff[i-3] = temp[1];
                                        buff[i-4] = temp[0];
                                }

                        }
                }

                printf("Inverted buffer\n");
                for(i=0; i<516; i++)
                {
                        printf("%x ", buff[i]);
                        if((i+1)%4 == 0)
                                printf("\n");
                }
            
 #endif
   
                size_buffer = sizeof(read_buffer);

		char array[12] = {0x43,0x59,0x55,0x53,0x01,0x00,0x00,0x00,0x01,0xEF,0xBA,0xD7};
              
                memcpy (&buff[4], &array[0], 12);
		printf("Anoop Bytes sending to config function\n");
		//int i=0;
                for(i=0; i<516; i++)
                {
                        printf("%x ", buff[i]);
                        if((i+1)%4 == 0)
                                printf("\n");
                }

		//for(i=0; i<28; i++)	
		// printf("read_buffer[%d] %x\n", i, read_buffer[i]);


                size_checksum= GetCheckSum((unsigned int *)(&buff[16]), 125);

		printf("checksum %x\n", size_checksum);
          

                //buff[12]= (size_checksum & 0xFF);
                //buff[13]= ((size_checksum >> 8) & 0xFF);
                //buff[14]= ((size_checksum >> 16) & 0xFF);
                //buff[15]= ((size_checksum >> 24) & 0xFF);

		//buff[12] = 0x83; 
		//buff[13] = 0xa6;
               	//buff[14] = 0x97;
		//buff[15] = 0x75;

               //if((silicon_id == 0x08A1) || (silicon_id == 0x08A2) || (silicon_id == 0x08A3))
                {
      
               rStatus= CyWriteDeviceConfig(handle,&buff[4]);
                if (rStatus == CY_SUCCESS){
                   printf(" Programming Flash is done");
                }
                else
                {
                    printf("Not done");
                }
               }
               //else
               //{
               //printf("wrong silicon id");
               // }

                fclose(fp);

                printf("\n File stream closed \n");

                

                rStatus= CyClose(handle);
                if (rStatus == CY_SUCCESS){
                   
                }
                else
                {
                    printf("Not closed");
                }


                break;
            case 3:
                exitApp = true;
                CyLibraryExit ();
                break;    
        }
    }while (exitApp == false);
    free (glDevice);
}
bool isCypressDevice (int deviceNum) {
    CY_HANDLE handle;
    unsigned char interfaceNum = 0;
    unsigned char sig[6];
int op;
    CY_RETURN_STATUS rStatus;
    rStatus = CyOpen (deviceNum, interfaceNum, &handle);
    if (rStatus == CY_SUCCESS){
        rStatus = CyGetSignature (handle, sig);
        if (rStatus == CY_SUCCESS){
            CyClose (handle);
            return true;
        }
        else {
            CyClose (handle);
            return false;
        }
    }
    else 
        return false;
}
void printListOfDevices (bool isPrint)
{
    int  index_i = 0, index_j = 0, i, j, countOfDevice = 0, devNum;
    int length, index = 0, numInterfaces, interfaceNum;
    bool set1 = false;

    unsigned char deviceID[CY_MAX_DEVICES];
    unsigned char functionality[64];
    CY_DEVICE_INFO deviceInfo;
    CY_DEVICE_CLASS deviceClass[CY_MAX_INTERFACES];
    CY_DEVICE_TYPE  deviceType[CY_MAX_INTERFACES];
    CY_RETURN_STATUS rStatus;

    deviceAddedRemoved = false; 
    CyGetListofDevices (&numDevices);
   
    for (i = 0; i < numDevices; i++){
        for (j = 0; j< CY_MAX_INTERFACES; j++)
            glDevice[i].interfaceFunctionality[j] = -1;
    }
    if (isPrint){
        printf ("\n\n---------------------------------------------------------------------------------\n");
        printf ("Device Number | VID | PID | INTERFACE NUMBER | FUNCTIONALITY \n");
        printf ("---------------------------------------------------------------------------------\n");
    }
    cyDevices = 0;
    for (devNum = 0; devNum < numDevices; devNum++){
        rStatus = CyGetDeviceInfo (devNum, &deviceInfo);
        interfaceNum = 0;
        if (!rStatus)
        {
            
            strcpy (functionality, "NA");
            numInterfaces = deviceInfo.numInterfaces;
            glDevice[index].numInterface = numInterfaces;
            cyDevices++;

            while (numInterfaces){
                if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_VENDOR)
                {
                    glDevice[index].deviceNumber = devNum;
                    switch (deviceInfo.deviceType[interfaceNum]){
                        case CY_TYPE_I2C:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_I2C;
                            strcpy (functionality, "VENDOR_I2C");
                            glDevice[index].isI2c = true;
                            break;
                        case CY_TYPE_SPI:
                            glDevice[index].interfaceFunctionality[interfaceNum] = CY_TYPE_SPI;
                            strcpy (functionality, "VENDOR_SPI");
                            glDevice[index].isSpi = true;
                            break;
                        default:
                            strcpy (functionality, "NA");
                            break;    
                    }
                }
                else if (deviceInfo.deviceClass[interfaceNum] == CY_CLASS_CDC){
                    strcpy (functionality, "NA");
                }
                if (isPrint) {
                    printf ("%d             |%x  |%x    | %d     | %s\n", \                
                            index, \
                            deviceInfo.vidPid.vid, \
                            deviceInfo.vidPid.pid,  \
                            interfaceNum, \
                            functionality \
                           );
                }
                interfaceNum++;
                numInterfaces--;
            }
            index++;
        }
    }
    if (isPrint){
        printf ("---------------------------------------------------------------------------------\n\n");
    }
    printf("Cydevices %d",cyDevices);
}

