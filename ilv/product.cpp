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
void product::changeprice()
{
	try{
		cout << "Введите новую цену товара : ";
		int new_price;
		cin >> new_price;
		if (new_price < 0) {
			throw "Новая цена вне диапозона реальных значений";
		}
		if (new_price == this->price) {
			throw "Новая цена совпадает со старой,изменения не будут произведены";
		}
		this->price = new_price;
	}
	catch(const char* error_message){
		cout << error_message << endl;
	}
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