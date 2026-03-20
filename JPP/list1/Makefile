CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2 -fPIC
ADAC    = gnatmake
CARGO   = cargo

C_DIR      = c_lib
ADA_DIR    = ada_lib
RUST_DIR   = rust_lib
CW_DIR     = c_wrappers
AW_DIR     = ada_wrappers
RW_DIR     = rust_wrappers
CT_DIR     = c_test
AT_DIR     = ada_test
PT_DIR     = python_test

.PHONY: all c_lib ada_lib rust_lib c_test ada_test rust_test python_test clean

all: c_lib ada_lib rust_lib c_test ada_test rust_test

c_lib: $(C_DIR)/libmathlib_c.a $(C_DIR)/libmathlib_c.so

$(C_DIR)/mathlib.o: $(C_DIR)/mathlib.c $(C_DIR)/mathlib.h
	$(CC) $(CFLAGS) -c $< -o $@

$(C_DIR)/libmathlib_c.a: $(C_DIR)/mathlib.o
	ar rcs $@ $^

$(C_DIR)/libmathlib_c.so: $(C_DIR)/mathlib.o
	$(CC) -shared -o $@ $^

ada_lib: $(ADA_DIR)/libmathlib_ada.a $(ADA_DIR)/libmathlib_ada.so

$(ADA_DIR)/libmathlib_ada.a: $(ADA_DIR)/mathlib.adb $(ADA_DIR)/mathlib.ads
	cd $(ADA_DIR) && gnatmake -c mathlib.adb
	cd $(ADA_DIR) && ar rcs libmathlib_ada.a mathlib.o

$(ADA_DIR)/libmathlib_ada.so: $(ADA_DIR)/mathlib.adb $(ADA_DIR)/mathlib.ads
	cd $(ADA_DIR) && gnatmake -c -fPIC mathlib.adb
	cd $(ADA_DIR) && gcc -shared -o libmathlib_ada.so mathlib.o -lgnat

rust_lib:
	cd $(RUST_DIR) && $(CARGO) build --release

c_test: c_lib ada_lib rust_lib
	$(CC) $(CFLAGS) \
	  $(CT_DIR)/main.c \
	  $(CW_DIR)/ada_wrapper.c \
	  -I$(C_DIR) -I$(CW_DIR) \
	  -L$(C_DIR)  -lmathlib_c \
	  -L$(ADA_DIR) -lmathlib_ada \
	  -L$(RUST_DIR)/target/release -lmathlib \
	  -lgnat -lpthread -ldl \
	  -Wl,-rpath,$(abspath $(C_DIR)) \
	  -Wl,-rpath,$(abspath $(ADA_DIR)) \
	  -Wl,-rpath,$(abspath $(RUST_DIR)/target/release) \
	  -o $(CT_DIR)/test_c
	@echo "Run: ./$(CT_DIR)/test_c"

ada_test: c_lib ada_lib rust_lib
	cd $(AT_DIR) && gnatmake test_main.adb \
	  -I../$(ADA_DIR) -I../$(AW_DIR) \
	  -largs \
	    -L../$(C_DIR)  -lmathlib_c \
	    -L../$(ADA_DIR) -lmathlib_ada \
	    -L../$(RUST_DIR)/target/release -lmathlib \
	    -lgnat -lpthread -ldl \
	    -Wl,-rpath,$(abspath $(C_DIR)) \
	    -Wl,-rpath,$(abspath $(ADA_DIR)) \
	    -Wl,-rpath,$(abspath $(RUST_DIR)/target/release)
	@echo "Run: ./$(AT_DIR)/test_main"

rust_test: c_lib ada_lib rust_lib
	cd $(RW_DIR) && $(CARGO) build --release
	@echo "Run: ./$(RW_DIR)/target/release/test_all"

python_test: c_lib ada_lib rust_lib
	python3 $(PT_DIR)/test_all.py

clean:
	rm -f $(C_DIR)/*.o $(C_DIR)/*.a $(C_DIR)/*.so $(CT_DIR)/test_c
	rm -f $(ADA_DIR)/*.o $(ADA_DIR)/*.a $(ADA_DIR)/*.so \
	      $(ADA_DIR)/*.ali $(AT_DIR)/*.o $(AT_DIR)/*.ali $(AT_DIR)/test_main
	cd $(RUST_DIR) && $(CARGO) clean
	cd $(RW_DIR)   && $(CARGO) clean