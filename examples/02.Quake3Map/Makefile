# Makefile for Irrlicht Examples
# It's usually sufficient to change just the target name and source file list
# and be sure that CXX is set to a valid compiler
Target = 02.Quake3Map
Sources = main.cpp

# general compiler settings
CPPFLAGS = -I../../include -I/usr/X11R6/include
CXXFLAGS = -O3 -ffast-math
#CXXFLAGS = -g -Wall

#default target is Linux
all: all_linux

ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif

# target specific settings
all_linux: LDFLAGS = -L/usr/X11R6/lib$(LIBSELECT) -L../../lib/Linux -lIrrlicht -lGL -lXxf86vm -lXext -lX11
all_linux clean_linux: SYSTEM=Linux
all_win32: LDFLAGS = -L../../lib/Win32-gcc -lIrrlicht
static_win32: LDFLAGS += -lgdi32 -lopengl32 -ld3dx9d -lwinmm -lm
static_win32: CPPFLAGS += -D_IRR_STATIC_LIB_
static_win32 all_win32 clean_win32: SYSTEM=Win32-gcc
static_win32 all_win32 clean_win32: SUF=.exe
# name of the binary - only valid for targets which set SYSTEM
DESTPATH = ../../bin/$(SYSTEM)/$(Target)$(SUF)

all_linux all_win32 static_win32:
	$(warning Building...)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Sources) -o $(DESTPATH) $(LDFLAGS)

clean: clean_linux clean_win32
	$(warning Cleaning...)

clean_linux clean_win32:
	@$(RM) $(DESTPATH)

.PHONY: all all_win32 clean clean_linux clean_win32 static_win32
