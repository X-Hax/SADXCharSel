// SADXCharSel.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\sadx-mod-loader\SADXModLoader\include\SADXModLoader.h"


extern "C"
{
	const int loc_414914 = 0x414914;
	__declspec(naked) void ChangeStartPosCharLoading()
	{
		__asm
		{
			mov eax, [CurrentCharacter]
			movzx eax, word ptr[eax]
			cmp eax, 7
			jmp loc_414914
		}
	}

	ObjectFuncPtr charfuncs[] = {
		Sonic_Main,
		Eggman_Main,
		Tails_Main,
		Knuckles_Main,
		Tikal_Main,
		Amy_Main,
		Gamma_Main,
		Big_Main
	};

	int selectedcharacter = -1;

	FunctionPointer(int, sub_42FB00, (), 0x42FB00);
	void LoadCharacter_r()
	{
		ClearPlayerArrays();
		int character = selectedcharacter == -1 ? CurrentCharacter : selectedcharacter;
		ObjectMaster *obj;
		if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
		{
			obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tornado_Main);
			obj->Data1->CharID = (char)CurrentCharacter;
		}
		else
		{
			int btn = ControllerPointers[0]->HeldButtons;
			if (btn & Buttons_Left)
			{
				character = Characters_Sonic;
				MetalSonicFlag = 0;
			}
			if (btn & Buttons_B)
			{
				character = Characters_Sonic;
				MetalSonicFlag = 1;
			}
			if (btn & Buttons_X)
				character = Characters_Eggman;
			if (btn & Buttons_R)
				character = Characters_Tails;
			if (btn & Buttons_Down)
				character = Characters_Knuckles;
			if (btn & Buttons_Y)
				character = Characters_Tikal;
			if (btn & Buttons_Right)
				character = Characters_Amy;
			if (btn & Buttons_L)
				character = Characters_Gamma;
			if (btn & Buttons_Up)
				character = Characters_Big;
			obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, charfuncs[character]);
			obj->Data1->CharID = character;
		}
		obj->Data1->CharIndex = 0;
		CharObj1Ptrs[0] = obj->Data1;
		CharObj2Ptrs[0] = (CharObj2*)obj->Data2;
		PutPlayerAtStartPointIGuess(obj->Data1);
		if (!CurrentCharacter && GameMode != GameModes_Mission && !MetalSonicFlag)
			Load2PTails(obj);
		switch (CurrentCharacter)
		{
		case Characters_Tails:
			LoadTailsOpponent(CurrentCharacter, 1, CurrentLevel);
			break;
		case Characters_Knuckles:
			if (sub_42FB00() != 1
				&& (GameMode == GameModes_Adventure_ActionStg
				|| GameMode == GameModes_Mission
				|| GameMode == GameModes_Trial))
			{
				LoadObject(LoadObj_Data1, 6, EmeraldRadarHud_Load_Load);
			}
			break;
		case Characters_Gamma:
			LoadChildObject(LoadObj_Data1, (ObjectFuncPtr)0x4C51D0, obj); // doesn't work?
			break;
		case Characters_Big:
			LoadObject(LoadObj_Data1, 6, BigHud_Main);
			break;
		}
		selectedcharacter = character;
	}

	const int loc_512BC6 = 0x512BC6;
	__declspec(naked) void ResetSelectedCharacter()
	{
		__asm
		{
			mov [selectedcharacter],-1
			push 0x512B40
			jmp loc_512BC6
		}
	}

	DataPointer(char, TimeSeconds, 0x3B0F128);
	DataPointer(char, TimeMinutes, 0x3B0EF48);
	void __cdecl SubtractSeconds(int seconds)
	{
		if (CurrentCharacter == Characters_Gamma)
		{
			AddSeconds(seconds);
			return;
		}
		int v1; // eax@1
		char v2; // cl@2

		v1 = TimeSeconds - seconds;
		if (v1  < 0)
		{
			v2 = TimeMinutes;
			while (1)
			{
				v1 += 60;
				TimeMinutes = --v2;
				if (v2 < 0)
				{
					break;
				}
				if (v1 > 0)
				{
					TimeSeconds = v1;
					return;
				}
			}
			TimeMinutes = 0;
			v1 = 0;
		}
		TimeSeconds = v1;
	}

	int CheckLoadTimeRemainHUD()
	{
		return CurrentCharacter != Characters_Gamma || sub_42FB00();
	}

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);
		WriteJump((void*)0x512BC1, ResetSelectedCharacter);
		WriteJump((void*)0x490C6B, (void*)0x490C80); // prevent Big from automatically loading Big's HUD
		WriteCall((void*)0x49FD54, SubtractSeconds);
		WriteCall((void*)0x4A0150, SubtractSeconds);
		WriteCall((void*)0x4A0198, SubtractSeconds);
		WriteCall((void*)0x483760, CheckLoadTimeRemainHUD);
		WriteJump((void*)0x47A907, (void*)0x47A936); // prevent Knuckles from automatically loading Emerald radar
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}