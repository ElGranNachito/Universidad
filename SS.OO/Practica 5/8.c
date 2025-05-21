#define LOC_TEXT_POINTER 1
#define LOC_TEXT_SIZE 2
#define LOC_CTRL 3

#define LOW_INK 1
#define READY 2
#define PRINTING 4
#define FINISHED 8
#define START 16

#define HP_LOW_INK_INT 1
#define HP_FINISHED_INT 2

#define MAX_ATTEMPTS 5

semaphore_t mtx;
semaphore_t status_changed_sem;

void init()
{
    sem_init(mtx, 1);
    sem_init(status_changed_sem, 0);

    request_irq(HP_LOW_INK_INT, low_ink_handler);
    request_irq(HP_FINISHED_INT, finished_handler);
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

void write(char* data, int size)
{
    mtx.wait();

    char* local_buffer = kalloc(size);

    copy_from_user(local_buffer, data, size);

    bool finished_printing = false;
    int attempt_counter = 0;

    while (!finished_printing && attempt_counter < MAX_ATTEMPTS)
    {
        OUT(LOC_TEXT_POINTER, local_buffer);
        OUT(LOC_TEXT_SIZE, size);
        OUT(LOC_CTRL, START);

        status_changed_sem.wait();

        finished_printing = (IN(LOC_CTRL) == FINISHED);

        attempt_counter++;
    }
    
    kfree(local_buffer);

    mtx.signal();
}

void remove()
{
    free_irq(HP_LOW_INK_INT, low_ink_handler);
    free_irq(HP_FINISHED_INT, finished_handler);
}

void low_ink_handler()
{
    status_changed_sem.signal();
}

void finished_handler()
{
    status_changed_sem.signal();
}