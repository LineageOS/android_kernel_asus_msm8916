
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/string.h>

#include <linux/mount.h>
#include <linux/unistd.h>
#include "test_lib.h"
#include "ini.h"

# define FT5X46_TEST 		1 /*1: Enable, 0: Disable*/

/*-----------------------------------------------------------
Error Code Define
-----------------------------------------------------------*/
#define		ERROR_CODE_OK						0x00
#define		ERROR_CODE_CHECKSUM_ERROR			0x01
#define		ERROR_CODE_INVALID_COMMAND			0x02
#define		ERROR_CODE_INVALID_PARAM			0x03
#define		ERROR_CODE_IIC_WRITE_ERROR			0x04
#define		ERROR_CODE_IIC_READ_ERROR			0x05
#define		ERROR_CODE_WRITE_USB_ERROR			0x06
#define		ERROR_CODE_WAIT_RESPONSE_TIMEOUT	0x07
#define		ERROR_CODE_PACKET_RE_ERROR			0x08
#define		ERROR_CODE_NO_DEVICE				0x09
#define		ERROR_CODE_WAIT_WRITE_TIMEOUT		0x0a
#define		ERROR_CODE_READ_USB_ERROR			0x0b
#define		ERROR_CODE_COMM_ERROR				0x0c
#define		ERROR_CODE_ALLOCATE_BUFFER_ERROR	0x0d
#define		ERROR_CODE_DEVICE_OPENED			0x0e
#define		ERROR_CODE_DEVICE_CLOSED			0x0f


struct Test_ConfigParam_FT5X46{
	short RawDataTest_Low_Min;
	short RawDataTest_Low_Max;
	short RawDataTest_High_Min;
	short RawDataTest_High_Max;

	boolean RawDataTest_LowFreq;
	boolean RawDataTest_HighFreq;
	boolean SCapCbTest_SetWaterproof_ON;
	boolean SCapCbTest_SetWaterproof_OFF;

	boolean SCapRawDataTest_SetWaterproof_ON;
	boolean SCapRawDataTest_SetWaterproof_OFF;

	boolean RawDataTest;
	boolean SCAPCbTest;
	boolean SCAPRawDataTest;

	//zax 20150112++++++++++++++++++++++++++++++++++
	boolean UniformityTest;
	boolean Uniformity_CheckTx;
	boolean Uniformity_CheckRx;
	boolean Uniformity_CheckMinMax;
	short Uniformity_MinMax_Hole;
	short Uniformity_Tx_Hole;
	short Uniformity_Rx_Hole;
	//zax 20150112-------------------------------------------

	
	short WeakShortTest_CG_Min;
	short WeakShortTest_CC_Min;

	short valid_node[TX_NUM_MAX][RX_NUM_MAX];
	short InvalidNodes[TX_NUM_MAX][RX_NUM_MAX];	
	short RawDataTest_Low_Min_node[TX_NUM_MAX][RX_NUM_MAX];	
	short RawDataTest_Low_Max_node[TX_NUM_MAX][RX_NUM_MAX];	
	short RawDataTest_High_Min_node[TX_NUM_MAX][RX_NUM_MAX];	
	short RawDataTest_High_Max_node[TX_NUM_MAX][RX_NUM_MAX];	

	short SCapCbTest_ON_Min_Value;
	short SCapCbTest_ON_Max_Value;
	short SCapCbTest_OFF_Min_Value;
	short SCapCbTest_OFF_Max_Value;

	short SCapRawDataTest_ON_Max_Value;
	short SCapRawDataTest_ON_Min_Value;
	short SCapRawDataTest_OFF_Max_Value;
	short SCapRawDataTest_OFF_Min_Value;

	short SCapCbTest_ON_Max[TX_NUM_MAX][RX_NUM_MAX];
	short SCapCbTest_ON_Min[TX_NUM_MAX][RX_NUM_MAX];
	short SCapCbTest_OFF_Max[TX_NUM_MAX][RX_NUM_MAX];
	short SCapCbTest_OFF_Min[TX_NUM_MAX][RX_NUM_MAX];

	short SCapRawDataTest_ON_Max[TX_NUM_MAX][RX_NUM_MAX];
	short SCapRawDataTest_ON_Min[TX_NUM_MAX][RX_NUM_MAX];
	short SCapRawDataTest_OFF_Max[TX_NUM_MAX][RX_NUM_MAX];
	short SCapRawDataTest_OFF_Min[TX_NUM_MAX][RX_NUM_MAX];

	short SCapCbTest_Min;
	short SCapCbTest_Max;

	short SCapRawDataTest_OffMin;
	short SCapRawDataTest_OffMax;
	short SCapRawDataTest_OnMin;
	short SCapRawDataTest_OnMax;

};

/*test section*/
#define Section_TestItem 	"TestItem"
#define Section_BaseSet "Basic_Threshold"
#define Section_SpecialSet "SpecialSet"
#define Section_INVALID_NODE "INVALID_NODE"

#define Item_RawDataTest_Low_Min "RawDataTest_Low_Min"
#define Item_RawDataTest_Low_Max "RawDataTest_Low_Max"
#define Item_RawDataTest_High_Min "RawDataTest_High_Min"
#define Item_RawDataTest_High_Max "RawDataTest_High_Max"
#define Item_RawDataTest_LowFreq "RawDataTest_LowFreq"
#define Item_RawDataTest_HighFreq "RawDataTest_HighFreq"

#define Item_WeakShortTest_CG_Min "WeakShortTest_CG"
#define Item_WeakShortTest_CC_Min "WeakShortTest_CC"/*�ۺϲ��������������*/

#define Special_RawDataTest_Low_Min "RawData_Min_Low_Tx"
#define Special_RawDataTest_Low_Max "RawData_Max_Low_Tx"
#define Special_RawDataTest_High_Min "RawData_Min_High_Tx"
#define Special_RawDataTest_High_Max "RawData_Max_High_Tx"
#define SCapCbTest_SetWaterproof_on "ScapCBTest_SetWaterproof_ON"
#define SCapCbTest_SetWaterproof_off "ScapCBTest_SetWaterproof_OFF"

#define SCapRawDataTest_SetWaterproof_on "SCapRawDataTest_SetWaterproof_ON"
#define SCapRawDataTest_SetWaterproof_off "SCapRawDataTest_SetWaterproof_OFF"

#define RawData_Test "RAWDATA_TEST"

#define SCAP_Cb_Test "SCAP_CB_TEST"
#define SCAP_RawData_Test "SCAP_RAWDATA_TEST"

#define SCapCbTest_ON_max "SCapCbTest_ON_Max"
#define SCapCbTest_ON_min "SCapCbTest_ON_Min"
#define SCapCbTest_OFF_max "SCapCbTest_OFF_Max"
#define SCapCbTest_OFF_min "SCapCbTest_OFF_Min"

#define SCapRawDataTestONMax "SCapRawDataTest_ON_Max"
#define SCapRawDataTestONMin "SCapRawDataTest_ON_Min"
#define SCapRawDataTestOFFMax "SCapRawDataTest_OFF_Max"
#define SCapRawDataTestOFFMin "SCapRawDataTest_OFF_Min"

#define SCapCbTest_min "SCapCbTest_Min"
#define SCapCbTest_max "SCapCbTest_Max"

#define SCapRawDataTest_Offmin "SCapRawDataTest_OFF_Min"
#define SCapRawDataTest_Offmax "SCapRawDataTest_OFF_Max"
#define SCapRawDataTest_Onmin "SCapRawDataTest_ON_Min"
#define SCapRawDataTest_Onmax "SCapRawDataTest_ON_Max"

#define ScapCB_ON_Max "ScapCB_ON_Max_"
#define ScapCB_ON_Min "ScapCB_ON_Min_"
#define ScapCB_OFF_Max "ScapCB_OFF_Max_"
#define ScapCB_OFF_Min "ScapCB_OFF_Min_"

#define ScapRawData_OFF_Min "ScapRawData_OFF_Min_"
#define ScapRawData_OFF_Max "ScapRawData_OFF_Max_"
#define ScapRawData_ON_Min "ScapRawData_ON_Min_"
#define ScapRawData_ON_Max "ScapRawData_ON_Max_"

//zax 20150112++++++++++++++++++++++++++++++++++
#define Item_UNIFORMITY_TEST 				"UNIFORMITY_TEST"
#define UNIFORMITY_CHECKTX 				"UniformityTest_Check_Tx"
#define UNIFORMITY_CHECKRX 				"UniformityTest_Check_Rx"
#define UNIFORMITY_CHECKMINMAX 			"UniformityTest_Check_MinMax"
#define UNIFORMITY_MINMAX_HOLE			"UniformityTest_MinMax_Hole"
#define UNIFORMITY_Tx_Hole 					"UniformityTest_Tx_Hole"
#define UNIFORMITY_Rx_Hole					"UniformityTest_Rx_Hole"
//zax 20150112-------------------------------------------

#define InvalidNode "InvalidNode"
#define		REG_ScCbBuf0	0x4E
#define		REG_ScCbBuf1	0x4F
#define		REG_ScCbAddrR	0x45
#define		REG_LINE_NUM	0x01
#define		REG_RawBuf0	0x36
/*
static inline void kernel_fpu_begin(void)
{
struct thread_info *me = current_thread_info();
preempt_disable();
if (me->status & TS_USEDFPU)
__save_init_fpu(me->task);
else
clts();
}

static inline void kernel_fpu_end(void)
{
stts();
preempt_enable();
}*/


static FTS_I2c_Read_Function focal_I2C_Read;
static FTS_I2c_Write_Function focal_I2C_write;

static boolean bRawdataTest = true;

static short iTxNum = 0;
static short iRxNum = 0;
static int SCap_iTxNum = 0;
static int SCap_iRxNum = 0;
//static unsigned char Reg_VolAddr = 0x05;

static struct Test_ConfigParam_FT5X46 g_TestParam_FT5X46;

static char *g_testparamstring = NULL;

//zax 20150112++++++++++++++++++++++++++++++++++
int total_item;
//static int rawdata[TX_NUM_MAX][RX_NUM_MAX];
static int SCap_rawData[TX_NUM_MAX][RX_NUM_MAX];
short TxLinearity[TX_NUM_MAX][RX_NUM_MAX] ;
short RxLinearity[TX_NUM_MAX][RX_NUM_MAX];
static short minHole[TX_NUM_MAX][RX_NUM_MAX];
static short maxHole[TX_NUM_MAX][RX_NUM_MAX];
static short invalide[TX_NUM_MAX][RX_NUM_MAX];
boolean m_bV3TP;
#define     REG_MAPPING_SWITCH      	0X54
#define     MIN_HOLE_LEVEL			0x80
#define	NODE_INVALID_TYPE 		0
#define	NODE_VALID_TYPE			1
#define 	NODE_KEY_TYPE 				2
//zax 20150112--------------------------------------------

//zax 20141116 ++++++++++++++
extern int TPrawdata[TX_NUM_MAX][RX_NUM_MAX];
extern int TX_NUM;
extern int RX_NUM;
extern int SCab_1;
extern int SCab_2;
extern int SCab_3;
extern int SCab_4;
extern int SCab_5;
extern int SCab_6;
extern int SCab_7;
extern int SCab_8;
//zax 20141116 -------------------
static short rawdata[TX_NUM_MAX][RX_NUM_MAX];
static int SCap_rawData[TX_NUM_MAX][RX_NUM_MAX];
//zax 20141116+++++++++++++++++++++++
static int Save_rawData[TX_NUM_MAX][RX_NUM_MAX];
//zax 20141116-------------------------------
static void TestTp(void);
void Ft5336_TestRawDataAndDiff(void);
static int StartScan(void);
//static int SetDriverVol(unsigned char vol);
//static char GetDriverVol(void);
//static void SetTxRxNum(short txnum,short rxnum);
static short GetTxNum(void);
static short GetRxNum(void);
//static char GetOffsetTx(unsigned char txindex);
//static char GetOffsetRx(unsigned char rxindex);
//static void SetOffsetTx(unsigned char txindex,unsigned char offset);
//static void SetOffsetRx(unsigned char rxindex,unsigned char offset);
static void GetRawData(short RawData[TX_NUM_MAX][RX_NUM_MAX]);
boolean StartTestTP(void);

static boolean FT5X46_TestItem(void);
boolean TestItem_RawDataTest_FT5X46(void);
boolean TestItem_WeakShortTest(void);
int WeakShort_GetAdcData( int AllAdcDataLen, int *pRevBuffer );
static boolean TestItem_SCapCbTest(void);
static boolean TestItem_SCapRawDataTest(void);
//zax 20150112++++++++++++++++++++++++++++++++++
static boolean TestItem_UniformityTest(void);
//zax 20150112-------------------------------------------
static void GetTestParam(void);

static void focal_msleep(int ms)
{
	msleep(ms);
}
int SetParamData(char * TestParamData)
{
	g_testparamstring = TestParamData;

	GetTestParam();
	return 0;
}
void FreeTestParamData(void)
{
	if(g_testparamstring)
		kfree(g_testparamstring);

	g_testparamstring = NULL;
}
/*
static short focal_abs(short value)
{
short absvalue = 0;
if(value > 0)
absvalue = value;
else
absvalue = 0 - value;

return absvalue;
}
*/
//zax 20141116 ++++++++++++++
void focal_save_scap_sample1(void)
{
	int i=0;
	int j=0;

	for (i = 0; i < iTxNum + 8; i++) {
		for (j = 0; j < iRxNum; j++) {
			TPrawdata[i][j] = Save_rawData[i][j];
			total_item=g_TestParam_FT5X46.RawDataTest_HighFreq+g_TestParam_FT5X46.Uniformity_CheckTx+g_TestParam_FT5X46.Uniformity_CheckRx;
			//databuf[i*TX_NUM_MAX+j]=SCap_rawData[i][j];
			//printk("zax SCap_rawData[i][j] %d\n",SCap_rawData[i][j]);
		}
	}
}
//zax 20141116 --------------------
void focal_save_mcap_sample(short *databuf, int* num1,int* num2)
{
	int i=0;
	int j=0;
	for (i = 0; i < TX_NUM_MAX; i++) {
		for(j = 0; j < RX_NUM_MAX; j++) {
			//databuf[i*TX_NUM_MAX+j]=rawdata[i][j];
			databuf[i*TX_NUM_MAX+j]=rawdata[i][j];
			//printk("zax SCap_rawData[i][j] %d\n",SCap_rawData[i][j]);
		}
	}
	
	*num1 = iTxNum;
	*num2 = iRxNum;
}

static int GetParamValue(char *section, char *ItemName, int defaultvalue) 
{
	int paramvalue = defaultvalue;
	char value[512];
	memset(value , 0x00, sizeof(value));
	if(ini_get_key(g_testparamstring, section, ItemName, value) < 0) {
		return paramvalue;
	} else {
		paramvalue = atoi(value);
	}

	return paramvalue;
}

static int GetParamString(char *section, char *ItemName, char *defaultvalue) {
	char value[512];
	int len = 0;
	memset(value , 0x00, sizeof(value));
	if(ini_get_key(g_testparamstring, section, ItemName, value) < 0) {
		return 0;
	} else {
		len = sprintf(defaultvalue, "%s", value);
	}

	return len;
}

static void GetTestParam(void)
{
	char str_tmp[128], str_node[64], str_value[512];
	int j, index, valuelen = 0, i = 0, k = 0;
	int iLenght = 0;
	memset(str_tmp, 0, sizeof(str_tmp));
	memset(str_node, 0, sizeof(str_node));
	memset(str_value, 0, sizeof(str_value));
	//zax 20141116+++++++++++++++++++++++
	memset(Save_rawData, 0, sizeof(Save_rawData));
	//zax 20141116-----------------------------
#if (FT5X46_TEST == 1)
	g_TestParam_FT5X46.RawDataTest_Low_Min = GetParamValue(Section_BaseSet, Item_RawDataTest_Low_Min, 3000);
	g_TestParam_FT5X46.RawDataTest_Low_Max = GetParamValue(Section_BaseSet, Item_RawDataTest_Low_Max, 15000);
	g_TestParam_FT5X46.RawDataTest_High_Min = GetParamValue(Section_BaseSet, Item_RawDataTest_High_Min, 3000);
	g_TestParam_FT5X46.RawDataTest_High_Max = GetParamValue(Section_BaseSet, Item_RawDataTest_High_Max, 15000);

	g_TestParam_FT5X46.RawDataTest_LowFreq = GetParamValue(Section_BaseSet, Item_RawDataTest_LowFreq, 0);
	g_TestParam_FT5X46.RawDataTest_HighFreq = GetParamValue(Section_BaseSet, Item_RawDataTest_HighFreq, 0);
	//zax 20150112++++++++++++++++++++++++++++++++++
	g_TestParam_FT5X46.UniformityTest = GetParamValue(Section_TestItem, Item_UNIFORMITY_TEST, 1);
	g_TestParam_FT5X46.Uniformity_CheckTx = GetParamValue(Section_BaseSet, UNIFORMITY_CHECKTX, 1);
	g_TestParam_FT5X46.Uniformity_CheckRx = GetParamValue(Section_BaseSet, UNIFORMITY_CHECKRX, 1);
	g_TestParam_FT5X46.Uniformity_CheckMinMax = GetParamValue(Section_BaseSet, UNIFORMITY_CHECKMINMAX, 1);
	g_TestParam_FT5X46.Uniformity_MinMax_Hole = GetParamValue(Section_BaseSet, UNIFORMITY_MINMAX_HOLE, 70);
	g_TestParam_FT5X46.Uniformity_Tx_Hole = GetParamValue(Section_BaseSet, UNIFORMITY_Tx_Hole, 20);
	g_TestParam_FT5X46.Uniformity_Rx_Hole = GetParamValue(Section_BaseSet, UNIFORMITY_Rx_Hole, 20);
	//printk("20150111   %d, %d, %d, %d, %d, %d, %d\n",g_TestParam_FT5X46.UniformityTest,g_TestParam_FT5X46.Uniformity_CheckTx,	g_TestParam_FT5X46.Uniformity_CheckRx,g_TestParam_FT5X46.Uniformity_CheckMinMax,	g_TestParam_FT5X46.Uniformity_MinMax_Hole,g_TestParam_FT5X46.Uniformity_Tx_Hole,g_TestParam_FT5X46.Uniformity_Rx_Hole);
	//zax 20150112-------------------------------------------
	g_TestParam_FT5X46.WeakShortTest_CG_Min = GetParamValue(Section_BaseSet, Item_WeakShortTest_CG_Min, 2000);
	g_TestParam_FT5X46.WeakShortTest_CC_Min = GetParamValue(Section_BaseSet, Item_WeakShortTest_CC_Min, 2000);

	
	g_TestParam_FT5X46.SCapCbTest_SetWaterproof_ON=GetParamValue(Section_BaseSet, SCapCbTest_SetWaterproof_on, 0);
	g_TestParam_FT5X46.SCapCbTest_SetWaterproof_OFF=GetParamValue(Section_BaseSet, SCapCbTest_SetWaterproof_off, 0);

	g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_ON=GetParamValue(Section_BaseSet, SCapRawDataTest_SetWaterproof_on, 0);
	g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_OFF=GetParamValue(Section_BaseSet, SCapRawDataTest_SetWaterproof_off, 0);

	g_TestParam_FT5X46.SCapCbTest_ON_Min_Value=GetParamValue(Section_BaseSet,SCapCbTest_ON_min , 0);
	g_TestParam_FT5X46.SCapCbTest_ON_Max_Value=GetParamValue(Section_BaseSet,SCapCbTest_ON_max , 240);

	g_TestParam_FT5X46.SCapCbTest_OFF_Min_Value=GetParamValue(Section_BaseSet,SCapCbTest_OFF_min , 0);
	g_TestParam_FT5X46.SCapCbTest_OFF_Max_Value=GetParamValue(Section_BaseSet,SCapCbTest_OFF_max , 240);

	g_TestParam_FT5X46.SCapRawDataTest_ON_Max_Value=GetParamValue(Section_BaseSet,SCapRawDataTestONMax , 8500);
	g_TestParam_FT5X46.SCapRawDataTest_ON_Min_Value=GetParamValue(Section_BaseSet,SCapRawDataTestONMin , 5000);
	g_TestParam_FT5X46.SCapRawDataTest_OFF_Max_Value=GetParamValue(Section_BaseSet,SCapRawDataTestOFFMax , 8500);
	g_TestParam_FT5X46.SCapRawDataTest_OFF_Min_Value=GetParamValue(Section_BaseSet,SCapRawDataTestOFFMin , 5000);

	g_TestParam_FT5X46.RawDataTest=GetParamValue(Section_TestItem, RawData_Test, 0);
	g_TestParam_FT5X46.SCAPCbTest=GetParamValue(Section_TestItem, SCAP_Cb_Test, 0);
	g_TestParam_FT5X46.SCAPRawDataTest=GetParamValue(Section_TestItem, SCAP_RawData_Test, 0);

	/* +++print test Item+++ */
	printk("[Focal][Selftest Item]  RawDataTest = %d, RawDataTest_LowFreq = %d, RawDataTest_HighFreq = %d \n",
	g_TestParam_FT5X46.RawDataTest, g_TestParam_FT5X46.RawDataTest_LowFreq, g_TestParam_FT5X46.RawDataTest_HighFreq);

	printk("[Focal][Selftest Item]  SCAPCbTest = %d, SCapCbTest_SetWaterproof_ON = %d, RawDataTest_HighFreq = %d \n",
	g_TestParam_FT5X46.SCAPCbTest, g_TestParam_FT5X46.SCapCbTest_SetWaterproof_ON, g_TestParam_FT5X46.SCapCbTest_SetWaterproof_OFF);

	printk("[Focal][Selftest Item]  SCAPRawDataTest = %d, SCapRawDataTest_SetWaterproof_ON = %d, SCapRawDataTest_SetWaterproof_OFF = %d \n",
	g_TestParam_FT5X46.SCAPRawDataTest, g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_ON, g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_OFF);
	/* ---print test Item--- */

	g_TestParam_FT5X46.SCapCbTest_Min=GetParamValue(Section_BaseSet, SCapCbTest_min, 0);
	g_TestParam_FT5X46.SCapCbTest_Max=GetParamValue(Section_BaseSet, SCapCbTest_max, 240);

	
	g_TestParam_FT5X46.SCapRawDataTest_OffMin=GetParamValue(Section_BaseSet, SCapRawDataTest_Offmin, 150);
	g_TestParam_FT5X46.SCapRawDataTest_OffMax=GetParamValue(Section_BaseSet, SCapRawDataTest_Offmax, 1000);
	g_TestParam_FT5X46.SCapRawDataTest_OnMin=GetParamValue(Section_BaseSet, SCapRawDataTest_Onmin, 150);
	g_TestParam_FT5X46.SCapRawDataTest_OnMax=GetParamValue(Section_BaseSet, SCapRawDataTest_Onmax, 1000);

	for (i = 0; i < TX_NUM_MAX; i++) 
	{
		for (j = 0; j < RX_NUM_MAX; j++) 
		{
			g_TestParam_FT5X46.RawDataTest_Low_Min_node[i][j] = g_TestParam_FT5X46.RawDataTest_Low_Min;
			g_TestParam_FT5X46.RawDataTest_Low_Max_node[i][j] = g_TestParam_FT5X46.RawDataTest_Low_Max;
			g_TestParam_FT5X46.RawDataTest_High_Min_node[i][j] = g_TestParam_FT5X46.RawDataTest_High_Min;
			g_TestParam_FT5X46.RawDataTest_High_Max_node[i][j] = g_TestParam_FT5X46.RawDataTest_High_Max;

			g_TestParam_FT5X46.SCapCbTest_ON_Max[i][j] = g_TestParam_FT5X46.SCapCbTest_Max;
			g_TestParam_FT5X46.SCapCbTest_ON_Min[i][j] = g_TestParam_FT5X46.SCapCbTest_Min;
			g_TestParam_FT5X46.SCapCbTest_OFF_Max[i][j] = g_TestParam_FT5X46.SCapCbTest_Max;
			g_TestParam_FT5X46.SCapCbTest_OFF_Min[i][j] = g_TestParam_FT5X46.SCapCbTest_Min;

			g_TestParam_FT5X46.SCapRawDataTest_ON_Max[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OnMax;
			g_TestParam_FT5X46.SCapRawDataTest_ON_Min[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OnMin;
			g_TestParam_FT5X46.SCapRawDataTest_OFF_Max[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OffMax;
			g_TestParam_FT5X46.SCapRawDataTest_OFF_Min[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OffMin;

			g_TestParam_FT5X46.valid_node[i][j] = 1;
			g_TestParam_FT5X46.InvalidNodes[i][j]=1;
		}
	}

	for (i = 0; i < TX_NUM_MAX; i++) 
	{
		for (j = 0; j < RX_NUM_MAX; j++) 
		{
			memset(str_tmp, 0x00, sizeof(str_tmp));
			sprintf(str_tmp, "%s[%d][%d]",InvalidNode,(i+1),(j+1));
			g_TestParam_FT5X46.valid_node[i][j] = GetParamValue(Section_INVALID_NODE, str_tmp, 1);
		}
	}

	/////////////////////////////////RawDataTest_Low_Min_node

	for (i = 0; i < TX_NUM_MAX; i++) 
	{		
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",Special_RawDataTest_Low_Min,(i+1));
		
		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.RawDataTest_Low_Min_node[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for (j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.RawDataTest_Low_Min_node[i][j] = g_TestParam_FT5X46.RawDataTest_Low_Min;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	/////////////////////////////////RawDataTest_Low_Max_node
	for (i = 0; i < TX_NUM_MAX; i++) {
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",Special_RawDataTest_Low_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.RawDataTest_Low_Max_node[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.RawDataTest_Low_Max_node[i][j] = g_TestParam_FT5X46.RawDataTest_Low_Max;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	


	/////////////////////////////////RawDataTest_High_Min_node
	for (i = 0; i < TX_NUM_MAX; i++) {
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",Special_RawDataTest_High_Min,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.RawDataTest_High_Min_node[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.RawDataTest_High_Min_node[i][j] = g_TestParam_FT5X46.RawDataTest_High_Min;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	/////////////////////////////////RawDataTest_High_Max_node
	for(i=0; i<TX_NUM_MAX; i++) 
	{		
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",Special_RawDataTest_High_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.RawDataTest_High_Max_node[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.RawDataTest_High_Max_node[i][j] = g_TestParam_FT5X46.RawDataTest_High_Max;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	//////��ȡ��ֵ�������������ã�����Basic_Threshold���

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapCB_ON_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		printk("[jacob] ScapCB_ON_Max_%d gat len = %d \n", i, valuelen);
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapCbTest_ON_Max[i][k] = (short)(atoi(str_tmp));
					//printk("zax8999  %d\n",g_TestParam_FT5X46.SCapCbTest_ON_Max[i][k] );
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapCbTest_ON_Max[i][j] = g_TestParam_FT5X46.SCapCbTest_Max;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapCB_ON_Min,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		printk("[jacob] ScapCB_ON_Min%d gat len = %d \n", i, valuelen);
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapCbTest_ON_Min[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapCbTest_ON_Min[i][j] = g_TestParam_FT5X46.SCapCbTest_Min;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapCB_OFF_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		printk("[jacob] ScapCB_OFF_Max%d gat len = %d \n", i, valuelen);
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapCbTest_OFF_Max[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapCbTest_OFF_Max[i][j] = g_TestParam_FT5X46.SCapCbTest_Max;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapCB_OFF_Min,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);

		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapCbTest_OFF_Min[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapCbTest_OFF_Min[i][j] = g_TestParam_FT5X46.SCapCbTest_Min;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	//////��ȡ��ֵ�������������ã�����Basic_Threshold���

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapRawData_OFF_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);

		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapRawDataTest_OFF_Max[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapRawDataTest_OFF_Max[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OffMax;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	for (i = 0; i < 2; i++) {

		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;
		
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapRawData_OFF_Min,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapRawDataTest_OFF_Min[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapRawDataTest_OFF_Min[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OffMin;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	//////��ȡ��ֵ�������������ã�����Basic_Threshold���

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;
		
		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapRawData_ON_Max,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapRawDataTest_ON_Max[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapRawDataTest_ON_Max[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OnMax;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	

	for (i = 0; i < 2; i++) {
		if(0 == i)iLenght = RX_NUM_MAX;
		if(1 == i)iLenght = TX_NUM_MAX;

		memset(str_value, 0x00, sizeof(str_value));
		memset(str_tmp, 0x00, sizeof(str_tmp));
		sprintf(str_tmp, "%s%d",ScapRawData_ON_Min,(i+1));

		valuelen = GetParamString(Section_SpecialSet, str_tmp, str_value);
		
		if (valuelen > 0) 
		{
			index = 0;
			k = 0;
			memset(str_tmp, 0x00, sizeof(str_tmp));
			for(j=0; j<valuelen; j++) 
			{
				if(',' == str_value[j]) 
				{
					g_TestParam_FT5X46.SCapRawDataTest_ON_Min[i][k] = (short)(atoi(str_tmp));
					index = 0;
					memset(str_tmp, 0x00, sizeof(str_tmp));
					k++;
				} 
				else 
				{
					if(' ' == str_value[j])
						continue;
					str_tmp[index] = str_value[j];
					index++;
				}
			}
		} 
		else 
		{
			for(j = 0; j < RX_NUM_MAX; j++) 
			{
				g_TestParam_FT5X46.SCapRawDataTest_ON_Min[i][j] = g_TestParam_FT5X46.SCapRawDataTest_OnMin;
#ifdef FOCAL_DBG
#endif
			}
		}			
	}	
#endif
}

int Init_I2C_Read_Func(FTS_I2c_Read_Function fpI2C_Read)
{
	focal_I2C_Read = fpI2C_Read;
	return 0;
}

int Init_I2C_Write_Func(FTS_I2c_Write_Function fpI2C_Write)
{
	focal_I2C_write = fpI2C_Write;
	return 0;
}

static int ReadReg(unsigned char RegAddr, unsigned char *RegData)
{
	return focal_I2C_Read(&RegAddr, 1, RegData, 1);
}

static int WriteReg(unsigned char RegAddr, unsigned char RegData)
{
	unsigned char cmd[2] = {0};
	cmd[0] = RegAddr;
	cmd[1] = RegData;
	return focal_I2C_write(cmd, 2);
}

static int StartScan(void)
{
	int err = 0, i = 0;
	unsigned char regvalue = 0x00;

	/*scan*/
	//if(WriteReg(0x00,0x40) < 0) {
	//	FTS_DBG("Enter factory failure\n");
	//}
	//focal_msleep(100);

	err = ReadReg(0x00,&regvalue);
	if (err < 0) 
	{
		FTS_DBG("Enter StartScan Err. RegValue: %d \n", regvalue);
		return err;
	}
	else 
	{
		regvalue |= 0x80;
		err = WriteReg(0x00,regvalue);
		if (err < 0) 
		{	//printk("zax StartScan3 \n");
			return err;
		}
		else 
		{
			for(i=0; i<20; i++) {
				focal_msleep(8);
				err = ReadReg(0x00,&regvalue);
				if (err < 0) 
				{//printk("zax StartScan4 \n");
					return err;
				} 
				else 
				{
					if (0 == (regvalue >> 7)) {
						break;
					}
				}
			}
			if (i >= 20) 
			{//printk("zax StartScan5 \n");
				return -5;
			}
		}
	}
	//printk("zax StartScan6\n");
	return 0;
}	
/*
static int SetDriverVol(unsigned char vol)
{
	return WriteReg(Reg_VolAddr,vol);
}

static char GetDriverVol(void)
{
	char vol = 0;
	unsigned char regvalue = 0x00;

	ReadReg(Reg_VolAddr,&regvalue);
	vol = (char)regvalue;

	return vol;
}

static void SetTxRxNum(short txnum,short rxnum)
{
	iTxNum = txnum;
	iRxNum = rxnum;
}
*/
static short GetTxNum(void)
{
	short txnum = 0;
	unsigned char regvalue = 0x00;

	if(WriteReg(0x00, 0x40) >= 0)
	{
		ReadReg(0x02,&regvalue);
		txnum = (short)regvalue;
	}
	else
	{
		return TX_NUM_MAX;
	}

	return txnum;
}

static short GetRxNum(void)
{
	short rxnum = 0;
	unsigned char regvalue = 0x00;

	if(WriteReg(0x00, 0x40) >= 0)
	{
		ReadReg(0x03,&regvalue);
		rxnum = (short)regvalue;
	}
	else
	{
		return RX_NUM_MAX;
	}

	return rxnum;
}
/*
static char GetOffsetTx(unsigned char txindex)
{
	char txoffset = 0;
	char regvalue = 0x00;

	ReadReg((0xad + txindex),&regvalue);
	txoffset = regvalue;

	return txoffset;
}

static char GetOffsetRx(unsigned char rxindex)
{
	char rxoffset = 0;
	char regvalue = 0x00;

	ReadReg((0xd6 + rxindex),&regvalue);
	rxoffset = regvalue;

	return rxoffset;
}

static void SetOffsetTx(unsigned char txindex,unsigned char offset)
{
	WriteReg((0xad + txindex),offset);
}

static void SetOffsetRx(unsigned char rxindex,unsigned char offset)
{  
	WriteReg((0xd6 + rxindex),offset);
}
*/
static void GetRawData(short RawData[TX_NUM_MAX][RX_NUM_MAX])
{
	//unsigned char LineNum = 0;
	unsigned char I2C_wBuffer[3];
	unsigned char *rrawdata = NULL;
	short len = 0, i = 0;	
	short ByteNum = 0;
	int ReCode = 0;
	rrawdata = kmalloc(iTxNum*iRxNum*2, GFP_KERNEL);

	if(WriteReg(0x00,0x40) >= 0)
	{
		if(StartScan() >= 0)
		{	
			I2C_wBuffer[0] = 0x01;
			I2C_wBuffer[1] = 0xaa;
			focal_msleep(10);
			ReCode = focal_I2C_write(I2C_wBuffer, 2);
			I2C_wBuffer[0] = (unsigned char)(0x36);
			focal_msleep(10);
			ReCode = focal_I2C_write(I2C_wBuffer, 1);		

			ByteNum = iTxNum*iRxNum * 2;
			if (ReCode >= 0) {					
				len = ByteNum;

				memset(rrawdata, 0x00, sizeof(char)*ByteNum);
				focal_msleep(10);
				ReCode = focal_I2C_Read(NULL, 0, rrawdata, len);	
				if (ReCode >= 0) 
				{
					for (i = 0; i < (len >> 1); i++) 
					{						
						RawData[i/iRxNum][i%iRxNum] = (short)((unsigned short)(rrawdata[i << 1]) << 8) \
							+ (unsigned short)rrawdata[(i << 1) + 1];
					}
				}
				else 
				{
					FTS_DBG("Get Rawdata failure\n");					
				}					
			}

		}
	}
	kfree(rrawdata);        
}

boolean StartTestTP(void) 
{
	bRawdataTest = true;
	printk("hjptest--->[focal] %s start \n", __func__);
	TestTp();

	return bRawdataTest;
}

static void TestTp(void) {
	int i = 0;//min = 0, max = 0;
	//unsigned char regvalue = 0x00;

	bRawdataTest = true;

	if(WriteReg(0x00, 0x40) < 0) {
		printk("Enter factory failure\n");
		bRawdataTest = false;
		goto Enter_WorkMode;
	}
	else
	{
		printk("Enter factory Successful\n");
	}
	focal_msleep(200);

	iTxNum = GetTxNum();
	focal_msleep(100);
	iRxNum = GetRxNum();
	//zax 20141116 ++++++++++++++
	TX_NUM=iTxNum;
	RX_NUM=iRxNum;
	//zax 20141116 -----------------
	bRawdataTest = FT5X46_TestItem();

Enter_WorkMode:	
	//the end, return work mode
	for (i = 0; i < 3; i++) {
		if (WriteReg(0x00, 0x00) >=0)
			break;
		else {
			focal_msleep(200);
		}
	}
}

boolean FT5X46_TestItem(void)
{
	boolean bTestReuslt = true;

	if(g_TestParam_FT5X46.RawDataTest)
		bTestReuslt = bTestReuslt & TestItem_RawDataTest_FT5X46();
	focal_msleep(1000);
	//zax 20150112++++++++++++++++++++++++++++++++++
	if(g_TestParam_FT5X46.UniformityTest)
		bTestReuslt = bTestReuslt & TestItem_UniformityTest();
//	focal_msleep(1000);
	//zax 20150112--------------------------------------------	
	if(g_TestParam_FT5X46.SCAPCbTest)
		bTestReuslt = bTestReuslt & TestItem_SCapCbTest();
	focal_msleep(1000);
	if(g_TestParam_FT5X46.SCAPRawDataTest)
		bTestReuslt = bTestReuslt & TestItem_SCapRawDataTest();
	
	printk("[focal] %s result = %d  \n", __func__, bTestReuslt);
	focal_msleep(1000);

	/*WeakShortTest for feature use*/
	/*bTestReuslt = bTestReuslt & TestItem_WeakShortTest();*/

	return bTestReuslt;		
}

boolean TestItem_RawDataTest_FT5X46(void)
{
	boolean bTestReuslt = true;
	boolean bUse = false;
	int i = 0, j = 0, count = 0;
	//char buf[1023];
	char *buf = NULL;	
	short min_value, max_value;
	FTS_DBG("hjptest--->==============================Test Item: -----  RawDataTest_FT5X46\n");
	
	buf = kmalloc(1024, GFP_KERNEL);
	memset(buf, 0x00, sizeof(char)*1024);

	//���õ�Ƶ��
	if(g_TestParam_FT5X46.RawDataTest_LowFreq == 1)
	{
		bUse = true;
		FTS_DBG("=========Set Frequecy Low\n");
		WriteReg(0x0a, 0x80);
		FTS_DBG("=========FIR State: ON\n");	
		WriteReg(0xFB, 0x01);
		focal_msleep(10);

		if( iTxNum == 0 || iRxNum == 0)
		{
			bTestReuslt  = false;
			FTS_DBG("Tx: %2d, Rx: %2d  \n", iTxNum, iRxNum);
			goto TEST_END;
		}

		//Read Raw Data
		for(i = 0; i < 2; i++)
		{
			focal_msleep(10);
			StartScan();
		}
		GetRawData(rawdata);
		/////////////////Print RawData Start	
		printk("hjptest--->[FocalTech]dump rawdata\n");
		for(i = 0;i < iTxNum;i++)
		{
/*			printk("\n");*/
			count = 0;
			for(j = 0;j < iRxNum;j++)
			{
				count += sprintf(buf + count, "%5d ", rawdata[i][j]);
/*				printk("%5d  ", rawdata[i][j]);*/
			}
			//printk("[Focal][testdata] TX%d = %s \n", i, buf);
		}
		//printk("\n");
		/////////////////Print RawData End

		for(i = 0;i < iTxNum;i++)
		{
			for(j = 0;j < iRxNum;j++)
			{
				if(0 == g_TestParam_FT5X46.valid_node[i][j])  continue;
				min_value = g_TestParam_FT5X46.RawDataTest_Low_Min_node[i][j];
				max_value = g_TestParam_FT5X46.RawDataTest_Low_Max_node[i][j];

				if(rawdata[i][j] < min_value || rawdata[i][j] > max_value)
				{
					bTestReuslt  = false;
					FTS_DBG("rawdata test failure. min_value=%d max_value=%d rawdata[%d][%d]=%d \n",  min_value, max_value, i+1, j+1, rawdata[i][j]);
				}
			}
		}

		FTS_DBG("hjptest--->=========FIR State: OFF\n");	
		WriteReg(0xFB, 0x00);	
		focal_msleep(10);

		//Read Raw Data
		for(i = 0; i < 2; i++)
		{
			focal_msleep(10);
			StartScan();
		}
		GetRawData(rawdata);

		/*Print RawData Start*/
		for(i = 0;i < iTxNum;i++)
		{
/*			printk("\n");*/
			count = 0;
			for(j = 0;j < iRxNum;j++)
			{
				count += sprintf(buf + count, "%5d ", rawdata[i][j]);
/*				printk("%5d  ", rawdata[i][j]);*/
			}
		//	printk("[Focal][testdata] TX%d = %s \n", i, buf);
		}
	//	printk("\n");
		/*Print RawData End*/

		for(i = 0;i < iTxNum;i++)
		{
			for(j = 0;j < iRxNum;j++)
			{
				if(0 == g_TestParam_FT5X46.valid_node[i][j])  continue;

				min_value = g_TestParam_FT5X46.RawDataTest_Low_Min_node[i][j];
				max_value = g_TestParam_FT5X46.RawDataTest_Low_Max_node[i][j];

				if(rawdata[i][j] < min_value || rawdata[i][j] > max_value)
				{
					bTestReuslt  = false;
					FTS_DBG("rawdata test failure. min_value=%d max_value=%d rawdata[%d][%d]=%d \n", \
						min_value, max_value, i+1, j+1, rawdata[i][j]);
				}
			}
		}	

	}

	//���ø�Ƶ��
	if(g_TestParam_FT5X46.RawDataTest_HighFreq== 1)
	{
		bUse = true;
		FTS_DBG("hjptest--->=========Set Frequecy High\n");
		WriteReg(0x0a, 0x81);
		FTS_DBG("hjptest--->=========FIR State: ON\n");	
		WriteReg(0xFB, 0x01);
		focal_msleep(10);

		if( iTxNum == 0 || iRxNum == 0)
		{
			bTestReuslt  = false;
			FTS_DBG("Tx: %2d, Rx: %2d  \n", iTxNum, iRxNum);
			goto TEST_END;
		}

		//Read Raw Data
		for(i = 0; i < 2; i++)
		{
			focal_msleep(10);
			StartScan();
		}
		GetRawData(rawdata);

		/////////////////Print RawData Start	
		for(i = 0;i < iTxNum;i++)
		{
/*			printk("\n");*/
			count = 0;
			for(j = 0;j < iRxNum;j++)
			{
				count += sprintf(buf + count, "%5d ", rawdata[i][j]);
/*				printk("%5d  ", rawdata[i][j]);*/
			}
		//	printk("[Focal][testdata] TX%d = %s \n", i, buf);
		}
	//	printk("\n");
		/////////////////Print RawData End

		for(i = 0;i < iTxNum;i++)
		{
			for(j = 0;j < iRxNum;j++)
			{
				if(0 == g_TestParam_FT5X46.valid_node[i][j])  continue;

				min_value = g_TestParam_FT5X46.RawDataTest_High_Min_node[i][j];
				max_value = g_TestParam_FT5X46.RawDataTest_High_Max_node[i][j];
				//jinpeng_he add comment here failed here1 
				if(rawdata[i][j] < min_value || rawdata[i][j] > max_value)
				{
					bTestReuslt  = false;
					FTS_DBG("rawdata test failure. min_value=%d max_value=%d rawdata[%d][%d]=%d \n", \
						min_value, max_value, i+1, j+1, rawdata[i][j]);
				}
			}
		}

		FTS_DBG("hjptest--->=========FIR State: OFF\n");	
		WriteReg(0xFB, 0x00);	
		focal_msleep(10);

		//Read Raw Data
		for(i = 0; i < 2; i++)
		{
			focal_msleep(10);
			StartScan();
		}
		GetRawData(rawdata);

		/*zax 20141116+++++++++++++++++++++++*/
		for(i = 0;i < iTxNum;i++)
		{
/*			printk("\n");*/
			count = 0;
			for(j = 0;j < iRxNum;j++)
			{
				count += sprintf(buf + count, "%5d ", rawdata[i][j]);
/*				printk("%5d  ", rawdata[i][j]);*/
				Save_rawData[i][j]=rawdata[i][j];
			}
		//	printk("[Focal][testdata] TX%d = %s \n", i, buf);
		}
	//	printk("\n");
		/*zax 20141116------------------------------*/

		for(i = 0;i < iTxNum;i++)
		{
			for(j = 0;j < iRxNum;j++)
			{
				if(0 == g_TestParam_FT5X46.valid_node[i][j])  continue;

				min_value = g_TestParam_FT5X46.RawDataTest_High_Min_node[i][j];
				max_value = g_TestParam_FT5X46.RawDataTest_High_Max_node[i][j];
				//jinpeng_he add comment here failed here12
				if(rawdata[i][j] < min_value || rawdata[i][j] > max_value)
				{
					bTestReuslt  = false;
					FTS_DBG("rawdata test failure. min_value=%d max_value=%d rawdata[%d][%d]=%d \n", \
						min_value, max_value, i+1, j+1, rawdata[i][j]);
				}
			}
		}	

	}
          
TEST_END:
           kfree(buf);       
	if(bUse)
	{
		if( bTestReuslt)
		{
			printk("hjptest--->RawData Test is OK.\n");
		}
		else
		{
			printk("hjptest--->RawData Test is NG.\n");
		}
	}
      
	return bTestReuslt;
}

/*==================================================================================

΢��·����FW������ؼĴ������������£�
ShortTestEn��0x07����     ΢��·����ʹ�ܼĴ���
ValLBuf0 ��0xF4����     ValL���ݼĴ���0
ValLBuf0 ��0xF5����     ValL���ݼĴ���1

΢��·��������ʱHost�����������
1. ΢��·����ʹ��
Host:  W 07 01          // ��������ʹ�ܺ�΢��·����һ�Σ�Ȼ�����»ָ�������ģʽ

2. �������ݶ�ȡ
Host:  W F4  R FF     // ���Զ̺ͻ��̲��Ե�����ValL���ݶ�����GroundValL[64]��MutualValL[64] 
3. ����˵����

Offset:  0x00~0x01

Ground_ShortTest:
�ض�·У׼��2   Byte          
�ض�·���ݣ�(TxNum+RxNum)*2   Byte  // Tx��ǰ��Rx�ں�

Mutual_ShortTest:
����·У׼��2  Byte
����·���ݣ�(TxNum+RxNum)*2   Byte  // Tx��ǰ��Rx�ں�
==================================================================================*/
boolean TestItem_WeakShortTest(void)
{
//#if 0
	//kernel_fpu_begin();	
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;
	boolean bRet = true;
	int i;
	int iAllAdcDataNum = 63;	
	int iMaxTx = 35;
	unsigned char iTxNum, iRxNum, iChannelNum;
	int iClbData_Ground, iClbData_Mutual, iOffset;
	int *iAdcData = NULL;
	/*int iAdcData[sizeof(int)*iAllAdcDataNum];*/
	int fKcal = 0;
	//int fMShortResistance[80*2], fGShortResistance[80*2];
	int* fMShortResistance=NULL;
	int* fGShortResistance=NULL;
	//float fKcal = 0;
	//float fMShortResistance[80*2], fGShortResistance[80*2];
	int iDoffset = 0, iDsen = 0, iDrefn = 0, iMaxD = 0;
	int iMin_CG = g_TestParam_FT5X46.WeakShortTest_CG_Min;
	int iCount = 0;
	int iMin_CC = g_TestParam_FT5X46.WeakShortTest_CC_Min;

	fMShortResistance = kmalloc(160, GFP_KERNEL);
	memset(fMShortResistance, 0x00, sizeof(int)*160);

	fGShortResistance = kmalloc(160, GFP_KERNEL);
	memset(fGShortResistance, 0x00, sizeof(int)*160);

	FTS_DBG("==============================Test Item: -----  Weak Short-Circuit Test \n");
	//TxNum�Ĵ�����0x02(Read Only)
	//RxNum�Ĵ�����0x03(Read Only)	

	focal_msleep(100);
	ReCode = ReadReg(0x02, &iTxNum);//Get Tx
	focal_msleep(100);

	ReCode = ReadReg(0x03, &iRxNum);//Get Rx

	if(ReCode < ERROR_CODE_OK)
	{
		FTS_DBG("Read Register Error!\n");
		bRet = false;
		goto TEST_END;
	}

	iChannelNum = iTxNum + iRxNum;
	iMaxTx = iTxNum;
	iAllAdcDataNum = 1 + (1 + iTxNum + iRxNum)*2;//��ͨ���� + �Ե�У׼���� + ͨ����У׼���� + Offset

	iAdcData = kmalloc(sizeof(int)*iAllAdcDataNum, GFP_KERNEL);// = new int[iAllAdcDataNum];
	memset(iAdcData, 0, sizeof(int)*iAllAdcDataNum);
	for(i = 0; i < 2; i++)
	{
		ReCode = WeakShort_GetAdcData( iAllAdcDataNum*2, iAdcData );
		//	SysDelay(50);
		if(ReCode < ERROR_CODE_OK)
		{
			bRet = false;
			goto TEST_END;
		}
	}

	iOffset = iAdcData[0];
	iClbData_Ground = iAdcData[1];
	iClbData_Mutual = iAdcData[2 + iChannelNum];

	printk("Offset: %4d,	\n", iAdcData[0]);

	printk("\nGround Data:\n");
	for(i = 0; i <= iChannelNum; i++)
	{
		if(i == 0)
			printk("ClbData_Ground: %4d", iAdcData[i + 1]);			
		else if(i <= iMaxTx)
			printk("Tx%02d: %4d, ",i, iAdcData[i+1]);
		else
			printk("Rx%02d: %4d, ", i - iMaxTx, iAdcData[i+1]);

		if(i % 5 == 0)
			printk("\n");
	}

	printk("\nChannel Data:\n");	
	for(i = 0; i <= iChannelNum; i++)
	{	
		if(i == 0)
			printk("ClbData_Mutual: %4d", iAdcData[i + 2 + iChannelNum]);			
		else if(i <= iMaxTx)
			printk("Tx%02d: %4d, ", i, iAdcData[i + 2 + iChannelNum]);
		else
			printk("Rx%02d: %4d, ", i - iMaxTx, iAdcData[i + 2 + iChannelNum]);

		if(i % 5 == 0)
			printk("\n");

	}

	//FTS_DBG("CC: %d, CG: %d, Tx: %d, Rx: %d\n", iMin_CC, iMin_CG, iTxNum, iRxNum);

	/////////////////////////////////////////////�Ե�����
	//::GetPrivateProfileString("BaseSet","Weak_Short_Min","992", str.GetBuffer(MAX_PATH),MAX_PATH,g_strIniFile);
	//int iMin_CG = atoi(str);

	iDoffset = iOffset - 1024;
	iDrefn = iClbData_Ground;

	fKcal = 1;
	//strAdc = "\r\n\r\nShort Circuit (Channel and Ground):\r\n";
	//str.Format("\r\nKcal: %.02f\r\n", fKcal);
	//strAdc += str;

	for(i = 0; i < iChannelNum; i++)
	{
		iDsen = iAdcData[i+2];
		if((2047+iDoffset) - iDsen <= 0)//С�ڵ���0��ֱ����PASS
		{
			continue;
		}

		//�����µĹ�ʽ���м���
		/*fGShortResistance[i] = (float)( iDsen - iDoffset + 410 ) * 251 * fKcal / (10*( 2047 + iDoffset - iDsen )) - 3;*/
		fGShortResistance[i] = ( iDsen - iDoffset + 410 ) * 251 * fKcal / (10*( 2047 + iDoffset - iDsen )) - 3;

		if(fGShortResistance[i] < 0) fGShortResistance[i] = 0;
		if((iMin_CG > fGShortResistance[i]) || (iDsen - iDoffset < 0))//С�ڵ���0��ֱ����0ŷ��·
		{
			if(iCount == 0)
				FTS_DBG("Short Circuit (Channel and Ground):\n");	

			iCount++;
			if(i+1 <= iMaxTx)
				FTS_DBG("Tx%02d: %d(kOhm),\n", i+1, fGShortResistance[i]);
			else
				FTS_DBG("Rx%02d: %d(kOhm),\n", i+1 - iMaxTx, fGShortResistance[i]);
		}
	}
	if(iCount > 0)
	{
		//TestResultInfo(strAdc);
		bRet = false;
	}

	/////////////////////////////////////////////ͨ��������
	//::GetPrivateProfileString("BaseSet","Weak_Short_Min_CC","992", str.GetBuffer(MAX_PATH),MAX_PATH,g_strIniFile);
	//int iMin_CC = atoi(str);

	iDoffset = iOffset - 1024;
	iDrefn = iClbData_Mutual;	
	fKcal = 1.0;
	//strAdc = "\r\n\r\nShort Circuit (Channel and Channel):\r\n";
	iCount = 0;
	for(i = 0; i < iChannelNum; i++)
	{
		iDsen = iAdcData[i+iChannelNum + 3];
		iMaxD = iDrefn;
		if(iMaxD<(iDoffset + 116)) iMaxD = iDoffset + 116;
		//if(iDsen - iDrefn <= 0)  continue;
		if(iDsen - iMaxD<= 0)  continue;

		//�����µĹ�ʽ
		/*fMShortResistance[i] = ( (float)1931 / ( iDsen - iDoffset - 116 ) * 16  - 19 ) *  fKcal - 6;*/
		//fMShortResistance[i] = ( (1931* 16) / ( iDsen - iDoffset - 116 )   - 19 ) *  fKcal - 6;//last fomula
		fMShortResistance[i] = ( ((2047 + iDoffset - iMaxD) / ( iDsen - iMaxD) ) *24 - 27 ) *  fKcal - 6;
		//if(fMShortResistance[i] < 0)continue;
		if(fMShortResistance[i] < 0 && fMShortResistance[i] >= -240 ) fMShortResistance[i] = 0;
		else if( fMShortResistance[i] < -240 )  continue;

		if( fMShortResistance[i] <= 0  || fMShortResistance[i] < iMin_CC )
		{
			if(iCount == 0)
				FTS_DBG("\nShort Circuit (Channel and Channel):\n");

			iCount++;
			if(i+1 <= iMaxTx)
				FTS_DBG("Tx%02d: %d(kOhm),\n", i+1, fMShortResistance[i]);
			else
				FTS_DBG("Rx%02d: %d(kOhm),\n", i+1 - iMaxTx, fMShortResistance[i]);

		}
	}
	if(iCount > 0)
	{
		//TestResultInfo(strAdc);
		bRet = false;
	}

TEST_END:
	if(ReCode < ERROR_CODE_OK) bRet = false;

	if( bRet )
	{
		FTS_DBG("//Weak Short Test is OK.\n");
	}
	else
	{
		FTS_DBG("//Weak Short Test is NG.\n");
	}
       kfree(fMShortResistance);
	kfree(fGShortResistance);
	kfree(iAdcData);

	return bRet;
	//kernel_fpu_end();
	//return true;
//#endif
      //  return true;
}
/*==================================================================================

΢��·����FW������ؼĴ������������£�
ShortTestEn��0x07����     ΢��·����ʹ�ܼĴ���
ValLBuf0 ��0xF4����     ValL���ݼĴ���0
ValLBuf0 ��0xF5����     ValL���ݼĴ���1

΢��·��������ʱHost�����������
1. ΢��·����ʹ��
Host:  W 07 01          // ��������ʹ�ܺ�΢��·����һ�Σ�Ȼ�����»ָ�������ģʽ

2. �������ݶ�ȡ
Host:  W F4  R FF     // ���Զ̺ͻ��̲��Ե�����ValL���ݶ�����GroundValL[64]��MutualValL[64] 
3. ����˵����

Offset:  0x00~0x01

Ground_ShortTest:
�ض�·У׼��2   Byte          
�ض�·���ݣ�(TxNum+RxNum)*2   Byte  // Tx��ǰ��Rx�ں�

Mutual_ShortTest:
����·У׼��2  Byte
����·���ݣ�(TxNum+RxNum)*2   Byte  // Tx��ǰ��Rx�ں�

==================================================================================*/
int WeakShort_GetAdcData( int AllAdcDataLen, int *pRevBuffer )
{
	int i = 0;
	unsigned char ReCode = ERROR_CODE_OK;
	int iReadDataLen = AllAdcDataLen;//Offset*2 + (ClbData + TxNum + RxNum)*2*2
	unsigned char *pDataSend = NULL;	
	/*unsigned char pDataSend[iReadDataLen+1];*/
	unsigned char Data = 0xff;

	pDataSend =  kmalloc(iReadDataLen+1, GFP_KERNEL);
	if(pDataSend == NULL)	return 0x0d;


	ReCode = WriteReg(0x07, 0x01);// ��������ʹ�ܺ�΢��·����һ��
	if(ReCode < ERROR_CODE_OK)return ReCode;

	focal_msleep(200);

	for(i = 0; i < 40; i++)//׼�������ݺ�FW��0x07�Ĵ�����Ϊ0
	{
		focal_msleep(50);//SysDelay(5);
		ReCode = ReadReg(0x07, &Data);
		if(ReCode >= ERROR_CODE_OK)
		{
			if(Data == 0)break;
		}
	}

	if(Data != 0)//if Data != 0, no ready
	{
		FTS_DBG("//Check Data: %d.\n", Data);
		return -1;
	}

	pDataSend[0] = 0xF4;
	//ReCode = HY_IIC_IO(hDevice, pDataSend, 1, pDataSend + 1, iReadDataLen);
	//focal_I2C_write(pDataSend, 1);	
	ReCode = focal_I2C_Read(pDataSend, 1, pDataSend + 1, iReadDataLen);

	if(ReCode >= ERROR_CODE_OK)
	{
		for(i = 0; i < iReadDataLen/2; i++)
		{
			pRevBuffer[i] = (pDataSend[1 + 2*i]<<8) + pDataSend[1 + 2*i + 1];
			//printk("Ch%02d: %02d, \n", i+1, pRevBuffer[i]);
		}
	}

	kfree(pDataSend);

	return ReCode;
}
/*===========================================================
//FreqType: 1:HighFreq, 0: LowFreq; FirState: 1: On, 0: Off;

============================================================*/
/*boolean GetData_RawDataTest(short **TestData, int * iTxNumber, int * iRxNumber, unsigned char  FreqType, unsigned char FirState)
{
	int i = 0, j = 0;
	int iRet = 0;
	int iMemCount=0;
	unsigned char regvalue = 0x00;

	//Enter Factory Mode
	if(WriteReg(0x00, 0x40) < 0) {
		FTS_DBG("Enter factory failure\n");
		return false;
	}
	else
	{
		FTS_DBG("Enter factory Successful\n");
	}
	focal_msleep(50);

	//Get Tx and Rx
	iTxNum = GetTxNum();	
	iRxNum = GetRxNum();	

	iMemCount=sizeof(short)*iRxNum*iTxNum;
	//printk("iMemCount %5d  \n", iMemCount);
	TestData = (short *)kmalloc(iMemCount, GFP_KERNEL);
	*TestData = (short *)malloc(iMemCount);
	short TestData_temp[iMemCount];
	*TestData = TestData_temp;
	printk("TestData Addr: 0x%5X Num: %5d\n", (unsigned int)*TestData, iMemCount);
	//Write Freq Type
	if(0 <= FreqType && FreqType <= 1)
	{
		if(1 == FreqType)	
			iRet = WriteReg(0x0a, 0x81);
		else
			iRet = WriteReg(0x0a, 0x80);	
		if(iRet < 0)return false;
		focal_msleep(10);
	}

	//Write FIR State
	if(0 <= FirState && FirState <= 1)
	{	
		if(1 == FirState)		
			iRet = WriteReg(0xFB, 0x01);
		else
			iRet = WriteReg(0xFB, 0x00);		
		if(iRet < 0)return false;
		focal_msleep(10);
	}

	//Read Raw Data
	for(i = 0; i < 2; i++)
	{
		//GetRawData(rawdata);
		//focal_msleep(10);
		StartScan();
	}
	GetRawData(rawdata);

	/////////////////Print RawData Start	
	for(i = 0;i < iTxNum;i++)
	{
	printk("\n");
	for(j = 0;j < iRxNum;j++)
	{
	printk("%5d  ", rawdata[i][j]);

	}
	}
	printk("\n");
	/////////////////Print RawData End

	printk("Return1 TX:%5d, Return1 RX:%5d  \n", iTxNum, iRxNum);

	if(iRet < 0)
		return false;
	else
	{
		for(i = 0;i < iTxNum;i++)
		{
			//printk("\n");
			for(j = 0;j < iRxNum;j++)
			{
				*((*TestData) + i*iRxNum + j) = rawdata[i][j];
				//printk("%5d  ", *((*TestData) + i*iRxNum + j));
			}
		}

		if(0==iTxNum || 0 ==iRxNum)
		{
			printk("Return2 TX:%5d, Return2 RX:%5d  \n", iTxNum, iRxNum);
			//Get Tx and Rx
			iRxNum = GetRxNum();
			//focal_msleep(10);
			iTxNum = GetTxNum();
			//focal_msleep(10);
		}
		if(0==iTxNum || 0 ==iRxNum)
		{
			ReadReg(0x02,&regvalue);

			printk("Return2 TX:%5d, Return2 RX:%5d  \n", iTxNum, iRxNum);
			//Get Tx and Rx
			ReadReg(0x03,&regvalue);
			iRxNum = regvalue;
			focal_msleep(10);
			ReadReg(0x02,&regvalue);
			iTxNum = regvalue;
			focal_msleep(10);                        
		}

		*iTxNumber = iTxNum;		
		*iRxNumber = iRxNum;
		//printk("Return3 iTX:%5d, Return3 iRX:%5d  \n", iTxNum, iRxNum);
		printk("Return3 TX:%5d, Return3 RX:%5d  \n", *iTxNumber, *iRxNumber);

		return true;
	}

}*/
boolean NeedProofOnTest(unsigned char chVal)
{
        return !( chVal & 0x20 );
}
boolean NeedProofOffTest(unsigned char chVal)
{
       return !( chVal & 0x80 );
}
bool NeedTxOnVal(unsigned char chVal)
{
       return !( chVal & 0x40 ) || !( chVal & 0x04 );
}
bool NeedRxOnVal(unsigned char chVal)
{
       return !( chVal & 0x40 ) || ( chVal & 0x04 );
}
bool NeedTxOffVal(unsigned char chVal)
{
       return 0x00 == (chVal & 0x03) || 0x10 == ( chVal & 0x03 );
}
bool NeedRxOffVal(unsigned char chVal)
{ 
       return 0x01 == (chVal & 0x03) || 0x10 == ( chVal & 0x03 );
}
unsigned char GetChannelNumNoMapping(void)
{
	//CString strTestResult, str, strTemp;
	unsigned char ReCode;

	unsigned char rBuffer[1]; //= new unsigned char;


	printk("Get Tx Num...\n");
	ReCode =ReadReg( 0x55,  rBuffer);
	//if(ReCode == ERROR_CODE_OK)
	if(ReCode >= ERROR_CODE_OK)
	{
		SCap_iTxNum = rBuffer[0];	
	}
	else
	{
		printk("Failed to get Tx number\n");
	}
	
	printk("Get Rx Num...\n");
	ReCode = ReadReg( 0x56,  rBuffer);
	//if(ReCode == ERROR_CODE_OK)
	if(ReCode >= ERROR_CODE_OK)
	{
		SCap_iRxNum = rBuffer[0];
	}
	else
	{
		printk("Failed to get Rx number\n");
	}

	return ReCode;
}
boolean SwitchToNoMapping(void)
{
	unsigned char chPattern = -1;
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char RegData = -1;
	ReCode = ReadReg( 0x53, &chPattern );

	printk("zax1 Switch To NoMapping ReCode %d\n",ReCode);

	printk("zax1 Switch To NoMapping chPattern %d\n",chPattern);
	if(1 == chPattern)
	{
		RegData = -1;
		ReCode =ReadReg( 0x54, &RegData );
		printk("zax2 Switch To NoMapping RegData %d    ReCode   %d\n",RegData,ReCode);
		if( 1 != RegData ) 
		{
			ReCode = WriteReg( 0x54, 1 );  //0-mapping 1-no mampping
			printk("zax2 Switch To NoMapping   ReCode   %d\n",ReCode);
			focal_msleep(20);
			GetChannelNumNoMapping();
		}
	}
	else
	{
		SCap_iTxNum=iTxNum;
		SCap_iRxNum=iRxNum;

	}
	//if( ReCode != ERROR_CODE_OK )
	if(ReCode < ERROR_CODE_OK)
	{
		printk("Switch To NoMapping Failed!\n");
		return false;
	}
	return true;
}
unsigned char GetTxSC_CB(unsigned char index, unsigned char *pcbValue)
{
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char wBuffer[4];
	if (index < 128) {	/*read one massage*/
		*pcbValue = 0;
		WriteReg(REG_ScCbAddrR, index);
		ReCode = ReadReg(REG_ScCbBuf0, pcbValue);
	} else {	/*read index - 128 massages*/
		WriteReg(REG_ScCbAddrR, 0);
		wBuffer[0] = REG_ScCbBuf0;
			ReCode = focal_I2C_Read(wBuffer, 1, pcbValue, index-128);
	}	

	return ReCode;
}

boolean TestItem_SCapCbTest()
{
	//CString str, strTemp;
	int i, index, Value, CBMin, CBMax;
	boolean bFlag = true;
	unsigned char ReCode;
	boolean btmpresult = true;
	int iMax, iMin, iAvg;
	unsigned char pReadData[300] = {0};
	unsigned char I2C_wBuffer[1];
	unsigned char regAddr = 0x09, regData = 0;

	FTS_DBG("hjptest--->==============================Test Item: -----  TestItem_SCapCbTest_FT5X46\n");

	ReCode = ReadReg( regAddr, &regData );
	printk("hjptest--->[Focal] %s : Water_Channel_Select ReCode = %d regData = %d \n", __func__, ReCode, regData);
	

	btmpresult = SwitchToNoMapping();
	//printk("zax StartScan btmpresult %d   \n",btmpresult);
	if( !btmpresult ) goto TEST_END;

	ReCode = StartScan();
	//printk("zax StartScan ReCode %d   \n",ReCode);
	if(ReCode >= ERROR_CODE_OK)
	{			
		for(i = 0; i < 1; i++)
		{
			memset(SCap_rawData, 0, sizeof(SCap_rawData));
			memset(pReadData, 0, sizeof(pReadData));

			/*water resist mode*/
			I2C_wBuffer[0] = REG_ScCbBuf0;
			ReCode = WriteReg( 0x44, 1 );	/*choose SCap working mode : 1 -> water resist 0 -> non water resist*/
			StartScan();
			ReCode = WriteReg( REG_ScCbAddrR, 0 );
			
			ReCode = GetTxSC_CB( SCap_iTxNum + SCap_iRxNum + 128, pReadData);
			
			for (index = 0; index < SCap_iRxNum; ++index) {
				SCap_rawData[0 + SCap_iTxNum][index]= pReadData[index];
			//	printk("zax SCap_rawData0  %d   \n", pReadData[index]);
			}

			for (index = 0; index < SCap_iTxNum; ++index) {
				SCap_rawData[1 + SCap_iTxNum][index] = pReadData[index + SCap_iRxNum];
			//	printk("zax SCap_rawData1  %d   \n", pReadData[index + SCap_iRxNum]);
			}

			/*water resist mode*/
			I2C_wBuffer[0] = REG_ScCbBuf0;
			ReCode = WriteReg( 0x44, 0 );	/*choose SCap working mode : 1 -> water resist 0 -> non water resist*/
			StartScan();
			ReCode = WriteReg( REG_ScCbAddrR, 0 );

			ReCode = GetTxSC_CB( SCap_iRxNum + SCap_iTxNum + 128, pReadData);
			for (index = 0; index < SCap_iRxNum; ++index) {
				SCap_rawData[2 + SCap_iTxNum][index]= pReadData[index];
			//	printk("zax SCap_rawData2  %d   \n", pReadData[index]);
			}

			for (index = 0; index < SCap_iTxNum; ++index) {
				SCap_rawData[3 + SCap_iTxNum][index] = pReadData[index + SCap_iRxNum];
			//	printk("zax SCap_rawData3  %d   \n", pReadData[index + SCap_iRxNum]);
			}

			if (ReCode < ERROR_CODE_OK) {
				printk("hjptest--->Failed to Get SCap CB!\n");
			}		
		}
	}
	else
	{
		printk("hjptest--->Failed to Scan SCap CB!!\n");
	}

	/*check if the data out of spec*/

	if (ReCode >= ERROR_CODE_OK) {	
		bFlag = NeedProofOnTest(regData);
		printk("hjptest--->Need Proof on Test %d	%d\n", bFlag ,g_TestParam_FT5X46.SCapCbTest_SetWaterproof_ON);
		if (g_TestParam_FT5X46.SCapCbTest_SetWaterproof_ON && bFlag) {
			CBMin = g_TestParam_FT5X46.SCapCbTest_ON_Min_Value;
			CBMax = g_TestParam_FT5X46.SCapCbTest_ON_Max_Value;
	
			iMax = -SCap_rawData[0+SCap_iTxNum][0];
			iMin = 2 * SCap_rawData[0+SCap_iTxNum][0];
			iAvg = 0;
			Value = 0;

			printk("hjptest--->WaterProof On Mode:  \n");
			printk("hjptest--->SCap CB_Rx:  \n");
			bFlag = NeedRxOnVal(regData);
			SCab_1=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for( i = 0;bFlag && i < SCap_iRxNum; i++ ) {
				if( g_TestParam_FT5X46.InvalidNodes[0][i] == 0 )
					continue;

				Value = SCap_rawData[0+SCap_iTxNum][i];
				Save_rawData[4+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				iAvg += Value;
				CBMin = g_TestParam_FT5X46.SCapCbTest_ON_Min[0][i];
				CBMax = g_TestParam_FT5X46.SCapCbTest_ON_Max[0][i];
			//	printk("node_%d Value = %d CBMax=%d CBMin=%d \n", i, Value, CBMax, CBMin);
				if(iMax < Value)
					iMax = Value;
				if(iMin > Value)
					iMin = Value;
				if(Value > CBMax || Value < CBMin)
					btmpresult = false;
			}

			printk("hjptest--->SCap CB_Tx:  \n");
			bFlag = NeedTxOnVal(regData);
			SCab_2=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for(i = 0;bFlag &&  i < SCap_iTxNum; i++) {

				if( g_TestParam_FT5X46.InvalidNodes[1][i] == 0 )
					continue;

				Value = SCap_rawData[1+SCap_iTxNum][i];
				Save_rawData[5+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				CBMin = g_TestParam_FT5X46.SCapCbTest_ON_Min[1][i];
				CBMax = g_TestParam_FT5X46.SCapCbTest_ON_Max[1][i];
			//	printk("node_%d Value = %d CBMax=%d CBMin=%d \n", i, Value, CBMax, CBMin);
				iAvg += Value;
				if(iMax < Value)
					iMax = Value;
				if(iMin > Value)
					iMin = Value;
				if(Value > CBMax || Value < CBMin)
					btmpresult = false;
			}

			iAvg = iAvg/(SCap_iTxNum + SCap_iRxNum);

			printk("hjptest--->Max SCap CB: %d, Min SCap CB: %d, Deviation Value: %d, Average Value: %d\n", iMax, iMin, iMax - iMin, iAvg);

		}
		
		bFlag = NeedProofOffTest(regData);
		printk("hjptest--->Need Proof off Test %d	%d\n", bFlag ,g_TestParam_FT5X46.SCapCbTest_SetWaterproof_OFF);
		
		if (g_TestParam_FT5X46.SCapCbTest_SetWaterproof_OFF && bFlag) {
			CBMin = g_TestParam_FT5X46.SCapCbTest_OFF_Min_Value;
			CBMax = g_TestParam_FT5X46.SCapCbTest_OFF_Max_Value;

			iMax = -SCap_rawData[2+SCap_iTxNum][0];
			iMin = 2 * SCap_rawData[2+SCap_iTxNum][0];
			iAvg = 0;
			Value = 0;

			printk("hjptest--->WaterProof Off Mode:  \n");
			printk("hjptest--->SCap CB_Rx:  \n");
			bFlag = NeedRxOffVal(regData);
			SCab_3=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for (i = 0; bFlag && (i < SCap_iRxNum); i++) {
				if( g_TestParam_FT5X46.InvalidNodes[0][i] == 0 )
					continue;

				Value = SCap_rawData[2+SCap_iTxNum][i];
				Save_rawData[6+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				iAvg += Value;

				CBMin = g_TestParam_FT5X46.SCapCbTest_OFF_Min[0][i];
				CBMax = g_TestParam_FT5X46.SCapCbTest_OFF_Max[0][i];
			//	printk("node_%d Value = %d CBMax=%d CBMin=%d \n", i, Value, CBMax, CBMin);
				if(iMax < Value)
					iMax = Value;
				if(iMin > Value)
					iMin = Value;
				if( Value > CBMax || Value < CBMin )
					btmpresult = false;
			}

			printk("hjptest--->SCap CB_Tx:  \n");
			bFlag = NeedTxOffVal(regData);
			SCab_4=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for(i = 0; bFlag && (i < SCap_iTxNum); i++)
			{
				if( g_TestParam_FT5X46.InvalidNodes[1][i] == 0 )
					continue;

				Value = SCap_rawData[3+SCap_iTxNum][i];
				Save_rawData[7+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/

				CBMin = g_TestParam_FT5X46.SCapCbTest_OFF_Min[1][i];
				CBMax = g_TestParam_FT5X46.SCapCbTest_OFF_Max[1][i];
			//	printk("node_%d Value = %d CBMax=%d CBMin=%d \n", i, Value, CBMax, CBMin);
				iAvg += Value;
				if(iMax < Value)
					iMax = Value;
				if(iMin > Value)
					iMin = Value;
				if(Value > CBMax || Value < CBMin)
					btmpresult = false;
			}

			iAvg = iAvg/(SCap_iTxNum + SCap_iRxNum);
			printk("Max SCap CB: %d, Min SCap CB: %d, Deviation Value: %d, Average Value: %d\n", iMax, iMin, iMax - iMin, iAvg);

		}
	} else {
		btmpresult = false;
	}

TEST_END:
	if (btmpresult) {
		printk("hjptest--->SCap CB Test is OK\n");
	} else {
		printk("hjptest--->SCap CB Test is NG\n");
	}
	return btmpresult;
}

unsigned char ReadRawData(unsigned char Freq, unsigned char LineNum, int ByteNum, int *pRevBuffer)
{
	unsigned char ReCode=ERROR_CODE_COMM_ERROR;
	unsigned char I2C_wBuffer[3];
	//unsigned char *pReadData = new unsigned char[ByteNum];
	//unsigned char *pReadDataTmp = new unsigned char[ByteNum*2];
	int i,iReadNum;
	unsigned char *pReadData = NULL;
	unsigned short BytesNumInTestMode1=0;
	
	pReadData =kmalloc(ByteNum, GFP_KERNEL);

	//if(hDevice == NULL)		return ERROR_CODE_NO_DEVICE;
	if(pReadData == NULL)		return ERROR_CODE_ALLOCATE_BUFFER_ERROR;

	iReadNum=ByteNum/342;

	if(0 != (ByteNum%342)) 
		iReadNum++;

	if(ByteNum <= 342)
	{
		BytesNumInTestMode1 = ByteNum;		
	}
	else
	{
		BytesNumInTestMode1 = 342;
	}
	
	ReCode = WriteReg(REG_LINE_NUM, LineNum);//Set row addr;

	//printk("zax ReadRawData1 %d\n",ReCode);
		//***********************************************************Read raw data in test mode1
		

		I2C_wBuffer[0] = REG_RawBuf0;	//set begin address
		//if(ReCode == ERROR_CODE_OK)
		if(ReCode >= ERROR_CODE_OK)
		{
			//ReCode = Comm_Base_IIC_IO(hDevice, I2C_wBuffer, 1, pReadData, BytesNumInTestMode1);
			focal_msleep(10);
			ReCode =focal_I2C_Read(I2C_wBuffer, 1, pReadData, BytesNumInTestMode1);
			
		}
		//printk("zax ReadRawData1 %d\n",ReCode);
		//printk("zax ReadRawData1 iReadNum  %d\n",iReadNum);
		for(i=1; i<iReadNum; i++)
		{//printk("zax ReadRawData  conti\n");
			//if(ReCode != ERROR_CODE_OK) 
			if(ReCode < ERROR_CODE_OK) 
			{
				//printk("zax ReadRawData  break\n");
				break;
			}
			if(i==iReadNum-1)//last packet
			{
				//ReCode = Comm_Base_IIC_IO(hDevice, NULL, 0, pReadData+342*i, ByteNum-342*i);
				focal_msleep(10);
				ReCode =focal_I2C_Read(NULL, 0, pReadData+342*i, ByteNum-342*i);
			}
			else
			{
				//ReCode = Comm_Base_IIC_IO(hDevice, NULL, 0, pReadData+342*i, 342);
				focal_msleep(10);
				ReCode =focal_I2C_Read(NULL, 0, pReadData+342*i, 342);
			}

		}
		
	
	//printk("zax final ReadRawData %d\n",ReCode);
	//if(ReCode == ERROR_CODE_OK)
	if(ReCode >= ERROR_CODE_OK)
	{
		for(i=0; i<(ByteNum>>1); i++)
		{
			pRevBuffer[i] = (pReadData[i<<1]<<8)+pReadData[(i<<1)+1];
		//	printk("zaxzax ReadRawData %d\n",pRevBuffer[i]);
			
		}
	}

	//delete []pReadData;
	//delete []pReadDataTmp;
	kfree(pReadData);
	return ReCode;
}
boolean TestItem_SCapRawDataTest()
{

	int i,RawDataMin,RawDataMax,Value;
	boolean bFlag = true;
	unsigned char ReCode;
	boolean btmpresult = true;
	int iMax, iMin, iAvg;
	int* prawData= NULL;
	unsigned char regAddr = 0x09, regData = 0;

	FTS_DBG("hjptest--->==============================Test Item: -----  Scap_RawData_Test_FT5X46\n");
	prawData = kmalloc(300, GFP_KERNEL);
	memset(prawData, 0x00, sizeof(int)*300);

   	 ReCode = ReadReg( regAddr, &regData );
	printk("hjptest--->[Focal] %s : Water_Channel_Select ReCode = %d regData = %d \n", __func__, ReCode, regData);

	btmpresult= SwitchToNoMapping();
	if (!btmpresult)
		goto TEST_END;

	ReCode = StartScan();

	if (ReCode >= ERROR_CODE_OK) {			
		for (i = 0; i < 1; i++) {
			memset(SCap_rawData, 0, sizeof(SCap_rawData));

			//memset(prawData, 0, sizeof(prawData));
 
			
			/*Water resist raw data*/
			//printk("zax TestItem_SCapRawDataTest (SCap_iRxNum + SCap_iTxNum)*2 %d %d 	%d\n",SCap_iTxNum,SCap_iRxNum,(SCap_iRxNum + SCap_iTxNum)*2);
			
			for (i = 0; i < 2; i++)
				ReCode = ReadRawData(0, 0xAC, (SCap_iRxNum + SCap_iTxNum)*2, prawData );
				//printk("zax TestItem_SCapRawDataTest ReadRawData1 %d\n",ReCode);

			memcpy(SCap_rawData[0+SCap_iTxNum], prawData, sizeof(int)*SCap_iRxNum);
			memcpy(SCap_rawData[1+SCap_iTxNum], prawData + SCap_iRxNum, sizeof(int)*SCap_iTxNum);

			/*non Water resist raw data*/
			for (i = 0; i < 2; i++)
				ReCode = ReadRawData(0, 0xAB, (SCap_iRxNum + SCap_iTxNum)*2, prawData );
				//printk("zax TestItem_SCapRawDataTest ReadRawData2 %d\n",ReCode);

			memcpy(SCap_rawData[2+SCap_iTxNum], prawData, sizeof(int)*SCap_iRxNum);
			memcpy(SCap_rawData[3+SCap_iTxNum], prawData + SCap_iRxNum, sizeof(int)*SCap_iTxNum);

			if (ReCode < ERROR_CODE_OK) {
				//str.Format("Error Code: %s",g_ErrorMsg[ReCode]);
				//TestResultInfo("\r\nFailed to Get SCap RawData! " + str);
				printk("hjptest--->Failed to Get SCap RawData!\n");
			}		
		}
	} else {
		//str.Format("Error Code: %s",g_ErrorMsg[ReCode]);
		//TestResultInfo("\r\nFailed to Scan SCap RawData! " + str);

		printk("hjptest--->Failed to Scan SCap RawData! \n");
	}

	/*check if the data out of spec*/

	if (ReCode >= ERROR_CODE_OK) {	
		bFlag=NeedProofOnTest(regData);
		if (g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_ON && bFlag ) {
			RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_ON_Min_Value;
			RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_ON_Max_Value;
			//iMax = iMin = RawData[0+g_ScreenSetParam.iTxNum][0];
			iMax = -SCap_rawData[0+SCap_iTxNum][0];
			iMin = 2 * SCap_rawData[0+SCap_iTxNum][0];
			iAvg = 0;
			Value = 0;

			printk("hjptest--->SCap RawData_Rx:\n");
			bFlag = NeedRxOnVal(regData);
			SCab_5=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for ( i = 0; bFlag && (i < SCap_iRxNum); i++ ) {
				if ( g_TestParam_FT5X46.InvalidNodes[0][i] == 0 )
					continue;

				Value = SCap_rawData[0+SCap_iTxNum][i];
				Save_rawData[0+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				iAvg += Value;
				RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_ON_Min[0][i];
				RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_ON_Max[0][i];
			//	printk("node_%d Value = %d RawDataMax=%d RawDataMin=%d \n", i, Value, RawDataMax, RawDataMin);

				if (iMax < Value)
					iMax = Value;
				if (iMin > Value)
					iMin = Value;
				if (Value > RawDataMax || Value < RawDataMin)
					btmpresult = false;
			}

			printk("hjptest--->SCap RawData_Tx:\n");
			bFlag = NeedTxOnVal(regData);
			SCab_6=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for (i = 0; bFlag && i < SCap_iTxNum; i++) {
				if ( g_TestParam_FT5X46.InvalidNodes[1][i] == 0 )
					continue;

				Value = SCap_rawData[1+SCap_iTxNum][i];
				Save_rawData[1+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_ON_Min[1][i];
				RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_ON_Max[1][i];
			//	printk("node_%d Value = %d RawDataMax=%d RawDataMin=%d \n", i, Value, RawDataMax, RawDataMin);
				iAvg += Value;
				if (iMax < Value)
					iMax = Value;
				if (iMin > Value)
					iMin = Value;
				if (Value > RawDataMax || Value < RawDataMin)
					btmpresult = false;
			}

			iAvg = iAvg/(SCap_iTxNum + SCap_iRxNum);
			printk("hjptest--->Max SCap RawData: %d, Min SCap RawData: %d, Deviation Value: %d, Average Value: %d\n", iMax, iMin, iMax - iMin, iAvg);

		}
		bFlag = NeedProofOffTest(regData);
		if (g_TestParam_FT5X46.SCapRawDataTest_SetWaterproof_OFF && bFlag) {
			RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_OFF_Min_Value;
			RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_OFF_Max_Value;
			iMax = -SCap_rawData[2+SCap_iTxNum][0];
			iMin = 2 * SCap_rawData[2+SCap_iTxNum][0];
			iAvg = 0;
			Value = 0;

			printk("hjptest--->SCap RawData_Rx:\n");
			bFlag = NeedRxOffVal(regData);
			SCab_7=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for (i = 0; bFlag && (i < SCap_iRxNum); i++) {
				if ( g_TestParam_FT5X46.InvalidNodes[0][i] == 0 )
					continue;

				Value = SCap_rawData[2+SCap_iTxNum][i];
				Save_rawData[2+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/
				iAvg += Value;

				RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_OFF_Min[0][i];
				RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_OFF_Max[0][i];
			//	printk("node_%d Value = %d RawDataMax=%d RawDataMin=%d \n", i, Value, RawDataMax, RawDataMin);
				if (iMax < Value)
					iMax = Value;
				if (iMin > Value)
					iMin = Value;
				if (Value > RawDataMax || Value < RawDataMin)
					btmpresult = false;
			}

			printk("hjptest--->SCap RawData_Tx:\n");
			bFlag = NeedTxOffVal(regData);
			SCab_8=bFlag;/*zax 20141116+++++++++++++++++++++++*/
			for (i = 0; bFlag && (i < SCap_iTxNum); i++) {
				if (g_TestParam_FT5X46.InvalidNodes[1][i] == 0)
					continue;

				Value = SCap_rawData[3+SCap_iTxNum][i];
				Save_rawData[3+SCap_iTxNum][i] = Value;	/*zax 20141116+++++++++++++++++++++++*/

				RawDataMin = g_TestParam_FT5X46.SCapRawDataTest_OFF_Min[1][i];
				RawDataMax = g_TestParam_FT5X46.SCapRawDataTest_OFF_Max[1][i];
			//	printk("node_%d Value = %d RawDataMax=%d RawDataMin=%d \n", i, Value, RawDataMax, RawDataMin);
				iAvg += Value;
				if (iMax < Value)
					iMax = Value;
				if (iMin > Value)
					iMin = Value;
				if (Value > RawDataMax || Value < RawDataMin)
					btmpresult = false;
			}

			iAvg = iAvg/(SCap_iTxNum + SCap_iRxNum);

			printk("hjptest--->Max SCap RawData: %d, Min SCap RawData: %d, Deviation Value: %d, Average Value: %d\n", iMax, iMin, iMax - iMin, iAvg);

		}
	} else {
		btmpresult = false;
	}
            
TEST_END:
	     kfree(prawData);
	if (btmpresult) {
		printk("hjptest--->SCap RawData Test is OK\n");
	} else {
		printk("hjptest--->SCap RawData Test is NG\n");
	}
	return btmpresult;

}
//zax 20150112++++++++++++++++++++++++++++++++++
bool AnalyzeTestResultMCap_TXX( boolean bIncludeKey)
{
	
	 int iTx, iRx = 0;
	bool bRedHat= true ;


	//printk("20150111  zax21  AnalyzeTestResultMCap_TXX\n" );
    	for (  iTx = 0; iTx < iTxNum; ++iTx )
    	{		
		for (  iRx = 0; iRx < iRxNum; ++iRx )
		{			
            		if( (NODE_INVALID_TYPE == invalide[iTx][iRx])
				|| (!bIncludeKey && (NODE_KEY_TYPE == invalide[iTx][iRx])) )  
			{					
					continue;
			}			
			if( TxLinearity[iTx][iRx] < minHole[iTx][iRx]
			 || TxLinearity[iTx][iRx] > maxHole[iTx][iRx]) 
			{

				
				printk("Tx= %d,Rx=%d,TXX=%d,min=%d,max=%d\n", iTx,iRx,TxLinearity[iTx][iRx], minHole[iTx][iRx], maxHole[iTx][iRx]);
				bRedHat = false;
				
			}
			
		}
		
   	}
	return bRedHat;
}
bool AnalyzeTestResultMCap_RXX( boolean bIncludeKey)
{
	
	int iTx, iRx = 0;
	bool bRedHat= true ;
	//printk("20150111  zax21  AnalyzeTestResultMCap_RXX\n" );
   	for (  iTx = 0; iTx < iTxNum; ++iTx )
    	{
		
		for (  iRx = 0; iRx < iRxNum; ++iRx )
		{
			//printk("20150111  zax20   %d,	%d,	%d,	%d\n",RxLinearity[iTx][iRx] ,minHole[iTx][iRx],maxHole[iTx][iRx], invalide[iTx][iRx]);
            		if( (NODE_INVALID_TYPE == invalide[iTx][iRx])
				|| (!bIncludeKey && (NODE_KEY_TYPE == invalide[iTx][iRx])) )  
			{
					
					//printk("20150111  zax22   iTx=%d,iRx=%d,invalide=%d,bIncludeKey=%d\n", iTx,iRx,invalide[iTx][iRx],bIncludeKey);
					continue;
			}
			
			if( RxLinearity[iTx][iRx] < minHole[iTx][iRx]
			 || RxLinearity[iTx][iRx] > maxHole[iTx][iRx]) 
			{
				printk("Tx= %d,Rx=%d,RXX=%d,min=%d,max=%d\n", iTx,iRx,RxLinearity[iTx][iRx], minHole[iTx][iRx], maxHole[iTx][iRx]);
				bRedHat = false;
				
			}		
		}
	
    	}
	return bRedHat;
}
void ArrayFillWithVal( short iFillVal, int fillNum)
{
	 int iRow,iCol;
	
	if( fillNum < 0 ) 
		fillNum = TX_NUM_MAX * RX_NUM_MAX;
	if( fillNum > TX_NUM_MAX * RX_NUM_MAX ) 
		fillNum = TX_NUM_MAX * RX_NUM_MAX;
	
	for (  iRow = 0; iRow < TX_NUM_MAX; ++iRow )
	{
		for (  iCol = 0; iCol < RX_NUM_MAX; ++iCol )
		{
			maxHole[iRow][iCol]=iFillVal;
		}
	}
}

static boolean TestItem_UniformityTest(void)
{
    	unsigned char ReCode = ERROR_CODE_OK;
	bool btmpresult = true;
	bool bTestResult = true;
	unsigned char strSwitch = -1;
	unsigned char chPattern=-1;
	int iDeviation;
	int iMax,iMin;
	int iUniform;
	
	int iRow,iCol;
	bool bResult;
	boolean bIncludeKey;

	FTS_DBG("hjptest--->==============================Test Item: -----  TestItem_UniformityTest\n");
	//���ж��Ƿ�Ϊv3���壬Ȼ���ȡ0x54��ֵ�����ж����趨��mapping�����Ƿ�һ�£���һ��д������
	//Uniformity test mapping��mappingǰ��0x54=1;mapping��0x54=0;
	ReCode = ReadReg( 0x53, &chPattern );
	printk("hjptest--->[1] ReCode = %d \n", ReCode);

	printk(" hjptest--->test result chPattern = %d and ReCode = %d !\n", chPattern, ReCode);
	//printk("20150111  zax0   %d\n",chPattern);
	if (chPattern == 1)
	{
		m_bV3TP = true;
		printk("hjptest--->m_bV3TP = %d \n", m_bV3TP);
	}else
	{
		m_bV3TP = false;
		printk("hjptest--->m_bV3TP = %d \n", m_bV3TP);
	}
	//printk("20150111  zax1   %d\n",m_bV3TP);
	memset(TxLinearity, 0, sizeof(TxLinearity));
	memset(RxLinearity, 0, sizeof(RxLinearity));
	if (m_bV3TP)
	{
		ReCode = ReadReg( REG_MAPPING_SWITCH, &strSwitch );
		//printk("20150111  zax2   %d\n",strSwitch);
		if (strSwitch != 0)
		{
			ReCode = WriteReg( REG_MAPPING_SWITCH, 0 );
			if( ReCode != ERROR_CODE_OK)	
			{
				
				printk("WriteReg fail \n");
				btmpresult = false;
			}
		}
		printk("[2] ReCode = %d \n", ReCode);
	}

	if( g_TestParam_FT5X46.Uniformity_CheckTx )
	{
		printk("hjptest--->Uniformity_CheckTx ! \n");
		for (  iRow = 0; iRow < iTxNum; ++iRow )
		{
			for (  iCol = 1; iCol < iRxNum; ++iCol )
			{
				//printk("20150111  zax4   %d,	%d,		%d\n",rawdata[iRow][iCol],iTxNum,iRxNum);
				if(rawdata[iRow][iCol] - rawdata[iRow][iCol-1]>=0)
				{
					iDeviation = ( rawdata[iRow][iCol] - rawdata[iRow][iCol-1] );
				}
				else
				{
					iDeviation = ( rawdata[iRow][iCol-1]-rawdata[iRow][iCol]);
				}
				
				if(rawdata[iRow][iCol]>= rawdata[iRow][iCol-1])
				{
					iMax =rawdata[iRow][iCol];
				}
				else
				{
					iMax =rawdata[iRow][iCol-1];
				}
				
				iMax = iMax ? iMax : 1;
				TxLinearity[iRow][iCol] =(short)( 100 * iDeviation / iMax);
				//printk("20150111  zaxzax   %d,	%d,	%d\n",TxLinearity[iRow][iCol] ,iDeviation,iMax);


			}
		}

		{
			
			bIncludeKey=false;
			
			memset( maxHole, 0, sizeof(maxHole) );
			memset( invalide, 0, sizeof(invalide) );
            		

			for (  iRow = 0; iRow < TX_NUM_MAX; ++iRow )
			{
				for (  iCol = 0; iCol < RX_NUM_MAX; ++iCol )
				{
					minHole[iRow][iCol]=0;//-32760;
					
					invalide[iRow][iCol]=g_TestParam_FT5X46.valid_node[iRow][iCol];
					//printk("20150111  zax4   %d,	%d\n",minHole[iRow][iCol],invalide[iRow][iCol]);
				}
			}
			ArrayFillWithVal(  g_TestParam_FT5X46.Uniformity_Tx_Hole,(-1) );

			for(  iRow = 0; iRow < iTxNum; ++iRow )
			{
				for(  iCol = 0; iCol < iRxNum; ++iCol )
					if( 0 == iCol ) invalide[iRow][iCol] = 0;
			}

			bResult = AnalyzeTestResultMCap_TXX(  bIncludeKey );

			

			if( !bResult )
			{
				
				printk("hjptest--->Uniformity_CheckTx fail\n");
				btmpresult = false;
			}
		}
	}
	if( g_TestParam_FT5X46.Uniformity_CheckRx )
	{
		printk("hjptest--->Uniformity_CheckRx ! \n");
		for (  iRow = 1; iRow < iTxNum; ++iRow )
		{
			for (  iCol = 0; iCol < iRxNum; ++iCol )
			{


				
				//printk("20150111  zax13   %d,	%d,		%d\n",rawdata[iRow][iCol],iTxNum,iRxNum);
				if(rawdata[iRow][iCol] - rawdata[iRow-1][iCol]>=0)
				{
					iDeviation = ( rawdata[iRow][iCol] - rawdata[iRow-1][iCol] );
				}
				else
				{
					iDeviation = ( rawdata[iRow-1][iCol]-rawdata[iRow][iCol]);
				}


				if(rawdata[iRow][iCol]>= rawdata[iRow-1][iCol])
				{
					iMax =rawdata[iRow][iCol];
				}
				else
				{
					iMax = rawdata[iRow-1][iCol];
				}
				
				iMax = iMax ? iMax : 1;
				RxLinearity[iRow][iCol] =(short)(  100 * iDeviation / iMax);
				//printk("20150111  zaxzax   %d,	%d,	%d\n",RxLinearity[iRow][iCol] ,iDeviation,iMax);


			}
		}

		{
			
			bIncludeKey=false;
			memset( maxHole, 0, sizeof(maxHole) );
			memset( invalide, 0, sizeof(invalide) );
			
			
			for (  iRow = 0; iRow < TX_NUM_MAX; ++iRow )
			{
				for (  iCol = 0; iCol < RX_NUM_MAX; ++iCol )
				{
					minHole[iRow][iCol]=0;//-32760;
					
					invalide[iRow][iCol]=g_TestParam_FT5X46.valid_node[iRow][iCol];
					//printk("20150111  zax4   %d,	%d\n",minHole[iRow][iCol],invalide[iRow][iCol]);
				}
			}
			ArrayFillWithVal(g_TestParam_FT5X46.Uniformity_Rx_Hole,(-1) );

			for(  iRow = 0; iRow < iTxNum; ++iRow )
			{
				for(  iCol = 0; iCol < iRxNum; ++iCol )
					if( 0 == iRow ) invalide[iRow][iCol] = 0;
			}

			bResult = AnalyzeTestResultMCap_RXX( bIncludeKey );

			if( !bResult )
			{
				
				printk("hjptest--->Uniformity_CheckRx fail\n");
				btmpresult = false;
			}
		}


		
	}
	
	if( g_TestParam_FT5X46.Uniformity_CheckMinMax )
	{
		printk("hjptest--->Uniformity_CheckMinMax ! \n");
		iMin = 100000, iMax = -100000;
		
		for (  iRow = 0; iRow < iTxNum; ++iRow )
		{
			for (  iCol = 0; iCol < iRxNum; ++iCol )
			{
				if( 0 == g_TestParam_FT5X46.valid_node[iRow][iCol] )
				{
					continue;
				}
				if( 2 == g_TestParam_FT5X46.valid_node[iRow][iCol] )
				{
					continue;
				}
			
				if(iMin<= rawdata[iRow][iCol])
				{
					iMin =iMin;
				}
				else
				{
					iMin =rawdata[iRow][iCol];
				}
				
				if( iMax>= rawdata[iRow][iCol])
				{
					iMax =iMax;					
				}
				else
				{
					iMax =rawdata[iRow][iCol];					
				}

				
			}
		}
		iMax = !iMax ? 1 : iMax;

		if(iMin<0)
		{
			iMin=(-1)*iMin;
		}
		if(iMax<0)
		{
			iMax=(-1)*iMax;
		}
		iUniform = 100 * (iMin) / (iMax);
		
		//printk("20150111  zax8  Uniformity_MinMax_Hole iMin==%d,iMax=%d,iUniform=%d\n",iMin,iMax,iUniform);
		if( iUniform < g_TestParam_FT5X46.Uniformity_MinMax_Hole )
		{
			btmpresult = false;
			printk("hjptest--->Uniformity_MinMax_Hole fail\n");
			
		}

	}

	//�ָ�v3�����mappingֵ
	if (m_bV3TP)
	{
		ReCode = WriteReg( REG_MAPPING_SWITCH, strSwitch );
		if( ReCode != ERROR_CODE_OK)	
		{
			printk("hjptest--->WriteReg fail\n");
			btmpresult = false;
		}
		printk("[3] ReCode = %d \n", ReCode);
	}

	printk(" test result btmpresult = %d and ReCode = %d !\n", btmpresult, ReCode);

	if( btmpresult && (ReCode >= ERROR_CODE_OK) )
	{
		bTestResult = true;
		printk("hjptest--->Uniformity test success\n");
		
	}
	else
	{
		printk("hjptest--->Uniformity test fail\n");
		bTestResult = false;
	
	}
	return bTestResult;
}
//zax 20150112--------------------------------------------
