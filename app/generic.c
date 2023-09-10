/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "app/app.h"
#include "app/generic.h"
#include "app/scanner.h"
#include "audio.h"
#include "driver/keyboard.h"
#include "dtmf.h"
#include "external/printf/printf.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/inputbox.h"
#include "ui/ui.h"

void GENERIC_Key_F(bool bKeyPressed, bool bKeyHeld)
{
	if (gInputBoxIndex) {
		if (!bKeyHeld && bKeyPressed) {
			gBeepToPlay = BEEP_500HZ_60MS_DOUBLE_BEEP_OPTIONAL;
		}
		return;
	}
	if (bKeyHeld || !bKeyPressed) {
		if (bKeyHeld || bKeyPressed) {
			if (!bKeyHeld) {
				return;
			}
			if (!bKeyPressed) {
				return;
			}
			if (gEeprom.KEY_LOCK) {
				gAnotherVoiceID = VOICE_ID_UNLOCK;
			} else {
				gAnotherVoiceID = VOICE_ID_LOCK;
			}
			gEeprom.KEY_LOCK = !gEeprom.KEY_LOCK;
			gRequestSaveSettings = true;
		} else {
			if (gScreenToDisplay != DISPLAY_MAIN) {
				return;
			}
			gWasFKeyPressed = !gWasFKeyPressed;
			if (!gWasFKeyPressed) {
				gAnotherVoiceID = VOICE_ID_CANCEL;
			}
			gUpdateStatus = true;
		}
	} else {
			gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
			return;


		gBeepToPlay = BEEP_440HZ_500MS;
		gPttWasReleased = true;
	}
}

void GENERIC_Key_PTT(bool bKeyPressed)
{
	gInputBoxIndex = 0;
	if (!bKeyPressed) {
		if (gScreenToDisplay == DISPLAY_MAIN) {
			if (gCurrentFunction == FUNCTION_TRANSMIT) {
				if (gFlagEndTransmission) {
					FUNCTION_Select(FUNCTION_FOREGROUND);
				} else {
					APP_EndTransmission();
					if (gEeprom.REPEATER_TAIL_TONE_ELIMINATION == 0) {
						FUNCTION_Select(FUNCTION_FOREGROUND);
					} else {
						gRTTECountdown = gEeprom.REPEATER_TAIL_TONE_ELIMINATION * 10;
					}
				}
				gFlagEndTransmission = false;
				gVOX_NoiseDetected = false;
			}
			RADIO_SetVfoState(VFO_STATE_NORMAL);
			gRequestDisplayScreen = DISPLAY_MAIN;
			return;
		}
		gInputBoxIndex = 0;
		return;
	}

	if (gScanState != SCAN_OFF) {
		SCANNER_Stop();
		gPttDebounceCounter = 0;
		gPttIsPressed = false;
		gRequestDisplayScreen = DISPLAY_MAIN;
		return;
	}


		if (gCssScanMode == CSS_SCAN_MODE_OFF) {
			if (gScreenToDisplay == DISPLAY_MENU || gScreenToDisplay == DISPLAY_FM) {
				gRequestDisplayScreen = DISPLAY_MAIN;
				gInputBoxIndex = 0;
				gPttIsPressed = false;
				gPttDebounceCounter = 0;
				return;
			}
			if (gScreenToDisplay != DISPLAY_SCANNER) {
				if (gCurrentFunction == FUNCTION_TRANSMIT && gRTTECountdown == 0) {
					gInputBoxIndex = 0;
					return;
				}
				gFlagPrepareTX = true;
				if (gDTMF_InputMode) {
					if (gDTMF_InputIndex || gDTMF_PreviousIndex) {
						if (gDTMF_InputIndex == 0) {
							gDTMF_InputIndex = gDTMF_PreviousIndex;
						}
						gDTMF_InputBox[gDTMF_InputIndex] = 0;
						if (gDTMF_InputIndex == 3) {
							gDTMF_CallMode = DTMF_CheckGroupCall(gDTMF_InputBox, 3);
						} else {
							gDTMF_CallMode = DTMF_CALL_MODE_DTMF;
						}
						sprintf(gDTMF_String, "%s", gDTMF_InputBox);
						gDTMF_PreviousIndex = gDTMF_InputIndex;
						gDTMF_ReplyState = DTMF_REPLY_ANI;
						gDTMF_State = DTMF_STATE_0;
					}
					gRequestDisplayScreen = DISPLAY_MAIN;
					gDTMF_InputMode = false;
					gDTMF_InputIndex = 0;
					return;
				}
				gRequestDisplayScreen = DISPLAY_MAIN;
				gFlagPrepareTX = true;
				gInputBoxIndex = 0;
				return;
			}
			gRequestDisplayScreen = DISPLAY_MAIN;
			gEeprom.CROSS_BAND_RX_TX = gBackupCROSS_BAND_RX_TX;
			gUpdateStatus = true;
			gFlagStopScan = true;
			gVfoConfigureMode = VFO_CONFIGURE_RELOAD;
			gFlagResetVfos = true;
		} else {
			RADIO_StopCssScan();
			gRequestDisplayScreen = DISPLAY_MENU;
		}

	gAnotherVoiceID = VOICE_ID_SCANNING_STOP;
	gPttWasPressed = true;
}

