# cguard – simple Makefile (alternative to CMake)
# Requires GCC or Clang

CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -I./include
OUTDIR  := build

.PHONY: all tests examples clean run-tests

all: tests examples

$(OUTDIR):
	mkdir -p $(OUTDIR)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
tests: $(OUTDIR)/test_scope_guard $(OUTDIR)/test_result

$(OUTDIR)/test_scope_guard: tests/test_scope_guard.c include/scope_guard.h | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTDIR)/test_result: tests/test_result.c include/result.h | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<

run-tests: tests
	@echo "=== Running tests ==="
	$(OUTDIR)/test_scope_guard
	$(OUTDIR)/test_result
	@echo "=== All tests passed ==="

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
examples: $(OUTDIR)/example_scope_guard $(OUTDIR)/example_result

$(OUTDIR)/example_scope_guard: examples/example_scope_guard.c include/scope_guard.h | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<

$(OUTDIR)/example_result: examples/example_result.c include/result.h | $(OUTDIR)
	$(CC) $(CFLAGS) -o $@ $<

# ---------------------------------------------------------------------------
# Cleanup
# ---------------------------------------------------------------------------
clean:
	rm -rf $(OUTDIR)
