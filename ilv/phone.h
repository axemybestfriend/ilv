#pragma once

#include "screensettings.h"
#include "product.h"

class phone :public screensettings,public product
{
protected:
	int ROM;
public:
	phone();
	phone(int id);
	phone(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal, int ROM);
	~phone();
	void outputphone();
};

