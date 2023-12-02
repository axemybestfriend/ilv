#include "monitor.h"

#include <iostream>
using namespace std;

monitor::monitor() {
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	cout << "Создан товар типа monitor\n";
}
monitor::monitor(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal) {
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	this->x = x;
	this->y = y;
	this->screenDiagonal = screenDiagonal;
	cout << "Создан товар типа monitor\n";
}
monitor::monitor(int id) {
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	cout << "Создан товар типа monitor\n";
}
monitor::~monitor() {
	cout << "Товар типа monitor удален\n";
}
void monitor::outputmonitor() {
	cout << "Характеристики товара\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->coloring << endl;
	cout << this->price << endl;
	cout << this->screenDiagonal << endl;
	cout << this->x << "X" << this->y << endl;
}
void monitor::turnon() {
	cout << "Монитор запущен\n";
}
void monitor::turnof() {
	cout << "Монитор выключен\n";
}