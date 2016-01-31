#include "pulse.go.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

pa_mainloop     *mainloop = NULL;
pa_context      *context  = NULL;
pa_mainloop_api *api      = NULL;
char            *server   = NULL;

int pulsetest_quit(int code) {
    if (api) {
        api->quit(api, code);
    }

    if (context) {
        pa_context_unref(context);
    }

    if (mainloop) {
        pa_signal_done();
        pa_mainloop_free(mainloop);
    }

    return code;
}

// void pulsetest_cb_server_info(pa_context *ctx, const pa_server_info *info, void* userdata) {
//     if(info){
//         printf("\n-- SERVER INFO --\n\n");
//         printf("   Server String: %s\n", pa_context_get_server(ctx));
//         printf("    Default Sink: %s\n", info->default_sink_name);
//     }else{
//         printf("DEBUG: server info failed\n");
//         pulsetest_quit(6);
//     }
// }

pa_context* pulse_get_context() {
    return context;
}

void pulse_context_state_callback(pa_context *ctx, void *goClient) {
    switch (pa_context_get_state(ctx)) {
    case PA_CONTEXT_CONNECTING:
        printf("DEBUG: context connecting...\n");
        break;
    case PA_CONTEXT_AUTHORIZING:
        printf("DEBUG: context auth...\n");
        break;
    case PA_CONTEXT_SETTING_NAME:
        printf("DEBUG: context setting name...\n");
        break;

    case PA_CONTEXT_READY:
        printf("DEBUG: context ready!\n");
        go_clientStartupDone(goClient, "");
        break;

    case PA_CONTEXT_TERMINATED:
        printf("DEBUG: context terminated\n");
        go_clientStartupDone(goClient, "Connection terminated");
        break;
    case PA_CONTEXT_FAILED:
        printf("DEBUG: context failed\n");
        go_clientStartupDone(goClient, "Connection failed");
        break;
    default:
        printf("DEBUG: connection failure: %s", pa_strerror(pa_context_errno(ctx)));
        pulsetest_quit(5);
        break;
    }
}

// void pulse_get_server_info_callback(pa_context *ctx, const pa_server_info *info, void *goOpCall) {
//     go_getServerInfoResponse(info, goOpCall);
// }


int pulse_mainloop_start(void *goClient) {
    int code = 0;

    if (!(mainloop = pa_mainloop_new())) {
        printf("pa_mainloop_new() failed.\n");
        return pulsetest_quit(1);
    }

    printf("DEBUG: mainloop created\n");


    api = pa_mainloop_get_api(mainloop);
    printf("DEBUG: api retrieved\n");

    context = pa_context_new(api, "pulsetest");
    printf("DEBUG: context created\n");


    pa_context_set_state_callback(context, pulse_context_state_callback, goClient);

    printf("DEBUG: registered state callback\n");

    if (pa_context_connect(context, server, 0, NULL) < 0) {
        printf("pa_context_connect() failed: %s\n", pa_strerror(pa_context_errno(context)));
        return pulsetest_quit(2);
    }

    if (pa_mainloop_run(mainloop, &code) < 0) {
        printf("pa_mainloop_run() failed\n");
        return pulsetest_quit(code);
    }else{
        printf("DEBUG: mainloop running\n");
    }

    return code;
}