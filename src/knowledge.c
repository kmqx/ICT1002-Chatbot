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
struct EntityLL *head;
struct EntityLL *tail;

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
	/* to be implemented */
	if (!chatbot_is_question(intent)){
	    return KB_INVALID;
	}
	// valid question
	struct EntityLL *current;
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
                response = current->what;
                return KB_OK;
            }
        }
        else if (compare_token(intent, "where") == 0) {
            // process where
            if (current->where[0] != '\0') {
                response = current->where;
                return KB_OK;
            }
        }
        else {
            // process who
            if (current->who[0] != '\0') {
                response = current->who;
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

	/* to be implemented */
	// invalid question word
	if(!chatbot_is_question(intent)){
	    return KB_INVALID;
	}
	struct EntityLL *current;
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
        struct EntityLL *target = calloc(1,sizeof(struct EntityLL));
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
	/* to be implemented */
    if(f == NULL){
        return -1;
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
        strcpy(entitybuf, tokenptr);
        tokenptr = strtok(NULL, "=");
        strcpy(responsebuf, tokenptr);
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
// debug
//int compare_token(const char *token1, const char *token2) {

    int i = 0;
    while (token1[i] != '\0' && token2[i] != '\0') {
        if (toupper(token1[i]) < toupper(token2[i]))
            return -1;
        else if (toupper(token1[i]) > toupper(token2[i]))
            return 1;
        i++;
    }

    if (token1[i] == '\0' && token2[i] == '\0')
        return 0;
    else if (token1[i] == '\0')
        return -1;
    else
        return 1;
}
int main(){
    FILE *fp = fopen("..\\src\\ICT1002_Group Project Assignment_Sample.ini","r");
    int c = knowledge_read(fp);
    printf("%d",c);
    struct EntityLL *c1 = head;
    while(c1 != NULL){
        printf("Entity: %s\n",c1->entity);
        printf("what: %s\n",c1->what);
        printf("where: %s\n",c1->where);
        printf("who: %s\n",c1->who);
        c1 = c1->next;
    }
    c1=head;
    knowledge_reset();
    return 0;
}
*/
/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	// free all nodes in linked-list and reset head & tail
	struct EntityLL *current = head;
    struct EntityLL *next;
	while (current != NULL){
	   next = current->next;
	   memset(current,0, sizeof(struct EntityLL));
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

	/* to be implemented */

}
