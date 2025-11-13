
CFLAGS += -I$(HOME)/opt/include
CFLAGS += -std=c99 -Wall -Wextra

LIBS += -L$(HOME)/opt/lib
LIBS += -lraylib
FRAMEWORKS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
LIBS += $(FRAMEWORKS)

watersim: src/main.c
	clang $(CFLAGS) $(LIBS) src/main.c -o watersim


compile_flags.txt: FORCE
	@echo "Generating compile_flags.txt for IDE support"
	@echo $(CFLAGS) | tr ' ' '\n' > $@

FORCE:

