#include "Headphone.h"

#include <iostream>
using namespace std;

Headphone::Headphone() {
	product::quantity += 1;
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
}
Headphone::Headphone(int id) {
	product::quantity += 1;
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	cout << "Создан товар типа Headphone\n";
}
Headphone::Headphone(int id, string brand, string name, string coloring, int price) {
	product::quantity += 1;
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	cout << "Создан товар типа Headphone\n";
}
Headphone::~Headphone() {
	product::quantity -= 1;
	cout << "Товар типа Headphone удален\n";
}
void Headphone::outputheadphone() {
	cout << "Характеристики товара\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->price << endl;
	cout << this->coloring << endl;
}
std::ostream& operator<<(std::ostream& stream, Headphone& a)
{
	a.output();
	return stream;
}