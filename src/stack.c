/** @file
   Implementacja stosu i operacji na nim.

   @author Krzysztof Olejnik <ko361240@students.mimuw.edu.pl>
*/

#include <assert.h>

#include "stack.h"

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
    assert(s->top != NULL);
    Node *to_delete;
    to_delete = s->top;
    s->top = s->top->next;
    PolyDestroy(&to_delete->p);
    free(to_delete);
}

Poly Top(Stack *s)
{
    assert(s->top != NULL);
    return s->top->p;
}

Poly Secound(Stack *s)
{
    assert(s->top != NULL && s->top->next != NULL);
    return s->top->next->p;
}

Poly PopAndReturn(Stack *s)
{
    assert(s->top != NULL);
    Node *to_delete = s->top;
    Poly to_return = to_delete->p;
    s->top = s->top->next;
    free(to_delete);
    return to_return;
}

void Clear(Stack *s)
{
    while (!IsEmpty(s))
    {
        Node *to_delete = s->top;
        s->top = to_delete->next;
        PolyDestroy(&to_delete->p);
        free(to_delete);
    }
}
