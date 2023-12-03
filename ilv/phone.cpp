#include "phone.h"

#include <iostream>
using namespace std;

phone::phone() {
	product::quantity += 1;
	this->id = 0;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	this->ROM = 0;
	cout << "Создан товар типа phone\n";
}
phone::phone(int id) {
	product::quantity += 1;
	this->id = id;
	this->brand = "NONE";
	this->name = "NONE";
	this->coloring = "NONE";
	this->price = 0;
	this->x = 0;
	this->y = 0;
	this->screenDiagonal = 0;
	this->ROM = 0;
	cout << "Создан товар типа phone\n";
}
phone::phone(int id, string brand, string name, string coloring, int price, int x, int y, int screenDiagonal, int ROM) {
	product::quantity += 1;
	this->id = id;
	this->brand = brand;
	this->name = name;
	this->coloring = coloring;
	this->price = price;
	this->x = x;
	this->y = y;
	this->screenDiagonal = screenDiagonal;
	this->ROM = ROM;
	cout << "Создан товар типа phone\n";
}
phone:: ~phone() {
	product::quantity -= 1;
	cout << "Товар типа phone удален\n";
}
void phone::outputphone() {
	cout << "Характеристики товара\n";
	cout << this->id << endl;
	cout << this->brand << endl;
	cout << this->name << endl;
	cout << this->ROM << endl;
	cout << this->price << endl;
	cout << this->coloring << endl;
	cout << this->screenDiagonal << endl;
	cout << this->x << "X" << this->y << endl;
}