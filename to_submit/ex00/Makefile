# ─── OUTPUT BINARY ────────────────────────────────────────────────────────────
NAME    = bureaucrat

# ─── COMPILER & FLAGS ─────────────────────────────────────────────────────────
CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I include

# ─── DIRECTORIES ──────────────────────────────────────────────────────────────
SRCDIR  = src
OBJDIR  = obj
INCDIR  = include

# ─── SOURCE & OBJECT FILES ────────────────────────────────────────────────────
SRCS    = src/main.cpp     src/Bureaucrat.cpp
OBJS    = obj/main.o       obj/Bureaucrat.o

# ─── DEFAULT TARGET ───────────────────────────────────────────────────────────
all: $(NAME)

# ─── LINK STEP ────────────────────────────────────────────────────────────────
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# ─── COMPILE STEP(S) ──────────────────────────────────────────────────────────
obj/main.o: src/main.cpp include/Bureaucrat.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/main.o

obj/Bureaucrat.o: src/Bureaucrat.cpp include/Bureaucrat.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/Bureaucrat.cpp -o obj/Bureaucrat.o

# ─── CLEAN RULES ──────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# ─── PHONY ────────────────────────────────────────────────────────────────────
.PHONY: all clean fclean re
