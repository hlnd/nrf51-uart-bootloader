CPPUTEST_HOME = /home/omh/Prosjekt/tdd-nrf51/CppUTest-3.3/

SDK_PATH = ../nrf51/nrf51822/

INCLUDE_PATHS += inc/
INCLUDE_PATHS += src/
INCLUDE_PATHS += $(SDK_PATH)Include/
INCLUDE_PATHS += $(SDK_PATH)Include/gcc/
INCLUDE_PATHS += $(SDK_PATH)Include/app_common/
INCLUDE_PATHS += $(SDK_PATH)Include/ble/softdevice/
INCLUDE_PATHS += $(CPPUTEST_HOME)include/

CPPFLAGS = -g $(addprefix -I, $(INCLUDE_PATHS)) -DNRF51
CXXFLAGS = -include $(CPPUTEST_HOME)include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS = -include $(CPPUTEST_HOME)include/CppUTest/MemoryLeakDetectorMallocMacros.h

TARGET = all_tests

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

SOURCES_TESTS = $(wildcard tests/*.cpp)
OBJECTS_TESTS = $(SOURCES_TESTS:.cpp=.o)

LDFLAGS = -L$(CPPUTEST_HOME)lib/ -lCppUTest -lstdc++

all: check

rebuild: clean all

check: $(TARGET)
	./$(TARGET)

clean: 
	rm src/*.o
	rm tests/*.o
	rm $(TARGET)

$(TARGET): $(OBJECTS) $(OBJECTS_TESTS)
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: check clean rebuild all
