/** @file
   Implementacja działań na wielomianach

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

#define CORRECT -1
#define WRONG_COMMAND -2
#define STACK_UNDERFLOW -3
#define WRONG_VALUE -4
#define WRONG_VARIABLE -5
#define EXP 1
#define IDX 2
#define POINT 3

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
            line = DoubleTheSize(line, current_size);
            current_size = current_size * 2;
        }

        int temp = (char)getchar();

        if (temp == EOF)
        {
            return NULL;
        }
        line[first_free] = (char)temp;
        first_free++;
    } while (line[first_free - 1] != '\n');

    line[first_free - 1] = '\0';

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
        return a > b ? a : b;
    }
}

long ChooseMin(long a, long b)
{
    if (a == CORRECT && b == CORRECT)
    {
        return CORRECT;
    }
    else if (a == CORRECT)
    {
        return b;
    }
    else if (b == CORRECT)
    {
        return a;
    }
    else
    {
        return a > b ? b : a;
    }
}

long IsNumber(char *line, long pos, bool after_comma, bool sign, bool first);

long IsComma(char *line, long pos)
{
    bool result = line[pos] == ',';

    if (pos == strlen(line) - 1 || !result)
    {
        if (!result)
        {
            return pos + 1;
        }

        return pos + 2;
    }

    return IsNumber(line, pos + 1, true, true, false);
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

        return pos + 2;
    }

    return Choose(IsNumber(line, pos + 1, false, true, false), IsOpening(line, pos + 1));
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

        return pos + 2;
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

long CheckSizeOfNumber(char *str, long pos, int type)
{
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

    return CORRECT;
}

long IsNumber(char *line, long pos, bool after_comma, bool sign, bool first)
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

    int type;

    if (after_comma)
    {
        type = EXP;
    }
    else
    {
        type = POINT;
    }

    //pierwsza cyfra wczytywanej liczby
    if (sign)
    {
        long checked_number = CheckSizeOfNumber(line, pos, type);

        if (checked_number != CORRECT)
        {
            return checked_number;
        }
    }

    if (after_comma)
    {
        return Choose(IsNumber(line, pos + 1, true, false, false), IsClosing(line, pos + 1));
    }
    else
    {
        if (!first)
        {
            return Choose(IsNumber(line, pos + 1, false, false, false), IsComma(line, pos + 1));
        }

        return IsNumber(line, pos + 1, false, false, true);
    }
}

long FirstCheck(char *line)
{
    long difference = 0;

    for (unsigned i = 0; i < strlen(line); i++)
    {
        if (line[i] == '(')
        {
            difference++;
        }
        else if (line[i] == ')')
        {
            difference--;
        }

        if (difference < 0)
        {
            return i + 1;
        }
    }

    if (difference > 0)
    {
        return strlen(line) + 1;
    }

    return CORRECT;
}

long CheckPoly2(char *line)
{
    long first_check = FirstCheck(line);
    long secound_check = Choose(IsNumber(line, 0, false, true, true), IsOpening(line, 0));

    return ChooseMin(first_check, secound_check);
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

    for (int i = pos; i < size; i++)
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
            printf("%li\n", err_col);
            PrintError(err_col, row_number);

            if (err_col == CORRECT)
            {
                Poly p = ParsePoly(line);
                Push(stack, p);
            }
        }

        row_number++;
    }

    Clear(stack);
    free(stack);

    return 0;
}

int main() {
    Calculate();

    return 0;
}
