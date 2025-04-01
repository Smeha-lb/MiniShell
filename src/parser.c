#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Token struct definition
typedef struct {
    int in_double_quotes;
    int in_single_quotes;
    char *value;
} token;

// Custom implementations of string functions
int my_strlen(const char* str);
char* my_strdup(const char* str);
void* my_realloc(void* ptr, size_t old_size, size_t new_size);

// Function prototypes
void free_tokens_on_error(token* result, int token_count);
int extract_quoted(const char* input, int input_len, int* i, char* token_buffer, char quote_char);
void add_token_to_result(char* token_buffer, token** result, int* token_count, int* max_tokens, 
                        int* current_size, int in_double_quotes, int in_single_quotes);
void process_token(const char* input, int input_len, int* i, char* token_buffer, token** result, 
                  int* token_count, int* max_tokens, int* current_size);
void finalize_result(token* result, int token_count, int max_tokens, int* count, int current_size);
void free_parsed_result(token* parsed_result, int count);

/**
 * Custom implementation of strlen
 */
int my_strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * Custom implementation of strdup
 */
char* my_strdup(const char* str) {
    int len = my_strlen(str);
    char* new_str = (char*)malloc((len + 1) * sizeof(char));
    if (new_str == NULL) {
        return NULL;
    }
    
    int i = 0;
    while (i <= len) {
        new_str[i] = str[i];
        i++;
    }
    
    return new_str;
}

/**
 * Custom implementation of realloc
 */
void* my_realloc(void* ptr, size_t old_size, size_t new_size) {
    void* new_ptr = malloc(new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    if (ptr != NULL) {
        char* src = (char*)ptr;
        char* dst = (char*)new_ptr;
        size_t i = 0;
        size_t copy_size = old_size < new_size ? old_size : new_size;
        
        while (i < copy_size) {
            dst[i] = src[i];
            i++;
        }
        
        free(ptr);
    }
    
    return new_ptr;
}

/**
 * Frees memory in case of allocation error
 */
void free_tokens_on_error(token* result, int token_count) {
    int j = 0;
    while (j < token_count) {
        free(result[j].value);
        j++;
    }
    free(result);
}

/**
 * Extracts content between quotes
 */
int extract_quoted(const char* input, int input_len, int* i, 
                  char* token_buffer, char quote_char) {
    int buffer_pos = 0;
    int escaped = 0;
    
    while (*i < input_len) {
        if (input[*i] == '\\' && !escaped) {
            escaped = 1;
            (*i)++;
            continue;
        }
        
        if (input[*i] == quote_char && !escaped) {
            (*i)++; // Skip the closing quote
            break;
        }
        
        token_buffer[buffer_pos++] = input[(*i)++];
        escaped = 0;
    }
    
    return buffer_pos;
}

/**
 * Adds a token to the result array
 */
void add_token_to_result(char* token_buffer, token** result, 
                        int* token_count, int* max_tokens, int* current_size,
                        int in_double_quotes, int in_single_quotes) {
    // Resize result array if needed
    if (*token_count >= *max_tokens) {
        int new_size = *current_size * 2;
        token* new_result = (token*)my_realloc(*result, 
                             *current_size, new_size);
        if (new_result == NULL) {
            free_tokens_on_error(*result, *token_count);
            return;
        }
        *result = new_result;
        *current_size = new_size;
        *max_tokens *= 2;
    }
    
    // Set token values
    (*result)[*token_count].in_double_quotes = in_double_quotes;
    (*result)[*token_count].in_single_quotes = in_single_quotes;
    (*result)[*token_count].value = my_strdup(token_buffer);
    
    if ((*result)[*token_count].value == NULL) {
        free_tokens_on_error(*result, *token_count);
        return;
    }
    
    (*token_count)++;
}

/**
 * Processes a single token
 */
void process_token(const char* input, int input_len, int* i, 
                  char* token_buffer, token** result, 
                  int* token_count, int* max_tokens, int* current_size) {
    int buffer_pos = 0;
    int in_double_quotes = 0;
    int in_single_quotes = 0;
    
    // Check if this token starts with a quote
    if (input[*i] == '"') {
        in_double_quotes = 1;
        (*i)++; // Skip the opening quote
        
        // Extract everything until the matching closing quote
        buffer_pos = extract_quoted(&input[0], input_len, i, 
                                   token_buffer, '"');
    } else if (input[*i] == '\'') {
        in_single_quotes = 1;
        (*i)++; // Skip the opening quote
        
        // Extract everything until the matching closing quote
        buffer_pos = extract_quoted(&input[0], input_len, i, 
                                   token_buffer, '\'');
    } else {
        // Extract non-quoted token until space
        while (*i < input_len && input[*i] != ' ') {
            token_buffer[buffer_pos++] = input[(*i)++];
        }
    }
    
    // Null-terminate the token
    token_buffer[buffer_pos] = '\0';
    
    // Add token to result
    add_token_to_result(token_buffer, result, token_count, max_tokens, 
                      current_size, in_double_quotes, in_single_quotes);
}

/**
 * Finalizes the result array
 */
void finalize_result(token* result, int token_count, 
                    int max_tokens, int* count, int current_size) {
    // Set count
    *count = token_count;
}

/**
 * Parses a string into tokens, splitting at spaces while preserving
 * content inside single or double quotes as single tokens.
 */
token* parse_string(const char* input, int* count) {
    if (input == NULL || count == NULL) {
        return NULL;
    }

    // Get input length
    int input_len = my_strlen(input);
    
    // Initial allocation size
    int max_tokens = 10;
    int current_size = max_tokens * sizeof(token);
    token* result = (token*)malloc(current_size);
    if (result == NULL) {
        return NULL;
    }

    int token_count = 0;
    
    // Buffer to build tokens
    char* token_buffer = (char*)malloc((input_len + 1) * sizeof(char));
    if (token_buffer == NULL) {
        free(result);
        return NULL;
    }
    
    int i = 0;
    while (i < input_len) {
        // Skip leading spaces
        while (i < input_len && input[i] == ' ') {
            i++;
        }
        
        if (i >= input_len) {
            break; // End of string
        }
        
        process_token(input, input_len, &i, token_buffer, &result, 
                     &token_count, &max_tokens, &current_size);
    }
    
    free(token_buffer);
    finalize_result(result, token_count, max_tokens, count, current_size);
    return result;
}

/**
 * Function to free the memory allocated by parse_string
 */
void free_parsed_result(token* parsed_result, int count) {
    if (parsed_result == NULL) {
        return;
    }
    
    int i = 0;
    while (i < count) {
        free(parsed_result[i].value);
        i++;
    }
    free(parsed_result);
}

/**
 * Example usage
 */
int main() {
    const char* test = "hello world \"this '$is' quoted\" not-quoted 'single quoted stuff'";
    int count;
    token* tokens = parse_string(test, &count);
    
    if (tokens) {
        printf("Found %d tokens:\n", count);
        int i = 0;
        while (i < count) {
            printf("%d: %s", i, tokens[i].value);
            if (tokens[i].in_double_quotes) {
                printf(" (in double quotes)");
            }
            if (tokens[i].in_single_quotes) {
                printf(" (in single quotes)");
            }
            printf("\n");
            i++;
        }
        
        free_parsed_result(tokens, count);
    }
    
    return 0;
}