BIN_DIR = bin
LIBRARY_DIR = library
COMPILER_DIR = compiler
RUNTIME_DIR = runtime

CXX = clang++
CXXFLAGS = `llvm-config --cxxflags` -std=c++17 -fexceptions

LIB_OBJ_DIR = $(BIN_DIR)/$(LIBRARY_DIR)/obj
COMP_OBJ_DIR = $(BIN_DIR)/$(COMPILER_DIR)/obj
RUN_OBJ_DIR = $(BIN_DIR)/$(RUNTIME_DIR)/obj

LIB_SRCS = $(wildcard $(LIBRARY_DIR)/*.cpp)
COMP_SRCS = $(wildcard $(COMPILER_DIR)/*.cpp)
RUN_SRCS = $(wildcard $(RUNTIME_DIR)/*.cpp)

LIB_OBJS = $(patsubst $(LIBRARY_DIR)/%.cpp,$(LIB_OBJ_DIR)/%.o,$(LIB_SRCS))
COMP_OBJS = $(patsubst $(COMPILER_DIR)/%.cpp,$(COMP_OBJ_DIR)/%.o,$(COMP_SRCS))
RUN_OBJS = $(patsubst $(RUNTIME_DIR)/%.cpp,$(RUN_OBJ_DIR)/%.o,$(RUN_SRCS))

LLVM_FLAGS = `llvm-config --cxxflags --ldflags --libs`

all: clean make library compiler runtime

library: $(LIB_OBJS)
	ar rcs $(LIBRARY_DIR)/libulang.a $(LIB_OBJS)

compiler: $(COMP_OBJS)
	$(CXX) $(CXXFLAGS) -g -o $(COMPILER_DIR)/ulang $(COMP_OBJS) -L$(LIBRARY_DIR) -lulang $(LLVM_FLAGS)

runtime: $(RUN_OBJS)
	ar rcs $(RUNTIME_DIR)/libulangrt.a $(RUN_OBJS)

$(LIB_OBJ_DIR)/%.o: $(LIBRARY_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(COMP_OBJ_DIR)/%.o: $(COMPILER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -I$(LIBRARY_DIR)

$(RUN_OBJ_DIR)/%.o: $(RUNTIME_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(LIB_OBJ_DIR) $(COMP_OBJ_DIR) $(RUN_OBJ_DIR) $(LIBRARY_DIR)/libulang.a $(COMPILER_DIR)/compiler $(RUNTIME_DIR)/libulangrt.a

make:
	mkdir -p $(LIB_OBJ_DIR) $(COMP_OBJ_DIR) $(RUN_OBJ_DIR)
