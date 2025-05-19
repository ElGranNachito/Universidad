#include <semaphore>
#include <iostream>
#include <thread>

#define NPROCESOS 10

std::thread procesos[NPROCESOS];

std::binary_semaphore mtx(1);
std::counting_semaphore<NPROCESOS> semA(0);
std::counting_semaphore<NPROCESOS> semB(0);
std::counting_semaphore<NPROCESOS> semC(0);

int done = 0;

void func_a()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::cout << "Visitamos la funcion A\n";
}

void func_b()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::cout << "Visitamos la funcion B\n";
}

void func_c()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::cout << "Visitamos la funcion C\n";
}

void proceso()
{
	while(true)
	{
		std::cout << "Iniciamos el recorrido\n";

		mtx.acquire();
		func_a();
		if(++done == NPROCESOS)
		{
			done = 0;
			semA.release(NPROCESOS);
		}
		mtx.release();
		semA.acquire();

		mtx.acquire();
		func_b();
		if (++done == NPROCESOS)
		{
			done = 0;
			semB.release(NPROCESOS);
		}
		mtx.release();
		semB.acquire();

		mtx.acquire();
		func_c();
		if (++done == NPROCESOS)
		{
			done = 0;
			semC.release(NPROCESOS);
		}
		mtx.release();
		semC.acquire();
	}
	
}

int main(int argc, char** argv)
{
	for (int i = 0; i < NPROCESOS; ++i)
		procesos[i] = std::thread(proceso);

	for (int i = 0; i < NPROCESOS; ++i)
		procesos[i].join();
}