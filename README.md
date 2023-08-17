# README
    This repository contains an example of a message sender and receiver using the Linux DBus.
  
## DBus
    DBus is a type of IPC that supports functions for communication between applications.
  
### Object Path
    Set up an virtual path so that you can identify the path to your application. This is the object path.
  
### Interface
    DBus interfaces are "methods" and "signals" for calling DBus objects. The user can call the methods and signals using the "interface".
  
#### Methods and Signals
    A method is an action that takes input and output via parameters. A signal is an operation that subscribes to a single bus and can only output via a signal.
  
## Linux dbus with C
    To use Linux dbus in c, the sd_bus.h header is required, and it required as a compile option.
<code> gcc ${filename}.c -o ${filename} 'pkg-config --cflags --libs libsystemd'  </code>
  
### Send
    The sender can call a method on the server via the server's bus name, object path, interface name, and method name, which allows it to connect to the server and send messages.
  
### Receive
    The receiver similarly looks for the server's object path and interface name, but instead of method names, it looks for signals that are members of the method to receive the message.