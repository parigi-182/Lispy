#ifndef _DEFINITIONS_
  #include"definitions.h"
#endif
#define _LVAL_LIB_



lval* lval_num(long x);

lval* lval_err(char* fmt, ...);

lval* lval_sym(char* s);

lval* lval_fun(lbuiltin func);

lval* lval_sexpr(void);

lval* lval_qexpr(void);

void lval_del(lval* v);

lval* lval_copy(lval* v);

lval* lval_add(lval* v, lval* x);

lval* lval_join(lval* x, lval* y);

lval* lval_pop(lval* v, int i);

lval* lval_take(lval* v, int i);

void lval_print(lval* v);

void lval_print_expr(lval* v, char open, char close);

void lval_print(lval* v);

void lval_println(lval* v);

void lval_print_str(lval* v);

void lval_print_str(lval* v);

lval* lval_str(char* s);