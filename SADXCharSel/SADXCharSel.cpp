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

	__int16 selectedcharacter = -1;

	int GetSelectedCharacter()
	{
		return selectedcharacter;
	}

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
			obj->Data1->CharID = (char)selectedcharacter;
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

	int GetCharacter0ID()
	{
		return GetCharacterID(0);
	}

	FunctionPointer(void, sub_469300, (int *, char, int), 0x469300);
	void SetResultsCamera()
	{
		switch (GetCharacter0ID())
		{
		case Characters_Sonic:
		case Characters_Tails:
			sub_469300((int*)0x919BF4, 3, 720);
			break;
		case Characters_Knuckles:
			sub_469300((int*)0x91A848, 3, 720);
			break;
		case Characters_Amy:
			sub_469300((int*)0x9196D0, 3, 720);
			break;
		case Characters_Gamma:
			sub_469300((int*)0x91A248, 3, 720);
			break;
		}
	}

	void __cdecl PlayStandardResultsVoice()
	{
		bool bosslevel = CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill;
		switch (GetCharacter0ID())
		{
		case Characters_Sonic:
			if (MetalSonicFlag)
				return;
			else if (bosslevel)
				Load_DelayedSound_Voice(1843);
			else
				Load_DelayedSound_Voice(1840);
			break;
		case Characters_Eggman:
			Load_DelayedSound_Voice(225);
			break;
		case Characters_Tails:
			if (bosslevel)
				Load_DelayedSound_Voice(1806);
			else
				Load_DelayedSound_Voice(1803);
			break;
		case Characters_Knuckles:
			if (bosslevel)
				Load_DelayedSound_Voice(1793);
			else
				Load_DelayedSound_Voice(1788);
			break;
		case Characters_Amy:
			if (bosslevel)
				Load_DelayedSound_Voice(1735);
			else
				Load_DelayedSound_Voice(1733);
			break;
		case Characters_Gamma:
			if (!bosslevel && CurrentCharacter == Characters_Big)
				Load_DelayedSound_Voice(1772);
			else
				Load_DelayedSound_Voice(1770);
			break;
		case Characters_Big:
			if (bosslevel)
				Load_DelayedSound_Voice(1750);
			else
				Load_DelayedSound_Voice(1747);
			break;
		}
	}

	DataPointer(short, word_3B2A2F0, 0x3B2A2F0);
	ObjectFunc(sub_47D300, 0x47D300);
	FunctionPointer(int, sub_46A820, (), 0x46A820);
	FunctionPointer(int, sub_46A7F0, (), 0x46A7F0);
	void __cdecl sub_461560()
	{
		CharObj2 *v3; // eax@20

		switch (CurrentLevel)
		{
		case LevelIDs_SpeedHighway:
			if (GetRaceWinnerPlayer() == 1)
			{
				SetResultsCamera();
				PlayStandardResultsVoice();
				Load_DelayedSound_BGM(MusicIDs_RoundClear);
			}
			else
				Load_DelayedSound_Voice(225);
			if (GameMode != 9 || HIBYTE(word_3B2A2F0) != 1)
				LoadObject(LoadObj_Data1, 3, sub_47D300);
			break;
		case LevelIDs_WindyValley:
		case LevelIDs_SkyDeck:
		case LevelIDs_IceCap:
		case LevelIDs_Casinopolis:
			if (sub_46A820())
			{
				if (sub_46A7F0() == 1)
					SetOpponentRaceVictory();
				else
					SetTailsRaceVictory();
			}
			if (GetRaceWinnerPlayer() == 1)
			{
				SetResultsCamera();
				PlayStandardResultsVoice();
				Load_DelayedSound_BGM(MusicIDs_RoundClear);
			}
			else
				Load_DelayedSound_Voice(214);
			if (GameMode != 9 || HIBYTE(word_3B2A2F0) != 1)
				LoadObject(LoadObj_Data1, 3, sub_47D300);
			break;
		default:
			SetTailsRaceVictory();
			if (CurrentLevel == LevelIDs_SandHill)
			{
				v3 = GetCharObj2(0);
				v3->PhysicsData.CollisionSize = 9;
				v3->PhysicsData.YOff = 4.5f;
			}
			SetResultsCamera();
			PlayStandardResultsVoice();
			Load_DelayedSound_BGM(MusicIDs_RoundClear);
			break;
		}
	}

	VoidFunc(sub_457D00, 0x457D00);
	void __cdecl sub_4141F0(ObjectMaster *obj)
	{
		EntityData1 *v1 = GetCharacterObject(0)->Data1;
		if (CharObj1Ptrs[1] && sub_46A820() && sub_46A7F0() == 1)
			v1 = CharObj1Ptrs[1];
		if (v1->Status & 3)
		{
			ForcePlayerAction(0, 19);
			switch (CurrentCharacter)
			{
			case Characters_Tails:
				sub_461560();
				break;
			case Characters_Gamma:
				if (GetCharacter0ID() == Characters_Gamma)
				{
					sub_469300((int*)0x91A248, 3, 720);
					switch (CurrentLevel)
					{
					case LevelIDs_EmeraldCoast:
						Load_DelayedSound_Voice(1772);
						break;
					case LevelIDs_HotShelter:
						Load_DelayedSound_Voice(1773);
						break;
					case LevelIDs_RedMountain:
						Load_DelayedSound_Voice(1774);
						break;
					case LevelIDs_WindyValley:
						Load_DelayedSound_Voice(1775);
						break;
					default:
						Load_DelayedSound_Voice(1770);
						break;
					}
				}
				else
				{
					SetResultsCamera();
					PlayStandardResultsVoice();
				}
				break;
			default:
				SetResultsCamera();
				PlayStandardResultsVoice();
				break;
			}
			sub_457D00();
			LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
			Load_DelayedSound_BGM(MusicIDs_RoundClear);
			CheckThingButThenDeleteObject(obj);
		}
	}

	DataPointer(short, PauseEnabled, 0x90BF1C);
	DataPointer(NJS_VECTOR, stru_3B2C6DC, 0x3B2C6DC);
	DataPointer(NJS_VECTOR, stru_3B2C6D0, 0x3B2C6D0);
	VoidFunc(sub_5919E0, 0x5919E0);
	FunctionPointer(void, sub_43EC90, (EntityData1 *, NJS_VECTOR *), 0x43EC90);
	FunctionPointer(void, sub_437D20, (void(__cdecl *a1)(int), char a2, char a3), 0x437D20);
	FunctionPointer(void, sub_464B00, (int), 0x464B00);
	void __cdecl LoadLevelResults_r()
	{
		NJS_VECTOR a1; // [sp+0h] [bp-18h]@12
		NJS_VECTOR a2; // [sp+Ch] [bp-Ch]@12

		DisableController(0);
		PauseEnabled = 0;
		DisableTimeThing();
		if (GameMode == GameModes_Mission)
			sub_5919E0();
		if (CurrentCharacter != Characters_Tails && GetCharacter0ID() == Characters_Tails)
			SetTailsRaceVictory();
		switch (CurrentCharacter)
		{
		case Characters_Tails:
			if (GetRaceWinnerPlayer() == 1)
				LoadObject((LoadObj)0, 3, sub_4141F0);
			else
			{
				ForcePlayerAction(0, 19);
				sub_461560();
				sub_457D00();
				LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
			}
			break;
		case Characters_Knuckles:
			ForcePlayerAction(0, 19);
			sub_457D00();
			LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
			SoundManager_Delete2();
			if ((CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill) || GetCharacter0ID() != Characters_Knuckles)
			{
				SetResultsCamera();
				PlayStandardResultsVoice();
			}
			else
			{
				sub_469300((int*)0x91A848, 3, 720);
				Load_DelayedSound_Voice(1790);
			}
			Load_DelayedSound_BGM(MusicIDs_RoundClear);
			break;
		case Characters_Amy:
			if (CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill)
				LoadObject((LoadObj)0, 3, sub_4141F0);
			else
			{
				ForcePlayerAction(0, 19);
				sub_457D00();
				LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
				SoundManager_Delete2();
				if (GetCharacter0ID() == Characters_Amy)
					Load_DelayedSound_Voice(1733);
				else
					PlayStandardResultsVoice();
				Load_DelayedSound_BGM(MusicIDs_RoundClear);
			}
			break;
		case Characters_Big:
			ForcePlayerAction(0, 19);
			a2.x = -36.072899f;
			a2.y = 5.7132001f;
			a2.z = -1.5176001f;
			sub_43EC90(CharObj1Ptrs[0], &a2);
			a1 = CharObj1Ptrs[0]->CollisionInfo->CollisionArray->v;
			stru_3B2C6DC = a1;
			njSubVector(&a1, &a2);
			stru_3B2C6D0 = a1;
			sub_437D20(sub_464B00, 1, 2);
			sub_457D00();
			LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
			if ((CurrentAct | (CurrentLevel << 8)) < LevelAndActIDs_Chaos0)
				SoundManager_Delete2();
			PlayStandardResultsVoice(); // regular game doesn't play any voices
			Load_DelayedSound_BGM(MusicIDs_RoundClear);
			break;
		default:
			LoadObject((LoadObj)0, 3, sub_4141F0);
			SoundManager_Delete2();
			break;
		}
	}

	void PlayPostResultsVoice1_i()
	{
		int v10;
		switch (GetCharacter0ID())
		{
		case Characters_Sonic:
			if (MetalSonicFlag)
			{
				PlayVoice(2044);
				return;
			}
			v10 = 1497;
			break;
		case Characters_Tails:
			v10 = 1460;
			break;
		case Characters_Knuckles:
			v10 = 1447;
			break;
		case Characters_Amy:
			v10 = 1392;
			break;
		case Characters_Big:
			v10 = 1404;
			break;
		default:
			return;
		}
		PlaySound(v10, 0, 0, 0);
	}

	const int loc_42866E = 0x42866E;
	__declspec(naked) void PlayPostResultsVoice1()
	{
		__asm
		{
			call PlayPostResultsVoice1_i
			jmp loc_42866E
		}
	}

	void PlayPostResultsVoice2_i()
	{
		int v10;
		switch (GetCharacter0ID())
		{
		case Characters_Sonic:
			if (MetalSonicFlag)
			{
				PlayVoice(2044);
				return;
			}
			v10 = 1499;
			break;
		case Characters_Tails:
			v10 = 1462;
			break;
		case Characters_Knuckles:
			v10 = 1449;
			break;
		case Characters_Amy:
			v10 = 1391;
			break;
		case Characters_Big:
			v10 = 1406;
			break;
		default:
			return;
		}
		PlaySound(v10, 0, 0, 0);
	}

	__declspec(naked) void PlayPostResultsVoice2()
	{
		__asm
		{
			call PlayPostResultsVoice2_i
			jmp loc_42866E
		}
	}

	void PlayPostResultsVoice3_i()
	{
		int v10;
		switch (GetCharacter0ID())
		{
		case Characters_Sonic:
			if (MetalSonicFlag)
			{
				PlayVoice(2044);
				return;
			}
			v10 = 1496;
			break;
		case Characters_Tails:
			v10 = 1459;
			break;
		case Characters_Knuckles:
			v10 = 1446;
			break;
		case Characters_Amy:
			v10 = 1389;
			break;
		case Characters_Big:
			v10 = 1403;
			break;
		default:
			return;
		}
		PlaySound(v10, 0, 0, 0);
	}

	__declspec(naked) void PlayPostResultsVoice3()
	{
		__asm
		{
			call PlayPostResultsVoice3_i
			jmp loc_42866E
		}
	}

	int SetAmyWinPose()
	{
		if (CurrentCharacter != Characters_Amy || (CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill))
			return 42;
		else
			return 32;
	}

	void __cdecl CheckLoadCapsule(ObjectMaster *obj)
	{
		if (GetCharacter0ID() == Characters_Big)
		{
			obj->Data1->Position.y += 10;
			obj->MainSub = OFrog;
			OFrog(obj);
		}
		else
			Capsule_Load(obj);
	}

	void __cdecl CheckLoadFroggy(ObjectMaster *obj)
	{
		if (GetCharacter0ID() != Characters_Big)
		{
			obj->MainSub = OFrog;
			OFrog(obj);
		}
		else
		{
			obj->MainSub = Froggy_Main;
			Froggy_Main(obj);
		}
	}

	void __cdecl OFrog_CheckTouch_i(ObjectMaster *obj)
	{
		EntityData1 *v1 = obj->Data1;
		int v2 = IsPlayerInsideSphere(&v1->Position, (v1->Scale.x + 1.0f) * 14.0f);
		if (v2 == 1)
		{
			SetTailsRaceVictory();
			LoadLevelResults();
			v1->Action = 3;
		}
		else if (v2 == 2 && CurrentCharacter == Characters_Tails)
		{
			SetOpponentRaceVictory();
			LoadLevelResults();
			v1->Action = 3;
		}
	}

	__declspec(naked) void OFrog_CheckTouch()
	{
		__asm
		{
			push eax
			call OFrog_CheckTouch_i
			pop eax
			retn
		}
	}

	DataArray(ObjectList *, ObjLists, 0x974AF8, 344);
	void ReplaceSETObject(ObjectFuncPtr find, ObjectFuncPtr replace)
	{
		for (size_t i = 0; i < ObjLists_Length; i++)
			if (ObjLists[i] != 0)
				for (int j = 0; j < ObjLists[i]->Count; j++)
					if (ObjLists[i]->List[j].LoadSub == find)
						ObjLists[i]->List[j].LoadSub = replace;
	}

	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		WriteCall((void*)0x41522C, SetSelectedCharacter);
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);
		WriteJump((void*)0x512BC1, ResetSelectedCharacter);
		WriteJump((void*)0x490C6B, (void*)0x490C80); // prevent Big from automatically loading Big's HUD
		WriteCall((void*)0x426005, GetCharacter0ID); // fix ResetTime() for Gamma
		WriteCall((void*)0x427F2B, GetCharacter0ID); // fix ResetTime2() for Gamma
		WriteData((char*)0x41486D, (char)0xEB); // fix time reset at level load for Gamma
		WriteData((__int16**)0x414A0C, &selectedcharacter); // fix 1min minimum at level restart for Gamma
		WriteCall((void*)0x426081, GetCharacter0ID); // fix Gamma's timer
		WriteCall((void*)0x4266C9, GetCharacter0ID); // fix Gamma's time bonus
		WriteCall((void*)0x426379, GetCharacter0ID); // fix Gamma's time display
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
		WriteJump(LoadLevelResults, LoadLevelResults_r);
		WriteData((char*)0x4879C1, (char)0x90);
		WriteCall((void*)0x4879C2, SetAmyWinPose);
		WriteData((void*)0x7A2061, 0x90u, 2); // make balloon work for all characters
		WriteCall((void*)0x61CB77, GetCurrentCharacterID); // make Twinkle Park playable
		WriteCall((void*)0x61CF8D, GetCurrentCharacterID); // ''
		WriteJump((void*)0x42850A, PlayPostResultsVoice1);
		WriteJump((void*)0x428603, PlayPostResultsVoice2);
		WriteJump((void*)0x428591, PlayPostResultsVoice3);
		WriteCall((void*)0x79D7E2, GetCharacter0ID); // fix cart jump voice
		ReplaceSETObject(Capsule_Load, CheckLoadCapsule);
		WriteData((ObjectFuncPtr*)0x4FA050, CheckLoadCapsule); // crashed plane in Emerald Coast
		WriteData((ObjectFuncPtr*)0x4DF3F0, CheckLoadCapsule); // Chaos Emerald in Windy Valley
		WriteData((ObjectFuncPtr*)0x5DD0E0, CheckLoadCapsule); // Chaos Emerald in Casinopolis
		WriteData((ObjectFuncPtr*)0x4ECFE0, CheckLoadCapsule); // Chaos Emerald in Ice Cap
		WriteData((ObjectFuncPtr*)0x7B0DD3, CheckLoadCapsule); // ending of Lost World
		WriteData((ObjectFuncPtr*)0x5B2523, CheckLoadCapsule); // ending of Final Egg
		WriteCall((void*)0x4FA352, OFrog_CheckTouch); // fix for Big in Tails levels
		ReplaceSETObject(Froggy_Main, CheckLoadFroggy);
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}