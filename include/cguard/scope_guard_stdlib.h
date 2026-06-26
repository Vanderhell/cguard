#ifndef CGUARD_SCOPE_GUARD_STDLIB_H
#define CGUARD_SCOPE_GUARD_STDLIB_H

#include <stdlib.h>

#include <cguard/scope_guard.h>

#if CGUARD_SCOPE_GUARD_ENABLE && CGUARD_HAS_SCOPE_GUARD

#define SG_DEFINE_FREE_CLEANUP(name, pointer_type)                            \
    SG_DEFINE_PTR_CLEANUP(name, pointer_type, free)

#endif

#endif
