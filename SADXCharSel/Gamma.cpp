#include "stdafx.h"

Trampoline* EggViper_t;
Trampoline* sub_580E70_t;
Trampoline* Egm3Sippo_t;
Trampoline* sub_580A90_t;
Trampoline* Chaos0_t;
Trampoline* Chaos2_t;


void Remove_Cursor(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;

	if (data) {
		if (data->Status & Status_Hurt) {
			E102KillCursor((ObjectMaster*)obj);
		}
	}
}

void sub_580A90_r(ObjectMaster* obj) {

	Remove_Cursor(obj);

	ObjectFunc(origin, sub_580A90_t->Target());
	origin(obj);
}

void Egm3Sippo_r(ObjectMaster* obj) {

	Remove_Cursor(obj);

	ObjectFunc(origin, Egm3Sippo_t->Target());
	origin(obj);
}

void sub_580E70_r(ObjectMaster* obj) {

	Remove_Cursor(obj);

	ObjectFunc(origin, sub_580E70_t->Target());
	origin(obj);
}

void EggViper_Main_r(ObjectMaster* obj) {

	Remove_Cursor(obj);

	ObjectFunc(origin, EggViper_t->Target());
	origin(obj);
}

void FixGammaBounce(unsigned __int8 playerID, float speedX, float speedY, float speedZ) {

	if (EntityData1Ptrs[playerID]) {

		if (EntityData1Ptrs[playerID]->CharID == Characters_Gamma)
			return;
	}

	return EnemyBounceThing(playerID, speedX, speedY, speedZ);
}


void EggViper_GammaFixes() {

	//Fix infinite Gamma bounce on Egg Viper Fight.
	WriteCall((void*)0x584430, FixGammaBounce);
	WriteCall((void*)0x580bed, FixGammaBounce);
	WriteCall((void*)0x580c7e, FixGammaBounce);
	WriteCall((void*)0x580d2c, FixGammaBounce);
	WriteCall((void*)0x580F27, FixGammaBounce);

	//remove gamma missile on Viper after a hit
	EggViper_t = new Trampoline((int)EggViper_Main, (int)EggViper_Main + 0x5, EggViper_Main_r);
	sub_580E70_t = new Trampoline((int)sub_580E70, (int)sub_580E70 + 0x5, sub_580E70_r);
	Egm3Sippo_t = new Trampoline((int)EGM3Sippo, (int)EGM3Sippo + 0x8, Egm3Sippo_r);
	sub_580A90_t = new Trampoline((int)sub_580A90, (int)sub_580A90 + 0x8, sub_580A90_r);
}



extern __int16 selectedcharacter[PLAYER_COUNT];

void Init_GammaFixes() {

	EggViper_GammaFixes();


	WriteCall((void*)0x426005, GetCharacter0ID); // fix ResetTime() for Gamma
	WriteCall((void*)0x427F2B, GetCharacter0ID); // fix ResetTime2() for Gamma
	WriteData((char*)0x41486D, (char)0xEB); // fix time reset at level load for Gamma
	WriteData((__int16**)0x414A0C, &selectedcharacter[0]); // fix 1min minimum at level restart for Gamma
	WriteCall((void*)0x426081, GetCharacter0ID); // fix Gamma's timer
	WriteCall((void*)0x4266C9, GetCharacter0ID); // fix Gamma's time bonus
	WriteCall((void*)0x426379, GetCharacter0ID); // fix Gamma's time display
	WriteCall((void*)0x4D677C, GetCharacter0ID); // fix item boxes for Gamma
	WriteCall((void*)0x4C06D9, GetCharacter0ID); // fix floating item boxes for Gamma
}