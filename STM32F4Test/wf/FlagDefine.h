#ifndef   __FlagDefine_h__
#define   __FlagDefine_h__
/************************************************************************/
/* ��һ��Ϊ���λ                                                       */
/************************************************************************/
typedef union   
{
	struct
	{
		unsigned bWithPC_Rx:1;//�Ƿ񴮿��յ�����֡
		unsigned bError:1;//��ǰ�Ƿ��д�����
		/************************************************************************/
		/* ������                                                               */
		/************************************************************************/
		unsigned bIRTxPulse:1;//IR��ǰ�Ƿ�Ϊ��������
		unsigned bIRTxFinish:1;
		unsigned bFirstIC:1;//�Ƿ�Ϊ��һ�β�׽
		unsigned bIRRxFinish:1;
	}Bits;
	unsigned int AllFlag;
} _GFlags;        // general flags


#endif
