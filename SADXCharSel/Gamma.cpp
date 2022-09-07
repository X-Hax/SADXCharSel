#include "stdafx.h"

static FunctionHook<void, task*> EggViper_t((intptr_t)EggViper_Main);
static FunctionHook<void, task*> sub_580E70_t((intptr_t)sub_580E70);
static FunctionHook<void, task*>  Egm3Sippo_t((intptr_t)EGM3Sippo);
static FunctionHook<void, task*> sub_580A90_t((intptr_t)sub_580A90);

static FunctionHook<void, task*>  Chaos0_t((intptr_t)Chaos0_Main);
static FunctionHook<void, task*> Chaos2_t((intptr_t)Chaos2_Main);
static FunctionHook<void, task*>  EggHornet_t((intptr_t)EggHornet_Main);

//Delete gamma shot on target
void Remove_Cursor(task* obj) {

	auto data = obj->twp;

	if (data) {
		if (data->flag & Status_Hurt) {
			E102KillCursor(obj);
		}
	}
}

//Allow gamma to target object
void Check_AllocateEnemyInit(task* obj, taskwk* data1)
{
	if (!obj || !data1)
		return;

	//if one of the player is gamma, init the target thing
	for (int i = 0; i < 8; i++) {

		if (!playertwp[i])
			continue;

		if (EntityData1Ptrs[i]->CharID == Characters_Gamma)
		{
			if (!data1->mode)
			{
				EnemyInitialize(obj, data1);
				EnemyPreserveHomePosition(data1, (enemywk*)obj->mwp);
			}

			Remove_Cursor(obj);
		}
	}
}

//Viper stuff
void sub_580A90_r(task* obj) {

	Remove_Cursor(obj);
	sub_580A90_t.Original(obj);
}

void Egm3Sippo_r(task* obj) {

	Remove_Cursor(obj);
	Egm3Sippo_t.Original(obj);
}

void sub_580E70_r(task* obj) {

	Remove_Cursor(obj);
	sub_580E70_t.Original(obj);
}

void EggViper_Main_r(task* obj) {

	Remove_Cursor(obj);
	EggViper_t.Original(obj);
}

void FixGammaBounce(unsigned __int8 playerID, float speedX, float speedY, float speedZ) {

	if (playertwp[playerID]) {

		if (playertwp[playerID]->counter.b[1] == Characters_Gamma)
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
	EggViper_t.Hook(EggViper_Main_r);

	sub_580E70_t.Hook(sub_580E70_r);
	Egm3Sippo_t.Hook(Egm3Sippo_r);
	sub_580A90_t.Hook(sub_580A90_r);
}


void Chaos2_Main_R(task* obj) {

	auto data1 = obj->twp;

	Check_AllocateEnemyInit(obj, data1);

	Chaos2_t.Original(obj);
}

void Chaos0_Main_R(task* obj) {

	auto data1 = obj->twp;
	Check_AllocateEnemyInit(obj, data1);

	Chaos0_t.Original(obj);
}

void __cdecl Chaos_Init()
{
	Chaos0_t.Hook(Chaos0_Main_R);
	Chaos2_t.Hook(Chaos2_Main_R);
}

void EggHornet_Main_R(task* obj) {

	auto data1 = obj->twp;
	Check_AllocateEnemyInit(obj, data1);

	EggHornet_t.Original(obj);
}


extern __int16 selectedcharacter[PLAYER_COUNT];

void Init_GammaFixes() {

	EggViper_GammaFixes();
	Chaos_Init();

	EggHornet_t.Hook(EggHornet_Main_R);

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