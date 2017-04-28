/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>

static boolean isNormal(const Poly *p)
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
    if (isNormal(p))
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

    if (isNormal(p))
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
static List *addToList(List *orygin, Mono *toAdd)
{
    List *newElement = (List *)malloc(sizeof(List));
    Mono *monoToAddCopy = MonoClone(toAdd);

    newElement->next = orygin;
    newElement->mono = monoToAddCopy;

    return newElement;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    Poly newPoly = PolyClone(p);

    if (!isNormal(p) && !isNormal(q))
    {
        newPoly->coeff += q->coeff;
    }
    else if (isNormal(p) && !isNormal(q))
    {
        List *iterator = newPoly->listOfMono;
        while (iterator->next != NULL)
        {
            iterator = iterator->next;
        }

        if (iterator->mono->exp == 0)
        {
            //na sam dół
            iterator->mono->p = PolyAdd(iterator->mono->p, q);
        }
        else
        {
            //dodajemy nowy człon
            Mono *constMono = (Mono *)malloc(sizeof(Mono));
            constMono->exp = 0;
            constMono->p = PolyClone(q);

            List *newElement = (List *)malloc(sizeof(List));
            newElement->mono = constMono;
            newElement->next = NULL;

            iterator->next = newElement;
        }
    }
    else if (!isNormal(p) && isNormal(q))
    {
        newPoly = PolyAdd(q, p);
    }
    else//oba normalne
    {
        List *currentToAdd = q->listOfMono;
        List *currentOrygin = newPoly->listOfMono;

        while (currentToAdd != NULL && currentOrygin != NULL)
        {
            if (current->mono->exp > currentOrygin->mono->exp)
            {
                newPoly = *addToList(listOfMono, current->mono);
            }
            else if (current->mono->ext < currentOrygin->mono->exp)
            {
                currentOrygin = currentOrygin->next;
            }
            else
            {
                //równe stopnie -> jakiś merge
            }
        }
    }

    return newPoly;
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

Poly PolyMul(const Poly *p, const Poly *q);

//tu chyba jakaś pomocnicza która stały wielomian neguje

Poly PolyNeg(const Poly *p)
{

}
