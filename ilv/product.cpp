#include "product.h"

int product::pricedifference(int b) {
	if (this->price - b > 0)cout << "������ ����� ������ ������� �� : " << this->price - b << endl;
	if (this->price - b < 0)cout << "������ ����� ������ ������� �� : " << -(this->price - b) << endl;
	else cout << "������ ����� ���������\n";
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
		cout << "������� ����� ���� ������ : ";
		int new_price;
		cin >> new_price;
		if (new_price < 0) {
			throw "����� ���� ��� ��������� �������� ��������";
		}
		if (new_price == this->price) {
			throw "����� ���� ��������� �� ������,��������� �� ����� �����������";
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
	cout << "������� ����� id ������ : ";
	cin >> this->id;
}
int product::countofproduct()
{
	return product::quantity;
}