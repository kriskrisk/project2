/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <stdlib.h>
#include <mem.h>
#include <assert.h>
#include <limits.h>

#include "poly.h"

/**
 * Porównuje wykładniki dwóch jednomianów.
 * Zwraca liczbę większą od zera gdy pierwszy jest większy,
 * zero gdy są równe i liczbę ujemną, gdy drugi jest większy.
 * @param[in] a : jednomian
 * @param[in] x : jednomian
 * @return wynik porównania
 */
static int CompareMono(const void * a, const void * b){
    return (((Mono *)a)->exp - ((Mono*)b)->exp);
}

void PolyDestroy(Poly *p)
{
    if (!PolyIsCoeff(p))
    {
        for (unsigned i = 0; i < p->size; i++)
        {
            MonoDestroy(&p->mono_arr[i]);
        }

        free(p->mono_arr);
        p->mono_arr = NULL;
    }
}

static void MonoArrClone(Mono *copy, const Mono *orygin, unsigned size)
{
    for (unsigned i = 0; i < size; i++)
    {
        copy[i] = MonoClone(&orygin[i]);
    }
}

static Mono *AllocMemForMonos(unsigned size)
{
    Mono *new_arr = (Mono *)calloc(size, sizeof(Mono));
    return new_arr;
}

Poly PolyClone(const Poly *p)
{
    if (PolyIsCoeff(p))
    {
        return *p;
    }

    Mono *arr_copy = AllocMemForMonos(p->size);
    MonoArrClone(arr_copy, p->mono_arr, p->size);

    return (Poly) {.mono_arr = arr_copy, .size = p->size};
}

/**
 * Dodaje wielomian i współczynnik.
 * Wielomian nie musi być normalny.
 * @param[in] p : wielomian
 * @param[in] c : współczynnik
 * @return wysumowany wielomian
 */
static Poly PolyAddCoeff(const Poly *p, poly_coeff_t c)
{
    if (PolyIsCoeff(p))
    {
        return PolyFromCoeff(p->coeff + c);
    }
    else if (p->mono_arr[0].exp == 0)
    {
        Poly new_poly = PolyAddCoeff(&p->mono_arr[0].p, c);

        if (PolyIsZero(&new_poly))
        {
            Mono *new_arr = AllocMemForMonos(p->size - 1);
            for (unsigned i = 0; i < p->size - 1; i++)
            {
                new_arr[i] = MonoClone(&p->mono_arr[i +1 ]);
            }

            new_poly.size = p->size - 1;
            new_poly.mono_arr = new_arr;
            return (Poly) {.size = p->size - 1, .mono_arr = new_arr};
        }
        else if (PolyIsCoeff(&new_poly) && p->size == 1)
        {
            return new_poly;
        }
        else
        {
            Mono *result_arr = AllocMemForMonos(p->size);
            result_arr[0] = MonoFromPoly(&new_poly, 0);
            for (unsigned i = 1; i < p->size; i++)
            {
                result_arr[i] = MonoClone(&p->mono_arr[i]);
            }

            return (Poly) {.size = p->size, .mono_arr = result_arr};
        }
    }
    else
    {
        Mono *result_arr = AllocMemForMonos(p->size + 1);
        Poly new_poly = PolyFromCoeff(c);
        result_arr[0] = MonoFromPoly(&new_poly, 0);
        for (unsigned i = 1; i < p->size + 1; i++)
        {
            result_arr[i] = MonoClone(&p->mono_arr[i - 1]);
        }

        return (Poly) {.size = p->size + 1, .mono_arr = result_arr};
    }
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (PolyIsZero(p))
    {
        return PolyClone(q);
    }

    if (PolyIsZero(q))
    {
        return PolyClone(p);
    }

    if (PolyIsCoeff(p) && PolyIsCoeff(q))
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    if (PolyIsCoeff(p))
    {
        return PolyAddCoeff(q, p->coeff);
    }

    if (PolyIsCoeff(q))
    {
        return PolyAddCoeff(p, q->coeff);
    }

    //tu może nie wydolić pamięciowo
    unsigned size = p->size + q->size;
    Mono *new_monos = AllocMemForMonos(size);

    for (unsigned i = 0; i < p->size; i++)
    {
        new_monos[i] = MonoClone(&p->mono_arr[i]);
    }

    for (unsigned i = p->size; i < size; i++)
    {
        new_monos[i] = MonoClone(&q->mono_arr[i - p->size]);
    }

    Poly result = PolyAddMonos(size, new_monos);
    free(new_monos);

    return result;
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    Mono *result_arr = AllocMemForMonos(count);
    memcpy(result_arr, monos, count * sizeof(Mono));
    qsort(result_arr, count, sizeof(Mono), CompareMono);
    unsigned int j = 0;

    for (unsigned i = 0; i < count; i++)
    {
        if (j == 0)
        {
            result_arr[j] = result_arr[i];
            j++;
        }
        else if (result_arr[i].exp == result_arr[j - 1].exp)
        {
            Poly sum_coeff = PolyAdd(&result_arr[i].p, &result_arr[j - 1].p);
            Mono sum = MonoFromPoly(&sum_coeff, result_arr[i].exp);

            PolyDestroy(&result_arr[i].p);
            PolyDestroy(&result_arr[j].p);

            if (PolyIsZero(&sum_coeff))
            {
                j--;
            }
            else
            {
                result_arr[j - 1] = sum;
            }
        }
        else
        {
            result_arr[j] = result_arr[i];
            j++;
        }
    }

    Poly p = (Poly) {.size = j, .mono_arr = result_arr};

    if (j == 0)
    {
        PolyDestroy(&p);
        p.coeff = 0;
    }
    else if (j == 1 && result_arr[0].exp == 0 && PolyIsCoeff(&result_arr[0].p))
    {
        p.coeff = result_arr[0].p.coeff;
        free(result_arr);
        p.mono_arr = NULL;
    }
    else
    {
        p.mono_arr = realloc(p.mono_arr, p.size * sizeof(Mono));
    }

    return p;
}

/**
 * Mnoży wielomnian przez stałą.
 * @param[in] p : wielomian.
 * @param[in] multiplier : stała będąca współczynnikiem wielomianu.
 * @return wynik mnożenia
 */
static Poly MulByCoeff(const Poly *p, const poly_coeff_t multiplier)
{
    if (PolyIsCoeff(p))
    {
        return PolyFromCoeff(p->coeff * multiplier);
    }
    else
    {
        Mono *new_arr = AllocMemForMonos(p->size);

        for (unsigned i = 0; i < p->size; i++)
        {
            Poly new_coeff = MulByCoeff(&p->mono_arr[i].p, multiplier);

            Mono new = (Mono) {.p = new_coeff, .exp = p->mono_arr[i].exp};
            new_arr[i] = new;
        }

        //PolyRealloc(&r);

        return (Poly) {.size = p->size, .mono_arr = new_arr};
    }
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (PolyIsZero(p) || PolyIsZero(q))
    {
        return PolyZero();
    }

    if (PolyIsCoeff(p))
    {
        return MulByCoeff(q, p->coeff);
    }

    if (PolyIsCoeff(q))
    {
        return MulByCoeff(p, q->coeff);
    }

    unsigned size = p->size * q->size;
    Mono *new_arr = AllocMemForMonos(size);
    unsigned curr_pos = 0;

    for (unsigned i = 0; i < p->size; i++)
    {
        for (unsigned j = 0; j < q->size; j++)
        {
            Mono new_mono;
            new_mono.p = PolyMul(&p->mono_arr[i].p, &q->mono_arr[j].p);
            new_mono.exp = p->mono_arr[i].exp + q->mono_arr[j].exp;

            new_arr[curr_pos] = new_mono;

            curr_pos++;
        }
    }

    Poly result = PolyAddMonos(size, new_arr);
    free(new_arr);

    return result;
}

Poly PolyNeg(const Poly *p)
{
    return MulByCoeff(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly q_negated = PolyNeg(q);
    Poly result = PolyAdd(p, &q_negated);
    PolyDestroy(&q_negated);

    return result;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    if (PolyIsZero(p))
    {
        return -1;
    }
    else if (PolyIsCoeff(p))
    {
        return 0;
    }
    else if(var_idx == 0)
    {
        return p->mono_arr[p->size - 1].exp;
    }
    else
    {
        poly_exp_t max = -1;

        for (unsigned i = 0; i < p->size; i++)
        {
            poly_exp_t curr = PolyDegBy(&p->mono_arr[i].p, var_idx - 1);

            if (curr > max)
            {
                max = curr;
            }
        }

        return max;
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsZero(p))
    {
        return -1;
    }
    else if (PolyIsCoeff(p))
    {
        return 0;
    }
    else
    {
        poly_exp_t max = -1;

        for (unsigned i = 0; i < p->size; i++)
        {
            poly_exp_t curr = PolyDeg(&p->mono_arr[i].p) + p->mono_arr[i].exp;

            if (curr > max)
            {
                max = curr;
            }
        }

        return max;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p))
    {
        if (PolyIsCoeff(q))
        {
            return p->coeff == q->coeff;
        }

        return false;
    }
    else if (PolyIsCoeff(q))
    {
        return false;
    }
    else if (p->size != q->size)
    {
        return false;
    }
    else
    {
        for (unsigned i = 0; i < p->size; i++)
        {
            if (p->mono_arr[i].exp != q->mono_arr[i].exp || !PolyIsEq(&p->mono_arr[i].p, &q->mono_arr[i].p))
            {
                return false;
            }
        }

        return true;
    }
}

bool PolyIsEq3(const Poly *p, const Poly *q)
{
    Poly subtraction = PolySub(p, q);

    bool isEq = PolyIsZero(&subtraction);
    PolyDestroy(&subtraction);

    return isEq;
}

/**
 * Zwraca wynik potęgowania.
 * @param[in] base : baza potęgowania
 * @param[in] exp : wykładnik potęgowania
 * @return wynik potęgowania
 */
static long Power(long base, long exp)
{
    long result = 1;
    while (exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }

    return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    if (PolyIsCoeff(p))
    {
        return PolyClone(p);
    }

    Poly result = PolyZero();

    for (unsigned i = 0; i < p->size; i++)
    {
        poly_coeff_t multiplier = Power(x, p->mono_arr[i].exp);
        Poly temp = MulByCoeff(&p->mono_arr[i].p, multiplier);
        Poly sum = PolyAdd(&result, &temp);

        PolyDestroy(&temp);
        PolyDestroy(&result);
        result = sum;
    }

    return result;
}

Poly PolyAt3(const Poly *p, poly_coeff_t x)
{
    if (PolyIsCoeff(p))
    {
        return PolyClone(p);
    }

    Poly result = PolyZero();
    poly_exp_t prev_exp = 0;
    poly_coeff_t multiplier = 1;

    for (unsigned i = 0; i < p->size; i++)
    {
        multiplier *= Power(x, p->mono_arr[i].exp - prev_exp);
        prev_exp = p->mono_arr[i].exp;
        Poly temp = MulByCoeff(&p->mono_arr[i].p, multiplier);
        Poly sum = PolyAdd(&result, &temp);

        PolyDestroy(&temp);
        PolyDestroy(&result);
        result = sum;
    }

    return result;
}
