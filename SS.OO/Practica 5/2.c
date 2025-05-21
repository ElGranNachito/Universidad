#define CHRONO_CURRENT_TIME 1
#define CHRONO_CTRL 2

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
    int currentTime = IN(CHRONO_CURRENT_TIME);

    copy_to_user(data, &currentTime, sizeof(currentTime));
}

void write(int* data)
{
    OUT(CHRONO_CTRL, 1);
    OUT(CHRONO_CTRL, 0);
}

void remove()
{

}