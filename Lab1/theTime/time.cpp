#include <time.h> // там clock(), clock_t, CLOCKS_PER_SEC
#include <intrin.h> // там __rdtsc()
#include <windows.h> // там функции QPC QueryPerformanceCounter(&tackts) и QueryPerformanceFrequency(&freq)
#include <math.h>
#include <iostream>
#include <string>
//#include <plog/Log.h>

using namespace std;

#define SIZE_DARR 200000

double arrStatic[SIZE_DARR];
double *arr = arrStatic;
class Clocks { // Общее для всех видов часов
public:
	string name; //   название типа часов
	__int64 time, freq; // время и частота
	__int64 realTime = 0;	//время на предыдущем подсчете
	__int64 dTime = 0;
	__int64 counter = 0; //количество изменений
	void my_func(int arcount) { // заполняет arcount элементов массива arr 
		for (int i = 0; i < arcount; i++) { arr[i] = sqrt(i / arcount); }
	}
	virtual void measure(int arcount) {}; // функция измерения времени
	void measSeries(int arcount); // серия из scount измерений с рабочей нагрузкой arcount
};
void Clocks::measSeries(int arcount) {
	measure(arcount);
	cout.width(10);
	cout.setf(ios::right);
	if (time - realTime > 0) {
		dTime = time - realTime;
		counter++;
		cout << arcount << "\t" << time << "\t" << dTime << endl;
	}
	realTime = time;
}
class Clock : public Clocks { // часы на основе функции clock()
public:
	Clock() { freq = CLOCKS_PER_SEC; name = "clock"; }
	void measure(int sz) {
		__int64 t = clock(); // стартовая засечка
		my_func(sz);
		t = clock() - t; // замер продолжительности
		time = 1000000 * t / freq;
	}
	void SearchPoint(int sz) {
		__int64 t1 = time;
	//	cout << endl << name << ": " << sz << " элементов" << endl;
		int k = 0;
		for (int i = 0; i < 4; i++) {
			measure(sz);
	//		cout << time << "\t" << freq << endl;
			if (time > t1) {
				k++;
			}
		}
		if (k > 2) {
			dTime = time - realTime;
			counter++;
			realTime = time;
		}
	}
};

class QPC : public Clocks { // часы на основе QPC
public:
	QPC() { QueryPerformanceFrequency((LARGE_INTEGER *)&freq); name = "QPC"; }
	void measure(int sz) {
		LONGLONG t_start, t_end;
		QueryPerformanceCounter((LARGE_INTEGER *)&t_start); // стартовая засечка
		my_func(sz);
		QueryPerformanceCounter((LARGE_INTEGER *)&t_end);
		time = 1000000 * (t_end - t_start) / freq;
	}
};

class TSC : public Clocks { // часы на основе Time Stamp Counter
public:
	TSC() { name = "TSC"; }
	void measure(int sz) {
		freq = hz_cpu();
		__int64 p[10];
		p[0] = __rdtsc();
		p[1] = __rdtsc();
		p[2] = __rdtsc();
		p[3] = __rdtsc();
		p[4] = __rdtsc();
		p[5] = __rdtsc();
		p[6] = __rdtsc();
		p[7] = __rdtsc();
		p[8] = __rdtsc();
		p[9] = __rdtsc();
		cout << "\nМассив. 1 - версия";
		cout << endl << name << ": " << 10 << " элементов" << endl
			<< "Время (нс) \tЧастота (гц)\n";
		for (int i = 1; i < 10; i++) {
			time = p[i] - p[i - 1];
	//		time = 1000000 * time / freq;
			cout << time << "\t" << freq << endl;
		}
		__int64 arr[10];
		__int64 *pA = arr;
		cout << "\nFOR. 2 - версия";
		for (int i = 0; i < 10; i++) {
			*pA++ = __rdtsc();
		}
		cout << endl << name << ": " << 10 << " элементов" << endl
			<< "Время (нс) \tЧастота (гц)\n";
		for (int i = 1; i < 10; i++) {
			time = arr[i] - arr[i - 1];
			cout << time << "\t" << freq << endl;
		}
		cout << "\nМассив. 2 - версия";
		__int64 pt[10];
		__int64 *pL = pt;
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		*pL++ = __rdtsc();
		cout << endl << name << ": " << 10 << " элементов" << endl
			<< "Время (нс) \tЧастота (гц)\n";
		for (int i = 1; i < 10; i++) {
			time = pt[i] - pt[i - 1];
			//		time = 1000000 * time / freq;
			cout << time << "\t" << freq << endl;
		}
	//	time = 1000000 * (p[1] - p[0]) / freq;
	}
	__int64 hz_cpu() { // замер частоты
		clock_t t_clock;
		__int64 t_tsc;
		t_clock = clock() + CLOCKS_PER_SEC;
		t_tsc = __rdtsc(); // взять TSC
		while (clock() < t_clock); // отсчет одной секунды
		return (__rdtsc() - t_tsc); // частота в герцах
	}
};

int main()
{
	Clock  cl;
	QPC  qpc;
	TSC  tsc;
	setlocale(LC_CTYPE, "rus");
	int sz = 0;
	for (sz = 0; cl.dTime <= 3; sz += 100) {
		cl.SearchPoint(sz);
	}
	cout << "Clock" << endl;
	for (int i = 0; i <= sz * 2; i += (sz / 10)) {
		cl.SearchPoint(i);
		cout << i << "\t" << cl.time <<endl;
	}
	cout << "QPC" << endl;
	cout << "arcount" << "\t" << "time" << endl;
	for (int sz = 10; qpc.counter <= 200; sz += 40) {
		qpc.measSeries(sz);
	}
	/*
	for (int sz = SIZE_DARR; qpc.counter <= 200; sz -= 40) {
		qpc.measSeries(sz);
	}
*/
	cout << cl.name << ": " << cl.dTime << "\n";
	tsc.measure(10);
	system("pause");
	return 0;
}