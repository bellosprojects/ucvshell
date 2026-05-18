#include <stdio.h>
#include <string.h>

// Función para eliminar espacios al inicio

int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

void trim(char *s) {
    if (s == NULL) return;

    int size = strlen(s);
    if (size == 0) return;

    // 1. Buscar el primer carácter que no sea espacio (ltrim)
    int start = 0;
    while (start < size && is_space(s[start])) {
        start++;
    }

    // 2. Buscar el último carácter que no sea espacio (rtrim)
    int end = size;
    while (end > start && is_space(s[end-1])) {
        end--;
    }

    // 3. Desplazar los caracteres hacia el inicio del array original
    int i;
    for (i = 0; start < end; i++, start++) {
        s[i] = s[start];
    }
    s[i] = '\0'; // Cerrar la string de forma segura

    return;
}