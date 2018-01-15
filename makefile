.PHONY: all
all: tcpcli tcpserv tags

tcpcli: tcpcli.o str_cli.o	./rdwr/readline.o ./rdwr/writen.o
	cc -o tcpcli tcpcli.o str_cli.o ./rdwr/readline.o ./rdwr/writen.o

tcpserv: tcpserv.o str_echo.o ./rdwr/readline.o ./rdwr/writen.o \
signal.o sigchldwait.o
	cc -o tcpserv tcpserv.o str_echo.o ./rdwr/readline.o ./rdwr/writen.o \
signal.o sigchldwait.o

.PHONY: tags
tags:
	ctags -R

tcpcli.o: tcpcli.c cli_serv.h

tcpserv.o: tcpserv.c cli_serv.h

str_cli.o: str_cli.c ./rdwr/rdwr.h

str_echo.o: str_echo.c ./rdwr/rdwr.h

readline.o: ./rdwr/readline.c

writen.o: ./rdwr/writen.c

readn.o: ./rdwr/readn.c

signal.o: signal.c cli_serv.h

sigchldwait.o: sigchldwait.c cli_serv.h

.PHONY: clean
clean:
	rm -f *.o ./rdwr/*.o tcpcli tcpserv
