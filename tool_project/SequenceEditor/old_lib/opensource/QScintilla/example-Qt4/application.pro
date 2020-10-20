CONFIG       += release
HEADERS       = mainwindow.h
SOURCES       = main.cpp \
                mainwindow.cpp
RESOURCES     = application.qrc
LIBS         += -L$(GFLIB_DIR)/opensource/QScintilla/Qt4/release -lqscintilla2
INCLUDEPATH	 += $(GFLIB_DIR)/opensource/QScintilla/Qt4
