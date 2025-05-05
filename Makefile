CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude `pkg-config --cflags glib-2.0`
LDFLAGS = `pkg-config --libs glib-2.0`

all: folders dserver dclient

dserver: bin/dserver
dclient: bin/dclient

folders:
	@mkdir -p src include obj bin

bin/dserver: obj/utils.o obj/mensagem.o obj/comandos.o obj/metadados.o obj/serveraux.o obj/executor.o obj/dserver.o
	$(CC) $(LDFLAGS) $^ -o $@

bin/dclient: obj/utils.o obj/mensagem.o obj/comandos.o obj/dclient.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/comandos.o: src/comandos.c include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/metadados.o: src/metadados.c include/metadados.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/serveraux.o: src/serveraux.c include/serveraux.h include/metadados.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/mensagem.o: src/mensagem.c include/mensagem.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/executor.o: src/executor.c include/executor.h include/mensagem.h include/serveraux.h include/metadados.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dserver.o: src/dserver.c include/executor.h include/mensagem.h include/serveraux.h include/metadados.h include/comandos.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/dclient.o: src/dclient.c include/comandos.h include/mensagem.h include/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/* bin/* obj bin

debug: clean
	$(MAKE) all CFLAGS="$(CFLAGS) -g" LDFLAGS="$(LDFLAGS)"