#ifndef TYPING_HELPER_H
#define TYPING_HELPER_H

#include <stdio.h>

typedef enum 
{
    PROMPT_TYPING,
    PROMPT_SUCCESS,
    PROMPT_FAIL
} prompt_state;

// Something that gets current word/phrase
// Get current word/phrase length
// Something that loads words/phrases into array. Dynamic size
void load_text_from_file_to_arr(FILE *file, char **arr);


#endif