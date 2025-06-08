#include"definitions.h"
#include"core.h"
#include"lenv_lib.h"
#include"lval_lib.h"
#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

mpc_parser_t* Lispy;

int main(int argc, char** argv) {
  
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr  = mpc_new("sexpr");
  mpc_parser_t* Qexpr  = mpc_new("qexpr");
  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* String = mpc_new("string");
  mpc_parser_t* Comment = mpc_new("comment");
  Lispy  = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,                            // Deined grammar
    "                                                     \
      number : /-?[0-9]+/ ;                               \
      symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
      sexpr  : '(' <expr>* ')' ;                          \
      qexpr  : '{' <expr>* '}' ;                          \
      expr   : <number> | <symbol> | <sexpr> | <qexpr> | <string> | <comment> ; \
      string : /\"(\\\\.|[^\"])*\"/ ;                     \
      comment : /;[^\\r\\n]*/ ;                           \
      lispy  : /^/ <expr>* /$/ ;                          \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, String, Comment, Lispy);
  
  puts("Lispy Version 0.0.0.0.7");
  puts("Press Ctrl+c to Exit\n");
  
  /* Global environment and provided builtins */
  lenv* e = lenv_new();
  lenv_add_builtins(e);
  
  
  switch (argc)
  {
    case 1:
      while (1) {                                        // Read, eval and print loop
      
        char* input = readline("lispy> ");
        add_history(input);
        
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
          lval* x = lval_eval(e, lval_read(r.output));
          lval_println(x);
          lval_del(x);
          mpc_ast_delete(r.output);
        } else {    
          mpc_err_print(r.error);
          mpc_err_delete(r.error);
        }
        
        free(input);
        
      }
      break;
    case 2:                                              // Execute instructions inside the provied (as argument) file
      for (int i = 1; i < argc; i++) {
      lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));
      lval* x = builtin_load(e, args);
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }
      break;
      
    default:
      puts("Argc number error.");
      puts("Shutting down.");
      break;
  }
  
  lenv_del(e);
  
  mpc_cleanup(8, Number, Symbol, Sexpr, Qexpr, Expr, String, Comment, Lispy);
  
  puts("Exiting.");
  return 0;
}