#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;
    sd_bus *bus = NULL;
    char input_buffer[256];
    const char *helloworld;
    int ret;

    /*
     * session 버스에 연결
     */
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon\n");
        goto finish;
    }

    /*
     * D-Bus 메시지 호출
     */
    while (1)
    {

    
    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        fprintf(stderr, "Failed to read input\n");
        goto finish;
    }
    char *end = strchr(input_buffer,'q');
    if(end != NULL){
        if(strcmp(end,"q\n")==0){
            break;
        }
    }
    char *newline = strchr(input_buffer, '\n');
    if (newline)
        *newline = '\0';


    ret = sd_bus_call_method(bus,
            "com.example.Server",   /* destination, server's bus name */
            "/com/example/method",    /* path, object path */
            "com.example.inter",     /* interface name */
            "send",                /* method name */
            &error,                 /* 실패 시 에러메시지 */
            &m,                     /* 성공 시 메시지 */
            "s",                    /* 인자형식(string) */
            input_buffer); /* 인자 */

    if (ret < 0) {
        fprintf(stderr, "Failed to issue method call: %s\n", error.message);
        goto finish;
    }

    /*
     * 성공 메시지를 읽어서 출력
     */
    ret = sd_bus_message_read(m, "s", &helloworld);
    if (ret < 0) {
        fprintf(stderr, "Failed to parse response message\n");
        goto finish;
    }

    printf("Received from server: %s\n", helloworld);
    
    }
finish:
    sd_bus_error_free(&error);
    sd_bus_message_unref(m);
    sd_bus_unref(bus);

    return 0;
}
