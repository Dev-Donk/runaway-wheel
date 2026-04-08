#ifndef TYPING_HELPER_H
#define TYPING_HELPER_H

#include <stdio.h>

#define MAX_TOKEN_SIZE 45

typedef struct {
    struct prompt_entry *head;
    size_t              size;
} prompt_list;

// Would want this to be private for security reasons,
// but lazy and returning entry would be annoying without
typedef struct {
    struct prompt_entry *next;
    size_t              data_length;
    char                *data;
} prompt_entry;

// Keep in mind amount of temrinators, different with linux and junk
void list_load_from_text_file(prompt_list *list, const char *file);
void list_free(prompt_list *list);
void list_print(const prompt_list *list);
prompt_entry* list_get_random_entry(const prompt_list *list);


#endif