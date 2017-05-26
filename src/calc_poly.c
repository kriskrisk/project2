/** @file
   Implementacja kaltulatora

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <limits.h>

#include "poly.h"
#include "check_poly.h"
#include "stack.h"

#define WRONG_COMMAND -2
#define STACK_UNDERFLOW -3
#define WRONG_VALUE -4
#define WRONG_VARIABLE -5

Poly ParseOnePoly(char *poly, unsigned *pos);

Mono ParseOneMono(char *poly, unsigned *pos)
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

Poly ParseOnePoly(char *poly, unsigned *pos)
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

        if (poly[*pos + 1] != '0')//CHANGE
        {
            monos[first_free] = ParseOneMono(poly, pos);
            first_free++;
        }
        else
        {
            //przesuwam się do następnego mono
            *pos = *pos + 3;
            while (isdigit(poly[*pos]))
            {
                *pos = *pos + 1;
            }
            *pos = *pos + 1;
        }

        //monos[first_free] = ParseOneMono(poly, pos);
        //first_free++;

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

    if (first_free == 0)//CHANGE
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

char *DoubleTheSize(char *array, unsigned size)
{
    char *new_array = (char *)malloc(2 * size);
    strncpy(new_array, array, size);
    free(array);
    return new_array;
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

long Choose(long a, long b)
{
    if (a == CORRECT || b == CORRECT)
    {
        return CORRECT;
    }
    else
    {
        return a > b ? a : b;
    }
}

long CheckSizeOfNumber(char *str, long pos, int type)
{
    bool empty = true;
    long long number = 0;
    long i = pos;
    long long max = 0;
    long long min = 0;

    if (type == EXP)
    {
        max = INT_MAX;
        min = 0;
    }
    else if (type == IDX)
    {
        max = UINT_MAX;
        min = 0;
    }
    else if (type == POINT)
    {
        max = LONG_MAX;
        min = LONG_MIN;
    }

    if (str[i] == '-')
    {
        empty = false;
        int value = str[i + 1] - '0';
        number = value * (-1);

        if (number > max || number < min)
        {
            return i + 1;
        }

        i = i + 2;
    }

    while (isdigit(str[i]))
    {
        empty = false;
        number *= 10;
        int value = str[i] - '0';

        if (str[pos] == '-')
        {
            number -= value;
        }
        else
        {
            number += value;
        }

        if (number > max || number < min)
        {
            return i + 1;
        }

        i++;
    }

    if (empty)
    {
        return pos + 1;
    }

    return CORRECT;
}

void PrintPoly(Poly *p);

void PrintMono(Mono *m)
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
            PrintMono(&(p->mono_arr[i]));

            if (i != p->size - 1)
            {
                printf("+");
            }
        }
    }
}

//sprawdza czy to co znajduje się w command[pos] - command[strlen(command) - 1] jest liczbą
bool CheckNumber(char *command, unsigned pos)
{
    unsigned size = strlen(command);

    if (pos == size)
    {
        return false;
    }

    for (unsigned i = pos; i < size; i++)
    {
        if (!isdigit(command[i]) && command[i] != '-')
        {
            return false;
        }
    }

    return true;
}

long CalculateComand(char *command, Stack *stack)
{
    if (strcmp(command, "ZERO") == 0)
    {
        Poly p = PolyZero();
        Push(stack, p);

        return CORRECT;
    }
    else if(strcmp(command, "IS_COEFF") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);

        if (PolyIsCoeff(&p))
        {
            printf("%d\n", 1);
        }
        else
        {
            printf("%d\n", 0);
        }

        return CORRECT;
    }
    else if(strcmp(command, "IS_ZERO") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);

        if (PolyIsZero(&p))
        {
            printf("%d\n", 1);
        }
        else
        {
            printf("%d\n", 0);
        }

        return CORRECT;
    }
    else if(strcmp(command, "CLONE") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        Poly new_p = PolyClone(&p);
        Push(stack, new_p);

        return CORRECT;
    }
    else if(strcmp(command, "ADD") == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolyAdd(&p, &q);
        Push(stack, new_poly);
        PolyDestroy(&p);
        PolyDestroy(&q);

        return CORRECT;
    }
    else if(strcmp(command, "MUL") == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolyMul(&p, &q);
        Push(stack, new_poly);
        PolyDestroy(&p);
        PolyDestroy(&q);

        return CORRECT;
    }
    else if(strcmp(command, "NEG") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly new_poly = PolyNeg(&p);
        Push(stack, new_poly);
        PolyDestroy(&p);

        return CORRECT;
    }
    else if(strcmp(command, "SUB") == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolySub(&p, &q);
        Push(stack, new_poly);
        PolyDestroy(&p);
        PolyDestroy(&q);

        return CORRECT;
    }
    else if(strcmp(command, "IS_EQ") == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        Poly q = Secound(stack);

        if (PolyIsEq(&p, &q))
        {
            printf("%d\n", 1);
        }
        else
        {
            printf("%d\n", 0);
        }

        return CORRECT;
    }
    else if(strncmp(command, "DEG_BY ", 7) == 0)
    {
        if (!CheckNumber(command, 7))
        {
            return WRONG_VARIABLE;
        }

        if (CheckSizeOfNumber(command, 7, IDX) != CORRECT)
        {
            return WRONG_VARIABLE;
        }

        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        unsigned idx = (unsigned)strtol(&(command[7]), NULL, 10);
        printf("%d\n", PolyDegBy(&p, idx));

        return CORRECT;
    }
    else if(strcmp(command, "DEG") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        printf("%d\n", PolyDeg(&p));

        return CORRECT;
    }
    else if(strncmp(command, "AT ", 3) == 0)
    {
        if (!CheckNumber(command, 3))
        {
            return WRONG_VALUE;
        }

        if (CheckSizeOfNumber(command, 3, POINT) != CORRECT)
        {
            return WRONG_VALUE;
        }

        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        poly_coeff_t x = strtol(&(command[3]), NULL, 10);
        Push(stack, PolyAt(&p, x));
        PolyDestroy(&p);

        return CORRECT;
    }
    else if(strcmp(command, "PRINT") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        PrintPoly(&p);
        printf("\n");

        return CORRECT;
    }
    else if(strcmp(command, "POP") == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Pop(stack);

        return CORRECT;
    }
    else
    {
        return WRONG_COMMAND;
    }
}

void PrintError(long error_type, unsigned curr_line)
{
    if (error_type != CORRECT)
    {
        if (error_type >= 0)
        {
            fprintf(stderr, "ERROR %u %li\n", curr_line, error_type);
        }
        else if (error_type == STACK_UNDERFLOW)
        {
            fprintf(stderr, "ERROR %u STACK UNDERFLOW\n", curr_line);
        }
        else if (error_type == WRONG_COMMAND)
        {
            fprintf(stderr, "ERROR %u WRONG COMMAND\n", curr_line);
        }
        else if (error_type == WRONG_VALUE)
        {
            fprintf(stderr, "ERROR %u WRONG VALUE\n", curr_line);
        }
        else if (error_type == WRONG_VARIABLE)
        {
            fprintf(stderr, "ERROR %u WRONG VARIABLE\n", curr_line);
        }
    }
}

int Calculate()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    Initialize(stack);
    unsigned row_number = 1;
    bool koniec = false;

    while (!koniec)
    {
        char *line = ReadOneLine();

        if (line == NULL)
        {
            koniec = true;
        }
        else if (isalpha(line[0]))
        {
            PrintError(CalculateComand(line, stack), row_number);
        }
        else
        {
            long err_col = CheckLine(line);
            PrintError(err_col, row_number);

            if (err_col == CORRECT)
            {
                Poly p = ParsePoly(line);
                Push(stack, p);
            }
        }

        free(line);
        row_number++;
    }

    Clear(stack);
    free(stack);

    return 0;
}

int main2() {
    Calculate();

    return 0;
}
