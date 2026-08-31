# GDB Session Trace — `demo.cpp` (vtable inspection walkthrough)

A screenshot-by-screenshot record of the live gdb session, from opening the
folder through inspecting vtables on both the correct (`Animal`/`Dog`) and
broken (`WrongAnimal`/`WrongDog`) hierarchies. Each entry: what was typed,
what appeared, and what it means.

---

### 1. Starting point

Files app open in `scratch/gdb_demo/`, a terminal open in the same
directory (`joseph@legion5:~/.../scratch/gdb_demo`), nothing running yet.
Folder contains `demo.cpp`, the compiled debug binary `demo_dbg`, and a
leftover `core_demo` from an earlier core-dump test.

---

### 2. `gdb ./demo_dbg`

```
Reading symbols from ./demo_dbg...
Catchpoint 1 (throw)
(gdb)
```

gdb launched and loaded `demo_dbg`'s debug symbols. `Catchpoint 1 (throw)`
printed **automatically**, with no command typed for it — that's the
project's `scratch/gdb_demo/.gdbinit` auto-loading and registering
`catch throw` on every session. `(gdb)` is the interactive prompt, waiting
for the first command.

---

### 3. `layout src`

Terminal split into two panes: a source-code pane on top (showing lines
37–47, the end of `demo.cpp`, since nothing has run yet) and the regular
`(gdb)` command line at the bottom. Status bar reads
`exec: No process (src) In:` with `L??  PC: ??` — no line number or program
counter yet because the program hasn't started.

This is TUI (Text User Interface) mode: source and commands visible at the
same time, instead of scrolling text.

---

### 4. `break 39`

```
Breakpoint 2 at 0x23a5: file demo.cpp, line 39.
```

A breakpoint was set on line 39 (`std::cout << a->makeSound() << "\n";`).
It's "Breakpoint 2" because breakpoint 1 was the `catch throw` from
`.gdbinit`. In the source pane, a `b+` marker appeared in the gutter next
to line 39, marking where execution will pause.

---

### 5. `run`

```
Starting program: .../demo_dbg
Breakpoint 2, main () at demo.cpp:39
```

The program actually started executing. It ran line 37 (`{`) and line 38
(`Animal* a = new Dog();`) automatically, then paused right before line 39
runs — because that's where the breakpoint is. The source pane highlighted
line 39, the gutter marker changed to `B+>` (the `>` meaning "this is where
we're currently stopped"), and the status bar filled in `L39` and a real
`PC:` address.

At this point: `a` exists and points at a `Dog` object, but
`a->makeSound()` has **not** run yet.

---

### 6. `print *a`

```
$1 = {
  _vptr.Animal = 0x555555558cb8 <vtable for Dog+16>
}
```

Dereferenced `a` to see the actual object in memory. The only field is a
compiler-generated one, `_vptr.Animal` — a hidden pointer every polymorphic
object carries, pointing to its class's vtable (virtual function table).

The key detail: even though `a` is declared `Animal*`, the vtable pointer
stored *inside the object* points at **Dog's** table
(`<vtable for Dog+16>`), not Animal's. This is the entire mechanism behind
`virtual` — the object itself remembers its real type, independent of what
pointer type is being used to reach it.

---

### 7. `info vtbl *a`

```
vtable for 'Animal' @ 0x555555558cb8 (subobject @ 0x55555556c2b0):
[0]: 0x55555555663e <Dog::makeSound[abi:cxx11]() const>
```

Dumped the actual contents of that vtable. Slot `[0]` (the entry for
`makeSound`) holds the address of `Dog::makeSound`, not `Animal::makeSound`.
This is the literal function pointer the CPU will jump to when `a->makeSound()`
executes on line 39 — proof that dynamic dispatch is just "follow the vptr,
then follow the function pointer in the table," not magic.

---

### 8. `break 43` then `continue`

```
Breakpoint 3 at 0x55555555641c: file demo.cpp, line 43.
Continuing.
Woof!

Breakpoint 3, main () at demo.cpp:43
```

Set a second breakpoint on line 43 (`std::cout << wa->makeSound() ...`),
then resumed execution. Line 39 ran (`a->makeSound()`), correctly printing
**"Woof!"** — `Dog::makeSound` via the vtable lookup just inspected. Line 40
(`delete a;`) also ran. Execution then continued past `Animal* a`'s scope
into the broken hierarchy: line 42 (`WrongAnimal* wa = new WrongDog();`) ran,
and execution paused again at line 43, right before `wa->makeSound()` runs.

---

### 9. `print *wa`

```
$2 = {<No data fields>}
```

Dereferenced `wa` the same way `a` was dereferenced in step 6 — but this
time, **no fields at all**, not even a vptr. Because `WrongAnimal::makeSound`
is not declared `virtual`, the compiler never generates a vtable for
`WrongAnimal` or `WrongDog` in the first place. There's nothing for a
polymorphic object to carry, because there's no dynamic dispatch to support.

Direct contrast with step 6: `Animal`'s object carried a vptr because
`virtual` requested a runtime lookup mechanism; `WrongAnimal`'s object
carries nothing because no such mechanism was requested.

---

### 10. `continue`

```
Continuing.
...
[Inferior 1 (process 11105) exited normally]
```

Execution resumed. `wa->makeSound()` printed **`"..."`** — `WrongAnimal`'s
placeholder text — **not** `"Woof!"`, even though `wa` genuinely points to a
`WrongDog` object. This is the bug made visible: with no vtable to consult,
the compiler bound `wa->makeSound()` to `WrongAnimal::makeSound` at compile
time (static dispatch), based purely on the declared pointer type. The
program then ran to completion and exited normally.

---

## Summary: what the two runs proved

| | `Animal* a` (virtual) | `WrongAnimal* wa` (no virtual) |
|---|---|---|
| Object holds a vptr? | Yes — `_vptr.Animal` → Dog's vtable | No — `{<No data fields>}` |
| `makeSound()` resolved | At runtime, via vtable lookup | At compile time, from declared type |
| Output | `Woof!` (Dog's real behavior) | `...` (WrongAnimal's, wrong) |

The `virtual` keyword is not just a marker the compiler checks —
it's the difference between an object carrying a vtable pointer or not, and
that pointer is the entire runtime mechanism that makes polymorphism work.
