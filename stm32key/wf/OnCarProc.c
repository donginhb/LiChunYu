#include "OnCarProc.h"
#include "Function.h"
#include "Variables.h"
#include "wfEEPROM.h"
#include "IRProc.h"
#include "Verify.h"
#include "wfDefine.h"
#include "lcyHash.h"

void OnCarProc(void)
{
	uint8_t i;
	while (1)
	{
		GetKeyParam();
		UsedDEC();
	}
	GetKeyParam();
	if(!RomStateFlags.bRomWrited)//ROMδ����д��
	{
		while(1)
		{
			IRTx2425Frame();
			IRRxProc(100);
			if(gFlags.bFuncRet)//�յ�����֡
			{
				IRRxDataProc();
				if(gFlags.bIRRxErr)//�������մ�������
					continue;
				switch(IRRxList[1])
				{
				case 0x26:
					ProcCommand_26();
					if(gFlags.bFuncRet)
					{
						if(!RomStateFlags.bStudy)
						{
							IRRxProc(100);
							if(IRRxList[1]==0x39)
							{
								ProcCommand_39();
								if(!gFlags.bFuncRet)
									continue;
							}
						}
						//�������ж�
						for(i=0;i<4;i++)
							RomDatas[i]=0x01;
						RomData_WriteBytes(0x90,RomDatas,4);						
						//�ص����ж�
						if(bBATON())
						{
							BAT_OFF();
							HAL_Delay(100);
							if(bOnCarPower()==GPIO_PIN_SET)
								NVIC_SystemReset();//��λ							
						}
						else
						{							
							NVIC_SystemReset();//��λ
						}
					}
					break;
				case 0x7a:
					break;
				}
			}
		}
	}
}
void ProcCommand_26(void)//��Ӧ0x27ָ��
{
	uint8_t i;
	gFlags.bFuncRet=0;	
	if(IRRxCount!=10)
		return;
	if(IRRxList[2]==0x00)
		return;
	for(i=0;i<8;i++)//��������յ�������������
	{
		RomDatas[i]=PSW[i]^IRRxList[2+i];
	}
	//��4�ֽ���2425ָ����������
	RomDatas[4]^=LeftTimes69;
	RomDatas[5]^=LeftTimes[LeftTimesM];
	RomDatas[6]^=LeftTimes[LeftTimesH];
	if(RomStateFlags.bStudy)
		RomDatas[7]^=0x24;
	else
		RomDatas[7]^=0x25;	
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=RomDatas[i];
	}
	lcyHashOnce();	
	/************************************************************************/
	/* ʹ�ô�����һ                                                         */
	/************************************************************************/
	UsedDEC();	
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	for(i=0;i<8;i++)
	{
		IRTxList[2+i]=lcyHashOut[i]^curHash[i];
	}
	IRTxList[0]=0x10;
	IRTxList[1]=0x27;
	IRTxCount=9;
	IRTxProc();
	gFlags.bFuncRet=1;
}
void ProcCommand_39(void)//Կ��ѧϰ����
{
	uint8_t i;
	gFlags.bFuncRet=0;	
	if(IRRxCount!=10)
		return;
	if(IRRxList[2]==0x00)
		return;
	BAT_ON();
	//�����ƶ�һλ�����:psw8,psw1,psw2,....,psw7
	RomData_ReadBytes(0x01,&RomDatas[1],8);
	RomDatas[0]=RomDatas[8];
	//����SSID�����
	for(i=0;i<4;i++)
	{
		RomDatas[i]=RomDatas[i]^SSID[i];
	}
	RomDatas[4]=RomDatas[4]^EE00;

	RomDatas[5] ^= curHash[7];
	RomDatas[6] ^= curHash[6];
	RomDatas[7] ^= curHash[5];

	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=RomDatas[i];
	}
	lcyHashOnce();
	//����0x39ָ�����������ݲ�һ�£���ѧϰʧ��
	for(i=0;i<8;i++)
	{
		if(lcyHashOut[i]!=IRRxList[2+i])
		{
			BAT_OFF();
			return;
		}
	}
	//���Ĵ洢��Ϊѧϰ��״̬
	//�����ƶ�2λ�����:psw7,psw8,psw1,psw2,....,psw6
	RomData_ReadBytes(0x01,&RomDatas[2],8);
	RomDatas[0]=RomDatas[6];
	RomDatas[1]=RomDatas[7];
	//����SSID�����
	for(i=0;i<4;i++)
	{
		RomDatas[i]=RomDatas[i]^SSID[i];
	}
	RomDatas[4]=RomDatas[4]^EE00;
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=RomDatas[i];
	}
	lcyHashOnce();
	for(i=0;i<8;i++)
	{
		RomDatas[i]=lcyHashOut[i];
	}
	for(i=0;i<4;i++)
	{
		RomDatas[4+i]=SSID[i];
	}
	RomStateFlags.bStudy=1;
	RomData_WriteBytes(0x88,RomDatas,8);
	RomDatas[0]=RomData_ReadByte(0x9e);
	SetBit_uint8(RomDatas[0],4);
	RomDatas[1]=0x00-RomDatas[0];
	RomData_WriteBytes(0x9e,RomDatas,2);
	BAT_OFF();
	gFlags.bFuncRet=1;
}
void IRTx2425Frame(void)
{
	IRTxList[0]=0x10;
	if(RomStateFlags.bStudy)
		IRTxList[1]=0x24;//��д��
	else
		IRTxList[1]=0x25;//ѧϰ��

	IRTxList[2]=LeftTimes[LeftTimes69];
	IRTxList[3]=LeftTimes[LeftTimesM];
	IRTxList[4]=LeftTimes[LeftTimesH];

	RomData_ReadBytes(0x09,&IRTxList[5],4);
	IRTxCount=9;
	IRTxProc();
}

