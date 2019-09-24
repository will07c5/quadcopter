
#include <OsalPrint.h>
#include <Memory.h>

#include <IpcUsr.h>
#include <ProcMgr.h>
#include <ti/ipc/MultiProc.h>

#include <sys/types.h>
#include <stdio.h>
#include <signal.h> 
int run_program = 1;

static void stop_program(int sig)
{
	run_program = 0;
}

int main(int argc, char **argv)
{
	Int status;
	Ipc_Config config;
	ProcMgr_Handle procMgrHandle;
	Char *procName;
	UInt16 procId;
	ProcMgr_AttachParams attachParams;
	ProcMgr_State state;
	Char *imageName = "quadcopter.xem3";
	UInt32 entryPoint; 
	UInt32 fileId;
	ProcMgr_StartParams startParams;
	ProcMgr_StopParams stopParams;

	signal(SIGINT, stop_program);

	Ipc_getConfig(&config);
	status = Ipc_setup(&config);
	if (status < 0) {
		printf("Ipc_setup failed %d\n", status);
		return 1;
	}

	procId = MultiProc_getId("SysM3");

	status = ProcMgr_open(&procMgrHandle, procId);
	if (status < 0) {
		printf("ProcMgr_open failed %d\n", status);
		goto exit_ipc_destroy;
	}

	status = ProcMgr_attach (procMgrHandle, &attachParams);
	if (status < 0) {
		printf ("ipcSetup: ProcMgr_attach failed [0x%x]\n", status);
		goto exit_procmgr_close;
	}
	else {
		printf ("ipcSetup: ProcMgr_attach status: [0x%x]\n", status);
		state = ProcMgr_getState (procMgrHandle);
		printf ("ipcSetup: After attach: ProcMgr_getState\n"
				  "    state [0x%x]\n", state);
	}

	status = ProcMgr_load (procMgrHandle, imageName, 2, &imageName,
							  &entryPoint, &fileId, procId);
	if(status < 0) {
		printf("ipc_setup: Error in ProcMgr_load: [0x%x]\n", status);
		goto exit_procmgr_detach;
	}

	startParams.proc_id = procId;
	status = ProcMgr_start (procMgrHandle, entryPoint, &startParams);
	if (status < 0) {
		printf ("ipc_setup: Error in ProcMgr_start [0x%x]\n", status);
		goto exit_procmgr_detach;
	}

	while (run_program) { sleep(1); }

	stopParams.proc_id = procId;
	status = ProcMgr_stop(procMgrHandle, &stopParams);

exit_procmgr_detach:
	ProcMgr_detach(procMgrHandle);

exit_procmgr_close:
	ProcMgr_close(&procMgrHandle);

exit_ipc_destroy:
	Ipc_destroy();

	return 0;
}
