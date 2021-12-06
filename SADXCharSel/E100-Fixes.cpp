#include "stdafx.h"

Trampoline* E101_Main_t;
Trampoline* MK2Main_t;
Trampoline* E103_Main_t;
Trampoline* E104_Main_t;


//Allow non gamma character to damage E100 Series.
void CheckAndSetDamage(EntityData1* data1, EntityData1* p1)
{
	if (GetCollidingEntityA(data1) && p1->Status & Status_Attack && p1->CharID != Characters_Gamma && data1->Action > 0)
	{
		data1->Status |= Status_Hurt;
	}
}

void E104Enemy_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];

	CheckAndSetDamage(data1, p1);

	ObjectFunc(origin, E104_Main_t->Target());
	origin(obj);
}



void E103Enemy_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];

	CheckAndSetDamage(data1, p1);

	ObjectFunc(origin, E103_Main_t->Target());
	origin(obj);
}


void E101_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];

	CheckAndSetDamage(data1, p1);

	ObjectFunc(origin, E101_Main_t->Target());
	origin(obj);
}



void MK2_Main_R(ObjectMaster* obj) {

	EntityData1* data1 = obj->Data1;
	EntityData1* p1 = EntityData1Ptrs[0];

	CheckAndSetDamage(data1, p1);

	ObjectFunc(origin, MK2Main_t->Target());
	origin(obj);
}



void __cdecl E100_Series_Fixes_Init()
{
	E103_Main_t = new Trampoline((int)E103_Main, (int)E103_Main + 0x7, E103Enemy_Main_R);
	E104_Main_t = new Trampoline((int)E104_Main, (int)E104_Main + 0x7, E104Enemy_Main_R);
	E101_Main_t = new Trampoline((int)E101_Main, (int)E101_Main + 0x5, E101_Main_R);
	MK2Main_t = new Trampoline((int)MK2_Main, (int)MK2_Main + 0x5, MK2_Main_R);
}