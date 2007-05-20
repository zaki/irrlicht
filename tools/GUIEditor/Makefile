CPPFLAGS =  -I../../include -I/usr/X11R6/include
LDFLAGS = -L/usr/X11R6/lib -L../../lib/Linux -lIrrlicht -lGL -lGLU -lXxf86vm -lXext -lX11

SRCS = CGUIAttributeEditor.cpp CGUIEditFactory.cpp CGUIEditWindow.cpp CGUIEditWorkspace.cpp CGUIPanel.cpp CGUITextureCacheBrowser.cpp CMemoryReadWriteFile.cpp main.cpp
OBJS = $(SRCS:%.cpp=%.o)
DEST = GUIEditor

all: $(DEST)

GUIEditor: $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(DEST)
