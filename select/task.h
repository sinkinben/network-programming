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

#endif