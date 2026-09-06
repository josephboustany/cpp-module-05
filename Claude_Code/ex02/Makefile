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
SRCS    = src/main.cpp     src/Bureaucrat.cpp     src/AForm.cpp \
          src/ShrubberyCreationForm.cpp     src/RobotomyRequestForm.cpp \
          src/PresidentialPardonForm.cpp
OBJS    = obj/main.o       obj/Bureaucrat.o       obj/AForm.o \
          obj/ShrubberyCreationForm.o       obj/RobotomyRequestForm.o \
          obj/PresidentialPardonForm.o

# ─── DEFAULT TARGET ───────────────────────────────────────────────────────────
all: $(NAME)

# ─── LINK STEP ────────────────────────────────────────────────────────────────
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# ─── COMPILE STEP(S) ──────────────────────────────────────────────────────────
obj/main.o: src/main.cpp include/Bureaucrat.hpp include/AForm.hpp include/ShrubberyCreationForm.hpp include/RobotomyRequestForm.hpp include/PresidentialPardonForm.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/main.o

obj/Bureaucrat.o: src/Bureaucrat.cpp include/Bureaucrat.hpp include/AForm.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/Bureaucrat.cpp -o obj/Bureaucrat.o

obj/AForm.o: src/AForm.cpp include/AForm.hpp include/Bureaucrat.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/AForm.cpp -o obj/AForm.o

obj/ShrubberyCreationForm.o: src/ShrubberyCreationForm.cpp include/ShrubberyCreationForm.hpp include/AForm.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/ShrubberyCreationForm.cpp -o obj/ShrubberyCreationForm.o

obj/RobotomyRequestForm.o: src/RobotomyRequestForm.cpp include/RobotomyRequestForm.hpp include/AForm.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/RobotomyRequestForm.cpp -o obj/RobotomyRequestForm.o

obj/PresidentialPardonForm.o: src/PresidentialPardonForm.cpp include/PresidentialPardonForm.hpp include/AForm.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/PresidentialPardonForm.cpp -o obj/PresidentialPardonForm.o

# ─── CLEAN RULES ──────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# ─── PHONY ────────────────────────────────────────────────────────────────────
.PHONY: all clean fclean re
