SOURCE_FILES = main.cpp ../Shared/Server_inet.cpp
HEADDER_FILES = ../Shared/
LIBRARYS = -lopencv_core -lopencv_highgui -lopencv_imgproc

OUTPUT_FILE =CAM_Server

all: build

build:
	g++ $(SOURCE_FILES) -I $(HEADDER_FILES)   $(LIBRARYS) -std=c++0x -lpthread -o $(OUTPUT_FILE)



rebuild: clean build
	
clean:
	rm $(OUTPUT_FILE)
