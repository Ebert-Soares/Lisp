#include "mpc.h"

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

long evaluation(mpc_ast_t* t){
  if(strstr(t->tag, "number")){
    return atoi(t->contents);
  }
  char* op = t->children[1]->contents;
  long x = evaluation(t->children[2]);
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")){
    x = eval_operation(x, op, evaluation(t->children[i]));
    i++;
  }
}

long eval_operation(long x, char* op, long y){
  if(strcmp(op, "+") == 0){return x + y;}
  if(strcmp(op, "-") == 0){return x - y;}
  if(strcmp(op, "*") == 0){return x * y;}
  if(strcmp(op, "/") == 0){return x / y};

}

int main(int argc, char** argv){

  mpc_parser_t* Number  = mpc_new("number");
  mpc_parser_t* Operator  = mpc_new("operator");
  mpc_parser_t* Expr  = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                               \
    number  : /-?[0-9]+/;                           \
    operator  : '+'  | '-' | '*' | '/' ;            \
    expr  : <number> | '(' <operator> <expr>+ ')' ; \
    lispy : /^/ <operator> <expr>+ /$/;             \
    ",
    Number, Operator, Expr, Lispy);

    printf("Lispy Version 0.0.0.0.2\n");
    printf("Press Ctrl+c to Exit\n");

while (1){
  char* input = readline("lispy> ");
  add_history(input);

  mpc_result_t r;
  if(mpc_parse("<stdin>", input, Lispy, &r)){
    long result = evaluation(r.output);
    printf("%li\n", result);
    mpc_ast_delete(r.output);
  }else{
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

free(input);
}




mpc_cleanup(4, Number, Operator, Expr, Lispy);

return 0;

}
