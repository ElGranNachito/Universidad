#define DOR_IO 1
#define ARM 2
#define SEEK_SECTOR 3

#define DOR_STATUS 4
#define ARM_STATUS 5
#define DATA_READY 6

#define SECTOR_SIZE 4096

#pragma region 7a

semaphore_t sem;

void init()
{
    sem_init(sem, 1);

    OUT(DOR_IO, 0);
}

void open()
{

}

void close()
{

}

void read(int* data)
{
    
}

void write(int sector, int* data)
{
    sem.wait();

    //Nos aseguramos de que el motor este encendido
    OUT(DOR_IO, 1);

    OUT(ARM, floor(sector / cantidad_de_sectores_por_pista()));

    OUT(SEEK_SECTOR, cantidad_de_sectores_por_pista() % sector);

    //Esperamos siempre 50ms para asegurarnos que el motor tenga velocidad suficiente
    sleep(50);

    //Esperamos a que el brazo este en la pista correcta
    while(!IN(ARM_STATUS)){}

    void* local_data_buffer = kalloc(SECTOR_SIZE);

    copy_from_user(local_data_buffer, data, SECTOR_SIZE);

    escribir_datos(local_data_buffer);

    kfree(local_data_buffer);

    while(!IN(DATA_READY)){}

    OUT(DOR_IO, 0);

    sleep(200);

    sem.signal();
}

void remove()
{

}

int cantidad_de_sectores_por_pista(){
    //Implementada
}

void escribir_datos(void* src){
    //Implementada
}

void sleep(int ms){
    //Implementada
}

#pragma endregion

#pragma region 7b

#define STATUS_INT 6
#define TIMER_INT 7

semaphore_t sem_access;
semaphore_t sem_timer;
semaphore_t sem_arm;
semaphore_t sem_data;

atomic<int> ticks_to_wait;
atomic<bool> arm_ready;
atomic<bool> data_ready;

void init()
{
    sem_init(sem_access, 1);
    sem_init(sem_timer, 1);
    sem_init(sem_arm, 1);
    sem_init(sem_data, 1);

    ticks_to_wait       = 0;
    arm_ready           = false;
    data_ready          = false;

    OUT(DOR_IO, 0);

    request_irq(STATUS_INT, status_changed_handler);
    request_irq(TIMER_INT, timer_tick_handler);
}

void open()
{

}

void close()
{

}

void read(int* data)
{
    
}

void write(int sector, int* data)
{
    sem_access.wait();

    arm_ready.set(0);
    data_ready.set(0);

    //Nos aseguramos de que el motor este encendido
    OUT(DOR_IO, 1);

    OUT(ARM, floor(sector / cantidad_de_sectores_por_pista()));

    OUT(SEEK_SECTOR, cantidad_de_sectores_por_pista() % sector);

    //Esperamos dos ticks para asegurarnos que pasen al menos 50ms y que el motor tenga suficiente velocidad
    ticks_to_wait.set(2);

    sem_timer.wait();
    sem_arm.wait();

    void* local_data_buffer = kalloc(SECTOR_SIZE);

    copy_from_user(local_data_buffer, data, SECTOR_SIZE);

    escribir_datos(local_data_buffer);

    kfree(local_data_buffer);

    sem_data.wait();

    OUT(DOR_IO, 0);

    //Esperamos al menos 200ms a que el motor se apague completamente
    ticks_to_wait.set(5);

    sem_timer.wait();
    sem_access.signal();
}

void remove()
{
    free_irq(STATUS_INT);
    free_irq(TIMER_INT);
}

void status_changed_handler()
{
    if(!arm_ready.load() && IN(ARM_STATUS)){
        arm_ready.set(1);
        sem_arm.signal();
    } else if (!data_ready.load() && IN(DATA_READY)){
        data_ready.set(1);
        sem_data.signal();
    }
}

void timer_tick_handler()
{
    if(ticks_to_wait.get_and_dec() == 1){
        sem_timer.signal();
    }
}

int cantidad_de_sectores_por_pista(){
    //Implementada
}

void escribir_datos(void* src){
    //Implementada
}

void sleep(int ms){
    //Implementada
}

void reset_timer(){
    //Supongo que existe implementada, algo como en el punto 2
}

#pragma endregion