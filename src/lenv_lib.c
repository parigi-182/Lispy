#ifndef _LENV_LIB_
 #include"lenv_lib.h"
#endif
#include<stdlib.h>
#include<string.h>

/* "Constructor" for a new environment */
lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->par = NULL;
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
  
}

/* Frees an environment and all its contents */
void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

/* Search a symbol in the provided environment or in its father (recursive method) */
lval* lenv_get(lenv* e, lval* k) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }

  if(e->par){
    return lenv_get(e->par, k);
  }else{
    return lval_err("Unbound Symbol '%s'", k->sym);
  }
}


/* Define or update the provided symbol in the provided environment */
void lenv_put(lenv* e, lval* k, lval* v) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);
  
  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}

/* Returns a copy (pointer to) of the provided environment */
lenv* lenv_copy(lenv* e){
  lenv* n = malloc(sizeof(lenv));
  n->par = e->par;
  n->count = e->count;
  n->syms = malloc(sizeof(char*) * n->count);
  n->vals = malloc(sizeof(lval*) * n->count);
  for (int i = 0; i < e->count; i++)
  {
    n->syms[i] = malloc(strlen(e->syms[i])+1);
    strcpy(n->syms[i], e->syms[i]);
    n->vals[i] = lval_copy(e->vals[i]);
  }
  return n;
}

/* Defines the provided symbol in the global environement */
void lenv_def(lenv* e, lval* k, lval* v){
  while (e->par) { e = e->par; }
  lenv_put(e, k, v); 
}