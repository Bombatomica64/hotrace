NAME=hotrace
GENNAME=gen
SRC=main.c hash.c utils.c
GEN=src/gen.c
SRCDIR=src
SRCS=$(addprefix $(SRCDIR)/, $(SRC))
CC=cc
CFLAGS=-Wall -Werror -Wextra -O3
LDFLAGS =
OBJDIR=obj
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
GENOBJ = $(GEN:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Compiling $(NAME)"
	@$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

gen: $(GENNAME)

$(GENNAME): $(GENOBJ)
	@echo "Compiling $(GENNAME)"
	@$(CC) $(GENOBJ) -o $(GENNAME)

$(OBJDIR)/%.o: $(GEN)
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up object files"
	@rm -f $(OBJ)
	@rm -rf $(OBJDIR)

fclean: clean
	@echo "Cleaning up $(NAME)"
	@rm -f $(NAME)

re: fclean all

# Added prof target as an alias to profile
prof: profile

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: re

gprof-report:
	@if [ -f gmon.out ]; then \
		gprof -lb $(NAME) > profile_report.txt; \
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

.PHONY: all clean fclean re
.SILENT:
