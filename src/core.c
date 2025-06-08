#ifndef _CORE_
 #include"core.h"
#endif
#ifndef _LVAL_LIB_
 #include"lval_lib.h"
#endif
#ifndef _LENV_LIB_
 #include"lenv_lib.h"
#endif
#pragma GCC diagnostic ignored "-Wunused-parameter"

char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    case LVAL_STR: return "String";
    default: return "Unknown";
  }
}

/* Builtin methods of the language */
lval* lval_lambda(lval* formals, lval* body){
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->builtin = NULL;
  v->env = lenv_new();
  v->formals = formals;
  v->body = body;
  return v;
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lenv* e, lval* a) {
  LASSERT_NUM("head", a, 1);
  LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("head", a, 0);
  
  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LASSERT_NUM("tail", a, 1);
  LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("tail", a, 0);

  lval* v = lval_take(a, 0);  
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_eval(lenv* e, lval* a) {
  LASSERT_NUM("eval", a, 1);
  LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  
  for (int i = 0; i < a->count; i++) {
    LASSERT_TYPE("join", a, i, LVAL_QEXPR);
  }
  
  lval* x = lval_pop(a, 0);
  
  while (a->count) {
    lval* y = lval_pop(a, 0);
    x = lval_join(x, y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  
  for (int i = 0; i < a->count; i++) {
    LASSERT_TYPE(op, a, i, LVAL_NUM);
  }
  
  lval* x = lval_pop(a, 0);
  
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }
  
  while (a->count > 0) {  
    lval* y = lval_pop(a, 0);
    
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->num /= y->num;
    }
    
    lval_del(y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_gt(lenv* e, lval* a){
  return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a){
  return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a){
  return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a){
  return builtin_ord(e, a, "<=");
}

lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_def(lenv* e, lval* a) {
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}

lval* builtin_var(lenv* e, lval* a, char* func){
  LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

  lval* syms = a->cell[0];
  for (int i = 0; i < syms->count; i++)
  {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
    "Function '%s' cannot define non symbol. Got %s, expected %s.", func, ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }

  LASSERT(a, (syms->count == a->count-1),
    "Function '%s' passed too many arguments for symbols. Got %i, expected %i.", func, syms->count, a->count-1);
  for (int i = 0; i < syms->count; i++)
  {
    if(strcmp(func, "def") == 0){
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }
    
    if(strcmp(func, "=") == 0){
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }
  lval_del(a);
  return lval_sexpr();
}

lval* builtin_lambda(lenv* e, lval* a){
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  for (int i = 0; i < a->cell[0]->count; i++)
  {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
      "Cannot define non symbol. Got %s, Expected %s.",
      ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
  }
  
  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval* builtin_ord(lenv* e, lval* a, char* op){
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);

  int r;
  if (strcmp(op, ">")  == 0) {
    r = (a->cell[0]->num >  a->cell[1]->num);
  }
  if (strcmp(op, "<")  == 0) {
    r = (a->cell[0]->num <  a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }
  lval_del(a);
  return lval_num(r);
}

int lval_eq(lval* x, lval* y){
  if(x->type != y->type){ return 0; }

  switch (x->type)
  {
    case LVAL_NUM: return (x->num == y->num);
      break;
    case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
      break;
    case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
      break;
    case LVAL_FUN: 
      if(x->builtin || y->builtin){
        return (x->builtin == y->builtin);
      }else{
        return (lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body));
      }
      break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      if(x->count != y->count){ return 0; }
      for (int i = 0; i < x->count; i++)
      {
        if(!lval_eq(x->cell[i], y->cell[i])){ return 0; }
      }
      return 1;
      break;
    case LVAL_STR: return (strcmp(x->str, y->str)==0);
      break;
    default:
      break;
  }
  return 0;
}

lval* builtin_if(lenv* e, lval* a){
  LASSERT_NUM("if", a, 3);
  LASSERT_TYPE("if", a, 0, LVAL_NUM);
  LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

  lval* x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;

  if(a->cell[0]->num){
    x = lval_eval(e, lval_pop(a,1));
  }else{
    x = lval_eval(e, lval_pop(a, 2));
  }
  lval_del(a);
  return x;
}

lval* builtin_cmp(lenv* e, lval* a, char* op){
  LASSERT_NUM(op, a, 2);

  int r;
  if(strcmp(op, "==")==0){
    r = lval_eq(a->cell[0], a->cell[1]);
  }
  if(strcmp(op, "!=")==0){
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a){
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a){
  return builtin_cmp(e, a, "!=");
}

lval* builtin_load(lenv* e, lval* a){
  LASSERT_NUM("load", a, 1);
  LASSERT_TYPE("load", a, 0, LVAL_STR);
  mpc_result_t r;
  if(mpc_parse_contents(a->cell[0]->str, Lispy, &r)){
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while (expr->count)
    {
      lval* x = lval_eval(e, lval_pop(expr, 0));
      lval_println(x);
      lval_del(x);
    }
    
    lval_del(expr);
    lval_del(a);

    return lval_sexpr();
  }else{
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    lval* err = lval_err("Could not load Library $s.", err_msg);
    free(err_msg);
    lval_del(a);
    return err;
  }
}
void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
} 




void lenv_add_builtins(lenv* e) {
  lenv_add_builtin(e, "def", builtin_def);
  
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "\\", builtin_lambda);
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=",   builtin_put);
  
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "if", builtin_if);
  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);
  lenv_add_builtin(e, ">",  builtin_gt);
  lenv_add_builtin(e, "<",  builtin_lt);
  lenv_add_builtin(e, ">=", builtin_ge);
  lenv_add_builtin(e, "<=", builtin_le);

  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "print", builtin_print);
  lenv_add_builtin(e, "load", builtin_load);
}

/* Calls the provided function and arguments*/
lval* lval_call(lenv* e, lval* f, lval* a){

  if(f->builtin){ return f->builtin(e, a); }           // Builtins function are immediatly resolved
  int given = a->count;
  int total = f->formals->count;


  while (a->count)
  {
    if(f->formals->count == 0){
      lval_del(a);
      return lval_err("Function passed too many arguments, Got %i, expected %i", given, total);
    }

    lval* sym = lval_pop(f->formals, 0);
    if(strcmp(sym->sym, "&") == 0){
      if(f->formals->count != 1){
        lval_del(a);
        return lval_err("Function format invalid. Symbol '&' not followed by single symbol.");
      }
      lval* nsym = lval_pop(f->formals,0);
      lenv_put(f->env, nsym, builtin_list(e, a));
      lval_del(sym);
      lval_del(nsym);
      break;
    }
    lval* val = lval_pop(a, 0);
    lenv_put(f->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }
  lval_del(a);


  if(f->formals->count > 0 && strcmp(f->formals->cell[0]->sym, "&") == 0){
    if(f->formals->count != 2){
      return lval_err("Function format invalid. Symbol '&' not followed by single symbol.");
    }
    lval_del(lval_pop(f->formals, 0));
    lval* sym = lval_pop(f->formals, 0);
    lval* val = lval_qexpr();
    lenv_put(f->env, sym, val);
    lval_del(sym);
    lval_del(val);
  }

  if(f->formals->count == 0){                        // Evaluate the function body in its environment
    f->env->par = e;
    return builtin_eval(f->env, lval_add(lval_sexpr(), lval_copy(f->body)));
  }else{
    return lval_copy(f);
  }

}

/* Evaluates the provided S-Expression */
lval* lval_eval_sexpr(lenv* e, lval* v) {
  
  for (int i = 0; i < v->count; i++) {                // Evaluates all children first
    v->cell[i] = lval_eval(e, v->cell[i]);
  }
  
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }
  
  if (v->count == 0) { return v; }  
  if (v->count == 1) { return lval_eval(e, lval_take(v, 0)); }
  
  lval* f = lval_pop(v, 0);                           // If not a single or empty expression, first element must be a function
  if (f->type != LVAL_FUN) {
    lval* err = lval_err( "S-Expression starts with incorrect type." "Got %s, Expected %s.",
      ltype_name(f->type),
      ltype_name(LVAL_FUN));
    
    lval_del(f);
    lval_del(v);
    return err;
  }
  
  lval* result = lval_call(e, f, v);
  lval_del(f);
  return result;
}

/* Evaluate the provided lval variables in the provided enrivonment */
lval* lval_eval(lenv* e, lval* v) {
  if (v->type == LVAL_SYM) {
    lval* x = lenv_get(e, v);                       // Look up symbol in environment
    lval_del(v);
    return x;
  }
  if (v->type == LVAL_SEXPR) {
    return lval_eval_sexpr(e, v);                   // Evaluate S-Expression
  }

  return v;                                         // All other types evaluate to themselves
}

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("Invalid Number.");
}

lval* lval_read_str(mpc_ast_t* t) {
  t->contents[strlen(t->contents)-1] = '\0';
  char* unescaped = malloc(strlen(t->contents+1)+1);
  strcpy(unescaped, t->contents+1);
  unescaped = mpcf_unescape(unescaped);
  lval* str = lval_str(unescaped);
  free(unescaped);
  return str;
}

lval* lval_read(mpc_ast_t* t) {
  
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
  if (strstr(t->tag, "string")) { return lval_read_str(t); }
  
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); } 
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }
  
  for (int i = 0; i < t->children_num; i++) {
    if (strstr(t->children[i]->tag, "comment")) { continue; }
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }
  
  return x;
}



lval* builtin_print(lenv* e, lval* a) {

  for (int i = 0; i < a->count; i++) {
    lval_print(a->cell[i]); putchar(' ');
  }
  putchar('\n');
  lval_del(a);

  return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a) {
  LASSERT_NUM("error", a, 1);
  LASSERT_TYPE("error", a, 0, LVAL_STR);
  lval* err = lval_err(a->cell[0]->str);
  lval_del(a);
  return err;
}