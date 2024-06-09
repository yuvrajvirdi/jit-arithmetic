#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *input;
    size_t pos;
} Lexer;

typedef enum { NUM, ADD, SUB, MUL, DIV } Op;

typedef struct Expr {
    Op op;
    union {
        long num;
        struct {
            struct Expr *left;
            struct Expr *right;
        };
    };
} Expr;

Lexer new_lexer(char *input) {
    Lexer lexer;
    lexer.input = input;
    lexer.pos = 0;
    return lexer;
}

char peek(Lexer *lexer) {
    return lexer->input[lexer->pos];
}

char next(Lexer *lexer) {
    return lexer->input[lexer->pos++];
}

Expr *parse_expr(Lexer *lexer);

Expr *parse_num(Lexer *lexer) {
    long num = 0;
    while (peek(lexer) >= '0' && peek(lexer) <= '9') {
        num = num * 10 + (next(lexer) - '0');
    }  
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->op = NUM;
    expr->num = num;
    return expr;
}

Expr *parse_factor(Lexer *lexer) {
    if (peek(lexer) == '(') {
        next(lexer);
        Expr *expr = parse_expr(lexer);
        if (next(lexer) != ')') {
            fprintf(stderr, "expected ')'\n");
            exit(EXIT_FAILURE);
        }
        return expr;
    } else {
        return parse_num(lexer);
    }
}

Expr *parse_term(Lexer *lexer) {
    Expr *expr = parse_factor(lexer);
    while (peek(lexer) == '*' || peek(lexer) == '/') {
        Op op = next(lexer) == '*' ? MUL : DIV;
        Expr *right = parse_factor(lexer);
        Expr *new_expr = (Expr *)malloc(sizeof(Expr));
        new_expr->op = op;
        new_expr->left = expr;
        new_expr->right = right;
        expr = new_expr;
    }
    return expr;
}

Expr *parse_expr(Lexer *lexer) {
    Expr *expr = parse_term(lexer);
    while (peek(lexer) == '+' || peek(lexer) == '-') {
        Op op = next(lexer) == '+' ? ADD : SUB;
        Expr *right = parse_term(lexer);
        Expr *new_expr = (Expr *)malloc(sizeof(Expr));
        new_expr->op = op;
        new_expr->left = expr;
        new_expr->right = right;
        expr = new_expr;
    }
    return expr;
}

long evaluate(Expr *expr) {
    switch (expr->op) {
        case NUM:
            return expr->num;
        case ADD:
            return evaluate(expr->left) + evaluate(expr->right);
        case SUB:
            return evaluate(expr->left) - evaluate(expr->right);
        case MUL:
            return evaluate(expr->left) * evaluate(expr->right);
        case DIV:
            return evaluate(expr->left) / evaluate(expr->right);
        default:
            fprintf(stderr, "Unknown operation\n");
            exit(EXIT_FAILURE);
    }
}

void free_expr(Expr *expr) {
    if (expr == NULL) {
        return;
    }
    if (expr->op != NUM) {
        free_expr(expr->left);
        free_expr(expr->right);
    }
    free(expr);
}

long jit_compile_and_execute(const char *expr_str) {
    Lexer lexer = new_lexer((char *)expr_str);
    Expr *expr = parse_expr(&lexer);
    long result = evaluate(expr);
    free_expr(expr);
    return result;
}