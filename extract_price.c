#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "cJSON.h"
#include "update.h"    
        
char* extract_price(const char* json)
{
    cJSON *root = cJSON_Parse(json);
    cJSON *JSON_price = cJSON_GetObjectItem(root, "price");
    char* price = strdup(JSON_price->valuestring);

    cJSON_Delete(root);

    return price;
}

