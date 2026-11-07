IncDir			:= ../../gpk/llc ../../gpk/gpk_engine ../galaxy_hell
Libs			:= galaxy_hell.a llc.a
GccLibs			:= xcb xcb-image xkbcommon xkbcommon-x11 pthread
Platform		:= Linux.x64
ProjectName		:= galaxy_hell
Configuration	:= Release
ProjectDir		?= .
SolutionDir		?= $(ProjectDir)/..

C_DEFS			:= -DNOMINMAX -DGPK_XCB
OPT				:= 3

ifeq ($(DEBUG), 1)
	Configuration	= Debug
	OPT				= 0
	C_DEFS			+= -DDEBUG
endif

OutDir		:= $(SolutionDir)/../$(Platform).$(Configuration)
IntDir		:= $(SolutionDir)/../obj/$(Platform).$(Configuration)/$(ProjectName)
EngineIntDir	:= $(IntDir)/gpk_engine

LIBS		:= $(patsubst %, $(OutDir)/%, $(Libs))
GCCLIBS		:= $(patsubst %, -l%, $(GccLibs))
INCDIR		:= $(patsubst %,-I%,$(IncDir))

CFLAGS		:= -std=c++23 -Wall -fpermissive -Wno-changes-meaning -O$(OPT) $(C_DEFS) $(INCDIR)
LLCFLAGS	:= -std=c++23 -Wall -Werror -O$(OPT) -I../../zlib $(C_DEFS) -include stdio.h -include stdlib.h
CC			:= g++

OBJ			:= $(IntDir)/galaxy_hell_win32.o
EngineSrc	:= gpk_celestial_body.cpp gpk_engine_orbit.cpp gpk_engine_planetary_system.cpp gpk_engine_renderbuffer.cpp gpk_rigid_body.cpp gpk_sun.cpp gpk_engine.cpp gpk_engine_collision.cpp gpk_engine_explorer.cpp gpk_engine_scene.cpp gpk_engine_shader.cpp gpk_planetary_system.cpp
EngineObj	:= $(patsubst %.cpp,$(EngineIntDir)/%.o,$(EngineSrc))

$(IntDir)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(EngineIntDir)/%.o: ../../gpk/gpk_engine/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(OutDir)/llc.a:
	$(MAKE) -C ../../gpk/llc DEBUG=$(DEBUG) CFLAGS="$(LLCFLAGS)"

$(OutDir)/galaxy_hell.a:
	$(MAKE) -C ../galaxy_hell DEBUG=$(DEBUG)

build: $(IntDir) $(EngineIntDir) $(OBJ) $(EngineObj) $(LIBS)
	mkdir -p $(OutDir)
	$(CC) $(OBJ) $(EngineObj) $(CFLAGS) -o $(OutDir)/$(ProjectName).bin $(LIBS) $(GCCLIBS)

$(IntDir) $(EngineIntDir):
	mkdir -p $@

.PHONY: clean

clean:
	rm -rf $(IntDir)
