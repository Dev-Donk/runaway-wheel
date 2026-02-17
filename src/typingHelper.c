
#include "typingHelper.h"

unsigned int max_words;
unsigned int max_word_length;
char **words;

unsigned int max_phrases;
unsigned int max_phrase_length;
char **phrases;

void load_text_from_file_to_arr(FILE *file, char **arr)
{
    // Count all entries first, store into temp, then copy?
    unsigned int entry_counter = 0;

    // Gonna need to parse by '\n', do not want any character to be part of parse
}