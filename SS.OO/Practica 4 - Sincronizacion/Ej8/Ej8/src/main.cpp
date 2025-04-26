#include <iostream>
#include <semaphore>
#include <thread>

std::binary_semaphore producer_semaphore(1);
std::binary_semaphore consumer_semahpore(0);
std::binary_semaphore b_semaphore(1);
std::binary_semaphore c_semaphore(0);

void procesoB()
{
	while(true)
	{
		b_semaphore.acquire();
		consumer_semahpore.acquire();

		printf("Soy B y consumo\n");

		producer_semaphore.release();
		c_semaphore.release();
	}
}

void procesoC()
{
	while(true)
	{
		c_semaphore.acquire();
		consumer_semahpore.acquire();

		printf("Soy C y consumo\n");
		printf("Soy C y consumo\n");

		producer_semaphore.release();
		b_semaphore.release();
	}
}

void productor()
{
	while(true)
	{
		producer_semaphore.acquire();

		printf("Soy el productor y produzco\n");

		std::this_thread::sleep_for(std::chrono::seconds(1));

		consumer_semahpore.release();
	}
}

int main(int argc, char** argv)
{
	std::thread procesos[3]
	{
		std::thread(productor),
		std::thread(procesoB),
		std::thread(procesoC)
	};

	for (auto& proceso : procesos)
		proceso.join();

	return 0;
}
