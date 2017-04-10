# Makefile for Irrlicht Examples
# It's usually sufficient to change just the target name and source file list
# and be sure that CXX is set to a valid compiler

# Name of the executable created (.exe will be added automatically if necessary)
Target := 01.HelloWorld
# List of source files, separated by spaces
Sources := main.cpp
# Path to Irrlicht directory, should contain include/ and lib/
IrrlichtHome := ../..
# Path for the executable. Note that Irrlicht.dll should usually also be there for win32 systems
BinPath = ../../bin/$(SYSTEM)

all_emscripten: EMSCRIPTEN=1

# general compiler settings (might need to be set when compiling the lib, too)
CPPFLAGS += -I$(IrrlichtHome)/include -I/usr/X11R6/include
ifndef NDEBUG
  ifdef EMSCRIPTEN
    LDFLAGS += -s DEMANGLE_SUPPORT=1
  endif
  CXXFLAGS += -g -Wall
else
  ifdef EMSCRIPTEN
    LDFLAGS += -O3
  endif
  CXXFLAGS += -O3
endif

#default target is Linux
all: all_linux

# target specific settings
 ll_linux all_emscripten all_win32 static_win32: LDFLAGS += -L$(IrrlichtHome)/lib/$(SYSTEM) -lIrrlicht
all_linux: LDFLAGS += -L/usr/X11R6/lib$(LIBSELECT) -lGL -lEGL -lGLESv1_CM -lGLESv2 -lXxf86vm -lXext -lX11 -lXcursor
all_linux clean_linux: SYSTEM=Linux
all_emscripten clean_emscripten: SYSTEM=emscripten
all_win32 clean_win32 static_win32: SYSTEM=Win32-gcc
all_win32 clean_win32 static_win32: SUF=.exe
all_emscripten clean_emscripten: SUF=.html
all_emscripten: CXXFLAGS += -fno-exceptions -fno-rtti -fstrict-aliasing  -std=gnu++11 -U__STRICT_ANSI__
all_emscripten: LDFLAGS += -lGLESv2 -s FULL_ES2=1 -lSDL --preload-file ../../media@/media -s ALLOW_MEMORY_GROWTH=1
static_win32: CPPFLAGS += -D_IRR_STATIC_LIB_
all_win32: LDFLAGS += -lopengl32 -lEGL -lGLESv1_CM -lGLESv2 -lm
static_win32: LDFLAGS += -lgdi32 -lwinspool -lcomdlg32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lopengl32 -lEGL -lGLESv1_CM -lGLESv2
# name of the binary - only valid for targets which set SYSTEM
DESTPATH = $(BinPath)/$(Target)$(SUF)

all_linux all_win32 all_emscripten static_win32:
	$(warning Building...)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Sources) -o $(DESTPATH) $(LDFLAGS)

clean: clean_linux clean_win32 clean_emscripten
	$(warning Cleaning...)

clean_linux clean_win32 clean_emscripten:
	@$(RM) $(DESTPATH)

.PHONY: all all_win32 all_emscripten static_win32 clean clean_linux clean_win32 clean_emscripten

#multilib handling
ifeq ($(HOSTTYPE), x86_64)
LIBSELECT=64
endif
#solaris real-time features
ifeq ($(HOSTTYPE), sun4)
LDFLAGS += -lrt
endif
