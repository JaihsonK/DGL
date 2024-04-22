#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char buffer[101];

char *get_item(FILE *fp, char *item)
{
    memset(buffer, 0, sizeof(buffer));
    if (!fp)
        return NULL;
    rewind(fp);

    while (fgets(buffer, 100, fp))
    {
        
        int i;
        for (i = 0; buffer[i] != '\n' && buffer[i] != 0; i++)
            ;
        if (buffer[i] == '\n')
            buffer[i] = 0;

        for (i = 0; buffer[i] != ':'; i++)
            ;
        if (buffer[i] == ':')
        {
            buffer[i] = 0;
            if (strcmp(buffer, item) == 0)
            {
                strcpy(buffer, &buffer[i + 1]);
                break;
            }
        }
    }
    return buffer;
}