#include "product.h"

int product::pricedifference(int b) {
	if (this->price - b > 0)cout << "Первый товар дороже второго на : " << this->price - b << endl;
	if (this->price - b < 0)cout << "Второй товар дороже первого на : " << -(this->price - b) << endl;
	else cout << "Товары стоят одинаково\n";
	return this->price - b;
}
int& product::getprice() {
	return price;
}
int* product::getid() {
	return &id;
}
void product::changeprice() {
	cout << "Введите новую цену товара : ";
	cin >> this->price;
}
void product::changeprice(int number)
{
	this->price = number;
}
void product::changeid() {
	cout << "Введите новый id товара : ";
	cin >> this->id;
}
int product::countofproduct()
{
	return product::quantity;
}