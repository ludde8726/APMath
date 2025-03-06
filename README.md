## APMath
APMath is (will be) an arbitary math library written in C.

Why you may ask. Well why not i say. 

Do you even know C? No, no I do not.

This is my attempt to understand this language, please forgive any inefficient code and memory leaks :). 

NOTE: I do not know how to write a Makefile, this one was written by ChatGPT.

### How the precision works.
When creating a APFloat or APComplex the global precision is used to determine the amount of significant digits that will be allocated. The precision or capacity of this "instance" will never change. This means that just because you change the global precision, more memory will **not** be allocated for any of the already created numbers. 

When using any mathematical operation, the precision of the result will be determined by the globally set precision. APMath does create a slightly higher temporary working precision to avoid any rounding errors but will round down the result to the correct precision. Optionally you can use the extended version (with suffix _ex) which allowes an additional parameter specifying the precision.

### How are the ops implemented?