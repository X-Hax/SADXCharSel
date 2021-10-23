#pragma once

FunctionPointer(void, sub_404A60, (int), 0x404A60);

ObjectFunc(E101_Main, 0x567fd0);
ObjectFunc(E103_Main, 0x4e7e90);
ObjectFunc(E104_Main, 0x605A90);
ObjectFunc(MK2_Main, 0x56C0B0);
ObjectFunc(EggViper_Main, 0x581E10);
ObjectFunc(E102KillCursor, 0x4CEFE0);
ObjectFunc(sub_580E70, 0x580E70);
ObjectFunc(EGM3Sippo, 0x583FB0);
ObjectFunc(sub_580A90, 0x580A90);
ObjectFunc(ZeroBoss_Main, 0x587C80);


// void __usercall(CharObj2 *a2@<edi>, EntityData1 *a3@<esi>)
const void* const sub_7B4450Ptr = (void*)0x7B4450;
inline void sub_7B4450(CharObj2* a2, EntityData1* a3)
{
	__asm
	{
		mov edi, [a2]
		mov esi, [a3]
		call sub_7B4450Ptr
	}
}


DataPointer(char, byte_3B2A2F1, 0x3B2A2F1);

DataPointer(int, dword_3B2A304, 0x3B2A304);
FunctionPointer(int, sub_42FB00, (), 0x42FB00);
FunctionPointer(ObjectMaster*, CheckLoadBird, (), 0x4C6820);
FunctionPointer(void, sub_469300, (int*, char, int), 0x469300);

ObjectFunc(sub_47D300, 0x47D300);
FunctionPointer(int, sub_46A820, (), 0x46A820);
FunctionPointer(int, sub_46A7F0, (), 0x46A7F0);
VoidFunc(sub_457D00, 0x457D00);

DataPointer(NJS_VECTOR, stru_3B2C6DC, 0x3B2C6DC);
DataPointer(NJS_VECTOR, stru_3B2C6D0, 0x3B2C6D0);
VoidFunc(sub_5919E0, 0x5919E0);
FunctionPointer(void, sub_43EC90, (EntityData1*, NJS_VECTOR*), 0x43EC90);
FunctionPointer(void, sub_437D20, (void(__cdecl* a1)(int), char a2, char a3), 0x437D20);
FunctionPointer(void, sub_464B00, (int), 0x464B00);


FunctionPointer(void, sub_43FA90, (EntityData1* a1, CharObj2** a2, CharObj2* a3), 0x43FA90);

DataPointer(NJS_TEXANIM, stru_91BB6C, 0x91BB6C);

FunctionPointer(int, sub_4751B0, (EntityData1*), 0x4751B0);
DataPointer(char, IsTrialCharSel, 0x3B2A2FA);
