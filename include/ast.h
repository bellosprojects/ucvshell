#ifndef AST_H
#define AST_H

/// @brief Identificador del tipo de nodo (Simple o Conector)
typedef enum {

    NODE_COMMAND,
    NODE_PIPE,
    NODE_AND,
    NODE_OR,
    NODE_SEMICOLON,
    UNKNOW_NODE

} node_type_t;

/// @brief Especificaciones del comando
typedef struct command {

    char **argv;
    char *input_file;
    char *output_file;
    int append_output;
    int background;

} command_t;

/// @brief Estructura del arbol y sus conecciones
typedef struct ast_node {

    node_type_t type;

    union {

        command_t *cmd;

        struct {
            
            struct ast_node *left;
            struct ast_node *right;

        } op;

    } data;

} ast_node_t;

/// @brief Token que representa un fragmento de la linea
typedef struct token {

    int type; // 0 texto generico | 1 operador
    char *value;

} token_t;

/// @brief Crea un comando con las especificaciones
/// @param argv La lista de comandos
/// @param input El archivo de entrada
/// @param output El archivo de salida
/// @param append Sobreescribir o agregar el final
/// @param gb 1 si es asincrono o 0 si es sincrono
/// @return Apuntador al comando
/// @warning El resultado se asigna con malloc. Es responsabilidad del caller liberarlo con free
command_t *crear_comando(char **argv, char *input, char* output, int append, int bg);

/// @brief Crea un nodo de tipo comando con las especificacion
/// @param cmd El apuntador al comando
/// @return Un apuntador al nodo
/// @warning El resultado se asigna con malloc. Es responsabilidad del caller liberarlo con free
ast_node_t* crear_nodo_comando(command_t* cmd);


/// @brief Crea un comando de tipo operador (binario)
/// @param type El tipo de comando (AND, PIPI, ETC...)
/// @param left Apuntador al nodo izquierdo
/// @param right Apuntador al nodo derecho
/// @return Apuntador el nodo operador
/// @warning El resultado se asigna con malloc. Es responsabilidad del caller liberarlo con free
ast_node_t* crear_nodo_operador(node_type_t type, ast_node_t *left, ast_node_t* right);

/// @brief Libera de forma segura un comando;
/// @param comando Apuntador al comando a liberar
void liberar_comando(command_t* comando);

/// @brief Libera de forma segura la memoria asignada a un nodo de forma recursiva
/// @param node El nodo a liberar (junto con todos sus hijos)
void liberar_ast(ast_node_t *nodo);

/// @brief Crea un Token para construir el AST
/// @param type Tipo del token
/// @param value Valor de token
/// @return Apuntador al token
/// @warning El resultado se asigna con malloc. Es responsabilidad del caller liberarlo con free
token_t *crear_token(int type, char*value);

/// @brief Libera de forma segura un token
/// @param token El apuntador al token a liberar
void liberar_token(token_t *token);

#endif