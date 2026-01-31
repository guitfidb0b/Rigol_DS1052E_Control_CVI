/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: panelCB */
#define  PANEL_CONNECT                    2       /* control type: command, callback function: connectCallback */
#define  PANEL_DEVICENAME                 3       /* control type: string, callback function: (none) */
#define  PANEL_RESET                      4       /* control type: command, callback function: resetCallback */
#define  PANEL_AUTO                       5       /* control type: command, callback function: autoCallback */
#define  PANEL_STOP                       6       /* control type: command, callback function: stopCallback */
#define  PANEL_RUN                        7       /* control type: command, callback function: runCallback */
#define  PANEL_MEASURE                    8       /* control type: command, callback function: measureCallback */
#define  PANEL_PERCHAN2                   9       /* control type: numeric, callback function: (none) */
#define  PANEL_PERCHAN1                   10      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQCHAN2                  11      /* control type: numeric, callback function: (none) */
#define  PANEL_FREQCHAN1                  12      /* control type: numeric, callback function: (none) */
#define  PANEL_VRMSCHAN2                  13      /* control type: numeric, callback function: (none) */
#define  PANEL_VRMSCHAN1                  14      /* control type: numeric, callback function: (none) */
#define  PANEL_VAVECHAN2                  15      /* control type: numeric, callback function: (none) */
#define  PANEL_VAVECHAN1                  16      /* control type: numeric, callback function: (none) */
#define  PANEL_VMINCHAN2                  17      /* control type: numeric, callback function: (none) */
#define  PANEL_VMINCHAN1                  18      /* control type: numeric, callback function: (none) */
#define  PANEL_VMAXCHAN2                  19      /* control type: numeric, callback function: (none) */
#define  PANEL_VMAXCHAN1                  20      /* control type: numeric, callback function: (none) */
#define  PANEL_WAVEMEAS                   21      /* control type: command, callback function: waveDataCallback */
#define  PANEL_WAVEFORM                   22      /* control type: graph, callback function: (none) */
#define  PANEL_STOPACQ                    23      /* control type: command, callback function: stopAcqCallback */
#define  PANEL_TIMER                      24      /* control type: timer, callback function: timerCallback */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK autoCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK connectCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK measureCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK resetCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK runCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopAcqCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK stopCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK waveDataCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif