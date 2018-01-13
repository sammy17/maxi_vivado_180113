#include <xparameters.h>
#include <xbgsub.h>
#include <xfeature.h>
#include <stdio.h>
#include <string.h>
#include <xtime_l.h>
#include "frame1.h"


#define TX_BASE_ADDR 0x11000000
#define RX_BASE_ADDR 0x12000000
#define BG_MODEL 0x14000000

#define M_AXI_BOUNDING_0 0x21000000
#define M_AXI_FEATUREH_0 0x29000000

#define M_AXI_BOUNDING_1 0x21100000
#define M_AXI_FEATUREH_1 0x29100000

#define M_AXI_BOUNDING_2 0x21200000
#define M_AXI_FEATUREH_2 0x29200000

uint32_t *m_buffer_TX = (uint32_t*) TX_BASE_ADDR;
uint8_t *m_test = (uint8_t*) TX_BASE_ADDR;
unsigned char *m_buffer_RX = (unsigned char*) RX_BASE_ADDR;

uint16_t * m_axi_bound_0 = (uint16_t *) M_AXI_BOUNDING_0;
uint16_t * m_axi_feature_0 = (uint16_t *) M_AXI_FEATUREH_0;

uint16_t * m_axi_bound_1 = (uint16_t *) M_AXI_BOUNDING_1;
uint16_t * m_axi_feature_1 = (uint16_t *) M_AXI_FEATUREH_1;

uint16_t * m_axi_bound_2 = (uint16_t *) M_AXI_BOUNDING_2;
uint16_t * m_axi_feature_2 = (uint16_t *) M_AXI_FEATUREH_2;

XBgsub backsub;
XFeature feature1;
XFeature feature2;
XFeature feature3;

int main()
{
	xil_printf("begin\n");
	XTime tStart, tEnd, t1, t2;

	XBgsub_Initialize(&backsub,XPAR_BGSUB_0_DEVICE_ID);
	XFeature_Initialize(&feature1,XPAR_FEATURE_0_DEVICE_ID);
	XFeature_Initialize(&feature2,XPAR_FEATURE_1_DEVICE_ID);
	XFeature_Initialize(&feature3,XPAR_FEATURE_2_DEVICE_ID);

	XBgsub_DisableAutoRestart(&backsub);
	XBgsub_InterruptGlobalDisable(&backsub);



	// Checking backsub ip first

	XBgsub_Set_frame_in(&backsub,(u32)TX_BASE_ADDR);
	XBgsub_Set_frame_out(&backsub,(u32)RX_BASE_ADDR);
	XBgsub_Set_init(&backsub,true);
	XBgsub_Set_bgmodel(&backsub,(u32)BG_MODEL);

	XTime_GetTime(&tStart); //Start measuring time

	memcpy(m_buffer_TX,frame1,sizeof(uint32_t)*76800/2);
	XBgsub_Start(&backsub);

	while(!XBgsub_IsDone(&backsub));

	XTime_GetTime(&tEnd);

	xil_printf("Backsub IP core is done\n");

	// checking feature IP core

	XFeature_Set_frame_in(&feature1,(u32)TX_BASE_ADDR);
	XFeature_Set_bounding(&feature1,(u32)M_AXI_BOUNDING_0);
	XFeature_Set_featureh(&feature1,(u32)M_AXI_FEATUREH_0);

	XFeature_Set_frame_in(&feature2,(u32)TX_BASE_ADDR);
	XFeature_Set_bounding(&feature2,(u32)M_AXI_BOUNDING_1);
	XFeature_Set_featureh(&feature2,(u32)M_AXI_FEATUREH_1);

	XFeature_Set_frame_in(&feature3,(u32)TX_BASE_ADDR);
	XFeature_Set_bounding(&feature3,(u32)M_AXI_BOUNDING_2);
	XFeature_Set_featureh(&feature3,(u32)M_AXI_FEATUREH_2);

	// setting random bounding box coordinates for bounding

	XTime_GetTime(&t1);

	m_axi_bound_0[0] = 64;
	m_axi_bound_0[1] = 64;
	m_axi_bound_0[2] = 128;
	m_axi_bound_0[3] = 128;

	m_axi_bound_1[0] = 64;
	m_axi_bound_1[1] = 64;
	m_axi_bound_1[2] = 128;
	m_axi_bound_1[3] = 128;

	m_axi_bound_2[0] = 64;
	m_axi_bound_2[1] = 64;
	m_axi_bound_2[2] = 128;
	m_axi_bound_2[3] = 128;

	XFeature_Start(&feature1);
	XFeature_Start(&feature2);
	XFeature_Start(&feature3);

	while(!XFeature_IsDone(&feature1));
	while(!XFeature_IsDone(&feature2));
	while(!XFeature_IsDone(&feature3));

	XTime_GetTime(&t2);

	xil_printf("Feature IP Core is done\n");

	xil_printf("Checking the output of backsub is correct...\n");

//	for (int i=0;i<10;i++){
////		xil_printf("Expected : %d, Actual : %d\n",m_test[2*i],m_buffer_RX[i]);
//	}

	xil_printf("Check memory location 0x29000000 for histogram\n");


//	xil_printf("Output took %llu clock cycles.\n", 2*(tEnd - tStart));
	printf("Bgsub   time : %.2f us.\n",1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND/1000000));
	printf("Feature time : %.2f us.\n",1.0 * (t2 - t1) / (COUNTS_PER_SECOND/1000000));
	return 0;
}
