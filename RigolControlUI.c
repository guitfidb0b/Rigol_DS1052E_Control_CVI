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
static unsigned char waveData[500];
static double waveDataFloat[500];
static int globalStatus = 0;

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
			int i;
			status=viOpenDefaultRM (&defaultRM);
			status = viFindRsrc (defaultRM, "USB?*INSTR", &findList, &numInstrs, instrResourceString);
			for (i=0; i<numInstrs; i++)
   			{
      			if (i > 0)
         			viFindNext (findList, instrResourceString);

      			status = viOpen (defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);
      			strcpy (stringinput,"*IDN?\n");
      			status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
				
				status = viRead (instr, buffer, 100, &retCount);
				SetCtrlVal(panelHandle, PANEL_DEVICENAME, buffer); 
      			SetCtrlAttribute(panelHandle, PANEL_RESET, ATTR_DIMMED, 0); 
				SetCtrlAttribute(panelHandle, PANEL_RUN, ATTR_DIMMED, 0);
				SetCtrlAttribute(panelHandle, PANEL_STOP, ATTR_DIMMED, 0);
				SetCtrlAttribute(panelHandle, PANEL_AUTO, ATTR_DIMMED, 0);
				SetCtrlAttribute(panelHandle, PANEL_MEASURE, ATTR_DIMMED, 0);
				SetCtrlAttribute(panelHandle, PANEL_WAVEMEAS, ATTR_DIMMED, 0);
				SetCtrlAttribute(panelHandle, PANEL_STOPACQ, ATTR_DIMMED, 0);
   			}
			
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
			//Reset Scope settings and close connection
			memset(stringinput, '\0', sizeof(stringinput));
			strcpy (stringinput,"*RST\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			
			//Clear buffer
			memset(buffer, '\0', sizeof(buffer));
			SetCtrlVal(panelHandle, PANEL_DEVICENAME, buffer); 
			SetCtrlAttribute(panelHandle, PANEL_RESET, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_RUN, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_STOP, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_AUTO, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_MEASURE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_WAVEMEAS, ATTR_DIMMED, 1);
			SetCtrlAttribute(panelHandle, PANEL_STOPACQ, ATTR_DIMMED, 1);
			SetCtrlVal(panelHandle, PANEL_VMAX, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VMIN, 0.0); 
			SetCtrlVal(panelHandle, PANEL_VAVE, 0.0);
			SetCtrlVal(panelHandle, PANEL_VRMS, 0.0);
			SetCtrlVal(panelHandle, PANEL_FREQ, 0.0);
			SetCtrlVal(panelHandle, PANEL_PER, 0.0);
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
			//Clear the stringinput buffer
			double num;
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMAX? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMAX, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VMIN? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VMIN, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VAV? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VAVE, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:VRMS? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_VRMS, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:FREQ? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_FREQ, num); 
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput,":MEAS:PER? [CHAN1]\n");
      		status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, 100, &retCount);
			num = atof(buffer);
			SetCtrlVal(panelHandle, PANEL_PER, num); 

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
			double voltScale;
			double voltOffset;
			double sampleRate;
			double xGainValue;
			int waveDataInt [sizeof(waveData)];
			
			memset(stringinput, '\0', sizeof(stringinput));
			memset(waveData, '\0', sizeof(waveData));
			memset(waveDataFloat, '\0', sizeof(waveData));
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
			voltScale = atof(buffer);
			
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput, ":CHAN1:OFFS?\n");
			status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, sizeof(buffer), &retCount);
			voltOffset = atof(buffer);
			
			memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput, ":ACQ:SAMP?\n");
			status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, sizeof(buffer), &retCount);
			sampleRate = atof(buffer);
			
			xGainValue = 1/sampleRate;
			
			/*memset(buffer, '\0', sizeof(buffer));
			strcpy (stringinput, ":ACQ:MODE?\n");
			status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
			status = viRead (instr, buffer, sizeof(buffer), &retCount);
			printf("Returned Val: %s", buffer);
			*///sampleRate = atof(buffer);
			
			/*printf("SampleRate: %f", sampleRate);
			printf("Timescale: %f\n", timeScale);*/	
			
			
			
			//while(globalStatus != 1){
			//Cast the ASCII data to int, 
				//if(globalStatus == 1){
				//	break;
				//}
				//else{
				memset(waveData, '\0', sizeof(waveData));
					strcpy (stringinput, ":WAV:POIN:MODE RAW\n");
					status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
					strcpy (stringinput, ":WAV:DATA? CHAN1\n");
					status = viWrite(instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
					status = viRead (instr, waveData, sizeof(waveData), &retCount);
					
					for(int i = 0; i < sizeof(waveData); i++){
						waveDataInt[i] = (int)waveData[i];
						waveDataFloat[i] = waveDataInt[i]*(-1) + 255;
						waveDataFloat[i] = (waveDataFloat[i] - 130.0 - voltOffset/voltScale*25)/25*voltScale;
					}	
					DeleteGraphPlot(panelHandle, PANEL_WAVEFORM, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panelHandle, PANEL_WAVEFORM, waveDataFloat, 500, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
					SetCtrlAttribute(panelHandle, PANEL_WAVEFORM, ATTR_XAXIS_GAIN, xGainValue);
				//}
			//}
	
	return 0;
}
