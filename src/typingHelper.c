
#include "typingHelper.h"

#include <stdlib.h>
#include <string.h>

#include "raylib.h"

void list_load_from_text_file(const char *file, prompt_entry *head)
{
    if(!FileExists(file)) {
        printf("\"%s\" does not exist.", file);
        exit(-1);
    }
    
    char *contents = LoadFileText(file);
    char *token;

    token = strtok(contents, "\n");

    head->data = token;
    head->data_length = strlen(token);
    head->next = NULL;

    prompt_entry *prev = head;

    while(token != NULL) 
    {
        // printf("ADDED TOKEN: %s\n", token);
        token = strtok(NULL, "\n");

        if(token == NULL) {
            continue;
        }

        prompt_entry *new_entry = (prompt_entry*)malloc(sizeof(prompt_entry));
        new_entry->data         = token;
        new_entry->data_length  = strlen(token);
        new_entry->next = NULL;

        prev->next = new_entry;

        prev = new_entry;

    }
}

void list_print(prompt_entry *list) 
{
    prompt_entry *curr = list;
    while(curr != NULL)
    {
        printf("%-10.10s | %-d\n", curr->data, curr->data_length);
        curr = (prompt_entry*)curr->next;
    }
}

void list_free(prompt_entry *list)
{
    prompt_entry *curr = list;
    prompt_entry *next = NULL;

    while(curr != NULL)
    {
        next = (prompt_entry*)curr->next;

        // printf("Unloaded: %-10.10s\n", curr->data);
        free(curr);

        curr = next;
    }
}