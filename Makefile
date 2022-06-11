CFLAGS += -Iinclude -g -Wall

all: clean main.c src/util.c
	clang $(CFLAGS) main.c src/util.c -lm -o custom_ls

valgrind: all
	valgrind --leak-check=full ./custom_ls
	valgrind --leak-check=full ./custom_ls -i ./ -l

address_sanitizer: CFLAGS += -fsanitize=undefined,address
address_sanitizer: all
	./custom_ls -l

memory_sanitizer: CFLAGS += -fsanitize=undefined,memory
memory_sanitizer: all
	./custom_ls -l

sanitizers: address_sanitizer memory_sanitizer

clean:
	rm -f ./custom_lsa