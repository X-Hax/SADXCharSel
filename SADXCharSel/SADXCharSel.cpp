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
			if (btn & Buttons_B)
			{
				character = Characters_Sonic;
				MetalSonicFlag = 1;
			}
			obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, charfuncs[character]);
			obj->Data1->CharID = character;
		}
		obj->Data1->CharIndex = 0;
		CharObj1Ptrs[0] = obj->Data1;
		CharObj2Ptrs[0] = (CharObj2*)obj->Data2;
		PutPlayerAtStartPointIGuess(obj->Data1);
		if (!CurrentCharacter && GameMode != GameModes_Mission && !MetalSonicFlag)
			Load2PTails(obj);
		LoadTailsOpponent(CurrentCharacter, 1, CurrentLevel);
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

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);
		WriteJump((void*)0x512BC1, ResetSelectedCharacter);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}