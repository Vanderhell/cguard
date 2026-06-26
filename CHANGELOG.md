# Changelog

## [2.0.0] - 2026-06-25

- Replaced anonymous `RESULT(T)` usage with named `RESULT_DEFINE(name, value_type)` results.
- Removed `.ok` from the result state model; `status` is authoritative.
- Added buffer-based errno message formatting with defined message lifetime.
- Split core scope guards from hosted stdlib and stdio adapters.
- Added capability macros and disabled/required scope-guard configuration.
- Added multi-translation-unit and scope-disabled build coverage.

## [1.0.0] - 2026-03-06

- Initial header-only release.
