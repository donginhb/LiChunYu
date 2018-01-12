#pragma once

enum ErrorCode
{
	ErrorCode_NoErr=0,


	ErrorCode_NoSerialProcCallback=256,
	
	ErrorCode_SerialProcError=300,

	ErrorCode_CanReadEEPROM_nErr=400

};


#define Command_CheckDevice 1



#define Command_SetCANBaudRate 100
#define Command_CANRead 101



#define Command_IRWork 200 //�����ȡ��Ҫ���ȡ
#define Command_IREn 201	//���⹩�翪��