#pragma once
#include "Headphone.h"
class wirelessHeadphone : public Headphone
{
protected:
	int batterycapacity;
public:
	wirelessHeadphone();
	wirelessHeadphone(int id);
	wirelessHeadphone(int id, string brand, string name, string coloring, int price, int batterycapacity);
	~wirelessHeadphone();
	int batterycapacitydifferenceheadphone(wirelessHeadphone b);
	void outputwirelessheadphone();
};

