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

	FunctionPointer(void, sub_404A60, (int), 0x404A60);
	void __cdecl SetSelectedCharacter(int arg)
	{
		if (selectedcharacter == -1)
			selectedcharacter = CurrentCharacter;
		int btn = ControllerPointers[0]->HeldButtons;
		if (btn & Buttons_Left)
		{
			selectedcharacter = Characters_Sonic;
			MetalSonicFlag = 0;
		}
		if (btn & Buttons_B)
		{
			selectedcharacter = Characters_Sonic;
			MetalSonicFlag = 1;
		}
		if (btn & Buttons_X)
			selectedcharacter = Characters_Eggman;
		if (btn & Buttons_R)
			selectedcharacter = Characters_Tails;
		if (btn & Buttons_Down)
			selectedcharacter = Characters_Knuckles;
		if (btn & Buttons_Y)
			selectedcharacter = Characters_Tikal;
		if (btn & Buttons_Right)
			selectedcharacter = Characters_Amy;
		if (btn & Buttons_L)
			selectedcharacter = Characters_Gamma;
		if (btn & Buttons_Up)
			selectedcharacter = Characters_Big;
		sub_404A60(arg);
	}

	FunctionPointer(int, sub_42FB00, (), 0x42FB00);
	void LoadCharacter_r()
	{
		ClearPlayerArrays();
		ObjectMaster *obj;
		if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
		{
			obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tornado_Main);
			obj->Data1->CharID = (char)CurrentCharacter;
		}
		else
		{
			obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, charfuncs[selectedcharacter]);
			obj->Data1->CharID = selectedcharacter;
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

	int GetCharacter0ID()
	{
		return GetCharacterID(0);
	}

	int GetSelectedCharacter()
	{
		return selectedcharacter;
	}

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		WriteCall((void*)0x41522C, SetSelectedCharacter);
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);
		WriteJump((void*)0x512BC1, ResetSelectedCharacter);
		WriteJump((void*)0x490C6B, (void*)0x490C80); // prevent Big from automatically loading Big's HUD
		WriteCall((void*)0x49FD54, SubtractSeconds);
		WriteCall((void*)0x4A0150, SubtractSeconds);
		WriteCall((void*)0x4A0198, SubtractSeconds);
		WriteCall((void*)0x483760, CheckLoadTimeRemainHUD);
		WriteJump((void*)0x47A907, (void*)0x47A936); // prevent Knuckles from automatically loading Emerald radar
		WriteData((void*)0x475E7C, 0x90u, 6); // make radar work when not Knuckles
		WriteData((void*)0x4764CC, 0x90u, 6); // make Tikal hints work when not Knuckles
		WriteCall((void*)0x4D677C, GetCharacter0ID); // fix item boxes for Gamma
		WriteCall((void*)0x4D6786, GetCharacter0ID); // fix item boxes for Big
		WriteCall((void*)0x4D6790, GetCharacter0ID); // fix item boxes for Sonic
		WriteCall((void*)0x4C06D9, GetCharacter0ID); // fix floating item boxes for Gamma
		WriteCall((void*)0x4C06E3, GetCharacter0ID); // fix floating item boxes for Big
		WriteCall((void*)0x4C06ED, GetCharacter0ID); // fix floating item boxes for Sonic
		WriteCall((void*)0x424D0A, GetSelectedCharacter); // fix character sfx for Casinopolis
		WriteData((void**)0x424F88, (void*)0x424E41); // ''
		WriteData((void**)0x424F8C, (void*)0x424E5C); // ''
		WriteData((void**)0x424F90, (void*)0x424E77); // ''
		WriteCall((void*)0x424E08, GetSelectedCharacter); // fix character sfx
		WriteCall((void*)0x4245F0, GetSelectedCharacter); // fix character voices in Chao Garden
		WriteCall((void*)0x4BFFEF, GetCharacter0ID); // fix 1up icon
		WriteCall((void*)0x4C02F3, GetCharacter0ID); // ''
		WriteCall((void*)0x4D682F, GetCharacter0ID); // ''
		WriteCall((void*)0x4D69AF, GetCharacter0ID); // ''
		WriteCall((void*)0x425E62, GetCharacter0ID); // fix life icon
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}