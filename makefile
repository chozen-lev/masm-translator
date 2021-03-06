NAME = masm

COMPILER = g++
# COMPILER = icpc

OBJECTS = main.cpp LexicalAnalyzer.cpp SyntaxAnalyzer.cpp Sentence.cpp Operand.cpp GrammarAnalyzer.cpp GenerateListing.cpp

OPT_FLAGS = -O3

BIN_DIR = Release
CFLAGS = $(OPT_FLAGS)

CFLAGS += -std=c++17

OBJ_LINUX = $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

.PHONY: all clean debug

all: $(BIN_DIR) $(BIN_DIR)/$(NAME)

debug: COMPILER = g++
debug: CFLAGS += -g
debug: $(BIN_DIR) clean $(BIN_DIR)/$(NAME)
	gdb $(BIN_DIR)/$(NAME)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/$(NAME)
	rm -rf $(BIN_DIR)/*.lst
	rm -rf $(BIN_DIR)/*.txt

$(BIN_DIR)/main.o: src/main.cpp src/LexicalAnalyzer.h src/SyntaxAnalyzer.h src/Sentence.h src/Operand.h src/GrammarAnalyzer.h src/GenerateListing.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/LexicalAnalyzer.o: src/LexicalAnalyzer.cpp src/LexicalAnalyzer.h src/Sentence.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/SyntaxAnalyzer.o: src/SyntaxAnalyzer.cpp src/SyntaxAnalyzer.h src/Sentence.h src/Operand.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/Sentence.o: src/Sentence.cpp src/Sentence.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/Operand.o: src/Operand.cpp src/Operand.h src/Sentence.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/GrammarAnalyzer.o: src/GrammarAnalyzer.cpp src/GrammarAnalyzer.h src/Sentence.h src/Operand.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/GenerateListing.o: src/GenerateListing.cpp src/GenerateListing.h
	$(COMPILER) $(CFLAGS) -o $@ -c $<

$(BIN_DIR)/$(NAME): $(OBJ_LINUX)
	$(COMPILER) $(CFLAGS) $(OBJ_LINUX) -o $(BIN_DIR)/$(NAME)

