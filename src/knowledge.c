/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * knowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "chat1002.h"

// global vars
EntityNode *head;
EntityNode *tail;

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {
	if (!chatbot_is_question(intent)){
	    return KB_INVALID;
	}
	// valid question
	EntityNode *current;
	current = head;
	int found = 0;
	// traverse and search linked-list for
	while (current != NULL){
        if (compare_token(current->entity,entity) == 0){
            found = 1;
            break;
        }
        current = current->next;
	}
	if (found) {
        // check if intent has corresponding response
        if (compare_token(intent, "what") == 0) {
            // process what
            if (current->what[0] != '\0') {
                strncpy(response,current->what,n);
                return KB_OK;
            }
        }
        else if (compare_token(intent, "where") == 0) {
            // process where
            if (current->where[0] != '\0') {
                strncpy(response,current->where,n);
                return KB_OK;
            }
        }
        else {
            // process who
            if (current->who[0] != '\0') {
                strncpy(response,current->who,n);
                return KB_OK;
            }
        }
    }
	return KB_NOTFOUND;
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_FOUND, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {
	// invalid question word
	if(!chatbot_is_question(intent)){
	    return KB_INVALID;
	}
	EntityNode *current;
	current = head;
	int entityFound = 0;
	// existing data structure
    if(current != NULL){
        // iterate linked-list
        while(current != NULL){
            if (compare_token(current->entity,entity) == 0){
                entityFound = 1;
                break;
            }
            // current node is not target, move on
            current = current->next;
        }
    }
    // target entity does not exist, create one and add to linked-list
    if (!entityFound){
        // allocate memory to prevent unexpected behaviour
        EntityNode *target = calloc(1,sizeof(EntityNode));
        if (target == NULL){
            return KB_NOMEM;
        }
        strcpy(target->entity,entity);
        memset(target->what,0,MAX_RESPONSE);
        memset(target->where,0,MAX_RESPONSE);
        memset(target->who,0,MAX_RESPONSE);
        target->next = NULL;
        current = target;
        // check if current node is first node in linked-list
        if (head == NULL){
            head = target;
            tail = target;
        }
        // if not first node and entity not found, create and add to linked-list
        else{
            tail->next = target;
            tail = target;
        }
    }
    // check and set response
    if(compare_token(intent,"what") == 0){
        // process what
        strcpy(current->what,response);
    }
    else if (compare_token(intent, "where") == 0){
        // process where
        strcpy(current->where,response);
    }
    else{
        // process who
        strcpy(current->who,response);
    }
    return KB_OK;
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
    if(f == NULL){
        return F_INVALID;
    }
    int count = 0;
    char entitybuf[MAX_ENTITY];
    char responsebuf[MAX_RESPONSE];
    char *tokenptr;
    // each line has maximum one entity and one response plus a =
    char line[MAX_RESPONSE + MAX_ENTITY + 1];
    char intentkey[MAX_INTENT];
    while(fgets(line,sizeof line, f) != NULL) {
        if (line[0] == '[') {
            // process section heading
            char *tmp = strchr(line, ']');
            int length = tmp - line;
            strncpy(intentkey, line + 1, length);
            intentkey[length - 1] = '\0';
            continue;
        }
        // check if current line is new line
        if (isspace(line[0])) {
            continue;
        }
        tokenptr = strtok(line, "=");
        strncpy(entitybuf, tokenptr,MAX_ENTITY);
        tokenptr = strtok(NULL, "=");
        if (tokenptr == NULL){
            return F_INVALID;
        }
        // replace newline with null to prevent double newline when write
        *strchr(tokenptr,'\n') = '\0';
        strncpy(responsebuf, tokenptr,MAX_RESPONSE);
        int success = knowledge_put(intentkey, entitybuf, responsebuf);
        if (success != KB_OK) {
            return success;
        }
        // clear buffer
        memset(entitybuf, 0, sizeof entitybuf);
        memset(responsebuf, 0, sizeof entitybuf);
        count++;
    }
    return count;
}

/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	// free all nodes in linked-list and reset head & tail
	EntityNode *current = head;
    EntityNode *next;
	while (current != NULL){
	   next = current->next;
	   memset(current,0, sizeof(EntityNode));
	   free(current);
	   current = next;
	}
	head = NULL;
	tail = NULL;
}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {
	EntityNode *current = head;
    fprintf(f,"[what]\n");
    // traverse linked-list to print for what
    while (current != NULL){
        // node has response for what
        if (current->what[0] != '\0'){
            // no need \n as response alr has \n
            fprintf(f,"%s=%s\n",current->entity,current->what);
        }
        current = current->next;
    }
    // reset current to start of linked-list
    current = head;
    // \n at start to create visual spacing between sections
    fprintf(f,"\n[where]\n");
    // traverse linked-list to print for where
    while (current != NULL){
        // node has response for where
        if (current->where[0] != '\0'){
            // no need \n as response alr has \n
            fprintf(f,"%s=%s\n",current->entity,current->where);
        }
        current = current->next;
    }
    // reset current to start of linked-list
    current = head;
    // \n at start to create visual spacing between sections
    fprintf(f,"\n[who]\n");
    // traverse linked-list to print for who
    while (current != NULL){
        // node has response for what
        if (current->who[0] != '\0'){
            // no need \n as response alr has \n
            fprintf(f,"%s=%s\n",current->entity,current->who);
        }
        current = current->next;
    }
    // fclose to be handled by caller function
}
