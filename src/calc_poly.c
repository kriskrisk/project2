/** @file
   Implementacja kaltulatora i funkcji działających na komendach.

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
#include "read_write_poly.h"

/** Błędna komenda */
#define WRONG_COMMAND -2

/** Za mało wielomianów na stosie */
#define STACK_UNDERFLOW -3

/** Błędna liczba */
#define WRONG_VALUE -4

/** Błędna zmienna */
#define WRONG_VARIABLE -5

/**
 * Sprawdza, czy liczba nie przekracza odpowiedniego zakresu.
 * @param[in] str : string zawierający liczbę
 * @param[in] pos : pozycja na której zaczyna się liczba
 * @param[in] type : rodzaj wczytywanej liczby
 * @return czy liczba jest poprawna
 */
static long CheckSizeOfNumber(char *str, long pos, int type)
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

/**
 * Sprawdza, czy ciąg znaków jest liczbą.
 * Rozpatrywany jest ciąg znaków od pozycji pos do kończa stringa.
 * @param[in] command : string zawierający liczbę
 * @param[in] pos : pozycja na której zaczyna się liczba
 * @return czy wszystkie znaki to cyfry
 */
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

/**
 * Przetwarza komendę.
 * Zwraca pozycję na której wystąpił błąd w komendzie.
 * Jeśli komenda jest poprawna zwraca -1 (CORRACT).
 * @param[in] command : string zawierający komendę
 * @param[in] stack : stos
 * @return czy liczba jest poprawna
 */
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

/**
 * Gdzy komenda nie jest poprawna wypisuje odpowiedni komunikat.
 * @param[in] error_type : jeden z czterech typów błędu
 * @param[in] curr_line : aktualny wiersz
 */
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

/**
 * Wykonuje działania na wielomianach.
 * Inicjuje stos, wczytuje linie i wykonuje odpowiedznie czynności
 * do momentu wczytania EOF.
 * @return zwraca 0 gdy operacje się powiodły
 */
int Calculate()
{
    Stack *stack = Initialize();
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

int main() {
    Calculate();

    return 0;
}
