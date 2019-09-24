#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Ipc.h>

#include "i2c.h"
Void tsk1_func(UArg arg0, UArg arg1)
{
    Int status;
    UInt16 hostProcId     = MultiProc_getId("MPU");

    System_printf( "Before Ipc_attach");
    System_flush();

    do {
        status = Ipc_attach(hostProcId);
    } while (status < 0);
    System_printf( "After Ipc_attach");
    System_flush();

    Ipc_detach(hostProcId);

    System_printf("Ipc_detach %d", status);
}

Int main(Int argc, Char* argv[])
{
    Ipc_start();

    System_printf( "Hello world" );

    //i2c_init(I2C4, I2C_SPEED_STANDARD);

    BIOS_start();
    return (0);
}
