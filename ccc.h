typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQL,
    ND_NEQL,
    ND_LOW,
    ND_LOW_EQL,
}NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;  // node type
    Node *lhs;      // Left Node
    Node *rhs;      // Right Node
    uint32_t val;   // use if kind is ND_NUM
};

typedef enum{
    TK_RESERVED,    // operator
    TK_NUM,         // integer token
    TK_EOF,         // end of file
} TokenKind;

typedef struct Token Token;
struct Token{
    TokenKind kind; // token type
    Token *next;    // next token
    uint32_t val;   // integer if kind is number
    char *str;      // string if kind is operator
    size_t len;     // operation length
};

extern Token *token;

extern char *user_input;

Token *tokenize(char *p);

Node *expr(void);

void gen(Node *node);

void error_at(char *loc, char *fmt, ...);
