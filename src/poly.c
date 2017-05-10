/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include "poly.h"

/**
 * Sprawdza, czy wielomian jest normalny.
 * @param[in] p : wielomian
 * @return Czy wielomian jest normalny?
 */
static inline bool IsNormal(const Poly *p)
{
    return p->is_normal;
}

/**
 * Usuwa listę jednomianów z pamięci.
 * @param[in] m : lista.
 */
static void ListOfMonoDestroy(Mono *list_of_mono)
{
    if (list_of_mono != NULL)
    {
        ListOfMonoDestroy(list_of_mono->next);
        MonoDestroy(list_of_mono);
        free(list_of_mono);
    }
}

void PolyDestroy(Poly *p)
{
    if (IsNormal(p))
    {
        ListOfMonoDestroy(p->list_of_mono);
    }

    free(p);
}

/**
 * Robi pełną, głęboką kopię listy.
 * @param[in] list_of_mono : lista.
 * @return skopiowana lista.
 */
static Mono *ListOfMonoClone(const Mono *list_of_mono)
{
    if (list_of_mono != NULL)
    {
        Mono *new_list_element = (Mono *)malloc(sizeof(Mono));
        *new_list_element = MonoClone(list_of_mono);
        new_list_element->next = ListOfMonoClone(list_of_mono->next);
        return new_list_element;
    }

    return NULL;
}

Poly PolyClone(const Poly *p)
{
    Poly copy;
    copy.is_normal = p->is_normal;

    if (IsNormal(p))
    {
        copy.list_of_mono = ListOfMonoClone(p->list_of_mono);
    }
    else
    {
        copy.coeff = p->coeff;
    }

    return copy;
}

static void AddNewElementToList(const Mono *mono, Mono **where_to_add)
{
    Mono *new_element = (Mono *)malloc(sizeof(Mono));
    *new_element = MonoClone(mono);

    *where_to_add = new_element;
}

static Mono *ListOfMonoAdd(Mono *p_list, Mono *q_list)
{
    Mono *sum = NULL;
    Mono **last_element = &sum;
    Mono *first_in_p = p_list;
    Mono *first_in_q = q_list;

    while (first_in_p != NULL && first_in_q != NULL)
    {
        if (first_in_p->exp > first_in_q->exp)
        {
            AddNewElementToList(first_in_p, last_element);
            last_element = &((*last_element)->next);
            first_in_p = first_in_p->next;
        }
        else if (first_in_p->exp < first_in_q->exp)
        {
            AddNewElementToList(first_in_q, last_element);
            last_element = &((*last_element)->next);
            first_in_q = first_in_q->next;
        }
        else
        {
            Poly *new_coeff = (Poly *)malloc(sizeof(Poly));
            *new_coeff = PolyAdd(&(first_in_p->p), &(first_in_q->p));

            if (!PolyIsZero(new_coeff))
            {
                Mono *new_element = (Mono *)malloc(sizeof(Mono));
                *new_element = MonoFromPoly(new_coeff, first_in_p->exp);

                *last_element = new_element;
                last_element = &((*last_element)->next);
            }
            else
            {
                PolyDestroy(new_coeff);
            }

            first_in_p = first_in_p->next;
            first_in_q = first_in_q->next;
        }
    }

    while (first_in_p != NULL)
    {
        AddNewElementToList(first_in_p, last_element);
        last_element = &((*last_element)->next);
        first_in_p = first_in_p->next;
    }

    while (first_in_q != NULL)
    {
        AddNewElementToList(first_in_q, last_element);
        last_element = &((*last_element)->next);
        first_in_q = first_in_q->next;
    }

    *last_element = NULL;

    return sum;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (!IsNormal(p) && !IsNormal(q))
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    Mono *summed_list = ListOfMonoAdd(p->is_normal? p->list_of_mono : NULL,
                                      q->is_normal? q->list_of_mono : NULL);

    if (summed_list == NULL)
    {
        return PolyZero();
    }

    return (Poly) {.is_normal = true, .list_of_mono = summed_list};
}

Mono *MakeListFromArray(unsigned count, const Mono monos[])
{
    bool used[count];
    for (int i = 0; i < count; i++)
    {
        used[i] = false;
    }

    Mono *list;
    Mono **last = &list;
    int max_position;
    int max_exp;

    for (int i = 0; i < count; i ++)
    {
        max_exp = -1;
        max_position = -1;

        for (int j = 0; j < count; j++)
        {
            if (monos[j].exp > max_exp && !used[j])
            {
                max_exp = monos[j].exp;
                max_position = j;
            }
        }

        used[max_position] = true;
        Mono *new_element = (Mono *)malloc(sizeof(Mono));
        *new_element = MonoClone(&monos[max_position]);
        *last = new_element;
        last = &(new_element->next);
    }

    *last = NULL;
    return list;
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    if (count == 0)
    {
        return PolyZero();
    }

    Poly sum;
    sum.is_normal = true;
    sum.list_of_mono = MakeListFromArray(count, monos);

    return sum;
}

/**
 * Mnoży wielomnian przez stałą.
 * @param[in] p : wielomian.
 * @param[in] multiplier : stała będąca współczynnikiem wielomianu.
 */
static void MulByCoeff(Poly *p, const poly_coeff_t multiplier)
{
    if (!p->is_normal)
    {
        p->coeff = p->coeff * multiplier;
    }
    else
    {

        Mono *iterator = p->list_of_mono;
        while (iterator != NULL)
        {
            MulByCoeff(&(iterator->p), multiplier);
        }
    }
}

static void PlaceInList(Mono **list, Mono *new_element)
{
    if (*list == NULL)
    {
        *list = new_element;
    }
    else
    {
        if (new_element->exp > (*list)->exp)
        {
            new_element->next = *list;
            *list = new_element;
        }
        else
        {
            if ((*list)->next == NULL)
            {
                (*list)->next = new_element;
            }
            else
            {
                PlaceInList(&(*list)->next, new_element);
            }
        }
    }
}

static Mono *Merge(Mono *first, Mono *secound)
{
    Mono *merged = (Mono *)malloc(sizeof(Mono));
    Poly *p = (Poly *)malloc(sizeof(Poly));
    *p = PolyAdd(&first->p, &secound->p);
    *merged = MonoFromPoly(p, first->exp);

    return merged;
}

static void MergeSameExp(Mono **list)
{
    if (*list != NULL && (*list)->next != NULL)
    {
        if ((*list)->exp == (*list)->next->exp)
        {
            Mono *temp = Merge(*list, (*list)->next);
            temp->next = (*list)->next->next;

            PolyDestroy(&(*list)->p);
            PolyDestroy(&(*list)->next->p);

            free((*list)->next);
            free(*list);

            *list = temp;
        }
        else
        {
            MergeSameExp(&(*list)->next);
        }
    }
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (PolyIsZero(p) || PolyIsZero(q))
    {
        return PolyZero();
    }

    if (!p->is_normal && !q->is_normal)
    {
        return PolyFromCoeff(p->coeff * q->coeff);
    }

    if (!p->is_normal)
    {
        Poly new_q = PolyClone(q);
        MulByCoeff(&new_q, p->coeff);
        return new_q;
    }

    if (!q->is_normal)
    {
        Poly new_p = PolyClone(p);
        MulByCoeff(&new_p, q->coeff);
        return new_p;
    }

    Poly result;
    result.is_normal = true;

    Mono *p_iterator = p->list_of_mono;
    Mono *q_iterator = q->list_of_mono;

    Mono **result_list;
    result_list = &result.list_of_mono;

    while (p_iterator != NULL)
    {
        while (q_iterator != NULL)
        {
            Mono *new_element = (Mono *)malloc(sizeof(Mono));
            Poly *mul_coeff = (Poly *)malloc(sizeof(Poly));
            *mul_coeff = PolyMul(&p_iterator->p, &q_iterator->p);
            *new_element = MonoFromPoly(mul_coeff,
                                        p_iterator->exp + q_iterator->exp);

            PlaceInList(result_list, new_element);
        }
    }

    MergeSameExp(result_list);

    return result;
}

static void AllCoeffNeg(Poly *p)
{
    if (!p->is_normal)
    {
        p->coeff = p->coeff * (-1);
    }
    else
    {
        Mono *iterator = p->list_of_mono;

        while (iterator != NULL)
        {
            AllCoeffNeg(&iterator->p);
            iterator = iterator->next;
        }
    }
}

Poly PolyNeg(const Poly *p) {
    Poly negated_poly = PolyClone(p);
    AllCoeffNeg(&negated_poly);

    return negated_poly;
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly *q_negated = (Poly *)malloc(sizeof(Poly));
    *q_negated = PolyNeg(q);

    return PolyAdd(p, q_negated);
}

static poly_coeff_t PolyDegByWithIdx(const Poly *p, unsigned var_idx, unsigned curr_idx)
{
    if (!p->is_normal)
    {
        return 0;
    }

    if (curr_idx < var_idx)
    {
        poly_coeff_t max_deg = 0;
        Mono *iterator = p->list_of_mono;

        while (iterator != NULL)
        {
            poly_coeff_t curr_deg = PolyDegByWithIdx(&iterator->p, var_idx, curr_idx + 1);

            if (curr_deg > max_deg)
            {
                max_deg = curr_deg;
            }

            iterator = iterator->next;
        }

        return max_deg;
    }

    return p->list_of_mono->exp;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    if (PolyIsZero(p))
    {
        return -1;
    }

    return PolyDegByWithIdx(p, var_idx, 0);
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsZero(p))
    {
        return -1;
    }

    if (!p->is_normal)
    {
        return 0;
    }

    Mono *iterator = p->list_of_mono;
    poly_exp_t max_sum_exp = 0;

    while (iterator != NULL)
    {
        poly_exp_t current_sum_exp = PolyDeg(&(iterator->p));
        current_sum_exp += iterator->exp;

        if (current_sum_exp > max_sum_exp)
        {
            max_sum_exp = current_sum_exp;
        }

        iterator = iterator->next;
    }

    return max_sum_exp;
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    Poly *subtraction = (Poly *)malloc(sizeof(Poly));
    *subtraction = PolySub(p, q);

    bool isEq = PolyIsZero(subtraction);
    PolyDestroy(subtraction);

    return isEq;
}


/**
 * Mnoży wielomian przez stałą.
 * Przejmuje na własność wielomian p.
 * @param[in] p : wielomian
 * @param[in] multiplier : mnożnik który jest współczynnikiem wielomianu
 */
static void MultiplyPoly(Poly *p, poly_coeff_t multiplier)
{
    if (p->is_normal)
    {
        Mono *iterator = p->list_of_mono;
        while (iterator != NULL)
        {
            MultiplyPoly(&iterator->p, multiplier);
            iterator = iterator->next;
        }
    }
    else
    {
        p->coeff = p->coeff * multiplier;
    }
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    if (PolyIsZero(p))
    {
        return PolyZero();
    }

    if (!p->is_normal)
    {
        return PolyFromCoeff(p->coeff);
    }

    Poly value_at = PolyZero();
    Mono *iterator =p->list_of_mono;

    while (iterator != NULL)
    {
        Poly *poly = (Poly *)malloc(sizeof(Poly));
        *poly = PolyClone(&iterator->p);

        poly_coeff_t multiplier = (poly_coeff_t)pow(x, iterator->exp);
        MultiplyPoly(poly, multiplier);

        Poly sum = PolyAdd(&(value_at), poly);

        PolyDestroy(poly);
        PolyDestroy(&value_at);

        value_at = sum;
        iterator = iterator->next;
    }

    return value_at;
}
