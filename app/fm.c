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
#include "app/action.h"
#include "app/fm.h"
#include "app/generic.h"
#include "audio.h"
#include "bsp/dp32g030/gpio.h"
#include "driver/bk1080.h"
#include "driver/eeprom.h"
#include "driver/gpio.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/inputbox.h"
#include "ui/ui.h"

uint16_t gFM_Channels[20];
bool gFmRadioMode;
uint8_t gFmRadioCountdown;
volatile uint16_t gFmPlayCountdown;
volatile int8_t gFM_ScanState;
bool gFM_AutoScan;
uint8_t gFM_ChannelPosition;
bool gFM_FoundFrequency;
bool gFM_AutoScan;
uint8_t gFM_ResumeCountdown;
uint16_t gFM_RestoreCountdown;

bool FM_CheckValidChannel(uint8_t Channel)
{
	if (Channel < 20 && (gFM_Channels[Channel] >= 760 && gFM_Channels[Channel] < 1080)) {
		return true;
	}

	return false;
}

uint8_t FM_FindNextChannel(uint8_t Channel, uint8_t Direction)
{
	uint8_t i;

	for (i = 0; i < 20; i++) {
		Channel %= 20;
		if (FM_CheckValidChannel(Channel)) {
			return Channel;
		}
		Channel += Direction;
	}

	return 0xFF;
}

int FM_ConfigureChannelState(void)
{
	uint8_t Channel;

	gEeprom.FM_FrequencyPlaying = gEeprom.FM_SelectedFrequency;
	if (gEeprom.FM_IsMrMode) {
		Channel = FM_FindNextChannel(gEeprom.FM_SelectedChannel, FM_CHANNEL_UP);
		if (Channel == 0xFF) {
			gEeprom.FM_IsMrMode = false;
			return -1;
		}
		gEeprom.FM_SelectedChannel = Channel;
		gEeprom.FM_FrequencyPlaying = gFM_Channels[Channel];
	}

	return 0;
}

void FM_TurnOff(void)
{
	gFmRadioMode = false;
	gFM_ScanState = FM_SCAN_OFF;
	gFM_RestoreCountdown = 0;
	GPIO_ClearBit(&GPIOC->DATA, GPIOC_PIN_AUDIO_PATH);
	gEnableSpeaker = false;
	BK1080_Init(0, false);
	gUpdateStatus = true;
}

void FM_EraseChannels(void)
{
	uint8_t i;
	uint8_t Template[8];

	memset(Template, 0xFF, sizeof(Template));
	for (i = 0; i < 5; i++) {
		EEPROM_WriteBuffer(0x0E40 + (i * 8), Template);
	}

	memset(gFM_Channels, 0xFF, sizeof(gFM_Channels));
}

void FM_Tune(uint16_t Frequency, int8_t Step, bool bFlag)
{
	GPIO_ClearBit(&GPIOC->DATA, GPIOC_PIN_AUDIO_PATH);
	gEnableSpeaker = false;
	if (gFM_ScanState == FM_SCAN_OFF) {
		gFmPlayCountdown = 120;
	} else {
		gFmPlayCountdown = 10;
	}
	gScheduleFM = false;
	gFM_FoundFrequency = false;
	gAskToSave = false;
	gAskToDelete = false;
	gEeprom.FM_FrequencyPlaying = Frequency;
	if (!bFlag) {
		Frequency += Step;
		if (Frequency < gEeprom.FM_LowerLimit) {
			Frequency = gEeprom.FM_UpperLimit;
		} else if (Frequency > gEeprom.FM_UpperLimit) {
			Frequency = gEeprom.FM_LowerLimit;
		}
		gEeprom.FM_FrequencyPlaying = Frequency;
	}

	gFM_ScanState = Step;
	BK1080_SetFrequency(gEeprom.FM_FrequencyPlaying);
}

void FM_PlayAndUpdate(void)
{
	gFM_ScanState = FM_SCAN_OFF;
	if (gFM_AutoScan) {
		gEeprom.FM_IsMrMode = true;
		gEeprom.FM_SelectedChannel = 0;
	}
	FM_ConfigureChannelState();
	BK1080_SetFrequency(gEeprom.FM_FrequencyPlaying);
	SETTINGS_SaveFM();
	gFmPlayCountdown = 0;
	gScheduleFM = false;
	gAskToSave = false;
	GPIO_SetBit(&GPIOC->DATA, GPIOC_PIN_AUDIO_PATH);
	gEnableSpeaker = true;
}

int FM_CheckFrequencyLock(uint16_t Frequency, uint16_t LowerLimit)
{
	uint16_t Test2;
	uint16_t Deviation;
	int ret = -1;

	Test2 = BK1080_ReadRegister(BK1080_REG_07);
	// This is supposed to be a signed value, but above function is unsigned
	Deviation = BK1080_REG_07_GET_FREQD(Test2);

	if (BK1080_REG_07_GET_SNR(Test2) >= 2) {
		uint16_t Status;

		Status = BK1080_ReadRegister(BK1080_REG_10);
		if ((Status & BK1080_REG_10_MASK_AFCRL) == BK1080_REG_10_AFCRL_NOT_RAILED && BK1080_REG_10_GET_RSSI(Status) >= 10) {
			// if (Deviation > -281 && Deviation < 280)
			if (Deviation < 280 || Deviation > 3815) {
				// not BLE(less than or equal)
				if (Frequency > LowerLimit && (Frequency - BK1080_BaseFrequency) == 1) {
					if (BK1080_FrequencyDeviation & 0x800) {
						goto Bail;
					}
					if (BK1080_FrequencyDeviation < 20) {
						goto Bail;
					}
				}
				// not BLT(less than)
				if (Frequency >= LowerLimit && (BK1080_BaseFrequency - Frequency) == 1) {
					if ((BK1080_FrequencyDeviation & 0x800) == 0) {
						goto Bail;
					}
					// if (BK1080_FrequencyDeviation > -21) {
					if (BK1080_FrequencyDeviation > 4075) {
						goto Bail;
					}
				}
				ret = 0;
			}
		}
	}

Bail:
	BK1080_FrequencyDeviation = Deviation;
	BK1080_BaseFrequency = Frequency;

	return ret;
}


void FM_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	
}

void FM_Play(void)
{
	if (!FM_CheckFrequencyLock(gEeprom.FM_FrequencyPlaying, gEeprom.FM_LowerLimit)) {
		if (!gFM_AutoScan) {
			gFmPlayCountdown = 0;
			gFM_FoundFrequency = true;
			if (!gEeprom.FM_IsMrMode) {
				gEeprom.FM_SelectedFrequency = gEeprom.FM_FrequencyPlaying;
			}
			GPIO_SetBit(&GPIOC->DATA, GPIOC_PIN_AUDIO_PATH);
			gEnableSpeaker = true;
			GUI_SelectNextDisplay(DISPLAY_FM);
			return;
		}
		if (gFM_ChannelPosition < 20) {
			gFM_Channels[gFM_ChannelPosition++] = gEeprom.FM_FrequencyPlaying;
		}
		if (gFM_ChannelPosition >= 20) {
			FM_PlayAndUpdate();
			GUI_SelectNextDisplay(DISPLAY_FM);
			return;
		}
	}

	if (gFM_AutoScan && gEeprom.FM_FrequencyPlaying >= gEeprom.FM_UpperLimit) {
		FM_PlayAndUpdate();
	} else {
		FM_Tune(gEeprom.FM_FrequencyPlaying, gFM_ScanState, false);
	}

	GUI_SelectNextDisplay(DISPLAY_FM);
}

void FM_Start(void)
{
	gFmRadioMode = true;
	gFM_ScanState = FM_SCAN_OFF;
	gFM_RestoreCountdown = 0;
	BK1080_Init(gEeprom.FM_FrequencyPlaying, true);
	GPIO_SetBit(&GPIOC->DATA, GPIOC_PIN_AUDIO_PATH);
	gEnableSpeaker = true;
	gUpdateStatus = true;
}

