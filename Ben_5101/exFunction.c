//Build F:\work_tfs\MCU\WDJ\PLC_Flow\PLC_Flow for device 16F689
//Using driver d:\Program Files\HI-TECH Software\PICC\9.71a\bin\picc.exe
#include "HardwareProfile.h"

unsigned char sub_1AF5_CheckRomData(void)//�洢���������Ƿ���ȷ
{
	unsigned char x;
	M24XX_Read(wfTempList,2,0x9e);
	if(wfTempList[0]==0xff)
	{
		if(wfTempList[1]==0xff)
		{
			BAT_CL=1;
			wfTempList[0]=eeprom_read(0x9e);
			wfTempList[1]=eeprom_read(0x9f);
			M24XX_Write(wfTempList,2,0x9e);
		}
	}
	M24XX_Read(wfTempList,2,0x9e);
	x=wfTempList[0]+wfTempList[1];
	if(x==0x00)
	{
		RAM.fCA.Byte=wfTempList[0]&0x7f;//0x21
		x=RAM.fCA.Byte&0xee;//0x20
		if(x==0x04)
			return 1;
		//return x^0x04;//0x24
	}
	return 0;
}
void adr_12C8(void)
{

}
void sub_1C8F_wait_f20_rxok(void)//�����������Ӧ��RAM_A4 BANKED��
{
	f20.rxok=0;
	while(f20.rxok==0)
	{
		CLRWDT();
	}
}
void adr_1C73_exitCCP(void)//�˳���׽��
{
	f20.ONbuzh=0;
	f20.ONcal=0;
	f20.rxok=0;
	CCP1IF=0;
	CCP1IE=0;
	PEIE=0;
	GIE=0;
	CCP1CON=0;
}
void sub_1C7A_RxByte(void)//����һ���ֽ�
{
	sub_1C8F_wait_f20_rxok();
	if(f21.over1)
		return;
	RAM.RAM_A3=RAM.RAM_A4;
	sub_1C8F_wait_f20_rxok();
	if(f21.over1)
		return;
	RAM.RAM_A3=MAKE_BYTE(RAM.RAM_A4,RAM.RAM_A3);	
}
void sub_1C9B_CheckCCPRx0_return_CNT0(void)//����ֵCNT0��Ϊ֡���ݳ���
{
	unsigned char x;
	CNT0=0;
	if(RAM.RAM_81==0x08)
		return;
	if(RAM.RAM_81==0x0f)
		return;
	if(RAM.RAM_81==0x6a)
		return;
	if(RAM.RAM_81==0x71)
		return;

	CNT0=2;
	if(RAM.RAM_81==0x73)
		return;
	if(RAM.RAM_81==0x79)
		return;	;

	CNT0=7;
	if(RAM.RAM_81==0x7a)
		return;
	CNT0=8;
	if(RAM.RAM_81==0x26)
		return;
	if(RAM.RAM_81==0x63)
		return;
	if(RAM.RAM_81==0x3e)
		return;
	if(RAM.RAM_81==0x39)
		return;
	CNT0=9;
}
void INI_T1_10mS()
{
	TMR1ON=0;
	TMR1L=0xEF;
	TMR1H=0xD8;
	TMR1ON=1;
	TMR1IE=0;
	TMR1IF=0;
}

void DELAY_N_10MS(unsigned char n)//��ʱn��10ms�ȴ���׽��ɣ���ɺ���������
{
	INI_T1_10mS();
	//��׽��ʱ�Ż��õ�
	while(n--)
	{
		if(f20.rxok)//δ��׽���
		{
			return;
		}
		while(TMR1IF==0);
		INI_T1_10mS();
	}
	
}
void sub_1C30_RxFrame(unsigned char n)//����1֡���ݣ�nΪ֡������ʼ��ʱ
{
	unsigned char i;
	f20.ONbuzh=1;//��׽��ʼ
	f20.ONcal=0;//��������־
	f20.rxok=0;
	
	RAM.RAM_A4=0;

	TMR3IF=0;
	TMR3ON=0;
	TMR3L=0;
	TMR3H=0;
	TMR3ON=1;

	f21.over1=1;

	CCP1CON=0b00000100;
	CCP1IF=0;
	CCP1IE=1;
	PEIE=1;
	GIE=1;
	DELAY_N_10MS(n);
	if(f21.over1)
	{
		adr_1C73_exitCCP();
		return;
	}	
	/************************************************************************/
	/* RX_HEAD                                                              */
	/************************************************************************/
	while(1)
	{
		sub_1C8F_wait_f20_rxok();
		if(f21.over1)
		{
			adr_1C73_exitCCP();
			return;
		}
		if(RAM.RAM_A4!=0)//����յ�����0�������µȴ�
			break;
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	sub_1C7A_RxByte();
	if(f21.over1)
	{
		adr_1C73_exitCCP();
		return;
	}	
	RAM.RAM_81=RAM.RAM_A3;
	sub_1C9B_CheckCCPRx0_return_CNT0();
	if(CNT0==0)
	{
		adr_1C73_exitCCP();
		return;
	}
	CCPRx[0]=RAM.RAM_81;
	for(i=0;i<CNT0;i++)
	{
		sub_1C7A_RxByte();//����1���ֽ�
		if(f21.over1)
		{
			adr_1C73_exitCCP();
			return;
		}
		CCPRx[i+1]=RAM.RAM_A3;
	}
	
}
void LOAD_8EE_F(unsigned char* pBuf,unsigned char addr)//addr��X_P����
{
	unsigned char i;
	if(f20.RDee)//'0'read 24C02,'1'read EEPROM
	{
		for(i=0;i<8;i++)
		{
			*pBuf=eeprom_read(addr+i);
			pBuf--;
		}
	}
	else
	{
		M24XX_Read(pBuf-7,8,addr);
	}
}
void sub_1B48_Load8EEto93to9A_byX_P(unsigned char addr)
{
	LOAD_8EE_F(&RAM.RAM_9A,addr);
//	LFSR	FSR0,0X9A		;RAM_9A
//		CALL	LOAD_8EE_F		;
}
void sub_19D9_LoadData(void)
{
	unsigned char x;
	if(RAM.fCA.b1)
	{
		x=0x4e;
	}
	else
		x=0x05;
	sub_1B48_Load8EEto93to9A_byX_P(x);
	
	if(RAM.fCA.b1)
	{
		x=0x56;
	}
	else
		x=0x9b;
	M24XX_Read(&RAM.RAM_97,1,x);
	RAM.RAM_98=0;
	RAM.RAM_99=0;
	RAM.RAM_9A=RAM.RAM_C2;
}
void sub_19F7_Data83and8Bis93(void)
{
	unsigned char i,x;
	RAM.RAM_81=RAM.RAM_C2;
	RAM.RAM_82=RAM.RAM_C3;
	if(RAM.fCA.b1==1 && RAM.RAM_82==0x0a)
	{
		sub_19D9_LoadData();//���ش洢���е�����
	}
	for(i=0;i<8;i++)
	{
		x=(&RAM.RAM_93)[i];
		(&RAM.RAM_8B)[i]=x;
		(&RAM.RAM_83)[i]=x;
	}
}
void sub_1BE7_93to9A_FanXuPaiLie(void)
{
	unsigned char i,x,w;
	pFSR0=&RAM.RAM_93;
	pFSR1=&RAM.RAM_9A;
	for(i=0;i<4;i++)
	{
		x=*pFSR0;
		*pFSR0=*pFSR1;
		pFSR0++;
		*pFSR1=x;
		pFSR1--;
	}
}

void ON_CCP()
{
	TMR3ON=0;
	TMR3L=3;
	TMR3H=0;
	TMR3ON=1;
	CCP1IF=0;
}
void INI_BJQ()
{
	CCP1CON=0b00001010;//�����Ƚ�ƥ��ʱ��������ж� ��CCPx ���Ų���Ӱ�죬 CCPxIF �� 1��
	CCPR1H=0x01;//500
	CCPR1L=0xf4;
}
void sub_1D4D_RFTXByte(unsigned char D)
{
	unsigned char i,x;
	for(i=0;i<8;i++)
	{
		while(CCP1IF==0);
		x = PORTC & 0b11000111;
		if((D&0x01)==0x01)
		{
			PORTC = x | 0b00101000;
		}
		else
			PORTC = x | 0b00011000;
		D=D>>1;
		ON_CCP();

		while(CCP1IF==0);

		PORTC = PORTC ^ 0b00110000;

		ON_CCP();

	}
}
void ON_CCP_IRB()
{
	TMR3ON=0;
	TMR3L=9;
	TMR3H=0;
	TMR3ON=1;
	CCP1IF=0;
}
void ON_CCP_IRA()
{
	TMR3ON=0;
	if(RAM.fCB.b6)
		TMR3L=32;
	else
		TMR3L=12;	
	TMR3H=0;
	TMR3ON=1;
	CCP1IF=0;
}
void sub_1D91_TxNibbleByte(unsigned char halfD)
{
	unsigned int x;
	unsigned char i;
	while(CCP1IF);
	if(f20.power)
	{
		//���ع���ʱ
		IR_B=0;
		__delay_us(30);
		IR_B=1;
		x=halfD;
		x=x*0x40;
		x=x+0x03e2;
		CCPR1H=HIGH_BYTE(x);
		CCPR1L=LOW_BYTE(x);
		CCP1CON=0b00001010;//�����Ƚ�ƥ��ʱ��������ж� ��CCPx ���Ų���Ӱ�죬 CCPxIF �� 1��
		ON_CCP_IRB();
	}
	else
	{
		//��ع���ʱ
		if(RAM.fCB.b6)
		{
			for(i=0;i<6;i++)
			{
				IR_A=!IR_A;
				__delay_us(3);
				IR_A=!IR_A;
				__delay_us(18);
			}
		}
		else
		{
			for(i=0;i<7;i++)
			{
				IR_A=!IR_A;
				__delay_us(5);
				IR_A=!IR_A;
				__delay_us(10);
			}			
		}
		x=halfD;
		x=x*0x80;
		x=x<<1;
		x=x+0x0488;
		CCPR1H=HIGH_BYTE(x);
		CCPR1L=LOW_BYTE(x);
		CCP1CON=0b00001010;
		ON_CCP_IRA();
	}
}
void sub_1D80_IRTxByte(unsigned char D)
{
	sub_1D91_TxNibbleByte(LOW_NIBBLE(D));
	sub_1D91_TxNibbleByte(HIGH_NIBBLE(D));
}
void sub_1D43_TxByte(unsigned char D)
{
	if(RF_EN)
	{
		sub_1D4D_RFTXByte(D);
	}
	else
		sub_1D80_IRTxByte(D);
}
void sub_1D42_TxByte0()
{
	sub_1D43_TxByte(0);
}

void sub_1D11_TxDataBuf(unsigned char* pBuf,unsigned char N)
{
	if(RF_EN)
	{
		INI_BJQ();
		ON_CCP();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
		sub_1D42_TxByte0();
	}
	while(N--)
	{
		sub_1D43_TxByte(*pBuf);
		pBuf++;
	}
}
void adr_1D04_TxFrame(unsigned char N)//N��X_P����
{
	//CNT0=N+1;
	//pFSR0=&RAM.RAM_80;

	CCP1CON=0b00001010;//�����Ƚ�ƥ��ʱ��������ж� ��CCPx ���Ų���Ӱ�죬 CCPxIF �� 1��
	CCPR1H=0;
	CCPR1L=100;
	ON_CCP();
	sub_1D11_TxDataBuf(&RAM.RAM_80,N+1);
	sub_1D91_TxNibbleByte(0);
	CCP1CON=0;
}
void TX_IR_B(unsigned char N)//N��X_P����:10 ram81 ram82 ��������ram83~ram8a
{
	RAM.RAM_80=0x10;	
	adr_1D04_TxFrame(N);
}
void sub_17F2_Tx88Data(void)
{
	RAM.RAM_D1=0x88;
	RAM.RAM_C2=0x0d;
	RAM.RAM_C3=0;

	f20.RDee=1;//��eeprom
	sub_1B48_Load8EEto93to9A_byX_P(0x88);
	if(RAM.fCB.b6)
	{
		sub_1BE7_93to9A_FanXuPaiLie(); //loaded data ����
		//CALL	sub_1BE7		;loaded data ����
	}
	sub_19F7_Data83and8Bis93();
	DELAY_N_10MS(10);
	BAT_CL=1;
	TX_IR_B(10);
	//else
	//{
	//	// 	BRA	adr_17F3		;
	//}
// 	BTFSS	RAM_CB,6,BANKED		;
// 	BRA	adr_17F3		;
// 	CALL	sub_1BE7		;loaded data ����
}
unsigned char sub_17DC_waitRxFrame(void)//��adr_17DC,A_P��Ϊ���յ�����
{
	unsigned char x=0,rx;
	while (1)
	{
		sub_1C30_RxFrame(x);//֡����ͷ��ʱ256��10ms������һ֡����
		if(f21.over1)
		{
			sub_1C30_RxFrame(100);//��ʱ������ʱ100��10ms����������
			if(f21.over1)//�ٳ�ʱ���˳�
			{
				BAT_CL=0;
				return 0;
			}
		}
		RAM.RAM_C2=RAM.RAM_81;
		rx=RAM.RAM_81;
		if(rx==0x0f)//�յ�����0x0f
		{
			sub_17F2_Tx88Data();
			x=50;
		}
		else 
		{
			BAT_CL=0;
			return rx;
		}
	}
	return 0;
	
// 	if(A_P!=0x0f)
// 	{
// 		BAT_CL=0;
// 		return;
// 	}
	
// 	MOVLW	.50			;
// 	BRA	adr_17DD		;adr_17DC

}
void LED_FLASH(void)
{
	LED=1;
	DELAY_N_10MS(30);
	LED=0;
	DELAY_N_10MS(30);
	LED=1;
	DELAY_N_10MS(30);
	LED=0;
	DELAY_N_10MS(30);
}
void sub_0C11(unsigned char N)
{

}
void sub_184A_TX_IR_B_10_05_00_data83()
{
	RAM.RAM_81=0x05;
	TX_IR_B(0x0a);
}
void adr_183c_wirteI2C_apisAddr_data8B(unsigned char ap)
{
	BAT_CL=1;
	M24XX_Write(&RAM.RAM_8B,8,ap);
}
void sub_1804_WriteDecode_hashcal_TxAnswer(void)
{
	unsigned char i,x,index,addr;
	for(i=0;i<8;i++)
	{
		(&RAM.RAM_8B)[i]=(&RAM.RAM_83)[i];
	}
	if(RAM.fCB.b5)//����д��������
	{
		//����д��Ľ������,��������RAM_8B
	}
	if(f_cmd35)
	{
		f_cmd35=0;
		if(f_zero)
		{
			//��83��ַ�����ݽ���hashcal
			sub_184A_TX_IR_B_10_05_00_data83();
			return;
		}
	}
	if(RAM.RAM_82<RAM.RAM_C4)//RAM.RAM_C4=0x10?????
	{
		index=RAM.RAM_82;
		addr=index*0x08;
		addr=addr+0x88;
		switch(RAM.RAM_81)
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
		adr_183c_wirteI2C_apisAddr_data8B(addr);
	}
	//��83��ַ�����ݽ���hashcal
	sub_184A_TX_IR_B_10_05_00_data83();
}
void sub_18C4_CheckSameFrame_82isFF()
{
	sub_1C30_RxFrame(100);
	if(f21.over1)
		return;
	f21.over1=1;
	if(RAM.RAM_81==RAM.RAM_C2)//����֡��������ͬ
	{
		f21.over1=0;
		if(RAM.RAM_82!=0xff)
			sub_1804_WriteDecode_hashcal_TxAnswer();
	}
}
void whileLEDFlash()
{
	while(1)
	{
		LED_FLASH();
	}
}
void sub_1B07_I2C_addr9e_dataWandnotW(unsigned char w)//д��ң��״̬
{
	RAM.RAM_8B=w;
	RAM.RAM_8C=0-RAM.RAM_8B;
	M24XX_Write(&RAM.RAM_8B,2,0x9e);
}
unsigned char sub_1B9D_XOR_93to9A_reA_P()
{
	unsigned char i,ret;
	ret=RAM.RAM_93;
	for(i=1;i<8;i++)
	{
		ret =ret ^ (&RAM.RAM_93)[i];
	}
	return ret;
}
unsigned char sub_1BB6_XOR_xptoxp8_XOR_I2C0809_reW(unsigned char xp)
{
	unsigned ap,ret;
	sub_1B48_Load8EEto93to9A_byX_P(xp);
	ap=sub_1B9D_XOR_93to9A_reA_P();
	M24XX_Read(wfTempList,2,xp+8);
	ret=ap;
	ret=ret^wfTempList[0];
	ret=ret^wfTempList[1];
	return ret;
}
unsigned char sub_1BAF_CheckEEdataXORis0(unsigned char w)
{
	unsigned char xp;
	xp=w;
	M24XX_Read(wfTempList,1,xp+8);
	if(wfTempList[0]>0x7f)
		return 0;//+8��EE���ܴ���7F.
	if(sub_1BB6_XOR_xptoxp8_XOR_I2C0809_reW(xp)==0)//��鵱ǰEE��ַ֮���10���Ƿ������Ϊ0��
		return 1;
	else 
		return 0;
}
unsigned char sub_16B8_CheckEE41andEE49_reC3()
{
	unsigned char xp;
	unsigned char EE_49,EE_53;
	RAM.RAM_C3=0;
	RAM.fD3.b1=1;
	RAM.fC9.b4=1;
	RAM.fC9.b5=1;
	M24XX_Read(&EE_53,1,0x53);
	M24XX_Read(&EE_49,1,0x49);
	if(sub_1BAF_CheckEEdataXORis0(0x41)==1)//��鵱ǰEE��ַ֮���10���Ƿ������Ϊ0
	{
		RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
		RAM.fC9.b4=0;
		if(EE_49==0)//EE_49
		{
			if(sub_1BAF_CheckEEdataXORis0(0x4B)==1)//��鵱ǰEE��ַ֮���10���Ƿ������Ϊ0
			{
				RAM.fC9.b5=0;
				if(EE_53==0x1f)
				{
					//adr_1704
					RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
					xp=EE_49; //EE_49
				}
				else
				{
					//adr_170B
					RAM.fD3.b1=1;//0x4B��ʼ�����ݶ�ƥ��
					xp=EE_53;//EE_53
				}
			}
			else
			{
				//adr_1704
				RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
				xp=EE_49; //EE_49
			}
		}
	}
	if(sub_1BAF_CheckEEdataXORis0(0x4B)==1)//��鵱ǰEE��ַ֮���10���Ƿ������Ϊ0
	{
		RAM.fC9.b5=0;
		if(RAM.fD3.b1)//0x41��ʼ�����ݶ�δƥ��
		{
			//adr_170B
			RAM.fD3.b1=1;//0x4B��ʼ�����ݶ�ƥ��
			xp=EE_53;//EE_53
		}
		else
		{
			if(EE_53==0)
			{
				if(EE_49==0x1f)
				{
					//adr_170B
					RAM.fD3.b1=1;//0x4B��ʼ�����ݶ�ƥ��
					xp=EE_53;//EE_53
				}
				else
				{
					//adr_1704
					RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
					xp=EE_49; //EE_49
				}
			}
			else
			{
				if(EE_53<EE_49)
				{
					//adr_1704
					RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
					xp=EE_49; //EE_49
				}
				else
				{
					//adr_170B
					RAM.fD3.b1=1;//0x4B��ʼ�����ݶ�ƥ��
					xp=EE_53;//EE_53
				}
			}
		}
	}
	else
	{
		//adr_1704
		RAM.fD3.b1=0;//0x41��ʼ�����ݶ�ƥ��
		xp=EE_49; //EE_49
	}
	//adr_1710
	if(RAM.fC9.b5==1 && RAM.fC9.b4==1)//��δƥ��
	{
		RAM.RAM_C3=xp;
		return RAM.RAM_C3;
	}
	//RAM.RAM_D4=EE_49;
	if(EE_49==0)
	{
		xp=EE_53;
		if(EE_53==0x1f)
		{
			RAM.RAM_C3=xp;
			return RAM.RAM_C3;
		}
	}
	xp=EE_53;
	if(EE_53==0)
	{
		xp=EE_49;
		if(EE_49==0x1f)
		{
			RAM.RAM_C3=xp;
			return RAM.RAM_C3;
		}
		xp=EE_53;
	}
	if(xp==EE_49)
	{
		xp=EE_53;
		RAM.RAM_C3=xp;
		return RAM.RAM_C3;
	}
	else if(xp>EE_49)
	{
		xp=EE_49;
		RAM.RAM_C3=xp;
		return RAM.RAM_C3;
	}
}
void sub_1468()
{
	sub_16B8_CheckEE41andEE49_reC3();
	if(RAM.fD3.b1==0)//0:0x41��ʼ�����ݶ�ƥ��
	{
		sub_1B48_Load8EEto93to9A_byX_P(0x41);
	}
	else
		sub_1B48_Load8EEto93to9A_byX_P(0x4b);
}
void sub_1854()
{
	unsigned char i;
	while(1)
	{
		sub_18C4_CheckSameFrame_82isFF();
		if(f21.over1)
			whileLEDFlash();
		if(RAM.RAM_82==0xff)
			break;
	}
	if(RAM.RAM_81==0x06 || RAM.RAM_81==0x68 || RAM.RAM_81==0x35 || RAM.RAM_81==0x37)
	{
		
	}
	else
	{
		//	CALL	sub_18E0		;����յ����ݵ�������
		//BNZ	adr_18BA		;
	}
	for(i=0;i<8;i++)
		(&RAM.RAM_83)[i]=0x00;
	/************************************************************************/
	/* ?                                                                     */
	/************************************************************************/
	if(RAM.RAM_81==0x00 || RAM.RAM_81==0x06||RAM.RAM_81==0x07 || RAM.RAM_81==0x03)
		sub_1B07_I2C_addr9e_dataWandnotW(0);
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	switch(RAM.RAM_81)
	{
	case 0x00:
		sub_1B07_I2C_addr9e_dataWandnotW(0x21);
		break;
	case 0x06:
		sub_1B07_I2C_addr9e_dataWandnotW(0x27);
		break;
	case 0x07:
		sub_1B07_I2C_addr9e_dataWandnotW(0x27);
		break;
	case 0x03:
		sub_1B07_I2C_addr9e_dataWandnotW(0x0c);
		break;
	}
	if(RAM.RAM_81==0x0e || RAM.RAM_81==0x68)
	{
		sub_184A_TX_IR_B_10_05_00_data83();
		LED=1;
	}
	if(RAM.RAM_81==0x00)
	{
		//xp=0x21
		//sub_1B07		;д��ң��״̬
		sub_184A_TX_IR_B_10_05_00_data83();
		LED=1;
	}
}
void sub_17B4(void)
{
	unsigned char rx;
	rx=sub_17DC_waitRxFrame();
	if(f21.over1)
	{
		whileLEDFlash();
	}
	RAM.fCB.b5=1;//������37ʱ��־
	if(rx==0x04 || rx==0x37)
	{
		RAM.RAM_C4=0x10;//�����ж��Ƿ���Ҫ�洢����������
		f20.RDee=1;
		sub_1B48_Load8EEto93to9A_byX_P(0x80);
		sub_1BE7_93to9A_FanXuPaiLie();
		sub_1804_WriteDecode_hashcal_TxAnswer();
		sub_1854();//no return -call
	}
}
void POW_CAR(void)
{
	//sub_1D11_TxDataBuf(wfTempList,8);
	if(sub_1AF5_CheckRomData()==0)//�洢���������Ƿ���ȷ
	{
		adr_12C8();
	}
	else if(RAM.fCA.Byte==0x4c)//�ж�9E��ַ�Ƿ�Ϊ4c
	{

	}
	else if(RAM.fCA.Byte==0x0c)//�ж�9E��ַ�Ƿ�Ϊ0c
	{

	}
	else if(RAM.fCA.Byte==0x21)//�ж�9E��ַ�Ƿ�Ϊ21
	{
		sub_17B4();
	}
	//	GOTO	sub_1784		;no return
}
void POW_BAT(void)
{

}

