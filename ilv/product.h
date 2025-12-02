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
	void output();
	static int quantity;
	virtual int pricedifference(int b);
	void startpd(int b);
	int& getprice();
	string getbrand();
	string getcoloring();
	string getname();
	int* getid();
	void changeprice();
	void changeprice(int number);
	void changeid();
	static int countofproduct();
};