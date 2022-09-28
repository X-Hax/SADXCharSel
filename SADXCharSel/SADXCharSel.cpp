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

CollisionInfo* oldcol = nullptr;

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
static FunctionHook<void> CharSelLoadA_t((intptr_t)CharSel_LoadA, ResetSelectedCharacter);
void ResetSelectedCharacter()
{
	memset(selectedcharacter, -1, SizeOfArray(selectedcharacter));
	CharSelLoadA_t.Original();
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

		init_Patches();
		E100_Series_Fixes_Init();
		Init_GammaFixes();

		InitSprites();

		WriteCall((void*)0x41522C, SetSelectedCharacter);
		WriteJump(LoadCharacter, LoadCharacter_r);
		WriteJump(LoadCharBoss, LoadCharBoss_r);
		WriteJump((void*)0x4B71A0, LoadCharBossAI_r);
		WriteJump((void*)0x41490D, ChangeStartPosCharLoading);


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