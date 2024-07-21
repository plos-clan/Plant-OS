#pragma once
#include <define.h>
#include <type.h>
size_t strlen(const char *s);
char  *strcpy(char *dest, const char *src);
void   insert_char(char *str, int pos, char ch);
void   delete_char(char *str, int pos);
int    strcmp(const char *s1, const char *s2);
char  *strcat(char *dest, const char *src);
int    isspace(int c);
int    isalpha(int c);
int    isdigit(int c);
int    isupper(int c);
void   strtoupper(char *str);
int    strncmp(const char *s1, const char *s2, size_t n);
char  *strchr(const char *s, int c);