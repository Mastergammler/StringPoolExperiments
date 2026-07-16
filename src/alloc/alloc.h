#ifndef MGLIBC_ALLOC
#define MGLIBC_ALLOC

#include <stdint.h>

typedef void* (*AllocFn)(void* ctx, uint64_t size);
typedef void (*RevertFn)(void* ctx, uint64_t size);

typedef struct
{
    void* alloc;
    AllocFn alloc_fn;
    RevertFn revert_fn;
} AllocContext;

#ifndef MEM_USE
#define MEM_USE(size) MglibcAllocCtx->alloc_fn(MglibcAllocCtx->alloc, size)
#endif

#ifndef MEM_REVERT
#define MEM_REVERT(size) MglibcAllocCtx->revert_fn(MglibcAllocCtx->alloc, size)
#endif

static AllocContext* MglibcAllocCtx;

#endif // MGLIBC_ALLOC
