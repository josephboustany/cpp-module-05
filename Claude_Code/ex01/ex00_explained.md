# ex00 — Mommy, when I grow up, I want to be a bureaucrat!

## 1. What the exercise is asking

Build a `Bureaucrat` class with:
- a `const std::string` name (set once, never changes),
- an `int` grade from **1** (highest rank) to **150** (lowest rank),
- getters for both,
- `incrementGrade()` / `decrementGrade()` to move the grade toward 1 / toward 150,
- two exception types — `GradeTooHighException`, `GradeTooLowException` — thrown by the
  constructor *and* by both grade-changing functions whenever the grade would leave
  `[1, 150]`,
- an `operator<<` overload that prints `"<name>, bureaucrat grade <grade>.\n"`.

Everything here is new territory compared to Module 4: Module 4 was about polymorphism
(virtual functions, abstract classes, interfaces). Module 5 opens with **exceptions** —
a completely different error-handling mechanism from anything used so far (no more
"return -1 and hope the caller checks").

---

## 2. New concepts (in detail, with examples)

### 2.1 `throw` / `try` / `catch` — how C++ exceptions actually work

Before this exercise, invalid input would typically be handled with a return code:

```cpp
int setGrade(int grade)
{
    if (grade < 1 || grade > 150)
        return -1; // caller must remember to check this
    _grade = grade;
    return 0;
}
```

The problem: nothing forces the caller to check the return value. Ignore it, and the
program silently continues in a broken state.

Exceptions flip this around. `throw` immediately **stops normal execution** and starts
unwinding the call stack, looking for a `catch` block that can handle the thrown                    
object's type. If none exists anywhere up the call chain, the program calls
`std::terminate()` and aborts. There is no way to "accidentally ignore" an exception
the way you can ignore a return code.

```cpp
try
{
    Bureaucrat b("Alice", 0); // constructor throws here
    std::cout << "never reached\n";
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
std::cout << "execution continues here\n";
```

Key mechanics:
- `throw someObject;` copies `someObject` and starts searching for a handler.
- A `try` block is paired with one or more `catch` blocks directly below it.
- `catch (Type& e)` matches if the thrown object's type is `Type` or **derives from**
  `Type`. This is why `catch (std::exception& e)` can catch *any* exception type that
  derives from `std::exception` — including both of ours.
- Once a matching `catch` runs, execution resumes **after** the whole `try/catch`
  block — it does not jump back to where the exception was thrown.

### 2.2 Catch by reference, not by value

```cpp
catch (std::exception& e)   // correct
catch (std::exception  e)   // works, but wrong
```

Catching by value **copies** the exception object, and if the actual thrown type is
`Bureaucrat::GradeTooHighException` but you catch `std::exception` by value, the copy
**slices** the object down to just the `std::exception` part — you lose the derived
`what()` override that would normally run. Catching by reference (`std::exception&`)
avoids the copy entirely and preserves virtual dispatch: `e.what()` still calls the
*derived* class's `what()`, exactly like calling any other virtual function through a
base reference.

**Summary:**

```
catch (std::exception& e)          catch (std::exception e)
   no copy — e IS the object          copy — sliced down to base

  thrown object:                     thrown object:            the copy (e):
  +----------------------+           +----------------------+   +----------------------+
  | vptr -> GradeTooHigh  |           | vptr -> GradeTooHigh  |   | vptr -> std::excep-  |
  |         vtable        |           |         vtable        |-->|         tion vtable  |
  | std::exception part   |           | std::exception part   |   | std::exception part  |
  +----------------------+           +----------------------+   +----------------------+
        ^                                                          your part: gone
        |
  e just names this ---┘

  e.what() -> your what()            e.what() -> std::exception::what()
```

Reference = no copy, vptr untouched, dispatch reaches your `what()`. Value = copy into
a base-typed object, derived part sliced off, copy's own vptr points at
`std::exception`'s vtable — generic message, silently. Always catch by reference.

### 2.3 `std::exception` and `what()`

`std::exception` (from `<exception>`) is the standard root of the exception hierarchy.
Its only interesting member is:

```cpp
virtual const char* what() const throw();
```

Any exception type meant to carry a human-readable message overrides this. The
subject requires our exceptions to be "catchable using try and catch blocks" with
`catch (std::exception& e)`, which is exactly why both custom exceptions **publicly
inherit from `std::exception`** instead of being some unrelated class.

### 2.4 `throw()` after a function — the (now-obsolete) C++98 exception specification

```cpp
virtual const char* what() const throw();
```
1. What throw() actually means (as a spec, not as the throw keyword)
You already know throw x; — the statement that raises an exception. 
throw() written after a function signature is a totally different thing: a dynamic exception specification, 
part of the function's type, that says "I promise this function will never let an exception escape."

void foo();          // may throw anything (or nothing) — no promise
void bar() throw();  // promises: throws nothing, ever
void baz() throw(int); // promises: throws only int (rare, mostly unused in practice)

2. Why it's mandatory specifically on what()

Look at the real declaration in <exception> from the standard library:

class exception {
public:
    virtual ~exception() throw();
    virtual const char* what() const throw();
};
what() is virtual, and the standard library itself promises throw() on it — "calling what() will never throw."

Now, the override rule in C++ (this is the actual rule, not specific to this project): when you override a virtual function, your override's exception specification must be at least as strict as the base's — never looser.

- Base says throw() (throws nothing) → override must also say throw(). That's the only option, because there's nothing stricter than "throws nothing."
- Base says throw(int) → override could say throw(int) or throw(), but not "throws anything."
- Base has no specification (throws anything) → override can say whatever it wants.

So because std::exception::what() is throw(), every override — including yours in Bureaucrat::GradeTooHighException::what() — is legally required to also be throw(). Writing just:

virtual const char* what() const { ... }   // no throw()

is a looser promise than the base ("may throw anything" vs "throws nothing"), and the compiler rejects that as an invalid override.

### 2.5 Custom exception classes as **nested classes**

```cpp
class Bureaucrat
{
    public:
        class GradeTooHighException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
        class GradeTooLowException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
    // ...
};
```

`GradeTooHighException` is declared **inside** `Bureaucrat`. This is a *nested class*
— a class scoped to another class, the same way a variable can be scoped to a
function. Outside `Bureaucrat`, its full name is `Bureaucrat::GradeTooHighException`;
inside `Bureaucrat`'s own member functions, the shorter `GradeTooHighException` is
enough because you're already in that scope. Nesting it here communicates "this
exception only makes sense in the context of a Bureaucrat" — exactly like
`std::vector<T>::iterator` is nested inside `std::vector`.

Throwing and catching it looks like this from outside the class:

```cpp
try
{
    Bureaucrat b("Bob", 200);
}
catch (Bureaucrat::GradeTooLowException& e)   // catch the specific type...
{
    std::cout << e.what() << "\n";
}
catch (std::exception& e)                      // ...or the general base
{
    std::cout << e.what() << "\n";
}
```

The subject explicitly says exception classes are **exempt from Orthodox Canonical
Form** — no need for a hand-written copy constructor, assignment operator, or
destructor on `GradeTooHighException`/`GradeTooLowException`. The compiler-generated
defaults are good enough since these classes hold no data of their own.

### 2.6 `const` member data and what it breaks

```cpp
private:
    const std::string _name;
```

Making `_name` `const` means it is initialized exactly once (in the constructor's
member-initializer list) and can **never be reassigned** for the lifetime of the
object. This has two consequences that ripple through the whole class:

1. **No default constructor is possible.** A `const` member must be initialized at
   construction — there's no "give it a value later." Since the exercise never asks
   for a `Bureaucrat` with no name, the class simply has no default constructor.
2. **The copy assignment operator cannot touch `_name`.** `_name = other._name;`
   would not compile — you cannot assign to a `const` object after it's constructed.
   `operator=` is still required (OCF says so), it just quietly does nothing to
   `_name` and only copies `_grade`:

```cpp
Bureaucrat& Bureaucrat::operator=(Bureaucrat const& other)
{
    if (this != &other)
        _grade = other._grade;   // _name is left untouched — can't assign to it
    return *this;
}
```

This is a known, intentional quirk of this exercise, not a bug — `main.cpp` test 7
demonstrates it directly (assigning `original` into `assigned` copies the grade but
`assigned` keeps its own original name).

### 2.7 `static const int` class members

Let me try a completely different way. Small and concrete.

## Two files

**math.cpp**
```cpp
int add(int a, int b)
{
    return a + b;
}
```

**main.cpp**
```cpp
int add(int a, int b);   // just a promise

int main()
{
    add(2, 3);
}
```

## Build it

```
g++ -c main.cpp   ->  main.o
g++ -c math.cpp   ->  math.o
g++ main.o math.o ->  program
```

The first two commands are **compiling**. The third is **linking**.

## What happens in each step

**Compiling main.cpp:** The compiler reads *only* `main.cpp`. It sees `add(2,3)`. It
cannot produce a `call` instruction because it doesn't know where `add` is —
`math.cpp` isn't open. But line 1 promised `add` exists, so the compiler trusts it and
produces `main.o` with a hole where the address belongs.

**Linking:** The linker opens `main.o`, sees the hole labeled `add`, opens `math.o`,
finds `add` at some address, writes that address into the hole. Done.

## Now break it

Delete `math.cpp`. Keep `main.cpp` exactly as it is.

```
g++ -c main.cpp    <- SUCCEEDS. The promise is still there.
g++ main.o         <- FAILS: undefined reference to 'add'
```

The compiler was happy. The linker wasn't. The hole labeled `add` had nothing to fill
it with, because no file anywhere actually contains `add`.

**That is a linker error.** You promised something and never delivered it.

## The same thing with a variable

Variables work identically.

**a.cpp**
```cpp
int counter = 5;      // the real variable — memory exists here
```

**main.cpp**
```cpp
extern int counter;   // just a promise
int main() { counter++; }
```

Delete `a.cpp` → `undefined reference to 'counter'`. Same error, same reason:
`main.o` has a hole where `counter`'s address goes, and no file contains the actual
variable.

## Your `_gradeMin`

This line in `Bureaucrat.hpp`:
```cpp
static const int _gradeMin;
```
is the **promise**. Like `int add(int,int);` or `extern int counter;`.

This line in `Bureaucrat.cpp`:
```cpp
const int Bureaucrat::_gradeMin = 1;
```
is the **real variable**. Like the body of `add`, or `int counter = 5;`.

If you delete the second line, you've promised something that doesn't exist. Any code
needing its address → hole → nothing to fill it → `undefined reference to
'Bureaucrat::_gradeMin'`.

---

That's it. **Header = promise. .cpp = the actual thing. Linker error = a promise
nobody kept.**

If this still isn't landing, tell me which of the three examples above broke down
for you — the `add` one, the `counter` one, or the `_gradeMin` one.

### 2.8 `operator<<` overloading for a custom type

```cpp
std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat);
```

`std::cout << someInt` works because `<<` is overloaded for every built-in type in
`<iostream>`. To make `std::cout << myBureaucrat` work, `<<` needs an overload for
`Bureaucrat` too. It **cannot** be a member function here: a member `operator<<` would
have `Bureaucrat` as its left-hand (calling) object, meaning you'd have to write
`myBureaucrat << std::cout`, which is backwards. So it's written as a **free
function** taking the stream first, the object second:

```cpp
std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat)
{
    out << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade() << ".\n";
    return out;
}
```

It works purely through `getName()`/`getGrade()` — the public getters — so no
`friend` declaration is needed (`friend` is forbidden by the subject anyway).
Returning `out` (the same stream, by reference) is what allows chaining:
`std::cout << bureaucrat1 << bureaucrat2;` — each `<<` call consumes one object and
hands back the stream for the next `<<` to use.

### 2.9 Exception safety during construction (stack unwinding)

```cpp
Bureaucrat::Bureaucrat(std::string const& name, int grade) : _name(name), _grade(grade)
{
    if (grade < _gradeMin)
        throw Bureaucrat::GradeTooHighException();
    if (grade > _gradeMax)
        throw Bureaucrat::GradeTooLowException();
    std::cout << _name << ", bureaucrat grade " << _grade << ", created.\n";
}
```

By the time the `if` checks run, `_name` has *already* been fully constructed (member
initializer lists run before the constructor body, in declaration order). If the
constructor throws, the object as a whole is considered to have never existed — the
caller never receives a `Bureaucrat` — but any **sub-objects that finished
constructing** (here, `_name`, a `std::string`) are still properly destroyed during
stack unwinding. There is no leak and no dangling half-built object; C++ guarantees
this automatically. This is why it's safe to initialize `_grade(grade)` in the list
*before* validating it — if validation fails, the whole object (including the
already-built `_name`) is cleanly torn down, and the invalid `_grade` value never
becomes observable to anyone.

**The same mechanism, on a fully-built object.** Construction failure isn't the
only place unwinding shows up — it also fires when a throw happens *after* an
object is already alive, from inside one of its member functions:

```cpp
try
{
    Bureaucrat topDog("TopDog", 1);
    topDog.incrementGrade();   // _grade-1=0 < _gradeMin(1) -> throw GradeTooHighException
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
```

Output, in this exact order:
```
TopDog, bureaucrat grade 1, created.
TopDog, bureaucrat grade 1, destroyed.
Caught: Bureaucrat grade too high (must be 1 or higher)
```

Unlike the constructor case, `topDog` here is a real, fully-constructed object
(the "created" message proves it) before `incrementGrade()` throws. So why does
its destructor fire *before* the `catch` body runs, rather than at the closing
`}` of the `try` block where it's declared?

Because a `throw` doesn't jump straight to the matching `catch` body. It happens
in two separate steps:

1. **Find the handler.** The runtime walks up the call stack looking for a
   `catch` whose type matches (`std::exception&` matches `GradeTooHighException`
   via inheritance) and locates it — *before* touching any objects.
2. **Unwind down to it.** Every fully-constructed automatic object that lives
   between the `throw` point and that matched `catch` gets its destructor called,
   in reverse order of construction. `topDog` is exactly such an object, so it is
   destroyed *as part of leaving the `try` block* — only then does control
   actually transfer into the `catch` body.

The reason this order is mandatory, not just a quirk: the `catch` body is a
**different scope** than the `try` block. If `topDog` stayed alive while the
`catch` body ran, an object would be alive after the scope that declared it was
already abandoned — something C++ never allows. Leaving a scope — whether by
falling through its closing `}`, `return`, `break`, or an exception — always
destroys every fully-constructed local object in it, in reverse order, before
the scope is considered left. An exception is just a more abrupt way of leaving
the `try` block than reaching its textual closing brace.

Contrast this with the construction-failure case above (`tooHigh`, `tooLow`):
there the throw happens *inside the constructor*, so the object never finishes
being built — there is nothing to unwind-destroy, and no "destroyed" message
appears at all. Here, the throw happens *after* construction finished, from a
later call on an already-live object — so unwinding has a real object to clean
up, and its destructor message appears before the catch runs.

---

### 2.10 Why `public` in `class GradeTooHighException : public std::exception`

Let's slow this down.

**What `catch (std::exception& e)` is really asking**

You threw a `GradeTooHighException`. Your catch clause says `std::exception&`. Those are
**different type names**.

So the compiler has to ask a question:

> "Can a `GradeTooHighException` be treated as a `std::exception`?"

If yes → this catch matches, run it.
If no → skip this catch, keep looking.

**The answer depends on the inheritance mode**

```cpp
class GradeTooHighException : public std::exception
```

**Public** inheritance means the relationship is announced to everyone. Any code,
anywhere, is allowed to say "this is a `std::exception`." Answer: **yes**.

```cpp
class GradeTooHighException : private std::exception
```

**Private** inheritance means the relationship is a secret kept inside the class. Code
*inside* `GradeTooHighException` knows about it. Code *outside* — including
`main.cpp` — is not permitted to use it.

So when the compiler asks the question from inside `main.cpp`, the answer is:
**no, not from here.**

**The consequence**

The catch clause doesn't match. It gets skipped, as if it weren't written for this
exception at all.

If `main.cpp` has only that one catch clause, nothing else matches either. The
exception keeps travelling outward, out of `main`, and there is nowhere left to go:

```
terminate called after throwing an instance of 'Bureaucrat::GradeTooHighException'
Aborted (core dumped)
```

The program dies — even though a `try`/`catch` was written that *looks* like it
should have handled it.

**Why "silently"**

That's the nasty part. **This still compiles.** No warning, no error. The compiler
doesn't say "hey, your catch will never match." It just quietly doesn't match, and
you find out when the program aborts at runtime.

**Try it**

Change one word in `Bureaucrat.hpp`:

```cpp
class GradeTooHighException : private std::exception
```

Recompile. Run. A test that previously printed "Caught: ..." will abort instead.

Change it back to `public`. It works again.

That word is what makes the object usable as a `std::exception` from the outside —
which is the entire reason it was inherited in the first place.

---

### 2.11 Why `const char*` in `virtual const char* what() const throw();`

`const char*` is a pointer to a **C-style string** (null-terminated array of `char`),
not a `std::string`. Two `const`s are doing different jobs here — worth separating.

**Why `const char*` and not `std::string`**

`std::exception` was designed to work reliably even in low-level, resource-constrained
failure situations — including things like `std::bad_alloc` (thrown when `new` fails
because memory is exhausted). If `what()` returned `std::string` by value, calling it
would require constructing a new `std::string`, which itself allocates memory. Doing a
heap allocation *while already reporting a memory-allocation failure* is exactly the
kind of thing to avoid. Returning a raw `const char*` requires no allocation at all —
it just hands back a pointer to memory that already exists. It also keeps the
interface usable everywhere, including old C-interop code and contexts with no
guarantee `<string>` machinery is safe to touch.

**Why `const char*` specifically (the two parts of that type)**

- `char*` → a pointer to (the first character of) a null-terminated string, exactly
  like a string literal's type.
- `const` in front of `char` → the caller cannot modify the characters through this
  pointer. That makes sense: `what()` typically returns a pointer to a string literal
  (`return "grade too high";`) or otherwise-owned internal data — letting the caller
  write through that pointer would be undefined behavior or corrupt shared state.

**In this project's overrides specifically**

```cpp
const char* Bureaucrat::GradeTooHighException::what() const throw()
{
    return "Bureaucrat::GradeTooHighException: grade too high (must be >= 1)";
}
```

That string literal `"..."` has **static storage duration** — it exists for the
entire life of the program, in read-only memory, independent of the exception object.
So returning a pointer to it is completely safe: the pointer stays valid long after
the function returns, no matter when or how the caller uses it, and there's no
ownership to manage (no `delete` needed, no dangling risk from the exception object
being destroyed). That's also why lifetime is never a concern here the way it would
be with a pointer to a local `char` array — a string literal isn't "local," it
outlives everything.

---

## 3. File-by-file, line-by-line walkthrough

### 3.1 `include/Bureaucrat.hpp`

```cpp
#ifndef BUREAUCRAT_HPP
#define BUREAUCRAT_HPP
```
Include guard, all-caps, matching the filename — prevents double inclusion if two
`.cpp` files both `#include "Bureaucrat.hpp"` and get compiled together.

```cpp
#include <string>
#include <exception>
#include <ostream>
```
- `<string>` for `std::string` (`_name`, `getName()`).
- `<exception>` for `std::exception`, the base class our two nested exceptions derive
  from.
- `<ostream>` for `std::ostream`, used only by the `operator<<` declaration. (Not
  `<iostream>` — headers should pull in only what they declare, and `<ostream>`
  is the minimal header that defines `std::ostream`; `<iostream>` additionally
  declares the global `std::cin`/`std::cout`/`std::cerr` objects, which no header has
  any business forcing on every file that includes it. That's implemented in
  `Bureaucrat.cpp` instead, which does use `std::cout` and includes `<iostream>`.)

```cpp
class Bureaucrat
{
    public:
```
Class declaration begins; everything below `public:` is usable from outside the class.

```cpp
        Bureaucrat(std::string const& name, int grade);
```
The only constructor. Takes the name by `const&` (no need to copy the caller's string
just to read it) and the grade by value (an `int` is cheap to copy — passing it by
reference would gain nothing).

```cpp
        Bureaucrat(Bureaucrat const& other);
        Bureaucrat& operator=(Bureaucrat const& other);
        ~Bureaucrat();
```
The rest of Orthodox Canonical Form: copy constructor, copy assignment (returns
`Bureaucrat&` so `a = b = c;` chains correctly, guards against self-assignment
internally), and destructor. No default constructor — see §2.6.

```cpp
        std::string const& getName() const;
        int getGrade() const;
```
Getters. `getName()` returns `const std::string&` rather than `std::string` by value
— returning a reference to the already-existing `_name` avoids copying the whole
string just to read it. The trailing `const` on both means "this function does not
modify the object it's called on," which lets these getters be called on a
`Bureaucrat const&` (exactly the situation `operator<<` is in).

```cpp
        void incrementGrade();
        void decrementGrade();
```
Grade mutators — declared here, defined (with the range checks) in the `.cpp`.

```cpp
        class GradeTooHighException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };

        class GradeTooLowException : public std::exception
        {
            public:
                virtual const char* what() const throw();
        };
```
The two nested exception types — see §2.5. `virtual` appears here in the declaration
per this project's convention (never repeated in the `.cpp` definition).

```cpp
    private:
        const std::string _name;
        int _grade;

        static const int _gradeMin;
        static const int _gradeMax;
};
```
Data members: `_name` is `const` (see §2.6), `_grade` is a plain mutable `int`. The
two `static const int` bounds are declared here and defined once in the `.cpp` (§2.7).
Declaration order matters — `_name` before `_grade`, matching the constructor's
initializer list, since C++ always initializes members in **declaration order**
regardless of the order written in the initializer list.

```cpp
std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat);

#endif
```
Free-function declaration for the insertion operator (§2.8), living outside the class
body since it's not a member. Closes the include guard.

---

### 3.2 `src/Bureaucrat.cpp`

```cpp
#include <iostream>
#include "Bureaucrat.hpp"
```
`<iostream>` here (not in the header) because this file is the one that actually
writes to `std::cout`.

```cpp
const int Bureaucrat::_gradeMin = 1;
const int Bureaucrat::_gradeMax = 150;
```
Out-of-line definitions for the static members declared in the header — see §2.7.
`Bureaucrat::` scopes them to the class; without it the compiler would think these
are two unrelated free globals named `_gradeMin`/`_gradeMax`.

```cpp
Bureaucrat::Bureaucrat(std::string const& name, int grade) : _name(name), _grade(grade)
{
    if (grade < _gradeMin)
        throw Bureaucrat::GradeTooHighException();
    if (grade > _gradeMax)
        throw Bureaucrat::GradeTooLowException();
    std::cout << _name << ", bureaucrat grade " << _grade << ", created.\n";
}
```
Member-initializer list sets `_name` and `_grade` before the body runs (see §2.9 for
why validating *after* initializing `_grade` is still safe). `grade < _gradeMin`
catches grade 0 or negative (too prestigious to exist); `grade > _gradeMax` catches
151+ (too lowly to exist). `Bureaucrat::GradeTooHighException()` default-constructs a
temporary exception object and `throw`s it — the `Bureaucrat::` qualifier is actually
optional inside a `Bureaucrat` member function (we're already in that scope) but is
kept for clarity/explicitness. If neither check fires, the constructor finishes
normally and prints the "created" message.

```cpp
Bureaucrat::Bureaucrat(Bureaucrat const& other) : _name(other._name), _grade(other._grade)
{
    std::cout << _name << ", bureaucrat grade " << _grade << ", copied.\n";
}
```
Copy constructor: both members are copied directly through the initializer list — no
validation needed, since `other` is by definition already a valid, fully-constructed
`Bureaucrat` (its own constructor already checked its grade).

```cpp
Bureaucrat& Bureaucrat::operator=(Bureaucrat const& other)
{
    if (this != &other)
    {
        _grade = other._grade;
    }
    std::cout << _name << ", bureaucrat grade " << _grade << ", assigned.\n";
    return *this;
}
```
`if (this != &other)` is the standard self-assignment guard (`a = a;` should be a
no-op, not undefined behavior). `_name` is deliberately not touched — see §2.6.
`return *this;` returns the current object by reference so assignments can chain
(`a = b = c;`).

```cpp
Bureaucrat::~Bureaucrat()
{
    std::cout << _name << ", bureaucrat grade " << _grade << ", destroyed.\n";
}
```
Destructor. No dynamic memory to free — `std::string`'s own destructor (called
automatically right after this body finishes) handles `_name`'s internal buffer. This
message exists purely so peer evaluation can see the object's lifetime on screen, per
this project's "constructors and destructors must always print" rule.

```cpp
std::string const& Bureaucrat::getName() const
{
    return _name;
}

int Bureaucrat::getGrade() const
{
    return _grade;
}
```
Straightforward accessors, matching the `.hpp` declarations exactly (including the
trailing `const`).

```cpp
void Bureaucrat::incrementGrade()
{
    if (_grade - 1 < _gradeMin)
        throw Bureaucrat::GradeTooHighException();
    _grade--;
}
```
"Increment" means *rank up*, i.e. move the numeric grade **down** toward 1 (grade 1 is
the best). Checks whether decrementing by one would cross below `_gradeMin` *before*
actually doing it — this avoids ever leaving `_grade` in an invalid state, even
momentarily.

```cpp
void Bureaucrat::decrementGrade()
{
    if (_grade + 1 > _gradeMax)
        throw Bureaucrat::GradeTooLowException();
    _grade++;
}
```
Mirror image: "decrement" moves the grade numerically **up** toward 150 (the worst
rank), checked the same way before mutating.

```cpp
const char* Bureaucrat::GradeTooHighException::what() const throw()
{
    return "Bureaucrat grade too high (must be 1 or higher)";
}

const char* Bureaucrat::GradeTooLowException::what() const throw()
{
    return "Bureaucrat grade too low (must be 150 or lower)";
}
```
`Bureaucrat::GradeTooHighException::` — two scope levels, because the function is
defined outside both the outer class (`Bureaucrat`) and the nested class
(`GradeTooHighException`). Each returns a `const char*` literal — string literals have
static storage duration (they live for the whole program), so returning a pointer to
one is always safe, no dangling pointer risk. `virtual` is **not** repeated here
(project convention — it only goes in the `.hpp`); `const throw()` **is** repeated,
because the exception specification is part of the function's exact signature and
must match the declaration.

```cpp
std::ostream& operator<<(std::ostream& out, Bureaucrat const& bureaucrat)
{
    out << bureaucrat.getName() << ", bureaucrat grade " << bureaucrat.getGrade() << ".\n";
    return out;
}
```
See §2.8. Note this only calls `bureaucrat.getName()`/`getGrade()` — both public — so
no special access privileges (and no `friend`) are needed.

---

### 3.3 `src/main.cpp`

The file runs seven independent test blocks; each is wrapped in its own `{ }` scope or
`try/catch` so constructors/destructors fire predictably and one test's objects don't
leak into the next test's output.

```cpp
std::cout << "--- 1. valid construction + operator<< ---\n";
{
    Bureaucrat bob("Bob", 42);
    std::cout << bob;
}
```
Extra braces `{ }` create a scope that ends right after the `std::cout << bob;` line,
forcing `bob`'s destructor to run immediately (and print) rather than at the end of
`main()` — this keeps each test's "created"/"destroyed" messages next to each other
in the output instead of all the destructors piling up at the very end.

```cpp
std::cout << "\n--- 2. construction with grade too high (0) ---\n";
try
{
    Bureaucrat tooHigh("TooHigh", 0);
    (void)tooHigh;
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
```
Grade `0` is below `_gradeMin` (1), so the constructor throws
`GradeTooHighException` before finishing — `tooHigh` never actually comes into
existence, so the `(void)tooHigh;` line is technically unreachable, but is kept to
avoid an "unused variable" warning if the exception is ever removed during
experimentation. `catch (std::exception& e)` catches it via the base class (§2.2) and
`e.what()` resolves virtually to `GradeTooHighException::what()`.

```cpp
std::cout << "\n--- 3. construction with grade too low (151) ---\n";
try
{
    Bureaucrat tooLow("TooLow", 151);
    (void)tooLow;
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
```
Mirror of test 2, with grade `151` (above `_gradeMax`) triggering
`GradeTooLowException` instead.

```cpp
std::cout << "\n--- 4. increment / decrement within range ---\n";
{
    Bureaucrat alice("Alice", 3);
    std::cout << alice;
    alice.incrementGrade();
    std::cout << alice;
    alice.decrementGrade();
    alice.decrementGrade();
    std::cout << alice;
}
```
`alice` starts at grade 3. One `incrementGrade()` moves her to 2 (better rank). Two
`decrementGrade()` calls move her from 2 → 3 → 4 (worse rank each time), demonstrating
both directions safely within bounds.

```cpp
std::cout << "\n--- 5. incrementGrade() past grade 1 throws ---\n";
try
{
    Bureaucrat topDog("TopDog", 1);
    topDog.incrementGrade();
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
```
`topDog` starts already at the best possible grade (1). Calling `incrementGrade()`
would need to go to 0, which is invalid, so it throws `GradeTooHighException` — this
time from a *method call*, not the constructor, showing the same exception type is
reused by both.

```cpp
std::cout << "\n--- 6. decrementGrade() past grade 150 throws ---\n";
try
{
    Bureaucrat bottomFeeder("BottomFeeder", 150);
    bottomFeeder.decrementGrade();
}
catch (std::exception& e)
{
    std::cout << "Caught: " << e.what() << "\n";
}
```
Mirror of test 5 at the opposite boundary: grade 150 is already the worst possible,
so `decrementGrade()` throws `GradeTooLowException`.

```cpp
std::cout << "\n--- 7. copy constructor and copy assignment ---\n";
{
    Bureaucrat original("Original", 10);
    Bureaucrat copyCtor(original);
    std::cout << copyCtor;

    Bureaucrat assigned("Assigned", 99);
    assigned = original;
    std::cout << assigned;
    std::cout << "Note: names differ on purpose (" << assigned.getName()
        << " vs " << original.getName() << ") because _name is const"
        " and is not copied by operator=.\n";
}
```
`copyCtor(original)` invokes the copy constructor — the printed grade (10) matches
`original` exactly, including the name, since the copy constructor *does* copy
`_name` (it's only reassignment of an existing `const` member that's illegal, not
initializing a brand-new one). `assigned = original` invokes `operator=` on an
*already-existing* object: `assigned` keeps its own name ("Assigned") because
`operator=` cannot touch `_name`, but its grade becomes `10`, copied from `original`
— exactly the asymmetry described in §2.6, printed out explicitly so it's visible in
the test output rather than just asserted in a comment.

```cpp
std::cout << "\n--- end of main ---\n";
return 0;
```
Final marker line, then a normal `0` exit.
