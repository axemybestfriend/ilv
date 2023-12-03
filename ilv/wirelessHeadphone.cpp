#include "wirelessHeadphone.h"

#include <iostream>
using namespace std;

wirelessHeadphone::wirelessHeadphone(){
	product::quantity += 1;
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->batterycapacity = 0;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
wirelessHeadphone::wirelessHeadphone(int id) {
	product::quantity += 1;
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->batterycapacity = 0;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
wirelessHeadphone::wirelessHeadphone(int id, string brand, string name, string coloring, int price, int batterycapacity) {
	product::quantity += 1;
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	this->batterycapacity = batterycapacity;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
wirelessHeadphone::~wirelessHeadphone() {
	product::quantity -= 1;
	cout << "����� ���� WirelessHeadphone ������\n";
}
int wirelessHeadphone::batterycapacitydifferenceheadphone(wirelessHeadphone b) {
	if (this->batterycapacity - b.batterycapacity > 0)cout << "����������� ������� ������ ������� ��: " << this->batterycapacity - b.batterycapacity << endl;
	if (this->batterycapacity - b.batterycapacity < 0)cout << "����������� ������� ������ ������� ��: " << -(this->batterycapacity - b.batterycapacity) << endl;
	else cout << "�� ������ ������������ �����";
	return this->batterycapacity - b.batterycapacity;
}
void wirelessHeadphone::outputwirelessheadphone() {
	cout << "�������������� ������\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->coloring << endl;
	cout << this->price << endl;
	cout << this->batterycapacity << endl;
}