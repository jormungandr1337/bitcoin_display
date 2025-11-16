#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "update.h"

// when libcurl downloads data, it delivers it in chunks — not all at once.
// we need a place to collect those chunks.

struct string 
{
    char *ptr;  // pointer to the dynamically allocated memory holding our data (the text returned from the API);
    size_t len; // how many bytes are currently stored
};

// this is function declaration
// beginner readable version: static int main(int x, char y);
static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)

/*
EXPLANATION:
    static - makes the function private, meaning that this function isnt visible from other files
    size_t - return type - it returns number of bytes as unsigned integers
    (void *ptr, size_t size, size_t nmemb, struct string *s) - function parameters
    PARAMETERS EXPLANATION:
        void *ptr - pointer to part of the JSON text
        size_t size - size of each data element
        size_t nmemb - number of elements in this chunk
        size*nmemb = total bytes recieved
        struct string *s - here i want to store all incoming data

*/
{
    size_t new_len = s->len + size * nmemb;     // calculating the size of string
    char *tmp = realloc(s->ptr, new_len + 1);   // resizing the memory buffer, to fit all data (+ 1 for \0)
    if (tmp == NULL)                            // self explanatory 
    {
        fprintf(stderr, "realloc fucked up\n");
        return 0;
    }
    s->ptr = tmp;
    memcpy(s->ptr + s->len, ptr, size * nmemb); 
    /*
        more detailed explanation of this
        s->ptr - here we are storing all chunks collected so far
        s->len - number of bytes in s->ptr
        s->ptr + s->len = end of current chunk, at this spot new chunks should be appended
        ptr - pointer to the new chunk of data
        size * nmemb = total size of new chunk
    */
    s->ptr[new_len] = '\0';                     // keep the string null-terminated, so it stays a valid C string
    s->len = new_len;                           // updating total length
    return size * nmemb;                        // returning amount of bytes
}

// this shit is used for holding string

static void init_string(struct string *s)
{
    s->len = 0;         // length 0
    s->ptr = malloc(1); // please give me 1 byte of memory somewhere in RAM, and tell me where it starts
    s->ptr[0] = '\0';   // assigning '\0' for C to know, where is the end of the string
}

char key[100];
char header[100] = "X-Api-Key: ";
char* result;

void api_key()
{
    printf("Paste your api key: ");
    scanf("%s", key);

    result = malloc(strlen(header) + strlen(key) + 1);

    strcpy(result, header);
    strcat(result, key);
}

char* recieve_api()
{ 
    CURL *curl = curl_easy_init();
    struct string s;
    struct curl_slist *headers = NULL;
    init_string(&s);

    char* key = result;

    if (curl)
    {
        // each curl_easy_setopt() just configures our request
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.api-ninjas.com/v1/bitcoin");
        headers = curl_slist_append(headers, key);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);   // calling "writefunc" function for handling all incoming data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);              // we are writing our response, to struct "s"
        curl_easy_perform(curl);                                    // actually pulling HTTP request
        curl_slist_free_all(headers);                               // freeing headers i guess 
        curl_easy_cleanup(curl);                                    // free libcurl resources (idk what resources were occupied, for us to free them, please explain)
    }
    return s.ptr;
}

void update_value()
{
    char* data = recieve_api();

    FILE *fptr;
    fptr = fopen("value.json", "w");
    fprintf(fptr, "%s", data);

    fclose(fptr);
    free(data);
}
