# ex00 — `main()` Trace

Traces every line of `src/main.cpp` to the output it produces, annotating
which variable each constructor/destructor call belongs to. Verified against
a fresh `make re && ./bureaucrat` run.

---

## Block 1 — lines 6–10: valid construction + `operator<<`

```cpp
Bureaucrat bob("Bob", 42);
std::cout << bob;
```

- `line 8`: calls `Bureaucrat(std::string const&, int)`. Member-init list sets
  `_name="Bob"`, `_grade=42` *before* the body runs. Body checks `42 < 1`? no.
  `42 > 150`? no. Falls through to the print:
  ```
  Bob, bureaucrat grade 42, created.
  ```
- `line 9`: `std::cout << bob` resolves to the free function
  `operator<<(std::ostream&, Bureaucrat const&)` (`Bureaucrat.cpp:81`), which
  calls `getName()`/`getGrade()`:
  ```
  Bob, bureaucrat grade 42.
  ```
- `line 10` (closing `}`): `bob` goes out of scope → destructor:
  ```
  Bob, bureaucrat grade 42, destroyed.
  ```

---

## Block 2 — lines 13–21: grade too high (0)

```cpp
Bureaucrat tooHigh("TooHigh", 0);
```

- Member-init list sets `_grade=0` — the object is *not yet fully
  constructed*, this is just data being placed in memory that will become
  `tooHigh` only if the constructor body finishes.
- Body: `0 < _gradeMin(1)` → true → `throw Bureaucrat::GradeTooHighException();`
- Because the throw happens *inside the constructor*, the object is
  considered never successfully constructed. `tooHigh` never exists — no
  "created" message, and **no destructor runs for it**.
- The temporary `GradeTooHighException` propagates up, matches
  `catch (std::exception& e)` at line 18 (caught **by reference**, so the
  virtual override is preserved, not sliced).
- `e.what()` dynamically dispatches to
  `Bureaucrat::GradeTooHighException::what()`:
  ```
  Caught: Bureaucrat grade too high (must be 1 or higher)
  ```

---

## Block 3 — lines 24–32: grade too low (151)

Same shape: `151 > _gradeMax(150)` → throws `GradeTooLowException`, caught,
`what()`:
```
Caught: Bureaucrat grade too low (must be 150 or lower)
```

---

## Block 4 — lines 35–43: increment/decrement within range

```cpp
Bureaucrat alice("Alice", 3);   // "Alice, bureaucrat grade 3, created."
std::cout << alice;             // "Alice, bureaucrat grade 3."
alice.incrementGrade();         // _grade-1=2, 2 < 1? no -> _grade-- -> _grade=2
std::cout << alice;             // "Alice, bureaucrat grade 2."
alice.decrementGrade();         // _grade+1=3, 3 > 150? no -> _grade++ -> _grade=3
alice.decrementGrade();         // _grade+1=4, 4 > 150? no -> _grade++ -> _grade=4
std::cout << alice;             // "Alice, bureaucrat grade 4."
```

Note the naming is inverted from intuition: lower grade number = higher rank,
so `incrementGrade()` actually *decreases* `_grade` (moves toward rank 1) and
`decrementGrade()` *increases* it — matches the subject's spec.

- End of block `}` → destructor on `alice` (now grade 4):
  ```
  Alice, bureaucrat grade 4, destroyed.
  ```

---

## Block 5 — lines 46–54: `incrementGrade()` past grade 1

```cpp
Bureaucrat topDog("TopDog", 1);   // "TopDog, bureaucrat grade 1, created."
topDog.incrementGrade();          // _grade-1=0, 0 < _gradeMin(1)? yes -> throw GradeTooHighException
```

- The throw happens *inside a member function call*, not the constructor —
  `topDog` is a fully-constructed, live local object at this point. Stack
  unwinding for the exception destroys it *before* control reaches the
  `catch` block:
  ```
  TopDog, bureaucrat grade 1, destroyed.
  ```
- Then the `catch` block runs:
  ```
  Caught: Bureaucrat grade too high (must be 1 or higher)
  ```

This ordering — destructor message before the caught message — is stack
unwinding in action: C++ guarantees every fully-constructed automatic object
between the `throw` and the matching `catch` gets destroyed as the stack
unwinds, and that happens before the handler body executes.

### Deep dive: why does `topDog` die before the `catch` body runs?

The confusion is thinking of `topDog`'s lifetime as tied to the *textual*
`try { }` braces, like a normal scope. It isn't. Once a `throw` happens,
normal scope-exit rules are replaced by **stack unwinding**, which is a
different (earlier) mechanism.

Step by step, in order:

```cpp
Bureaucrat topDog("TopDog", 1);   // fully constructed, alive
topDog.incrementGrade();          // _grade-1=0 < _gradeMin(1) -> throw GradeTooHighException()
```

At the moment of `throw`, the runtime does **not** jump straight to the
`catch` body. It does this instead:

1. **Find a matching handler.** It walks up the call stack looking for a
   `catch` clause whose type matches the thrown exception (`std::exception&`
   matches `GradeTooHighException`, since it inherits from `std::exception`).
   It finds the one in this code.
2. **Unwind the stack down to that handler.** "Unwinding" means: for every
   fully-constructed automatic (local, non-heap) object that lives between
   the `throw` point and the matched `catch`, call its destructor, in
   reverse order of construction. `topDog` is exactly such an object — it
   lives inside the `try` block, between the throw (inside
   `incrementGrade()`, called on `topDog`) and the `catch`. So its
   destructor runs **now**, as part of unwinding:
   ```
   TopDog, bureaucrat grade 1, destroyed.
   ```
3. **Only after unwinding completes** does control actually transfer into the
   `catch (std::exception& e)` body:
   ```
   Caught: Bureaucrat grade too high (must be 1 or higher)
   ```

**Why this has to be true, not just "the rule":** think about *why*
`topDog` must die before the `catch` body runs, not after. The `catch` body
is a **different scope** than the `try` block. If `topDog` were still alive
while the `catch` body executed, that would mean an object could stay alive
after the scope that declared it was abandoned — a dangling scope with a
live object in it, which C++ never allows. The language guarantees: leaving
a scope (however you leave it — falling through the `}`, `return`, `break`,
or an exception) destroys every fully-constructed local object declared in
it, in reverse order, before you're considered to have left. An exception is
just a more abrupt way of leaving the `try` block than reaching its closing
`}`.

**Contrast with the normal (no-throw) case:** if `incrementGrade()` never
threw, `topDog` would live until the closing `}` of the `try` block — the
destructor message would appear right at the brace, with no catch involved.
The exception path just moves that same destructor call earlier, to the
exact moment the object's scope is torn down by the throw, instead of
waiting for the textual end of the block.

**Why blocks 2–3 (`tooHigh`, `tooLow`) look different:** there, the throw
happens *inside the constructor itself* — the object never finishes being
built, so there's nothing to unwind-destroy, and no "destroyed" message
appears at all. In block 5, `topDog` had *already* fully finished
construction (the "created" message proves it) before the later call to
`incrementGrade()` threw — so it's a real, live object that unwinding must
clean up.

---

## Block 6 — lines 57–65: `decrementGrade()` past grade 150

Mirror of block 5: `BottomFeeder` at grade 150, `decrementGrade()` →
`150+1=151 > 150` → throw `GradeTooLowException` → unwind destroys
`bottomFeeder` first:
```
BottomFeeder, bureaucrat grade 150, destroyed.
```
then catch:
```
Caught: Bureaucrat grade too low (must be 150 or lower)
```

---

## Block 7 — lines 68–79: copy constructor and copy assignment

```cpp
Bureaucrat original("Original", 10);   // "Original, bureaucrat grade 10, created."
Bureaucrat copyCtor(original);         // copy ctor: _name("Original"), _grade(10)
                                        // "Original, bureaucrat grade 10, copied."
std::cout << copyCtor;                 // "Original, bureaucrat grade 10."
```

Note the copy-constructor message prints `_name`/`_grade` *of the new
object* (`copyCtor`), which happens to equal `"Original"`/`10` since it was
copied — the message text is identical to `original`'s own fields, this is
not a bug, just a consequence of the copy.

```cpp
Bureaucrat assigned("Assigned", 99);   // "Assigned, bureaucrat grade 99, created."
assigned = original;                   // operator=: self-check false, copies only _grade (10)
                                        // "Assigned, bureaucrat grade 10, assigned."
std::cout << assigned;                 // "Assigned, bureaucrat grade 10."
```

Note `assigned`'s `_name` is still `"Assigned"` — `operator=` never touches
`_name` (it's `const`), only `_grade` was overwritten from 99 → 10. That's the
point of this block, confirmed by the explicit print on lines 76–78:
```
Note: names differ on purpose (Assigned vs Original) because _name is const and is not copied by operator=.
```

- End of block `}` → destructors run in **reverse declaration order**:
  `assigned`, then `copyCtor`, then `original`:
  ```
  Assigned, bureaucrat grade 10, destroyed.
  Original, bureaucrat grade 10, destroyed.   <- this is copyCtor (same fields as original)
  Original, bureaucrat grade 10, destroyed.   <- this is original itself
  ```

---

## Final line

`line 81`:
```
--- end of main ---
```
then `return 0;`.

---

## Takeaways

Every constructed/destroyed message maps 1:1 to a named variable's lifetime;
the two "silent" throws (blocks 2–3) are the visible proof that a constructor
which throws never produces an object to destroy, while blocks 5–6 are the
visible proof that a fully-built object *does* get destroyed during
unwinding before the handler runs.

---

## Full captured output (fresh `make re && ./bureaucrat`)

```
--- 1. valid construction + operator<< ---
Bob, bureaucrat grade 42, created.
Bob, bureaucrat grade 42.
Bob, bureaucrat grade 42, destroyed.

--- 2. construction with grade too high (0) ---
Caught: Bureaucrat grade too high (must be 1 or higher)

--- 3. construction with grade too low (151) ---
Caught: Bureaucrat grade too low (must be 150 or lower)

--- 4. increment / decrement within range ---
Alice, bureaucrat grade 3, created.
Alice, bureaucrat grade 3.
Alice, bureaucrat grade 2.
Alice, bureaucrat grade 4.
Alice, bureaucrat grade 4, destroyed.

--- 5. incrementGrade() past grade 1 throws ---
TopDog, bureaucrat grade 1, created.
TopDog, bureaucrat grade 1, destroyed.
Caught: Bureaucrat grade too high (must be 1 or higher)

--- 6. decrementGrade() past grade 150 throws ---
BottomFeeder, bureaucrat grade 150, created.
BottomFeeder, bureaucrat grade 150, destroyed.
Caught: Bureaucrat grade too low (must be 150 or lower)

--- 7. copy constructor and copy assignment ---
Original, bureaucrat grade 10, created.
Original, bureaucrat grade 10, copied.
Original, bureaucrat grade 10.
Assigned, bureaucrat grade 99, created.
Assigned, bureaucrat grade 10, assigned.
Assigned, bureaucrat grade 10.
Note: names differ on purpose (Assigned vs Original) because _name is const and is not copied by operator=.
Assigned, bureaucrat grade 10, destroyed.
Original, bureaucrat grade 10, destroyed.
Original, bureaucrat grade 10, destroyed.

--- end of main ---
```
