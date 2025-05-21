#define BTN_STATUS 1
#define BTN_STATUS_PRESSED 0x00000001
#define BTN_STATUS_RESET 0x00000002

#define BTN_PRESSED 1

#pragma region BUSY_WAITING

void init()
{

}

void open()
{

}

void close()
{

}

void read(int* data)
{
    while(!IN(BTN_STATUS & BTN_STATUS_PRESSED)){}

    return BTN_PRESSED;
}

void write(int* data)
{
    OUT(BTN_STATUS, BTN_STATUS_RESET);
}

void remove()
{

}

#pragma endregion

#pragma region INTERRUPT

#define KEY_PRESSED_INT 7

#define BTN_INT 0x00000004

semaphote_t sem;

void init()
{
    sem_init(sem, 0);

    request_irq(KEY_PRESSED_INT, key_pressed_handler);
}

void open()
{
    free_irq(KEY_PRESSED_INT);
}

void close()
{

}

void read(int* data)
{
    OUT(BTN_STATUS, BTN_INT);
    sem.wait();

    int read_data = IN(BTN_STATUS) & BTN_STATUS_PRESSED;

    copy_to_user(data, read_data);
}

void write(int* data)
{
    OUT(BTN_STATUS, BTN_STATUS_RESET);
}

void remove()
{

}

void key_pressed_handler()
{
    //Esto no va a funcionar bien si se acumulan varios reads, pues desactivamos el handler de interrupts de todos los llamados despues del primero.
    //Habria que llevar un contandor u otro semaforo mas para hacerlo bien
    OUT(BTN_STATUS, IN(BTN_STATUS) ^ BTN_INT);
    sem.signal();
}

#pragma endregion