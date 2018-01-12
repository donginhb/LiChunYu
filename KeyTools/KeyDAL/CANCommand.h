#pragma once
#include "windows.h"

#define CAN_RTR_REMOTE 0x02;

#define CAN_RTR_DATA 0x00;

#define CAN_ID_EXT 0x04;

#define CAN_ID_STD 0x00;


#define CANCommand_PacketLen 24+12
class CANCommand
{
public:
	CANCommand(void);
	~CANCommand(void);

public:
	//taken from c struct in platform sdk 
	//         //�����趨���  2+1+1+1+1=6
	// 		public ushort Prescaler;
	// 		public byte BS1;
	// 		public byte BS2;
	// 		public byte SJW;
	// 		public byte Param;

	//��������	2+2+4+4+8+2+2=22			
	unsigned short TxStdId;//Min_Data = 0 and Max_Data = 0x7FF
	unsigned short TxBak1;
	unsigned int TxExtId;//Min_Data = 0 and Max_Data = 0x1FFFFFFF
	byte TxIDE;//CAN_Identifier_Type
	byte TxRTR;//CAN_remote_transmission_request
	byte TxDLC;//Min_Data = 0 and Max_Data = 8
	byte TxBak2;//����
	byte TxData[8];

	unsigned short TxRetry;
	unsigned short TxTimeOut;
	//������������  2+2+2+2+4=12
	byte bTxOnly;
	byte RxIDE;
	byte RxRetry;
	byte NeedRxPocket;
	unsigned short RxTimeOut;
	unsigned short NeedStdId;
	unsigned int NeedExtId;//Min_Data = 0 and Max_Data = 0x1FFFFFFF

	byte Packet[CANCommand_PacketLen];

	/************************************************************************/
	/* dll����������                                                        */
	/************************************************************************/
	int SerialReadTimeout;//�˴�ָ�ʱʱ����Ĭ��3s
	int SerialRetry;//�˴�ָ����pc�����Դ���

	//bool bNeedSaveData;//�˴�ָ��������Ƿ���Ҫ�洢
	
	int ReadRegAddr;//��̬�ֽڵ�ַ
	bool bNeedReadReg;//�Ƿ��ж�̬�ֽ���Ҫ�ı�	

	void MakePacket();
};

