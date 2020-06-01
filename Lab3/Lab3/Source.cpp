//**************************************************************************************
// Реализация вычисления интеграла функции ln((1+x)/(1-x)) на промежутке [0:1)
// ln: май 2020:: ivanzahna@gmail.com
//**************************************************************************************

#include <intrin.h> // там __rdtsc()
#include <math.h>
#include <iostream>
#include <time.h>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

#define SIZE_COUNT 1000000 // количество интервалов

//mutex mtx;

__int64 hz_cpu() { // замер частоты
	clock_t t_clock;
	__int64 t_tsc;
	t_clock = clock() + CLOCKS_PER_SEC;
	t_tsc = __rdtsc(); // взять TSC
	while (clock() < t_clock); // отсчет одной секунды
	return (__rdtsc() - t_tsc); // частота в герцах
}

void calculateIn (int start, int end, double &y) {
	double z, y1, x;
	for (int i = start; i < end; i++) {
		x = double(i + 0.5) / SIZE_COUNT;	//метод средних трапеций
		z = (1 + x) / (1 - x);
		y1 = log(z);
//		mtx.lock();	//ограничение по данным, от которого пришлось отказаться, ввиду съедания скорости
		y += y1 * 1 / SIZE_COUNT;
//		mtx.unlock();
	}
}

/**
 * @class Threads_Class
 * Родительский класс
 */
class Threads_Class {
public:
	double y = 0;
	__int64 time = 0;
	string name;
	virtual void calculate() {};	// Вычисление интеграла
	void measSeries();	//Подсчет времени, которое занимает этот расчет
};
void Threads_Class::measSeries() {
	__int64 freq = hz_cpu();
	__int64 start = __rdtsc();
	calculate();
	__int64 end = __rdtsc();
	cout << name << ":" << "y = " << y << endl;
	time = 1000000 * (end - start) / freq;	//Вывод времени в микросекундах
	cout << "time = ";
	cout <<  time << endl;
}

/**
 * @class Threads_One
 * Класс для расчета интеграла в одном потоке
 */
class Threads_One : public Threads_Class {
public:
	Threads_One() { name = "Single"; } //имя потока
	void calculate() {
		thread Th = thread(calculateIn, 0, SIZE_COUNT, ref(y));
		Th.join();
	//	calculateIn(0, SIZE_COUNT, ref(y));
	}
};

/**
 * @class Threads_Two
 * Класс для расчета интеграла в двух потоках
 */
class Threads_Two : public Threads_Class {
public:
	double y1 = 0;	//переменная, в которой хранится результат работы второго потока
	Threads_Two() { name = "Double";}
	void calculate() {
		thread Th[2];
		Th[0] = thread(calculateIn, 0, SIZE_COUNT / 2, ref(y1));
		Th[1] = thread(calculateIn, SIZE_COUNT / 2, SIZE_COUNT, ref(y));
		Th[0].join();
		Th[1].join();
		y += y1;
	}
};

/**
 * @class Threads_Three
 * Класс для расчета интеграла в трех потоках
 */
class Threads_Three : public Threads_Class {
public:
	double y1[3] = { 0, 0, 0 };	//массив со значениями результатов работы потоков
	int third = SIZE_COUNT / 3;
	int arr[4] = { 0, third , SIZE_COUNT - third, SIZE_COUNT };	//массив интервалов для объявления потоков 
	Threads_Three() { name = "Triple "; }
	void calculate() {
		thread Th[3];
		for (int i = 0; i < 3; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 3; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

/**
 * @class Threads_Four
 * Класс для расчета интеграла в четырех потоках
 */
class Threads_Four : public Threads_Class {
public:
	double y1[4] = { 0, 0, 0, 0 };
	int half = SIZE_COUNT / 2;
	int quadr = SIZE_COUNT / 4;
	int arr[5] = { 0, quadr , half, SIZE_COUNT - quadr, SIZE_COUNT };
	Threads_Four() { name = "Quadruple"; }
	void calculate() {
		thread Th[4];
		for (int i = 0; i < 4; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 4; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

/**
 * @class Threads_Five
 * Класс для расчета интеграла в пяти потоках
 */
class Threads_Five : public Threads_Class {
public:
	double y1[5] = { 0, 0, 0, 0, 0 };
	int five1 = SIZE_COUNT / 5;
	int arr[6] = { 0, five1 , five1 * 2, five1 * 3, SIZE_COUNT - five1, SIZE_COUNT };
	Threads_Five() { name = "Quintuple"; }
	void calculate() {
		thread Th[5];
		for (int i = 0; i < 5; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 5; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

/**
 * @class Threads_Six
 * Класс для расчета интеграла в шести потоках
 */
class Threads_Six : public Threads_Class {
public:
	double y1[6] = { 0, 0, 0, 0, 0, 0 };
	int third = SIZE_COUNT / 3;
	int half = SIZE_COUNT / 2;
	int six = SIZE_COUNT / 6;
	int arr[7] = { 0, six, third, half, half + six, SIZE_COUNT - six, SIZE_COUNT };
	Threads_Six() { name = "Sextuple"; }
	void calculate() {
		thread Th[6];
		for (int i = 0; i < 6; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 6; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

/**
 * @class Threads_Seven
 * Класс для расчета интеграла в семи потоках
 */
class Threads_Seven : public Threads_Class {
public:
	double y1[7] = { 0, 0, 0, 0, 0, 0, 0 };
	int seven = SIZE_COUNT / 7;
	int arr[8] = { 0, seven, seven * 2, seven * 3, seven * 4,
		seven * 5, SIZE_COUNT - seven, SIZE_COUNT };
	Threads_Seven() { name = "Septuple"; }
	void calculate() {
		thread Th[7];
		for (int i = 0; i < 7; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 7; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

/**
 * @class Threads_Eigth
 * Класс для расчета интеграла в восьми потоках
 */
class Threads_Eigth : public Threads_Class {
public:
	double y1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int half = SIZE_COUNT / 2;
	int quadr = SIZE_COUNT / 4;
	int egt = SIZE_COUNT / 8;
	int arr[9] = { 0, egt, quadr, half - egt , half, half + egt,
		SIZE_COUNT - quadr,  SIZE_COUNT - egt, SIZE_COUNT };
	Threads_Eigth() { name = "Octuple"; }
	void calculate() {
		thread Th[8];
		for (int i = 0; i < 8; i++) {
			Th[i] = thread(calculateIn, arr[i], arr[i + 1], ref(y1[i]));
		}
		for (int i = 0; i < 8; i++) {
			Th[i].join();
			y += y1[i];
		}
	}
};

int main()
{
	setlocale(LC_CTYPE, "rus");
	cout << "Count of calculation = " << SIZE_COUNT << endl;
	Threads_One One;
	Threads_Two Two;
	Threads_Three Three;
	Threads_Four Four;
	Threads_Five Five;
	Threads_Six Six;
	Threads_Seven Seven;
	Threads_Eigth Eigth;

	One.measSeries();
	Two.measSeries();
	Three.measSeries();
	Four.measSeries();
	Five.measSeries();
	Six.measSeries();
	Seven.measSeries();
	Eigth.measSeries();
	system("pause");
	return 0;
}