# Contributing to cguard

## Rules

- Keep the library header-only.
- Keep the public API small.
- Avoid heap allocation inside the library itself.
- Preserve the separation between core scope guards and hosted adapters.
- Do not add AI or Copilot attribution to commits or release notes.

## Working locally

```sh
cmake -S . -B build -DCGUARD_BUILD_TESTS=ON -DCGUARD_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

## Change expectations

- Add or update tests for behavior changes.
- Update docs when public symbols or portability limits change.
- Keep borrowed message ownership and cleanup failure behavior explicit.
