#ifndef   __FlagDefine_h__
#define   __FlagDefine_h__
/************************************************************************/
/* ��һ��Ϊ���λ                                                       */
/************************************************************************/
typedef union   
{
	struct
	{
		unsigned bRx:1;
		unsigned bSame:1;//�Ƚ��ã��ж��Ƿ���ͬ
		unsigned bSleep:1;
		unsigned bAlarm:1;//�ߵ�λͬʱ׷��
	};
	unsigned char AllFlag;
} GFlags;        // general flags
#endif
