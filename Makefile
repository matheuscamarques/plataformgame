CXX ?= g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Isrc -MMD -MP
# Corrige link com SFML quando o `ld` do linuxbrew/homebrew sombreia o `ld` do
# sistema: ele não procura em /lib/x86_64-linux-gnu, então as dependências
# transitivas do SFML (libGL, freetype, X11, Xrandr, udev) falham com
# "not found (try using -rpath or -rpath-link)" + "undefined reference".
# O -rpath-link só vale em tempo de link, não altera o binário final,
# e é inofensivo quando já se usa o `ld` do sistema.
LDFLAGS := -Wl,-rpath-link,/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR := src
OBJ_DIR := compiled
BIN_DIR := build
TARGET := $(BIN_DIR)/plataformer
ASSET := arial.ttf

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Objetos do jogo sem o main(): testes linkam contra eles.
GAME_OBJS := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

TEST_DIR := tests
TEST_BUILD := $(BIN_DIR)/tests
TEST_SRCS := $(sort $(wildcard $(TEST_DIR)/test_*.cpp))
TEST_BINS := $(patsubst $(TEST_DIR)/%.cpp,$(TEST_BUILD)/%,$(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
	cp $(ASSET) $(BIN_DIR)/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.o plataformer

# mantido por compatibilidade: limpa e recria as pastas
clear: clean
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

start: all
	./$(TARGET)

watch:
	watch -n 2 $(MAKE) all

test: all $(TEST_BINS)
	@set -e; for t in $(TEST_BINS); do echo "== $$t"; $$t; done

$(TEST_BUILD)/%: $(TEST_DIR)/%.cpp $(GAME_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< $(GAME_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

.PHONY: all clean clear run start watch test

-include $(DEPS)
