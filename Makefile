
DIR_FREETYPE2_INC=-I/usr/include/freetype2
DIR_FREETYPE2_LIB=

LIBS:=`sdl-config --libs` -lGL -lGLU -lpng -lfreetype -logg -lvorbis -lvorbisfile 
CFLAGS:=$(CFLAGS) `sdl-config --cflags` $(DIR_FREETYPE2_INC)

#Windows libs, for now, manually uncomment
#LIBS:= -D_WINDOWS -Ic:/Dev-Cpp/include/SDL -Ic:/Dev-Cpp/include -Ic:/Dev-Cpp/include/GL -Ic:/Dev-Cpp/include/GLU -Ic:/Dev-Cpp/include/freetype2 -Dmain=SDL_main -Lc:/Dev-Cpp/lib -lmingw32 -lSDLmain -lSDL -mwindows -lfreetype -logg -lvorbisfile -lopengl32 -ldevil -lglu32 -lvorbis -lILU -lILUT

DESTDIR?=

SOURCES:=$(wildcard src/*.cc)
HEADERS:=$(wildcard src/*.h)
OBJECTS:=$(subst src/,.build/debug/,$(subst .cc,.o,$(SOURCES)))
OBJECTS_64:=$(subst src/,.build/debug64/,$(subst .cc,.o,$(SOURCES)))
OBJECTS_RELEASE:=$(subst src/,.build/release/,$(subst .cc,.o,$(SOURCES)))

all: release

include $(wildcard .build/deps/*.d)

run: game
	./game

runopt: fretscpp
	./wrapdisp
	
clean: 
	-rm -rf game game-r wrapdisp oneshot oneshot-opt .build

release: wrapdisp

.build/debug/%.o: src/%.cc
	@mkdir -p $(@D)
	@mkdir -p .build/deps
	g++ -MF $(addprefix .build/deps/debug-,$(subst .o,.d,$(subst /,_,$(subst .build/debug/,,$@)))) -MMD -c $< -Isrc -o $@ $(CFLAGS)

.build/release/%.o: src/%.cc
	@mkdir -p $(@D)
	@mkdir -p .build/deps
	g++ -MF $(addprefix .build/deps/release-,$(subst .o,.d,$(subst /,_,$(subst .build/release/,,$@)))) -O3 -DDISABLEMESSAGES -MMD -c $< -Isrc -o $@ $(CFLAGS)

.build/debug64/%.o: src/%.cc
	@mkdir -p $(@D) 
	@mkdir -p .build/deps
	g++ -MF $(addprefix .build/deps/,$(subst .o,.d,$(subst /,_,$(subst .build/debug64/,,$@)))) -m64 -MMD -c $< -Isrc -o $@ $(CFLAGS)


wrapdisp-noopt: $(OBJECTS)
	g++ -o $@ $(OBJECTS) $(LIBS)

oneshot: $(SOURCES) $(HEADERS)
	for i in $(SOURCES); do echo "#include \"$$i\""; done | g++ -g3 -ggdb -O0 -x c++ -o oneshot $(CFLAGS) - $(LIBS)

oneshot-opt: $(SOURCES) $(HEADERS)
	for i in $(SOURCES); do echo "#include \"$$i\""; done | g++ -O3 -DDISABLEMESSAGES -x c++ -o oneshot $(CFLAGS) - $(LIBS)

run-oneshot: oneshot
	./oneshot

wrapdisp: $(OBJECTS_RELEASE)
	g++ -O3 -DDISABLEMESSAGES -o $@ $^ $(LIBS)

cpxprep: cpxprep.cpp
	g++ $< -o $@

ubudeps:
	sudo apt-get install build-essential libsdl-dev libvorbis-dev libfreetype6-dev libgl1-mesa-dev libglu1-mesa-dev libpng-dev


spec:
	find data -type d | while read A; do echo '%__install -m 755 -d $${RPM_BUILD_ROOT}'/usr/share/games/wrapdisp/$$A; done >wrapdisp.spec
	echo '%__install -m 755 wrapdisp/wrapdisp $${RPM_BUILD_ROOT}/usr/bin/wrapdisp' >>wrapdisp.spec
	find data -type f | while read A; do echo '%__install -m 0644 wrapdisp/'$$A '$${RPM_BUILD_ROOT}'/usr/share/games/wrapdisp/$$A; done >>wrapdisp.spec

install: wrapdisp
	install -m 755 -d $(DESTDIR)/usr
	install -m 755 -d $(DESTDIR)/usr/bin

versionincrement:
	sed "s/[.]/ /" VERSION | bash -c "read QQ B; echo $$[QQ+0].$$[B+1]" >VERSION.tmp
	mv VERSION VERSION.prev
	mv VERSION.tmp VERSION
	git add VERSION

preproc.cpp: wrapdisp.cpp
	g++ -E $< $(LIBS)>$@
	
.PHONY: run install all versionincrement ubudeps ubusongs spec
