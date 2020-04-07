#include <intrin.h> // там __rdtsc()
#include <math.h>
#include <iostream>
#include <time.h>
#include <string>



using namespace std;

__int64 hz_cpu() { // замер частоты
	clock_t t_clock;
	__int64 t_tsc;
	t_clock = clock() + CLOCKS_PER_SEC;
	t_tsc = __rdtsc(); // взять TSC
	while (clock() < t_clock); // отсчет одной секунды
	return (__rdtsc() - t_tsc); // частота в герцах
}

class Log_Class { // Общее для всех логарифмов
public:
	string Type; //   название типа вычисления логарифма
	__int64 time = 0;
	__int64 freq = hz_cpu();
	virtual double calculate_Log(double x) { return 0; }; // функция вычисления логарифма
	void time_Serch(); // измерение скорости работы логарифмов от 0.01 до 1 с шагом 0.01
};
void Log_Class::time_Serch() {
	__int64 start = __rdtsc();
	for (double x = 0.01; x < 1; x += 0.01) {
		
		calculate_Log(x);
		//	time = 1000000 * (fin - start) / freq;
	}
	__int64 fin = __rdtsc();
	time = fin - start;
}

class Lib_Log : public Log_Class { // вычисление логарифма через библиотечную функцию
public:
	Lib_Log() { Type = "Lib_Log"; }
	double calculate_Log(double x) {
		long double z = (1 + x) / (1 - x);
		long double y = log(z);
		return y;
	}
};

class My_Log : public Log_Class { // вычисление логарифма через собственную функцию
public:
	My_Log() { Type = "My_Log"; }
	int k = 80;
	double calculate_Log(double x) {
		long double y = 0;
		double x1 = x;
		x *= x;
		for (int i = 0; i < k; i++) {
			y += x1 / (2 * i + 1);
			x1 *= x; //т.к. pow оказался лишкоммедленным, пришлось заменить его на это
		}
		return y * 2;
	}
};


int main()
{
	setlocale(LC_CTYPE, "rus");
	double dif = pow(2, -16);
	Lib_Log LL;
	My_Log ML;
	LL.time_Serch();
	ML.time_Serch();
	//поиск оптимального значения k для ML
	while (ML.time > LL.time) {
		printf_s("%d\tLL\n", LL.time);
		printf_s("%d\tML\t", ML.time);
		printf_s("%d\n", ML.k);
		ML.k--;
		LL.time_Serch();
		ML.time_Serch();
	}
	printf_s("k = %d\n", ML.k);
	double x, z, m_log, l_log;
	//вычисление диапазона, попадающего в погрешность
	for (x = 0.01; x < 1; x += 0.01) {
		m_log = ML.calculate_Log(x);
		l_log = LL.calculate_Log(x);
		z = abs(l_log - m_log);
		if (z > dif) {
			printf_s("Lib_ln:%.16f\tMy_ln:%.16f\n", l_log, m_log);
			cout << z << endl;
			cout << "x = " << x << endl;
			break;
		}
	}
	double rDif = 0;
	int count = 0;
	//вычисление погрешности для остальных числе
	for (; x < 1; x += 0.01) {
		m_log = ML.calculate_Log(x);
		l_log = LL.calculate_Log(x);
		rDif += abs(l_log - m_log);
		count++;
	}
	rDif /= count;
	printf_s("DIF:%.16f\n", rDif);
	system("pause");
	return 0;
}