/** @file
   Implementacja stosu i operacji na nim.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"

/**
 * Struktura będąca elementem stosu.
 * Przechowuje wielomian.
 */
typedef struct Node
{
    Poly p; ///< wielomian
    struct Node *next; ///< następny element stosu
} Node;

/**
 * Struktura będąca stosem.
 * Wskazuje na element na szczycie stosu.
 */
typedef struct Stack
{
    Node *top; ///< szczyt stosu
} Stack;

/**
 * Inicjalizuje stos.
 * @return zainicjalizowany stos
 */
extern Stack *Initialize();

/**
 * Sprawdza, czy stos jest pusty.
 * @param[in] s : stos
 * @return czy jest pusty
 */
extern bool IsEmpty(Stack *s);

/**
 * Wstawia wielomian na stos.
 * @param[in] s : stos
 * @param[in] p : wielomian
 */
extern void Push(Stack *s, Poly p);

/**
 * Zdejmuje wielomian ze stosu.
 * @param[in] s : stos
 */
extern void Pop(Stack *s);

/**
 * Zwraca wielomian z wierzchołka stosu.
 * @param[in] s : stos
 * @return wielomian
 */
extern Poly Top(Stack *s);

/**
 * Zwraca wielomian znajdujący się pod wierzchołkiem stosu.
 * @param[in] s : stos
 * @return wielomian
 */
extern  Poly Secound(Stack *s);

/**
 * Zdejmuje i zwraca wielomian z wierzchołka stosu.
 * @param[in] s : stos
 * @return wielomian
 */
extern Poly PopAndReturn(Stack *s);

/**
 * Niszczy stos.
 * @param[in] s : stos
 */
extern void Clear(Stack *s);

#endif /* __STACK_H__ */
