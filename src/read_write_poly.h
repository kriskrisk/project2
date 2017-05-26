/** @file
   Implementacja wczytywania i wypisywania wielomianów.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#ifndef __READ_WRITE_POLY_H__
#define __READ_WRITE_POLY_H__

#include "poly.h"

/**
 * Tworzy wielomian na podstawie stringa.
 * @param[in] poly : string zawierający wielomian
 * @return wielomian
 */
extern Poly ParsePoly(char *poly);

/**
 * Wczytuje linię ze standardowego wejścia.
 * @return string
 */
extern char *ReadOneLine();

/**
 * Wypisuje wielomian na standardowe wyjście.
 * @param[in] p : wielomian
 */
extern void PrintPoly(Poly *p);

#endif /* __READ_WRITE_POLY_H__ */
