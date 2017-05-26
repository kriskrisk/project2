/** @file
   Implementacja stosu i operacji na nim.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"

typedef struct Node
{
    Poly p;
    struct Node *next;
} Node;

typedef struct Stack
{
    Node *top;
} Stack;

extern void Initialize(Stack *s);

extern bool IsEmpty(Stack *s);

extern void Push(Stack *s, Poly p);

extern void Pop(Stack *s);

extern Poly Top(Stack *s);

extern  Poly Secound(Stack *s);

extern Poly PopAndReturn(Stack *s);

extern void Clear(Stack *s);

#endif /* __STACK_H__ */
