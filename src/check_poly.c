/** @file
   Implementacja funkcji sprawdzających poprawność wielomianu.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#include "check_poly.h"

/**
 * Sprawdza, czy liczba nie przekracza odpowiedniego zakresu.
 * @param[in] str : string zawierający liczbę
 * @param[in] pos : pozycja na której zaczyna się liczba
 * @param[in] type : rodzaj wczytywanej liczby
 * @return czy liczba jest poprawna
 */
static bool CheckSizeOfNumber(char *str, unsigned *pos, int type)
{
    long long number = 0;
    bool sign = true;
    long long max = 0;
    long long min = 0;
    bool empty = true;

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

    if (str[*pos] == '-')
    {
        empty = false;
        sign = false;
        int value = str[*pos + 1] - '0';
        number = value * (-1);

        if (number > max || number < min)
        {
            return false;
        }

        *pos = *pos + 2;
    }

    while (isdigit(str[*pos]))
    {
        empty = false;
        number *= 10;
        int value = str[*pos] - '0';

        if (!sign)
        {
            number -= value;
        }
        else
        {
            number += value;
        }

        if (number > max || number < min)
        {
            return false;
        }

        *pos = *pos + 1;
    }

    if (empty)
    {
        return false;
    }

    return true;
}

/**
 * Sprawdza, czy liczba jest poprawnym współczynnikiem wielomianu.
 * @param[in] line : string zawierający liczbę
 * @param[in] pos : pozycja na której zaczyna się liczba
 * @return czy liczba jest poprawnym współczynnikiem
 */
static bool CheckCoeff(char *line, unsigned *pos)
{
    return CheckSizeOfNumber(line, pos, POINT);
}

/**
 * Sprawdza, czy liczba jest poprawnym wykładnikiem jednomianu.
 * @param[in] line : string zawierający liczbę
 * @param[in] pos : pozycja na której zaczyna się liczba
 * @return czy liczba jest poprawnym wykładnikiem
 */
static bool CheckExp(char *line, unsigned *pos)
{
    return CheckSizeOfNumber(line, pos, EXP);
}

/**
 * Sprawdza poprawność listy jednomianów.
 * @param[in] line : string zawierający listę jednomianów
 * @param[in] pos : pozycja na której zaczyna się lista
 * @return czy lista jest poprawna
 */
static bool CheckListOfMono(char *line, unsigned *pos);

/**
 * Sprawdza poprawność wielomianu.
 * @param[in] line : string zawierający wielomian
 * @param[in] pos : pozycja na której zaczyna się wielomian
 * @return czy wielomian jest poprawny
 */
static bool CheckPoly(char *line, unsigned *pos);

/**
 * Sprawdza poprawność jednomianu.
 * @param[in] line : string zawierający jednomian
 * @param[in] pos : pozycja na której zaczyna się jednomian
 * @return czy jednomian jest poprawny
 */
static bool CheckMono(char *line, unsigned *pos)
{
    if (line[*pos] != '(')
    {
        return false;
    }

    *pos = *pos + 1;

    if (!CheckPoly(line, pos))
    {
        return false;
    }

    if (line[*pos] != ',')
    {
        return false;
    }

    *pos = *pos + 1;

    if (!CheckExp(line, pos))
    {
        return false;
    }

    if (line[*pos] != ')')
    {
        return false;
    }

    *pos = *pos + 1;

    return true;
}

static bool CheckListOfMono(char *line, unsigned *pos)
{
    bool current;

    while(true)
    {
        current = CheckMono(line, pos);

        if (!current)
        {
            return current;
        }

        if (line[*pos] == '+')
        {
            *pos = *pos + 1;
        }
        else
        {
            return current;
        }
    }
}

static bool CheckPoly(char *line, unsigned *pos)
{
    if (line[*pos] == '(')
    {
        return CheckListOfMono(line, pos);
    }
    else
    {
        return CheckCoeff(line, pos);
    }
}

long CheckLine(char *line)
{
    unsigned pos = 0;

    if (CheckPoly(line, &pos) && line[pos] == '\0')
    {
        return CORRECT;
    }

    return pos + 1;
}
