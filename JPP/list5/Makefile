# ─── Compiler configuration ───────────────────────────────────────────────────
GHC     = ghc
OCAMLC  = ocamlfind ocamlopt
SMLNJ   = sml
SWIPL   = swipl

# ─── Directory layout ─────────────────────────────────────────────────────────
HS_DIR  = haskell
ML_DIR  = ocaml
SML_DIR = sml
PL_DIR  = prolog

# ─── Colors ───────────────────────────────────────────────────────────────────
GREEN   = \033[0;32m
YELLOW  = \033[0;33m
CYAN    = \033[0;36m
BOLD    = \033[1m
RESET   = \033[0m

.PHONY: all haskell ocaml sml prolog all_test clean

all: haskell ocaml sml

# ─── Haskell ──────────────────────────────────────────────────────────────────
haskell:
	cd $(HS_DIR) && $(GHC) -O2 -o test_hs task.hs test.hs 2>&1
	@echo "$(GREEN)Haskell build complete$(RESET)"

# ─── OCaml ────────────────────────────────────────────────────────────────────
ocaml:
	cd $(ML_DIR) && $(OCAMLC) -package str -linkpkg task.ml test.ml -o test_ml 2>&1
	@echo "$(GREEN)OCaml build complete$(RESET)"

# ─── SML (Standard ML — compile via script) ───────────────────────────────────
sml:
	@echo "$(GREEN)SML loaded via interpreter at test time$(RESET)"

# ─── All Tests ────────────────────────────────────────────────────────────────
all_test: haskell ocaml
	mkdir -p logs
	@echo ""
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)           Running all tests          $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"

	@echo "\n$(YELLOW)▶ Haskell:$(RESET)"
	./$(HS_DIR)/test_hs 2>&1 | tee logs/haskell.out

	@echo "\n$(YELLOW)▶ OCaml:$(RESET)"
	./$(ML_DIR)/test_ml 2>&1 | tee logs/ocaml.out

	@echo "\n$(YELLOW)▶ SML (Standard ML):$(RESET)"
	cd $(SML_DIR) && echo 'use "task.sml"; use "test.sml";' | $(SMLNJ) 2>&1 | tee ../logs/sml.out

	@echo "\n$(YELLOW)▶ Prolog:$(RESET)"
	cd $(PL_DIR) && $(SWIPL) -q -t halt test.pl 2>&1 | tee ../logs/prolog.out

	@echo ""
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(BOLD)$(CYAN)          All tests finished           $(RESET)"
	@echo "$(BOLD)$(CYAN)══════════════════════════════════════$(RESET)"
	@echo "$(GREEN)Logs saved in ./logs/$(RESET)"

# ─── Clean ────────────────────────────────────────────────────────────────────
clean:
	rm -f  $(HS_DIR)/test_hs $(HS_DIR)/*.hi $(HS_DIR)/*.o
	rm -f  $(ML_DIR)/test_ml $(ML_DIR)/*.cmi $(ML_DIR)/*.cmx $(ML_DIR)/*.o
	rm -rf logs