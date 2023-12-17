#pragma once
#include "Headphone.h"

template <typename T> class wirelessHeadphone : public Headphone
{
protected:
	T model;
	int batterycapacity;
public:
	wirelessHeadphone();
	wirelessHeadphone(int a);
	wirelessHeadphone(int id, string brand, string name, string coloring, int price, int batterycapacity, T model);
	~wirelessHeadphone();
	wirelessHeadphone operator=(Headphone& C);
	int batterycapacitydifferenceheadphone(wirelessHeadphone b);
	void output();
};

