SOURCE_FILES := main.cpp i2c_access.cpp Sensor_Server.cpp logfile.cpp Sensor_Server_working_thread.cpp

OBJECT_FILES := $(patsubst %.cpp,objectfiles/%.o,$(SOURCE_FILES))

GPP = g++

HEADDER_FILES = 
LIBRARYS = -lopencv_core -lopencv_highgui -lopencv_imgproc ../connection_library/connection_library.so
DEBUG = -ggdb

OUTPUT_FILE =Sensor_Unit

.PHONY: all
all: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJECT_FILES)
	$(GPP)  $(OBJECT_FILES) $(LIBRARYS) $(DEBUG) -std=c++0x -lpthread -o $(OUTPUT_FILE)

objectfiles/%.o: %.cpp
	@mkdir -p objectfiles
	$(GPP) $(HEADDER_FILES) $(DEBUG) -std=c++0x -c $< -o $@

rebuild: clean all

clean:
	rm -f $(OUTPUT_FILE)
	rm -fr objectfiles/
