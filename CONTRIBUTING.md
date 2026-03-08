# Contributing to cguard

Thanks for your interest. cguard values correctness, simplicity, and minimal footprint.

---

## Guiding principles

1. **Header-only** — no source files, no build step for users.
2. **No dependencies** — standard C library only.
3. **No magic** — every macro should be understandable in 30 seconds.
4. **Document everything** — every public symbol gets a Doxygen comment.

---

## Getting started

```sh
git clone https://github.com/Vanderhell/cguard.git
cd cguard
make run-tests    # all tests must pass
```

---

## Submitting changes

1. Fork the repo and create a branch: `git checkout -b feature/my-thing`
2. Write code + tests + documentation.
3. Run `make run-tests` — zero failures required.
4. Open a Pull Request with a clear description of the change and why.

---

## Code style

- C11, `snake_case` for everything.
- Public macros: `SG_` prefix (scope_guard) or `RESULT_` prefix (result).
- Internal symbols: `_sg_` or `_result_` prefix (double underscore reserved for compiler).
- 4-space indent, no tabs.
- Max line length: 100 characters.
- Every public API symbol must have a Doxygen `/** ... */` comment.

---

## What we accept

- Bug fixes with a failing test that demonstrates the bug.
- New cleanup helpers (`SG_AUTO_*`) for common patterns.
- New `result_status_t` codes with strong justification.
- Portability improvements (e.g. C99 compat, new compiler support).
- Documentation improvements.

## What we don't accept

- Runtime overhead (no hidden heap allocs, no global state).
- MSVC workarounds that complicate the codebase — better to document the limitation.
- New header files that expand scope beyond cleanup + result.

---

## Reporting issues

Open a GitHub issue with:
- Compiler name and version (`gcc --version`)
- OS / platform
- Minimal reproducer
