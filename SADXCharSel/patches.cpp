#include "stdafx.h"

extern __int16 selectedcharacter[PLAYER_COUNT];
extern bool MetalSonicFlags[PLAYER_COUNT];
extern int currentplayer;

static FunctionHook<void, task*> Sonic_Main_t((intptr_t)Sonic_Main);
static FunctionHook<void, task*> Sonic_Display_t((intptr_t)Sonic_Display);
static FunctionHook<void, task*> Tails_Jiggle_Main_t((intptr_t)Tails_Jiggle_Main);
static FunctionHook<void, task*> Knuckles_Jiggle_Main_t((intptr_t)0x473CE0);
static FunctionHook<void, task*> Amy_Jiggle_Main2_t((intptr_t)0x485C50);
static FunctionHook<void, task*> Amy_Jiggle_Main_t((intptr_t)0x485F40);

static FunctionHook<signed int, NJS_MATRIX_PTR, NJS_ACTION*, float, Uint32, NJS_MATRIX_PTR> CalcMMMatrix_t((intptr_t)0x4B81F0);
static FunctionHook<void, taskwk*, motionwk2*, playerwk*> ProcessVertexWelds_t((intptr_t)ProcessVertexWelds);

int GetSelectedCharacter()
{
	return selectedcharacter[0];
}

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

signed int CalcMMMatrix_r(NJS_MATRIX_PTR a1, NJS_ACTION* a2, float n, Uint32 instance, NJS_MATRIX_PTR a5)
{
	if (!a2 || !a2->motion)
	{
		return 0;
	}

	return CalcMMMatrix_t.Original(a1, a2, n, instance, a5);
}

void ProcessVertexWelds_r(taskwk* a1, motionwk2* a2, playerwk* a3)
{
	if (!a3 || !a3->mj.plactptr[a3->mj.reqaction].actptr)
	{
		return;
	}

	ProcessVertexWelds_t.Original(a1, a2, a3);
}

void __cdecl Sonic_Display_r(task* obj)
{
	if (GameState == 16) {
		// do the swap when the game is paused, since the main function is discarded
		// fixes metal sonic reverting to sonic
		SetMetalSonicFlag(pNum);
	}

	Sonic_Display_t.Original(obj);
}

void Sonic_Main_r(task* obj) {

	SetMetalSonicFlag(pNum);
	currentplayer = pNum;


	Sonic_Main_t.Original(obj);

	// reset the flag to the state of player 1
	MetalSonicFlag = MetalSonicFlags[0];
}

void Tails_Jiggle_Main_r(task* obj) {
	if (!EntityData1Ptrs[pNum] || EntityData1Ptrs[pNum]->CharID != Characters_Tails) {
		FreeTask(obj);
		return;
	}

	Tails_Jiggle_Main_t.Original(obj);
}

void Knuckles_Jiggle_Main_r(task* obj) {
	if (!EntityData1Ptrs[pNum] || EntityData1Ptrs[pNum]->CharID != Characters_Knuckles) {
		FreeTask(obj);
		return;
	}

	Knuckles_Jiggle_Main_t.Original(obj);
}

void Amy_Jiggle_Main_r(task* obj) {
	if (!EntityData1Ptrs[pNum] || EntityData1Ptrs[pNum]->CharID != Characters_Amy) {
		FreeTask(obj);
		return;
	}

	Amy_Jiggle_Main_t.Original(obj);
}


void Amy_Jiggle_Main2_r(task* obj) {
	if (!EntityData1Ptrs[pNum] || EntityData1Ptrs[pNum]->CharID != Characters_Amy) {
		FreeTask(obj);
		return;
	}

	Amy_Jiggle_Main2_t.Original(obj);
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
	for (size_t i = 0; i < ObjLists.size(); i++)
		if (ObjLists[i] != 0)
			for (int j = 0; j < ObjLists[i]->Count; j++)
				if (ObjLists[i]->List[j].LoadSub == find)
					ObjLists[i]->List[j].LoadSub = replace;
}


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

void init_Patches()
{
	// Enables WriteAnalogs for controllers >= 2 (3)
	Uint8 patch[3] = { 0x83u, 0xFFu, 0x04u };
	WriteData((void*)0x0040F180, patch);

	// Object patches
	WriteData((Uint8*)0x007A4DC4, PLAYER_COUNT); // Spring_Main
	WriteData((Uint8*)0x007A4FF7, PLAYER_COUNT); // SpringB_Main
	WriteData((Uint8*)0x0079F77C, PLAYER_COUNT); // SpringH_Main
	WriteData((Uint8*)0x004418B8, PLAYER_COUNT); // IsPlayerInsideSphere (could probably use a better name!)

	//Fix Big -> MS Swap crash 
	CalcMMMatrix_t.Hook(CalcMMMatrix_r);
	ProcessVertexWelds_t.Hook(ProcessVertexWelds_r);

	Sonic_Display_t.Hook(Sonic_Display_r);
	Sonic_Main_t.Hook(Sonic_Main_r);
	Tails_Jiggle_Main_t.Hook(Tails_Jiggle_Main_r);
	Knuckles_Jiggle_Main_t.Hook(Knuckles_Jiggle_Main_r);
	Amy_Jiggle_Main2_t.Hook(Amy_Jiggle_Main2_r);
	Amy_Jiggle_Main_t.Hook(Amy_Jiggle_Main_r);

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
}