// HIDDevice.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarKey2.h"
#include "HIDDevice.h"
#include "InteractionData.h"

extern "C" {
	// This file is in the Windows DDK available from Microsoft.
#include "hidsdi.h"
#include "dbt.h"
#include <setupapi.h>
}
// CHIDDevice

IMPLEMENT_DYNAMIC(CHIDDevice, CWnd)

CHIDDevice::CHIDDevice()
{
	MyDeviceDetected=FALSE;
	VendorID=0x03d8;
	ProductID=0x000d;
	m_pProgress=NULL;
}

CHIDDevice::~CHIDDevice()
{
}


BEGIN_MESSAGE_MAP(CHIDDevice, CWnd)
END_MESSAGE_MAP()


bool CHIDDevice::FindDevice()
{
	GUID HidGuid;
	//ȡ�����
	HidD_GetHidGuid(&HidGuid);	
	//��ȡһ���豸��ϢȺ������ָ����������豸
	HANDLE hDevInfo;
	hDevInfo = SetupDiGetClassDevs(
		&HidGuid,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_INTERFACEDEVICE
		);
	SP_DEVICE_INTERFACE_DATA devInfoData;
	devInfoData.cbSize = sizeof(devInfoData);
	int MemberIndex = 0;
	bool LastDevice = FALSE;
	LONG Result;
	ULONG Length;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	ULONG Required;
	HIDD_ATTRIBUTES Attributes;
	do
	{
		Result=SetupDiEnumDeviceInterfaces(
			hDevInfo,
			0,
			&HidGuid,
			MemberIndex,
			&devInfoData
			);

		if (Result != 0)
		{			
			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				NULL, 
				0, 
				&Length, 
				NULL);

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData -> cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			//��ȡ��ϢȺ��һ���豸����Ϣ
			Result = SetupDiGetDeviceInterfaceDetail 
				(hDevInfo, 
				&devInfoData, 
				detailData, 
				Length, 
				&Required, 
				NULL);
			DeviceHandle=CreateFile 
				(detailData->DevicePath, 
				0, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, 
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING, 
				0, 
				NULL);
			Attributes.Size = sizeof(Attributes);

			//��ȡ�������Ʒ���汾��
			Result = HidD_GetAttributes 
				(DeviceHandle, 
				&Attributes
				);
			MyDeviceDetected = FALSE;

			if( (Attributes.VendorID == VendorID) && (Attributes.ProductID == ProductID))
			{
				MyDeviceDetected = TRUE;
// 				USB·�������������֣����ڰβ��豸ʱ��ʾ
// 								MyDevicePathName = detailData->DevicePath;	
// 				
// 								//Register to receive device notifications.
// 				
// 								RegisterForDeviceNotifications();
				PHIDP_PREPARSED_DATA	PreparsedData;	//struct
				//��ȡ�����豸������Ϣ�Ļ������ľ��
				HidD_GetPreparsedData(DeviceHandle, &PreparsedData);
				HIDP_CAPS Capabilities;
				HidP_GetCaps(PreparsedData, &Capabilities);
				OutputReportByteLength=Capabilities.OutputReportByteLength;
				InputReportByteLength=Capabilities.InputReportByteLength;
				HidD_FreePreparsedData(PreparsedData);
				//DeviceUsage = (Capabilities.UsagePage * 0x100) + Capabilities.Usage;

				// Get a handle for writing Output reports.

				WriteHandle=CreateFile 
					(detailData->DevicePath, 
					GENERIC_WRITE, 
					FILE_SHARE_READ|FILE_SHARE_WRITE, 
					(LPSECURITY_ATTRIBUTES)NULL,
					OPEN_EXISTING, 
					0, 
					NULL);
				ReadHandle=CreateFile 
					(detailData->DevicePath, 
					GENERIC_READ, 
					FILE_SHARE_READ|FILE_SHARE_WRITE,
					(LPSECURITY_ATTRIBUTES)NULL, 
					OPEN_EXISTING, 
					FILE_FLAG_OVERLAPPED, 
					NULL);
				if (hEventObject == NULL)	//0
				{
					hEventObject = CreateEvent 
						(NULL, 
						TRUE, 
						TRUE, 
						_T(""));
					HIDOverlapped.hEvent = hEventObject;
					HIDOverlapped.Offset = 0;
					HIDOverlapped.OffsetHigh = 0;
				}
			}
			else
			{
				CloseHandle(DeviceHandle);
			}
			free(detailData);
		} //if (Result != 0)
		else
		{			
			LastDevice=TRUE;
		}
		MemberIndex = MemberIndex + 1;
	}
	while ((LastDevice == FALSE) && (MyDeviceDetected == FALSE));
	if(MyDeviceDetected==true)
	{
		if(CheckDevice())
			return true;
	}
	return false;
}

CInteractionData CHIDDevice::DeviceInteraction(CInteractionData tx)
{
	return DeviceInteraction(tx,40);
}

CInteractionData CHIDDevice::DeviceInteraction(CInteractionData tx,int n)
{
	DWORD	BytesWritten = 0;
	INT		Index =0;
	BOOL	Result=FALSE;
	DWORD NumberOfBytesRead;
	CInteractionData rx;
// 	try
// 	{
		if (WriteHandle != INVALID_HANDLE_VALUE)
		{
			byte* out=tx.GetDataList();
			Result = WriteFile(
				WriteHandle,
				out,
				OutputReportByteLength,
				&BytesWritten,
				NULL);
			delete[] out;
		}

		if (Result==FALSE)
		{
			CloseHandles();
			MyDeviceDetected = FALSE;
			return rx;
			//MessageBox("WriteFile Error","WriteFile",MB_OK);
		}
// 	}
// 	catch (CException* e)
// 	{
// 		throw _T("USBWirteError!!"+e->GetErrorMessage());
// 	}
	byte InputReport[256];
	if (ReadHandle != INVALID_HANDLE_VALUE)
	{
		Result = ReadFile 
		(ReadHandle, 
		InputReport, 
		InputReportByteLength, 
		&NumberOfBytesRead,
		(LPOVERLAPPED) &HIDOverlapped); 
	}
	if(m_pProgress==NULL)
	{
		Result = WaitForSingleObject(hEventObject,30000);
		switch (Result)
		{
		case WAIT_OBJECT_0:
			{
				rx.SetData(InputReport);
			}
			break;
		case WAIT_TIMEOUT:
			{			
				Result = CancelIo(ReadHandle);
				CloseHandles();
				MyDeviceDetected = FALSE;
				break;
			}
		default:
			{			
				CloseHandles();
				MyDeviceDetected = FALSE;
				break;
			}
		}
	}
	else
	{
		bool bOK=false;
		while(true)
		{
			Result = WaitForSingleObject(hEventObject,500);
			switch (Result)
			{
			case WAIT_OBJECT_0:
				{
					rx.SetData(InputReport);
					bOK=true;
				}
				break;
			case WAIT_TIMEOUT:
				{			
					
				}
				break;
			default:
				{			
					CloseHandles();
					MyDeviceDetected = FALSE;
					break;
				}
			}
			if(bOK)
			{
				break;
			}				
			n--;
			m_pProgress->SetPos(n);
			m_pProgress->UpdateWindow();
			if(n==0)
			{				
				break;
			}
		}
		m_pProgress->SetPos(0);
		if(!bOK)
		{
			Result = CancelIo(ReadHandle);
			CloseHandles();
			MyDeviceDetected = FALSE;
		}
	}
	ResetEvent(hEventObject);
	return rx;
}

void CHIDDevice::CloseHandles()
{
	//Close open handles.
	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle=INVALID_HANDLE_VALUE;
	}

	if (ReadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(ReadHandle);
		ReadHandle=INVALID_HANDLE_VALUE;
	}

	if (WriteHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(WriteHandle);
		WriteHandle=INVALID_HANDLE_VALUE;
	}
}

HANDLE CHIDDevice::RegisterForUsbEvents(HANDLE hWnd)
{
	DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;  

	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);  
	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE; 
	GUID gHid;
	HidD_GetHidGuid(&gHid);
	DevBroadcastDeviceInterface.dbcc_classguid = gHid;  
	return RegisterDeviceNotification(hWnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
}

bool CHIDDevice::UnregisterForUsbEvents(HANDLE hHandle)
{
	return (bool)UnregisterDeviceNotification(hHandle);
}

void CHIDDevice::Close()
{
	MyDeviceDetected=false;
	CloseHandles();
}
const byte B[4]={0x2E,0x74,0x5C,0x3A};
void CHIDDevice::DECRYPT()
{
	int CNT0 = 0;
	byte j = 5;
	byte i;
	byte MASK;
	byte tmp_K;
	tmp_K = 0;
	while (CNT0 < 528) //0x210
	{
		if ((CNT0 & 0x0007) == 0)
		{
			if (j == 7)
			{
				j = 0;
			}
			else
			{
				j++;
			}
			tmp_K = KEY[j];
		}

		if ((HOP[1] & 0x08) == 0x08)
		{
			MASK = 0x10;
		}
		else
		{
			MASK = 0x01;
		}
		if ((HOP[2] & 0x01) == 0x01)
		{
			MASK = (byte)(MASK << 2);
		}
		if ((HOP[3] & 0x01) == 0x01)
		{
			MASK = (byte)(MASK << 1);
		}

		if ((HOP[0] & 0x02) == 0x02)
		{
			i = 0x01;
		}
		else
		{
			i = 0x00;
		}

		if ((HOP[0] & 0x40) == 0x40)
		{
			i = (byte)(i + 0x02);
		}

		if ((MASK & B[i]) == 0)
		{
			MASK = 0x00;
		}
		else
		{
			MASK = 0x80;
		}
		MASK = (byte)(MASK ^ HOP[2] ^ HOP[0] ^ tmp_K);

		HOP[0] = (byte)(HOP[0] << 1);
		if ((HOP[1] & 0x80) == 0x80)
		{
			HOP[0] = (byte)(HOP[0] | 0x01);
		}
		else
		{
			HOP[0] = (byte)(HOP[0] & 0xFE);
		}

		HOP[1] = (byte)(HOP[1] << 1);
		if ((HOP[2] & 0x80) == 0x80)
		{
			HOP[1] = (byte)(HOP[1] | 0x01);
		}
		else
		{
			HOP[1] = (byte)(HOP[1] & 0xFE);
		}

		HOP[2] = (byte)(HOP[2] << 1);
		if ((HOP[3] & 0x80) == 0x80)
		{
			HOP[2] = (byte)(HOP[2] | 0x01);
		}
		else
		{
			HOP[2] = (byte)(HOP[2] & 0xFE);
		}

		HOP[3] = (byte)(HOP[3] << 1);
		if ((MASK & 0x80) == 0x80)
		{
			HOP[3] = (byte)(HOP[3] | 0x01);
		}
		else
		{
			HOP[3] = (byte)(HOP[3] & 0xFE);
		}

		tmp_K = (byte)(tmp_K << 1);
		CNT0++;
	}
}

void CHIDDevice::ENCODE()
{
	int CNT0 = 0;
	byte j = 0;
	byte i;
	byte MASK;
	byte tmp_K;
	tmp_K = 0;
	while (CNT0 < 528) //0x210
	{
		if ((CNT0 & 0x0007) == 0)
		{
			if (j == 0)
			{
				j = 7;
			}
			else
			{
				j--;
			}
			tmp_K = KEY[j];
		}

		if ((HOP[1] & 0x10) == 0x10)
		{
			MASK = 0x10;
		}
		else
		{
			MASK = 0x01;
		}
		if ((HOP[2] & 0x02) == 0x02)
		{
			MASK = (byte)(MASK << 2);
		}
		if ((HOP[3] & 0x02) == 0x02)
		{
			MASK = (byte)(MASK << 1);
		}

		if ((HOP[0] & 0x04) == 0x04)
		{
			i = 0x01;
		}
		else
		{
			i = 0x00;
		}

		if ((HOP[0] & 0x80) == 0x80)
		{
			i = (byte)(i + 0x02);
		}

		if ((MASK & B[i]) == 0)
		{
			MASK = 0x00;
		}
		else
		{
			MASK = 0x01;
		}

		MASK = (byte)(MASK ^ HOP[1] ^ HOP[3] ^ tmp_K);

		HOP[3] = (byte)(HOP[3] >> 1);
		if ((HOP[2] & 0x01) == 0x01)
		{
			HOP[3] = (byte)(HOP[3] | 0x80);
		}
		else
		{
			HOP[3] = (byte)(HOP[3] & 0x7F);
		}

		HOP[2] = (byte)(HOP[2] >> 1);
		if ((HOP[1] & 0x01) == 0x01)
		{
			HOP[2] = (byte)(HOP[2] | 0x80);
		}
		else
		{
			HOP[2] = (byte)(HOP[2] & 0x7F);
		}

		HOP[1] = (byte)(HOP[1] >> 1);
		if ((HOP[0] & 0x01) == 0x01)
		{
			HOP[1] = (byte)(HOP[1] | 0x80);
		}
		else
		{
			HOP[1] = (byte)(HOP[1] & 0x7F);
		}

		HOP[0] = (byte)(HOP[0] >> 1);
		if ((MASK & 0x01) == 0x01)
		{
			HOP[0] = (byte)(HOP[0] | 0x80);
		}
		else
		{
			HOP[0] = (byte)(HOP[0] & 0x7F);
		}

		tmp_K = (byte)(tmp_K >> 1);
		CNT0++;
	}
}

bool CHIDDevice::CheckDevice()
{
	CInteractionData tx;

	tx.buff[0] = 0x03;
	tx.buff[1] = 0x55;	//����-->�豸
	CInteractionData rx = DeviceInteraction(tx);
	if (!rx.bOK || rx.CheckDataList() == false)
		return false;
	HexID[0] = rx.buff[0x10];
	HexID[1] = rx.buff[0x11];
	HexID[2] = rx.buff[0x12];
	HexID[3] = rx.buff[0x13];
	//             if (HexID[0] != 0x04 || HexID[1] != 0x45 || HexID[2] != 0x61 || HexID[3] != 0x67)
	//                 return false;
	tx.Init();
	rx = DeviceInteraction(tx);
	if (!rx.bOK || rx.CheckDataList() == false)
		return false;
	HexVer[0] = rx.buff[0x10];
	HexVer[1] = rx.buff[0x11];
	return true;
}

CInteractionData CHIDDevice::Check_mima()
{
	CInteractionData tx;

	tx.buff[0] = 0x03;
	tx.buff[1] = 0x55;	//����-->�豸
	CInteractionData rx = DeviceInteraction(tx);
	if (!rx.bOK)
	{
		rx.hidResult= HIDResult_USBError;
		return rx;
	}
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
		return rx;
	}
	KEY[0] = 0x02;
	KEY[1] = 0x58;
	KEY[2] = 0xBE;
	KEY[3] = 0x24;
	//             KEY[4] = 0x8A;
	//             KEY[5] = 0x95;
	//             KEY[6] = 0x60;
	//             KEY[7] = 0x3D;

	KEY[4] = (byte)(HexID[0] - 0x77);
	KEY[5] = (byte)(HexID[1] - 0x55);
	KEY[6] = (byte)(HexID[2] - 0x33);
	KEY[7] = (byte)(HexID[3] - 0x11);

	HOP[0] = rx.buff[0x14];
	HOP[1] = rx.buff[0x15];
	HOP[2] = rx.buff[0x16];
	HOP[3] = rx.buff[0x17];

	DECRYPT();
	if ((HOP[0x02] == rx.buff[0x18]) && (HOP[3] == rx.buff[0x19]))
		rx.mimaok=true;
	else
	{
		rx.mimaok=false;
		rx.hidResult=HIDResult_MimaErr;
	}
	return rx;
}

CInteractionData CHIDDevice::ReadKeyData(byte type)
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = type;
	tx.buff[1] = 0x55;	//����-->�豸 
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_ReadError;
	}
	return rx;
}

CInteractionData CHIDDevice::ReadPSW1(byte type)
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = type;
	tx.buff[1] = 0x55;	//����-->�豸 
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x03)
	{
		rx.hidResult= HIDResult_GetPSWError3;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_GetPSWError1;
	}
	return rx;
}

CInteractionData CHIDDevice::ReadPSW2()
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x01;
	tx.buff[1] = 0xaa;	//����<--�豸
	tx.buff[4] = 0x20;
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] != 0x01)
	{
		rx.hidResult= HIDResult_GetPSWError2;		
	}
	else
	{
		rx.hidResult= HIDResult_OK;
	}
	return rx;
}

CInteractionData CHIDDevice::WriteKeyData1(int step)
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x01;
	tx.buff[1] = 0x55;	//����-->�豸
	int len=(step+1)*0x30;
	if(len<0xa0)
	{
		tx.buff[4] = 0x30;//���δ������ݵĳ��ȣ�
	}
	else
	{
		tx.buff[4]=0xa0-step*0x30;
	}
	tx.buff[5] += step*0x30;
	for (int i = 0; i < tx.buff[4]; i++)
	{
		tx.buff[i + 0x10] = KeyDataBuf[tx.buff[5]+i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_ReadError;
	}
	return rx;
}

CInteractionData CHIDDevice::WriteKeyData2(byte type)
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = type;
	tx.buff[1] = 0x55;	//����-->�豸 
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_ReadError;
	}
	return rx;
}



const byte CHIDDevice::DTnec[12]={0x00,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x23,0x01,0x26};

const byte CHIDDevice::DTmot[12]={0x00,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x23,0x01,0x26};

CInteractionData CHIDDevice::EZSReadDataStep( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x0D;
	tx.buff[1] = 0x55;	//����-->�豸
	tx.buff[0x0A] = 0x02;//cnt
	tx.buff[0x0B] = 0x6F;            

	tx.buff[0x0C] = (byte)step; 
	byte tmp_cmd = tx.buff[0x0C];

	rx = DeviceInteraction(tx);
	if (!rx.bOK)
	{
		rx.hidResult= HIDResult_USBError;
	}
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[0x01] == 0x01)
	{
		rx.hidResult= HIDResult_ReadEZSError1;
	}
	else if (rx.buff[0x01] == 0x02)
	{
		rx.hidResult= HIDResult_ReadEZSError2;
	}
	else if (rx.buff[0x0B] != 0x70)
	{
		rx.hidResult= HIDResult_ReadEZSError3;
	}
	//             else if (rx.buff[0x0C] != tmp_cmd)
	//             {
	//                 return HIDResult.ReadEZSError4;
	//             }
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_ReadEZSError5;
	}
	return rx;
}

const byte CHIDDevice::CANInfo_AT[0x09][0x10]={
	{0x00,0x0B,0xD0,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x03,0x80,0x00,0x00,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x11,0x00},
	{0x01,0x03,0x80,0x00,0x00,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x0A,0x00},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x02,0x1A,0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x03},
	{0x01,0x03,0x80,0x00,0x00,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x04,0x31,0x22,0x6F,0x00,0xFF,0xFF,0xFF,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
	{0x02,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

CInteractionData CHIDDevice::CANInfoDataStep( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4]=0x20;
	for(int i=0;i<0x10;i++)
	{
		tx.buff[i+0x10]=CANInfo_AT[step][i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

const byte CHIDDevice::CANEeprom_Cmd00[0x24]=
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x23,0x26
};

const byte CHIDDevice::CANEeprom_a[0x02][0x10]=
{
	{0x00,0x0B,0xD0,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x03,0x80,0xBF,0xE0,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x11,0x00},
};

const byte CHIDDevice::CANEeprom_b[0x03][0x10]=
{
	{0x01,0x03,0x80,0xBF,0xE0,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x0A,0x00},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x04,0x31,0x22,0x6F,0x00,0xFF,0xFF,0xFF,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
};

const byte CHIDDevice::CANEeprom_c[0x04][0x10]=
{
	{0x01,0x03,0x80,0xBF,0xE0,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x0A,0x00},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x02,0x21,0x05,0x6F,0x26,0xFF,0xFF,0xFF,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x02},
	{0x02,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

CInteractionData CHIDDevice::CANEeprom1Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4]=0x20;
	for(int i=0;i<0x10;i++)
	{
		tx.buff[i+0x10]=CANEeprom_a[step][i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANEeprom2Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANEeprom_b[step%3][i];
	}	
	if (0x01 == (step % 3))
	{
		tx.buff[0x0A + 0x10] = CANEeprom_Cmd00[step / 3];
	}            
	rx = DeviceInteraction(tx);
	rx.rxCount = CANEeprom_b[step % 3][0x0f];
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANEeprom3Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANEeprom_c[step][i];
	}	
	rx = DeviceInteraction(tx);
	rx.rxCount = CANEeprom_c[step][0x0f];
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

const byte CHIDDevice::CANwr_a[0x06][0x10]=
{
	{0x00,0x0B,0xD0,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x01,0x03,0x80,0xBF,0xE0,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x11,0x00},
	{0x01,0x03,0x80,0x00,0x00,0x08,0x02,0x3E,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0x0A,0x00},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x10,0x0B,0x31,0x22,0x0A,0x7C,0xFE,0xD6,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x21,0x5C,0x68,0xFF,0x98,0x9C,0x00,0x00,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
};

const byte CHIDDevice::CANwr_b[0x03][0x10]=
{
	{0x03,0x1C,0xF0,0xC8,0x61,0x90,0x8F,0x00,0x00,0x00,0x1C,0x00,0x00,0x0A,0x11,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x03,0x31,0x22,0x0B,0x7C,0xFE,0xD6,0x5C,0x00,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
};

const byte CHIDDevice::CANwr_bChk[0x05][0x10]=
{
	{0x01,0xAA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x08,0x10,0x0C,0x71,0x22,0x73,0x0B,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x07,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x07,0x03,0x7F,0x31,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x08,0x55,0x10,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},//c check
};

const byte CHIDDevice::CANwr_c[0x03][0x10]=
{
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x03,0x31,0x22,0x0B,0x7C,0xFE,0xD6,0x5C,0x10,0x00},
	{0x03,0x1C,0xF0,0xC8,0x61,0x90,0x8F,0x00,0x00,0x00,0x1C,0x00,0x00,0x0A,0x1E,0x01},
	{0x03,0x1C,0xF0,0xC8,0x61,0x90,0x8F,0x00,0x00,0x00,0x1C,0x00,0x00,0x0A,0x1E,0x01},
};

const byte CHIDDevice::CANwr_d[0x04][0x10]=
{
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x03,0x31,0x22,0x0B,0x7C,0xFE,0xD6,0x5C,0x10,0x00},
	{0x04,0x1C,0x00,0x48,0x45,0xD8,0x26,0x81,0xB0,0x5D,0x63,0x00,0x00,0x00,0x1E,0x01},
	{0x03,0x1C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x0A,0x1E,0x01},
	{0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

const byte CHIDDevice::CANwr_e[0x04][0x10]=
{
	{0x05,0x1C,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x01},
	{0x03,0x1C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x0A,0x20,0x01},
	{0x01,0x9C,0x00,0xBF,0xE0,0x08,0x30,0x08,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
	{0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};

CInteractionData CHIDDevice::CANWrite1Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANwr_a[step][i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANWrite2Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANwr_b[step][i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else if(rx.buff[1] == 0x03)
	{
		rx.hidResult=HIDResult_CANWriteWait;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANWrite3Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANwr_c[step][i];
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else if(rx.buff[1] == 0x03)
	{
		rx.hidResult=HIDResult_CANWriteWait;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANWrite4Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANwr_d[step%2][i];
	}
	if (step % 2 == 1)
	{
		for (int i = 0; i < 0x08; i++)
		{
			tx.buff[i + 0x13] = DTwr[0x08 * DTwrIndex + i];//DATAwr[k][i];
		}
		tx.buff[0x12] = (byte)DTwrIndex;
		DTwrIndex++;
	}
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else if (rx.buff[1] == 0x03)
	{
		rx.hidResult= HIDResult_CANWriteWait;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}

CInteractionData CHIDDevice::CANWrite5Step( int step )
{
	CInteractionData tx;
	CInteractionData rx;
	tx.buff[0] = 0x14;
	tx.buff[1] = 0x55;	//����-->�豸 
	tx.buff[4] = 0x20;
	for (int i = 0; i < 0x10; i++)
	{
		tx.buff[i + 0x10] = CANwr_e[step][i];
	}           
	rx = DeviceInteraction(tx);
	if (!rx.bOK)
		rx.hidResult= HIDResult_USBError;
	else if (rx.CheckDataList() == false)
	{
		rx.hidResult= HIDResult_RxError;
	}
	else if (rx.buff[1] == 0x00)
	{
		rx.hidResult= HIDResult_OK;
	}
	else if (rx.buff[1] == 0x03)
	{
		rx.hidResult= HIDResult_CANWriteWait;
	}
	else
	{
		rx.hidResult= HIDResult_CANErr;
	}
	return rx;
}



// CHIDDevice ��Ϣ�������


