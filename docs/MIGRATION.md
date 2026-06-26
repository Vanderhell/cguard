# Migration

## From anonymous `RESULT(T)`

- Replace repeated anonymous `RESULT(T)` declarations with `RESULT_DEFINE(name, value_type)`.
- Update function signatures, variables, and headers to use the named type.

## From `.ok`

- Check `status == RESULT_OK` or use `result_is_ok()` / `name##_is_ok()`.

## From pointer-to-pointer releasers

- If you used `SG_DEFINE_PTR_CLEANUP`, change the releaser to accept the held value, not a pointer-to-pointer.
- Keep direct `SG_DEFER` callbacks on the cleanup ABI, which receives a pointer to the guarded variable.
