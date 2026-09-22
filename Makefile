CXX ?= g++
# -O2 com determinismo travado: -g p/ gdb, -ffp-contract=off p/ não
# fundir a*b+c em FMA, -fno-fast-math p/ sin/cos/sqrt estáveis.
# Sem essas 3, noise_view/test_cave/test_ores divergem 1 bit vs -O0.
CXXFLAGS := -std=c++17 -Wall -Wextra -Isrc -MMD -MP -O2 -g -ffp-contract=off -fno-fast-math
# ccache quando existir (rebuild 30-50% mais rápido); sem ele, build puro.
CCACHE := $(shell command -v ccache 2>/dev/null)
ifneq ($(CCACHE),)
CXX := ccache $(CXX)
endif
# Corrige link com SFML quando o `ld` do linuxbrew/homebrew sombreia o `ld` do
# sistema: ele não procura em /lib/x86_64-linux-gnu, então as dependências
# transitivas do SFML (libGL, freetype, X11, Xrandr, udev) falham com
# "not found (try using -rpath or -rpath-link)" + "undefined reference".
# O -rpath-link só vale em tempo de link, não altera o binário final,
# e é inofensivo quando já se usa o `ld` do sistema.
LDFLAGS := -Wl,-rpath-link,/lib/x86_64-linux-gnu:/usr/lib/x86_64-linux-gnu
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -pthread

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
# Stress fora do ciclo diário (lento por design, não por bug).
STRESS_TESTS := $(filter %_stress,$(TEST_BINS))
FAST_TESTS := $(filter-out %_stress,$(TEST_BINS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
	cp $(ASSET) $(BIN_DIR)/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.o plataformer logs

# mantido por compatibilidade: limpa e recria as pastas
clear: clean
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

start: all
	./$(TARGET)

watch:
	watch -n 2 $(MAKE) all

test: all $(FAST_TESTS)
	@set -e; for t in $(FAST_TESTS); do echo "== $$t"; $$t; done

# Suite completa, incluindo stress (lento: ~1min só o slime_stress).
test-all: all $(TEST_BINS)
	@set -e; for t in $(TEST_BINS); do echo "== $$t"; $$t; done

$(TEST_BUILD)/%: $(TEST_DIR)/%.cpp $(GAME_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< $(GAME_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# Camadas: core/ nunca depende de support/ (BodyPartId mora em core).
test-layers:
	@! grep -rn '#include.*support/' src/core/ && echo "OK: core nao depende de support"

# P1 cavernas: visualizador de noise standalone (fora de src/, sem Entity).
NOISE_VIEW := $(BIN_DIR)/tools/noise_view
NOISE_DIR := $(BIN_DIR)/noise

noise-view: $(NOISE_VIEW)

$(NOISE_VIEW): tools/noise_view/noise_view.cpp src/world/Generation.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Isrc $^ -o $@ $(LDFLAGS) $(LDLIBS)

LIGHT_VIEW := $(BIN_DIR)/tools/light_view
light-view: $(LIGHT_VIEW)

$(LIGHT_VIEW): tools/light_view/light_view.cpp src/world/LightPropagator.cpp src/world/RaycastLight.cpp src/support/Spatial/spatialhash.cpp src/entities/Entity.cpp src/component/component.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Isrc $^ -o $@ $(LDFLAGS) $(LDLIBS)

.PHONY: all clean clear run start watch test test-all test-layers noise-view light-view

-include $(DEPS)
