/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <math.h>

static boolean IsNormal(const Poly *p)
{
    return p->var_idx != NO_VARIABLE;
}

//komentarz
static void ListDestroy(List *listOfMono)
{
    if (listOfMono != NULL)
    {
        ListDestroy(listOfMono->next);
        MonoDestroy(listOfMono->mono);
        free(listOfMono);
    }
}

void PolyDestroy(Poly *p)
{
    if (IsNormal(p))
    {
        ListDestroy(p->listOfMono);
    }

    free(p);
}

//tu musi wjecha komentarz
static List *ListClone(const List *listOfMono)
{
    if (listOfMono != NULL)
    {
        List *newList = (List *)malloc(sizeof(List));
        newList->next = ListClone(listOfMono->next);
        newList->mono = MonoClone(listOfMono->mono);
        return newList;
    }

    return NULL;
}

Poly PolyClone(const Poly *p)
{
    Poly copy;
    copy.var_idx = p->var_idx;

    if (IsNormal(p))
    {
        copy.listOfMono = ListClone(p->listOfMono);
    }
    else
    {
        copy.coeff = p->coeff;
    }

    return copy;
}

//komntarz: wstawia przed wskazywane miejsce
static List *AddToList(List *orygin, Mono *toAdd)
{
    List *newElement = (List *)malloc(sizeof(List));
    Mono *monoToAddCopy = MonoClone(toAdd);

    newElement->next = orygin;
    newElement->mono = monoToAddCopy;

    return newElement;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (!IsNormal(p) && !IsNormal(q))
    {
          Poly Result;
          Result.var_idx = NO_VARIABLE;
          Result.coeff = p->coeff + q->coeff;

          return Result;
    }
    else if (IsNormal(p) && !IsNormal(q))
    {
        Poly Result = PolyClone(p);

        if (PolyIsZero(q))
        {
            return Result;
        }

        List *iterator = Result->listOfMono;
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }

        if (iterator->mono->exp == 0)
        {
            iterator->mono->p = PolyAdd(iterator->mono->p, q);
        }
        else
        {
            List *newElement = (List *)malloc(sizeof(List));
            newElement->next = NULL;
            newElement->mono = MonoFromPoly(p, 0);

            iterator->next = newElement;
        }

        return Result;
    }
    else if (!IsNormal(p) && IsNormal(q))
    {
        return PolyAdd(q, p);
    }
    else
    {
        List *listOfMono = (List *)malloc(sizeof(List));

        List *pIterator = p->listOfMono;
        List *qIterator = q->listOfMono;

        while (pIterator != 0 && qIterator != 0)
        {
            if (pIterator->mono->exp > qIterator->mono->exp)
            {
                listOfMono = AddToList(listOfMono, pIterator);
                pIterator = pIterator->next;
            }
            else if (pIterator->mono->exp < qIterator->mono->exp)
            {
                listOfMono = AddToList(listOfMono, qIterator);
                qIterator = qIterator->next;
            }
            else
            {
                Poly newCoeff = PolyAdd(pIterator->mono->p, qIterator->mono->p);

                if (!PolyIsZero(newCoeff))
                {
                    Mono *newMono = (Mono *)malloc(sizeof(Mono));
                    newMono->exp = pIterator->mono->exp;
                    newMono->p = newCoeff;

                    List *newElement = (List *)malloc(sizeof(List));
                    newElement->next = NULL;
                    newElement->mono = newMono;

                    listOfMono = AddToList(listOfMono, newElement);
                }

                pIterator = pIterator->next;
                qIterator = qIterator->next;
            }
        }

        List *listOfMonoEnd = listOfMono;

        while (listOfMonoEnd->next != NULL) {
            listOfMonoEnd = listOfMonoEnd->next;
        }

        if (pIterator != NULL)
        {
            listOfMonoEnd->next = pIterator;
        }
        else
        {
            listOfMonoEnd->next = qIterator;
        }

        return (Poly) {.var_idx = 0, .listOfMono = Result};
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

    Poly newPoly;
    newPoly.var_idx = 0;

    List *first = (List *)malloc(sizeof(List));
    first->mono = &monos[0];

    List *previous = first;
    newPoly.listOfMono = first;

    for (i = 1; i < count; i++)
    {
        List *newElement = (List *)malloc(sizeof(List));
        newElement->mono = &monos[i];

        previous->next = newElement;
        previous = newElement;
    }

    prevoius->next = NULL;

    return newPoly;
}

static Mono *MonoMul(const Mono pMono, const Mono qMono)
{
    return (Mono) {.p = pMono.exp + qMono.exp,
     .exp = PolyMul(pMono.p, qMono.q)};
}

//zakładamy, żę p jest normalny
static Poly MulByCoeff(const Poly *p, const poly_coeff_t coeff)
{
    Poly result = PolyClone(p);
    List *iterator = result->listOfMono;
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

    List *pIterator = p->listOfMono;
    List *newMonoList = (List *)malloc(sizeof(List));
    while (pIterator != NULL)
    {
        List *qIterator = q->listOfMono;

        while (qIterator != NULL)
        {
            MonoMul(pIterator->mono, qIterator->mono);
        }
    }
}

Poly PolyNeg(const Poly *p)
{
    Poly negatedPoly = PolyClone(p);

    if (negatedPoly->var_idx == NO_VARIABLE)
    {
        negatedPoly->coeff == (-1) * negatedPoly->coeff;
    }
    else
    {
        List *iterator = negatedPoly.listOfMono;

        while (iterator != NULL)
        {
            iterator->mono->p = PolyNeg(iterator->mono->p);
            //mam nadzieję że tu nie tracę wskaźnika
            iterator = iterator->next;
        }
    }

    return negatedPoly;
}

Poly PolySub(const Poly *p, const Poly *q)
{
    return PolyAdd(p, PolyNeg(q));
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
        List *iterator = p.listOfMono;
        poly_exp_t maxExp = 0;

        while (iterator != NULL)
        {
            poly_exp_t currentExp = PolyDegBy(iterator->mono->p, var_idx);
            if (currentExp > maxExp)
            {
                maxExp = currentExp;
            }

            iterator = iterator->next;
        }

        return maxExp;
    }

    if (p->var_idx == var_idx)
    {
        return p->listOfMono->mono->exp;
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

    List *iterator = p.listOfMono;
    poly_exp_t maxSumExp = 0;

    while (iterator != NULL)
    {
        poly_exp_t currentSumExp = PolyDeg(iterator->mono->p);

        currentSumExp += iterator->mono->exp;

        if (currentSumExp > maxSumExp)
        {
            maxSumExp = currentSumExp;
        }

        iterator = iterator->next;
    }

    return maxSumExp;
}

static bool ListIsEq(List *pList, List *qList)
{
    if (pList == NULL || qList == NULL)
    {
        if (pList != qList)
        {
            return false;
        }
    }

    if (pList->mono.exp != qList->mono.exp) {
        return false;
    }

    return ListIsEq(pList->next, qList->next,) &&
     PolyIsEq(pList->mono.p, qList->mono.p);
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

    return ListIsEq(p->listOfMono, q->listOfMono);
}

static void MultiplyPoly(Poly *p, poly_coeff_t multiplier)
{
    if (p->var_idx != NO_VARIABLE)
    {
        p->var_idx--;

        List *iterator = p->listOfMono;
        while (iterator != NULL)
        {
            MultiplyPoly(&iterator->mono->p, multiplier);
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
        return 0;
    }

    if (p->var_idx == NO_VARIABLE)
    {
        return p->coeff;
    }

    Poly orygin = PolyClone(p);
    Poly valueAt = PolyZero();
    List *iterator = orygin->listOfMono;
    while (iterator != NULL)
    {
        poly_coeff_t multiplier = pow(x, iterator->mono->exp);
        MultiplyPoly(&iterator->mono->p, multiplier);

        valueAt = PolyAdd(valueAt, iterator->mono->p);

        iterator = iterator->next;
    }

    PolyDestroy(orygin);

    return valueAt;
}
