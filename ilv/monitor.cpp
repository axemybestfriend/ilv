#include "monitor.h"

#include <iostream>
using namespace std;

monitor::monitor() {
	product::quantity += 1;
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->countOfSales = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	cout << "������ ����� ���� monitor\n";
}
monitor::monitor(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal, int countOfSales) {
	product::quantity += 1;
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	this->countOfSales = countOfSales;
	this->x = x;
	this->y = y;
	this->screenDiagonal = screenDiagonal;
	cout << "������ ����� ���� monitor\n";
}
monitor::monitor(int id) {
	product::quantity += 1;
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->countOfSales = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	cout << "������ ����� ���� monitor\n";
}
monitor::~monitor() {
	product::quantity -= 1;
	cout << "����� ���� monitor ������\n";
}
int monitor::pricedifference(int b)
{
	if (this->price - b > 0)cout << "������ ������� ������ ������� �� : " << this->price - b << endl;
	if (this->price - b < 0)cout << "������ ������� ������ ������� �� : " << -(this->price - b) << endl;
	else cout << "�������� ����� ���������\n";
	return this->price - b;
}
void monitor::output() {
	this->product::output();
	cout << this->countOfSales << endl;
	cout << this->screenDiagonal << endl;
	cout << this->x << "X" << this->y << endl;
}
void turnon(const monitor &a) {
	cout << "������� " << a.name << " (" << a.id << ") �������\n" ;
}
void turnoff(const monitor& a) {
	cout << "������� " << a.name << " (" << a.id << ") ��������\n";
}

int monitor::operator +(int number) const
{
	return (this->price + number);
}
monitor& monitor::operator++()
{
	this->countOfSales++;
	return *this;
}
monitor monitor::operator++(int)
{
	monitor copy = *this;
	++(*this);
	return copy;
}
monitor& monitor::operator=(int number)
{
	this->price += 1000;
	return *this;
}