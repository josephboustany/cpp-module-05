# C++ Module 5 — Project Rules & Conventions

---

## Context
- Student at 42 school, working through the C++ module series (Modules 0–4 already completed).
- Working directory: `/home/joseph/Workspace/42/Core_Projects/Cpp_Modules/Cpp_Module_5/Claude_Code`
- Module 5 topic: repetition and exceptions (`throw`/`try`/`catch`, custom exception classes, `std::exception`).
- Peer evaluation is a real constraint — constructor/destructor messages and canonical form are checked by peers.
- Each exercise gets an explanation file (`exNN_explained.md`) inside its directory.

---

## Learned Conventions (confirmed during this module)

- **Protected attribute naming follows the exercise spec exactly.** The underscore prefix rule applies to private members only. If the exercise PDF names a protected attribute `type`, use `type` — not `_type`. Deviating breaks peer evaluation.
- **`virtual` keyword goes in the `.hpp` declaration only — never in the `.cpp` definition.** Repeating it in the definition is a compile error.
- **Polymorphic methods print only their own content, no class-name prefix.** E.g. `makeSound()` outputs `"Woof! Woof!\n"`, not `"Dog: Woof! Woof!\n"`. Prefixing inside the method causes double-labeling when main() also prints `getType()`.
- **Include guards must be ALL_CAPS.** `#ifndef ANIMAL_HPP` — never mixed case like `ANIMAL_hpp`. The `#ifndef` and `#define` tokens must match exactly and be fully uppercase.
- **Constructor/destructor messages must match the class name casing exactly.** E.g., `"Cat destructor called\n"` not `"cat destructor called\n"`. A single wrong-case letter is visible in peer output checks.
- **East const and west const are identical.** `const std::string&` and `std::string const&` mean exactly the same thing. The subject uses east const style (`T const&`) — follow it for consistency but never treat the other as wrong.
- **Suppress unused-parameter warnings with `(void)param;`.** With `-Wextra -Werror`, an unreferenced parameter is a compile error. Write `(void)param;` as the function body when the parameter must exist in the signature but is intentionally unused (e.g. `AMateria::use(ICharacter& target)` default no-op).
- **Passing by `*` signals ownership transfer; passing by `&` signals borrow.** When a function takes a pointer, the caller hands ownership over — the callee (or the object it stores in) is responsible for `delete`. When a function takes a reference, the caller keeps ownership and must `delete` it themselves. This is a convention, not enforced by the compiler — both sides must follow the same understanding to avoid double-free or leak.
- **Custom exception classes are exempt from Orthodox Canonical Form.** The subject says so explicitly. If the class holds no data of its own (nested exception types with only an overridden `what()`), the compiler-generated default constructor/copy/assignment/destructor are sufficient — no need to hand-write them.
- **`catch` by reference, never by value.** `catch (std::exception& e)` preserves virtual dispatch to the derived `what()`; `catch (std::exception e)` copies and *slices* the object down to the base part, silently losing the override.
- **Overriding `std::exception::what()` requires the exact same exception specification.** `virtual const char* what() const throw();` — the trailing `throw()` must be repeated (it's part of the signature C++98 requires to match the base declaration), even though `virtual` itself still follows the project's declaration-only rule.

---

## Compilation
- Compiler: `c++` with flags `-Wall -Wextra -Werror`
- Standard: C++98 (`-std=c++98` must still compile)

---

## Naming Conventions
- Exercise directories: `ex00`, `ex01`, ..., `exn`
- Class names: UpperCamelCase (e.g., `BrickWall`)
- Files named after their class: `ClassName.hpp`, `ClassName.cpp`, `ClassName.tpp`
- Private member attributes prefixed with `_` (e.g., `_hitPoints`)
- Follow naming exactly as specified in the exercise guidelines

---

## Output
- Every output message must end with a newline character (`\n`)
- All output goes to standard output (`std::cout`)
- Constructors and destructors must always print a message (required by 42 for peer evaluation)

---

## Forbidden
- `printf()`, `*alloc()`, `free()` — use C++ alternatives
- `using namespace <ns_name>`
- `friend` keyword
- STL containers: `vector`, `list`, `map`, etc.
- STL algorithms: anything requiring `<algorithm>`
- C++11 or later features
- Boost or any external library
- Function implementations in header files (except function templates)

---

## Headers
- Each header must be self-contained (include all its own dependencies)
- All headers must have include guards (`#ifndef / #define / #endif`) to prevent double inclusion
- Include `<iostream>` in `.cpp` files only — not in headers — to avoid forcing it on every consumer

---

## Orthodox Canonical Form
Every class must implement these four special functions explicitly:

| Function                  | Triggered by                                       |
|---------------------------|----------------------------------------------------|
| Default constructor       | `ClassName a;`                                     |
| Copy constructor          | `ClassName b(a);` or `ClassName b = a;`            |
| Copy assignment operator  | `b = a;` (both already exist)                      |
| Destructor                | end of scope or `delete`                           |

- Use the **member initializer list** (`: member(value), ...`) to initialize members, not body assignment
- Copy assignment operator must have a **self-assignment guard**: `if (this != &other)`
- Copy assignment operator must return `*this` by reference (`ClassName&`)
- Initialization order follows declaration order in the class — keep the list in the same order

### OCF in derived classes
- Derived copy constructor **must** call the base copy constructor explicitly in its initializer list:
  `Derived::Derived(const Derived& other) : Base(other) { ... }`
  Without this, the base default constructor runs and base members are not copied.
- Derived copy assignment **must** call `Base::operator=(other)` in its body after the self-assignment guard.
- Derived destructor body runs first, then the base destructor runs automatically — no need to call it explicitly.

---

## Inheritance & Polymorphism

### `virtual` on methods
- Mark a method `virtual` in the base class to enable **dynamic dispatch** (runtime lookup via vtable).
- Derived classes override by declaring the same method with the same signature — the `override` keyword does not exist in C++98.
- Without `virtual`, calling through a base pointer always runs the base method regardless of the object's actual type (static dispatch).

### Virtual destructor (mandatory rule)
- **Any base class whose objects may be `delete`d through a base class pointer must have a `virtual` destructor.**
- Without it, only the base destructor fires on `delete`, skipping the derived destructor and leaking any derived resources.
- The `virtual` keyword appears only in the declaration (`.hpp`); never repeat it in the definition (`.cpp`).

### Inheritance access
- Use `public` inheritance for "is-a" relationships: `class Dog : public Animal`.
- `protected` members of the base are accessible inside derived class methods but not from outside.

### WrongAnimal pattern
- To demonstrate the necessity of `virtual`, exercises may require a parallel hierarchy (e.g. `WrongAnimal` / `WrongCat`) with no `virtual` keyword.
- The wrong hierarchy shows static dispatch and the missing derived-destructor problem side by side with the correct hierarchy.

---

## Code Style
- No enforced style, but code must be clean and readable
- Prefer C++-style standard library functions over C equivalents
- Pass objects by `const &` when only reading them; pass by value only for small primitives or intentional copies
- Use `unsigned int` for values that cannot be negative (HP, energy, damage); always guard against underflow before subtracting

---

## Directory Structure (per exercise)
```
exNN/
├── Makefile
├── <binary>          ← output binary at root
├── include/          ← all .hpp headers
├── obj/              ← .o object files (auto-created by make, never committed)
└── src/              ← all .cpp source files
```
- Compile with `-I include` so headers are included as `"ClassName.hpp"` everywhere
- `@mkdir -p obj` placed at the start of each compile rule to create `obj/` on first build
- `clean` removes object files; `fclean` also removes the binary

---

## Makefile Rules (mandatory)
- Rules: `$(NAME)`, `clean`, `fclean`, `re`, `all`
- `all` must be the first (default) target — runs on plain `make`
- No relinking when sources are unchanged
- For multi-binary projects: one rule per binary; `all` compiles all
- All source files must be **explicitly named** — no wildcards (`*.cpp`, `*.o`, etc.)

## Makefile Style
- Section comments: `# ─── SECTION NAME ───────` style, in this order:
  1. OUTPUT BINARY
  2. COMPILER & FLAGS
  3. DIRECTORIES
  4. SOURCE & OBJECT FILES
  5. DEFAULT TARGET
  6. LINK STEP
  7. COMPILE STEP(S)
  8. CLEAN RULES
  9. PHONY
- `OBJS` listed explicitly by name, **not** derived from `SRCS` via `$(SRCS:.cpp=.o)`
- One compile rule per `.cpp` file — no pattern rule `%.o: %.cpp`
- Each compile rule lists its `.cpp` **and** its `.hpp` dependencies so Make recompiles on header changes

### Makefile template
```makefile
# ─── OUTPUT BINARY ────────────────────────────────────────────────────────────
NAME    = <binary>

# ─── COMPILER & FLAGS ─────────────────────────────────────────────────────────
CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I include

# ─── DIRECTORIES ──────────────────────────────────────────────────────────────
SRCDIR  = src
OBJDIR  = obj
INCDIR  = include

# ─── SOURCE & OBJECT FILES ────────────────────────────────────────────────────
SRCS    = src/main.cpp     src/ClassName.cpp
OBJS    = obj/main.o       obj/ClassName.o

# ─── DEFAULT TARGET ───────────────────────────────────────────────────────────
all: $(NAME)

# ─── LINK STEP ────────────────────────────────────────────────────────────────
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# ─── COMPILE STEP(S) ──────────────────────────────────────────────────────────
obj/main.o: src/main.cpp include/ClassName.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/main.o

obj/ClassName.o: src/ClassName.cpp include/ClassName.hpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c src/ClassName.cpp -o obj/ClassName.o

# ─── CLEAN RULES ──────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# ─── PHONY ────────────────────────────────────────────────────────────────────
.PHONY: all clean fclean re
```

---

## Scratch Directory
- `Scratch/` lives at the root of the working directory alongside the `exNN/` folders
- Contains one subdirectory per exercise (`Scratch/ex00/`, `Scratch/ex01/`, …) with the **same file structure** as the real exercise
- **Makefile**: fully filled (identical copy of the real exercise Makefile)
- **All other files** (`.hpp`, `.cpp`): created empty — the user fills them in as practice
- When creating a Scratch scaffold, mirror every file from the real exercise directory; never generate implementation content in the source/header files
- Each Scratch exercise also gets a `trace.md` that traces every line of `main()` to its output, annotating which variable each constructor/destructor call belongs to

---

## Explanation Files
- Each exercise gets an `exNN_explained.md` at the root of its directory
- Standard three-part structure:
  1. **What the exercise is asking** — plain-terms description of the objects and how they interact, with analogies and the full main() flow
  2. **How to think through this** — step-by-step design decisions in the order you face them from a blank file (why each choice, not just what it is)
  3. **New concepts** — concepts directly used in the solution, explained with code examples
  4. **File-by-file walkthrough** — every file in dependency-first reading order (interfaces → abstract base → concrete leaves → composites → main)
  5. **Memory ownership summary** — table of who owns each pointer after each operation
- Each exercise also gets an `exNN_files_overview.md` — a single table listing all files and one-line description of what each does, in the same reading order
- Concept clarifications may be appended inline below the relevant section when the user asks
- Do **not** include concepts from future exercises or the broader module — only what is in the current solution

---

## Completed Exercises (Module 5)

- **ex00** — Bureaucrat: `const std::string _name`, `int _grade` (1=highest, 150=lowest), full OCF (no default constructor — `_name` is const so it must be set at construction; `operator=` copies `_grade` only, leaves `_name` untouched). Nested exception classes `Bureaucrat::GradeTooHighException` / `GradeTooLowException`, both public-inheriting `std::exception`, each overriding `virtual const char* what() const throw();` — thrown by the constructor and by `incrementGrade()`/`decrementGrade()` whenever the grade would leave `[1, 150]`. `static const int _gradeMin`/`_gradeMax` (1/150) declared in header, defined in `.cpp`. Free-function `operator<<` prints `"<name>, bureaucrat grade <grade>.\n"`. Binary: `bureaucrat`. Explanation file: `ex00/ex00_explained.md` (new-concepts section covers throw/try/catch mechanics, catch-by-reference vs slicing, `std::exception`/`what()`, the C++98 `throw()` exception specification, nested exception classes, const members forcing no default ctor and an asymmetric `operator=`, `static const int` class constants, `operator<<` as free function, stack unwinding safety during constructor validation). No files-overview or Scratch scaffold yet.
  - §2.2 (catch by reference vs value) was expanded with a side-by-side ASCII vptr
    diagram — reference case shows `e` naming the thrown object directly (vptr
    untouched, `e.what()` reaches the derived override); value case shows the copy's
    own vptr reset to `std::exception`'s vtable after slicing. Keep this diagram style
    (not plain prose) if the section is touched again — that was an explicit user
    correction.
  - §2.7 (`static const int` class members) originally had a full odr-use walkthrough
    (value-use vs odr-use, address-taking, storage) but that framing didn't land for
    the user even after expansion. It was fully replaced with a concrete
    build-then-break example: two tiny files (`add`/`math.cpp` calling into
    `main.cpp`) compiled and linked step by step, then deliberately broken (delete the
    file with the real definition) to show the compile step still succeeds while the
    link step fails with `undefined reference to 'add'` — same pattern repeated for a
    variable (`extern int counter;` vs `int counter = 5;`), then mapped onto
    `static const int _gradeMin;` (the promise, in `.hpp`) vs
    `const int Bureaucrat::_gradeMin = 1;` (the real thing, in `.cpp`). See
    [[concrete-broken-example-teaching-style]] — default to this concrete
    promise/reality build-then-break style for any future linker/compile-mechanics
    explanation in this project, rather than a terminology-first one.
