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
    char *data;
    int  data_length;
    struct prompt_entry *next;
} prompt_entry;

// Something that gets current word/phrase
// Get current word/phrase length
// Something that loads words/phrases into array. Dynamic size
// Keep in mind amount of temrinators, different with linux and junk
void list_load_from_text_file(const char *file, prompt_entry *head);

void list_print(prompt_entry *list);

void list_free(prompt_entry *list);


#endif