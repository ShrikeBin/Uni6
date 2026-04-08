# ─── Compiler configuration ───────────────────────────────────────────────────
CC     = clang++
CFLAGS = -Wall -Wextra -O2
ADAC   = gnatmake
CARGO  = cargo

# ─── Directory layout ─────────────────────────────────────────────────────────
CT_DIR = cpp
AT_DIR = ada
RW_DIR = rust

# ─── Colors ───────────────────────────────────────────────────────────────────
GREEN  = \033[0;32m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
BOLD   = \033[1m
RESET  = \033[0m

.PHONY: all cpp ada rust all_test clean

all: cpp ada rust

# ─── C++ test binary ──────────────────────────────────────────────────────────
cpp:
	$(CC) $(CFLAGS) $(CT_DIR)/*.cpp -I$(CT_DIR) -o $(CT_DIR)/test_cpp
	@echo "$(GREEN)C++ build complete: ./$(CT_DIR)/test_cpp$(RESET)"

# ─── Ada test binary ──────────────────────────────────────────────────────────
ada:
	cd $(AT_DIR) && $(ADAC) test.adb -o test_ada
	@echo "$(GREEN)Ada build complete: ./$(AT_DIR)/test_ada$(RESET)"

# ─── Rust test binary ─────────────────────────────────────────────────────────
rust:
	cd $(RW_DIR) && $(CARGO) build --release
	@echo "$(GREEN)Rust build complete$(RESET)"

# ─── All Tests ────────────────────────────────────────────────────────────────
all_test: cpp ada rust
	@echo "\n$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)           Running all tests          $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	
	@echo "$(YELLOW)▶ Rust:$(RESET)"
	./$(RW_DIR)/target/release/test_rust || true
	
	@echo "\n$(YELLOW)▶ Ada:$(RESET)"
	./$(AT_DIR)/test_ada || true
	
	@echo "\n$(YELLOW)▶ C++:$(RESET)"
	./$(CT_DIR)/test_cpp || true

	@echo "\n$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)          All tests finished           $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"

# ─── Clean ────────────────────────────────────────────────────────────────────
clean:
	# Clean C++
	rm -f $(CT_DIR)/*.o $(CT_DIR)/test_cpp
	# Clean Ada
	rm -f $(AT_DIR)/*.o $(AT_DIR)/*.ali $(AT_DIR)/test_ada
	# Clean Rust
	cd $(RW_DIR) && $(CARGO) clean