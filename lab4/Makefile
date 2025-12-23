CFLAGS = -Wall -Wextra -I./include
LDFLAGS = -ldl
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)

all: $(BUILD_DIR) libs programs

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

libs: $(BUILD_DIR)/libprime_gcd_impl1.so $(BUILD_DIR)/libprime_gcd_impl2.so

$(BUILD_DIR)/libprime_gcd_impl1.so: src/libprime_gcd_impl1.c include/libprime_gcd.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ src/libprime_gcd_impl1.c

$(BUILD_DIR)/libprime_gcd_impl2.so: src/libprime_gcd_impl2.c include/libprime_gcd.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ src/libprime_gcd_impl2.c

programs: $(BUILD_DIR)/static_program $(BUILD_DIR)/dynamic_program

$(BUILD_DIR)/static_program: src/static_program.c $(BUILD_DIR)/libprime_gcd_impl1.so
	$(CC) $(CFLAGS) -o $@ src/static_program.c \
		-L$(BUILD_DIR) -lprime_gcd_impl1 \
		-Wl,-rpath,'$$ORIGIN'

$(BUILD_DIR)/dynamic_program: src/dynamic_program.c
	$(CC) $(CFLAGS) -o $@ src/dynamic_program.c $(LDFLAGS) \
		-Wl,-rpath,'$$ORIGIN'

test: all
	@echo "=== Тест статической программы ==="
	$(BUILD_DIR)/static_program <<< "1 1 20"
	@echo "\n=== Тест динамической программы ==="
	cd $(BUILD_DIR) && ./dynamic_program <<< "1 1 20"

run-static: all
	LD_LIBRARY_PATH=$(BUILD_DIR) $(BUILD_DIR)/static_program

run-dynamic: all
	cd $(BUILD_DIR) && LD_LIBRARY_PATH=. ./dynamic_program

clean:
	rm -rf $(BUILD_DIR)

install: libs
	sudo cp $(BUILD_DIR)/libprime_gcd_impl*.so /usr/local/lib/
	sudo cp include/libprime_gcd.h /usr/local/include/
	sudo ldconfig

.PHONY: all libs programs test clean install run-static run-dynamic