# Changelog

All notable changes to cguard are documented here.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).
Versioning follows [Semantic Versioning](https://semver.org/).

---

## [1.0.0] – 2026-03-06

### Added
- `scope_guard.h` – RAII-style cleanup via `__attribute__((cleanup))`
  - `SG_DEFER(func)` core macro
  - `SG_AUTO_FREE` – automatic `free()`
  - `SG_AUTO_FCLOSE` – automatic `fclose()`
  - `SG_DEFINE_MUTEX_CLEANUP` – generate typed mutex cleanup functions
  - `sg_free`, `sg_fclose` built-in cleanup functions
- `result.h` – unified success/failure return values
  - `result_t` base type with `.ok`, `.status`, `.msg`
  - `RESULT(T)` typed result macro
  - `RESULT_OK_VAL()`, `RESULT_ERR_VAL()`, `RESULT_ERR_MSG()` constructors
  - `RESULT_VAL_OK()`, `RESULT_VAL_ERR()` typed constructors
  - `RESULT_TRY`, `RESULT_TRY_CALL`, `RESULT_GOTO` propagation helpers
  - `result_status_str()` human-readable status
  - `result_from_errno()` POSIX errno adapter
- `cguard.h` umbrella header
- CMake build system with install support
- Makefile for simple builds
- Full test suite (`tests/`)
- Examples (`examples/`)
