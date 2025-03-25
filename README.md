# APMath
APMath is (will be) an arbitary math library written in C.

Why you may ask. Well why not i say. 

Do you even know C? No, no I do not.

This is my attempt to understand this language, please forgive any inefficient code and memory leaks :). 

NOTE: I do not know how to write a Makefile, this one was written by ChatGPT.

## How the numbers work
### Storage
Each number type APInt, APFloat, and APComplex is represented using its own structure. However, they all depend on each other.

#### APInt (Arbitrary Precision Integer)

The APInt type serves as the foundation for all other number types. It is defined as follows:
```
typedef struct {
    int8_t sign;
    uint32_t size;
    uint32_t capacity;
    uint8_t *digits;
} APInt;
```
* sign: Determines whether the number is positive (1) or negative (-1).
* size: Represents the number of digits currently stored in the structure.
* capacity: Indicates the total allocated space, i.e., the maximum number of digits that can be stored.
* digits: A pointer to an array containing the digits of the number in base 10, stored in little-endian order (least significant digit first).

**Little-Endian Storage**

Numbers are stored in reverse order to simplify mathematical operations. For example, the number 123 would be stored as:

```
[3, 2, 1]
```
This format makes arithmetic operations like long addition more efficient, as they typically start from the least significant digit (index 0).

#### APFloat (Arbitrary Precision Floating-Point)

The APFloat type extends APInt to support floating-point numbers. It is defined as:
```
typedef struct {
    int8_t sign;
    APInt *significand;
    int64_t exponent;
} APFloat;
```
* sign: Determines the sign of the floating-point number (1 for positive, -1 for negative).
* significand: A pointer to an APInt structure storing all significant digits (non-zero digits).
* exponent: Defines the power of ten by which the significand is scaled, effectively "floating" the decimal point.

**Example: Storing a Floating-Point Number**

Consider the number 0.000000123. The only significant digits are 123, since the leading zeros can be represented using an exponent. Instead of storing the full decimal number, we use:

```
significand = 123  
exponent = -7  
```
Which means that the number is represented as:

$123 \cdot 10^{-7}$

This approach significantly reduces storage requirements by eliminating unnecessary leading or trailing zeros.

#### APComplex (Arbitrary Precision Complex Number)
The APComplex type extends the number system to support complex numbers, which consist of both a real and an imaginary part. It is defined as follows:

```
typedef struct {
    APFloat *real;
    APFloat *imag;
} APComplex;
```

* real: A pointer to an APFloat representing the real part of the complex number.
* imag: A pointer to an APFloat representing the imaginary part of the complex number.

**Understanding Complex Numbers**
A complex number is typically expressed in the form $a+bi$, where:

* a is the real part, and
* b is the imaginary part (multiplied by i, where $i^2 = -1$).

The complex number type has not yet been implemented but will be soon.

### Precision
When creating a APFloat or APComplex the global precision is used to determine the amount of significant digits that will be allocated. The precision or capacity of this "instance" will never change. This means that just because you change the global precision, more memory will **not** be allocated for any of the already created numbers. 

When using any mathematical operation, the precision of the result will be determined by the globally set precision. APMath does create a slightly higher temporary working precision to avoid any rounding errors but will round down the result to the correct precision. Optionally you can use the extended version (with suffix _ex) which allowes an additional parameter specifying the precision.

## How are the ops implemented?
### Integer ops
Most of the integer operations are implemented using the methods taught in school to perform calculations on paper, however some need a more creative approach.
### Addition
The addition function follows the long addition method commonly taught in school. When computing addition it is important that we first check the sign of each operand and if necessary dispatch to another operation. For example $-x + y = y - x$ and $x + -y = x - y$.

**Algorithm Explanation**

The algorithm follows these key steps:

1. Digit-by-Digit Addition
    * Start at the least significant digit (index 0).
    * Add corresponding digits from x and y along with any carry from the previous step.
    * Store the result for each digit.
    * Compute the sum for the current digit, $s=x_i+y_i+c$ where c is the carry from the previous step.

2. Carry Propagation
    * Extract the final digit to store, $digit_i = s \mod 10$.
    * If $s \ge 10$, we handle carrying to the next step by setting $c = 1$.
    * Repeat this process for all digits.

3. Handling Final Carry
    * After processing all digits, if there is a remaining carry, append it as a new most significant digit.

### Subtraction
The subtraction function is implemented in a similar way to the addition function using the long subtraction method. When computing subtraction ee assume that $x \ge y$ to avoid dealing with negative results. If $x < y$, subtraction can be handled separately by negating the final result. Similarily to addition we always check the sign of the operands before doing the computation since we can use some arithmetic rules to generalize the algorithm. When the sign of the operands are not equal we negate the second parameter and perform an addition instead.

**Algorithm Explanation**
1. Digit-by-Digit Subtraction
    * Start at the least significant digit (index 0).
    * Subtract the corresponding digits $d = x_i - y_i - b$ where b is the borrow from the previous step.
2. Handling Borrowing
    * If $d \lt 0$, we need to handle borrowing from the next digit.
    * If borrow is necessary we set $digit_i = d + 10$ and $b = 1$. Since we always assume $x \ge y$ we will never have any borrow left once the subtraction is done.

### Multiplication
The multiplication function is implemented using the long multiplication method. In this function we don't have to worry about the signs of the operands leading to different functionality since the resulting sign is $1$ if operands sign are the same or $-1$ if they are different.

**Algorithm Explanation**
1. Digit-by-Digit Multiplication
    * Start at the least significant digit (index 0) of x and iterate through all digits of y.
    * Compute the intermediate product $p_{i+j} = x_i \cdot y_j + c$ where c is the carry from the previous step. 

2. Handling Carry Propagation
    * Extract the final digit to store, $digit_{i+j} = p \mod 10$.
    * If $p_{i+j} \ge 10$, compute the new carry for the next iteration, $c = \lfloor p/10 \rfloor$
    * Continue iterating for all digits of x and y or while we have carry.