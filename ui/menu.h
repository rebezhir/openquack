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

#ifndef UI_MENU_H
#define UI_MENU_H

#include <stdbool.h>
#include <stdint.h>

enum {
	MENU_SQL		= 0,
	MENU_STEP		= 1,
	MENU_TXP		= 2,
	MENU_R_DCS		= 3,
	MENU_R_CTCS		= 4,
	MENU_T_DCS		= 5,
	MENU_T_CTCS		= 6,
	MENU_SFT_D		= 7,
	MENU_OFFSET		= 8,
	MENU_W_N		= 9,
	MENU_SCR		= 10,
	MENU_BCL		= 11,
	MENU_MEM_CH		= 12,
	MENU_SAVE		= 13,
	MENU_VOX		= 14,
	MENU_ABR		= 15,
	MENU_TDR		= 16,
	MENU_WX			= 17,
	MENU_BEEP		= 18,
	MENU_TOT		= 19,
	MENU_SC_REV		= 20,
	MENU_MDF		= 21,
	MENU_AUTOLK		= 22,
	MENU_S_ADD1		= 23,
	MENU_S_ADD2		= 24,
	MENU_STE		= 25,
	MENU_RP_STE		= 26,
	MENU_MIC		= 27,
	MENU_1_CALL		= 28,
	MENU_S_LIST		= 29,
	MENU_SLIST1		= 30,
	MENU_SLIST2		= 31,
	MENU_AUTO_1750	= 32,
	MENU_ANI_ID		= 33,
	MENU_UPCODE		= 34,
	MENU_DWCODE		= 35,
	MENU_D_ST		= 36,
	MENU_D_RSP		= 37,
	MENU_D_HOLD		= 38,
	MENU_D_PRE		= 39,
	MENU_PTT_ID		= 40,
	MENU_D_DCD		= 41,
	MENU_D_LIST		= 42,
	MENU_ROGER		= 43,
	MENU_VOL		= 44,
	MENU_AM			= 45,
	MENU_DEL_CH		= 46,
	MENU_F1_SHORT	= 47,
	MENU_F1_LONG	= 48,
	MENU_F2_SHORT	= 49,
	MENU_F2_LONG	= 50,
	MENU_RESET		= 51,
	MENU_ABOUT      = 52,
	MENU_ALL_TX		= 53,
	MENU_F_LOCK		= 54,
	MENU_200TX		= 55,
	MENU_500TX		= 56,
	MENU_350EN		= 57,
	MENU_SCREN		= 58,
};

extern bool gIsInSubMenu;

extern uint8_t gMenuCursor;
extern int8_t gMenuScrollDirection;
extern uint32_t gSubMenuSelection;

void UI_DisplayMenu(void);

#endif

