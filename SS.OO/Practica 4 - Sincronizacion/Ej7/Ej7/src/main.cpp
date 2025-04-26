#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>

std::vector<std::thread> procesos;
std::vector<std::shared_ptr<std::counting_semaphore<1>>> semaforos;

void proceso(size_t pid)
{
	semaforos[pid]->acquire();

	printf("Soy el proceso %llu\n", pid);

	semaforos[(pid + 1) % procesos.size()]->release();
}

int main(int argc, char** argv)
{
	int n = 0, i = 0;

	std::cin >> n >> i;

	procesos.reserve(n);
	semaforos.reserve(n);

	for (int j = 0; j < n; ++j)
	{
		procesos.emplace_back(proceso, j);
		semaforos.emplace_back(std::make_shared<std::counting_semaphore<1>>(0));
	}

	semaforos[i]->release();

	for (auto& t : procesos)
		t.join();

	exit(0);
}