NAME = masm

# COMPILER = g++
COMPILER = icpc

OBJECTS = main.cpp LexicalAnalyzer.cpp SyntaxAnalyzer.cpp Sentence.cpp Operand.cpp

OPT_FLAGS = -O3

BIN_DIR = Release
CFLAGS = $(OPT_FLAGS)

CFLAGS += -std=c++17

OBJ_LINUX = $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

.PHONY: all clean

all: $(BIN_DIR) $(BIN_DIR)/$(NAME)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/$(NAME)
	rm -rf $(BIN_DIR)/*.lst
	rm -rf $(BIN_DIR)/*.txt

$(BIN_DIR)/main.o: src/main.cpp src/LexicalAnalyzer.h src/SyntaxAnalyzer.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/LexicalAnalyzer.o: src/LexicalAnalyzer.cpp src/LexicalAnalyzer.h src/types.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/SyntaxAnalyzer.o: src/SyntaxAnalyzer.cpp src/SyntaxAnalyzer.h src/Sentence.h src/types.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/Sentence.o: src/Sentence.cpp src/Sentence.h src/Operand.h src/types.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/Operand.o: src/Operand.cpp src/Operand.h src/types.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/$(NAME): $(OBJ_LINUX)
	$(COMPILER) $(CFLAGS) $(OBJ_LINUX) -o $(BIN_DIR)/$(NAME)