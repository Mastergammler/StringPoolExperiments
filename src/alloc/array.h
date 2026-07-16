#include "../assert.h"
#include "alloc.h"

typedef struct
{
    unsigned char magic[4];
    uint32_t count;
} ArrHead;

typedef enum
{
    ARR_OK,
    ARR_MISSING_HEADER,
    ARR_NOT_CONTIGUOUS
} ArrError;

static const unsigned char ARR_MAGIC[4] = {0xAA, 0x22, 0xAA, 0x11};

#define arr_append(arr, val)                                                   \
    ({                                                                         \
        ArrError ret = ARR_OK;                                                 \
        ArrHead* header;                                                       \
        if (arr == NULL)                                                       \
        {                                                                      \
            header = (ArrHead*)MEM_USE(sizeof(ArrHead));                       \
            header->count = 0;                                                 \
            memcpy(header->magic, ARR_MAGIC, 4);                               \
            arr = (void*)(header + 1);                                         \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            header = (ArrHead*)(arr) - 1;                                      \
        }                                                                      \
                                                                               \
        bool headerFound = memcmp(header->magic, ARR_MAGIC, 4) == 0;           \
        ASSERT(headerFound, "Invalid array. Header data is missing.");         \
        if (!headerFound)                                                      \
        {                                                                      \
            ret = ARR_MISSING_HEADER;                                          \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            void* actualElMem = (void*)MEM_USE(sizeof(*arr));                  \
            uint32_t elIdx = header->count++;                                  \
            void* elMem = &(arr)[elIdx];                                       \
            ASSERT(actualElMem == elMem,                                       \
                   "Array not contiguous! Actual location: %p :: Index %i "    \
                   "location: %p",                                             \
                   (void*)actualElMem, elIdx, (void*)elMem);                   \
            if (actualElMem == elMem)                                          \
            {                                                                  \
                (arr)[elIdx] = (val);                                          \
            }                                                                  \
            else                                                               \
            {                                                                  \
                header->count--;                                               \
                MEM_REVERT(sizeof(*arr));                                      \
                ret = ARR_NOT_CONTIGUOUS;                                      \
            }                                                                  \
        }                                                                      \
        /* macro expression, returning the error code */                       \
        ret;                                                                   \
    })

#define arr_len(arr) ((arr) ? ((ArrHead*)(arr) - 1)->count : 0)
