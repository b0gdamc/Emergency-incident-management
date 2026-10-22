build:
	gcc -o tema1 main.c tema1.c
run:
	./tema1
clean:
	rm tema1
valgrind:
	valgrind --leak-check=yes --track-origins=yes ./tema1
pack:
	zip 314CB_MIRAUTA_BOGDAN-CONSTANTIN_tema1.zip main.c tema1.c tema1.h README Makefile