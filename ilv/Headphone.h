#pragma once
#include "product.h"
class Headphone : public product
{
public:
	Headphone();
	Headphone(int id);
	Headphone(int id, string brand, string name, string coloring, int price);
	~Headphone();
	friend ostream& operator<<(std::ostream& stream, Headphone& a);
	void outputheadphone();
};

