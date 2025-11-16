#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "update.h"
#include "cJSON.h"

char* read_json()
{
    FILE *fptr = fopen("value.json", "r"); 
    char *json = malloc(1000); 

    fgets(json, 1000, fptr); 
    fclose(fptr); 
    return json;
}

int main()
{
    api_key();
    system("clear");
    update_value();
    
    for (;;)
    {
        update_value();
        char* json = read_json();
        char* value = extract_price(json);
        printf("\rCurrent ₿ value in USD: %s", value);
        fflush(stdout);

        free(json);
        sleep(15);
    }

    return 0;
} 


