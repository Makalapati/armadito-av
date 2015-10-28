#include <stdio.h>
#include "scan.h"
#include <windows.h> 
#include <conio.h>
#include <libuhuru-config.h>
#include <libuhuru/core.h>
#include <libuhuru/ipc.h>
#include "utils/json.h"

#include "named_pipe_client.h"

struct named_pipe_info {
	HANDLE * hPipe;
};

static void scan_callback(struct uhuru_report *report, void *callback_data)
{
	// unused currently
	//struct named_pipe_info *np_info = (struct named_pipe_info *)callback_data;

	char * response;
	const char * message;
	HANDLE * hPipe = (HANDLE*)malloc(sizeof(HANDLE));

	if (connect_to_IHM(report->scan_id, hPipe) < 0){
		printf("Error when trying to connect to \\\\.\\pipe\\IHM_scan_%d \n", report->scan_id);
		return;
	}

	message = json_get_report_msg(report);

	if (send_message_to_IHM(hPipe, (char*)message, &response) < 0){
		printf("Error when writing callback msg on Pipe");
		return;
	}

	// Traiter la r�ponse ici
	closeConnection_to_IHM(hPipe);
	printf("callback report sent to IHM !\n");

}

int start_new_scan(struct new_scan* scan, uhuru* uhuru)
{
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	
	struct named_pipe_info *np_info;
	struct uhuru_scan *u_scan;


	if (scan != NULL && scan->scan_id > 0 && scan->scan_path != NULL){

		// We test connection to IHM before launching a new scan
		printf("\n\n #### Start scanning (%d) %s ####\n", scan->scan_id, scan->scan_path);
		if (connect_to_IHM(scan->scan_id, &hPipe) < 0){
			printf("Error when trying to connect to \\\\.\\pipe\\IHM_scan_%d \n", scan->scan_id);
			return -1;
		}

		closeConnection_to_IHM(&hPipe);



		// Not really used currently
		np_info = (struct named_pipe_info*)malloc(sizeof(struct named_pipe_info));
		np_info->hPipe = &hPipe;
	
		printf("--- uhuru_scan_new() ---\n");
		u_scan = uhuru_scan_new(uhuru, scan->scan_id, scan->scan_path, UHURU_SCAN_RECURSE);

		printf("--- uhuru_scan_add_callback() ---\n");
		uhuru_scan_add_callback(u_scan, scan_callback, np_info);

		printf("--- uhuru_scan_run() ---\n");
		uhuru_scan_run(u_scan);

		printf("--- uhuru_scan_free() ---\n");

		// fake callback
		/*
		struct uhuru_report *report = (struct uhuru_report *)malloc(sizeof(struct uhuru_report));
		report->mod_name = "clamav";
		report->mod_report = "Trojan.Mal32-77";
		report->action = UHURU_ACTION_QUARANTINE;
		report->path = "C://cygwin64//home//malware.exe";
		report->status = UHURU_MALWARE;

		scan_callback(report, np_info);
		free(report);
		*/

		// free and close all
		uhuru_scan_free(u_scan);
		free(np_info);
	} 


	return 0;
}
