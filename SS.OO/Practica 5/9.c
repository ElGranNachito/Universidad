//Constantes

#define KEYCODE_MASK 0x3FFF
#define TARGET_MASK 0xFFFF ^ KEYCODE_MASK

#define READ_OK 1
#define READ_FAILED 2
#define APP_UP 4
#define APP_DOWN 8

//Registros

#define KEYB_REG_DATA 1
#define KEYB_REG_CONTROL 2
#define KEYB_REG_AUX 4
#define KEYB_REG_STATUS 8

//Interrupciones

#define IRQ_KEYB 1

char input_mem[3][100];
char buffer_lectura[3][1000];
atomic_int buffer_start[3];
atomic_int buffer_end[3];
bool procesos_activos[3];

semaphore_t read_mtx[3];
semaphore_t procesos_activos_mtx;

int get_buffer_length(int i){
    //Implementada
}

bool write_to_buffer(int i, char src){
    //Implementada
}

bool write_to_all_buffers(char src){
    //Implementada
}

void copy_from_buffer(int i, char* dst, int size){
    //Implementada
}

int keycode_2_ascii(int keycode){
    //Implementada
}

void irq_keyboard_handler(){
    uint32_t keypress_event_args    = IN(KEYB_REG_DATA);
    uint32_t keycode                = keypress_event_args & KEYCODE_MASK;
    uint32_t target_process         = keypress_event_args & TARGET_MASK;

    bool was_key_read_successfuly = false;   

    if(target_process != 0)
    {
        was_key_read_successfuly = write_to_buffer(target_process, keycode_2_ascii(keycode));
        read_mtx[target_process - 1].signal();
    }
    else
    {
        was_key_read_successfuly = write_to_all_buffers(keycode_2_ascii(keycode));

        for(int i = 0; i < 3; ++i)
            read_mtx[i].signal();
    }

    OUT(KEYB_REG_CONTROL, was_key_read_successfuly ? READ_OK : READ_FAILED);
}

void load(){
    request_irq(IRQ_KEYB, irq_keyboard_handler);

    sem_init(read_mtx[0:3], 1);
    sem_init(procesos_activos_mtx, 1);
}

void open(){

    int new_app_id = 0;

    procesos_activos_mtx.wait();

    for(int i = 0; i < 3; ++i)
    {
        if(!procesos_activos[i]){
            new_app_id = i+1;
            procesos_activos[i] = true;
            break;
        }
    }

    procesos_activos_mtx.signal();

    //Si no habia espacio para la nueva aplicacion devolvemos un error
    if(new_app_id == 0)
        return -1;

    OUT(KEYB_REG_AUX, new_app_id);
    OUT(KEYB_REG_STATUS, APP_UP);
}

int Y(int id, char* buffer, int length) {

    //Nunca va a leer mas que la capacidad del buffer
    if(length > 1000)
        return 0;

    //Si no se han escrito tantos bytes como se piden, esperamos
    while(length > get_buffer_length(id))
        read_mtx[id-1].wait();

    void* local_buffer = kalloc(length);

    copy_from_buffer(id, local_buffer, length);

    copy_to_user(buffer, local_buffer, length);

    kfree(local_buffer);

    return length;
}

int write(char* input, int size, int proceso) {

    //No se puede escribir mas que la capacidad del buffer
    if(size > 100)
        return 0;

    copy_from_user(input_mem[proceso], input, size);
    return size;
}

void close(int id){
    OUT(KEYB_REG_AUX, id);
    OUT(KEYB_REG_STATUS, APP_DOWN);

    procesos_activos_mtx.wait();
    procesos_activos[id-1] = false;
    procesos_activos_mtx.signal();
}

void unload(){
    free_irq(IRQ_KEYB);
}