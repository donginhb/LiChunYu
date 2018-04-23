﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using uint8_t = System.Byte; 
using WFNetLib;
using System.Threading;
using WFNetLib.Log;
using System.IO;
namespace 钥匙滚步测试
{
	public partial class Form1 : Form
	{
		lcyHashCal LCYHashCal;
		const uint8_t LeftTimesL = 0;
		const uint8_t LeftTimesM = 1;
		const uint8_t LeftTimesH = 2;

		const uint8_t ROM_9E = 0x21;
		const uint8_t ROM_9F = 0xDF;
		uint8_t[] lcyHashIn=new uint8_t[0x08];
		uint8_t[] lcyHashOut=new uint8_t[0x08];
		uint8_t[] RomDatas=new uint8_t[16];
		uint8_t[] LeftTimesAddr=new uint8_t[3];//使用的段地址
		uint8_t[] OtherLeftTimesAddr=new uint8_t[3];//未使用的段地址
		uint8_t[] LeftTimes=new uint8_t[3];
		uint8_t LeftTimes69Addr;
		uint8_t[] curHash=new uint8_t[8];
		uint8_t LeftTimes69;
		uint8_t[] SSID=new uint8_t[4];
		uint8_t[] PSW = new uint8_t[8];
		uint8_t EE00;
		uint8_t[] eeprom=new uint8_t[256]
		{
			//;00-0F
			0x00,	0x47,	0x80,	0x22,	0x71,	0x85,	0x87,	0x2F,
			0x9B,	0xCB,	0x61,	0x90,	0x8F,	0xE3,	0x74,	0x90,
			//;10-1F
			0xA4,	0xA1,	0x0D,	0x94,	0x2F,	0x47,	0xFD,	0xF6,
			0x60,	0xDD,	0xC6,	0xC2,	0xD4,	0xD6,	0xFB,	0xB3,
			//;20-2F
			0xC8,	0xB3,	0x58,	0x91,	0x80,	0xCD,	0xE0,	0x27,
			0x5D,	0x6A,	0xA0,	0xD9,	0xDF,	0x73,	0xFE,	0xF1,
			//;30-3F
			0x50,	0x5D,	0xFA,	0x2C,	0x2C,	0x2E,	0xA5,	0xE5,
			0xA8,	0x69,	0xED,	0x15,	0x21,	0xF4,	0x0C,	0x2F,
			//;40-4F
			0x2A,	0xEF,	0xCE,	0x93,	0x03,	0xE9,	0xC0,	0xA7,
			0xE2,	0x1E,	0xC3,	0x1A,	0x77,	0x86,	0x65,	0xB8,
			//;50-5F
			0xAB,	0x7D,	0xD2,	0x1F,	0x2D,	0xFE,	0xC7,	0x84,
			0x9B,	0x1C,	0xDF,	0xF7,	0xDE,	0x7C,	0xB0,	0xC4,
			//;60-6F
			0xF1,	0x15,	0xFB,	0x45,	0x14,	0xA8,	0x79,	0x78,
			0x23,	0x7F,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			//;70-7F
			0xFF,	0xFF,	0xFF,	0x4F,	0x8F,	0xEC,	0x14,	0x8E,
			0x69,	0x86,	0xE5,	0x7F,	0xFF,	0x00,	0x00,	0x02,
			//;80-8F
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;90-9F
			0x01,	0x01,	0x01,	0x01,	0xFF,	0xFF,	0xFF,	0xFF,
			0x73,	0x0C,	0xFF,	0xFF,	0xFF,	0xFF,	0x14,	0xeC,
			//;a0-aF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;b0-bF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;c0-cF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;d0-dF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;e0-eF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F,
			//;f0-fF
			0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,	0xFF,
			0x55,	0x24,	0x29,	0x2C,	0xCB,	0x61,	0x90,	0x8F
		};
		void BAT_ON()
		{

		}
		List<byte[]> needHash;
		uint8_t RomData_ReadByte(uint8_t addr)
		{
			return eeprom[addr];
		}
		void RomData_ReadBytes(uint8_t addr,ref uint8_t[] pBuf,uint8_t nLen)
		{
			for(int i=0;i<nLen;i++)
			{
				pBuf[i]=eeprom[addr+i];
			}
		}
		void RomData_WriteByte(uint8_t addr, uint8_t x)
		{
			eeprom[addr] = x;
		}
		void RomData_WriteByte(int addr, uint8_t x)
		{
			eeprom[addr] = x;
		}
		void RomData_WriteBytes(uint8_t addr,uint8_t[] pBuf,uint8_t nLen)
		{
			for (int i = 0; i < nLen; i++)
			{
				eeprom[addr + i] = pBuf[i];
			}
		}
		void ReverseRom(int addr)
		{
			if (RomData_ReadByte((uint8_t)addr) != 0xff)
			{
				RomData_WriteByte((uint8_t)addr, 0xff);
			}
			else
			{
				RomData_WriteByte((uint8_t)addr, 0);
			}
		}		
		public Form1()
		{
			InitializeComponent();
		}
		void NVIC_SystemReset()
		{
			bExit = true;
			MessageBox.Show("复位");
		}
		private void Form1_Load(object sender, EventArgs e)
		{
			LCYHashCal = new lcyHashCal();			
			needHash=new List<byte[]>();
			FileStream fs;
			fs = new FileStream("Hash.txt", FileMode.Open);			
			StreamReader sw = new StreamReader(fs);
			fs.Seek(0, SeekOrigin.Begin);
			string str;
			while(true)
			{
				try
				{
					str = sw.ReadLine();
					byte[] h = WFNetLib.StringFunc.StringsFunction.strToHexByte(str, " ");
					needHash.Add(h);
				}
				catch// (System.Exception ex)
				{
					break;
				}
			}			
			//关闭流
			sw.Close();
			fs.Close();/**/
		}

		private void button1_Click(object sender, EventArgs e)
		{
			timer1.Enabled = true;
			Thread xx=new Thread(new ThreadStart(CalcTest));
			xx.Start();
		}
		bool bExit;
		void CalcTest()
		{
			int index = 0;
			bExit = false;
			t = 0;			
			while (true)
			{
				index++;
				this.Invoke((EventHandler)(delegate
				{
					textBox2.Text = index.ToString();
					progressBar1.PerformStep();
				}));
				if (index == 3968)
					bExit = false;
				GetKeyParam();
				UsedDEC();
				if (bExit)
				{
					this.Invoke((EventHandler)(delegate
					{
						timer1.Enabled = false;
					}));
					return;
				}
			}
		}
		int t;
		private void timer1_Tick(object sender, EventArgs e)
		{
			this.Invoke((EventHandler)(delegate
			{
				t++;
				textBox3.Text = t.ToString();
			}));
		}
	}
}
