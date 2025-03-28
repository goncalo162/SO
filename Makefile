CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --cflags --libs glib-2.0`

all: folders dserver dclient

dserver: bin/dserver
dclient: bin/dclient

folders:
	@mkdir -p src include obj bin

bin/dserver: obj/dserver.o obj/comandos.o obj/utils.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/dclient: obj/dclient.o obj/comandos.o obj/utils.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/dserver.o: src/dserver.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dclient.o: src/dclient.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/comandos.o: src/comandos.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/* bin/* obj bin

debug: clean
	$(MAKE) CFLAGS+="-g -Wall -Wextra -O2 -Iinclude" all
