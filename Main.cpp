#include <stdio.h>
#include <string.h>

#define XPLM300
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"

#define ROW 6
char Buffer[ROW][40];
XPLMWindowID Window = NULL;
XPLMDataRef	DR_speed, DR_pitch, DR_roll, DR_heading, DR_vertical, DR_altitude;

//---------------------------------------------------------------------------
// Function prototypes

float OnLoop(float elapsedMe, float elapsedSim, int counter, void *refcon);
void OnDrawWindow(XPLMWindowID inWindowID, void *inRefcon);

//---------------------------------------------------------------------------
// SDK Mandatory Callbacks

PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc){
	strcpy(outName, "MotionMonitor");
	strcpy(outSig, "lakwsh.motionmonitor");
	strcpy(outDesc, "");

	int bounds[4]; // 0=left, 1=top, 2=right, 3=bottom
	XPLMGetScreenBoundsGlobal(&bounds[0], &bounds[3], &bounds[2], &bounds[1]);
	
	Window = XPLMCreateWindow(bounds[0] + 200, bounds[3] - 200, bounds[0] + 380, bounds[3] - 310, 1, OnDrawWindow, NULL, NULL, NULL);

	DR_speed	= XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");
	DR_pitch	= XPLMFindDataRef("sim/flightmodel/position/true_theta");
	DR_roll		= XPLMFindDataRef("sim/flightmodel/position/true_phi");
	DR_heading	= XPLMFindDataRef("sim/flightmodel/position/mag_psi");
	DR_vertical	= XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm");
	DR_altitude	= XPLMFindDataRef("sim/flightmodel/position/y_agl");
	memset(Buffer, 0, sizeof(Buffer));

	XPLMRegisterFlightLoopCallback(OnLoop, 1.0, NULL);

	return 1;
}

PLUGIN_API void	XPluginStop(void){
	XPLMDestroyWindow(Window);
	XPLMUnregisterFlightLoopCallback(OnLoop, NULL);
}

PLUGIN_API int XPluginEnable(void){
	return 1;
}

PLUGIN_API void XPluginDisable(void){ }

void OnDrawWindow(XPLMWindowID inWindowID, void *inRefcon){

	float rgb[] = {1.0, 1.0, 1.0};
	int l, t, r, b;

	XPLMGetWindowGeometry(inWindowID, &l, &t, &r, &b);
	XPLMDrawTranslucentDarkBox(l, t, r, b);

	for(int i=0; i < ROW; i++) XPLMDrawString(rgb, l + 15, t - 20 - 15 * i, Buffer[i], NULL, xplmFont_Basic);
}

float OnLoop(float elapsedMe, float elapsedSim, int counter, void *refcon){
	sprintf(Buffer[0], "airspeed = %8.2f kias", XPLMGetDataf(DR_speed));
	sprintf(Buffer[1], "   pitch = %8.2f deg", XPLMGetDataf(DR_pitch));
	sprintf(Buffer[2], "    roll = %8.2f deg", XPLMGetDataf(DR_roll));
	sprintf(Buffer[3], " heading = %8.2f deg", XPLMGetDataf(DR_heading));
	sprintf(Buffer[4], "vertical = %8.2f fpm", XPLMGetDataf(DR_vertical));
	sprintf(Buffer[5], "altitude = %8.2f feet", XPLMGetDataf(DR_altitude) * 3.28);

	return 0.1;
}