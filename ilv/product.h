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
public:
	static int quantity;
	int pricedifference(int b);
	int& getprice();
	int* getid();
	void changeprice();
	void changeprice(int number);
	void changeid();
	static int countofproduct();
};