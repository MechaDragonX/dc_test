#include <kos.h>

// Use default init settings
KOS_INIT_FLAGS(INIT_DEFAULT);

int main(int arc, char *argv[])
{
    maple_device_t* controller = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if(!controller)
    {
        return -1;
    }

    // Game loop
    int running = 1;
    while(running)
    {
        cont_state_t* state = (cont_state_t*)maple_dev_status(controller);
        if(state && (state->buttons & CONT_A))
        {
            running = 0;
        }

        thd_sleep(20);
    }

    // Return to system menu
    arch_menu();

    return 0;
}
