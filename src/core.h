#ifndef _DEFINITIONS_
#include"definitions.h"
#endif
#define _CORE_
#include"mpc.h"

char* ltype_name(int t);

lval* lval_lambda(lval* formals, lval* body);

lval* builtin_list(lenv* e, lval* v);

lval* builtin_head(lenv* e, lval* a);

lval* builtin_tail(lenv* e, lval* a);

lval* builtin_eval(lenv* e, lval* a);

lval* builtin_join(lenv* e, lval* a);

lval* builtin_op(lenv* e, lval* a, char* op);

lval* builtin_add(lenv* e, lval* a);

lval* builtin_sub(lenv* e, lval* a);

lval* builtin_mul(lenv* e, lval* a);

lval* builtin_div(lenv* e, lval* a);

lval* builtin_def(lenv* e, lval* a);

lval* builtin_put(lenv* e, lval* a);

lval* builtin_gt(lenv* e, lval* a);

lval* builtin_lt(lenv* e, lval* a);

lval* builtin_ge(lenv* e, lval* a);

lval* builtin_le(lenv* e, lval* a);

lval* builtin_if(lenv* e, lval* a);

lval* builtin_eq(lenv* e, lval* a);

lval* builtin_ne(lenv* e, lval* a);

lval* builtin_print(lenv* e, lval* a);

lval* builtin_error(lenv* e, lval* a);

lval* builtin_lambda(lenv* e, lval* a);

lval* builtin_var(lenv* e, lval* a, char* func);

lval* builtin_ord(lenv* e, lval*a, char* op);

lval* builtin_load(lenv* e, lval* a);

void lenv_add_builtin(lenv* e, char* name, lbuiltin func);

void lenv_add_builtins(lenv* e);

lval* lval_call(lenv* e, lval* f, lval* a);

lval* lval_eval(lenv* e, lval* v);

lval* lval_eval_sexpr(lenv* e, lval* v);

lval* lval_read_num(mpc_ast_t* t);

lval* lval_read_str(mpc_ast_t* t);

lval* lval_read(mpc_ast_t* t);