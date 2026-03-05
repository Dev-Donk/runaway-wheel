
#include "typingHelper.h"

#include <stdlib.h>
#include <string.h>

#include "raylib.h"

void list_load_from_text_file(prompt_list *list, const char *file)
{
    if(!FileExists(file)) { // This relies on set resource directory in include
        printf("\"%s\" does not exist.", file);
        exit(-1);
    }
    
    size_t list_size_count  = 0;
    char *contents          = LoadFileText(file);
    char *token             = strtok(contents, "\n\0");
    prompt_entry *prev      = NULL;

    while(token != NULL) 
    {

        if(token == NULL) 
        {
            break;
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

        if(list->head == NULL)
        {
            list->head = (struct prompt_entry*)new_entry;
        }
        
        // Could probably be improved around here, but works for now
        if(prev != NULL)
        {
            prev->next  = (struct prompt_entry*)new_entry;
        }

        // printf("ADDED TOKEN: %s\n", token);

        prev = new_entry;
        list_size_count++;

        token = strtok(NULL, "\n");
    }

    list->size = list_size_count;
}

void list_print(prompt_list *list) 
{
    prompt_entry *curr = (prompt_entry*)list->head;

    for (size_t i = 0; i < list->size || curr != NULL; i++)
    {
        printf("Entry [%3d/%3d]: \"%-10.10s\" | Size: %-d\n", i + 1, list->size, curr->data, curr->data_length);
        curr = (prompt_entry*)curr->next;
    }
    
}

void list_free(prompt_list *list)
{
    prompt_entry *curr      = (prompt_entry*)list->head;
    prompt_entry *next      = NULL;

    while(curr != NULL)
    {
        next = (prompt_entry*)curr->next;

        // printf("Unloaded: %-10.10s\n", curr->data);
        free(curr);

        curr = next;
    }

    free(list); // Assuming if malloced
}

prompt_entry* list_get_random_entry(prompt_list *list)
{
    prompt_entry *ret   = (prompt_entry*)list->head;
    int max_count       = GetRandomValue(0, list->size);

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