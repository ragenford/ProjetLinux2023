CC=gcc
CFLAGS=-Wall -Wextra -std=c99

all: controller zombie labo

info.o: info.h info.c
	$(CC) $(CFLAGS) -c info.c -o info.o

controller.o: info.h controller.c
	$(CC) $(CFLAGS) -c controller.c -o controller.o

zombie.o: info.h zombie.c
	$(CC) $(CFLAGS) -c zombie.c -o zombie.o

labo.o: info.h labo.c
	$(CC) $(CFLAGS) -c labo.c -o labo.o

controller: controller.o info.o
	$(CC) $(CFLAGS) controller.o info.o -o controller

zombie: zombie.o info.o
	$(CC) $(CFLAGS) zombie.o info.o -o zombie

labo: labo.o info.o
	$(CC) $(CFLAGS) labo.o info.o -o labo

clean:
	rm -f *.o controller zombie labo
