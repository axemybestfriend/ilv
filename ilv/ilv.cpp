﻿#include "class.h"

#include <iostream>
using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");

	int id, price, screenDiagonal, x, y;
	string brand, name, coloring;

	cout << "Введите id товара: ";
	cin >> id;
	cout << "Введите бренд товара: ";
	cin >> brand;
	cout << "Введите название товара: ";
	cin >> name;
	cout << "Введите расцветку товара: ";
	cin >> coloring;
	cout << "Введите стоимость товара: ";
	cin >> price;
	cout << "Введите диагональ экрана: ";
	cin >> screenDiagonal;
	cout << "Введите разрешение экрана (например: 720 480): ";
	cin >> x >> y;

	monitor* a = new monitor(id, brand, name, coloring, price, x, y, screenDiagonal);

	a->outputmonitor();
	a->turnon();
	a->turnof();

	monitor arr1[10];
	for (int i = 0; i < 10; i++)arr1[i] = *a;

	arr1[0].changeprice();

	for (int i = 0; i < 9; i++)arr1[i].pricedifference(arr1[i + 1].getprice());

	monitor b = *a;

	delete a;

	monitor* arr2 = new monitor[10];

	for (int i = 0; i < 10; i++)arr2[i] = b;

	delete[]arr2;
}