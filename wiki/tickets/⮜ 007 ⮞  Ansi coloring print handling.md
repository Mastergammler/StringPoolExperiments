---
created: 2026-09-29T10:13
finished: 
---
#ticket/open

## Tasks
- [ ] Check if approach could work -> can i just pass it through?
    -> How to hanlde str len with it?
- [ ] Create a style property for it (foreground / background etc)?

## Description
It would be nice to have the ability to print in ansi colors via settings.
This would only be relevant for the print buffer then etc?


**Idea**
Have 2 kind of input options:
- `printc("...",color,args...);` - here it colors the whole line
- `printc("...",fmt_s(str,.color = ..),fmt_n(num,.color = ..)`

=> This then just handles the color printing, for either the wohle line or 
   for the single expanded arg

*!!! This then would change the length of required string 
     if i write it in the string directly !!!*

### Questions
- Would i only do this for the print buffer, or also for the format?
    -> Should it be part of the string, or just written?

