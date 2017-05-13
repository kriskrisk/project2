/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <stdlib.h>

#include "poly.h"

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

/**
 * Zwraca długość listy.
 * @param[in] list : lista jednomianów.
 * @return ilość elementów w liście.
 */
static  unsigned int ListLen(Mono *list)
{
    unsigned int list_len = 0;
    Mono *iterator = list;

    while (iterator != NULL)
    {
        list_len++;
        iterator = iterator->next;
    }

    return list_len;
}

void PolyDestroy(Poly *p)
{
    if (p->is_normal)
    {
        ListOfMonoDestroy(p->list_of_mono);
    }
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

    if (p->is_normal)
    {
        copy.list_of_mono = ListOfMonoClone(p->list_of_mono);
    }
    else
    {
        copy.coeff = p->coeff;
    }

    return copy;
}

/**
 * Wstawia nowy element do listy na wskazane miejsce.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p mono i
 * wskaźnik (oraz strukturę na którą wskazuje) wskazany przez @p where_to_add.
 * @param[in] mono : jednomian, nowy element listy
 * @param[in] where_to_add : wskaźnik na listę
 */
static void AddNewElementToList(const Mono *mono, Mono **where_to_add)
{
    Mono *new_element = (Mono *)malloc(sizeof(Mono));
    *new_element = MonoClone(mono);

    *where_to_add = new_element;
}

/**
 * Dodaje dwie listy jednomianów.
 * W wyniku dodawania list powstaje posortowana lista.
 * Alokuje pamięć na listę wynikową.
 * @param[in] p_list : lista jednomianów
 * @param[in] q_list : lista jednomianów
 * @return suma list jednomianów
 */
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
            Poly new_coeff = PolyAdd(&(first_in_p->p), &(first_in_q->p));

            if (!PolyIsZero(&new_coeff))
            {
                Mono *new_element = (Mono *)malloc(sizeof(Mono));
                *new_element = MonoFromPoly(&new_coeff, first_in_p->exp);

                *last_element = new_element;
                last_element = &((*last_element)->next);
            }
            else
            {
                PolyDestroy(&new_coeff);
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

/**
 * Zwraca listę jednomianów danego wielominu.
 * W przypadku wielominu stałego zwraca nową litę zawierającą jednomian
 * równy temu wielomianowi stałemu.
 * Alokuje pamięć dla nowej listy wielomianu stałego.
 * @param[in] p : wielomian
 * @return lista jednomianów
 */
static Mono *PolyGetList(const Poly *p)
{
    if (!p->is_normal)
    {
        Mono *new_list = (Mono *)malloc(sizeof(Mono));
        Poly new_coeff = PolyClone(p);
        *new_list = MonoFromPoly(&new_coeff, 0);

        return new_list;
    }

    return p->list_of_mono;
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    if (!p->is_normal && !q->is_normal)
    {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    if (PolyIsZero(p))
    {
        return PolyClone(q);
    }

    if (PolyIsZero(q))
    {
        return PolyClone(p);
    }
    Mono *p_list;
    Mono *q_list;

    p_list = PolyGetList(p);
    q_list = PolyGetList(q);

    Mono *summed_list = ListOfMonoAdd(p_list, q_list);

    //usunięcie niepotrzebnej alokacji listy wielominu stałego
    if (!p->is_normal)
    {
        ListOfMonoDestroy(p_list);
    }

    if (!q->is_normal)
    {
        ListOfMonoDestroy(q_list);
    }

    if (summed_list == NULL)
    {
        return PolyZero();
    }

    return (Poly) {.is_normal = true, .list_of_mono = summed_list};
}

/**
 * Dodaje dwa jednomiany o tym samym wykładniku.
 * Jeśli w wyniku dodawania powstaje wielomian zerowy zwraca NULL.
 * @param[in] first : jednomian
 * @param[in] secound : jednomian
 * @return wysumowany jednomian
 */
static Mono *Merge(Mono *first, Mono *secound)
{
    Poly p = PolyAdd(&first->p, &secound->p);

    if (PolyIsZero(&p))
    {
        return NULL;
    }
    else
    {
        Mono *merged = (Mono *)malloc(sizeof(Mono));
        *merged = MonoFromPoly(&p, first->exp);
        return merged;
    }
}

/**
 * Sumuje jednomiany o tych samych wykładnikach znajdujące się w liście.
 * Zakładamy, że lista jest posortowana nierosnąco.
 * @param[in] list : wskaźnik na listę jednomianów
 */
static void MergeSameExp(Mono **list)
{
    if (*list != NULL && (*list)->next != NULL)
    {
        if ((*list)->exp == (*list)->next->exp)
        {
            Mono *temp = Merge(*list, (*list)->next);

            if (temp != NULL)
            {
                temp->next = (*list)->next->next;
            }
            else
            {
                temp = (*list)->next->next;
            }

            MonoDestroy((*list)->next);
            free((*list)->next);
            MonoDestroy(*list);
            free(*list);

            *list = temp;
            MergeSameExp(list);
        }
        else
        {
            MergeSameExp(&(*list)->next);
        }
    }
}

static int CompareMono(const void * a, const void * b){
    return (((Mono *)a)->exp - ((Mono*)b)->exp) * (-1);
}

/**
 * Tworzy listę z jednomianów z tablicy.
 * Przejmuje na własność zawartość tablicy @p monos.
 * Założenie: tablica nie jest pusta.
 * @param[in] count : liczba jednomianów w tablicy
 * @param[in] monos : tablica jednomianów
 * @return lista jednomianów
 */
static Mono *MakeListFromArray(unsigned count, const Mono monos[])
{
    Mono *hack_monos = (Mono *)monos;
    qsort(hack_monos, count, sizeof(Mono), CompareMono);

    unsigned int prev_i = 0;
    unsigned int i = 0;
    unsigned int j = 1;

    while (i < count)
    {
        if (j < count && hack_monos[i].exp == hack_monos[j].exp)
        {
            Poly p = PolyAdd(&hack_monos[i].p, &hack_monos[j].p);
            Mono sum = MonoFromPoly(&p, hack_monos[i].exp);

            PolyDestroy(&hack_monos[i].p);
            PolyDestroy(&hack_monos[j].p);
            hack_monos[i] = sum;

            if (PolyIsZero(&p))
            {
                if (prev_i > 0)
                {
                    i = prev_i;
                }
                else if (j + 1 < count)
                {
                    hack_monos[i].exp = hack_monos[j + 1].exp;
                }
                else
                {
                    return NULL;
                }
            }

            j++;
        }
        else if (j < count)
        {
            hack_monos[i].next = &hack_monos[j];
            prev_i = i;
            i = j;
            j++;
        }
        else
        {
            prev_i = i;
            i = j;
        }
    }

    hack_monos[count - 1].next = NULL;

    Mono *iterator = hack_monos;
    Mono **last;
    Mono *result;
    last = &result;

    while (iterator != NULL)
    {
        Mono *new_element = (Mono *)malloc(sizeof(Mono));
        *last = new_element;
        last = &new_element->next;
        *new_element = *iterator;

        iterator = iterator->next;
    }

    return result;
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    if (count == 0)
    {
        return PolyZero();
    }

    Poly sum;
    Mono *mono_list = MakeListFromArray(count, monos);

    if (mono_list != NULL)
    {
        sum.is_normal = true;
        if (PolyIsZero(&mono_list->p))
        {
            sum = PolyZero();
            free(mono_list);
        }
        else
        {
            sum.list_of_mono = mono_list;
        }
    }
    else
    {
        sum = PolyZero();
    }

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
            iterator = iterator->next;
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

    size_t p_list_len = 0;
    size_t q_list_len = 0;

    Mono *p_iterator = p->list_of_mono;
    Mono *q_iterator = q->list_of_mono;

    while (p_iterator != NULL)
    {
        p_list_len++;
        p_iterator = p_iterator->next;
    }

    while (q_iterator != NULL)
    {
        q_list_len++;
        q_iterator = q_iterator->next;
    }

    size_t size = p_list_len * q_list_len;

    Mono *mul_array = (Mono *)calloc(size, sizeof(Mono));

    p_iterator = p->list_of_mono;

    size_t curr_p = 0;
    size_t curr_q = 0;

    while (p_iterator != NULL)
    {
        q_iterator = q->list_of_mono;
        curr_q = 0;

        while (q_iterator != NULL)
        {
            Poly mul_coeff = PolyMul(&p_iterator->p, &q_iterator->p);
            mul_array[q_list_len * curr_p + curr_q] =
                    MonoFromPoly(&mul_coeff, p_iterator->exp + q_iterator->exp);

            q_iterator = q_iterator->next;
            curr_q++;
        }

        p_iterator = p_iterator->next;
        curr_p++;
    }

    Poly result = PolyAddMonos(size, mul_array);
    free(mul_array);

    return result;
}

/**
 * Neguje wszystkie wielomiany stałe wchodzące w skaład podanego wielomianu.
 * @param[in] p : wielomian
 */
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

Poly PolyNeg(const Poly *p)
{
    Poly negated_poly = PolyClone(p);
    AllCoeffNeg(&negated_poly);

    return negated_poly;
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly q_negated = PolyNeg(q);

    return PolyAdd(p, &q_negated);
}

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną.
 * @param[in] p : wielominan
 * @param[in] var_idx : indeks zmiennej
 * @param[in] curr_idx : indeks głównej zmiennej wielomianu @p p
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
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
    Poly subtraction = PolySub(p, q);

    bool isEq = PolyIsZero(&subtraction);
    PolyDestroy(&subtraction);

    return isEq;
}

/**
 * Mnoży wielomian przez stałą.
 * @param[in] p : wielomian
 * @param[in] multiplier : mnożnik który jest współczynnikiem wielomianu
 */
static void MultiplyPoly(Poly *p, poly_coeff_t multiplier)
{
    if (p->is_normal)
    {
        Mono **iterator;
        iterator = &p->list_of_mono;
        while (*iterator != NULL)
        {
            MultiplyPoly(&(*iterator)->p, multiplier);
            if (PolyIsZero(&(*iterator)->p))
            {
                if ((*iterator)->next != NULL)
                {
                    Mono *temp = (*iterator)->next;
                    **iterator = MonoClone(temp);
                    MonoDestroy(temp);
                    free(temp);
                }
                else
                {
                    free(*iterator);
                    *iterator = NULL;
                }
            }
            else
        {
            iterator = &(*iterator)->next;
        }
    }
    if (p->list_of_mono == NULL)
    {
        p->is_normal = false;
        p->coeff = 0;
    }
}
else
{
p->coeff = p->coeff * multiplier;
}
}

/**
 * Zwraca wynik potęgowania.
 * @param[in] base : baza potęgowania
 * @param[in] exp : wykładnik potęgowania
 * @return wynik potęgowania
 */
long power(long base, long exp)
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
    if (PolyIsZero(p))
    {
        return PolyZero();
    }

    if (!p->is_normal)
    {
        return PolyFromCoeff(p->coeff);
    }

    unsigned int p_list_len = ListLen(p->list_of_mono);

    unsigned int all_mono_list_len = 0;
    Mono *iterator = p->list_of_mono;

    //zlicza ile jednomianów powinno znaleźć się w tablicy
    for (unsigned int i = 0; i < p_list_len; i++)
    {
        if (iterator->p.is_normal)
        {
            all_mono_list_len += ListLen(iterator->p.list_of_mono);
        }
        else
        {
            all_mono_list_len ++;
        }

        iterator = iterator->next;
    }

    iterator = p->list_of_mono;
    Mono *mono_array = (Mono *)calloc(all_mono_list_len, sizeof(Mono));
    unsigned int next_free_space = 0;

    for (unsigned int i = 0; i < p_list_len; i++)
    {
        if (iterator->p.is_normal)
        {
            Mono *secound_iterator = iterator->p.list_of_mono;
            while (secound_iterator != NULL)
            {
                mono_array[next_free_space] = MonoClone(secound_iterator);
                poly_coeff_t multiplier = power(x, iterator->exp);
                MultiplyPoly(&mono_array[next_free_space].p, multiplier);

                next_free_space++;
                secound_iterator = secound_iterator->next;
            }
        }
        else
        {
            mono_array[next_free_space] = MonoFromPoly(&iterator->p, 0);
            poly_coeff_t multiplier = power(x, iterator->exp);
            MultiplyPoly(&mono_array[next_free_space].p, multiplier);

            next_free_space++;
        }

        iterator = iterator->next;
    }

    Poly result = PolyAddMonos(all_mono_list_len, mono_array);
    free(mono_array);

    return result;
}
