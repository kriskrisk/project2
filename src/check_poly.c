/** @file
   Implementacja funkcji sprawdzających poprawność wielomianu.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

#include "check_poly.h"

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

bool CheckCoeff(char *line, unsigned *pos)
{
    return CheckSizeOfNumber(line, pos, POINT);
}

bool CheckExp(char *line, unsigned *pos)
{
    return CheckSizeOfNumber(line, pos, EXP);
}

bool CheckListOfMono(char *line, unsigned *pos);

bool CheckPoly(char *line, unsigned *pos);

bool CheckMono(char *line, unsigned *pos)
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

bool CheckListOfMono(char *line, unsigned *pos)
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

bool CheckPoly(char *line, unsigned *pos)
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
