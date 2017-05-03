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
    return p->var_idx != NO_VARIABLE;
}

/**
 * Usuwa listę z pamięci.
 * @param[in] m : lista.
 */
static void ListDestroy(List *list_of_mono)
{
    if (list_of_mono != NULL)
    {
        ListDestroy(list_of_mono->next);
        MonoDestroy(&(list_of_mono->mono));
        free(list_of_mono);
    }
}

void PolyDestroy(Poly *p)
{
    if (IsNormal(p))
    {
        ListDestroy(p->list_of_mono);
    }

    free(p);
}

/**
 * Robi pełną, głęboką kopię listy.
 * @param[in] list_of_mono : lista.
 * @return skopiowana lista.
 */
static List *ListClone(const List *list_of_mono)
{
    if (list_of_mono != NULL)
    {
        List *new_list_element = (List *)malloc(sizeof(List));
        new_list_element->next = ListClone(list_of_mono->next);
        new_list_element->mono = MonoClone(&(list_of_mono->mono));
        return new_list_element;
    }

    return NULL;
}

Poly PolyClone(const Poly *p)
{
    Poly copy;
    copy.var_idx = p->var_idx;

    if (IsNormal(p))
    {
        copy.list_of_mono = ListClone(p->list_of_mono);
    }
    else
    {
        copy.coeff = p->coeff;
    }

    return copy;
}

/**
 * Przestawia element z jednej listy do drugiej.
 * Usuwa pierwszy element z listy @to_add i wstawia go na koniec drugiej listy.
 * @param[in] end_of_list : lista jednomianów.
 * @param[in] to_add : element który ma być dodany.
 */
static void MoveToList(List **end_of_list, List **to_add)
{
    *end_of_list = *to_add;
    *to_add = (*to_add)->next;
}

/**
 * Zwraca kopię listy jednomianów danego wielomianu.
 * Jeśli wielomian jest stały tworzy jednoelementową
 * listę z jednomianem o stopniu równym zero.
 * @param[in] p : wielomian.
 * @return skopiowana lista jednomianów.
 */
static List *GetListFromPoly(const Poly *p)
{
    if (!IsNormal(p))
    {
        List *p_mono = (List *)malloc(sizeof(List));
        p_mono->next = NULL;
        p_mono->mono = MonoFromPoly(p, 0);

        return p_mono;
    }

    return ListClone(p->list_of_mono);
}

//dodaje dwie listy jednomianów
//obie muszą istnieć oczywiście
static List *ListAdd(List *p_list, List *q_list)
{
    List *sum;
    List **last_element;
    *last_element = sum;

    while (p_list != NULL && q_list != NULL)
    {
        if (p_list->mono.exp > q_list->mono.exp)
        {
            MoveToList(last_element, &p_list);
            last_element = &((*last_element)->next);
        }
        else if (p_list->mono.exp < q_list->mono.exp)
        {
            MoveToList(last_element, &q_list);
            last_element = &((*last_element)->next);
        }
        else
        {
            Poly new_coeff = PolyAdd(&(p_list->mono.p), &(q_list->mono.p));

            if (!PolyIsZero(&new_coeff))
            {
                Mono *new_mono = (Mono *)malloc(sizeof(Mono));
                new_mono->exp = p_list->mono.exp;
                new_mono->p = new_coeff;

                List *new_element = (List *)malloc(sizeof(List));
                new_element->next = NULL;
                new_element->mono = *new_mono;

                MoveToList(last_element, &new_element);
                last_element = &((*last_element)->next);
            }

            List *to_delete = p_list;
            p_list = p_list->next;
            to_delete->next = NULL;
            ListDestroy(to_delete);

            to_delete = p_list;
            p_list = p_list->next;
            to_delete->next = NULL;
            ListDestroy(to_delete);
        }
    }


}

//oba muszą mieć ten sam indeks głównej zmiennej
Poly PolyAdd2(const Poly *p, const Poly *q)
{
    if (!IsNormal(p) && !IsNormal(q))
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    return (Poly) {.var_idx = p->var_idx,
            .list_of_mono = ListAdd(GetListFromPoly(p), GetListFromPoly(q))};
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (!IsNormal(p) && !IsNormal(q))
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }
    else if (IsNormal(p) && !IsNormal(q))
    {
        Poly result = PolyClone(p);

        if (PolyIsZero(q))
        {
            return result;
        }

        List *iterator = result.list_of_mono;
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }

        if (iterator->mono.exp == 0)
        {
            iterator->mono.p = PolyAdd(&(iterator->mono.p), q);
        }
        else
        {
            List *new_element = (List *)malloc(sizeof(List));
            new_element->next = NULL;
            new_element->mono = MonoFromPoly(p, 0);

            iterator->next = new_element;
        }

        return result;
    }
    else if (!IsNormal(p) && IsNormal(q))
    {
        return PolyAdd(q, p);
    }
    else
    {
        List *list_of_mono = (List *)malloc(sizeof(List));

        List *p_iterator = p->list_of_mono;
        List *q_iterator = q->list_of_mono;

        while (p_iterator != 0 && q_iterator != 0)
        {
            if (p_iterator->mono.exp > q_iterator->mono.exp)
            {
                list_of_mono = AddToList(list_of_mono, &(p_iterator->mono));
                p_iterator = p_iterator->next;
            }
            else if (p_iterator->mono.exp < q_iterator->mono.exp)
            {
                list_of_mono = AddToList(list_of_mono, &(q_iterator->mono));
                q_iterator = q_iterator->next;
            }
            else
            {
                Poly new_coeff = PolyAdd(&(p_iterator->mono.p), &(q_iterator->mono.p));

                if (!PolyIsZero(&new_coeff))
                {
                    Mono *new_mono = (Mono *)malloc(sizeof(Mono));
                    new_mono->exp = p_iterator->mono.exp;
                    new_mono->p = new_coeff;

                    List *new_element = (List *)malloc(sizeof(List));
                    new_element->next = NULL;
                    new_element->mono = *new_mono;

                    list_of_mono = AddToList(list_of_mono, new_element);
                }

                p_iterator = p_iterator->next;
                q_iterator = q_iterator->next;
            }
        }

        List *list_of_mono_end = list_of_mono;

        while (list_of_mono_end->next != NULL) {
            list_of_mono_end = list_of_mono_end->next;
        }

        if (p_iterator != NULL)
        {
            list_of_mono_end->next = p_iterator;
        }
        else
        {
            list_of_mono_end->next = q_iterator;
        }

        return (Poly) {.var_idx = 0, .list_of_mono = list_of_mono};
    }
}

//zakładam, że tablica monos[] jest posortowana malejąco
Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    int i;
    if (count == 0)
    {
        return PolyZero();
    }

    Poly new_poly;
    new_poly.var_idx = 0;

    List *first = (List *)malloc(sizeof(List));
    first->mono = monos[0];

    List *previous = first;
    new_poly.list_of_mono = first;

    for (i = 1; i < count; i++)
    {
        List *new_element = (List *)malloc(sizeof(List));
        new_element->mono = monos[i];

        previous->next = new_element;
        previous = new_element;
    }

    previous->next = NULL;

    return new_poly;
}

/*
static Mono MonoMul(const Mono p_mono, const Mono q_mono)
{
    return (Mono) {.exp = p_mono.exp + q_mono.exp,
     .p = PolyMul(&(p_mono.p), &(q_mono.p))};
}

//zakładamy, żę p jest normalny
static Poly MulByCoeff(const Poly *p, const poly_coeff_t coeff)
{
    Poly result = PolyClone(p);
    List *iterator = result.list_of_mono;
    while (iterator != NULL)
    {
        
    }
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (PolyIsZero(p) || PolyIsZero(q))
    {
        return PolyZero();
    }

    if (p->var_idx == NO_VARIABLE && q->var_idx == NO_VARIABLE)
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    if (p->var_idx == NO_VARIABLE)
    {
        return MulByCoeff(q, p->coeff);
    }

    if (q->var_idx == NO_VARIABLE)
    {
        return MulByCoeff(p, q->coeff);
    }

    List *pIterator = p->list_of_mono;
    List *newMonoList = (List *)malloc(sizeof(List));
    while (pIterator != NULL)
    {
        List *qIterator = q->list_of_mono;

        while (qIterator != NULL)
        {
            MonoMul(pIterator->mono, qIterator->mono);
        }
    }
}
 */

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
