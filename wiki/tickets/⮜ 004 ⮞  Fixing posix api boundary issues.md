---
created: 2026-08-18T21:53
finished: 2026-08-19T23:03
---
#ticket/done

> DONE: basic number works, but boundaries have to be correct
> NEXT: fix boundaries for int parsing / result obj

## Tasks
- [x] Own number parse function
- [ ] ~Own float parse function~
- [x] Macro for string conversion -> Or rather function

## Description
Working on [[syshub]] i ran into issues with the posix api again and again.
I should have my own solution, that can parse string based on the length,
so that this doesn't occur anymore.

There are two major cases:
1. Parsing numbers
2. Pushing a string value to a posix api (paths etc)

**Convertion function**
Not sure why i thought it should be a macro, it can just be a fuction,
this makes more sense in this case
