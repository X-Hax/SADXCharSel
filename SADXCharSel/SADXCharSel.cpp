// SADXCharSel.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "IniFile.hpp"
#include "Indicator.h"

#include <algorithm>
using std::string;
using std::unordered_map;
using std::vector;
using std::transform;
HelperFunctions help;

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

__int16 selectedcharacter[PLAYER_COUNT] = { -1, -1, -1, -1 };
short defaultcharacters[Characters_MetalSonic] = { Characters_Sonic, Characters_Eggman, Characters_Tails, Characters_Knuckles, Characters_Tikal, Characters_Amy, Characters_Gamma, Characters_Big };
short bosscharacters[] = { Characters_Sonic, Characters_Knuckles, Characters_Gamma };
short bossai[] = { Characters_Sonic,Characters_Knuckles,Characters_Gamma };
int raceaicharacter = Characters_Sonic;
int tailsaicharacter = Characters_Tails;
bool enableindicator = true;
bool MetalSonicFlags[PLAYER_COUNT];
int currentplayer;

int GetSelectedCharacter()
{
	return selectedcharacter[0];
}

void CheckUnloadMetalTexs() {
	uint8_t player = PLAYER_COUNT;
	while (1) {
		if (MetalSonicFlags[player] == true) return;
		if (player == 0) {
			UnloadCharTextures(Characters_MetalSonic);
			return;
		}
		player--;
	}
}

void ChooseSelectedCharacter(int i)
{
	int btn = HeldButtons[i];
	if (btn & Buttons_Left)
	{
		selectedcharacter[i] = Characters_Sonic;

		if (MetalSonicFlags[i] == true) {
			MetalSonicFlags[i] = false;
			CheckUnloadMetalTexs();
			LoadSoundList(1);
		}
	}
	if (btn & Buttons_B)
	{
		selectedcharacter[i] = Characters_Sonic;

		if (MetalSonicFlags[i] == false) {
			MetalSonicFlags[i] = true;
			LoadSoundList(62);
			LoadCharTextures(Characters_MetalSonic);
		}
	}
	if (btn & Buttons_X)
		selectedcharacter[i] = Characters_Eggman;
	if (btn & Buttons_R)
		selectedcharacter[i] = Characters_Tails;
	if (btn & Buttons_Down)
		selectedcharacter[i] = Characters_Knuckles;
	if (btn & Buttons_Y)
		selectedcharacter[i] = Characters_Tikal;
	if (btn & Buttons_Right)
		selectedcharacter[i] = Characters_Amy;
	if (btn & Buttons_L)
		selectedcharacter[i] = Characters_Gamma;
	if (btn & Buttons_Up)
		selectedcharacter[i] = Characters_Big;

	if (MetalSonicFlags[i] == true && selectedcharacter[i] != Characters_Sonic) {
		MetalSonicFlags[i] = false;
		CheckUnloadMetalTexs();
	}
}


void __cdecl SetSelectedCharacter(int arg)
{
	if (selectedcharacter[0] == -1)
		selectedcharacter[0] = defaultcharacters[CurrentCharacter];
	ChooseSelectedCharacter(0);
	sub_404A60(arg);
}


void __cdecl Eggman_Display(ObjectMaster* obj)
{
	sub_7B4450(((EntityData2*)obj->Data2)->CharacterData, obj->Data1);
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

ObjectMaster* LoadCharObj(int i)
{
	ObjectMaster* obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, charfuncs[selectedcharacter[i]]);
	obj->Data1->CharID = (char)selectedcharacter[i];
	if (selectedcharacter[i] == Characters_Eggman)
		obj->DisplaySub = Eggman_Display;
	obj->Data1->CharIndex = (char)i;
	EntityData1Ptrs[i] = obj->Data1;
	EntityData2Ptrs[i] = (EntityData2*)obj->Data2;
	return obj;
}

int tailsracelevels[] = {
	LevelAndActIDs_WindyValley3,
	LevelAndActIDs_SpeedHighway1,
	LevelAndActIDs_SkyDeck1,
	LevelAndActIDs_IceCap3,
	LevelAndActIDs_Casinopolis2
};

bool isracelevel = false;

ObjectMaster* LoadTailsOpponent_r()
{
	RaceWinnerPlayer = 0;
	FastSonicAI = IsFastSonicAI();
	int lvlact = levelact(CurrentLevel, CurrentAct);
	if (lvlact == LevelAndActIDs_IceCap3 && GameMode == GameModes_Trial && byte_3B2A2F1 == 1)
		return nullptr;
	int aicourse = -1;
	for (size_t i = 0; i < LengthOfArray(tailsracelevels); i++)
		if (lvlact == tailsracelevels[i])
		{
			aicourse = i;
			break;
		}
	if (aicourse == -1) return nullptr;
	isracelevel = true;
	AICourse = aicourse;
	if (lvlact == LevelAndActIDs_SpeedHighway1 || raceaicharacter == Characters_MetalSonic)
	{
		LoadObject((LoadObj)(LoadObj_UnknownB | LoadObj_Data1), 0, Eggman2PAI);
		if (lvlact != LevelAndActIDs_SpeedHighway1)
			LoadObject(LoadObj_Data1, 8, MRace_EggMobile_TexlistManager);
		return nullptr;
	}
	else
	{
		if (selectedcharacter[1] == -1)
			selectedcharacter[1] = raceaicharacter;
		LoadObject((LoadObj)(LoadObj_UnknownB | LoadObj_Data1), 0, Sonic2PAI_Load);
		ObjectMaster* v3 = LoadCharObj(1);
		v3->Data1->Unknown = 2;
		DisableController(1u);
		return v3;
	}
}

ObjectMaster* Load2PTails_r(ObjectMaster* player1)
{
	if (!TailsAI_ptr
		&& (((CurrentAct | (unsigned __int16)(CurrentLevel << 8)) & 0xFF00) == LevelAndActIDs_Casinopolis1
			|| CheckTailsAI()))
	{
		ObjectMaster* v1 = LoadObject(LoadObj_Data1, 0, TailsAI_Main);
		TailsAI_ptr = v1;
		if (v1)
		{
			if (selectedcharacter[1] == -1)
				selectedcharacter[1] = tailsaicharacter;
			v1->Data1->CharID = (char)selectedcharacter[1];
			v1->Data1->CharIndex = 1;
			v1->DeleteSub = TailsAI_Delete;
			ObjectMaster* v3 = LoadCharObj(1);
			v3->Data1->Position.x = player1->Data1->Position.x - njCos(player1->Data1->Rotation.y) * 10;
			v3->Data1->Position.y = player1->Data1->Position.y;
			v3->Data1->Position.z = player1->Data1->Position.z - njSin(player1->Data1->Rotation.y) * 10;
			v1->Data1->Action = 0;
			dword_3B2A304 = 0;
			return v3;
		}
	}
	return nullptr;
}


void LoadCharacter_r()
{
	isracelevel = false;
	ClearPlayerArrays();
	ObjectMaster* obj;
	if (CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2)
	{
		obj = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, Tornado_Main);
		obj->Data1->CharID = (char)CurrentCharacter;
		obj->Data1->CharIndex = 0;
		EntityData1Ptrs[0] = obj->Data1;
		EntityData2Ptrs[0] = (EntityData2*)obj->Data2;
		MovePlayerToStartPoint(obj->Data1);
	}
	else
	{
		obj = LoadCharObj(0);
		MovePlayerToStartPoint(obj->Data1);
		ObjectMaster* lastobj = obj;
		ObjectMaster* o2 = nullptr;
		if (!CurrentCharacter && GameMode != GameModes_Mission && !MetalSonicFlags[0])
			o2 = Load2PTails_r(obj);
		switch (CurrentCharacter)
		{
		case Characters_Tails:
			o2 = LoadTailsOpponent_r();
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
		case Characters_Amy:
			CheckLoadBird();
			break;
		case Characters_Big:
			LoadObject(LoadObj_Data1, 6, BigHud_Main);
			break;
		}
		if (o2)
			lastobj = o2;
		else if (selectedcharacter[1] != -1)
		{
			obj = LoadCharObj(1);
			obj->Data1->Position.x = lastobj->Data1->Position.x - njCos(lastobj->Data1->Rotation.y) * 10;
			obj->Data1->Position.y = lastobj->Data1->Position.y;
			obj->Data1->Position.z = lastobj->Data1->Position.z - njSin(lastobj->Data1->Rotation.y) * 10;
			lastobj = obj;
		}
		for (int i = 2; i < PLAYER_COUNT; i++)
			if (selectedcharacter[i] != -1)
			{
				obj = LoadCharObj(i);
				obj->Data1->Position.x = lastobj->Data1->Position.x - njCos(lastobj->Data1->Rotation.y) * 10;
				obj->Data1->Position.y = lastobj->Data1->Position.y;
				obj->Data1->Position.z = lastobj->Data1->Position.z - njSin(lastobj->Data1->Rotation.y) * 10;
				lastobj = obj;
			}
		if (enableindicator)
			InitIndicator();
	}
}

void ResetSelectedCharacter();
Trampoline CharSelLoadA_t(0x00512BC0, 0x00512BC6, ResetSelectedCharacter);
void ResetSelectedCharacter()
{
	memset(selectedcharacter, -1, SizeOfArray(selectedcharacter));
	VoidFunc(original, CharSelLoadA_t.Target());
	original();
}


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
		if (MetalSonicFlags[0])
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


void __cdecl sub_461560()
{
	CharObj2* v3; // eax@20

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
		if (GameMode != GameModes_Trial || byte_3B2A2F1 != 1)
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
		if (GameMode != GameModes_Trial || byte_3B2A2F1 != 1)
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


void __cdecl sub_4141F0(ObjectMaster* obj)
{
	EntityData1* v1 = GetCharacterObject(0)->Data1;
	if (EntityData1Ptrs[1] && sub_46A820() && sub_46A7F0() == 1)
		v1 = EntityData1Ptrs[1];
	if (v1->Status & 3)
	{
		for (int i = 0; i < PLAYER_COUNT; i++)
			ForcePlayerAction(i, 19);
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
			for (int i = 0; i < PLAYER_COUNT; i++)
				ForcePlayerAction(i, 19);
			sub_461560();
			sub_457D00();
			LoadObject(LoadObj_Data1, 5, j_ScoreDisplay_Main);
		}
		break;
	case Characters_Knuckles:
		for (int i = 0; i < PLAYER_COUNT; i++)
			ForcePlayerAction(i, 19);
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
			for (int i = 0; i < PLAYER_COUNT; i++)
				ForcePlayerAction(i, 19);
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
		for (int i = 0; i < PLAYER_COUNT; i++)
			ForcePlayerAction(i, 19);
		a2.x = -36.072899f;
		a2.y = 5.7132001f;
		a2.z = -1.5176001f;
		sub_43EC90(EntityData1Ptrs[0], &a2);
		a1 = EntityData1Ptrs[0]->CollisionInfo->CollisionArray->center;
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
		if (MetalSonicFlags[0])
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
		if (MetalSonicFlags[0])
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
		if (MetalSonicFlags[0])
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

void __cdecl CheckLoadCapsule(ObjectMaster* obj)
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

void __cdecl CheckLoadFroggy(ObjectMaster* obj)
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

void __cdecl OFrog_CheckTouch_i(ObjectMaster* obj)
{
	EntityData1* v1 = obj->Data1;
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

void ReplaceSETObject(ObjectFuncPtr find, ObjectFuncPtr replace)
{
	for (size_t i = 0; i < ObjLists_Length; i++)
		if (ObjLists[i] != 0)
			for (int j = 0; j < ObjLists[i]->Count; j++)
				if (ObjLists[i]->List[j].LoadSub == find)
					ObjLists[i]->List[j].LoadSub = replace;
}

static string trim(const string& s)
{
	auto st = s.find_first_not_of(' ');
	if (st == string::npos)
		st = 0;
	auto ed = s.find_last_not_of(' ');
	if (ed == string::npos)
		ed = s.size() - 1;
	return s.substr(st, (ed + 1) - st);
}

static const unordered_map<string, uint8_t> charnamemap = {
	{ "sonic", Characters_Sonic },
	{ "eggman", Characters_Eggman },
	{ "tails", Characters_Tails },
	{ "knuckles", Characters_Knuckles },
	{ "tikal", Characters_Tikal },
	{ "amy", Characters_Amy },
	{ "gamma", Characters_Gamma },
	{ "big", Characters_Big },
	{ "eggmanai", Characters_MetalSonic }
};

static uint8_t ParseCharacterID(const string& str, Characters def)
{
	string s = trim(str);
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	auto ch = charnamemap.find(s);
	if (ch != charnamemap.end())
		return ch->second;
	return def;
}

const char* buttonstrings[] = {
	"Left:  Sonic",
	"X:     Eggman",
	"R:     Tails",
	"Down:  Knuckles",
	"Y:     Tikal",
	"Right: Amy",
	"L:     Gamma",
	"Up:    Big",
	"B:     Metal Sonic",
	"Start: Quit"
};

CollisionInfo* oldcol = nullptr;

void __cdecl CheckDeleteAnimThing(EntityData1* a1, CharObj2** a2, CharObj2* a3)
{
	for (int i = 0; i < 8; i++)
		if (EntityData1Ptrs[i] && EntityData1Ptrs[i] != a1 && EntityData1Ptrs[i]->CharID == a1->CharID)
			return;
	sub_43FA90(a1, a2, a3);
}

void __cdecl SetBigLifeTex(NJS_SPRITE* _sp, Int n, Float pri, NJD_SPRITE attr)
{
	if (MetalSonicFlags[0])
		stru_91BB6C.texid = 24;
	else
		stru_91BB6C.texid = selectedcharacter[0] + 12;
	njDrawSprite2D_ForcePriority(_sp, n, pri, attr);
}

int __cdecl CheckKnucklesBoundaryThing(EntityData1* data)
{
	return CurrentCharacter == Characters_Knuckles && sub_4751B0(data);
}

int __cdecl SetSonicWinPose_i()
{
	if (CurrentCharacter != Characters_Amy || (CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill))
		return 75;
	else
		return 47;
}

const int loc_4961DD = 0x4961DD;
__declspec(naked) void SetSonicWinPose()
{
	__asm
	{
		call SetSonicWinPose_i
		mov word ptr[esi + 124h], ax
		jmp loc_4961DD
	}
}

int __cdecl SetKnucklesWinPose_i()
{
	if (CurrentCharacter != Characters_Amy || (CurrentLevel >= LevelIDs_Chaos0 && CurrentLevel != LevelIDs_SandHill))
		return 39;
	else
		return 84;
}

const int loc_476B62 = 0x476B62;
__declspec(naked) void SetKnucklesWinPose()
{
	__asm
	{
		call SetKnucklesWinPose_i
		mov word ptr[edi + 124h], ax
		jmp loc_476B62
	}
}


void __cdecl LoadCharBoss_r(CharBossData* a1)
{
	if (a1)
	{
		if (a1->BossID < 6)
		{
			short c = bosscharacters[a1->BossID / 2];
			ObjectMaster* v1 = LoadObject((LoadObj)(LoadObj_UnknownA | LoadObj_Data1 | LoadObj_Data2), 1, charfuncs[c]);
			v1->Data1->CharID = (char)c;
			v1->Data1->CharIndex = 1;
			a1->BossCharacter = v1;
		}
		else
			a1->BossCharacter = nullptr;
	}
}

int bossids[] = { 0, 0, 0, 2, 0, 0, 4, 0 };
decltype(LoadSonicBossAI) bossaifuncs[] = { LoadSonicBossAI, LoadKnucklesBossAI, LoadGammaBossAI };
ObjectMaster* __cdecl LoadCharBossAI_r(CharBossData* a1)
{
	CharacterBossActive = 1;
	a1->anonymous_3 = 0;
	if (a1->BossID < 6)
	{
		a1->BossID = bossids[bossai[a1->BossID / 2]] | (a1->BossID & 1);
		ObjectMaster* v1 = bossaifuncs[a1->BossID / 2](a1);
		SetupCharBossArena(v1);
		return v1;
	}
	return nullptr;
}

void Teleport(uint8_t to, uint8_t from)
{
	if (EntityData1Ptrs[to] == nullptr || EntityData1Ptrs[from] == nullptr)
		return;

	EntityData1Ptrs[from]->Position = EntityData1Ptrs[to]->Position;
	EntityData1Ptrs[from]->Rotation = EntityData1Ptrs[to]->Rotation;

	if (CharObj2Ptrs[from] != nullptr)
		CharObj2Ptrs[from]->Speed = {};

	EntityData1Ptrs[from]->Action = 1;
	EntityData1Ptrs[from]->Status &= ~Status_Attack;
}

const string charnames[8] = { "Sonic", "Eggman", "Tails", "Knuckles", "Tikal", "Amy", "Gamma", "Big" };


void SwapSonicTextures(NJS_TEXLIST* sonictex) {
	if (GameMode == GameModes_Menu)
		return;

	if (MetalSonicFlag == 1) {
		njSetTexture(&METALSONIC_TEXLIST);
	}
	else {
		njSetTexture(sonictex);
	}
}

inline void SetMetalSonicFlag(uint8_t id) {
	//if metal sonic is selected for the current character, swap the flag

	if (MetalSonicFlags[id] == true) {
		MetalSonicFlag = 1;
	}
	else {
		MetalSonicFlag = 0;
	}
}

void Sonic_Display_r(ObjectMaster* obj);
Trampoline Sonic_Display_t(0x4948C0, 0x4948C7, Sonic_Display_r);
void __cdecl Sonic_Display_r(ObjectMaster* obj)
{
	if (GameState == 16) {
		// do the swap when the game is paused, since the main function is discarded
		// fixes metal sonic reverting to sonic
		SetMetalSonicFlag(obj->Data1->CharIndex);
	}

	ObjectFunc(origin, Sonic_Display_t.Target());
	origin(obj);
}

void Sonic_Main_r(ObjectMaster* obj);
Trampoline Sonic_Main_t(0x49A9B0, 0x49A9B7, Sonic_Main_r);
void Sonic_Main_r(ObjectMaster* obj) {
	SetMetalSonicFlag(obj->Data1->CharIndex);
	currentplayer = obj->Data1->CharIndex;

	ObjectFunc(origin, Sonic_Main_t.Target());
	origin(obj);

	// reset the flag to the state of player 1
	MetalSonicFlag = MetalSonicFlags[0];
}

void Tails_Jiggle_Main_r(ObjectMaster* obj);
Trampoline Tails_Jiggle_Main_t(0x45B840, 0x45B847, Tails_Jiggle_Main_r);
void Tails_Jiggle_Main_r(ObjectMaster* obj) {
	if (!EntityData1Ptrs[obj->Data1->CharIndex] || EntityData1Ptrs[obj->Data1->CharIndex]->CharID != Characters_Tails) {
		CheckThingButThenDeleteObject(obj);
		return;
	}

	ObjectFunc(origin, Tails_Jiggle_Main_t.Target());
	origin(obj);
}

void Knuckles_Jiggle_Main_r(ObjectMaster* obj);
Trampoline Knuckles_Jiggle_Main_t(0x473CE0, 0x473CE7, Knuckles_Jiggle_Main_r);
void Knuckles_Jiggle_Main_r(ObjectMaster* obj) {
	if (!EntityData1Ptrs[obj->Data1->CharIndex] || EntityData1Ptrs[obj->Data1->CharIndex]->CharID != Characters_Knuckles) {
		CheckThingButThenDeleteObject(obj);
		return;
	}

	ObjectFunc(origin, Knuckles_Jiggle_Main_t.Target());
	origin(obj);
}

void Amy_Jiggle_Main_r(ObjectMaster* obj);
Trampoline Amy_Jiggle_Main_t(0x485F40, 0x485F47, Amy_Jiggle_Main_r);
void Amy_Jiggle_Main_r(ObjectMaster* obj) {
	if (!EntityData1Ptrs[obj->Data1->CharIndex] || EntityData1Ptrs[obj->Data1->CharIndex]->CharID != Characters_Amy) {
		CheckThingButThenDeleteObject(obj);
		return;
	}

	ObjectFunc(origin, Amy_Jiggle_Main_t.Target());
	origin(obj);
}

void Amy_Jiggle_Main2_r(ObjectMaster* obj);
Trampoline Amy_Jiggle_Main2_t(0x485C50, 0x485C57, Amy_Jiggle_Main2_r);
void Amy_Jiggle_Main2_r(ObjectMaster* obj) {
	if (!EntityData1Ptrs[obj->Data1->CharIndex] || EntityData1Ptrs[obj->Data1->CharIndex]->CharID != Characters_Amy) {
		CheckThingButThenDeleteObject(obj);
		return;
	}

	ObjectFunc(origin, Amy_Jiggle_Main2_t.Target());
	origin(obj);
}

void __cdecl MetalSonic_AfterImage_Display_r(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;
	CharObj2* co2 = GetCharObj2(data->CharIndex);
	if (co2)
	{
		if (IsVisible(&data->Position, 15.0))
		{
			BackupConstantAttr();
			AddConstantAttr(0, NJD_FLAG_USE_ALPHA);
			njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
			float alpha = data->Scale.x - 1.0;
			SetMaterialAndSpriteColor_Float(alpha, 1.0, 1.0, 1.0);
			njPushMatrix(0);
			njTranslateV(0, &data->Position);
			Angle ang = data->Rotation.z;
			if (ang)
			{
				njRotateZ(0, ang);
			}
			ang = data->Rotation.x;
			if (ang)
			{
				njRotateX(0, ang);
			}
			ang = data->Rotation.y;
			if (ang != 0x8000)
			{
				njRotateY(0, (-32768 - ang));
			}
			Sonic_DrawOtherShit(co2->AnimationThing.Index, co2);
			njPopMatrix(1u);
			njColorBlendingMode(0, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
			ClampGlobalColorThing_Thing();
			RestoreConstantAttr();
		}
	}
}

void __cdecl MetalSonic_AfterImage_Main_r(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	MetalSonic_AfterImage_Display_r(obj);
	data->Scale.x -= 0.1f;

	if (data->Scale.x <= 0.0f)
	{
		CheckThingButThenDeleteObject(obj);
	}
}

void __cdecl MetalSonic_AfterImages_Main_r(ObjectMaster* obj)
{
	EntityData1* data = obj->Data1;

	if (data->CharIndex == 0) {
		data->Object = (NJS_OBJECT*)data->Index;
		obj->MainSub = MetalSonic_AfterImages_Main;
		MetalSonic_AfterImages_Main(obj);
		return;
	}

	if (data->Index-- < 0)
	{
		CheckThingButThenDeleteObject(obj);
	}
	else
	{
		ObjectMaster* character = GetCharacterObject(data->CharIndex);
		if (character)
		{
			CharObj2* co2 = GetCharObj2(data->CharIndex);
			if (co2)
			{
				ObjectMaster* afterimage = LoadObject(LoadObj_Data1, 4, MetalSonic_AfterImage_Main_r);
				if (afterimage)
				{
					EntityData1* imgdata = afterimage->Data1;
					imgdata->CharIndex = data->CharIndex;
					imgdata->Position = character->Data1->CollisionInfo->CollisionArray->center;
					imgdata->Rotation = character->Data1->Rotation;
					imgdata->Scale.x = 0.5f;
					afterimage->DisplaySub = MetalSonic_AfterImage_Display_r;
				}
			}
		}
	}
}

void __cdecl Sonic_Run1Ani_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index != 146)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 5;
			}
		}
		a1->AnimationThing.Index = 146;
	}
	else
	{
		a1->AnimationThing.Index = 12;
	}
}

static void __declspec(naked) Sonic_Run1Ani_()
{
	__asm
	{
		push esi
		call Sonic_Run1Ani_r
		pop esi
		retn
	}
}

void __cdecl Sonic_SpringAni_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index != 16)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 20;
			}
		}
		a1->AnimationThing.Index = 16;
	}
	else
	{
		a1->AnimationThing.Index = 16;
	}
}

static void __declspec(naked) Sonic_SpringAni_()
{
	__asm
	{
		push esi
		call Sonic_SpringAni_r
		pop esi
		retn
	}
}

void __cdecl Sonic_Walk3Ani_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index == 146)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 5;
			}
		}
	}
	a1->AnimationThing.Index = 11;
}

static void __declspec(naked) Sonic_Walk3Ani_()
{
	__asm
	{
		push esi
		call Sonic_Walk3Ani_r
		pop esi
		retn
	}
}

void __cdecl Sonic_Run2Ani_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index != 13)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 10;
			}
		}
		a1->AnimationThing.Index = 13;
	}
	else
	{
		a1->AnimationThing.Index = 13;
	}
}

static void __declspec(naked) Sonic_Run2Ani_()
{
	__asm
	{
		push esi
		call Sonic_Run2Ani_r
		pop esi
		retn
	}
}

void __cdecl Sonic_Spin_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index != 14)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 10;
			}
		}
		a1->AnimationThing.Index = 14;
	}
	else
	{
		a1->AnimationThing.Index = 14;
	}
}

static void __declspec(naked) Sonic_Spin_()
{
	__asm
	{
		push esi
		call Sonic_Spin_r
		pop esi
		retn
	}
}

void __cdecl Sonic_JumpPadAni_r(CharObj2* a1)
{
	if (MetalSonicFlag)
	{
		if (a1->AnimationThing.Index != 73)
		{
			ObjectMaster* v1 = LoadObject(LoadObj_Data1, 3, MetalSonic_AfterImages_Main_r);
			if (v1)
			{
				v1->Data1->CharIndex = currentplayer;
				v1->Data1->Index = 20;
			}
		}
		a1->AnimationThing.Index = 73;
	}
	else
	{
		a1->AnimationThing.Index = 73;
	}
}

static void __declspec(naked) Sonic_JumpPadAni_()
{
	__asm
	{
		push esi
		call Sonic_JumpPadAni_r
		pop esi
		retn
	}
}

bool redirect = false;


extern "C"
{
	__declspec(dllexport) void __cdecl OnControl()
	{
		if (GameMode != GameModes_Menu)
			for (int i = 0; i < PLAYER_COUNT; i++)
				if (ControllerPointers[i]->HeldButtons & Buttons_Z)
				{
					Controllers[i].HeldButtons &= ~(Buttons_B | Buttons_L | Buttons_R | Buttons_X | Buttons_Y);
					Controllers[i].HeldButtons &= ~(Buttons_L | Buttons_R);
					Controllers[i].LTriggerPressure = 0;
					Controllers[i].RTriggerPressure = 0;
					PressedButtons[i] &= ~(Buttons_B | Buttons_L | Buttons_R | Buttons_X | Buttons_Y | Buttons_Down | Buttons_Left | Buttons_Right | Buttons_Up);
				}
		if (redirect)
			*(float*)0x03B0E7A4 = 0.0f;
		for (int i = 2; i < PLAYER_COUNT; i++)
		{
			if (!IsControllerEnabled(i))
			{
				memset(&Controllers[i], 0, sizeof(ControllerData));
				continue;
			}

			memcpy(&Controllers[i], ControllerPointers[i], sizeof(ControllerData));
		}
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		if (ControllerPointers[0]->PressedButtons & Buttons_D)
		{
			redirect = !redirect;
			*ControllerPointers[1] = {};
		}

		if (redirect)
		{
			*ControllerPointers[1] = *ControllerPointers[0];
			*ControllerPointers[0] = {};
		}
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{

		if (!IsGamePaused() && oldcol)
		{
			if (HIBYTE(oldcol->flag) & 0x80)
			{
				if (oldcol->CollisionArray)
				{
					FreeMemory(oldcol->CollisionArray);
					oldcol->CollisionArray = nullptr;
				}
			}
			FreeMemory(oldcol);
			oldcol = nullptr;
		}
		if (GameMode == GameModes_Menu) {
			if (IsTrialCharSel && (CharacterSelection & 0x600)) {
				MetalSonicFlags[0] = true;
			}
			else {
				MetalSonicFlags[0] = false;
			}
		}
		if (GameMode == GameModes_Menu || CurrentLevel == LevelIDs_SkyChase1 || CurrentLevel == LevelIDs_SkyChase2 || !GetCharacterObject(0))
			return;
		short oldchar[PLAYER_COUNT];
		memcpy(oldchar, selectedcharacter, SizeOfArray(selectedcharacter));
		for (int i = 0; i < PLAYER_COUNT; i++)
		{
			int btn = HeldButtons[i];
			if (btn & Buttons_C && GetCharacterObject(i))
			{
				if (btn & Buttons_Up)
					Teleport(0, i);
				else if (btn & Buttons_Down)
					Teleport(1, i);
				else if (btn & Buttons_Left)
					Teleport(2, i);
				else if (btn & Buttons_Right)
					Teleport(3, i);
			}
			if (btn & Buttons_Z)
			{
				ChooseSelectedCharacter(i);
				bool startenabled = i > 0 && (i != 1 || (!TailsAI_ptr && !isracelevel));
				if (startenabled && btn & Buttons_Start)
				{
					selectedcharacter[i] = -1;
					ObjectMaster* obj = GetCharacterObject(i);
					if (obj)
						DeleteObject_(obj);
				}
				int sc = selectedcharacter[i];
				if (sc == Characters_Sonic && MetalSonicFlags[i])
					sc = Characters_MetalSonic;
				int textpos = (i * 19 + 1) << 16 | 0xA;
				SetDebugFontSize((unsigned short)(8 * min(VerticalStretch, HorizontalStretch)));
				for (size_t j = 0; j < LengthOfArray(buttonstrings); j++)
				{
					if (!startenabled && j == LengthOfArray(buttonstrings) - 1)
						break;
					if (j == sc)
						SetDebugFontColor(0xFF00FF00);
					else
						SetDebugFontColor(0xFFBFBFBF);
					DisplayDebugString(textpos++, (char*)buttonstrings[j]);
				}
				SetDebugFontColor(0xFFBFBFBF);
				SetDebugFontSize(8);
			}
			if (selectedcharacter[i] == -1) continue;
			if (selectedcharacter[i] == oldchar[i])
			{
				continue;
			}
			if (GetCharacterObject(i))
			{
				ObjectMaster* obj = GetCharacterObject(i);
				CharObj2* obj2 = ((EntityData2*)obj->Data2)->CharacterData;
				short powerups = obj2->Powerups;
				short jumptime = obj2->JumpTime;
				short underwatertime = obj2->UnderwaterTime;
				float loopdist = obj2->LoopDist;
				NJS_VECTOR speed = obj2->Speed;
				ObjectMaster* heldobj = obj2->ObjectHeld;
				obj->DeleteSub(obj);
				obj->MainSub = charfuncs[selectedcharacter[i]];
				if (selectedcharacter[i] == Characters_Eggman)
					obj->DisplaySub = Eggman_Display;
				obj->Data1->CharID = (char)selectedcharacter[i];
				obj->Data1->Action = 0;
				obj->Data1->Status &= ~(Status_Attack | Status_Ball | Status_LightDash | Status_Unknown3);
				if (!oldcol)
				{
					oldcol = obj->Data1->CollisionInfo;
					obj->Data1->CollisionInfo = nullptr;
				}
				else
					Collision_Free(obj);
				obj->MainSub(obj);
				obj2 = ((EntityData2*)obj->Data2)->CharacterData;
				obj2->Powerups = powerups;
				obj2->JumpTime = jumptime;
				obj2->UnderwaterTime = underwatertime;
				obj2->LoopDist = loopdist;
				obj2->Speed = speed;
				obj2->ObjectHeld = heldobj;
			}
			else
			{
				ObjectMaster* obj = LoadCharObj(i);
				ObjectMaster* p1 = GetCharacterObject(0);
				obj->Data1->Position.x = p1->Data1->Position.x - njCos(p1->Data1->Rotation.y) * 10;
				obj->Data1->Position.y = p1->Data1->Position.y;
				obj->Data1->Position.z = p1->Data1->Position.z - njSin(p1->Data1->Rotation.y) * 10;
				obj->Data1->Rotation.y = p1->Data1->Rotation.y;
				EnableController(i);
			}
		}
		if (oldchar[0] == selectedcharacter[0])
			return;
		if (oldchar[0] == Characters_Gamma)
		{
			char min, sec, fr;
			GetTime2(&min, &sec, &fr);
			int t = fr + (sec * 60) + (min * 3600);
			if (t < 10800)
			{
				t = 10800 - t;
				fr = t % 60;
				sec = (t /= 60) % 60;
				SetTime2(t / 60, sec, fr);
			}
			else
				SetTime2(0, 0, 0);
		}
		else if (selectedcharacter[0] == Characters_Gamma)
		{
			char min, sec, fr;
			GetTime2(&min, &sec, &fr);
			int t = fr + (sec * 60) + (min * 3600);
			if (t < 10800)
			{
				t = 10800 - t;
				fr = t % 60;
				sec = (t /= 60) % 60;
				SetTime2(t / 60, sec, fr);
			}
			else
				SetTime2(1, 0, 0);
		}
		switch (CurrentLevel)
		{
		case LevelIDs_Casinopolis:
			switch (selectedcharacter[0])
			{
			default:
				if (MetalSonicFlags[0])
					LoadSoundList(62);
				else
					LoadSoundList(1);
				if (VoiceLanguage)
					LoadSoundList(60);
				else
					LoadSoundList(59);
				break;
			case Characters_Knuckles:
				LoadSoundList(49);
				if (VoiceLanguage)
				{
					LoadSoundList(70);
					LoadSoundList(58);
				}
				else
				{
					LoadSoundList(69);
					LoadSoundList(57);
				}
				break;
			case Characters_Amy:
				LoadSoundList(46);
				if (VoiceLanguage)
					LoadSoundList(64);
				else
					LoadSoundList(63);
				break;
			case Characters_Gamma:
				LoadSoundList(48);
				if (VoiceLanguage)
					LoadSoundList(68);
				else
					LoadSoundList(67);
				break;
			case Characters_Big:
				LoadSoundList(47);
				if (VoiceLanguage)
					LoadSoundList(66);
				else
					LoadSoundList(65);
				break;
			}
			break;
		case LevelIDs_SSGarden:
		case LevelIDs_ECGarden:
		case LevelIDs_MRGarden:
		case LevelIDs_ChaoRace:
			LoadCharVoices();
			break;
		default:
			switch (selectedcharacter[0])
			{
			default:
				if (MetalSonicFlags[0])
					LoadSoundList(62);
				else
					LoadSoundList(1);
				if (VoiceLanguage)
					LoadSoundList(72);
				else
					LoadSoundList(71);
				break;
			case Characters_Knuckles:
				LoadSoundList(49);
				if (VoiceLanguage)
					LoadSoundList(70);
				else
					LoadSoundList(69);
				break;
			case Characters_Amy:
				LoadSoundList(46);
				if (VoiceLanguage)
					LoadSoundList(64);
				else
					LoadSoundList(63);
				break;
			case Characters_Gamma:
				LoadSoundList(48);
				if (VoiceLanguage)
					LoadSoundList(68);
				else
					LoadSoundList(67);
				break;
			case Characters_Big:
				LoadSoundList(47);
				if (VoiceLanguage)
					LoadSoundList(66);
				else
					LoadSoundList(65);
				break;
			case Characters_Tails:
				LoadSoundList(1);
				if (VoiceLanguage)
					LoadSoundList(72);
				else
					LoadSoundList(71);
				break;
			}
			break;
		}
	}


	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{

		help = helperFunctions;

		// Enables WriteAnalogs for controllers >= 2 (3)
		Uint8 patch[3] = { 0x83u, 0xFFu, 0x04u };
		WriteData((void*)0x0040F180, patch);

		// Object patches
		WriteData((Uint8*)0x007A4DC4, PLAYER_COUNT); // Spring_Main
		WriteData((Uint8*)0x007A4FF7, PLAYER_COUNT); // SpringB_Main
		WriteData((Uint8*)0x0079F77C, PLAYER_COUNT); // SpringH_Main
		WriteData((Uint8*)0x004418B8, PLAYER_COUNT); // IsPlayerInsideSphere (could probably use a better name!)

		InitSprites();
		E100_Series_Fixes_Init();
		Init_GammaFixes();

		WriteCall((void*)0x41522C, SetSelectedCharacter);
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump(LoadCharBoss, LoadCharBoss_r);
		WriteJump((void*)0x4B71A0, LoadCharBossAI_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);
		WriteJump((void*)0x490C6B, (void*)0x490C80); // prevent Big from automatically loading Big's HUD


		WriteJump((void*)0x47A907, (void*)0x47A936); // prevent Knuckles from automatically loading Emerald radar
		WriteData<6>((void*)0x475E7C, 0x90u); // make radar work when not Knuckles
		WriteData<6>((void*)0x4764CC, 0x90u); // make Tikal hints work when not Knuckles

		WriteCall((void*)0x4D6786, GetCharacter0ID); // fix item boxes for Big
		WriteCall((void*)0x4D6790, GetCharacter0ID); // fix item boxes for Sonic

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
		WriteData<2>((void*)0x7A2061, 0x90u); // make balloon work for all characters
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
		WriteCall((void*)0x44B0A4, CheckDeleteAnimThing);
		ReplaceSETObject(Froggy_Main, CheckLoadFroggy);
		WriteData<6>((void*)0x48ADA5, 0x90u); // prevent Amy from loading the bird
		WriteCall((void*)0x46FC91, SetBigLifeTex); // fix life icon in Big levels
		WriteCall((void*)0x478937, CheckKnucklesBoundaryThing);
		WriteCall((void*)0x478AFC, CheckKnucklesBoundaryThing);
		WriteCall((void*)0x47B395, CheckKnucklesBoundaryThing);
		WriteCall((void*)0x47B423, CheckKnucklesBoundaryThing);
		WriteJump((void*)0x4961D4, SetSonicWinPose);
		WriteJump((void*)0x476B59, SetKnucklesWinPose);
		WriteCall((void*)0x4E966C, GetCharacter0ID); // fix ice cap snowboard 1
		WriteCall((void*)0x4E9686, GetCharacter0ID); // fix ice cap snowboard 2
		WriteCall((void*)0x597B1C, GetCharacter0ID); // fix sand hill snowboard
		WriteCall((void*)0x4949ED, SwapSonicTextures); // use the correct texture for Sonic / Metal Sonic 

		//Metal Sonic AfterImages for every player
		WriteJump((void*)Sonic_Run1AniPtr, Sonic_Run1Ani_);
		WriteJump((void*)Sonic_SpringAniPtr, Sonic_SpringAni_);
		WriteJump((void*)Sonic_Walk3AniPtr, Sonic_Walk3Ani_);
		WriteJump((void*)Sonic_Run2AniPtr, Sonic_Run2Ani_);
		WriteJump((void*)Sonic_SpinPtr, Sonic_Spin_);
		WriteJump((void*)Sonic_JumpPadAniPtr, Sonic_JumpPadAni_);

		const IniFile* settings = new IniFile(std::string(path) + "\\config.ini");
		for (int i = 0; i < Characters_MetalSonic; i++)
			defaultcharacters[i] = ParseCharacterID(settings->getString("Player1", charnames[i]), (Characters)i);
		tailsaicharacter = ParseCharacterID(settings->getString("Player2", "TailsAICharacter"), Characters_Tails);
		raceaicharacter = ParseCharacterID(settings->getString("Player2", "RaceAICharacter"), Characters_Sonic);
		bosscharacters[0] = ParseCharacterID(settings->getString("CharBoss", "SonicChar"), Characters_Sonic);
		bossai[0] = ParseCharacterID(settings->getString("CharBoss", "SonicAI"), Characters_Sonic);
		bosscharacters[1] = ParseCharacterID(settings->getString("CharBoss", "KnucklesChar"), Characters_Knuckles);
		bossai[1] = ParseCharacterID(settings->getString("CharBoss", "KnucklesAI"), Characters_Knuckles);
		bosscharacters[2] = ParseCharacterID(settings->getString("CharBoss", "GammaChar"), Characters_Gamma);
		bossai[2] = ParseCharacterID(settings->getString("CharBoss", "GammaAI"), Characters_Gamma);
		enableindicator = settings->getBool("Misc", "EnableIndicator", true);
		delete settings;
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}