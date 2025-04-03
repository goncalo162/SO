CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

all: folders dserver dclient

dserver: bin/dserver
dclient: bin/dclient

folders:
	@mkdir -p src include obj bin

bin/dserver: obj/utils.o obj/comandos.o obj/executor.o obj/dserver.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/dclient: obj/utils.o obj/comandos.o obj/dclient.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/comandos.o: src/comandos.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/executor.o: src/executor.c include/executor.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dserver.o: src/dserver.c include/executor.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dclient.o: src/dclient.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/* bin/* obj bin

debug: clean
	$(MAKE) CFLAGS+="-g -Wall -Wextra -O2 -Iinclude" all
