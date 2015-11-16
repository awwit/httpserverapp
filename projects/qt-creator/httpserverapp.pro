TARGET = httpserverapp
TEMPLATE = lib

CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

lessThan(QT_MAJOR_VERSION, 5) {
	CONFIG += object_with_source
	QMAKE_CXXFLAGS += -std=c++1y
} else {
	CONFIG += object_parallel_to_source
}

unix {
	DEFINES += POSIX
}

CONFIG(debug, debug|release):DEFINES += DEBUG

SOURCES += \
    ../../src/FileIncoming.cpp \
    ../../src/Main.cpp \
    ../../src/Test.cpp \
    ../../src/Utils.cpp \
    ../../src/Socket.cpp \
    ../../src/System.cpp

HEADERS += \
    ../../src/FileIncoming.h \
    ../../src/Main.h \
    ../../src/Test.h \
    ../../src/Utils.h \
    ../../src/RawData.h \
    ../../src/ServerRequest.h \
    ../../src/ServerResponse.h \
    ../../src/Socket.h \
    ../../src/System.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
