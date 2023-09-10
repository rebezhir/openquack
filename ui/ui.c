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

#include <string.h>
#include "app/dtmf.h"
#include "app/scanner.h"
#include "driver/keyboard.h"
#include "misc.h"
#include "ui/inputbox.h"
#include "ui/main.h"
#include "ui/menu.h"
#include "ui/scanner.h"
#include "ui/ui.h"

GUI_DisplayType_t gScreenToDisplay;
GUI_DisplayType_t gRequestDisplayScreen = DISPLAY_INVALID;

uint8_t gAskForConfirmation;
bool gAskToSave;
bool gAskToDelete;

void GUI_DisplayScreen(void)
{
	switch (gScreenToDisplay) {
	case DISPLAY_MAIN:
		UI_DisplayMain();
		break;
	case DISPLAY_FM:
		break;
	case DISPLAY_MENU:
		UI_DisplayMenu();
		break;
	case DISPLAY_SCANNER:
		UI_DisplayScanner();
		break;
	default:
		break;
	}
}

void GUI_SelectNextDisplay(GUI_DisplayType_t Display)
{
	if (Display != DISPLAY_INVALID) {
		if (gScreenToDisplay != Display) {
			gInputBoxIndex = 0;
			gIsInSubMenu = false;
			gCssScanMode = CSS_SCAN_MODE_OFF;
			gScanState = SCAN_OFF;
			gAskForConfirmation = 0;
			gDTMF_InputMode = false;
			gDTMF_InputIndex = 0;
			gF_LOCK = false;
			gAskToSave = false;
			gAskToDelete = false;
			if (gWasFKeyPressed) {
				gWasFKeyPressed = false;
				gUpdateStatus = true;
			}
		}
		gUpdateDisplay = true;
		gScreenToDisplay = Display;
	}
}

