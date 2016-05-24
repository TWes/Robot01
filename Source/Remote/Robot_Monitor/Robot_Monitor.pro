SOURCES += \
    main.cpp \
    map_widget.cpp \
    main_window.cpp \
    Sensor_Connection.cpp \    
    status_widget.cpp

HEADERS += \
    map_widget.hpp \
    main_window.hpp \
    Sensor_Connection.hpp \
    Sensor_structs.hpp \
    status_widget.hpp

QMAKE_CXXFLAGS += -std=c++0x

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../connection_library/release/ -lconnection_library
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../connection_library/debug/ -lconnection_library
else:unix: LIBS += -L$$PWD/../connection_library/ -lconnection_library

INCLUDEPATH += $$PWD/../connection_library
DEPENDPATH += $$PWD/../connection_library
