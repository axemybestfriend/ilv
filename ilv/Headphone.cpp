#include "Headphone.h"

#include <iostream>
using namespace std;

Headphone::Headphone() {
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	cout << "������ ����� ���� Headphone\n";
}
Headphone::Headphone(int id) {
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	cout << "������ ����� ���� Headphone\n";
}
Headphone::Headphone(int id, string brand, string name, string coloring, int price) {
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	cout << "������ ����� ���� Headphone\n";
}
Headphone::~Headphone() {
	cout << "����� ���� Headphone ������\n";
}
void Headphone::outputheadphone() {
	cout << "�������������� ������\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->price << endl;
	cout << this->coloring << endl;
}