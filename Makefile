################################################################################
# IZG project, framework compilation
################################################################################

################################################################################
# promenne prekladu

ifeq ($(OS), Windows_NT)      # Windows/MinGW
    INCS = -I../SDL-1.2.15/include -IQ:/mingw/SDL/SDL
    LIBS = -lmingw32 -lSDLmain -lSDL -L../SDL-1.2.15/lib -LQ:/mingw/SDL
    CFLAGS = -g3 -Wall $(INCS) -Wno-unused-function -fmessage-length=0
    REMOVE = del
    BIN = izg_proj.exe
else ifeq ($(shell uname), Darwin) # Mac OS X
    INCS = -I/usr/local/include/SDL
    LIBS = `sdl-config --libs`
    CFLAGS = -g3 -Wall $(INCS) -Wno-unused-function -fmessage-length=0 `sdl-config --cflags`
    REMOVE = rm -f
    BIN = izg_proj
else                          # Linux/GCC
    INCS = -I/usr/include/SDL
    LIBS = -lSDL -lm
#    LIBS=-m32 -lSDL
#    LIBS=-m64 -lSDL
    CFLAGS = -g3 -Wall $(INCS) -Wno-unused-function -fmessage-length=0
#    CFLAGS = -m32 -g3 -Wall $(INCS) -Wno-unused-function -fmessage-length=0
#    CFLAGS = -m64 -g3 -Wall $(INCS) -Wno-unused-function -fmessage-length=0
    REMOVE = rm -f
    BIN = izg_proj
endif

CC = gcc

################################################################################
# obecny cil kompilace
all: $(BIN)

################################################################################
# linkovani vysledneho souboru
$(BIN):	main.o student.o render.o transform.o model.o vector.o color.o bmp.o
	$(CC) main.o student.o render.o transform.o model.o vector.o color.o bmp.o -o $(BIN) $(LIBS)
#	strip $(BIN)

################################################################################
# kompilace hlavniho souboru
main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

################################################################################
# kompilace studentskeho souboru
student.o: student.c student.h
	$(CC) -c student.c -o student.o $(CFLAGS)

################################################################################
# kompilace dilcich souboru
render.o: render.c render.h
	$(CC) -c render.c -o render.o $(CFLAGS)
transform.o: transform.c transform.h
	$(CC) -c transform.c -o transform.o $(CFLAGS)
model.o: model.c model.h
	$(CC) -c model.c -o model.o $(CFLAGS)
vector.o: vector.c vector.h
	$(CC) -c vector.c -o vector.o $(CFLAGS)
color.o: color.c color.h
	$(CC) -c color.c -o color.o $(CFLAGS)
bmp.o: bmp.c bmp.h
	$(CC) -c bmp.c -o bmp.o $(CFLAGS)

################################################################################
# cil vycisteni souboru prekladu
clean: clear
clear:
	$(REMOVE) *.o $(BIN)

################################################################################
################################################################################
