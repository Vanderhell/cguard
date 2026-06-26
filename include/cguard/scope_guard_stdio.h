#ifndef CGUARD_SCOPE_GUARD_STDIO_H
#define CGUARD_SCOPE_GUARD_STDIO_H

#include <stdio.h>

#include <cguard/scope_guard.h>

#if CGUARD_SCOPE_GUARD_ENABLE && CGUARD_HAS_SCOPE_GUARD

#define SG_DEFINE_FCLOSE_CLEANUP(name)                                        \
    SG_DEFINE_PTR_CLEANUP(name, FILE *, fclose)

#endif

#endif
