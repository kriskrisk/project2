#include <stdio.h>
#include "poly.h"

static void PrintMono(Mono mono_to_print);

static void PrintPoly(Poly poly_to_print)
{
    printf("Poly:\n");

    if (poly_to_print.var_idx == NO_VARIABLE || PolyIsZero(&poly_to_print))
    {
        printf("%d\n", (int)poly_to_print.coeff);
        printf("%p\n", poly_to_print.list_of_mono);
        printf("%d\n", poly_to_print.var_idx);
    }
    else
    {
        List *iterator = poly_to_print.list_of_mono;
        printf("%d\n", poly_to_print.var_idx);
        while (iterator != NULL)
        {
            PrintMono(iterator->mono);
            iterator = iterator->next;
        }
    }


}

static void PrintMono(Mono mono_to_print)
{
    printf("Mono:\n");
    printf("exp: %d\n", mono_to_print.exp);
    PrintPoly(mono_to_print.p);
}

int main()
{
    Poly test = PolyFromCoeff(3);
    Mono mono_test = MonoFromPoly(&test, 2);
    List *list = (List *)malloc(sizeof(List));
    list->next = NULL;
    list->mono = mono_test;
    Poly big;
    big.var_idx = 0;
    big.list_of_mono = list;

    Poly big2 = PolyClone(&big);
    PrintPoly(big2);
}
