//==============================================================================
//
// Title:		RigolControlUI
// Purpose:		A short description of the application.
//
// Created on:	1/24/2026 at 3:29:54 PM by Robert Chavez.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
/* Functions like strcpy are technically not secure because they do */
/* not contain a 'length'. But we disable this warning for the VISA */
/* examples since we never copy more than the actual buffer size.   */
#define _CRT_SECURE_NO_DEPRECATE
#endif

/*
1) Fix the time issue in the Plot
2) Create a button that wraps data up in JSON .txt File
3) Refactor Code
4) Create Real time waveform in graph
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "visa.h"

#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "RigolControlUI.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;

static ViSession defaultRM;     
static ViSession instr;
static ViUInt32 numInstrs;
static ViFindList findList;   
static ViUInt32 retCount;
static ViUInt32 writeCount;
static ViStatus status;
static char instrResourceString[VI_FIND_BUFLEN];

static unsigned char buffer[100];
static char stringinput[512];
static unsigned char waveData1[500];
static double waveDataFloat1[500];
static unsigned char waveData2[500];
static double waveDataFloat2[500];

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (panelHandle = LoadPanel (0, "RigolControlUI.uir", PANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (panelHandle));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel (panelHandle);
	return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		QuitUserInterface (0);
	return 0;
}

int CVICALLBACK connectCallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			//Connect to Device: Rigol Scope as instrument, query
			status = viOpenDefaultRM (&defaultRM);
			status = viFindRsrc (defaultRM, "USB?*INSTR", &findList, &numInstrs, instrResourceString);
      		status = viOpen (defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);
      		strcpy (stringinput,"*IDN?\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			
			//Set initial conditions for objects in panel
			SetCtrlVal(panelHandle, PANEL_DEVICENAME, buffer); 
      		SetCtrlAttribute(panelHandle, PANEL_RESET, ATTR_DIMMED, 0); 
			SetCtrlAttribute(panelHandle, PANEL_RUN, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_STOP, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_AUTO, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_MEASURE, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_WAVEMEAS, ATTR_DIMMED, 0);
			SetCtrlAttribute(panelHandle, PANEL_STOPACQ, ATTR_DIMMED, 0);
			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK resetCallback (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Reset Scope settings and close remote connection
			memset(stringinput, '\0', sizeof(stringinput));
			strcpy (stringinput,"*RST\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			memset(buffer, '\0', sizeof(buffer));
			
			//Reset panel objects
			SetCtrlVal(panelHandle, PANEL_DEVICENAME, buffer); 
			SetCtrlAttribute(panelHandle, PANEL_RESET, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_RUN, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_STOP, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_AUTO, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_MEASURE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_WAVEMEAS, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_STOPACQ, ATTR_DIMMED, 1);
			SetCtrlVal(panelHandle, PANEL_VMAXCHAN1, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VMINCHAN1, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VAVECHAN1, 0.0);
			SetCtrlVal(panelHandle, PANEL_VRMSCHAN1, 0.0);
			SetCtrlVal(panelHandle, PANEL_FREQCHAN1, 0.0);
			SetCtrlVal(panelHandle, PANEL_PERCHAN1, 0.0);
			SetCtrlVal(panelHandle, PANEL_VMAXCHAN2, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VMINCHAN2, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VAVECHAN2, 0.0);
			SetCtrlVal(panelHandle, PANEL_VRMSCHAN2, 0.0);
			SetCtrlVal(panelHandle, PANEL_FREQCHAN2, 0.0);
			SetCtrlVal(panelHandle, PANEL_PERCHAN2, 0.0);
			DeleteGraphPlot(panelHandle, PANEL_WAVEFORM, -1, VAL_IMMEDIATE_DRAW);
			
			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK runCallback (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Sends command for scope run
			memset(stringinput, '\0', sizeof(stringinput));
			strcpy (stringinput,":RUN\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			
			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK stopCallback (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			memset(stringinput, '\0', sizeof(stringinput));
			strcpy (stringinput,":STOP\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);

			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK autoCallback (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			memset(stringinput, '\0', sizeof(stringinput));
			strcpy (stringinput,":AUTO\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);

			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK measureCallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//Get common measurements
			double num;
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMAX? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMAXCHAN1, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMAX? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMAXCHAN2, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMIN? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMINCHAN1, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMIN? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMINCHAN2, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VAV? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VAVECHAN1, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VAV? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VAVECHAN2, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VRMS? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VRMSCHAN1, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VRMS? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VRMSCHAN2, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:FREQ? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_FREQCHAN1, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:FREQ? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_FREQCHAN2, num);
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:PER? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_PERCHAN1, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:PER? [CHAN2]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_PERCHAN2, num);

			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK waveDataCallback (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			
			break;
			
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK stopAcqCallback (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			break;
		case EVENT_RIGHT_CLICK:

			break;
	}
	return 0;
}

int CVICALLBACK timerCallback (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	/*
	If Measure waveform pressed, enable Timer
	acquire waveform data at timer polling rate
	If stop pressed, disable timer
	*/
	double timeScale;
	double timeOffset;
	double voltScale1, voltScale2;
	double voltOffset1, voltOffset2;
	double sampleRate;
	double xGainValue;
	int waveDataInt1 [500];
	int waveDataInt2 [500];
			
	memset(stringinput, '\0', sizeof(stringinput));
	memset(waveData1, '\0', sizeof(waveData1));
	memset(waveDataFloat1, '\0', sizeof(waveData1));
	memset(waveData2, '\0', sizeof(waveData2));
	memset(waveDataFloat2, '\0', sizeof(waveData2));
	memset(buffer, '\0', sizeof(buffer));
			
	//Get timescale
	strcpy (stringinput, ":TIM:SCAL?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	timeScale = atof(buffer);
			
	//Get time offset
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":TIM:OFFS?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	timeOffset = atof(buffer);
			
	//Get volt scale
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":CHAN1:SCAL?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	voltScale1 = atof(buffer);
	
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":CHAN2:SCAL?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	voltScale2 = atof(buffer);
			
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":CHAN1:OFFS?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	voltOffset1 = atof(buffer);
	
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":CHAN2:OFFS?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	voltOffset2 = atof(buffer);
			
	memset(buffer, '\0', sizeof(buffer));
	strcpy (stringinput, ":ACQ:SAMP?\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, buffer, sizeof(buffer), &retCount);
	sampleRate = atof(buffer);
			
	xGainValue = 1/sampleRate;
			
	//memset(waveData1, '\0', sizeof(waveData1));
	strcpy (stringinput, ":WAV:POIN:MODE RAW\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	
	strcpy (stringinput, ":WAV:DATA? CHAN1\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, waveData1, sizeof(waveData1), &retCount);
	
	strcpy (stringinput, ":WAV:DATA? CHAN2\n");
	status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
	status = viRead (instr, waveData2, sizeof(waveData2), &retCount);
					
	for(int i = 0; i < sizeof(waveData1); i++){
		waveDataInt1[i] = (int)waveData1[i];
		waveDataFloat1[i] = waveDataInt1[i]*(-1) + 255;
		
		waveDataInt2[i] = (int)waveData2[i];
		waveDataFloat2[i] = waveDataInt2[i]*(-1) + 255;
		
		waveDataFloat1[i] = (waveDataFloat1[i] - 130.0 - voltOffset1/voltScale1*25)/25*voltScale1;
		waveDataFloat2[i] = (waveDataFloat2[i] - 130.0 - voltOffset2/voltScale2*25)/25*voltScale2;
	}	
	DeleteGraphPlot(panelHandle, PANEL_WAVEFORM, -1, VAL_DELAYED_DRAW);
	PlotY(panelHandle, PANEL_WAVEFORM, waveDataFloat1, 500, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
	PlotY(panelHandle, PANEL_WAVEFORM, waveDataFloat2, 500, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
	SetCtrlAttribute(panelHandle, PANEL_WAVEFORM, ATTR_XAXIS_GAIN, xGainValue);

	return 0;
}
