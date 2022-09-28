#include "stdafx.h"

static FunctionHook<void, task*> E101_Main_t((intptr_t)E101_Main);
static FunctionHook<void, task*> E103_Main_t((intptr_t)E103_Main);
static FunctionHook<void, task*> E104_Main_t((intptr_t)E104_Main);
static FunctionHook<void, task*> MK2Main_t((intptr_t)MK2_Main);

//Allow non gamma character to damage E100 Series.
void CheckAndSetDamage(taskwk* data1, taskwk* p1)
{
	if (GetCollidingEntityA((EntityData1*)data1) && p1->flag & Status_Attack && p1->counter.b[1] != Characters_Gamma && data1->mode > 0)
	{
		data1->flag |= Status_Hurt;
	}
}

void E104Enemy_Main_R(task* obj) {

	auto data1 = obj->twp;
	auto p1 = playertwp[0];

	CheckAndSetDamage(data1, p1);

	E104_Main_t.Original(obj);
}

void E103Enemy_Main_R(task* obj) {

	auto data1 = obj->twp;
	auto p1 = playertwp[0];

	CheckAndSetDamage(data1, p1);

	E103_Main_t.Original(obj);
}

void E101_Main_R(task* obj) {

	auto data1 = obj->twp;
	auto p1 = playertwp[0];

	CheckAndSetDamage(data1, p1);

	E101_Main_t.Original(obj);
}

void MK2_Main_R(task* obj) {

	auto data1 = obj->twp;
	auto p1 = playertwp[0];

	CheckAndSetDamage(data1, p1);

	MK2Main_t.Original(obj);
}

void __cdecl E100_Series_Fixes_Init()
{
	E103_Main_t.Hook(E103Enemy_Main_R);
	E104_Main_t.Hook(E104Enemy_Main_R);
	E101_Main_t.Hook(E101_Main_R);
	MK2Main_t.Hook(MK2_Main_R);
}