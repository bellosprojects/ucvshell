#ifndef PATH_UTILS_H
#define PATH_UTILS_H

/**
 * Busca la ruta absoluta de un comando en las carpetas del $PATH
 * 
 * @param command Nombre del comando (ej: "ls", "gcc", "cat")
 * @return String con la ruta absoluta (ej: "/bin/ls") 
 *          o NULL si no encuentra el comando.
 * 
 * @warning El string retornado es asignado con malloc(), el caller
 * debe liberarlo con free() cuando ya no lo necesite.
 */
char* find_command_path(const char* command);

#endif