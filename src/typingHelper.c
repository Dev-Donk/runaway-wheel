
#include "typingHelper.h"

#include <stdlib.h>
#include <string.h>

#include "raylib.h"

int list_load_from_text_file(const char *file, prompt_entry *list_head)
{
    if(!FileExists(file)) {
        printf("\"%s\" does not exist.", file);
        exit(-1);
    }
    
    int list_size       = 0;
    char *contents      = LoadFileText(file);
    char *token         = strtok(contents, "\n\0");
    prompt_entry *prev;

    list_head->data          = token;
    list_head->data_length   = strlen(token);
    list_head->next          = NULL;

    prev = list_head;

    while(token != NULL) 
    {
        // printf("ADDED TOKEN: %s\n", token);
        token = strtok(NULL, "\n");

        if(token == NULL) {
            continue;
        }

        prompt_entry *new_entry = (prompt_entry*)malloc(sizeof(prompt_entry));

        if(new_entry == NULL)
        {
            printf("Unable to create new prompt entry.\n");
            exit(-1);
        }

        new_entry->data         = token;
        new_entry->data_length  = strlen(token);
        new_entry->next         = NULL;

        prev->next  = new_entry;
        prev        = new_entry;

        list_size++;

    }

    return list_size;
}

void list_print(prompt_entry *list) 
{
    prompt_entry *curr = list;

    while(curr != NULL)
    {
        printf("Entry: \"%-10.10s\" | Size: %-d\n", curr->data, curr->data_length);
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

prompt_entry* list_get_random_entry(prompt_entry *list, int list_size)
{
    prompt_entry *ret   = list;
    int max_count       = GetRandomValue(0, list_size);

    for(int i = 0; i < max_count; i++)
    {
        // Leave at end of list, incase list_size exceeds actual size of list
        if(ret->next == NULL)
        {
            break;
        }

        ret = (prompt_entry*)ret->next;

    }

    return ret;
    
}