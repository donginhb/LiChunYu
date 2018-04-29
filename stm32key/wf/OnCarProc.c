#include "OnCarProc.h"
#include "Function.h"
#include "Variables.h"
#include "wfEEPROM.h"
#include "IRProc.h"
#include "Verify.h"
#include "wfDefine.h"
#include "lcyHash.h"
#include "lcyIRDecode.h"

void OnCarProc(void)
{
	uint8_t i;
	GetKeyParam();
	if(RomStateFlags.bRomWrited)//ROM��д��
	{
		while(1)
		{
			IRTx2425Frame();
			IRRxProc(100);
			if(gFlags.bFuncRet)//�յ�����֡
			{
				switch(IRCommand)
				{
				case 0x26:
					ProcCommand_26();
					if(gFlags.bFuncRet)
					{
						if(!RomStateFlags.bStudy)
						{
							IRRxProc(1000);
							if(IRCommand==0x39)
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
						while(1)
						{
							HAL_Delay(100);
							IRTx_10_28();
							HAL_Delay(100);
							IRTx_10_33_SSID();
#ifdef KeepPower
							if(bOnCarPower()==OnCarPowerState_OFF)
								NVIC_SystemReset();
#endif
						}
						//�ص����ж�
						//if(bBATON())
						//{
						//	BAT_OFF();
						//	HAL_Delay(100);
						//	if(bOnCarPower()==GPIO_PIN_SET)
						//		NVIC_SystemReset();//��λ							
						//}
						//else
						//{							
						//	NVIC_SystemReset();//��λ
						//}
					}
					break;
				case 0x7a:
					ProcCommand_7A();
					break;
				}
			}
		}
	}
	else//ûд�룬�ȴ�����ָ��
	{
		if(EE9e==0x0c)
		{
			ChangeKeyState(0x4c);
		}
		if(EE9e==0x4c)
		{
			RomData_ReadBytes(0x09,SSID,4);
			while(1)
			{
				IRTx_10_28();
				HAL_Delay(100);
				IRTx_10_33_SSID();
				HAL_Delay(100);
			}
		}
		else if(EE9e==0x21)
		{
			//sub_17B4
			while(1)
			{
				IRRxProc(0);//���Խ׶Σ�������ʱ
				switch(IRCommand)
				{
				case 0x0f:
					ProcCommand_0F();
					break;
				case 0x7a:
					ProcCommand_7A();
					break;
				case 0x04:
					ProgramWork(0x04,0x10);
					break;
				case 0x37:
					ProgramWork(0x37,0x10);
					break;
				default:
					LEDFlash();
					break;
				}
			}
		}
 		else
 		{
 			while(1)
 			{
 				IRRxProc(0);//���Խ׶Σ�������ʱ
 				if(gFlags.bFuncRet)
 				{
 					if(IRCommand==0x0f)
 					{
 						ProcCommand_0F();
 						continue;
 					}
 					break;
 				}
 				LEDFlash();
 			}			
 			switch(IRCommand)
 			{
 			case 0x7a:
 				ProcCommand_7A();
 				break;
 			case 0x0e:
 				ProgramWork(0x0e,0x02);
 				break;
 			case 0x00:
 				ProgramWork(0x00,0x03);
 				break;
 			case 0x02:
 				ProgramWork(0x02,0x10);
 				break;
 			case 0x35:
 				ProgramWork(0x35,0x10);
 				break;
 			case 0x03:
 				ProgramWork(0x03,0x14);
 				break;
 			default:
 				LEDFlash();
 				break;
 			}
 		}
	}
}
void ProgramWork(uint8_t keyType,uint8_t maxNum)
{
	uint8_t i,addr,lastAddr,x;
	uint8_t rxData[8];
	if(IRCommandParam!=0xff)//��д������
	{
		if((keyType==0x37) || (keyType==0x04))//��Ҫ�������ʹ��
		{
			for(i=0;i<8;i++)
				lcyIRDecodeIn[i]=IRRxList[3+i];
			lcyIRDecode();
			for(i=0;i<8;i++)
				rxData[i]=lcyIRDecodeOut[i];
		}
		else
		{
			for(i=0;i<8;i++)
				rxData[i]=IRRxList[3+i];
		}
		//35ָ��ȫ��0�Ĵ����ڼ���ֱ�ӽ���
		if(keyType==0x35)
		{
			x=0;
			for(i=0;i<8;i++)
			{
				x^=IRRxList[3];
			}
			if(x==0)
			{
				for(i=0;i<8;i++)
					lcyHashIn[i]=rxData[i];
				lcyHashOnce();
				for(i=0;i<8;i++)
					IRTxList[3+i]=lcyIRDecodeOut[i];
				IRTxList[0]=0x10;
				IRTxList[1]=0x05;
				IRTxList[2]=IRCommandParam;
				IRTxCount=11;
				IRTxProc();
				LEDFlash();
			}
		}
		if(IRCommandParam<maxNum)
		{
			addr=IRCommandParam*0x08;
			addr=addr+0x88;
			switch(keyType)
			{
			case 0x0e:
				//0x88+i*8			
				break;
			case 0x00:
				addr=addr-0x08;
				//0x80+i*8
				break;
			case 0x68:
				addr=addr-0x86;//;-0x08-0x7e;
				//0x02+i*8
				break;
			case 0x03:
				addr=addr-0x90;//-0x08-0x7e-0x02-0x02;
				//i*8-0x02
				break;
			default:
				addr=addr-0x88;//-0x08-0x7e-0x02;
				//0x00+i*8
				break;
			}		
			RomData_WriteBytes(addr,rxData,8);
			lastAddr=addr;
		}
		//���յ����ݽ���hashcal�󷵻�
		for(i=0;i<8;i++)
			lcyHashIn[i]=IRRxList[3+i];
		lcyHashOnce();
		for(i=0;i<8;i++)
			IRTxList[3+i]=lcyHashOut[i];
		IRTxList[0]=0x10;
		IRTxList[1]=0x05;
		IRTxList[2]=IRCommandParam;
		IRTxCount=11;
		IRTxProc();
	}
	else//д��洢�豸��
	{
		IRTxList[0]=0x10;
		IRTxList[1]=0x05;
		IRTxList[2]=IRCommandParam;
		for(i=0;i<8;i++)
			IRTxList[3+i]=0xff;
		IRTxCount=11;
		/************************************************************************/
		/* ��06��68��35��37д��ָ���Ҫ�Ƚ���һ��У��                         */
		/************************************************************************/
		if((IRCommand!=0x06) &&
			(IRCommand!=0x68) &&
			(IRCommand!=0x35) &&
			(IRCommand!=0x37) )
		{
			i=RomData_ReadByte(lastAddr+7);
			if(IRCommand==i)//7F��Ԫ��02��Ҳ����У����02������ʱ�������ݼ��
			{
				VerifyEEDatas(maxNum,lastAddr);
				if(!gFlags.bFuncRet)
				{
					IRTxProc();
					BAT_OFF();
					LED_ON();
					WaitCarPowerOff();
					//while(1);
				}
			}
		}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		for(i=0;i<8;i++)
			IRTxList[3+i]=0x00;
		if((IRCommand==0x0e) || (IRCommand==0x68))
		{
			IRTxProc();
			BAT_OFF();
			LED_ON();
			WaitCarPowerOff();
			//while(1);
		}
		while(1)
		{
			if(IRCommand==0x00)
			{
				ChangeKeyState(0x21);
			}
			else if((IRCommand==0x06) || (IRCommand==0x07))
			{
				ChangeKeyState(0x27);
			}
			else if (IRCommand==0x03)
			{
				ChangeKeyState(0x0c);
			}
			else
			{
				break;
			}
			IRTxProc();
			BAT_OFF();
			LED_ON();
			WaitCarPowerOff();
			//while(1);
		}
		CheckDataBlockVerify(0x55);
		x=RomData_ReadByte(0x55+8);
		if((!gFlags.bFuncRet) || (x>0x7f))
		{
			LEDFlash();
		}
		BAT_ON();
		x=x|0x03;//��λ����
		//����0x73��
		RomData_ReadBytes(0x55, RomDatas, 8);
		for (i = 0; i < 8; i++)
		{
			lcyHashIn[i] = RomDatas[7 - i];
		}
		lcyHashCalc(3);
		for (i = 0; i < 8; i++)
		{
			RomDatas[7 - i] = lcyHashOut[i];
		}
		RomDatas[8]=x;
		RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
		RomData_WriteBytes(0x73, RomDatas, 10);//д�뵱ǰ�����Σ���ʹ��ƥ��
		//����ʹ�ô���
		for(i=0;i<8;i++)
		{
			RomDatas[i]=0xff;
		}
		addr=0;
		i=0x7f;
		while(1)
		{
			if(i==x)
			{
				break;
			}
			i--;
			if(addr==0)
				addr=7;
			else
				addr--;
		}
		RomDatas[addr]=i;
		RomData_WriteBytes(0x69, RomDatas, 8);

		RomData_ReadBytes(0x5f, RomDatas, 9);	
		RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
		RomData_WriteBytes(0x5f, RomDatas, 10);
		ReverseRom(0x5f+9);

		// 	GetCalcTimes69();
		// 	GetLeftTimeBlock(LeftTimesH);
		// 	GetLeftTimeBlock(LeftTimesM);
		// 	GetLeftTimeBlock(LeftTimesL);

		x=RomData_ReadByte(0x98);
		if(x==0xff)//д���ص�ѹ
		{
			RomDatas[0]=0x73;
			RomDatas[1]=0x0c;
			RomData_WriteBytes(0x98,RomDatas,2);
		}
		RomDatas[0]=0x01;
		RomDatas[1]=0x01;
		RomDatas[2]=0x01;
		RomDatas[3]=0x01;
		x=RomData_ReadByte(0x00);
		if(GetBit(x,7)==0)
			RomDatas[4]=0xff;
		else
			RomDatas[4]=0x00;
		RomDatas[5]=0xff;
		RomData_WriteBytes(0x90,RomDatas,6);
		RomData_ReadBytes(0x01, RomDatas, 12);
		for(i=0;i<4;i++)
		{
			x=RomDatas[i];
			RomDatas[i]=RomDatas[7-i];
			RomDatas[7-i]=x;
		}
		x=RomDatas[8];
		RomDatas[8]=RomDatas[11];
		RomDatas[11]=x;

		x=RomDatas[9];
		RomDatas[9]=RomDatas[10];
		RomDatas[10]=x;
		RomData_WriteBytes(0x01,RomDatas,12);

		if((keyType==0x37) || (keyType==0x04))//��Ҫ�������ʹ��
		{
			ChangeKeyState(0x05);
		}
		else
		{
			ChangeKeyState(0x04);
		}
		IRTxProc();
		BAT_OFF();
		LED_ON();
		WaitCarPowerOff();
		//while(1);
	}
}
void ProcCommand_26(void)//��Ӧ0x27ָ��
{
	uint8_t i;
	gFlags.bFuncRet=0;	
	if(IRRxCount!=10)
		return;
	if(IRCommandParam==0x00)
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
	/*                                                                      */
	/************************************************************************/
	for(i=0;i<8;i++)
	{
		IRTxList[2+i]=lcyHashOut[i]^curHash[i];
	}
	IRTxList[0]=0x10;
	IRTxList[1]=0x27;
	IRTxCount=10;	
	IRTxProc();
	/************************************************************************/
	/* ʹ�ô�����һ																																	*/
	/************************************************************************/	
	UsedDEC();	
	BAT_ON();
	GetKeyParam();
	BAT_OFF();
	gFlags.bFuncRet=1;
}
void ProcCommand_0F(void)
{
	//��Щ�ط�δ���
	uint8_t i;
	IRTxList[0]=0x10;
	IRTxList[1]=0x0d;
	IRTxList[2]=0x00;
	for(i=0;i<8;i++)
	{
		IRTxList[3+i]=eeprom_8E[i];
	}
	IRTxCount=11;
	IRTxProc();
}
void ProcCommand_7A(void)//��ѯ����
{
	IRTxCount=0;
	switch(IRCommandParam)
	{
	case 0:
		//����һֱ����00
		break;
	case 1:
		//��غ���һֱ����00
		break;
	case 2:
		//RFһֱ����00
		break;
	case 3:
		//�ȴ���������
		WaitCarPowerOff();
		break;
	case 4:
		//�в�����IO����
		//�ȴ���������
		WaitCarPowerOff();
		break;
	case 5:
		//��LED
		LED_ON();
		WaitCarPowerOff();
		break;
	case 6:
		//�յ�����д��98-9D
		RomData_WriteBytes(0x98,&IRRxList[3],6);
	case 7:
		RomData_ReadBytes(0x98,&IRTxList[3],8);
		IRTxList[0]=0x10;
		IRTxList[1]=0x7b;
		IRTxList[2]=IRCommandParam;
		IRTxCount=9;
		break;
	case 8:
		RomData_ReadBytes(0x9c,&IRTxList[3],4);
		IRTxList[0]=0x10;
		IRTxList[1]=0x7b;
		IRTxList[2]=IRCommandParam;
		IRTxList[3]=ROMVer;
		IRTxList[4]=0x45;//��ص���
		IRTxList[7]=RomData_ReadByte(0x00);
		IRTxList[8]=0x01;
		IRTxCount=9;		
		break;
	}
	if(IRTxCount!=0)
	{
		//HAL_Delay(50);
		IRTxProc();
	}
	WaitCarPowerOff();
}
void ProcCommand_39(void)//Կ��ѧϰ����
{
	uint8_t i;
	gFlags.bFuncRet=0;	
	if(IRRxCount!=10)
		return;
	if(IRCommandParam==0x00)
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
	SetBit_uint8(EE9e,4);
	ChangeKeyState(EE9e);
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

	IRTxList[2]=LeftTimes69;
	IRTxList[3]=LeftTimes[LeftTimesM];
	IRTxList[4]=LeftTimes[LeftTimesH];

	RomData_ReadBytes(0x09,&IRTxList[5],4);
	IRTxCount=9;
	IRTxProc();
}
void WaitCarPowerOff(void)
{
	while(1)
	{
		if(bOnCarPower()==OnCarPowerState_OFF)
			NVIC_SystemReset();
	}
}
void LEDFlash(void)
{
	while(1)
	{
		LED_ON();
		HAL_Delay(200);
		LED_OFF();
		HAL_Delay(200);
		WaitCarPowerOff();
	}
}
