#pragma once

#include <iostream>
using namespace std;

class product{
protected:
	int id;
	string brand;
	string name;
	string coloring;
	int price;
public:
	int pricedifference(int b);
	int getprice();
	int getid();
	void changeprice();
	void changeid();
};