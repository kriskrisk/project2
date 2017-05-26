/** @file
   Implementacja wczytywania i wypisywania wielomianów.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <malloc.h>
#include <stdio.h>
#include <ctype.h>

#include "poly.h"

/**
 * Pomocnicza funkcja do tworzenia wielomianu na podstawie stringa.
 * @param[in] poly : string zawierający wielomian
 * @param[in] pos: pozycja od której należy zacząć wczytywanie
 * @return wielomian
 */
static Poly ParseOnePoly(char *poly, unsigned *pos);

/**
 * Pomocnicza funkcja do tworzenia jednomianu na podstawie stringa.
 * @param[in] poly : string zawierający jednomian
 * @param[in] pos: pozycja od której należy zacząć wczytywanie
 * @return jednomian
 */
static Mono ParseOneMono(char *poly, unsigned *pos)
{
    Poly poly_coeff;
    *pos = *pos + 1;

    if (poly[*pos] == '(')
    {
        poly_coeff = ParseOnePoly(poly, pos);
    }
    else
    {
        char *next;
        poly_coeff_t coeff = strtol(&poly[*pos], &next, 10);

        while (&poly[*pos] != next)
        {
            *pos = *pos + 1;
        }

        poly_coeff = PolyFromCoeff(coeff);
    }

    *pos = *pos + 1;

    char *next;
    poly_exp_t exp = (poly_exp_t)strtol(&poly[*pos], &next, 10);

    while (&poly[*pos] != next)
    {
        *pos = *pos + 1;
    }

    *pos = *pos + 1;

    return MonoFromPoly(&poly_coeff, exp);
}

static Poly ParseOnePoly(char *poly, unsigned *pos)
{
    if (poly[*pos] != '(')
    {
        char *next;
        Poly PolyCoeff = PolyFromCoeff(strtol(&poly[*pos], &next, 10));

        while (&poly[*pos] != next)
        {
            *pos = *pos + 1;
        }

        return PolyCoeff;
    }

    Mono *monos = (Mono *)malloc(sizeof(Mono));
    unsigned size = 1;
    unsigned first_free = 0;
    bool end_of_list = false;

    while (!end_of_list)
    {
        if (size == first_free)
        {
            size = size * 2;
            monos = realloc(monos, size * sizeof(Mono));
        }

        if (poly[*pos + 1] == '0')
        {
            //przesuwam pos do następnego jednomianu
            *pos = *pos + 3;

            while (isdigit(poly[*pos]))
            {
                *pos = *pos + 1;
            }

            *pos = *pos + 1;
        }
        else if (poly[*pos + 1] == '-' && poly[*pos + 2] == '0')
        {
            //przesuwam pos do następnego jednomianu
            *pos = *pos + 4;

            while (isdigit(poly[*pos]))
            {
                *pos = *pos + 1;
            }

            *pos = *pos + 1;
        }
        else
        {
            monos[first_free] = ParseOneMono(poly, pos);
            first_free++;
        }

        if (poly[*pos] != '+')
        {
            end_of_list = true;
        }
        else
        {
            *pos = *pos + 1;
        }
    }

    monos = realloc(monos, first_free * sizeof(Mono));

    if (first_free == 0)
    {
        return PolyZero();
    }

    Poly result = PolyAddMonos(first_free, monos);
    free(monos);

    return result;
}

Poly ParsePoly(char *poly)
{
    unsigned pos = 0;
    return ParseOnePoly(poly, &pos);
}

char *ReadOneLine()
{
    char *line = (char *)malloc(sizeof(char));
    unsigned first_free = 0;
    unsigned current_size = 1;

    do
    {
        if (first_free == current_size)
        {
            current_size = current_size * 2;
            line = realloc(line, current_size * sizeof(char));//DoubleTheSize(line, current_size);
        }

        int temp = (char)getchar();

        if (temp == EOF)
        {
            free(line);
            return NULL;
        }

        line[first_free] = (char)temp;
        first_free++;
    } while (line[first_free - 1] != '\n');

    line[first_free - 1] = '\0';

    line = realloc(line, first_free);

    return line;
}

void PrintPoly(Poly *p);

/**
 * Wypisuje jednomian na standardowe wyjście.
 * @param[in] p : jednomian
 */
static void PrintMono(Mono *m)
{
    printf("(");
    PrintPoly(&m->p);
    printf(",%d)", m->exp);
}

void PrintPoly(Poly *p)
{
    if (PolyIsCoeff(p))
    {
        printf("%li", p->coeff);
    }
    else
    {
        for (unsigned i = 0; i < p->size; i++)
        {
            PrintMono(&(p->arr[i]));

            if (i != p->size - 1)
            {
                printf("+");
            }
        }
    }
}