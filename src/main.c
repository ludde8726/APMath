#include <stdio.h>

#include "APInt.h"
#include "APFloat.h"
#include "APComplex.h"
#include "APAdd.h"
#include "APMul.h"
#include "Print.h"

void wprint_apint(APInt *num) {
    size_t i;
    for (i=0; i<num->capacity; i++) printf("%d ", num->digits[i]);
}

void pprint_apint(APInt *num) {
    for (size_t i = num->size; i>0; i--) {
        printf("%d", num->digits[i-1]);
    }
    printf("\n");
}

int main() {
    APInt* bigint;
    bigint = init_apint(10);
    if (bigint == NULL) perror("Could not create apint");
    wprint_apint(bigint);
    printf("\n");
    free_apint(bigint);

    APFloat* apfloat;
    apfloat = init_apfloat(10);
    if (apfloat == NULL) perror("Could not create float");
    printf("exponent: %lld \n", apfloat->exponent);
    wprint_apint(apfloat->significand);
    printf("\n");
    free_apfloat(apfloat);

    APComplex* apcomplex;
    apcomplex = init_apcomplex(10);
    if (apcomplex == NULL) perror("Could not create apcomplex");
    printf("real: ");
    wprint_apint(apcomplex->real->significand);
    printf("\n");
    printf("imag: ");
    wprint_apint(apcomplex->imag->significand);
    printf("\n");
    free_apcomplex(apcomplex);

    APInt* bigint1;
    APInt* bigint2;
    bigint1 = init_apint(2);
    bigint2 = init_apint(2);
    if (bigint1 == NULL) perror("Could not create apint");
    if (bigint2 == NULL) perror("Could not create apint");
    bigint1->digits[0] = 1;
    bigint1->digits[1] = 2; // 21
    bigint1->sign = 1;
    bigint1->size = 2;
    bigint2->digits[0] = 9;
    bigint2->digits[1] = 4; // 49
    bigint2->sign = 1;
    bigint2->size = 2;
    // -21 + 49 = 49 - 21 = 28
    APInt *res = apint_sub(bigint1, bigint2);
    print_apint(res);
    free_apint(res);

    APFloat* bigfloat1;
    APFloat* bigfloat2;
    bigfloat1 = init_apfloat(4);
    bigfloat2 = init_apfloat(4);
    if (bigfloat1 == NULL) perror("Could not create apint");
    if (bigfloat2 == NULL) perror("Could not create apint");

    bigfloat1->significand = bigint1;
    bigfloat1->significand->size = 2;
    bigfloat1->exponent = 100;

    bigfloat2->significand = bigint2;
    bigfloat2->significand->size = 2;
    bigfloat2->exponent = 100;

    APFloat *res2 = apfloat_add(bigfloat1, bigfloat2);
    pprint_apint(res2->significand);
    printf("%s\n\n", string_apfloat(res2));
    printf("EXPONENT: %lld\n", res2->exponent);

    APComplex *abc = init_apcomplex(4);
    abc->real = bigfloat1;
    abc->imag = bigfloat2;
    printf("%s\n\n", string_apcomplex(abc));


    // free_apint(bigint1);
    // free_apint(bigint2);
    free_apfloat(bigfloat1);
    free_apfloat(bigfloat2);
    free_apfloat(res2);

    APInt* bigint3;
    APInt* bigint4;
    bigint3 = init_apint(2);
    bigint4 = init_apint(2);
    if (bigint3 == NULL) perror("Could not create apint");
    if (bigint4 == NULL) perror("Could not create apint");
    bigint3->digits[0] = 1;
    bigint3->digits[1] = 2; // 21
    bigint3->sign = 1;
    bigint3->size = 2;
    bigint4->digits[0] = 9;
    bigint4->digits[1] = 4; // 49
    bigint4->size = 2;
    APInt *res4 = apint_mul(bigint3, bigint4);
    // printf("%d", res4->size);
    print_apint(bigint3);
    print_apint(bigint4);
    print_apint(res4);
    free_apint(bigint3);
    free_apint(bigint4);
    // free_apint(res4);


    APInt *lint = apint_from_string("-00123456789");
    APInt *lint2 = apint_from_string("123456789");
    APInt *lres = apint_mul(lint, lint2);
    print_apint(lint);
    print_apint(lint2);
    print_apint(lres);
    free_apint(lint);
    free_apint(lint2);
}
// 110 + 1,10 -> 110 + 110 * 10^-2
//               11000 * 10^-2 + 110 * 10^-2 
//               10^-2 * (11000 + 110)

// 110 + 1,10 -> 110 + 110 * 10^-2
//               110 + 00110
//               110
//              +00110
//               111,10