#include "class.h"
#include "wirelessHeadphone.cpp"

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
	monitor* b = new monitor(id, brand, name, coloring, price-100, x, y, screenDiagonal, countOfSales);

	a->product::pricedifference(b->getprice());
	a->pricedifference(b->getprice());
	
	wirelessHeadphone<string>* c = new wirelessHeadphone<string>(100);//через конструктор базового класса
	wirelessHeadphone<int>* d = new wirelessHeadphone<int>(100);
	Headphone ilv(100);

	cout << ilv;
	
	*c = ilv;
	ilv.outputheadphone();
	c->output();
	//sqreensettings abstract
}