#include "pxcsession.h"
#include "pxccapturemanager.h"
#include "pxccapture.h"
#include "pxchandmodule.h"
#include "pxchandcursormodule.h"
#include "pxchanddata.h"
#include "pxcsensemanager.h"
#include "pxcmetadata.h"
#include "service/pxcsessionservice.h"
#include "PXCHandConfiguration.h"
#include "pxccursorconfiguration.h"
#include <string>
#include <iostream>

using namespace std;

int main(){
	PXCHandCursorModule *handCursorAnalyzer = NULL;
	PXCCursorData* outputCursorData = NULL;
	PXCCursorConfiguration* configCursor = NULL;

	PXCSession *c_session = PXCSession::CreateInstance();
	if (!c_session) {
		return -1;
	}
	PXCSenseManager *c_senseManager = c_session->CreateSenseManager();
	if (!c_senseManager) {
		return -1;
	}
	
	pxcStatus status = c_senseManager->EnableHandCursor(0);

	if (status != pxcStatus::PXC_STATUS_NO_ERROR)
	{
		return status;
	}
	handCursorAnalyzer = c_senseManager->QueryHandCursor();
	if (handCursorAnalyzer == NULL) {
		return status;
	}
	outputCursorData = handCursorAnalyzer->CreateOutput();
	configCursor = handCursorAnalyzer->CreateActiveConfiguration();

	if (configCursor != NULL) {
		configCursor->EnableGesture(PXCCursorData::GestureType::CURSOR_HAND_CLOSING);
		configCursor->EnableGesture(PXCCursorData::GestureType::CURSOR_CLOCKWISE_CIRCLE);
		configCursor->EnableGesture(PXCCursorData::GestureType::CURSOR_COUNTER_CLOCKWISE_CIRCLE);
		configCursor->ApplyChanges();
	}
	if (c_senseManager->Init() == PXC_STATUS_NO_ERROR)
	{
		//Start working on current frame
		while (c_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR)
		{
			if (outputCursorData->Update() == PXC_STATUS_NO_ERROR) {
				int numOfGesture = outputCursorData->QueryFiredGesturesNumber();
				PXCCursorData::ICursor *icursor = 0;
				outputCursorData->QueryCursorData(PXCCursorData::ACCESS_ORDER_NEAR_TO_FAR, 0, icursor);
				if (icursor != 0) {
					PXCPoint3DF32 point = icursor->QueryCursorImagePoint();
					cout << "point:" << point.x << "," << point.y << endl;
				}
				for (int i = 0; i < numOfGesture; i++)
				{
					PXCCursorData::GestureData gestureData;
					if (outputCursorData->QueryFiredGestureData(i, gestureData) == PXC_STATUS_NO_ERROR)
					{
						std::string gestureName = "";
						switch (gestureData.label)
						{
						case PXCCursorData::CURSOR_CLOCKWISE_CIRCLE:
							gestureName = "back";
							break;
						case PXCCursorData::CURSOR_COUNTER_CLOCKWISE_CIRCLE:
							gestureName = "back";
							break;						
						case PXCCursorData::CURSOR_HAND_CLOSING:
							gestureName = "cursor_hand_closing";
							break;
						default:
							break;
						}
						cout << gestureName << endl;
					}
				}
			}
			c_senseManager->ReleaseFrame();
			//Finish Work on frame			
		}
	}
	return 0;
}