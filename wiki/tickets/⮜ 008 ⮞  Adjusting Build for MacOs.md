---
created: 2026-10-30T09:40
finished: 
---
#ticket/open

## Tasks
- [ ] Build fails on mac, despite using clang etc
- [ ] Also a bunch of warnings, about things aditionally

## Description
1. Apparently `%lu` is not valid on mac, because it's not quite the same type
    -> But i can use `<inttypes.h>` and then write `"...%" PRIu64 "` for 
       printf statements
    => This is a warning only
2. Deprecated attributes don't exist
    -> `[[deprecated...]]` -> `__attribute__((deprecated(...)))`
    => These are [[c23]] features, which i'm not explicitly compile for




