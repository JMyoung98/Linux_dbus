#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

static int method_json(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
    const char *str;
    int r;

    r = sd_bus_message_read(m, "s", &str);
    if (r < 0) {
        fprintf(stderr, "Failed to parse argument: %s\n", strerror(-r));
        return r;
    }

    printf("Received request: %s\n", str);

    // Respond back to the client
    sd_bus_message *reply = NULL;
    r = sd_bus_message_new_method_return(m, &reply);
    if (r < 0) {
        fprintf(stderr, "Failed to create reply message: %s\n", strerror(-r));
        return r;
    }

    r = sd_bus_message_append(reply, "s", str);
    if (r < 0) {
        fprintf(stderr, "Failed to append reply argument: %s\n", strerror(-r));
        sd_bus_message_unref(reply);
        return r;
    }

    r = sd_bus_send(NULL, reply, NULL);
    if (r < 0) {
        fprintf(stderr, "Failed to send reply: %s\n", strerror(-r));
    }

    // Broadcast the received message to all clients
    sd_bus *bus = (sd_bus *)userdata;
    r = sd_bus_emit_signal(bus,
                            "/com/example/method",
                            "com.example.inter",
                            "signal",
                            "s", str);
     if (r < 0) {
         fprintf(stderr, "Failed to emit broadcast signal: %s\n", strerror(-r));
     }
    sd_bus_message_unref(reply);

    return 0;
}

int main(int argc, char *argv[]) {
    sd_bus_slot *slot = NULL;
    sd_bus *bus = NULL;
    int r;

    // Connect to the user session bus
    r = sd_bus_open_user(&bus);
    if (r < 0) {
        fprintf(stderr, "Failed to connect to user session bus: %s\n", strerror(-r));
        goto finish;
    }

    // Install the D-Bus object at path "/com/example/Test"
    static const sd_bus_vtable vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_METHOD("send", "s", "s", method_json, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_VTABLE_END
    };

    r = sd_bus_add_object_vtable(bus,
                                 &slot,
                                 "/com/example/method",
                                 "com.example.inter",
                                 vtable,
                                 bus); // Pass 'bus' as userdata
    if (r < 0) {
        fprintf(stderr, "Failed to add object to bus: %s\n", strerror(-r));
        goto finish;
    }

    // Request bus name on D-Bus
    r = sd_bus_request_name(bus, "com.example.Server", 0);
    if (r < 0) {
        fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
        goto finish;
    }

    printf("Server is ready to receive requests...\n");

    // Process requests
    while (1) {
        r = sd_bus_process(bus, NULL);
        if (r < 0) {
            fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
            break;
        }
        if (r > 0) // handled a request
            continue;
        r = sd_bus_wait(bus, (uint64_t) -1);
        if (r < 0) {
            fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
            break;
        }
    }

finish:
    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
