#include "wirelessHeadphone.h"

#include <iostream>
using namespace std;
template <typename T> wirelessHeadphone<T>::wirelessHeadphone() {
	product::quantity += 1;
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->batterycapacity = 0;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
template <typename T> wirelessHeadphone<T>::wirelessHeadphone(int a) {
	this->Headphone::Headphone();
	this->batterycapacity = a;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
template <typename T> wirelessHeadphone<T>::wirelessHeadphone(string a) {
	this->Headphone::Headphone();
	this->batterycapacity = 0;
	this->name = a;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
template <typename T> wirelessHeadphone<T>::wirelessHeadphone(int id, string brand, string name, string coloring, int price, int batterycapacity, T model) {
	product::quantity += 1;
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	this->batterycapacity = batterycapacity;
	this->model = model;
	cout << "������ ����� ���� WirelessHeadphone\n";
}
template <typename T> wirelessHeadphone<T>::~wirelessHeadphone() {
	product::quantity -= 1;
	cout << "����� ���� WirelessHeadphone ������\n";
}
template <typename T> wirelessHeadphone<T> wirelessHeadphone<T>::operator=(Headphone& C)
{
	this->id = *C.getid();
	this->brand = C.getbrand();
	this->name = C.getname();
	this->coloring = C.getcoloring();
	this->price = C.getprice();
	return *this;
}
template <typename T> int wirelessHeadphone<T>::batterycapacitydifferenceheadphone(wirelessHeadphone<T> b) {
	if (this->batterycapacity - b.batterycapacity > 0)cout << "����������� ������� ������ ������� ��: " << this->batterycapacity - b.batterycapacity << endl;
	if (this->batterycapacity - b.batterycapacity < 0)cout << "����������� ������� ������ ������� ��: " << -(this->batterycapacity - b.batterycapacity) << endl;
	else cout << "�� ������ ������������ �����";
	return this->batterycapacity - b.batterycapacity;
}
template <typename T> void wirelessHeadphone<T>::output() {
	cout << "�������������� ������\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->coloring << endl;
	cout << this->price << endl;
	cout << this->batterycapacity << endl;
	cout << this->model << endl;
}