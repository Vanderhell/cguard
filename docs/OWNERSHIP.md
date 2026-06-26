# Ownership

- `result_t.message` is borrowed storage.
- `result_from_errno_buffer()` writes into caller-owned storage.
- `result_from_errno()` uses internal storage with defined lifetime.
- Scope-guard cleanup does not report cleanup failures back to the guarded code.
