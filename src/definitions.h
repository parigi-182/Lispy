#ifndef _DEFINITIONS_
 #define _DEFINITIONS_
#endif
#ifndef mpc_h
  #include "mpc.h"
#endif

extern mpc_parser_t* Lispy;

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);


struct lval {
  int type;           // Type of the value
  long num;           // Numeric value
  char* err;          // Error message  
  char* sym;          // Symbol name
  char* str;          // String value
  
  lbuiltin builtin;   // Builtin function pointer
  lenv* env;          // Enviroment for functions
  lval* formals;      // Formal arguments for user defind function
  lval* body;         // Function body for user defined function
  int count;          // Number of sub expression (S/Q)
  lval** cell;        // Array of sub expression (S/Q)

};

struct lenv {
  lenv* par;          // Parent environment (for scooping)
  int count;          // Number of symbol defined
  char** syms;        // Array of symbol names
  lval** vals;        // Arry of values corresponding to symbols
};

/* Types and constants for the interpreter */
enum
{
  LVAL_ERR,           // Error type
  LVAL_NUM,           // Number type
  LVAL_SYM,           // Symbol type
  LVAL_FUN,           // Function type
  LVAL_SEXPR,         // S-expression type
  LVAL_QEXPR,         // Q-expression type
  LVAL_STR            // String type
};


char* ltype_name(int t);

#ifndef _LVAL_LIB_
 #include"lval_lib.h"
#endif


/* Assert macros for argument validation  */

/* Validate a condition */
#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }

/* Validate argument type */
#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

/* Validate argument number */
#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

/* Validate non empty Q-expression */
#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);
