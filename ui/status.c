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
#include "misc.h"
#include "settings.h"
#include "ui/status.h"
#include "ui/helper.h"
#include "font.h"

void UI_DisplayStatusbarDigits(uint8_t Size, const char *pString, uint8_t x)
{
	uint8_t i;

	for (i = 0; i < Size; i++) {
		memcpy(gStatusLine + (i * 7) + x, gFontSmallDigits[(uint8_t)pString[i]], 7);
	}
}

void UI_DisplayStatus(void)
{
	char String[5];
	memset(gStatusLine, 0, sizeof(gStatusLine));
	if (gCurrentFunction == FUNCTION_POWER_SAVE) {
		memcpy(gStatusLine, BITMAP_PowerSave, sizeof(BITMAP_PowerSave));
	}
	if (gBatteryDisplayLevel < 2) {
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
	}
	if (gChargingWithTypeC) {
		memcpy(gStatusLine + 100, BITMAP_USB_C, sizeof(BITMAP_USB_C));
	}
	if (gEeprom.KEY_LOCK) {
		memcpy(gStatusLine + 90, BITMAP_KeyLock, sizeof(BITMAP_KeyLock));
	} else if (gWasFKeyPressed) {
		memcpy(gStatusLine + 90, BITMAP_F_Key, sizeof(BITMAP_F_Key));
	}

	if (gEeprom.VOX_SWITCH) {
		memcpy(gStatusLine + 71, BITMAP_VOX, sizeof(BITMAP_VOX));
	}
	if (gEeprom.CROSS_BAND_RX_TX != CROSS_BAND_OFF) {
		memcpy(gStatusLine + 58, BITMAP_WX, sizeof(BITMAP_WX));
	}
	if (gEeprom.DUAL_WATCH != DUAL_WATCH_OFF) {
		memcpy(gStatusLine + 45, BITMAP_TDR, sizeof(BITMAP_TDR));
	}
	memset (String, 0x06, sizeof(String));
    UI_DisplayStatusbarDigits(5, String, 20);
	ST7565_BlitStatusLine();

}

