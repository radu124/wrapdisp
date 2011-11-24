PROGRAMNAME=wrapdisp


DIR_FREETYPE2_INC=-I/usr/include/freetype2
DIR_FREETYPE2_LIB=

LIBS:=`sdl-config --cflags --libs` -lGL -lGLU -lpng -lfreetype -logg -lvorbis -lvorbisfile $(DIR_FREETYPE2_INC)
#Windows libs, for now, manually uncomment
#LIBS:= -D_WINDOWS -Ic:/Dev-Cpp/include/SDL -Ic:/Dev-Cpp/include -Ic:/Dev-Cpp/include/GL -Ic:/Dev-Cpp/include/GLU -Ic:/Dev-Cpp/include/freetype2 -Dmain=SDL_main -Lc:/Dev-Cpp/lib -lmingw32 -lSDLmain -lSDL -mwindows -lfreetype -logg -lvorbisfile -lopengl32 -ldevil -lglu32 -lvorbis -lILU -lILUT

DESTDIR?=

all: $(PROGRAMNAME)

run: $(PROGRAMNAME)
	./$(PROGRAMNAME) data/test

include $(wildcard .build/*.d)

runopt: fretscpp
	./splexhd

clean:
	-rm -rf .build
	-rm $(PROGRAMNAME) $(PROGRAMNAME)-release *.tags

release: $(PROGRAMNAME)

.build/$(PROGRAMNAME).cpp: $(PROGRAMNAME).cpx $(wildcard *.cpx) .build/cpxprep
	mkdir -p .build
	.build/cpxprep $< >$@
	#echo -n "$@: ">$(PROGRAMNAME).cpp.d
	#grep -E "^#line" $@ | sed -e 's/.* "//' -e 's/"//' | sort -u | while read A; do echo -n "$$A "; done >>$(PROGRAMNAME).cpp.d
	#geany -g game.cpp.tags `readlink -f $@`

$(PROGRAMNAME): .build/$(PROGRAMNAME).cpp
	g++ -MMD -MF .build/$@.d $< -O0 -g3 -ggdb -o $@ $(LIBS)

$(PROGRAMNAME)-release: .build/$(PROGRAMNAME).cpp
	g++ -DDISABLEMESSAGES $< -O3 -g3 -ggdb -o $@ $(LIBS)

.build/cpxprep: cpxprep.cpp
	mkdir -p .build
	g++ $< -o $@

ubudeps:
	sudo apt-get install build-essential libsdl-dev libvorbis-dev libfreetype6-dev libgl1-mesa-dev libglu1-mesa-dev libpng-dev

ubusongs:
	sudo apt-get install fretsonfire-songs-sectoid

game64: $(PROGRAMNAME).cpp
	g++ -m64 -MMD -MF $@.d $< -O0 -g3 -ggdb -o $@ $(LIBS)

spec:
	find data -type d | while read A; do echo '%__install -m 755 -d $${RPM_BUILD_ROOT}'/usr/share/games/$(PROGRAMNAME)/$$A; done >$(PROGRAMNAME).spec
	echo '%__install -m 755 $(PROGRAMNAME)/$(PROGRAMNAME) $${RPM_BUILD_ROOT}/usr/bin/$(PROGRAMNAME)' >>$(PROGRAMNAME).spec
	find data -type f | while read A; do echo '%__install -m 0644 $(PROGRAMNAME)/'$$A '$${RPM_BUILD_ROOT}'/usr/share/games/$(PROGRAMNAME)/$$A; done >>$(PROGRAMNAME).spec

install: $(PROGRAMNAME)
	install -m 755 -d $(DESTDIR)/usr
	install -m 755 -d $(DESTDIR)/usr/bin
	install -m 755 -d $(DESTDIR)/usr/share
	install -m 755 -d $(DESTDIR)/usr/share/games
	install -m 755 -d $(DESTDIR)/usr/share/games/$(PROGRAMNAME)
	find data -type d | while read A; do install -m 755 -d "$(DESTDIR)/usr/share/games/$(PROGRAMNAME)/$$A"; done
	install -m 755 $(PROGRAMNAME) $(DESTDIR)/usr/bin/$(PROGRAMNAME)
	find data -type f | while read A; do install -m 644 "$$A" "$(DESTDIR)/usr/share/games/$(PROGRAMNAME)/$$A"; done

versionincrement:
	sed "s/[.]/ /" VERSION | bash -c "read QQ B; echo $$[QQ+0].$$[B+1]" >VERSION.tmp
	mv VERSION VERSION.prev
	mv VERSION.tmp VERSION
	git add VERSION

preproc.cpp: $(PROGRAMNAME).cpp
	g++ -E $< $(LIBS)>$@

.PHONY: run install all versionincrement ubudeps ubusongs spec
