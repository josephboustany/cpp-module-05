# ex00 — Defense Sheet (Bureaucrat)

Quick-recall cheat sheet for peer evaluation / oral defense. Read this first
when coming back after a while; drill into `ex00_explained.md` (concepts) or
`ex00_trace.md` (line-by-line output trace) if a point needs more depth.

---

## What this exercise wants you to learn

Module 5 is about **exceptions**, and `Bureaucrat` is the vehicle for six
specific lessons, all of which a peer evaluator can probe:

1. **`throw` / `try` / `catch` is a *separate* control-flow channel** from
   normal `return`s — an exception unwinds the stack until it finds a
   matching handler, skipping all the normal code in between.
2. **Custom exceptions are made by inheriting `std::exception`** and
   overriding `what()` — not by inventing your own error-reporting scheme.
3. **Catch by reference, not by value** — this is the single most-tested
   trap in this exercise. Catching by value slices the object and silently
   loses your override.
4. **A `const` member changes what the class can even offer** — no default
   constructor, an `operator=` that can only touch some members. This is
   Module 4/5 OCF discipline meeting a real design constraint, not an
   exception to it.
5. **Stack unwinding is safe and automatic** — both when a constructor
   throws mid-build, and when a fully-alive object's method throws later.
   Nothing leaks, nothing dangles, in both cases.
6. **Class-level constants (`static const int`) belong to the class, not
   any instance** — declared in the header, defined once in the `.cpp`.

If a peer asks "why does this exercise exist", the honest answer is: to
force you to design a class around a real invariant (`grade` must stay in
`[1, 150]`) that can be violated at two different moments (construction,
and later mutation) — and to handle both moments idiomatically with
exceptions instead of error codes or `assert`.

---

## Talking points, one per concept (say these out loud, don't just recognize them)

### 1. Why is there no default constructor?
`_name` is `const std::string`. A `const` member must be initialized once,
at construction, from the member-initializer list — there is no valid
"empty" `_name` to default to, so a default constructor would have nothing
correct to write. The exercise forces you to notice this the moment you try
to write one and the compiler rejects an uninitialized `const` member.

### 2. Why does `operator=` only copy `_grade`?
Because `_name` is `const`, assigning to it after construction is a compile
error — you *cannot* write `_name = other._name;` no matter how the method
is structured. So the copy-assignment operator is necessarily asymmetric:
it copies `_grade`, leaves `_name` alone, and still returns `*this` and
still guards `this != &other`. This is demonstrated deliberately in block 7
of `main()` (`assigned = original` keeps the name `"Assigned"`).

### 3. Why catch by reference and never by value?
`catch (std::exception& e)` binds directly to the thrown object — its vptr
is untouched, so `e.what()` dispatches virtually to the derived override
(`GradeTooHighException::what()`). `catch (std::exception e)` **copies**
the thrown object into a new `std::exception`, and that copy only has the
base subobject — the derived part is sliced off, the vptr resets to the
base's vtable, and `e.what()` silently returns the wrong string instead of
erroring. This is a silent bug, not a compile error, which is exactly why
it's worth defending confidently: nothing will warn you if you get it wrong.

### 4. Why are the exceptions nested inside `Bureaucrat`?
Grouping: `GradeTooHighException`/`GradeTooLowException` only make sense in
relation to a `Bureaucrat`'s grade. Nesting communicates that scope and
avoids cluttering the global namespace. Practical consequence: outside the
class you must qualify the name (`Bureaucrat::GradeTooHighException`);
inside the class's own members, the short name works.

### 5. Why `public` inheritance from `std::exception` and not `private`?
`catch (std::exception& e)` needs `GradeTooHighException` to *be usable
as* an `std::exception` from outside the class — that's an is-a
relationship, which is what `public` inheritance means. With `private`
inheritance, `what()` becomes private in the derived class from any
outside caller's point of view, and the `catch` clause in `main()` would
not compile.

### 6. Why `virtual const char* what() const throw();` exactly, word for word?
- `virtual` — inherited from the base declaration; must match to override,
  not to declare fresh, but the keyword is only repeated in `.hpp` per this
  project's convention (never in `.cpp`).
- `const` (after the parameter list) — `what()` must be callable on a
  `const std::exception&`, which is exactly what a `catch` clause binds to.
- `throw()` — the C++98 exception specification; must be repeated exactly
  to match the base class signature, otherwise it's not a valid override.
  It also documents (and the compiler can rely on) the guarantee that
  `what()` itself will never throw — it does no allocation, no string
  construction, nothing that could fail.

### 7. What actually happens when the constructor throws (blocks 2–3)?
The member-initializer list runs first (`_name` gets fully built), *then*
the body validates `grade` and throws if it's out of range. Because the
`Bureaucrat` never finishes constructing, the caller never receives an
object — there is no "created" message and no matching destructor call.
But `_name`, the sub-object that *did* finish building, is still cleanly
destroyed during unwinding. Nothing leaks; the invalid `_grade` is never
observed by anyone.

### 8. What actually happens when a *live* object's method throws (blocks 5–6)?
This is the trickier one to explain well, so get the two-step model right:
1. The runtime first **finds the matching `catch`** by walking up the call
   stack — it does not touch any objects yet.
2. It then **unwinds** down to that handler, destroying every
   fully-constructed automatic object in between, in reverse order.

`topDog` is such an object — alive, fully built, sitting in the `try`
block. So its destructor runs *during* step 2, **before** control ever
enters the `catch` body. That's why the output shows
`TopDog, ..., destroyed.` before `Caught: ...` — not because of the
program's textual order, but because leaving a scope (by exception, same
as by `return`/`break`/falling off the `}`) always destroys its locals
before the scope is considered exited, and the `catch` body is a different
scope than the `try` body.

### 9. Why `static const int _gradeMin;` declared in the header but defined in the `.cpp`?
The in-class form gives the compiler a *value* to use at compile time
(inlining comparisons, etc.) but not a real storage location. If anything
ever needs an actual address for it (taking `&_gradeMin`, binding it to a
reference — an "odr-use"), the linker needs one real definition to point
to. Without the out-of-class `const int Bureaucrat::_gradeMin = 1;` in the
`.cpp`, that use fails at **link time** with "undefined reference", not at
compile time — a subtle failure mode worth being able to describe if asked.

### 10. What's the point of the free-function `operator<<`?
It can't be a member because the left-hand operand of `<<` is `std::cout`
(an `std::ostream`), not a `Bureaucrat` — a member operator would need the
`Bureaucrat` on the left. Being a free function also means it only needs
`getName()`/`getGrade()` (public accessors), so it doesn't need `friend`
access — consistent with this project's "no `friend`" rule.

---

## Common peer-eval traps to pre-empt

- **"Why not `catch (std::exception e)`?"** — Have the slicing answer from
  point 3 ready verbatim; this is the #1 question for this exercise.
- **"Show me it throws for grade 0 and grade 151."** — Know off-hand that
  `_gradeMin = 1` (grade **too high** in rank-number terms, i.e. grade < 1)
  throws `GradeTooHighException`, and `_gradeMax = 150` (grade too low in
  rank, i.e. grade > 150) throws `GradeTooLowException`. The naming is
  numerically inverted from what "high/low" sounds like — rank 1 is the
  *highest* rank, so a grade below 1 is "too high" a rank to exist.
- **"What if I copy-construct or assign, does the copy get re-validated?"**
  — No: the copy/assignment constructors trust that `other` already passed
  validation in its own constructor. Be ready to justify that as
  intentional, not a missed check.
- **"Destroy the object mid-`try` and show me the order."** — This is
  exactly block 5/6 from `main()`. Be able to narrate the two-step
  find-handler-then-unwind model from point 8 without hesitating.

---

## Where to look for more depth

- `ex00_explained.md` — full concept write-ups with code examples (§2.1–2.11).
- `ex00_trace.md` — every line of `main()` mapped to its exact output, with
  the stack-unwinding deep dive under Block 5.
