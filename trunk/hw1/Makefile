# Makefile for Asst-3, 15-497
# Kiran Bhat, kiranb@cs.cmu.edu

GRAPHICS_BASE = /afs/cs/academic/class/15462

# include and library paths
INCLUDE =       -I$(GRAPHICS_BASE)/include 

#MESALIBS = 	-L/usr/lib -lGLU -lGL
MESALIBS = 	-L/afs/cs/user/kiranb/www/animation/GL -lGLU -lGL
FLTKLIB = 	-L$(GRAPHICS_BASE)/src/fltk -lfltk 
LIBPATH =       -L$(GRAPHICS_BASE)/lib 
X11LIB = 	-L/usr/X11R6/lib -lX11
LINUXLIBS =	$(MESALIBS) $(X11LIB) $(FLTKLIB) $(LIBPATH) -lXext -lpicio -ltiff -ljpeg -lm 
SUNLIBS = 	$(MESALIBS) $(X11LIB) $(FLTKLIB) $(LIBPATH) -lXext -lpicio -ltiff -ljpeg -lm 

# compiler and settings to use
LINUXCC =	g++ 
LINUXCFLAGS =	-O2 -g $(INCLUDE)
SUNCC =		g++ 
SUNCFLAGS =	-O2 $(INCLUDE)

# ---------- BEGIN SECTION TO CHANGE AS NEEDED ----------

# name of the program
PROGNAME =	clothSim

# compiler to use
CC = 		$(LINUXCC)

# compiler flags to use
CFLAGS =	$(LINUXCFLAGS)

# list of libraries to link
LIBS =		$(LINUXLIBS)

# list of source files to compile
SOURCE =	player.cxx Cloth1.cxx interface.cxx MathDefs.cxx PhysEnv1.cxx transform.cxx \
		skeleton.cxx display.cxx motion.cxx vector.cxx debugfile.cxx

# list of object files to link
OBJECT =	player.o Cloth1.o interface.o MathDefs.o PhysEnv1.o transform.o \
		skeleton.o display.o motion.o vector.o debugfile.o

# ---------- END SECTION TO CHANGE AS NEEDED ----------

.SUFFIXES: .cxx

.cxx.o: 
	$(CC) -c $(CFLAGS) $<

all: i386_rh71

i386_rh71: $(PROGNAME) $(PROGNAME).i386_rh71

sun4x_57: $(PROGNAME) $(PROGNAME).sun4x_57

$(PROGNAME): $(OBJECT)
	rm -f $(PROGNAME) ;
	ln -s $(PROGNAME).@sys $(PROGNAME)

$(PROGNAME).sun4x_57: $(OBJECT)
	$(CC) $(CFLAGS) -o $(PROGNAME).sun4x_57 $(OBJECT) -lsocket $(LIBS)

$(PROGNAME).i386_rh71: $(OBJECT)
	$(CC) $(CFLAGS) -o $(PROGNAME).i386_rh71 $(OBJECT) $(LIBS)

clean:
	-rm -rf core *.o *~ "#"*"#" Makefile.i386_rh71.bak

depend:
	makedepend -fMakefile.i386_rh71 $(IPATH) $(SOURCE)


# DO NOT DELETE
cloth.o: motion.h skeleton.h transform.h PhysEnv.h Skeleton_cloth.h
player.o: interface.h transform.h display.h
interface.o: interface.h player.h
display.o: display.h skeleton.h motion.h
transform.o: transform.h
motion.o: skeleton.h motion.h
skeleton.o: skeleton.h


