/** @file
   Implementacja działań na wielomianach

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mem.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <windef.h>

#include "poly.h"

#define CORRECT -1

typedef struct Node
{
    Poly p;
    struct Node *next;
} Node;

typedef struct Stack
{
    Node *top;
} Stack;

void Initialize(Stack *s)
{
    s->top = NULL;
}

bool IsEmpty(Stack *s)
{
    return s->top == NULL;
}

void Push(Stack *s, Poly p)
{
    Node *new_node = (Node *)malloc(sizeof(Node));

    if (IsEmpty(s))
    {
        s->top = new_node;
        new_node->next = NULL;
    }
    else
    {
        new_node->next = s->top;
        s->top = new_node;
    }
}

void Pop(Stack *s)
{
    if (IsEmpty(s))
    {
        printf("\nStack is Empty\n");
    }
    else
    {
        Node *to_delete;
        to_delete = s->top;
        s->top = s->top->next;
        PolyDestroy(&to_delete->p);
        free(to_delete);
    }
}

//czeba zalokować coś na to co zwróci
Poly PopAndReturn(Stack *s)
{
    assert(s->top != NULL);
    Node *to_delete = s->top;
    Poly to_return = to_delete->p;
    s->top = s->top->next;
    return to_return;
}

Poly ParseOnePoly(char **next_to_parse);

Mono ParseOneMono(char **next_to_parse)
{
    Poly poly_coeff;

    if ((*next_to_parse)[1] == '(')
    {
        char *temp = &(*next_to_parse)[1];
        poly_coeff = ParseOnePoly(&temp);
        *next_to_parse = temp;
    }
    else
    {
        poly_coeff_t coeff = strtol(&(*next_to_parse)[1], next_to_parse, 10);
        poly_coeff = PolyFromCoeff(coeff);
    }

    poly_exp_t exp = (poly_exp_t)strtol(&(*next_to_parse)[1], next_to_parse, 10);
    *next_to_parse = &(*next_to_parse)[1];

    return MonoFromPoly(&poly_coeff, exp);
}

unsigned CountMonos(const char *next_to_parse)
{
    int deference = 0;
    unsigned count = 1;

    for (unsigned i = 0; i < strlen(next_to_parse); i++)
    {
        if (next_to_parse[i] == '(')
        {
            deference++;
        }
        else if (next_to_parse[i] == ')')
        {
            deference--;
        }
        else if (next_to_parse[i] == '+' && deference == 0)
        {
            count++;
        }

        if (deference == -1 || (next_to_parse[i] == ',' && deference == 0)) {
            break;
        }
    }

    return count;
}

Poly ParseOnePoly(char **next_to_parse)
{
    if ((*next_to_parse)[0] != '(')
    {
        return PolyFromCoeff(strtol(&(*next_to_parse)[0], next_to_parse, 10));
    }

    unsigned size = CountMonos(*next_to_parse);
    Mono *monos = (Mono *)malloc(size * sizeof(Mono));

    for (int i = 0; i < size; i++)
    {
        if (i > 0)
        {
            *next_to_parse = &(*next_to_parse)[1];
        }
        monos[i] = ParseOneMono(next_to_parse);
    }

    Poly poly = PolyAddMonos(size, monos);
    free(monos);

    return poly;
}

Poly ParsePoly(char *poly)
{
    char *dummy = poly;
    char **next_to_parse = &dummy;

    return ParseOnePoly(next_to_parse);
}

void DoubleTheSize(char *array, unsigned size)
{
    char *new_array = (char *)malloc(2 * size * sizeof(char));
    strncpy(new_array, array, size);
    free(array);
    *array = *new_array;
}

char *ReadOneLine()
{
    char *line = (char *)malloc(2 * sizeof(char));
    unsigned first_free = 0;
    unsigned current_size = 2;

    do
    {
        if (first_free == current_size)
        {
            DoubleTheSize(line, current_size);
            current_size = current_size * 2;
        }
        line[first_free] = (char)getchar();
        first_free++;
    } while (line[first_free - 1] != '\n');

    realloc(line, first_free);

    return line;
}

long Choose(long a, long b)
{
    if (a == CORRECT || b == CORRECT)
    {
        return CORRECT;
    }
    else
    {
        return max(a, b);
    }
}

long IsNumber(char *line, long pos, bool after_comma, bool sign);

long IsComma(char *line, int pos)
{
    bool result = line[pos] == ',';

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return CORRECT;
    }

    return IsNumber(line, pos + 1, true, true);
}

long IsOpening(char *line, int pos)
{
    bool result = line[pos] == '(';

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return CORRECT;
    }

    return Choose(IsNumber(line, pos + 1, false, true), IsOpening(line, pos + 1));
}

long IsPlus(char *line, int pos)
{
    bool result = line[pos] == '+';

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return CORRECT;
    }

    return IsOpening(line, pos + 1);
}

long IsClosing(char *line, int pos)
{
    bool result = line[pos] == ')';

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return CORRECT;
    }

    return Choose(IsPlus(line, pos + 1), IsComma(line, pos + 1));
}

bool CheckSizeOfNumber(char *number, bool after_comma)
{
    char *ptr;
    long con_num = strtol(number, &ptr, 10);

    if (con_num == 0 && number == ptr)
    {
        return false;
    }

    if (after_comma && con_num > INT_MAX)
    {
        return false;
    }
//if (errno == ERANGE)
    return true;
}

long IsNumber(char *line, long pos, bool after_comma, bool sign)
{
    bool is_number = isdigit(line[pos]);
    bool is_sign = line[pos] == '-';
    bool result = sign ? is_number || is_sign : is_number;

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return CORRECT;
    }

    //pierwsza cyfra wczytywanej liczby
    if (sign && !CheckSizeOfNumber(&(line[pos]), after_comma))
    {
        if (after_comma)
        {
            return pos + 6;
        }
        else
        {
            return pos + 11;
        }
    }

    if (after_comma)
    {
        return Choose(IsNumber(line, pos + 1, true, false), IsClosing(line, pos + 1));
    }
    else
    {
        return Choose(IsNumber(line, pos + 1, false, false), IsComma(line, pos + 1));
    }
}

long CheckPoly(char *line)
{
    return Choose(IsNumber(line, 0, false, true), IsOpening(line, 0));
}

void CalculateComand(char *command)
{
    if (strcmp() == 0)
}

int Calculate()
{
    Stack *stack = NULL;
    Initialize(stack);
    unsigned row_number = 1;
    bool koniec = false;

    while (!koniec)
    {
        char *line = ReadOneLine();

        if (isalpha(line[0]))
        {
            CalculateComand(line);
        }
        else
        {
            CheckPoly(line);
            Poly p = ParsePoly(line);
            Push(stack, p);
        }

        row_number++;
        koniec = true;
    }

    return 0;
}

int main(int argc, char **argv) {
    char str[100];

    printf( "Enter a value :");
    gets( str );

    printf("%li", CheckPoly(str));

    //scanf("%*[^\n]%*c");

    //Poly p = ParsePoly(str);
//
//    if (PolyIsZero(&p)) {
//        return 1;
//    }

    return 0;
}
