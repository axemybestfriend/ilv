#pragma once

#include "product.h"
#include "screensettings.h"

class monitor :public screensettings, public product
{
public:
	monitor();
	monitor(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal);
	monitor(int id);
	~monitor();
	void outputmonitor();
	void turnon();
	void turnof();
};

