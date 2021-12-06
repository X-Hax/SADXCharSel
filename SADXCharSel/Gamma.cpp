#include "stdafx.h"

Trampoline* EggViper_t;
Trampoline* sub_580E70_t;
Trampoline* Egm3Sippo_t;
Trampoline* sub_580A90_t;

Trampoline* Chaos0_t;
Trampoline* Chaos2_t;
Trampoline* EggHornet_t;


//Delete gamma shot on target
void Remove_Cursor(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;

	if (data) {
		if (data->Status & Status_Hurt) {
			E102KillCursor((ObjectMaster*)obj);
		}
	}
}

//Allow gamma to target object
void Check_AllocateObjectData2(ObjectMaster* obj, EntityData1* data1)
{
	if (!obj || !data1)
		return;

	//if one of the player is gamma, init the target thing
	for (int i = 0; i < 8; i++) {

		if (!EntityData1Ptrs[i])
			continue;

		if (EntityData1Ptrs[i]->CharID == Characters_Gamma)
		{
			if (!data1->Action)
			{
				AllocateObjectData2(obj, data1);
				ObjectData2_SetStartPosition(obj->Data1, (ObjectData2*)obj->Data2);
			}

			Remove_Cursor(obj);
		}
	}
}

//Viper stuff
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


void Chaos2_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;

	Check_AllocateObjectData2(obj, data1);

	ObjectFunc(origin, Chaos2_t->Target());
	origin(obj);
}

void Chaos0_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	Check_AllocateObjectData2(obj, data1);

	ObjectFunc(origin, Chaos0_t->Target());
	origin(obj);
}


void __cdecl Chaos_Init()
{
	Chaos0_t = new Trampoline((int)Chaos0_Main, (int)Chaos0_Main + 0x7, Chaos0_Main_R);
	Chaos2_t = new Trampoline((int)Chaos2_Main, (int)Chaos2_Main + 0x6, Chaos2_Main_R);
}


void EggHornet_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	Check_AllocateObjectData2(obj, data1);

	ObjectFunc(origin, EggHornet_t->Target());
	origin(obj);
}


extern __int16 selectedcharacter[PLAYER_COUNT];



void Init_GammaFixes() {

	EggViper_GammaFixes();
	Chaos_Init();

	EggHornet_t = new Trampoline((int)EggHornet_Main, (int)EggHornet_Main + 0x7, EggHornet_Main_R);


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