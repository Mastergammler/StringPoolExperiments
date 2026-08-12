---
created: 2026-08-12T10:20
finished: 
---
#ticket/open

## Tasks

## Description
Currently the num printing alg first calculates the length and then the numbers.
It looks to me, as there could be a approach using [[xor]] to read the numbers 
needed out of the int, instead doing division:

```cpp
int64_t cmp = pow(10, digitCount -1)
int leader = val / cmp;
val -= leader * cmp;
```
This has many operations, we have [[pow]], then a division + mulitplication + 
substraction. It seems like there should be an easier way, using 
[[bit operations]].

At least the loop should be unified, to get the digit count & the digits 
in one loop and not two.

The [[xor]] operation already work for certain numbers, where the bits fall in 
places that the 10er potency doesn't occupy
```cpp
(10) ^ 1010 = 0
(11) ^ 1010 = 1
(14) ^ 1010 = 4
(15) ^ 1010 = 5
```

**Secondary approach**
Maybe this approach, looking at 4 bit at the time and compare them is also 
wrong, maybe i just to see it like pulling the bits through the line one by one
and then sorting them in same way to determine everything.

The problem is actually this: The last bits are not always represeting the last 
numbers

eg `8 + 2 = 10 -> 0`, but they are in different places, or `16 + 4 = 20 -> 0`
or even more `16 + 8 = 24 -> 4` but they are not using the last bits, so we 
can never have a guarantee, where the last numbers are showing up.
So that is why doing it in sets of 4 bits or something is not a valid approach.

On thing though is, tha last bit, is always there for [[uneven]] numbers, 
because 1 is special in that sense

