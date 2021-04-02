/*
 * ICT1002 (C Language) Group Project.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "chat1002.h"


/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {
    return "Chatbot";
}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {
    return "User";
}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {
    // force flush response buffer to prevent reset response from popping up
    *response = '\0';
    /* check for empty input */
    if (inc < 1) {
        snprintf(response, n, "");
        return 0;
    }

    /* look for an intent and invoke the corresponding do_* function */
    if (chatbot_is_exit(inv[0]))
        return chatbot_do_exit(inc, inv, response, n);
    else if (chatbot_is_smalltalk(inv[0]))
        return chatbot_do_smalltalk(inc, inv, response, n);
    else if (chatbot_is_load(inv[0]))
        return chatbot_do_load(inc, inv, response, n);
    else if (chatbot_is_question(inv[0]))
        return chatbot_do_question(inc, inv, response, n);
    else if (chatbot_is_reset(inv[0]))
        return chatbot_do_reset(inc, inv, response, n);
    else if (chatbot_is_save(inv[0]))
        return chatbot_do_save(inc, inv, response, n);
    else {
        snprintf(response, n, "I don't understand \"%s\".", inv[0]);
        return 0;
    }

}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {
    return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {
    snprintf(response, n, "Goodbye!");
    return 1;
}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {
    return compare_token(intent, "LOAD") == 0;
}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {
    int start = 1;
    // if input is intent only
    if (inc == 1) {
        snprintf(response, n, "Filename cannot be empty!");
        return 0;
    }
        // check connective words
    else if (compare_token(inv[1], "from") == 0) {
        // if no filename behind connective word
        if (inc < 3) {
            snprintf(response, n, "Filename cannot be empty!");
            return 0;
        }
        // filename present, push start index back by 1
        start = 2;
    }
    // build filename
    char *filename = strcpy(filename, inv[start]);
    for (int i = start + 1; i < inc; i++) {
        strcat(filename, " ");
        strcat(filename, inv[i]);
    }

    FILE *f;
    f = fopen(filename, "r");
    // if unavailable to open file
    if (f == NULL) {
        snprintf(response, n, "File Not Found!");
        return 0;
    }
    int nresponses = knowledge_read(f);
    if (nresponses == F_INVALID){
        snprintf(response,n,"Invalid file supplied. Please check again.");
        return 0;
    }
    fclose(f);
    snprintf(response, n, "Loaded %d responses from file %s", nresponses, filename);
    return 0;
}


/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
    char keywords[3][6] = {"what", "where", "who"};
    for (int i = 0; i < 3; i++) {
        if (compare_token(intent, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


/*
 * Answer a question.
 *
 * inv[0] contains the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {
    char answer[MAX_RESPONSE];
    char entity[MAX_ENTITY];
    int entityStart;

    if (inc < 2) {
        snprintf(response,n,"That is not a valid question.");
        return 0;
    }

    if (compare_token(inv[1],"is") == 0 || compare_token(inv[1],"are") == 0) {
        entityStart = 2;
    } else {
        entityStart = 1;
    }

    // Building entity
    for (entityStart; entityStart<inc; entityStart++) {
        strcat(entity,inv[entityStart]);
        strcat(entity," ");
    }
    if (strlen(entity) == 0) {
        snprintf(response,n,"I do not understand your question.");
        return 0;
    }
    *strrchr(entity, ' ') = '\0';

    int isSuccess = knowledge_get(inv[0], entity, answer, n);
    if (isSuccess == KB_INVALID) {
        //question is not a question inv[0] is not what who where etc
        snprintf(response,n,"I do not understand your question.");
        return 0;
    } else if (isSuccess == KB_NOTFOUND) {
        // insert new answer since not found

        // rebuild question to re-display
        char *qn = calloc(1,MAX_INPUT);
        if (qn == NULL){
            snprintf(response,n,"failed to allocate memory for question.");
            return 0;
        }
        char *holder = qn;
        for (int i=0;i<inc;i++){
            sprintf(holder," %s",inv[i]);
            holder = holder + strlen(inv[i]) + 1;
        }
        prompt_user(answer,n,"I don't know.%s?",qn);

        if (isspace(answer) || strlen(answer) == 0){
            snprintf(response,n,">:(");
            return 0;
        }
        knowledge_put(inv[0],entity,answer);
        snprintf(response,n,"Thank you.");
        return 0;
    }
    //final output = entity + is/are + response from knowledge_get
    snprintf(response,n,answer);
    return 0;
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {
    return compare_token(intent, "RESET") == 0;
}

/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
    knowledge_reset();
    snprintf(response, n, "Reset Completed Successfully!");
    return 0;
}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "save"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {
    return compare_token(intent, "SAVE") == 0;
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
    int start = 1;
    // if input is intent only
    if (inc == 1) {
        snprintf(response, n, "Filename cannot be empty!");
        return 0;
    }
        // check connective words
    else if (compare_token(inv[1], "as") == 0 || compare_token(inv[1],"to") == 0) {
        // if no filename behind connective word
        if (inc < 3) {
            snprintf(response, n, "Filename cannot be empty!");
            return 0;
        }
        // filename present, push start index back by 1
        start = 2;
    }
    // build filename
    char *filename = strcpy(filename, inv[start]);
    for (int i = start + 1; i < inc; i++) {
        strcat(filename, " ");
        strcat(filename, inv[i]);
    }

    FILE *f;
    if (fopen(filename, "r")){
        char consent;
        prompt_user(&consent,2,"File exists. Overwrite? [y/n]: ");
        if (tolower(consent) != 'y'){
            snprintf(response,n,"Operation Aborted.");
            return 0;
        }
    }
    f = fopen(filename, "w");
    if (f == NULL) {
        snprintf(response, n, "Error! Unable to get handle to file.");
        return 0;
    }
    knowledge_write(f);
    fclose(f);
    snprintf(response, n, "Entries has been successfully saved to %s", filename);
    return 0;
}


/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent) {
    char keywords[7][10] = {"hello","hi","bye","goodbye","target","how","it's"};
    for (int i=0; i<7; i++){
        if (compare_token(intent, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n) {
    if(compare_token(inv[0],"how") == 0){
        if (compare_token(inv[inc-1],"you") == 0){
            snprintf(response, n, "Not too bad, can't complain.");
            return 0;
        }
        snprintf(response, n, "How what now?");
        return 0;
    }
    else if (compare_token(inv[0],"it's") == 0){
        char *output = calloc(1,MAX_RESPONSE);
        if (output == NULL){
            snprintf(response,n,"Failed to allocate memory for response!");
            return 0;
        }
        char *holder = output;
        for (int i=1;i<inc;i++){
            sprintf(holder," %s",inv[i]);
            holder = holder + strlen(inv[i]) + 1;
        }
        snprintf(response,n,"Indeed it's%s.",output);
        return 0;
    }
    else if(compare_token(inv[0], "hello") == 0 || compare_token(inv[0], "hi") == 0) {
        snprintf(response, n, "Hello!");
        return 0;
    }
    else if(compare_token(inv[0], "goodbye") == 0 || compare_token(inv[0],"bye") == 0) {
        snprintf(response, n, "Goodbye");
        return 1;
    }
    else if(compare_token(inv[0], "target") == 0){
        snprintf(response, n, "Eliminated");
        return 0;
    }
    return 0;
}
