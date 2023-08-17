#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

static int handle_broadcast_signal( sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *message;
    int r;

    r = sd_bus_message_read(m, "s", &message);
    if (r < 0) {
        fprintf(stderr, "Failed to parse broadcast signal: %s\n", strerror(-r));
        return r;
    }

    printf("Received broadcast signal: %s\n", message);

    return 0;
}

int main() {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int ret;

    /*
     * session 버스에 연결
     */
    ret = sd_bus_default_user(&bus);
    if (ret < 0) {
        fprintf(stderr, "Failed to connect to dbus-daemon\n");
        goto finish;
    }
    
    // Request bus name on D-Bus (set your own unique bus name)
    ret = sd_bus_request_name(bus, "com.example.Client3", 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-ret));
        goto finish;
    }

    /*
     * D-Bus 신호 핸들링을 위한 슬롯 등록
     */
    ret = sd_bus_add_match(bus, NULL,
                           "interface='com.example.inter',member='signal',path='/com/example/method',type='signal'", handle_broadcast_signal, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to add match rule: %s\n", strerror(-ret));
        goto finish;
    }

    printf("Waiting for broadcast signals...\n");

    // Process incoming signals
    while (1) {
        ret = sd_bus_process(bus, NULL);
        if (ret < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-ret));
            break;
        }
        if (ret > 0) // handled a signal
            continue;
        ret = sd_bus_wait(bus, (uint64_t) -1);
        if (ret < 0) {
            fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-ret));
            break;
        }
    }

finish:
    sd_bus_error_free(&error);
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return 0;
}
