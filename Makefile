CC = cc
CFLAGS = -Wall -Wextra -Werror -O3 -march=native
LDFLAGS =
SOURCES = main.c hash.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = hotrace

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

fclean:
	rm -f $(OBJECTS) $(EXECUTABLE) gmon.out

re: fclean all

# Added prof target as an alias to profile
prof: profile

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: re

gprof-report:
	@if [ -f gmon.out ]; then \
		gprof -lb $(EXECUTABLE) > profile_report.txt; \
		echo "Profile report generated as profile_report.txt"; \
	else \
		echo "Error: gmon.out not found. Run the program with 'make profile' first."; \
	fi

gprof-visual: gprof-report
	@if [ -f $(HOME)/.local/bin/gprof2dot ]; then \
		$(HOME)/.local/bin/gprof2dot -f gprof profile_report.txt | dot -Tpng -o profile.png; \
		echo "Visual profile generated as profile.png"; \
	else \
		echo "Please install gprof2dot with: pip3 install --user gprof2dot"; \
		exit 1; \
	fi

.PHONY: all clean fclean re profile prof gprof-report gprof-visual