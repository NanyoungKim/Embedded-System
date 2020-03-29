#include<iostream>
using namespace std;


int main() {

	
	for (int k = 0; k < 4; k++) {

		int LEDs = 0x80;
		cout << LEDs << endl;

		for (int i = 0; i < 7; i++) {

			LEDs = LEDs >> 1;
			cout << LEDs << endl;
		}
		for (int i = 0; i < 7; i++) {
			LEDs = LEDs << 1;
			cout << LEDs << endl;
		}
		for (int i = 0; i < 7; i++) {

			LEDs = LEDs >> 1;
			cout << LEDs << endl;
		}

	}

	system("pause");
	return 0;
}
