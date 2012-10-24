# Project: gluxEngine
# Compiler: Default GCC compiler
# Compiler Type: MingW 3

OUTPUT=bordel

# Variable contains directories which contains source files
SOURCEDIRS=src src/glux_engine src/gen

SOURCE=${foreach var,${SOURCEDIRS},${wildcard ${var}/*.cpp ${var}/*.c}}
HEADER=${foreach var,${SOURCEDIRS},${wildcard ${var}/*.hpp ${var}/*.h}}
OBJ=${SOURCE:%.cpp=%.o}

CPP       = g++
LIBS      = -lGL -lGLU -lX11 -L/usr/X11R6/lib -L/usr/lib/nvidia -L/usr/local/lib -lOpenCL `sdl-config --cflags --libs` -l3ds -lGLEW -lAntTweakBar 
BIN       = gluxEngine
ifeq ($(OS), Windows_NT)
	CXXFLAGS  = -I./LIB/include -I./LIB/include/system
else
	CXXFLAGS = -I./LIB/include
endif
RM        = rm -f
LINK      = g++

.PHONY: all all-before all-after clean clean-custom
all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) ${OBJ} $(BIN)

${BIN}: ${OBJ}
	$(LINK) ${OBJ} -o bin/gluxEngine $(LIBS) 

%.o: %.cpp %.h
	${CPP} -o $@ ${CXXFLAGS} -c $< 


run: ${BIN}
	./bin/gluxEngine

