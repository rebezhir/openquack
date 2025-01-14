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

#include "bitmaps.h"
#include "driver/keyboard.h"
#include "driver/st7565.h"
#include "functions.h"
#include "helper/battery.h"
#include "external/printf/printf.h"
#include "misc.h"
#include "settings.h"
#include "ui/status.h"
#include "ui/helper.h"
#include "win1251/font8x7_ASCIIpart.h"
#include "win1251/font8x7_cyrillic.h"

char StatusbarString[10];

void UI_DisplayStatusbarString(uint8_t x)
{
	uint8_t i = 0;

	while (StatusbarString[i] && i<10) {
		if (is_ASCIIpart(StatusbarString[i]))
		{
			memcpy(gStatusLine + (i * 7) + x, gFont8x7_ASCIIpart[(uint8_t)StatusbarString[i]-0x20], 7);
		}
		else if (is_cyrillic(StatusbarString[i]))
		{
			memcpy(gStatusLine + (i * 7) + x, gFont8x7_cyrillic[(uint8_t)StatusbarString[i]-0xC0], 7);
		}
		i++;
	}
}


void UI_DisplayStatus(void)
{
	
	memset(gStatusLine, 0, sizeof(gStatusLine));
	if (gCurrentFunction == FUNCTION_POWER_SAVE) {
		memcpy(gStatusLine, BITMAP_PowerSave, sizeof(BITMAP_PowerSave));
	}
	/*if (gBatteryDisplayLevel < 2) {
		if (gLowBatteryBlink == 1) {
			memcpy(gStatusLine + 110, BITMAP_BatteryLevel1, sizeof(BITMAP_BatteryLevel1));
		}
	} else {
		if (gBatteryDisplayLevel == 2) {
			memcpy(gStatusLine + 110, BITMAP_BatteryLevel2, sizeof(BITMAP_BatteryLevel2));
		} else if (gBatteryDisplayLevel == 3) {
			memcpy(gStatusLine + 110, BITMAP_BatteryLevel3, sizeof(BITMAP_BatteryLevel3));
		} else if (gBatteryDisplayLevel == 4) {
			memcpy(gStatusLine + 110, BITMAP_BatteryLevel4, sizeof(BITMAP_BatteryLevel4));
		} else {
			memcpy(gStatusLine + 110, BITMAP_BatteryLevel5, sizeof(BITMAP_BatteryLevel5));
		}
	}*/
	
	if (gChargingWithTypeC) {
		memcpy(gStatusLine + 80, BITMAP_USB_C, sizeof(BITMAP_USB_C));
	}
	if (gEeprom.KEY_LOCK) {
		memcpy(gStatusLine + 70, BITMAP_KeyLock, sizeof(BITMAP_KeyLock));
	} else if (gWasFKeyPressed) {
		memcpy(gStatusLine + 70, BITMAP_F_Key, sizeof(BITMAP_F_Key));
	}

	if (gEeprom.VOX_SWITCH) {
		memcpy(gStatusLine + 51, BITMAP_VOX, sizeof(BITMAP_VOX));
	}
	if (gEeprom.CROSS_BAND_RX_TX != CROSS_BAND_OFF) {
		memcpy(gStatusLine + 38, BITMAP_WX, sizeof(BITMAP_WX));
	}
	if (gEeprom.DUAL_WATCH != DUAL_WATCH_OFF) {
		memcpy(gStatusLine + 25, BITMAP_TDR, sizeof(BITMAP_TDR));
	}
    sprintf(StatusbarString, "%d.%02dV",
                        gBatteryVoltageAverage / 100,
                        gBatteryVoltageAverage % 100);
    UI_DisplayStatusbarString(92);
    ST7565_BlitStatusLine();
        }
