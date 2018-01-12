#pragma once
#include "CANCommand.h"
extern "C" __declspec(dllexport) bool __stdcall CANReadEEPROMProc(int mode);

extern "C" __declspec(dllexport) void __stdcall GetEEPROMData(byte* pBuf,int& len);
 class CANReadEEPROM
 {
 public:
 	CANReadEEPROM(void);
 	~CANReadEEPROM(void);
 public:
 	CANCommand WakeUp;
 	CANCommand GetDataSeq[10];
 	int GetDataSeqLen;//��ʹ�õ�֡��
 
	byte ReadReg[100];//��Ҫ��ȡ�ĵ�ַ��Ԫ
	int ReadRegLen;//��Ҫ��ȡ�ĵ�ַ��Ԫ����

 	int PacketCount;
 };
void initCANReadEEPROM();

