# Result Types

`result_t` stores:

- `status`
- `message`

Named typed results are declared with:

```c
RESULT_DEFINE(name, value_type);
```

Use:

- `name##_ok(value)`
- `name##_err(status, message)`
- `name##_is_ok(value)`

The message pointer is borrowed storage. Do not free it.
