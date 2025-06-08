#ifndef _DEFINITIONS_
  #include"definitions.h"
#endif
#define _LENV_LIB_
lenv* lenv_new(void);

void lenv_del(lenv* e);

lval* lenv_get(lenv* e, lval* k);

void lenv_put(lenv* e, lval* k, lval* v);

lenv* lenv_copy(lenv* e);

void lenv_def(lenv* e, lval* k, lval* v);