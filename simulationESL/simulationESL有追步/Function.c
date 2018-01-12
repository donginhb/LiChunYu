#include "HardwareProfile.h"
const unsigned char rxHeader[2][10]=
{
	{0x90,0x91,0x92,0x93,0x94,0xa0,0xb0,0xc0,0xc5,0xc7},
	{0x90,0x98,0xa0,0xa8,0xb0,0xb8,0xc0,0xc8,0xe0,0xe8}
};
void UpdateEEPROM(void)
{
	unsigned char addr,eep,i,psw;
	eeprom_write(0x90,0x2c);
	addr=uartRxList[1]<<4;//�ҵ���ַ
	eep=eeprom_read(0xf0+uartRxList[1]);
	if(eep==0xff)//��һ��ʹ�ô�Կ��
	{
		eeprom_write(0xf0+uartRxList[1],0x01);//ǰ8Ϊ��λ
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
			lcyHashIn[i]=uartRxList[2+i];
		}
		lcyHashOnce();
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i+8,lcyHashOut[i]);
		}
	}
	else if(eep==0)//��8Ϊ��λhash
	{
		//�滻ԭǰ8��λ����
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
		}
		//�����ı�־λ���ĵ�ǰ8Ϊ��λ��ַ
		eeprom_write(0xf0+uartRxList[1],0x01);		
	}
	else//ǰ8Ϊ��λhash
	{
		addr+=8;
		//�滻ԭ��8��λ����
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
		}
		//�����ı�־λ���ĵ���8Ϊ��λ��ַ
		eeprom_write(0xf0+uartRxList[1],0x00);		
	}
	//���������,������
	for(i=0;i<8;i++)
	{
		psw=eeprom_read(0xd0+i);
		lcyHashIn[i]=uartRxList[2+i]^psw;
	}
	lcyHashOnce();
	for(i=0;i<8;i++)
	{
		uartTxList[2+i]=lcyHashOut[i];
	}
	uartTxList[0]=0x97;
	uartTxList[1]=uartRxList[1];
}
void ProcCommand_work_0x3x(void)
{
	unsigned char addr,eep,i;
	unsigned int step;
	eep=eeprom_read(0x90);
	if(eep==0x22)//�ո�renew 
	{
		uartTxList[0]=0x88;
		for(i=0;i<9;i++)
		{
			uartTxList[i]=0x00;
		}
	}
// 	eep=eeprom_read(0xff);
// 	if(eep==0xff)
// 		return;
	addr=uartRxList[1]<<4;
	eep=eeprom_read(0xf0+uartRxList[1]);
	if(eep==0)//��8Ϊ��λhash
		addr+=8;
	bSameHash_eep(&uartRxList[2],addr);
	if(gFlags.bSame)//�����ͬ,���������ͻ�86 00 00 00 ---
	{
		uartTxList[0]=0x86;
		for(i=0;i<9;i++)
		{
			uartTxList[i]=0x00;
		}
		return;
	}
	//����ͬ��������ټ��
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=uartRxList[i+2];
	}	
	lcyHashOnce();
	bSameHash_eep(lcyHashOut,addr);
	if(gFlags.bSame)//�����ͬ����ɹ�
	{
		UpdateEEPROM();
		return;
	}
	/************************************************************************/
	/* ׷��                                                                 */
	/************************************************************************/
	addr=uartRxList[1]<<4;
	eep=eeprom_read(0xf0+uartRxList[1]);
	if(eep==0)//��8Ϊ��λhash		
	{
		for(i=0;i<8;i++)
		{
			keyH[i]=eeprom_read(addr+8+i);
			keyL[i]=eeprom_read(addr+i);
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			keyH[i]=eeprom_read(addr+i);
			keyL[i]=eeprom_read(addr+8+i);
		}
	}
	bSameHash(keyL,keyH);
	if(gFlags.bSame)//��ֻͬ׷��λ
		gFlags.bHL=0;
	else
	{
		for(i=0;i<8;i++)
		{
			lcyHashIn[i]=keyH[i];
		}
		lcyHashOnce();
		bSameHash(lcyHashOut,keyL);
		if(gFlags.bSame)//H+1=L,Ҳֻ׷��λ
			gFlags.bHL=0;
		else
			gFlags.bHL=1;//����ߵ�λͬʱ׷��
	}
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=uartRxList[i+2];
	}	
	lcyHashOnce();
	for(step=0;step<10000;step++)
	{
		CLRWDT();
		for(i=0;i<8;i++)
		{
			lcyHashIn[i]=lcyHashOut[i];
		}	
		lcyHashOnce();
		bSameHash(lcyHashOut,keyH);//��׷��
		if(gFlags.bSame)//�����ͬ����ɹ�
		{
			addr=uartRxList[1]<<4;
			eep=eeprom_read(0xf0+uartRxList[1]);
			if(eep!=0xff)//׷����ˢ�¸ߵ�λ
			{
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=uartRxList[i+2];
				}	
				lcyHashOnce();
				if(eep==0)//��8Ϊ��λhash
				{				
					eeprom_write(addr+i+8,lcyHashOut[i]);				
				}
				else
				{
					eeprom_write(addr+i,lcyHashOut[i]);
				}
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=lcyHashOut[i];
				}
				lcyHashOnce();
				if(eep==0)//��8Ϊ��λhash
				{				
					eeprom_write(addr+i,lcyHashOut[i]);				
				}
				else
				{
					eeprom_write(addr+i+8,lcyHashOut[i]);
				}
			}
			UpdateEEPROM();
			return;
		}
		if(gFlags.bHL)//ͬʱ׷��,����׷��λ
		{
			bSameHash(lcyHashOut,keyL);
			if(gFlags.bSame)//�����ͬ����ɹ�
			{
				addr=uartRxList[1]<<4;
				eep=eeprom_read(0xf0+uartRxList[1]);
				if(eep!=0xff)//׷����ˢ�¸ߵ�λ
				{
					for(i=0;i<8;i++)
					{
						lcyHashIn[i]=uartRxList[i+2];
					}	
					lcyHashOnce();
					if(eep==0)//��8Ϊ��λhash
					{				
						eeprom_write(addr+i+8,lcyHashOut[i]);				
					}
					else
					{
						eeprom_write(addr+i,lcyHashOut[i]);
					}
					for(i=0;i<8;i++)
					{
						lcyHashIn[i]=lcyHashOut[i];
					}
					lcyHashOnce();
					if(eep==0)//��8Ϊ��λhash
					{				
						eeprom_write(addr+i,lcyHashOut[i]);				
					}
					else
					{
						eeprom_write(addr+i+8,lcyHashOut[i]);
					}
				}
				UpdateEEPROM();
				return;
			}
		}
	}
	uartTxList[0]=0x93;
	for(i=0;i<9;i++)
	{
		uartTxList[i]=0x00;
	}
}
//read
void ProcCommand_read_0x76(void)
{
	unsigned char i,addr,h,l,eep;
	uartTxList[0]=0x78;
	uartTxList[1]=uartRxList[2];	
	addr=0xff;
	for(i=0;i<10;i++)
	{
		if(rxHeader[0][i]==uartRxList[2])
		{
			addr=rxHeader[1][i];
			break;
		}
	}
	if(addr!=0xff)
	{
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=eeprom_read(addr+i);
		}
	}
	else if(uartRxList[2]<0x90)//Կ����Ҫ���⴦��
	{
		l=LOW_NIBBLE(uartRxList[2]);
		h=HIGH_NIBBLE(uartRxList[2]);
		if(h==0x08)//dealer
		{
			addr=uartRxList[2]&0xf0;
			for(i=0;i<8;i++)
			{
				uartTxList[i+2]=eeprom_read(addr+i);
			}
		}
		else//key
		{
			eep=eeprom_read(0xf0+h);
			if(eep==0xff)//δʹ�ù�
			{
				addr=uartRxList[2]&0xf0;
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=eeprom_read(addr+i);
				}
			}
			else if(l==0)
			{
				addr=uartRxList[2];
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=eeprom_read(addr+i);
				}
			}
			else if(l==1)
			{
				addr=h<<4;
				addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=eeprom_read(addr+i);
				}
			}
			else if(l==2)
			{			
				addr=h<<4;
				if(eep==0x01)//��8λΪ��λ
					addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=eeprom_read(addr+i);
				}	
				lcyHashOnce();
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=lcyHashOut[i];
				}
			}
			else if(l==3)
			{
				addr=h<<4;
				if(eep==0x01)//��8λΪ��λ
					addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=eeprom_read(addr+i);
				}	
				lcyHashCalc(2);
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=lcyHashOut[i];
				}
			}
			else
			{
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=0;
				}
			}
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=0;
		}
	}	
}
//renew
void ProcCommand_renew_0x52(void)
{
	unsigned char i,eep;
	uartTxList[0]=0x77;
	uartTxList[1]=0x03;
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=uartRxList[i+2];
	}	
	lcyHashCalc(101);
	bSameHash_eep(lcyHashOut,0x80);	
	if(gFlags.bSame)//�ɹ�
	{
		uartTxList[2]=0x79;
		uartTxList[3]=0x52;
		eep=eeprom_read(0x90);
		if(LOW_NIBBLE(eep)==0x02)//����Ϊ��
			uartTxList[4]=0xff;
		else
			uartTxList[4]=0x00;
		uartTxList[5]=0x00;
		uartTxList[6]=0x00;
		uartTxList[7]=0x00;
		uartTxList[8]=0x00;
		uartTxList[9]=0x00;
		eeprom_write(0x90,0x22);
		eeprom_write(0x91,0xff);	
		for(i=0;i<8;i++)//ǰ8Ϊ��λ
		{
			eeprom_write(0xf0+i,0xff);	
		}
		eeprom_write(0xff,0xff);
	}
	else//ʧ��
	{

	}
}
void ProcCommand_lock_0x42(void)
{
	unsigned char i,eep,addr;
	eep=eeprom_read(0x90);
	if(eep==0x22)//�ո�renew 
	{
		uartTxList[0]=0x88;
		for(i=0;i<9;i++)
		{
			uartTxList[i]=0x00;
		}
	}
	// 	eep=eeprom_read(0xff);
	// 	if(eep==0xff)
	// 		return;
	addr=uartRxList[1]<<4;
	eep=eeprom_read(0xf0+uartRxList[1]);
	if(eep==0)//��8Ϊ��λhash
		addr+=8;
	bSameHash_eep(&uartRxList[2],addr);
	if(gFlags.bSame)
	{
		for(i=0;i<8;i++)
		{
			uartTxList[2+i]=0x00;
		}
		uartTxList[0]=0x80;
		uartTxList[1]=uartRxList[1];
	}
	eep=eeprom_read(0x90);
	eep=eep&0x0f;
	eep=eep|0x20;
	eeprom_write(0x90,eep);
}
//�ݲ����ж�
void ProcCommand_write_0x15(void)
{
	unsigned char i;
	uartTxList[0]=0x8a;
	uartTxList[1]=0x00;
	for(i=0;i<8;i++)
	{
		uartTxList[i+2]=0;
	}
}
//дkey & dealer
void ProcCommand_write_0x96(void)
{
	unsigned char i,addr;
	uartTxList[0]=0x8b;
	uartTxList[1]=uartRxList[1];
	if(uartRxList[1]<0x09)
	{
		addr=uartRxList[1]<<4;
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
			eeprom_write(addr+i+8,uartRxList[2+i]);
		}
		
	}
	else if(uartRxList[1]==0x09)//psw
	{
		addr=0xd0;
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
			eeprom_write(addr+i+8,uartRxList[2+i]);
		}
	}
	else if(uartRxList[1]==0x0a)//У���
	{
		eeprom_write(0x90,0x24);//ǿ�и���״̬������a0 01 ָ��
		eeprom_write(0x91,0x00);
	}
// 	else
// 	{
// 
// 	}
	//��������
	for(i=0;i<8;i++)
	{
		lcyHashIn[i]=uartRxList[2+i];
	}
	lcyHashOnce();
	for(i=0;i<8;i++)
	{
		uartTxList[i+2]=lcyHashOut[i];
	}
}
//��ָ�����Һ���
void ProcCommand_write_0xa0(void)
{
	unsigned char i;
	uartTxList[0]=0xa1;
	uartTxList[1]=0xaa;		
	for(i=0;i<8;i++)
	{
		uartTxList[i+2]=uartRxList[i+2];
	}
}
void ProcCommand_write_0x9a(void)
{
	unsigned char i,addr;
	uartTxList[0]=0x98;
	uartTxList[1]=0x00;
	addr=0xa8;
	if(uartRxList[1]!=0)
		addr+=8;
	//ǿ���ַ�����β
	uartRxList[9]=0x00;
	for(i=0;i<8;i++)
	{
		eeprom_write(addr+i,uartRxList[2+i]);
	}
	for(i=0;i<8;i++)
	{
		uartTxList[i+2]=0;
	}
}
// void ProcCommand52(void)
// {
// 
// }
// void ProcCommand52(void)
// {
// 
// }
// void ProcCommand52(void)
// {
// 
// }
/*
void ProcCommand( void )
{
	unsigned char sum,addr,h,l;
	unsigned char i;
	sum=GetVerify_Sum(uartRxList,10);
	if(sum!=uartRxList[10])
	{
		return;
	}
	if(uartRxList[0]==0x76)//������	
	{
		uartTxList[0]=0x78;
		uartTxList[1]=uartRxList[2];	
		addr=0xff;
		for(i=0;i<10;i++)
		{
			if(rxHeader[0][i]==uartRxList[2])
			{
				addr=rxHeader[1][i];
				break;
			}
		}
		if(addr!=0xff)
		{
			for(i=0;i<8;i++)
			{
				uartTxList[i+2]=eeprom_read(addr+i);
			}
		}
		else if(uartRxList[2]<0x90)//Կ����Ҫ���⴦��
		{
			l=LOW_NIBBLE(uartRxList[2]);
			h=HIGH_NIBBLE(uartRxList[2]);
			if(l==0)
			{
				addr=uartRxList[2];
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=eeprom_read(addr+i);
				}
			}
			else if(l==1)
			{
				addr=h<<4;
				addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=eeprom_read(addr+i);
				}
			}
			else if(l==2)
			{
				addr=h<<4;
				addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=eeprom_read(addr+i);
				}	
				lcyHashOnce();
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=lcyHashOut[i];
				}
			}
			else if(l==3)
			{
				addr=h<<4;
				addr=addr+0x08;
				for(i=0;i<8;i++)
				{
					lcyHashIn[i]=eeprom_read(addr+i);
				}	
				lcyHashCalc(2);
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=lcyHashOut[i];
				}
			}
			else
			{
				for(i=0;i<8;i++)
				{
					uartTxList[i+2]=0;
				}
			}
		}
		else
		{
			for(i=0;i<8;i++)
			{
				uartTxList[i+2]=0;
			}
		}		
	}
	else if(uartRxList[0]==0x52)//renew
	{
		uartTxList[0]=0x77;
		uartTxList[1]=0x03;
		for(i=0;i<8;i++)
		{
			lcyHashIn[i]=uartRxList[i+2];
		}	
		lcyHashCalc(101);
		sum=0;
		for(i=0;i<8;i++)
		{
			addr=eeprom_read(0x80+i);
			if(lcyHashOut[i]!=addr)
			{
				sum=1;
				break;
			}
		}
		if(sum==0)//�ɹ�
		{

		}
		else//ʧ��
		{

		}
	}
	else if(uartRxList[0]==0x15)//д����
	{
		uartTxList[0]=0x8a;
		uartTxList[1]=0x00;
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=0;
		}
	}
	else if(uartRxList[0]==0x96)
	{
		uartTxList[0]=0x8b;
		uartTxList[1]=uartRxList[1];
		if(uartRxList[1]<0x09)
		{
			addr=uartRxList[1]<<4;
			for(i=0;i<8;i++)
			{
				eeprom_write(addr+i,uartRxList[2+i]);
			}
		}
		else if(uartRxList[1]==0x09)//psw
		{
			addr=0xd0;
			for(i=0;i<8;i++)
			{
				eeprom_write(addr+i,uartRxList[2+i]);
			}
		}
		else if(uartRxList[1]==0x0a)//У���
		{

		}
		else
		{

		}
		for(i=0;i<8;i++)
		{
			lcyHashIn[i]=uartRxList[2+i];
		}
		lcyHashOnce();
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=lcyHashOut[i];
		}
	}
	else if(uartRxList[0]==0xa0)
	{
		uartTxList[0]=0xa1;
		uartTxList[1]=0xaa;		
// 		for(i=0;i<8;i++)
// 		{
// 			lcyHashIn[i]=uartRxList[2+i];
// 		}
// 		lcyHashOnce();
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=uartRxList[i+2];
		}
	}
	else if(uartRxList[0]==0x9a)
	{
		uartTxList[0]=0x98;
		uartTxList[1]=0x00;
		addr=0xa8;
		if(uartRxList[1]!=0)
			addr+=8;
		for(i=0;i<8;i++)
		{
			eeprom_write(addr+i,uartRxList[2+i]);
		}
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=0;
		}
	}
	else
	{
		for(i=0;i<8;i++)
		{
			uartTxList[i+2]=0;
		}
	}
	uartTxList[10]=GetVerify_Sum(uartTxList,10);
	Uart1_PutChars(uartTxList,11);
}
*/