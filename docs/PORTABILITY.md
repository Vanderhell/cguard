# Portability

- Primary language mode: C11.
- Result-only builds do not require scope-guard support.
- Scope guards depend on compiler support for `__attribute__((cleanup))`.
- `CGUARD_SCOPE_GUARD_ENABLE` disables scope-guard use.
- `CGUARD_SCOPE_GUARD_REQUIRE` turns missing scope-guard support into a build error.
