#include "Function.h"
#include "wfEEPROM.h"
#include "Variables.h"
#include "..\..\..\WF_Device\wfDefine.h"
#include "..\..\..\WF_Device\Verify.h"
#include "lcyHash.h"
#include "IRProc.h"
#include "lcyIRDecode.h"

void GetKeyParam(void)
{
	uint8_t x;
	RomData_ReadBytes(0x9e,RomDatas,2);
	if(RomDatas[0]==0xff && RomDatas[1]==0xff)//��δ��ʼ��
	{
		BAT_ON();
		RomDatas[0]=ROM_9E;
		RomDatas[1]=ROM_9F;
		RomData_WriteBytes(0x9e,RomDatas,2);//��ʼ��ROM�汾
	}
	RomData_ReadBytes(0x9e,RomDatas,2);
	x=RomDatas[0]+RomDatas[1];
	RomStateFlags.bRomWrited=0;
	RomStateFlags.bStudy=0;
	if(x!=0)
		NVIC_SystemReset();//�洢���޷���ʼ����ϵͳ��λ
	if(GetBit(RomDatas[0],2)!=0)//д����
	{
		RomStateFlags.bRomWrited=1;
	}
	if(GetBit(RomDatas[0],4)!=0)//ѧϰ��
	{
		RomStateFlags.bStudy=1;
	}
	if(RomStateFlags.bRomWrited)//д���ˣ���ȡϵͳ����
	{
		//ʹ�ô�����λ
		//���69~70�洢���е�һ����Ϊ0xff�ĵ�Ԫ��������Ϊ�������
		//70-6f-6e-6d-6c-6b-6a-69-70
		GetCalcTimes69();
		//ʹ�ô�����λ
		GetLeftTimeBlock(LeftTimesH);
		if(LeftTimes[LeftTimesH]==0xff)//ee�����쳣���ݴ�
		{
			Fix2Dand37data();
		}
		//ʹ�ô�����λ
		GetLeftTimeBlock(LeftTimesM);
		if(LeftTimes[LeftTimesM]==0xff)//ee�����쳣���ݴ�
		{
			Fix41and4Bdata();
		}
		//ʹ�ô�����λ���öΣ�ͬʱ�ҵ���ǰhash
		Check55and5fdata();
	}
}
//���ʹ�ô���
void GetLeftTimeBlock(uint8_t nBlock)
{
	uint8_t t1,t2,addr1,addr2,tTop,x,y;
	switch(nBlock)
	{
	case LeftTimesH:
		addr1=0x2d;
		addr2=0x37;
		tTop=0x2f;
		break;
	case LeftTimesM:
		addr1=0x41;
		addr2=0x4b;
		tTop=0x1f;
		break;
	case LeftTimesL:
		addr1=0x55;
		addr2=0x5f;
		break;
	default:
		return;
	}
	CheckDataBlockVerify(addr1);
	t1=RomDatas[8];
	if(!gFlags.bFuncRet)
	{
		t1=0xff;		
	}
	CheckDataBlockVerify(addr2);
	t2=RomDatas[8];	
	if(!gFlags.bFuncRet)
	{
		t2=0xff;				
	}
	LeftTimesAddr[nBlock]=0xff;//�쳣ֵ
	if((t1!=0xff) && (t2!=0xff))
	{	
		if(nBlock!=LeftTimesL)
		{
			if(t1>t2)//�ô��
			{
				if((t2==0) && (t1==tTop))//���磺00 1f��ʹ��00
				{
					LeftTimesAddr[nBlock]=addr2;
					OtherLeftTimesAddr[nBlock]=addr1;
					LeftTimes[nBlock]=t2;
				}
				else//��������ʹ�ô��
				{
					LeftTimesAddr[nBlock]=addr1;
					OtherLeftTimesAddr[nBlock]=addr2;
					LeftTimes[nBlock]=t1;
				}			
			}
			else//t2>t1
			{
				if((t1==0) && (t2==tTop))//���磺00 1f��ʹ��00
				{
					LeftTimesAddr[nBlock]=addr1;
					OtherLeftTimesAddr[nBlock]=addr2;
					LeftTimes[nBlock]=0;
				}
				else//��������ʹ�ô��
				{
					LeftTimesAddr[nBlock]=addr2;
					OtherLeftTimesAddr[nBlock]=addr1;
					LeftTimes[nBlock]=t2;
				}
			}
			return;//����
		}
		else
		{
			tTop=LeftTimes69&0xfc;
			if(t1==tTop)
			{
				LeftTimesAddr[nBlock]=addr1;
				OtherLeftTimesAddr[nBlock]=addr2;
				LeftTimes[nBlock]=t1;	
				return;
			}
			else if(t2==tTop)
			{
				LeftTimesAddr[nBlock]=addr2;
				OtherLeftTimesAddr[nBlock]=addr1;
				LeftTimes[nBlock]=t2;
				return;
			}
			else//���ζ���ƥ�䣬��Ҫ�������ɵ�λhash
			{
				t1=0xff;
				t2=0xff;
			}
		}
	}
	else if(t1!=0xff)//��һ��ƥ�䣬���������ɵڶ���
	{
		LeftTimesAddr[nBlock]=addr1;
		OtherLeftTimesAddr[nBlock]=addr2;
		LeftTimes[nBlock]=t1;		
	}
	else if(t2!=0xff)
	{
		LeftTimesAddr[nBlock]=addr2;
		OtherLeftTimesAddr[nBlock]=addr1;
		LeftTimes[nBlock]=t2;
	}
	if(t1!=0xff || t2!=0xff)//��Ҫ�������ɵڶ���
	{
		switch(nBlock)
		{
		case LeftTimesH:
			if(LeftTimes[LeftTimesH]==0)//��λΪ0�������ɵڶ���
				break;
			else
			{	
				t2=LeftTimes[LeftTimesH]-1;
				if(t2>=0x24)
				{
					t1=0x24;
					addr1=0x25;//RomData_ReadBytes(0x25,RomDatas,8);//49152+49152+49152
				}
				else if(t2>=0x18)
				{
					t1=0x18;
					addr1=0x1d;//RomData_ReadBytes(0x1d,RomDatas,8);//49152+49152
				}
				else if(t2>=0xc)
				{
					t1=0x0c;
					addr1=0x15;//RomData_ReadBytes(0x15,RomDatas,8);//49152+49152
				}
				else
				{
					t1=0x00;
					addr1==0x0d;//RomData_ReadBytes(0x0d,RomDatas,8);//49152
				}
				t1=t2-t1;
				FixDataBlock(OtherLeftTimesAddr[LeftTimesH],addr1,4096,t1,LeftTimes[LeftTimesH]-1);
			}
		case LeftTimesM:
			if(LeftTimes[LeftTimesM]==0)//��λΪ0������Ҫ���λ��λ
			{
				if (LeftTimes[LeftTimesH]==0)//��λΪ0�����޷���λ������������λ�ڶ���
					break;
				else//��λ������λ
				{
					FixDataBlock(OtherLeftTimesAddr[LeftTimesM],OtherLeftTimesAddr[LeftTimesH],128,0x1f,0x1f);
				}
			}
			else
			{
				FixDataBlock(OtherLeftTimesAddr[LeftTimesM],LeftTimesAddr[LeftTimesH],128,LeftTimes[LeftTimesM]-1,LeftTimes[LeftTimesM]-1);
			}
			break;
		case LeftTimesL:
			if(LeftTimes[LeftTimesL]==0)//��λΪ0������Ҫ����λ��λ
			{
				if ((LeftTimes[LeftTimesM]==0) && (LeftTimes[LeftTimesH]==0))//��λ����λ��Ϊ0�����޷���λ���������ɵ�λ�ڶ���
					break;
				else//��λ
				{
					FixDataBlock(OtherLeftTimesAddr[LeftTimesL],OtherLeftTimesAddr[LeftTimesM],0x7c,1,0x7c);
				}
			}
			else
			{
				FixDataBlock(OtherLeftTimesAddr[LeftTimesL],LeftTimesAddr[LeftTimesM],LeftTimes[LeftTimesL]-4,1,LeftTimes[LeftTimesL]-4);
			}
			break;
		}
	}
	else//���ζ���ƥ�䣬��Ҫ��С�Ĵ�����������
	{
		if(t1<t2)
		{
			LeftTimesAddr[nBlock]=addr1;
			OtherLeftTimesAddr[nBlock]=addr2;
			LeftTimes[nBlock]=t1;
		}
		else
		{
			LeftTimesAddr[nBlock]=addr2;
			OtherLeftTimesAddr[nBlock]=addr1;
			LeftTimes[nBlock]=t2;
		}
		switch(nBlock)
		{
		case LeftTimesH:
			if(LeftTimes[LeftTimesH]==0)//��λΪ0������ɵ�һ��
			{
				RomData_ReadBytes(0x0d,RomDatas,8);
				RomDatas[8]=0;
				RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
				RomData_WriteBytes(LeftTimesAddr[LeftTimesH],RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
			}
			else
			{	
				t2=LeftTimes[LeftTimesH]-1;
				if(t2>=0x24)
				{
					t1=0x24;
					addr1=0x25;//RomData_ReadBytes(0x25,RomDatas,8);//49152+49152+49152
				}
				else if(t2>=0x18)
				{
					t1=0x18;
					addr1=0x1d;//RomData_ReadBytes(0x1d,RomDatas,8);//49152+49152
				}
				else if(t2>=0xc)
				{
					t1=0x0c;
					addr1=0x15;//RomData_ReadBytes(0x15,RomDatas,8);//49152+49152
				}
				else
				{
					t1=0x00;
					addr1==0x0d;//RomData_ReadBytes(0x0d,RomDatas,8);//49152
				}
				t1=t2-t1;
				FixDataBlock(OtherLeftTimesAddr[LeftTimesH],addr1,4096,t1,LeftTimes[LeftTimesH]-1);//�޸�-1��
				FixDataBlock(LeftTimesAddr[LeftTimesH],OtherLeftTimesAddr[LeftTimesH],4096,1,LeftTimes[LeftTimesH]);//�޸���ǰ
			}
		case LeftTimesM:
			if(LeftTimes[LeftTimesM]==0)//��λΪ0������Ҫ���λ��λ
			{
				RomData_ReadBytes(LeftTimes[LeftTimesH],RomDatas,8);
				RomDatas[8]=0;
				RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
				RomData_WriteBytes(LeftTimesAddr[LeftTimesM],RomDatas,10);
				if (LeftTimes[LeftTimesH]==0)//��λΪ0��������ɵ�ǰ��			
					break;
				else//��λ������λ
				{
					FixDataBlock(OtherLeftTimesAddr[LeftTimesM],OtherLeftTimesAddr[LeftTimesH],128,0x1f,0x1f);
				}
			}
			else
			{
				FixDataBlock(OtherLeftTimesAddr[LeftTimesM],LeftTimesAddr[LeftTimesH],128,LeftTimes[LeftTimesM]-1,LeftTimes[LeftTimesM]-1);
				FixDataBlock(LeftTimesAddr[LeftTimesM],OtherLeftTimesAddr[LeftTimesM],128,1,LeftTimes[LeftTimesM]);
			}
			break;
		case LeftTimesL:
			LeftTimes[LeftTimesL]=LeftTimes69&0xfc;			
			if(LeftTimes[LeftTimesL]==0)//��λΪ0������Ҫ����λ��λ
			{
				RomData_ReadBytes(LeftTimes[LeftTimesM],RomDatas,8);
				RomDatas[8]=0;
				RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
				RomData_WriteBytes(LeftTimesAddr[LeftTimesL],RomDatas,10);
				if ((LeftTimes[LeftTimesM]==0) && (LeftTimes[LeftTimesH]==0))//��λ����λ��Ϊ0�����޷���λ���������ɵ�λ�ڶ���
					break;
				else//��λ
				{
					FixDataBlock(OtherLeftTimesAddr[LeftTimesL],OtherLeftTimesAddr[LeftTimesM],0x7c,1,0x7c);
				}				
			}
			else
			{
				FixDataBlock(OtherLeftTimesAddr[LeftTimesL],LeftTimesAddr[LeftTimesM],LeftTimes[LeftTimesL]-4,1,LeftTimes[LeftTimesL]-4);
				FixDataBlock(LeftTimesAddr[LeftTimesL],OtherLeftTimesAddr[LeftTimesL],4,1,LeftTimes[LeftTimesL]);
			}
			break;
		}
	}
}
//fixAddr:Ҫ�޸��ĵ�ַ
//hashAddr��ʹ�õ�hash�ĵ�ַxiuf
//stepLen:�޸���������
//times��Ŀ�����
void FixDataBlock(uint8_t fixAddr,uint8_t hashAddr,uint16_t stepLen,uint8_t loopTimes,uint8_t leftTiems)
{
	uint8_t i;
	RomData_ReadBytes(hashAddr,RomDatas,8);
	if(loopTimes!=0)
	{
		for(i=0;i<8;i++)
		{
			lcyIRDecodeIn[i]=RomDatas[7-i];
		}
		for(i=0;i<loopTimes;i++)
		{
			HashCalc_N(stepLen);
		}
		for(i=0;i<8;i++)
		{
			RomDatas[7-i]=lcyIRDecodeOut[i];
		}
	}
	RomDatas[8]=leftTiems;
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
	RomData_WriteBytes(fixAddr,RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
}
//�޸�2d��37������
//ʹ�ü���Ԥ���49152���������ݴ�
void Fix2Dand37data(void)
{
	uint8_t t2d,t37,i,x;
	t2d=RomData_ReadByte(0x35);
	t37=RomData_ReadByte(0x3f);
	//��С��,��𿪹�ȥ׷��
	if(t2d<t37)
	{
		LeftTimesAddr[LeftTimesH]=0x2d;
		LeftTimes[LeftTimesH]=t2d;
	}
	else
	{
		LeftTimesAddr[LeftTimesH]=0x37;
		LeftTimes[LeftTimesH]=t37;
	}
	if(LeftTimes[LeftTimesH]==0)//��λ�����þ�
	{
		//ʹ��0d��
		RomData_ReadBytes(0x0d,RomDatas,8);
		RomDatas[8]=LeftTimes[LeftTimesH];
		RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
		RomData_WriteBytes(LeftTimesLAddr[LeftTimesH],RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
		return;
	}
	IRTx_10_33_SSID();
	/************************************************************************/
	/* ��ʼ�ݴ�ʹ�ô�����                                                   */
	/************************************************************************/		
	if(LeftTimes[LeftTimesH]>=0x24)
	{
		x=0x24;
		RomData_ReadBytes(0x25,RomDatas,8);//49152+49152+49152
	}
	else if(LeftTimes[2]>=0x18)
	{
		x=0x18;
		RomData_ReadBytes(0x1d,RomDatas,8);//49152+49152
	}
	else if(LeftTimes[2]>=0xc)
	{
		x=0x0c;
		RomData_ReadBytes(0x15,RomDatas,8);//49152+49152
	}
	else
	{
		x=0x00;
		RomData_ReadBytes(0x0d,RomDatas,8);//49152
	}
	x=LeftTimes[LeftTimesH]-x;
	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[7-i];
	}
	for(i=0;i<x;i++)
	{
		HashCalc_N(4096);
	}
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[LeftTimesH];
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
	RomData_WriteBytes(LeftTimesLAddr[LeftTimesH],RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
	/************************************************************************/
	/* �����ݴ���һ��                                                       */
	/************************************************************************/
	HashCalc_N(4096);
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[LeftTimesH]-1;
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
	if(RomDatas[9]==0xff)
		RomDatas[9]=0;
	else
		RomDatas[9]=0xff;
	if(LeftTimesLAddr[LeftTimesH]==0x2d)
	{
		RomData_WriteBytes(0x37,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
	else
	{
		RomData_WriteBytes(0x2d,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
}
//�޸�41��4b������
//ʹ�ø�λ���������ݴ�
void Fix41and4Bdata(void)
{
	uint8_t t41,t4b,i;
	uint16_t C0BF;
	t41=RomData_ReadByte(0x49);
	t4b=RomData_ReadByte(0x53);
	//��С��,��𿪹�ȥ׷��
	if(t41<t4b)
	{
		LeftTimesAddr[LeftTimesM]=0x41;
		LeftTimes[LeftTimesM]=t41;
	}
	else
	{
		LeftTimesAddr[LeftTimesM]=0x4b;
		LeftTimes[LeftTimesM]=t4b;
	}
	/************************************************************************/
	/* ��ʼ�ݴ�ʹ�ô�����                                                   */
	/************************************************************************/	
	C0BF=0x80;
	C0BF=C0BF*LeftTimes[LeftTimesM];
	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[7-i];
	}
	HashCalc_N(C0BF);
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[1];
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
	RomData_WriteBytes(LeftTimesLAddr[LeftTimesH],RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
	/************************************************************************/
	/* �����ݴ���һ��                                                       */
	/************************************************************************/
	HashCalc_N(0x80);
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[LeftTimesM]-1;
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
	if(RomDatas[9]==0xff)
		RomDatas[9]=0;
	else
		RomDatas[9]=0xff;
	if(LeftTimesLAddr[LeftTimesM]==0x41)
	{
		RomData_WriteBytes(0x4b,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
	else
	{
		RomData_WriteBytes(0x41,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
}
//�޸�55��5f������
//ʹ����λ���������ݴ�
void Fix55and5Fdata(void)
{
	uint8_t t55,t5f,i;
	uint16_t C0BF;
	t55=RomData_ReadByte(0x5d);
	t5f=RomData_ReadByte(0x67);
	//��С��,��𿪹�ȥ׷��
	if(t55<t5f)
	{
		LeftTimesAddr[LeftTimesM]=0x41;
		LeftTimes[LeftTimesM]=t55;
	}
	else
	{
		LeftTimesAddr[LeftTimesM]=0x4b;
		LeftTimes[LeftTimesM]=t5f;
	}
	/************************************************************************/
	/* ��ʼ�ݴ�ʹ�ô�����                                                   */
	/************************************************************************/	
	C0BF=0x80;
	C0BF=C0BF*LeftTimes[LeftTimesM];
	for(i=0;i<8;i++)
	{
		lcyIRDecodeIn[i]=RomDatas[7-i];
	}
	HashCalc_N(C0BF);
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[1];
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
	RomData_WriteBytes(LeftTimesLAddr[LeftTimesH],RomDatas,10);//д�뵱ǰ�����Σ���ʹ��ƥ��
	/************************************************************************/
	/* �����ݴ���һ��                                                       */
	/************************************************************************/
	HashCalc_N(0x80);
	for(i=0;i<8;i++)
	{
		RomDatas[7-i]=lcyIRDecodeOut[i];
	}
	RomDatas[8]=LeftTimes[LeftTimesM]-1;
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);	
	if(RomDatas[9]==0xff)
		RomDatas[9]=0;
	else
		RomDatas[9]=0xff;
	if(LeftTimesLAddr[LeftTimesM]==0x41)
	{
		RomData_WriteBytes(0x4b,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
	else
	{
		RomData_WriteBytes(0x41,RomDatas,10);//д�뵱ǰ�����Σ���ʹ�䲻ƥ��
	}
}
//���55��5f������
void Check55and5fdata(void)
{
	uint8_t t55,t5f,t;
	RomStateFlags.b55=0;
	RomStateFlags.b5f=0;	
	CheckDataBlockVerify(0x55);
	if(gFlags.bFuncRet)
	{
		RomStateFlags.b55=1;		
	}
	CheckDataBlockVerify(0x5f);
	if(gFlags.bFuncRet)
	{
		RomStateFlags.b5f=1;		
	}
	t=LeftTimes[0]&0xfc;//Ӧ�ô洢�Ĵ���
	t55=0xff;
	t5f=0xff;
	LeftTimesAddr[0]=0xff;//��ֵΪ����ֵ�������ж��Ƿ����
	if(RomStateFlags.b55 && RomStateFlags.b5f)//ȫƥ�䣬ʹ�ô������
	{
		t55=RomData_ReadByte(0x55+8);
		t5f=RomData_ReadByte(0x5f+8);
		if(t55==t)
		{
			LeftTimesAddr[0]=0x55;//ʹ��55��
		}
		if(t5f==t)
		{
			LeftTimesAddr[0]=0x5f;//ʹ��5f��
		}
	}
	else if(RomStateFlags.b55)//ֻ��һ��ƥ��
	{
		t55=RomData_ReadByte(0x55+8);
		if(t55==t)
		{
			LeftTimesAddr[0]=0x55;//ʹ��55��
		}
	}
	else if(RomStateFlags.b5f)//ֻ��һ��ƥ��
	{
		t5f=RomData_ReadByte(0x5f+8);
		if(t5f==t)
		{
			LeftTimesAddr[0]=0x5f;//ʹ��55��
		}
	}
	if(LeftTimesAddr[0]==0xff)//û�ҵ�ƥ��ģ���Ҫ�ݴ�
	{

	}
}
//sub_1442_USEDEC:
//ʹ�ô�����һ
void UsedDEC(void)
{
	uint8_t x;
	GetCalcTimes69();	
	if(LeftTimes[0]==0)//0-1��Ҫ��λ��д��0x7f
		x=0x7f;
	else
		x=LeftTimes[0]-1;
	RomData_WriteByte(LeftTimesLAddr,0xff);//ԭ��ַд0xff��дһ��д���һ��
	if(LeftTimesLAddr==0x69)
	{	
		RomData_WriteByte(0x70,x);
	}
	else
	{
		RomData_WriteByte(LeftTimesLAddr-1,x);
	}
}

//ÿʹ��4�θ������ݴ洢��
//sub_1490
void UpdateStepDatas(void)
{
	uint8_t c4,x,ram9b,i;
	if(RomStateFlags.b55  && RomStateFlags.b5f)
	{
		return;
	}
	if(!RomStateFlags.b41 && !RomStateFlags.b4b)//41��4Bȫ��ƥ�䣬���ݴ�
	{
		Adjust41and4BData();
	}
	if(RomStateFlags.b55 || RomStateFlags.b5f)
	{
		CalcTimes_D2=CalcTimes_D2-4;
	}
	Adjust55and5FData();
}
//55��5F��ȫ��ƥ��ʱ����
//sub_14B1
void Adjust55and5FData(void)
{
	uint8_t addr,i;
	if(CalcTimes_D2>0x7f)
	{
		//ȫƥ��ʱ�����н�λ����
		if(!(RomStateFlags.b41 && RomStateFlags.b4b))//41��4B��ȫ��ƥ��ʱ,������
		{
			Adjust2Dand37Data();
		}
		CalcTimes_D2=0x7c;
		if(LeftTimesAddr[1]==0x41)
		{
			addr=0x4b;
		}
		else
		{
			addr=0x41;
		}
	}
	else
	{
		addr=LeftTimesAddr[1];
	}
	RomData_ReadBytes(addr,RomDatas,8);	
	if(CalcTimes_D2!=0)
	{
		for(i=0;i<8;i++)
		{
			lcyIRDecodeIn[i]=RomDatas[7-i];
		}
		HashCalc_N(CalcTimes_D2);
		for(i=0;i<8;i++)
		{
			RomDatas[7-i]=lcyIRDecodeOut[i];
		}
	}
	RomDatas[8]=CalcTimes_D2;
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
	if(LeftTimesAddr[0]==0x55)
	{
		RomData_ReadBytes(0x5f,RomDatas,10);
	}
	else
	{
		RomData_ReadBytes(0x55,RomDatas,10);
	}
	Check55and5fdata();
}
//41��4B��ȫ��ƥ��ʱ����
//sub_1562
void Adjust41and4BData(void)
{
	uint8_t x,i;
	uint16_t C0BF;
	if(RomStateFlags.b41 && RomStateFlags.b4b)
		return;//ȫƥ���򷵻�
	if(!RomStateFlags.b2d && !RomStateFlags.b37)//2D��37��ȫ��ƥ��ʱ,������
	{
		Adjust2Dand37Data();
	}	
	if(LeftTimes[1]==0)//��λʹ�ô����þ�
	{
		LeftTimes[1]=0x1f;
		if(RomStateFlags.b2d && RomStateFlags.b37)//2D��37�β���ȫƥ��ʱ,������
		{
			Adjust2Dand37Data();
		}		
		if(GetBit(LeftTimes[2],0))
		{
			RomData_ReadBytes(0x2d,RomDatas,8);
		}
		else
		{
			RomData_ReadBytes(0x37,RomDatas,8);
		}
	}
	else
	{
		LeftTimes[1]--;
		if(GetBit(LeftTimes[2],0))
		{
			RomData_ReadBytes(0x37,RomDatas,8);
		}
		else
		{
			RomData_ReadBytes(0x2d,RomDatas,8);
		}
	}
	if(LeftTimes[1]!=0)
	{		
		C0BF=0x80;
		C0BF=C0BF*LeftTimes[1];
		for(i=0;i<8;i++)
		{
			lcyIRDecodeIn[i]=RomDatas[7-i];
		}
		HashCalc_N(C0BF);
		for(i=0;i<8;i++)
		{
			RomDatas[7-i]=lcyIRDecodeOut[i];
		}
	}
	RomDatas[8]=LeftTimes[1];
	RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
	if(GetBit(LeftTimes[1],0))
	{
		RomData_ReadBytes(0x41,RomDatas,10);
	}
	else
	{
		RomData_ReadBytes(0x4b,RomDatas,10);
	}
	Fix41and4Bdata();
}
//2D��37��ȫ��ƥ��ʱ����
//sub_15BE
void Adjust2Dand37Data(void)
{
	uint8_t x,ram9b,i;
	if(RomStateFlags.b2d && RomStateFlags.b37)
		return;//ȫƥ���򷵻�
	if(LeftTimes[2]==0)//ʹ�ô����þ�
	{
		Fix2Dand37data();
		return;
	}
	if(!RomStateFlags.b2d)//��2d�β�ƥ�䣬��7e�����ݼ��ص�2d����
	{
		IRTx_10_33_SSID();
		RomData_ReadBytes(0x7e,RomDatas,8);
		x=RomData_ReadByte(0x86);
		x=x&0x7f;
		RomDatas[8]=x;
		RomDatas[9]=GetVerify_byteXOR(RomDatas,9);
		RomData_WriteBytes(0x2d,RomDatas,10);
	}
	if(!RomStateFlags.b37)//��37�β�ƥ��
	{
		IRTx_10_33_SSID();
		ram9b=0;
		CheckDataBlockVerify(0x7e);
		if(gFlags.bFuncRet)
		{
			x=RomData_ReadByte(0x86);
			if(x!=0xff)
			{
				ram9b=x;
			}
		}		
		LeftTimes[2]--;
		if(GetBit(ram9b,0))//������ʹ��37�����ݼ���
		{
			RomData_ReadBytes(0x37,RomDatas,8);
		}
		if(ram9b==0)//37��7E�ξ���ƥ�䣬���ݵ�ǰ�Ĵ�����rom���������ɸ�λhash
		{
			//sub_1643
			IRTx_10_33_SSID();
			if(LeftTimes[2]>=0x24)
			{
				ram9b=0x24^0x80;
				RomData_ReadBytes(0x25,RomDatas,8);//49152+49152+49152
			}
			else if(LeftTimes[2]>=0x12)
			{
				ram9b=0x12^0x80;
				RomData_ReadBytes(0x1d,RomDatas,8);//49152+49152
			}
			else
			{
				ram9b=0x00^0x80;
				RomData_ReadBytes(0x15,RomDatas,8);//49152
			}
		}		
		while(1)
		{
			ram9b++;
			x=ram9b&0x7f;
			if(x==LeftTimes[2])
			{
				ram9b=LeftTimes[2];
			}
			for(i=0;i<8;i++)
			{
				lcyIRDecodeIn[i]=RomDatas[7-i];
			}
			HashCalc_N(4096);
			for(i=0;i<8;i++)
			{
				RomDatas[7-i]=lcyIRDecodeOut[i];
			}
			RomDatas[8]=ram9b;
			RomDatas[9]=GetVerify_byteXOR(RomDatas,9);				
			if(GetBit(ram9b,0))//����д��0x37��
			{
				RomData_WriteBytes(0x37,RomDatas,10);
			}
			else//ż��д��0x37��
			{
				RomData_WriteBytes(0x7e,RomDatas,10);
			}
			if(ram9b<0x80)
				break;
		}
		Fix2Dand37data();
	}
}
//��תָ����ַ�����ݣ���Ϊ0xff��дΪ0����֮������0xff��дΪ0xff
void ReverseRom(uint8_t addr)
{
	if(RomData_ReadByte(addr)!=0xff)
	{
		RomData_WriteByte(addr,0xff);
	}
	else
	{
		RomData_WriteByte(addr,0);
	}
}
//sub_13EB_Get69to70_FirstNFF
//���69~70�洢���е�һ����Ϊ0xff�ĵ�Ԫ��������Ϊ�������
//70-6f-6e-6d-6c-6b-6a-69-70
void GetCalcTimes69(void)
{
	uint8_t i,x;
	RomData_ReadBytes(0x69,RomDatas,8);
	for(i=8;i!=0;i--)
	{
		x=i-1;
		if(RomDatas[x]!=0)
			break;
	}
	if(i==0)//û�з�ff��Ԫ,������0x69д��00�����ٵĵ�λ����
	{
		LeftTimes69=0;
		RomData_WriteByte(0x69,0);
// 		CalcTimes_D2=0;
// 		CalcTimes_BF=0;
		LeftTimesLAddr=0x69;
		return;
	}
	else
	{
		//�ݴ����ж���һ���Ƿ�ҲΪ��ff
		if(i==1)//�������ж�
		{
			LeftTimes69=RomDatas[0];
// 			CalcTimes_D2=CalcTimes[0]&0xfc;
// 			CalcTimes_BF=CalcTimes[0]&0x03;
			LeftTimesLAddr=0x69;
			return;
		}
		else 
		{
			x=i-2;
			if(RomDatas[x]!=0xff)//������������ʹ����һ��
			{
				LeftTimes69=RomDatas[x];
// 				CalcTimes_D2=CalcTimes[0]&0xfc;
// 				CalcTimes_BF=CalcTimes[0]&0x03;
				LeftTimesLAddr=0x69+x;				
				ReverseRom(0x68+i);//����rom
			}
			else//û����ʹ�ô�ֵ
			{
				x=i-1;
				LeftTimes69=RomDatas[x];
// 				CalcTimes_D2=CalcTimes[0]&0xfc;
// 				CalcTimes_BF=CalcTimes[0]&0x03;
				LeftTimesLAddr=0x69+x;
			}
		}
		//rom�ݴ�:���ж�ǰһ���Ƿ�ҲΪ��ff,ֻ��70��Ԫʱ��Ҫ�ж�
		if(i==8)//70��Ԫ
		{
			if(RomDatas[0]!=0xff)
			{
				ReverseRom(0x69);
			}
		}
	}
}
void HashCalc_N(uint32_t nCount)
{
	uint32_t i;
	for(i=0;i<nCount;i+=4)
	{
		lcyHashCalc(4);
		IRTx_10_33_SSID();
	}
}
//������ݶ�У��
void CheckDataBlockVerify(uint8_t Addr)
{
	uint8_t x;
	gFlags.bFuncRet=0;	
	RomData_ReadBytes(Addr,RomDatas,10);
	if(RomDatas[8]>0x7f)
	{
		gFlags.bFuncRet=0;
		return;
	}
	x=GetVerify_byteXOR(RomDatas,9);
	if(x==RomDatas[9])
		gFlags.bFuncRet=1;
}

