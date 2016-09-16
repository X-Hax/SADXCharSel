#pragma once
#include "GameObject.h"

class Indicator : GameObject
{
private:
	Indicator(ObjectMaster *obj);
	bool TexLoaded;
public:
	void Main();
	void Display();
	void Delete();
	static void __cdecl Init(ObjectMaster *obj);
};

void InitIndicator();
void InitSprites();
