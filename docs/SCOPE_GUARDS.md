# Scope Guards

Core scope guards use `SG_DEFER(cleanup_fn)` only when `CGUARD_HAS_SCOPE_GUARD` is true.

Cleanup functions passed to `SG_DEFER` must match the compiler cleanup ABI and accept a pointer to the guarded variable.
Releaser adapters such as `SG_DEFINE_PTR_CLEANUP(name, pointer_type, release_fn)` call `release_fn(*resource)`, so releasers can stay value-taking.

Hosted adapters live in separate headers:

- `cguard/scope_guard_stdlib.h`
- `cguard/scope_guard_stdio.h`

`SG_DISMISS(var)` clears the guarded variable to transfer ownership or prevent cleanup.

Cleanup is lexical only. It does not run across `longjmp`, abort paths, `_Exit`, reset, or power loss.
