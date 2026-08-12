> Tried a approach to get the current 10er number with xor
> It works every 2 bits, but for the other cases it doesn't
> But there seems to be some behaviour between the two
> -> Maybe there is a way to handle this this way

*The problem*
I need the last digit, per 10er potency, in order to read it
So for 125, i need 5 (1), 2 (10), 1 (100).

num / factor -> and then the last one, kind of

## Description
Instead of printing into a transient pool, have the string be expanded 
on demand -> so just calculate the length in the moment.

Maybe store the values in int or something, so i don't have to compute twice?
-> But then i could almost already just store the string on the object
=> And then i copy it over again, so maybe just calculate length is sufficient?

**Idea**
I could store the numbers in 5bits each and just decode that later
That would be 100 bits -> but 12 * 8 = 96, should be sufficient
-> Because [[u64]] max, is 1844 -> so just a one, for the first bit

-> I could then just shift the value, to the ascii for the correct number,
during the print operation
=> Tbh this sounds pretty sexy, because it just happens to fit in 12 byte

*ACTUALLY* digits 0-9 fit in 4 bits -> that means we only need 20 * 4 = 
80 bit = 10 byte 

Data would look like this:

```cpp
struct NumPrint
{
    byte digits[10];
    u8 digit_count;
    bool negative;
    int pad_left_count;
    int pad_right_count;
    char pad_char;

    int str_len;
}
```
**Architecture**
I need 2 functions: 
1. To convert the number into the NumPrint
2. To convert the num print into string (into the correct buffer)

**FMT Float**
```cpp
struct FloatPrint
{
    byte digits[10];
    u8 digit_count;
    // just save the comma pos, and the rest as a single value?
    // -> does this work for the right amount of precision?
    u8 comma_pos;
}
```
- Question is, how do i actually handle rounding etc?

