CC = gcc
CFLAGS = -std=c89 -pedantic -I.

SRC_DIR=src
BUILD_DIR=build

# Find all .c and .cpp files in the SRC_DIR
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
CPP_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)

INCLUDES = -I$(SRC_DIR)

# Define the object files that will be generated in the BUILD_DIR
C_OBJECTS := $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
CPP_OBJECTS := $(CPP_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

EXECUTABLES = main

# The all target
all: $(EXECUTABLES)

main: $(C_OBJECTS) $(CPP_OBJECTS)
	$(CC) $(CFLAGS) -o main $(C_OBJECTS) $(CPP_OBJECTS) $(LIB)

# Compile .c to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile .cpp to .o
#$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
#	@mkdir -p $(@D)
#	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLES)
