#include "OnCarProc.h"
#include "Function.h"
#include "Variables.h"
#include "wfEEPROM.h"
#include "IRProc.h"
#include "Verify.h"
#include "wfDefine.h"
#include "lcyIRDecode.h"
#include "lcyHash.h"

void OnCarProc(void)
{
	uint8_t i;
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
						Check55and5fdata();
						Fix41and4Bdata();
						Fix2Dand37data();
						if(!(RomStateFlags.b55 && RomStateFlags.b5f))
						{
							if(CalcTimes_BF==0)
							{
								UpdateStepDatas();
							}
						}
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
							Check55and5fdata();
							Fix41and4Bdata();
							Fix2Dand37data();
							UpdateStepDatas();
							Adjust41and4BData();
							Adjust2Dand37Data();
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
	uint8_t i,x;
	gFlags.bFuncRet=0;	
	if(IRRxCount!=10)
		return;
	if(IRRxList[2]==0x00)
		return;
	RomData_ReadBytes(0x01,RomDatas,8);//��������
	for(i=0;i<8;i++)//����յ�������������
	{
		RomDatas[i]=RomDatas[i]^IRRxList[2+i];
	}
	for(i=0;i<4;i++)//��4���ֽ�,��24ָ��Ͳ������
	{
		RomDatas[4+i]=RomDatas[4+i]^Com24DataBak[i];
	}
	/************************************************************************/
	/* ʹ�ô�����һ                                                         */
	/************************************************************************/
	if(CalcTimes_D2<0x80)// �����λ
	{
		x=LeftTimes[0]&0x03;
		if(x==0)
		{
			ReverseRom(LeftTimesAddr[0]);//�л�ʹ�õ�λʹ�ö�
		}
		UsedDEC();
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[i];
	}
	lcyHashOnce();
	for(i=0;i<8;i++)
	{
		IRTxList[2+i]=lcyIRDecodeOut[i]^curHash[i];
	}
	IRTxList[0]=0x10;
	IRTxList[1]=0x27;
	IRTxCount=9;
	IRTxProc();
	gFlags.bFuncRet=1;
}
void ProcCommand_39(void)//Կ��ѧϰ����
{
	uint8_t i,x;
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
		x=RomData_ReadByte(0x09+i);
		RomDatas[i]=RomDatas[i]^x;
	}
	x=RomData_ReadByte(0x00);
	x=x>>6;
	RomDatas[4]=RomDatas[4]^x;

	RomDatas[5] ^= curHash[7];
	RomDatas[6] ^= curHash[6];
	RomDatas[7] ^= curHash[5];

	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[i];
	}
	lcyHashOnce();
	//����0x39ָ�����������ݲ�һ�£���ѧϰʧ��
	for(i=0;i<8;i++)
	{
		lcyIRDecodeOut[i]!=IRRxList[2+i];
		BAT_OFF();
		return;
	}
	//���Ĵ洢��Ϊѧϰ��״̬
	//�����ƶ�2λ�����:psw7,psw8,psw1,psw2,....,psw6
	RomData_ReadBytes(0x01,&RomDatas[2],8);
	RomDatas[0]=RomDatas[6];
	RomDatas[1]=RomDatas[7];
	//����SSID�����
	for(i=0;i<4;i++)
	{
		x=RomData_ReadByte(0x09+i);
		RomDatas[i]=RomDatas[i]^x;
	}
	x=RomData_ReadByte(0x00);
	x=x>>6;
	RomDatas[4]=RomDatas[4]^x;
	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[i];
	}
	lcyHashOnce();
	for(i=0;i<8;i++)
	{
		RomDatas[i]=lcyIRDecodeOut[i];
	}
	RomData_ReadBytes(0x09,&RomDatas[4],4);
	RomStateFlags.bRomWrited=1;
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
	uint8_t i,x;
	Get2425TxParam();
	IRTxList[0]=0x10;
	x=RomData_ReadByte(0x9e);
	if(GetBit(x,4))
		IRTxList[1]=0x24;//��д��
	else
		IRTxList[1]=0x25;//ѧϰ��
	Com24DataBak[3]=IRTxList[1];
	for(i=0;i<3;i++)
	{
		IRTxList[2+i]=LeftTimes[i];
		Com24DataBak[i]=IRTxList[2+i];
	}
	RomData_ReadBytes(0x09,&IRTxList[5],4);
	IRTxCount=9;
	IRTxProc();
}
void Get2425TxParam(void)
{
	uint8_t i,x;
	Check55and5fdata();
	Fix41and4Bdata();
	Fix2Dand37data();
	if(!RomStateFlags.b55 && !RomStateFlags.b5f)//55��5F�ξ���ƥ��
	{
		if(!RomStateFlags.b41 && !RomStateFlags.b4b)//41��4B��ȫ��ƥ��
		{
			//�����ж�
		}
		//sub_1490
		//�ݴ�rom�е�����
		UpdateStepDatas();
		if(bBATON()==GPIO_PIN_RESET)
		{
			Check55and5fdata();
			Fix41and4Bdata();
			Fix2Dand37data();
		}
		else
		{
			NVIC_SystemReset();//��λ
		}
	}
	if(RomStateFlags.b55 || RomStateFlags.b5f)//0x55 0x5F������1��ƥ��
	{
		//���69~70�洢���е�һ����Ϊ0xff�ĵ�Ԫ��������Ϊ�������
		//70-6f-6e-6d-6c-6b-6a-69-70
		GetCalcTimes69();
		RomData_ReadBytes(LeftTimesAddr[0],RomDatas,8);
		if(LeftTimes[0]!=0)
		{
			//��4�β��ù���
			if(CalcTimes_BF!=0)
			{
				for(i=0;i<8;i++)
				{
					lcyIRDecodeIn[i]=RomDatas[7-i];
				}
				HashCalc_N(CalcTimes_BF);
				for(i=0;i<8;i++)
				{
					RomDatas[7-i]=lcyIRDecodeOut[i];
					curHash[i]=lcyIRDecodeOut[i];
				}
			}
		}
		else//�������Ϊ0
		{
			//�����ж�
			UsedDEC();//�����λ
			//�ƻ�41��4B��ƥ��ε�У���ֽڣ�Ŀ���´β�ʹ�����ˣ�����һ��
			ReverseRom(LeftTimesAddr[1]+0x09);
			x=RomData_ReadByte(LeftTimesAddr[1]+0x08);//��λ����
			if(x!=0)//��λ����Ϊ0�����ƻ���λʹ�öε�У��
			{
				ReverseRom(LeftTimesAddr[2]+0x09);
				if(LeftTimesAddr[2]==0x37)//37�Σ���Ҫ�ƻ�7e�ε�У��
					ReverseRom(0x87);//(0x7e+0x09);
			}
			ReverseRom(LeftTimesAddr[0]+0x09);
			//�ص����ж�
			if(bBATON()==GPIO_PIN_RESET)
			{
				//D2�еĴ���Ҫ��λ0x80��Ŀ�ģ�
				CalcTimes_D2=0x80;
				return;
			}
			else
			{
				NVIC_SystemReset();//��λ
			}
		}
	}
}

