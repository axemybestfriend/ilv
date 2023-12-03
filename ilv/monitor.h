#pragma once

#include "product.h"
#include "screensettings.h"

class monitor :public screensettings, public product
{
	friend void turnon(const monitor& a);
	friend void turnoff(const monitor& a);
private:
	int countOfSales;
public:
	monitor();
	monitor(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal,int countOfSales);
	monitor(int id);
	~monitor();
	void outputmonitor();
	monitor& operator++();
	monitor operator++(int);
	int operator +(int number) const;
	monitor& operator =(int number);
};