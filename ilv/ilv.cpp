#include "class.h"

#include <iostream>
using namespace std;

int product::quantity = 0;

int main()
{
	setlocale(LC_ALL, "Russian");

	int id, price, screenDiagonal, x, y;
	int countOfSales = 2;
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

	monitor* a = new monitor(id, brand, name, coloring, price, x, y, screenDiagonal, countOfSales);

	monitor arr1[3][3];
	for (int i = 0; i < 3; i++)for (int j = 0; j < 3; j++)arr1[i][j] = *a;

	arr1[0][0].changeprice();

	for (int i = 0; i < 3; i++)for (int j = 0; j < 2; j++)arr1[i][j].pricedifference(arr1[i][j + 1].getprice());




	//a->outputmonitor();

	//turnon(*a); // friendzas
	//turnoff(*a);

	//int* p1 = a->getid();
	//int p2 = a->getprice();

	//int p3 = product::countofproduct(); // static метод

	//cout << *p1 << " " << p2 << " " << p3 << endl;
	//monitor b = *a;

	//b = b + 1000; //перегрузка операторов
	//++b;
	//b++;
	//b.outputmonitor();


	//monitor arr1[10];
	//for (int i = 0; i < 10; i++)arr1[i] = *a;

	//arr1[0].changeprice();

	//for (int i = 0; i < 9; i++)arr1[i].pricedifference(arr1[i + 1].getprice());

	//monitor b = *a;

	//delete a;

	//monitor* arr2 = new monitor[10];

	//for (int i = 0; i < 10; i++)arr2[i] = b;

	//delete[]arr2;
}