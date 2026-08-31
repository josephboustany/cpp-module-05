# GDB Reference — from the `scratch/Bureaucrat` session

Working notes from a live gdb session against `scratch/Bureaucrat.cpp`. Each
section is a technique that was actually run and verified, with the real
output underneath so you can see what "correct" looks like.

---

## 0. Build with debug symbols

gdb needs `-g` to map machine instructions back to source lines/variable
names. `-O0` keeps the optimizer from reordering/eliding things while you're
still learning to read what's happening.

```sh
c++ -Wall -Wextra -Werror -std=c++98 -g -O0 -c Bureaucrat.cpp -o Bureaucrat_g.o
c++ -Wall -Wextra -Werror -std=c++98 -g -O0 -c main.cpp -o main_g.o
c++ -g -O0 Bureaucrat_g.o main_g.o -o scratch_dbg
```

Your normal project binary (built without `-g`) still runs fine under gdb,
but you lose source lines, variable names, and `print`-by-name — you'd only
see raw addresses and registers. Always keep a `-g -O0` build around for
debugging.

---

## 1. Breakpoints, args, backtrace

```sh
gdb -q --batch \
  -ex "break Bureaucrat::Bureaucrat(std::string const&, int)" \
  -ex "run" \
  -ex "print name" \
  -ex "print grade" \
  -ex "bt" \
  -ex "continue" \
  scratch_dbg
```

```
Breakpoint 1, Bureaucrat::Bureaucrat (this=0x7fffffffd6a0, name="Bob", grade=42) at Bureaucrat.cpp:7
7	Bureaucrat::Bureaucrat(std::string const& name, int grade) : _name(name), _grade(grade)
$1 = "Bob"
$2 = 42
#0  Bureaucrat::Bureaucrat (this=0x7fffffffd6a0, name="Bob", grade=42) at Bureaucrat.cpp:7
#1  0x0000555555555941 in main () at main.cpp:8
```

- `break Class::Method(arg types)` — set the breakpoint. gdb needs the full
  signature only when overloads exist (constructors here have 2: default-ish
  ctor and copy ctor).
- `this=0x7fffffffd6a0` — gdb shows the implicit `this` pointer for free on
  every member function breakpoint.
- `print <name>` — inspect any variable/parameter in scope by name.
- `bt` (backtrace) — the call stack: frame `#0` is where you're stopped,
  `#1` is who called it. Here, frame `#1` shows the exact `main.cpp:8` line
  that triggered this constructor call.
- `continue` — resume until the next breakpoint hit or program exit.

---

## 2. `catch throw` — stop at the exact instant something is thrown

This is the module-5-specific one: it fires **before** your `try`/`catch` in
`main` ever sees the exception, and fires even if nothing ever catches it.

```sh
gdb -q --batch \
  -ex "catch throw" \
  -ex "run" \
  -ex "bt" \
  -ex "up" \
  -ex "print grade" \
  -ex "continue" \
  scratch_dbg
```

```
Catchpoint 1 (exception thrown), 0x00007ffff7cbb35a in __cxa_throw () from libstdc++.so.6
#0  0x00007ffff7cbb35a in __cxa_throw () from libstdc++.so.6
#1  0x000055555555535d in Bureaucrat::Bureaucrat (this=..., name="TooHigh", grade=0) at Bureaucrat.cpp:10
#2  0x00005555555559dd in main () at main.cpp:15
#1  0x000055555555535d in Bureaucrat::Bureaucrat (...) at Bureaucrat.cpp:10
10			throw Bureaucrat::GradeTooHighException();
$1 = 0
```

- `catch throw` — a *catchpoint*, not a breakpoint. Fires on any C++ `throw`,
  anywhere.
- Frame `#0` is always `__cxa_throw` itself (libstdc++'s throw machinery —
  every `throw` compiles down to a call to this). Not useful to inspect
  directly.
- `up` — move one frame up the stack, out of `__cxa_throw` and into the code
  that actually called `throw` (`Bureaucrat.cpp:10`). Now `print grade` works
  because you're back in a frame that has that variable.
- Use this when: an exception is thrown from somewhere unexpected, or
  swallowed by the wrong `catch`, or you want to see object state at the
  moment of the throw (which may be gone/changed by the time your `catch`
  block runs).
- Related: `catch catch` fires when a `catch` block is entered (the other end
  of the same trip).

---

## 3. Watchpoints — catch a variable *changing*, not a line executing

No need to guess which line mutates something — gdb tells you when it
changes and what it changed from/to.

```sh
gdb -q --batch \
  -ex "break Bureaucrat::incrementGrade" \
  -ex "run" \
  -ex "watch _grade" \
  -ex "continue" \
  -ex "bt" \
  scratch_dbg
```

```
Breakpoint 1, Bureaucrat::incrementGrade (this=0x7fffffffd6a0) at Bureaucrat.cpp:42
Hardware watchpoint 2: _grade
Old value = 3
New value = 2
Bureaucrat::incrementGrade (this=0x7fffffffd6a0) at Bureaucrat.cpp:45
#0  Bureaucrat::incrementGrade (this=0x7fffffffd6a0) at Bureaucrat.cpp:45
#1  0x0000555555555b27 in main () at main.cpp:38
```

- `watch <expr>` — can only be set once you're stopped inside a scope where
  the variable/member exists (here, after hitting the breakpoint so `this`
  and `_grade` are in scope).
- gdb picks a **hardware watchpoint** automatically when possible (fast — no
  single-stepping needed). It reports old and new value the instant the
  write happens.
- Best use: OCF bugs. e.g. "does `operator=` really leave `_name` alone?" —
  `watch _name` across an `operator=` call and see whether it ever fires.

---

## Quick pattern for one-off checks

```sh
gdb -q --batch -ex "<cmd1>" -ex "<cmd2>" ... path/to/binary
```

Runs gdb non-interactively, prints output, exits — good for quick checks
without dropping into an interactive session. Drop `--batch` and the `-ex`
flags to get the normal interactive prompt (`gdb path/to/binary`, then type
commands one at a time — better once you're exploring rather than repeating
a known sequence).

---

## 4. Vtable inspection — see virtual dispatch happen

Demo code: `scratch/gdb_demo/demo.cpp` — `Animal`/`Dog` (virtual, correct)
next to `WrongAnimal`/`WrongDog` (no `virtual`, broken) for direct contrast.

```sh
cd scratch/gdb_demo
c++ -Wall -Wextra -g -O0 demo.cpp -o demo_dbg
gdb ./demo_dbg
```

Then, at the `(gdb)` prompt:

```
(gdb) break 39
(gdb) run
(gdb) print *a
(gdb) info vtbl *a
```

```
Breakpoint 1, main () at demo.cpp:39
39		std::cout << a->makeSound() << "\n";
$1 = {_vptr.Animal = 0x555555558cb8 <vtable for Dog+16>}
vtable for 'Animal' @ 0x555555558cb8 (subobject @ 0x55555556c2b0):
[0]: 0x55555555663e <Dog::makeSound[abi:cxx11]() const>
```

Even though the compile-time type is `Animal*`, the object's hidden
`_vptr.Animal` field points at **Dog's** vtable, and slot `[0]` holds
`Dog::makeSound`, not `Animal::makeSound`. That's the entire mechanism
`virtual` buys you, visible as data.

Now continue to the `WrongAnimal` line and compare:

```
(gdb) break 43
(gdb) continue
(gdb) print *wa
```

```
Breakpoint 2, main () at demo.cpp:43
43		std::cout << wa->makeSound() << "\n";
$2 = {<No data fields>}
```

No `_vptr` field at all — because `WrongAnimal::makeSound` isn't `virtual`,
the compiler never generates a vtable for the class. There's nothing for
gdb (or the CPU) to look up at runtime; the call to `makeSound()` was
resolved to `WrongAnimal::makeSound` at compile time, which is exactly why
it prints `"..."` instead of `"Woof!"` even though the object is really a
`WrongDog`.

---

## 5. Core dumps — freeze and reload program state on demand

You don't need an actual crash. From inside a live gdb session, dump the
current state to a file, then reload that file later with no program
running at all:

```
(gdb) break 39
(gdb) run
(gdb) generate-core-file core_demo
(gdb) quit
```

```
Saved corefile core_demo
```

Now, in a **fresh** gdb invocation — no `run`, the state is frozen in the
file:

```sh
gdb ./demo_dbg core_demo
```

```
(gdb) bt
(gdb) print a
```

```
Core was generated by `.../demo_dbg'.
Program terminated with signal SIGTRAP, Trace/breakpoint trap.
#0  main () at demo.cpp:39
$1 = (Animal *) 0x55555556c2b0
```

`bt` and `print` work exactly as before — this is how you hand someone a
snapshot of a crash (or just an interesting moment) without needing them to
reproduce it live. (Note: Ubuntu's default crash handler is `apport`, not a
plain core file, so a real segfault won't drop a `core` file next to your
binary unless you configure `ulimit -c unlimited` *and* apport's
`core_pattern` — `generate-core-file` sidesteps all of that.)

---

## 6. TUI mode — watch source while you step

Unlike the sections above, this one you have to run yourself in a real
terminal — it's a curses UI, not something batch/scripted output can show.

```sh
gdb -tui ./demo_dbg
```

Then at the prompt:

```
(gdb) layout src
(gdb) break 39
(gdb) run
(gdb) next
```

You'll get a split view: source on top (current line highlighted), the
`(gdb)` command prompt below. Useful keys:

- `Ctrl-L` — redraw if the display glitches
- `Ctrl-X 2` — add a second window (e.g. registers) — repeat to cycle layouts
- `Ctrl-X 1` — back to one window
- `Ctrl-X A` — toggle TUI on/off without losing your session
- `layout asm` / `layout regs` / `layout split` — swap what the top window shows

Step through with `next`/`step` as usual and watch the highlighted line move
in real time instead of re-printing `list` after every step.

---

## 7. A project `.gdbinit` — stop retyping the same setup

`scratch/gdb_demo/.gdbinit` (gdb auto-loads a `.gdbinit` from the directory
you launch it in):

```
set pagination off
set print pretty on
set confirm off
catch throw

define vptr
  print *(void**)$arg0
end
document vptr
Usage: vptr <pointer-expression>
Prints the vtable pointer stored at the start of a polymorphic object.
end
```

**One-time setup per machine** — gdb refuses to auto-load a local
`.gdbinit` until you whitelist its directory (security: a `.gdbinit` can run
arbitrary commands, so gdb won't trust one dropped into a repo you cloned):

```sh
mkdir -p ~/.config/gdb
echo "add-auto-load-safe-path /home/joseph/data/Core_Projects/Cpp_Modules/cpp-module-05/Claude_Code" >> ~/.config/gdb/gdbinit
```

After that, every `gdb <binary>` run from anywhere under this project
auto-registers `catch throw` and the custom `vptr` command:

```sh
gdb ./demo_dbg
```
```
Catchpoint 1 (throw)
(gdb) break 39
(gdb) run
(gdb) vptr a
$1 = (void *) 0x555555558cb8 <vtable for Dog+16>
```

---

## Where to go next in this project

| Skill | Status |
|---|---|
| Breakpoints, print, backtrace | ✅ §1 |
| `catch throw` / `catch catch` | ✅ §2 |
| `watch <member>` | ✅ §3 |
| Vtable inspection | ✅ §4 |
| Core dumps | ✅ §5 |
| TUI mode | ✅ §6 (try it yourself — not batch-scriptable) |
| Project `.gdbinit` | ✅ §7 |

Next natural extensions once you're deeper into later exercises: conditional
breakpoints (`break X if grade == 150`), `finish` (run until the current
function returns and show its return value), and `info registers` /
`x/` (examine memory) for anything involving raw pointers.
