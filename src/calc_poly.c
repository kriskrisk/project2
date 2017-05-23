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
#include <limits.h>
#include <windef.h>

#include "poly.h"

#define CORRECT -1
#define WRONG_COMMAND -2
#define STACK_UNDERFLOW -3
#define WRONG_VALUE -4
#define WRONG_VARIABLE -5
#define EXP 1
#define IDX 2
#define POINT 3

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
    new_node->p = p;

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

Poly Top(Stack *s)
{
    if (IsEmpty(s))
    {
        printf("\nStack is Empty\n");
        return PolyZero();//do poprawy
    }
    else
    {
        return s->top->p;
    }
}

Poly Secound(Stack *s)
{
    if (!IsEmpty(s) && s->top->next != NULL)
    {
        return s->top->next->p;
    }
    //error?
    return
}

//czeba zalokować coś na to co zwróci
Poly PopAndReturn(Stack *s)
{
    assert(s->top != NULL);
    Node *to_delete = s->top;
    Poly to_return = to_delete->p;
    s->top = s->top->next;
    free(to_delete);
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

    for (unsigned i = 0; i < size; i++)
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

long IsComma(char *line, long pos)
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

long IsOpening(char *line, long pos)
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

long IsPlus(char *line, long pos)
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

long IsClosing(char *line, long pos)
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

bool CheckSizeOfNumber(char *str, int type)
{
    long long number = 0;
    unsigned i = 0;
    long long max;
    long long min;

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

    while (isdigit(str[0]))
    {
        number *= 10;
        int value = str[0] - '0';
        number += value;

        if (number > max || number < min)
        {
            return false;
        }

        i++;
    }

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
    if (sign && !CheckSizeOfNumber(&(line[pos]), EXP))
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

void PolyPrint(Poly *p)
{
    //wypisz wielomian
}

long CalculateComand(char *command, Stack *stack)
{
    if (strncmp(command, "ZERO", 4) == 0)
    {
        Poly p = PolyZero();
        Push(stack, p);

        return CORRECT;
    }
    else if(strncmp(command, "IS_COEFF", 8) == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);

        if (PolyIsCoeff(&p))
        {
            printf("%d", 1);
        }
        else
        {
            printf("%d", 0);
        }

        return CORRECT;
    }
    else if(strncmp(command, "IS_ZERO", 7) == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);

        if (PolyIsZero(&p))
        {
            printf("%d", 1);
        }
        else
        {
            printf("%d", 0);
        }

        return CORRECT;
    }
    else if(strncmp(command, "CLONE", 5) == 0)
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
    else if(strncmp(command, "ADD", 3) == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolyAdd(&p, &q);
        Push(stack, new_poly);

        return CORRECT;
    }
    else if(strncmp(command, "MUL", 3) == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolyMul(&p, &q);
        Push(stack, new_poly);

        return CORRECT;
    }
    else if(strncmp(command, "NEG", 3) == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly new_poly = PolyNeg(&p);
        Push(stack, new_poly);

        return CORRECT;
    }
    else if(strncmp(command, "SUB", 3) == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = PopAndReturn(stack);
        Poly q = PopAndReturn(stack);
        Poly new_poly = PolySub(&p, &q);
        Push(stack, new_poly);

        return CORRECT;
    }
    else if(strncmp(command, "IS_EQ", 5) == 0)
    {
        if (IsEmpty(stack) || stack->top->next == NULL)
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        Poly q = Secound(stack);

        if (PolyIsEq(&p, &q))
        {
            printf("%d", 1);
        }
        else
        {
            printf("%d", 0);
        }

        return CORRECT;
    }
    else if(strncmp(command, "DEG", 3) == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        printf("%d", PolyDeg(&p));

        return CORRECT;
    }
    else if(strncmp(command, "DEG_BY ", 7) == 0)
    {
        if (!CheckSizeOfNumber(&(command[7]), IDX))
        {

        }

        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        unsigned idx = (unsigned)strtol(&(command[7]), NULL, 10);
        printf("%d", PolyDegBy(&p, idx));
    }
    else if(strncmp(command, "AT ", 3) == 0)
    {
        Poly p = PopAndReturn(stack);
        poly_coeff_t x = strtol(&(command[3]), NULL, 10);
        Push(stack, PolyAt(&p, x));

    }
    else if(strncmp(command, "PRINT", 5) == 0)
    {
        if (IsEmpty(stack))
        {
            return STACK_UNDERFLOW;
        }

        Poly p = Top(stack);
        PolyPrint(&p);

        return CORRECT;
    }
    else if(strncmp(command, "POP", 3) == 0)
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
            printf("ERROR %u %li\n", curr_line, error_type);
        }
        else if (error_type == STACK_UNDERFLOW)
        {
            printf("ERROR %u STACK UNDERFLOW\n", curr_line);
        }
        else if (error_type == WRONG_COMMAND)
        {
            printf("ERROR %u WRONG COMMAND\n", curr_line);
        }
        else if (error_type == WRONG_VALUE)
        {
            printf("ERROR %u WRONG VALUE\n", curr_line);
        }
        else if (error_type == WRONG_VARIABLE)
        {
            printf("ERROR %u WRONG VARIABLE\n", curr_line);
        }
    }
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
            PrintError(CalculateComand(line, stack), row_number);
        }
        else
        {
            long err_col = CheckPoly(line);
            PrintError(err_col, row_number);

            if (err_col != CORRECT)
            {
                Poly p = ParsePoly(line);
                Push(stack, p);
            }
        }

        row_number++;
        koniec = true;
    }

    return 0;
}

int main2(int argc, char **argv) {
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
