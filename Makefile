CC = cc
CFLAGS = -Wall -Wextra -Werror -O3 -march=native
LDFLAGS =
SOURCES = main.c hash.c hash2.c table.c utils.c
OBJ_DIR = obj
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.c=.o))
EXECUTABLE = hotrace

all: $(EXECUTABLE)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(EXECUTABLE): $(OBJ_DIR) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# Updated rule for object files in the obj directory
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(EXECUTABLE) gmon.out

re: fclean all



test-hash-functions: $(EXECUTABLE)
	@echo "Testing with MurmurHash3 (64-bit)..."
	time ./$(EXECUTABLE) -hash 0 < test.txt > murmur64_results.txt
	@echo "Testing with FNV-1a..."
	time ./$(EXECUTABLE) -hash 1 < test.txt > fnv1a_results.txt
	@echo "Testing with djb2..."
	time ./$(EXECUTABLE) -hash 2 < test.txt > djb2_results.txt
	@echo "Testing with MurmurHash3 (128-bit)..."
	time ./$(EXECUTABLE) -hash 3 < test.txt > murmur128_results.txt
	@echo "All tests completed!"
gen:
	@rm -f gen test.txt
	@echo "Generating files..."
	cc gen.c -o gen
	./gen > test.txt
	@echo "Test file generated as test.txt"

# Added prof target as an alias to profile
prof: profile

exec: gen
	@echo "Executing the program..."
	./$(EXECUTABLE) < test.txt

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: re

gprof-report: 
	@if [ -f gmon.out ]; then \
		gprof -lp -v $(EXECUTABLE) > profile_report.txt; \
		echo "Detailed profile report generated as profile_report.txt"; \
	else \
		echo "Error: gmon.out not found. Run the program with 'make profile' first."; \
	fi

gprof-visual: gprof-report 
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof profile_report.txt | dot -Tpng -o profile.png; \
		echo "Visual profile generated as profile.png"; \
	else \
		echo "Please install gprof2dot with: pip3 install --user gprof2dot"; \
		exit 1; \
	fi

# Complete profiling workflow in a single command
profile-all: profile
	@echo "Generating test data..."
	@rm -f gen test.txt
	cc gen.c -o gen
	./gen > test.txt
	@echo "Running program with profiling enabled..."
	./$(EXECUTABLE) < test.txt
	@echo "Generating profiling report..."
	gprof -lb $(EXECUTABLE) > profile_report.txt
	@echo "Profile report generated as profile_report.txt"
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof profile_report.txt | dot -Tpng -o profile.png && \
		echo "Visual profile generated as profile.png"; \
	fi
	@echo "Profiling complete!"

# Add sanitizer targets
asan: CFLAGS += -fsanitize=address -g
asan: LDFLAGS += -fsanitize=address
asan: re

ubsan: CFLAGS += -fsanitize=undefined -g
ubsan: LDFLAGS += -fsanitize=undefined
ubsan: re

# Combined sanitizers
san: CFLAGS += -fsanitize=address,undefined -g
san: LDFLAGS += -fsanitize=address,undefined
san: re

# Run with sanitizers enabled
test-san: san
	@echo "Running with sanitizers..."
	./$(EXECUTABLE) < test.txt > /dev/null 2>san_errors.txt

# Memory leak sanitizer (can be run standalone from ASan)
lsan: CFLAGS += -fsanitize=leak -g
lsan: LDFLAGS += -fsanitize=leak
lsan: re

# Memory sanitizer (detects uninitialized reads)
# Note: Requires all code including libraries to be built with MSan
msan: CFLAGS += -fsanitize=memory -g -fPIE -fno-omit-frame-pointer
msan: LDFLAGS += -fsanitize=memory -fPIE
msan: re

# Thread sanitizer (for multi-threaded code)
# Note: Your code seems single-threaded so this is for completeness
tsan: CFLAGS += -fsanitize=thread -g
tsan: LDFLAGS += -fsanitize=thread
tsan: re

# Profile-guided optimization - step 1: generate profile
pgo-gen: CFLAGS += -fprofile-generate -g
pgo-gen: LDFLAGS += -fprofile-generate
pgo-gen: re
	./$(EXECUTABLE) < test.txt > /dev/null

# Profile-guided optimization - step 2: use profile data
pgo-use: CFLAGS += -fprofile-use -g
pgo-use: LDFLAGS += -fprofile-use
pgo-use: re

# Thorough analysis with multiple tools in sequence
analyze: clean
	@echo "Running Address Sanitizer..."
	@make asan
	@./$(EXECUTABLE) < test.txt > /dev/null 2>asan_errors.txt || true
	@echo "Running Undefined Behavior Sanitizer..."
	@make ubsan
	@./$(EXECUTABLE) < test.txt > /dev/null 2>ubsan_errors.txt || true
	@echo "Running Leak Sanitizer..."
	@make lsan
	@./$(EXECUTABLE) < test.txt > /dev/null 2>lsan_errors.txt || true
	@echo "Running Memory Sanitizer..."
	@make msan
	@./$(EXECUTABLE) < test.txt > /dev/null 2>msan_errors.txt || true
	@echo "Analysis complete. Check *_errors.txt files for details."


# Profile each hash function
profile-hash-functions: profile
	@echo "Generating test data..."
	@rm -f gen test.txt
	cc gen.c -o gen
	./gen > test.txt
		
	@echo "==== Profiling MurmurHash3 (64-bit) ===="
	@rm -f gmon.out
	./$(EXECUTABLE) -hash 0 < test.txt > /dev/null
	gprof -lp $(EXECUTABLE) > murmur64_profile.txt
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof murmur64_profile.txt | dot -Tpng -o murmur64_profile.png && \
		echo "Visual profile generated as murmur64_profile.png"; \
	fi
		
	@echo "==== Profiling FNV-1a ===="
	@rm -f gmon.out
	./$(EXECUTABLE) -hash 1 < test.txt > /dev/null
	gprof -lp $(EXECUTABLE) > fnv1a_profile.txt
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof fnv1a_profile.txt | dot -Tpng -o fnv1a_profile.png && \
		echo "Visual profile generated as fnv1a_profile.png"; \
	fi
		
	@echo "==== Profiling djb2 ===="
	@rm -f gmon.out
	./$(EXECUTABLE) -hash 2 < test.txt > /dev/null
	gprof -lp $(EXECUTABLE) > djb2_profile.txt
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof djb2_profile.txt | dot -Tpng -o djb2_profile.png && \
		echo "Visual profile generated as djb2_profile.png"; \
	fi
		
	@echo "==== Profiling MurmurHash3 (128-bit) ===="
	@rm -f gmon.out
	./$(EXECUTABLE) -hash 3 < test.txt > /dev/null
	gprof -lp $(EXECUTABLE) > murmur128_profile.txt
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f prof murmur128_profile.txt | dot -Tpng -o murmur128_profile.png && \
		echo "Visual profile generated as murmur128_profile.png"; \
	fi
		
	@echo "All hash functions profiled! Check *_profile.txt and *_profile.png files"

.PHONY: all clean fclean re profile prof gprof-report gprof-visual profile-all exec gen analyze