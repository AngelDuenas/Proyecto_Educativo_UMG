# Makefile - Operación Pizarrón
# Uso (desde la raíz del repo, en la terminal Git Bash):
#   mingw32-make        compila a build/gestion_escolar.exe   (en Linux/macOS: make)
#   mingw32-make -j4    lo mismo, pero más rápido (compila varios archivos a la vez)
#   mingw32-make run    compila y ejecuta
#   mingw32-make clean  borra la carpeta build/

CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread -isystem backend/third_party -Ibackend/src
LDFLAGS  := -pthread
LDLIBS   :=
EJECUTABLE := build/gestion_escolar

ifeq ($(OS),Windows_NT)
	CXXFLAGS += -D_WIN32_WINNT=0x0A00
	LDLIBS   += -lws2_32 -lwsock32
	EJECUTABLE := build/gestion_escolar.exe
endif

FUENTES := $(wildcard backend/src/*.cpp)
OBJETOS := $(patsubst backend/src/%.cpp,build/%.o,$(FUENTES))
DEPENDENCIAS := $(OBJETOS:.o=.d)

all: $(EJECUTABLE)

$(EJECUTABLE): $(OBJETOS)
	$(CXX) $(OBJETOS) -o $@ $(LDFLAGS) $(LDLIBS)

build/%.o: backend/src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

run: all
	./$(EJECUTABLE)

clean:
	rm -rf build

-include $(DEPENDENCIAS)

.PHONY: all run clean
