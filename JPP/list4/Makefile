# ─── Compiler configuration ───────────────────────────────────────────────────
ADAC   = gnatmake
JAVAC  = javac
JAVA   = java
GO     = go

# ─── Directory layout ─────────────────────────────────────────────────────────
AT_DIR = ada
GO_DIR = go
JV_DIR = java

# ─── Colors ───────────────────────────────────────────────────────────────────
GREEN  = \033[0;32m
YELLOW = \033[0;33m
CYAN   = \033[0;36m
BOLD   = \033[1m
RESET  = \033[0m

.PHONY: all ada go java all_test clean

all: ada go java

# ─── Ada binaries ─────────────────────────────────────────────────────────────
ada:
	mkdir -p $(AT_DIR)/obj
	cd $(AT_DIR) && $(ADAC) philosophers.adb  -o philosophers  2>&1
	cd $(AT_DIR) && $(ADAC) star_server.adb   -o star_server   2>&1
	@echo "$(GREEN)Ada build complete$(RESET)"

# ─── Go binary ────────────────────────────────────────────────────────────────
go:
	cd $(GO_DIR) && $(GO) build -o concurrency_go . 2>&1
	@echo "$(GREEN)Go build complete: ./$(GO_DIR)/concurrency_go$(RESET)"

# ─── Java classes ─────────────────────────────────────────────────────────────
java:
	mkdir -p $(JV_DIR)/out
	$(JAVAC) -d $(JV_DIR)/out $(JV_DIR)/Philosophers.java $(JV_DIR)/StarServer.java $(JV_DIR)/Main.java
	@echo "$(GREEN)Java build complete$(RESET)"

# ─── All Tests ────────────────────────────────────────────────────────────────
all_test: ada go java
	mkdir -p logs
	@echo "\n$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)           Running all tests          $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"

	@echo "\n$(YELLOW)▶ Ada — Philosophers:$(RESET)"
	./$(AT_DIR)/philosophers > logs/ada_philosophers.out 2> logs/ada_philosophers.err || true

	@echo "\n$(YELLOW)▶ Ada — Star Server:$(RESET)"
	./$(AT_DIR)/star_server > logs/ada_star_server.out 2> logs/ada_star_server.err || true

	@echo "\n$(YELLOW)▶ Go (Philosophers + Star Server):$(RESET)"
	./$(GO_DIR)/concurrency_go > logs/go.out 2> logs/go.err || true

	@echo "\n$(YELLOW)▶ Java (Philosophers + Star Server):$(RESET)"
	cd $(JV_DIR) && $(JAVA) -cp out Main > ../logs/java.out 2> ../logs/java.err || true

	@echo "\n$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)          All tests finished           $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(GREEN)Logs saved in ./logs/$(RESET)"

# ─── Clean ────────────────────────────────────────────────────────────────────
clean:
	rm -f $(AT_DIR)/*.o $(AT_DIR)/*.ali $(AT_DIR)/philosophers $(AT_DIR)/star_server
	rm -rf $(AT_DIR)/obj
	rm -f  $(GO_DIR)/concurrency_go
	rm -rf $(JV_DIR)/out
	rm -rf logs