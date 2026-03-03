#ifndef TYPING_HELPER_H
#define TYPING_HELPER_H

#include <stdio.h>

typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL
} prompt_state;

typedef struct {
    struct prompt_entry *next;
    int                 data_length;
    char                *data;
} prompt_entry;

// Keep in mind amount of temrinators, different with linux and junk
int list_load_from_text_file(const char *file, prompt_entry *list_head);

void list_print(prompt_entry *list);

void list_free(prompt_entry *list);

prompt_entry* list_get_random_entry(prompt_entry *list, int list_size);


#endif