# Project: gluxEngine
# Compiler: Default GCC compiler
# Compiler Type: MingW 3

CPP       = g++
OBJ       = bordel/main.o bordel/ipsm2.o bordel/font.o bordel/light.o bordel/material.o bordel/object.o bordel/scene.o bordel/shadow.o bordel/texture.o bordel/render_target.o bordel/camera.o bordel/material_generator.o bordel/load3DS.o bordel/compute.o bordel/loadScene.o bordel/draw.o
LINKOBJ   = bordel/main.o bordel/ipsm2.o bordel/font.o bordel/light.o bordel/material.o bordel/object.o bordel/scene.o bordel/shadow.o bordel/texture.o bordel/render_target.o bordel/camera.o bordel/material_generator.o bordel/load3DS.o bordel/compute.o bordel/loadScene.o bordel/draw.o
LIBS      = -lGL -lGLU -lX11 -L/usr/X11R6/lib -L/usr/lib/nvidia -L/usr/local/lib -lOpenCL `sdl-config --cflags --libs` -l3ds -lGLEW -lAntTweakBar 
BIN       = gluxEngine
CXXFLAGS  = -I./LIB/include 
RM        = rm -f
LINK      = g++

.PHONY: all all-before all-after clean clean-custom
all: all-before $(BIN) all-after

clean: clean-custom
	$(RM) $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(LINK) $(LINKOBJ) -o bin/gluxEngine $(LIBS) 

bordel/main.o: src/main.cpp
	$(CPP) -c src/main.cpp -o bordel/main.o $(CXXFLAGS)

bordel/ipsm2.o: src/ipsm2.cpp
	$(CPP) -c src/ipsm2.cpp -o bordel/ipsm2.o $(CXXFLAGS)

bordel/font.o:src/glux_engine/font.cpp
	$(CPP) -c src/glux_engine/font.cpp -o bordel/font.o $(CXXFLAGS)

bordel/light.o: src/glux_engine/light.cpp
	$(CPP) -c src/glux_engine/light.cpp -o bordel/light.o $(CXXFLAGS)

bordel/mat.o:src/glux_engine/mat.cpp
	$(CPP) -c src/glux_engine/mat.cpp -o bordel/mat.o $(CXXFLAGS)

bordel/material.o: src/glux_engine/material.cpp
	$(CPP) -c src/glux_engine/material.cpp -o bordel/material.o $(CXXFLAGS)

bordel/material_generator.o: src/glux_engine/material_generator.cpp
	$(CPP) -c src/glux_engine/material_generator.cpp -o bordel/material_generator.o $(CXXFLAGS)
	
bordel/object.o: src/glux_engine/object.cpp
	$(CPP) -c src/glux_engine/object.cpp -o bordel/object.o $(CXXFLAGS)
	
bordel/load3DS.o: src/glux_engine/load3DS.cpp
	$(CPP) -c src/glux_engine/load3DS.cpp -o bordel/load3DS.o $(CXXFLAGS)

bordel/scene.o: src/glux_engine/scene.cpp
	$(CPP) -c src/glux_engine/scene.cpp -o bordel/scene.o $(CXXFLAGS)
	
bordel/render_target.o: src/glux_engine/render_target.cpp
	$(CPP) -c src/glux_engine/render_target.cpp -o bordel/render_target.o $(CXXFLAGS)
	
bordel/camera.o: src/glux_engine/camera.cpp
	$(CPP) -c src/glux_engine/camera.cpp -o bordel/camera.o $(CXXFLAGS)

bordel/shadow.o: src/glux_engine/shadow.cpp
	$(CPP) -c src/glux_engine/shadow.cpp -o bordel/shadow.o $(CXXFLAGS)

bordel/texture.o: src/glux_engine/texture.cpp
	$(CPP) -c src/glux_engine/texture.cpp -o bordel/texture.o $(CXXFLAGS)

bordel/draw.o: src/glux_engine/draw.cpp
	$(CPP) -c src/glux_engine/draw.cpp -o bordel/draw.o $(CXXFLAGS)

bordel/compute.o: src/glux_engine/compute.cpp
	$(CPP) -c src/glux_engine/compute.cpp -o bordel/compute.o $(CXXFLAGS)

bordel/loadScene.o: src/glux_engine/loadScene.cpp
	$(CPP) -c src/glux_engine/loadScene.cpp -o bordel/loadScene.o $(CXXFLAGS)

