#ifndef TASK_H
#define TASK_H
typedef enum
{
    ADD,
    SUB,
    MUL,
    DIV
} op_t;

typedef struct task
{
    op_t op;
    int lval, rval;
    int result;
} task_t;

static inline void compute(task_t *p)
{
    if (p->op == ADD)
        p->result = p->lval + p->rval;
    if (p->op == SUB)
        p->result = p->lval - p->rval;
    if (p->op == MUL)
        p->result = p->lval * p->rval;
    if (p->op == DIV)
        p->result = p->lval / p->rval;
}

#endif