#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>

#include "iopcdefine.h"
#include "iopccmd_service.h"
#include "iopcops_mq.h"
#include "iopcops_misc.h"

#define TASK_TIMER 	"tasktimer"

static struct task_t tasks[10];

uint8_t* get_name_timer()
{
    return TASK_TIMER;
}

uint8_t* get_help_timer()
{
    return "Timer task";
}

void* task_handler_timer(void* ptr)
{
    struct msg_t req;
    struct msg_t res;
    uint8_t group;
    uint8_t cmdno;
    uint8_t *req_data_ptr;
    struct task_t* task_ptr = NULL;
//    void *pres;
    int i = 0;
    //uint8_t *res_data_ptr;
//    pthread_t thread_id;

#if 1
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 1 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 2 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 3 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 4 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 5 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 6 1", NULL);
    GET_INSTANCE(ops_misc)->execute_cmd("/usr/local/bin/iopccmd raw 1 7 1", NULL);
#endif

    for(;;) {
        memset((uint8_t*)&req, 0, sizeof(struct msg_t));
        memset((uint8_t*)&res, 0, sizeof(struct msg_t));

        GET_INSTANCE(ops_mq)->get_from(TASK_TIMER, &req);

	memcpy(&res.hdr, &req.hdr, sizeof(struct msg_hdr_t));
	strcpy(res.hdr.src, req.hdr.dst);
        strcpy(res.hdr.dst, req.hdr.src);

	group = req.hdr.fn;
        cmdno = req.hdr.cmd;

	req_data_ptr = (uint8_t*)&req.data;
        //res_data_ptr = (uint8_t*)&res.data;

	if((group == 0xAA) && (cmdno == 0x55)) {
            uint8_t full_path[STR_LEN];
            uint8_t libname[STR_LEN];
	    uint8_t funcname[STR_LEN];
            struct req_background_t* req_background = (struct req_background_t*)req_data_ptr;

	    memset(full_path, 0, STR_LEN);
	    memset(libname, 0, STR_LEN);
	    memset(funcname, 0, STR_LEN);

	    memcpy(libname, &req_background->str[0], req_background->libname_len);
	    memcpy(funcname, &req_background->str[req_background->libname_len], req_background->funcname_len);
	    printf("[%d:%s]\n[%d:%s]\n", req_background->libname_len, libname, req_background->funcname_len, funcname);
	    sprintf(full_path, "%s/%s", USR_LIBS_PATH, libname);
	    void *dl_handle = dlopen((const char *)full_path, RTLD_LAZY);
            if(!dl_handle){
                printf("dlopen [%s] error[%s]\n", full_path, dlerror());
	        dlclose(dl_handle);
            }
	    task_start_fn_t task_start = (task_start_fn_t)dlsym(dl_handle, funcname);
	    for(i=0;i<10;i++) {
                task_ptr = &tasks[i];
		if(task_ptr->isused == 0) {
                    task_ptr->isused = 1;
		    pthread_t task_id;
		    task_ptr->dl_handle = dl_handle;
		    task_ptr->task_id = &task_id;
                    pthread_create(&task_id, NULL, task_start, NULL);
//		    pthread_join(task_id, &pres);
		    break;
		}
	    }
	}
	sleep(1);

	if(strcmp(res.hdr.dst, "") != 0) {
	    GET_INSTANCE(ops_mq)->set_to(res.hdr.dst, &res);
	}
    }
}

struct task_desc_t task_descs[] = {
    {"get_name_timer", "get_help_timer", "task_handler_timer"},
    {"", "", ""},
};
