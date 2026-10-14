# DUEÑO: P5 · Tareas: T02 (repo y guía de compilación)
# Uso:  make        → compila build/gestion_escolar
#       make run    → compila y ejecuta (desde la raíz del repo)
#       make clean  → borra build/

CXX      ?= g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pthread
SRC      := $(wildcard backend/src/*.cpp)
OBJ      := $(patsubst backend/src/%.cpp,build/obj/%.o,$(SRC))

ifeq ($(OS),Windows_NT)
    CXXFLAGS += -D_WIN32_WINNT=0x0A00
    LIBS     := -lws2_32 -lwsock32 -lcrypt32
    BIN      := build/gestion_escolar.exe
else
    LIBS     :=
    BIN      := build/gestion_escolar
endif

.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

build/obj/%.o: backend/src/%.cpp $(wildcard backend/src/*.hpp)
	@mkdir -p build/obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf build
