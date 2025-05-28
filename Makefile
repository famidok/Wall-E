CLANG=clang
GCC=gcc

CLANG_FLAGS=-O2 -g -Wall -target bpf

BUILD_DIR=build

.PHONY: all clean

all: \
  $(BUILD_DIR)/blacklist.o \
  $(BUILD_DIR)/blacklist_config_writer \
  $(BUILD_DIR)/blacklist_map \
  $(BUILD_DIR)/ratelimit.o \
  $(BUILD_DIR)/ratelimit_config_writer \
  $(BUILD_DIR)/ratelimit_map

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Blacklist targets
$(BUILD_DIR)/blacklist.o: src/core/blacklist.c | $(BUILD_DIR)
	$(CLANG) $(CLANG_FLAGS) -c $< -o $@

$(BUILD_DIR)/blacklist_config_writer: src/helpers/blacklist_config_writer.c | $(BUILD_DIR)
	$(GCC) -o $@ $<

$(BUILD_DIR)/blacklist_map: src/maps/blacklist_map.c | $(BUILD_DIR)
	$(GCC) -o $@ $< -lbpf -ljansson -lpthread

# Ratelimit targets
$(BUILD_DIR)/ratelimit.o: src/core/ratelimit.c | $(BUILD_DIR)
	$(CLANG) $(CLANG_FLAGS) -c $< -o $@

$(BUILD_DIR)/ratelimit_config_writer: src/helpers/ratelimit_config_writer.c | $(BUILD_DIR)
	$(GCC) -o $@ $<

$(BUILD_DIR)/ratelimit_map: src/maps/ratelimit_map.c | $(BUILD_DIR)
	$(GCC) -o $@ $< -lbpf -ljansson -lpthread

clean:
	rm -rf $(BUILD_DIR)
