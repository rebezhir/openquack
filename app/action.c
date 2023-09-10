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

#include "app/action.h"

#include "app/app.h"
#include "app/dtmf.h"
#include "app/scanner.h"
#include "audio.h"
#include "bsp/dp32g030/gpio.h"
#include "driver/bk4819.h"
#include "driver/gpio.h"
#include "functions.h"
#include "misc.h"
#include "settings.h"
#include "ui/inputbox.h"
#include "ui/ui.h"

static void ACTION_FlashLight(void) {
    switch (gFlashLightState) {
        case 0:
            gFlashLightState++;
            GPIO_SetBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
            break;
        case 1:
            gFlashLightState++;
            break;
        default:
            gFlashLightState = 0;
            GPIO_ClearBit(&GPIOC->DATA, GPIOC_PIN_FLASHLIGHT);
    }
}

void ACTION_Power(void) {
    if (++gTxVfo->OUTPUT_POWER > OUTPUT_POWER_HIGH) {
        gTxVfo->OUTPUT_POWER = OUTPUT_POWER_LOW;
    }

    gRequestSaveChannel = 1;
    gAnotherVoiceID = VOICE_ID_POWER;
    gRequestDisplayScreen = gScreenToDisplay;
}

static void ACTION_Monitor(void) {
    if (gCurrentFunction != FUNCTION_MONITOR) {
        RADIO_SelectVfos();
        RADIO_SetupRegisters(true);
        APP_StartListening(FUNCTION_MONITOR);
        return;
    }
    if (gScanState != SCAN_OFF) {
        ScanPauseDelayIn10msec = 500;
        gScheduleScanListen = false;
        gScanPauseMode = true;
    }
    RADIO_SetupRegisters(true);
    gRequestDisplayScreen = gScreenToDisplay;

}

void ACTION_Scan(bool bRestart) {
    if (gScreenToDisplay != DISPLAY_SCANNER) {
        RADIO_SelectVfos();
        GUI_SelectNextDisplay(DISPLAY_MAIN);
        if (gScanState != SCAN_OFF) {
            SCANNER_Stop();
            gAnotherVoiceID = VOICE_ID_SCANNING_STOP;
        } else {
            CHANNEL_Next(true, 1);
            AUDIO_SetVoiceID(0, VOICE_ID_SCANNING_BEGIN);
            AUDIO_PlaySingleVoice(true);
        }
    }
}

void ACTION_Vox(void) {
    gEeprom.VOX_SWITCH = !gEeprom.VOX_SWITCH;
    gRequestSaveSettings = true;
    gFlagReconfigureVfos = true;
    gAnotherVoiceID = VOICE_ID_VOX;
    gUpdateStatus = true;
}

static void ACTION_1750(void) {
    gInputBoxIndex = 0;
    gAlarmState = ALARM_STATE_TX1750;
    gAlarmRunningCounter = 0;
    gFlagPrepareTX = true;
    gRequestDisplayScreen = DISPLAY_MAIN;
}

static void ACTION_SwitchAB(void) {
}

static void ACTION_VFOMR(void) {
}

void ACTION_Handle(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld) {
    uint8_t Short;
    uint8_t Long;

    if (gScreenToDisplay == DISPLAY_MAIN && gDTMF_InputMode) {
        if (Key == KEY_SIDE1 && !bKeyHeld && bKeyPressed) {
            gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
            if (gDTMF_InputIndex) {
                gDTMF_InputIndex--;
                gDTMF_InputBox[gDTMF_InputIndex] = '-';
                if (gDTMF_InputIndex) {
                    gPttWasReleased = true;
                    gRequestDisplayScreen = DISPLAY_MAIN;
                    return;
                }
            }
            gAnotherVoiceID = VOICE_ID_CANCEL;
            gRequestDisplayScreen = DISPLAY_MAIN;
            gDTMF_InputMode = false;
        }
        gPttWasReleased = true;
        return;
    }

    if (Key == KEY_SIDE1) {
        Short = gEeprom.KEY_1_SHORT_PRESS_ACTION;
        Long = gEeprom.KEY_1_LONG_PRESS_ACTION;
    } else {
        Short = gEeprom.KEY_2_SHORT_PRESS_ACTION;
        Long = gEeprom.KEY_2_LONG_PRESS_ACTION;
    }
    if (!bKeyHeld && bKeyPressed) {
        gBeepToPlay = BEEP_1KHZ_60MS_OPTIONAL;
        return;
    }
    if (bKeyHeld || bKeyPressed) {
        if (!bKeyHeld) {
            return;
        }
        Short = Long;
        if (!bKeyPressed) {
            return;
        }
    }
    switch (Short) {
        case 1:
            ACTION_FlashLight();
            break;
        case 2:
            ACTION_Power();
            break;
        case 3:
            ACTION_Monitor();
            break;
        case 4:
            ACTION_Scan(true);
            break;
        case 5:
            ACTION_Vox();
            break;
        case 6:
            ACTION_VFOMR();
            break;
        case 7:
            ACTION_SwitchAB();
            break;
        case 8:
            ACTION_1750();
            break;
    }
}
