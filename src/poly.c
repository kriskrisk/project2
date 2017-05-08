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

    Mono *summed_list = ListOfMonoAdd(p->list_of_mono, q->list_of_mono);

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

Poly PolyNeg(const Poly *p)
{
    Poly negated_poly = PolyClone(p);

    if (negated_poly.var_idx == NO_VARIABLE)
    {
        negated_poly.coeff = (-1) * negated_poly.coeff;
    }
    else
    {
        List *iterator = negated_poly.list_of_mono;

        while (iterator != NULL)
        {
            iterator->mono.p = PolyNeg(&(iterator->mono.p));
            //mam nadzieję że tu nie tracę wskaźnika
            iterator = iterator->next;
        }
    }

    return negated_poly;
}

Poly PolySub(const Poly *p, const Poly *q)
{
    //a co z alokacją pamięci?
    Poly negated_poly = PolyNeg(q);
    return PolyAdd(p, &(negated_poly));
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    if (PolyIsZero(p))
    {
        return -1;
    }

    if (p->var_idx == NO_VARIABLE)
    {
        return 0;
    }

    if (p->var_idx < var_idx)
    {
        List *iterator = p->list_of_mono;
        poly_exp_t max_exp = 0;

        while (iterator != NULL)
        {
            poly_exp_t current_exp = PolyDegBy(&(iterator->mono.p), var_idx);
            if (current_exp > max_exp)
            {
                max_exp = current_exp;
            }

            iterator = iterator->next;
        }

        return max_exp;
    }

    if (p->var_idx == var_idx)
    {
        return p->list_of_mono->mono.exp;
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsZero(p))
    {
        return -1;
    }

    if (p->var_idx == NO_VARIABLE)
    {
        return 0;
    }

    List *iterator = p->list_of_mono;
    poly_exp_t max_sum_exp = 0;

    while (iterator != NULL)
    {
        poly_exp_t current_sum_exp = PolyDeg(&(iterator->mono.p));

        current_sum_exp += iterator->mono.exp;

        if (current_sum_exp > max_sum_exp)
        {
            max_sum_exp = current_sum_exp;
        }

        iterator = iterator->next;
    }

    return max_sum_exp;
}


/**
 * Sprawdza równość dwóch list.
 * @param[in] p_list : lista jednomianów
 * @param[in] q_list : lista jednomianów
 * @return `p_list = q_list`
 *//*

 odjęcie i przyrównanie do zera


static bool ListIsEq(List *p_list, List *q_list)
{
    if (p_list == NULL || q_list == NULL)
    {
        if (p_list != q_list)
        {
            return false;
        }
    }

    if (p_list->mono.exp != q_list->mono.exp) {
        return false;
    }

    return ListIsEq(p_list->next, q_list->next) && PolyIsEq(&(p_list->mono.p), &(q_list->mono.p));
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    if (PolyIsZero(p) != PolyIsZero(q))
    {
        return false;
    }

    if (p->var_idx == NO_VARIABLE || q->var_idx == NO_VARIABLE)
    {
        if (q->var_idx != p->var_idx)
        {
            return false;
        }

        if (p->coeff != q->coeff)
        {
            return false;
        }
    }

    return ListIsEq(p->list_of_mono, q->list_of_mono);
}

*/
/**
 * Mnoży wielomian przez stałą.
 * @param[in] p : wielomian
 * @param[in] multiplier : mnożnik który jest współczynnikiem wielomianu
 *//*

static void MultiplyPoly(Poly *p, poly_coeff_t multiplier)
{
    if (p->var_idx != NO_VARIABLE)
    {
        p->var_idx--;

        List *iterator = p->list_of_mono;
        while (iterator != NULL)
        {
            MultiplyPoly(&iterator->mono.p, multiplier);
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

    if (p->var_idx == NO_VARIABLE)
    {
        return PolyFromCoeff(p->coeff);
    }

    Poly orygin = PolyClone(p);
    Poly value_at = PolyZero();
    List *iterator = orygin.list_of_mono;
    while (iterator != NULL)
    {
        poly_coeff_t multiplier = (poly_coeff_t)pow(x, iterator->mono.exp);
        MultiplyPoly(&iterator->mono.p, multiplier);

        value_at = PolyAdd(&(value_at), &(iterator->mono.p));

        iterator = iterator->next;
    }

    PolyDestroy(&orygin);

    return value_at;
}
*/
