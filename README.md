# gallows_game
This is the Gallows Game using a TCP socket communication. Clients can connect with a server in order to initiate a game. The game was developed in portuguese, but is not that difficult to play since it is pretty straight-forward.

In this repositories, there are 3 files, client.c, server.c and Makefile.

In order to compile the program, a simple make is necessary:

```sh
$ make all
```

A server must hold the connection between 2 clients, so first the server must be initialized with an arbitrary connection port:

```sh
$ ./server <port>
```

Following, the clients can initialize using the command:

```sh
$ ./client <server_ip> <server_port> <new_port>
```

Now, both clients can have fun! 
