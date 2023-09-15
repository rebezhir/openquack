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
#include "bitmaps.h"
#include "dcs.h"
#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "helper/battery.h"
#include "misc.h"
#include "settings.h"
#include "ui/helper.h"
#include "ui/inputbox.h"
#include "ui/menu.h"
#include "ui/ui.h"
#include "version.h"

static const char MenuList[][11] = {
	// 0x00
	"Squelch",    "Freq step",    "TX power",    "RX DCS",
	"RX CTCSS", "TX DCS",   "TX CTCSS", "Shift to",
	// 0x08
	"Offset", "Bandwidth",     "Scrambler",    "Busy lock",
	"Save Ch.", "Save",    "VOX",    "Backlight",
	// 0x10
	"Dual RX",    "Crossband",      "Beep",   "TX limit",
    "Scan mode",  "Display",    "Autolock",   "Lock type",
	// 0x18
	"Add to S1", "Add to S2",  "Tail tone",    "RP-STE",
	"Mic gain",    "Fast call",  "Scanlist", "Scanlist 1",
	// 0x20
	"Scanlist 2",  "ANI-ID", "Upcode",
	"Downcode", "DTMF Sound",    "Response",  "DTMF hold",
	// 0x28
	"DTMF time",  "PTT-ID",  "Decoder",  "Contacts",
	 "Roger",   "Battery",    "AM RX",
	// 0x30
	"Delete Ch.", "F1 short", "F1 long", "F2 short", "F2 long",
	"Reset", "Author", "All TX", "F-Lock",
    "200 MHz TX",   "500 MHz TX",  "350 EN", "Scrambler", "Calibrate",
	// 0x38
};

static const uint16_t gSubMenu_Step[] = {
	250,
	500,
	625,
	1000,
	1250,
	2500,
	833,
};

static const char gSubMenu_TXP[3][7] = {
	"Low",
	"Middle",
	"High",
};

static const char gSubMenu_SFT_D[3][4] = {
	"Off",
	"+",
	"-",
};

static const char gSubMenu_W_N[2][7] = {
	"Wide",
	"Narrow",
};

static const char gSubMenu_OFF_ON[2][4] = {
	"Off",
	"On",
};

static const char gSubMenu_SAVE[5][4] = {
	"Off",
	"1:1",
	"1:2",
	"1:3",
	"1:4",
};

static const char gSubMenu_CHAN[3][10] = {
	"Off",
	"Channel A",
	"Channel B",
};

static const char gSubMenu_ABOUT[9] = {
	"RebeZhir",
};

static const char gSubMenu_SC_REV[3][10] = {
	"Pause 5s",
	"Keep RX",
	"Full stop",
};

static const char gSubMenu_MDF[3][10] = {
	"Frequency",
	"Channel #",
	"Name",
};

static const char gSubMenu_LOCK_TYPE[3][11] = {
	"Only front",
	"With F1/F2",
	"All keys",
};

static const char gSubMenu_D_RSP[4][6] = {
	"Null",
	"Ring",
	"Reply",
	"Both",
};

static const char gSubMenu_PTT_ID[4][9] = {
	"Off",
	"TX start",
	"TX end",
	"Both",
};


static const char gSubMenu_ROGER[3][9] = {
	"Off",
	"Standard",
	"Frog",
};

static const char gSubMenu_FUNCTIONS[9][11] = {
	"None",
	"Flashlight",
	"TX Power",
	"Monitor",
	"Scanner",
	"VOX on/off",
	"VFO/MR",
	"A/B",
	"1750 Hz",
};

static const char gSubMenu_RESET[2][4] = {
	"VFO",
	"All",
};

static const char gSubMenu_F_LOCK[6][4] = {
	"Off",
	"FCC",
	"CE",
	"GB",
	"430",
	"438",
};

bool gIsInSubMenu;

uint8_t gMenuCursor;
int8_t gMenuScrollDirection;
uint32_t gSubMenuSelection;

void UI_DisplayMenu(void)
{
	char String[16];
	char Contact[16];
	uint8_t i;

	memset(gFrameBuffer, 0, sizeof(gFrameBuffer));


	UI_PrintString(MenuList[gMenuCursor], 0, 127,  0, 11, true);
	for (i = 4; i < 123; i++) {
		gFrameBuffer[1][i] |= 0b10000000; //горизонтальная линия
	}
	NUMBER_ToDigits(gMenuCursor + 1, String);
	UI_DisplaySmallDigits(2, String + 6, 4, 6);
	if (gIsInSubMenu) {
		memcpy(gFrameBuffer[3], BITMAP_MenuIndicator, sizeof(BITMAP_MenuIndicator));
	}
	else  {
		memcpy(gFrameBuffer[1], BITMAP_MenuIndicator, sizeof(BITMAP_MenuIndicator));		
	}

	memset(String, 0, sizeof(String));

	switch (gMenuCursor) {
	case MENU_SQL:
	case MENU_MIC:
		sprintf(String, "%d", gSubMenuSelection);
		break;

	case MENU_STEP:
		sprintf(String, "%d.%02dKHz", gSubMenu_Step[gSubMenuSelection] / 100, gSubMenu_Step[gSubMenuSelection] % 100);
		break;

	case MENU_TXP:
		strcpy(String, gSubMenu_TXP[gSubMenuSelection]);
		break;

	case MENU_R_DCS:
	case MENU_T_DCS:
		if (gSubMenuSelection == 0) {
			strcpy(String, "Off");
		} else if (gSubMenuSelection < 105) {
			sprintf(String, "D%03oN", DCS_Options[gSubMenuSelection - 1]);
		} else {
			sprintf(String, "D%03oI", DCS_Options[gSubMenuSelection - 105]);
		}
		break;

	case MENU_R_CTCS:
	case MENU_T_CTCS:
		if (gSubMenuSelection == 0) {
			strcpy(String, "Off");
		} else {
			sprintf(String, "%d.%dHz", CTCSS_Options[gSubMenuSelection - 1] / 10, CTCSS_Options[gSubMenuSelection - 1] % 10);
		}
		break;

	case MENU_SFT_D:
		strcpy(String, gSubMenu_SFT_D[gSubMenuSelection]);
		break;

	case MENU_OFFSET:
		if (!gIsInSubMenu || gInputBoxIndex == 0) {
			sprintf(String, "%d.%05d", gSubMenuSelection / 100000, gSubMenuSelection % 100000);
			break;
		}
		for (i = 0; i < 3; i++) {
			if (gInputBox[i] == 10) {
				String[i] = '-';
			} else {
				String[i] = gInputBox[i] + '0';
			}
		}
		String[3] = '.';
		for (i = 3; i < 6; i++) {
			if (gInputBox[i] == 10) {
				String[i + 1] = '-';
			} else {
				String[i + 1] = gInputBox[i] + '0';
			}
		}
		String[7] = '-';
		String[8] = '-';
		String[9] = 0;
		String[10] = 0;
		String[11] = 0;
		break;

	case MENU_W_N:
		strcpy(String, gSubMenu_W_N[gSubMenuSelection]);
		break;

	case MENU_SCR:
	case MENU_VOX:
	case MENU_ABR:
		if (gSubMenuSelection == 0) {
			strcpy(String, "Off");
		} else {
			sprintf(String, "%d", gSubMenuSelection);
		}
		break;

	case MENU_BCL:
	case MENU_BEEP:
	case MENU_AUTOLK:
	case MENU_S_ADD1:
	case MENU_S_ADD2:
	case MENU_STE:
	case MENU_D_ST:
	case MENU_D_DCD:
	case MENU_AM:
	case MENU_ALL_TX:
	case MENU_200TX:
	case MENU_500TX:
	case MENU_350EN:
	case MENU_SCREN:
		strcpy(String, gSubMenu_OFF_ON[gSubMenuSelection]);
		break;
    case MENU_ABOUT:
		strcpy(String, gSubMenu_ABOUT);
		break; 
	case MENU_MEM_CH:
	case MENU_1_CALL:
	case MENU_DEL_CH:
		UI_GenerateChannelStringEx(
			String,
			RADIO_CheckValidChannel((uint16_t)gSubMenuSelection, false, 0),
			(uint8_t)gSubMenuSelection
			);
		break;

	case MENU_SAVE:
		strcpy(String, gSubMenu_SAVE[gSubMenuSelection]);
		break;

	case MENU_TDR:
	case MENU_WX:
		strcpy(String, gSubMenu_CHAN[gSubMenuSelection]);
		break;

	case MENU_TOT:
		if (gSubMenuSelection == 0) {
			strcpy(String, "Off");
		} else {
			sprintf(String, "%dmin", gSubMenuSelection);
		}
		break;

	case MENU_SC_REV:
		strcpy(String, gSubMenu_SC_REV[gSubMenuSelection]);
		break;

	case MENU_MDF:
		strcpy(String, gSubMenu_MDF[gSubMenuSelection]);
		break;

	case MENU_RP_STE:
		if (gSubMenuSelection == 0) {
			strcpy(String, "Off");
		} else {
			sprintf(String, "%d*100ms", gSubMenuSelection);
		}
		break;

	case MENU_S_LIST:
		sprintf(String, "List %d", gSubMenuSelection);
		break;

	case MENU_LOCK_TYPE:
		sprintf(String, gSubMenu_LOCK_TYPE[gSubMenuSelection]);
		break;

	case MENU_ANI_ID:
		strcpy(String, gEeprom.ANI_DTMF_ID);
		break;

	case MENU_UPCODE:
		strcpy(String, gEeprom.DTMF_UP_CODE);
		break;

	case MENU_DWCODE:
		strcpy(String, gEeprom.DTMF_DOWN_CODE);
		break;

	case MENU_D_RSP:
		strcpy(String, gSubMenu_D_RSP[gSubMenuSelection]);
		break;

	case MENU_D_HOLD:
		sprintf(String, "%ds", gSubMenuSelection);
		break;

	case MENU_D_PRE:
		sprintf(String, "%d*10ms", gSubMenuSelection);
		break;

	case MENU_PTT_ID:
		strcpy(String, gSubMenu_PTT_ID[gSubMenuSelection]);
		break;

	case MENU_D_LIST:
		gIsDtmfContactValid = DTMF_GetContact((uint8_t)gSubMenuSelection - 1, Contact);
		if (!gIsDtmfContactValid) {
			// Ghidra being weird again...
			memcpy(String, "NULL\0\0\0", 8);
		} else {
			memcpy(String, Contact, 8);
		}
		break;

	case MENU_ROGER:
		strcpy(String, gSubMenu_ROGER[gSubMenuSelection]);
		break;

	case MENU_VOL:
		sprintf(String, "%d.%02dV", gBatteryVoltageAverage / 100, gBatteryVoltageAverage % 100);
		break;
		
	case MENU_F1_SHORT:
    case MENU_F1_LONG:
	case MENU_F2_SHORT:
	case MENU_F2_LONG:
		strcpy(String, gSubMenu_FUNCTIONS[gSubMenuSelection]);
		break;

	case MENU_RESET:
		strcpy(String, gSubMenu_RESET[gSubMenuSelection]);
		break;

	case MENU_F_LOCK:
		strcpy(String, gSubMenu_F_LOCK[gSubMenuSelection]);
		break;
	
	case MENU_CALIBRATION: 
                sprintf(String, "%d.%02dV", gBatteryVoltageAverage / 100,
                        gBatteryVoltageAverage % 100);
                break;
	}

	UI_PrintString(String, 0, 127, 2, 10, true);

	if (gMenuCursor == MENU_OFFSET) {
		UI_PrintString("MHz", 0, 127, 4, 10, true);
	}
	if (gMenuCursor == MENU_ABOUT) {
		UI_PrintString(Version, 0, 127, 4, 10, true);
	}
	if ((gMenuCursor == MENU_RESET || gMenuCursor == MENU_MEM_CH || gMenuCursor == MENU_DEL_CH) && gAskForConfirmation) {
		if (gAskForConfirmation == 1) {
			strcpy(String, "SURE?");
		} else {
			strcpy(String, "WAIT!");
		}
		UI_PrintString(String, 0, 127, 4, 8, true);
	}

	if ((gMenuCursor == MENU_R_CTCS || gMenuCursor == MENU_R_DCS) && gCssScanMode != CSS_SCAN_MODE_OFF) {
		UI_PrintString("SCAN", 0, 127, 4, 8, true);
	}

	if (gMenuCursor == MENU_UPCODE) {
		if (strlen(gEeprom.DTMF_UP_CODE) > 8) {
			UI_PrintString(gEeprom.DTMF_UP_CODE + 8, 50, 127, 4, 8, true);
		}
	}
	if (gMenuCursor == MENU_DWCODE) {
		if (strlen(gEeprom.DTMF_DOWN_CODE) > 8) {
			UI_PrintString(gEeprom.DTMF_DOWN_CODE + 8, 0, 127, 4, 8, true);
		}
	}
	if (gMenuCursor == MENU_D_LIST && gIsDtmfContactValid) {
		Contact[11] = 0;
		memcpy(&gDTMF_ID, Contact + 8, 4);
		sprintf(String, "ID:%s", Contact + 8);
		UI_PrintString(String, 0, 127, 4, 8, true);
	}

	if (gMenuCursor == MENU_R_CTCS || gMenuCursor == MENU_T_CTCS ||
		gMenuCursor == MENU_R_DCS || gMenuCursor == MENU_T_DCS || gMenuCursor == MENU_D_LIST) {
			uint8_t Offset;

			NUMBER_ToDigits((uint8_t)gSubMenuSelection, String);
			Offset = (gMenuCursor == MENU_D_LIST) ? 2 : 3;
			UI_DisplaySmallDigits(Offset, String + (8 - Offset), 105, 6);
	}

	if (gMenuCursor == MENU_SLIST1 || gMenuCursor == MENU_SLIST2) {
		i = gMenuCursor - MENU_SLIST1;

		if (gSubMenuSelection == 0xFF) {
			sprintf(String, "NULL");
		} else {
			UI_GenerateChannelStringEx(String, true, (uint8_t)gSubMenuSelection);
		}

		if (gSubMenuSelection == 0xFF || !gEeprom.SCAN_LIST_ENABLED[i]) {
			UI_PrintString(String, 0, 127, 2, 8, 1);
		} else {
			UI_PrintString(String, 0, 127, 0, 8, 1);
			if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH1[i])) {
				sprintf(String, "PRI1:%d", gEeprom.SCANLIST_PRIORITY_CH1[i] + 1);
				UI_PrintString(String, 0, 127, 2, 8, 1);
			}
			if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH2[i])) {
				sprintf(String, "PRI2:%d", gEeprom.SCANLIST_PRIORITY_CH2[i] + 1);
				UI_PrintString(String, 0, 127, 4, 8, 1);
			}
		}
	}

	ST7565_BlitFullScreen();
}

