#include <iostream>
#include <thread>

int x = 0;
int y = 0;

void procesoA()
{
	x++;
	printf("%d\n", x);
}

void procesoB()
{
	x++;
}

void procesoC()
{
	for(; x < 4; x++)
	{
		y = 0;
		printf("%d", x);
		y = 1;
	}
}

void procesoD()
{
	while(x < 4)
	{
		if (y == 1)
			printf("a");
	}
}

int main(int argc, char** argv)
{
	auto t1 = std::thread(procesoA);
	auto t2 = std::thread(procesoB);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	t1.join();
	t2.join();

	x = 0;

	printf("--------\n");

	auto t3 = std::thread(procesoC);
	auto t4 = std::thread(procesoD);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	t3.join();
	t4.join();

	exit(0);
}

/*
 *a) Para cada caso hay varias salidas posibles
 *
 *b)
 *
 *En el primer caso podria ser que el procesoA imprima 1 o 2 dependiendo de si el procesoB se llega a ejecutar antes o no.
 *
 *En el segundo caso podria ocurrir que se impriman una o mas 'a' entre cada numero o al final.
 *
 */