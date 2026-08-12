---
created: 2026-08-04T23:10
finished: 2026-08-14T11:10
---
#ticket/closed

[[Removing Transient Pool first approach]]

## Tasks
- [x] Int printing up to [[u64]] 
- [x] 2 Versions for signed and unsigned required
- [x] Remove need for intermediate functions (`expand_xxx`)
- [x] Functions should accept [[PoolOptions]]
- [x] Format prints into the buffer directly
- [x] Fix printing error
- [x] Function rework
    - [ ] ~~Print float optimizations~~ -> Not necessary
    - [x] Write binary
    - [x] Print num ([[i64]])
- [x] Circular buffer handling
- [x] Fix *pool_check_next* handling, it doesn't handle circular buffers ...

## Description
As it turned out, the first approach was not a good idea, the 
[[data structures]] turned into a mess really quick, and everything got too 
complicated. 

And I also realized, that it would be way easier, to just use the default print 
way there as well, just do it directly into the target buffer. This would be 
congruent with the rest of the architecture, and would also be alot simpler.
I might be able to remove moste of the valist formatting logic that way.
Maybe even some more data - structures

**The issues**
The only really big issue is with the [[circular buffer]], because that is what 
i need the string length beforehand, to check if i write it at the beginning, 
or at the end of the buffer, to prevent split strings (one half at the end and 
the next half at the start of that buffer)

> I want to prevent it, because else i need to implement special handling in 
> every single function i might use a circular buffer

*[[realloc]] Approach*
So, if i run out of space, i will simply [[realloc]], which should not be too 
big of a overhead. I'll check the following before though
- `CanFit` - count all string length together, and see if it's already bigger 
- `LikelyFits` - add average num length + some extra buffer space (eg 16/32)
```cpp
if(!canFit || !likelyFits) -> Wrap right away
else // try to print it
    if(didntFit)  realloc();
```
With this approach, the likelyhood of hitting this case should be very low, 
so it should be fine performance wise. 
(Which the overhead should not be hughe anyway)

This might only matter for really long strings, 
but that's not what the lib is designed for anyway ...

