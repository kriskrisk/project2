/** @file
   Implementacja funkcji sprawdzających poprawność wielomianu.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#ifndef __CHECK_POLY_H__
#define __CHECK_POLY_H__

/** Dane są poprawne */
#define CORRECT -1

/** Liczba jest wykładnikiem */
#define EXP 1

/** Liczba jest indeksem zaiennej */
#define IDX 2

/** Liczba jest wartością zmiennej */
#define POINT 3

/**
 * Sprawdza poprawność wielomianu zawartego w stringu.
 * @param[in] line : string zawierający wielomian
 * @return pozycja znaku, który powoduje to, że wielomian jest niepoprawny,
 * lub -1 (CORRECT) gdy jest poprawny.
 */
extern long CheckLine(char *line);

#endif /* __CHECK_POLY_H__ */
