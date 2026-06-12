#include <stdio.h>
#include <string.h>

int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

void trim(char *s) {
    if (s == NULL) return;

    int size = strlen(s);
    if (size == 0) return;

    int start = 0;
    while (start < size && is_space(s[start])) {
        start++;
    }

    int end = size;
    while (end > start && is_space(s[end-1])) {
        end--;
    }

    int i;
    for (i = 0; start < end; i++, start++) {
        s[i] = s[start];
    }
    s[i] = '\0';

    return;
}