# Auto-loaded by `gdb` when launched from this directory (gdb must be told
# to trust local .gdbinit files once — see Step 4 in gdb_reference.md).

set pagination off
set print pretty on
set confirm off

# Catch every throw automatically on every debug session in this project —
# no need to type it each time.
catch throw

# Shortcut: `bt` already exists, but here's how you'd define your own.
# Example: a one-liner to dump an object's vtable pointer given a pointer name.
define vptr
  print *(void**)$arg0
end
document vptr
Usage: vptr <pointer-expression>
Prints the vtable pointer stored at the start of a polymorphic object.
end
