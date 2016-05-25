SOURCE_FILES := GPIO_Interface.cpp main.cpp Steering.cpp
OBJECT_FILES := $(patsubst %.cpp,objectfiles/%.o,$(SOURCE_FILES))

GPP = g++

HEADDER_FILES = -I ../Shared/
LIBRARYS = ../connection_library/connection_library.so
DEBUG = -ggdb

OUTPUT_FILE = GPIO

.PHONY: all
all: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJECT_FILES)
	$(GPP) $(OBJECT_FILES) $(LIBRARYS) $(DEBUG) -std=c++0x -lpthread -o $(OUTPUT_FILE)

objectfiles/%.o: %.cpp
	@mkdir -p objectfiles
	$(GPP) $(HEADDER_FILES) $(DEBUG) -std=c++0x -c $< -o $@

# In case the Source file is not int the actual dir, look in Shared folder
objectfiles/%.o: ../Shared/%.cpp
	@mkdir -p objectfiles
	$(GPP) $(HEADDER_FILES) -std=c++0x -c $< -o $@ 

rebuild: clean all

clean:
	rm -f $(OUTPUT_FILE)
	rm -fr objectfiles/
