#pragma once

#include <iostream>
using namespace std;

class product {
protected:
	int id;
	string brand;
	string name;
	string coloring;
	int price;
	static int quantity;
public:
	int pricedifference(int b);
	int& getprice();
	int* getid();
	void changeprice();
	void changeprice(int number);
	void changeid();
	static int countofproduct();
	static void setquanity(int number);
};