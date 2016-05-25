SOURCE_FILES := main.cpp
OBJECT_FILES := $(patsubst %.cpp,objectfiles/%.o,$(SOURCE_FILES))

GPP = g++

HEADDER_FILES =
LIBRARYS = ../connection_library/connection_library.so

OUTPUT_FILE = joystick_node

.PHONY: all
all: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJECT_FILES)
	$(GPP)  $(OBJECT_FILES) $(LIBRARYS) -std=c++0x -lpthread -o $(OUTPUT_FILE)

objectfiles/%.o: %.cpp
	@mkdir -p objectfiles
	$(GPP) $(HEADDER_FILES) -std=c++0x -c $< -o $@

# In case the Source file is not int the actual dir, look in Shared folder
objectfiles/%.o: ../Shared/%.cpp
	@mkdir -p objectfiles
	$(GPP) $(HEADDER_FILES) -std=c++0x -c $< -o $@ 

rebuild: clean all

clean:
	rm -f $(OUTPUT_FILE)
	rm -fr objectfiles/
