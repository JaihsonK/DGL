/**
 * Dodge Interpreted Language
 * An interpreted language to write 2D games in
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define isdigit(c) (c >= '0' && c <= '9')

#define MAX_INCLUDES 70
#define MAX_LINE_LENGTH 100
#define MAX_VARIABLES 200

//characters that cannot be in a variable identifier
#define NON_VAR_CHARS " \t\n\r\a!@#$%^&*()_+=-[]\\{}|:\";'<>?,./~`"

typedef struct
{
    size_t index;

    size_t line;
    size_t column;
} fstream_position;

typedef struct _finfo finfo;
struct _finfo
{
    size_t fsize;
    FILE *fp;

    finfo *includes[MAX_INCLUDES];
    size_t includes_index;
};

struct interpreter
{
    finfo *main_prog;
    fstream_position pos;
    char *stream;
} dil;

struct variable
{
    _Bool in_use;
    char identifier[21];
    long long value;
};

size_t fgetline(FILE *fp, char *out, int max)
{
    char c;
    size_t i;
    for (i = 0; i < max - 1; i++)
    {
        if ((c = fgetc(fp)) == EOF || c == '\n' || c == '\r')
            break;

        out[i] = c;
    }
    out[i] = 0;
    return i;
}

// read the next ; terminated line out of dil.stream
size_t pgetline(char *out, int max)
{
    if (!dil.stream || !out || max < 2)
        return 0;

    size_t i;
    for (i = 0; i < max - 1 && dil.stream[i + dil.pos.index] != ';' && dil.stream[i + dil.pos.index] != 0; i++)
        out[i] = dil.stream[i + dil.pos.index];

    dil.pos.index += i + 1;
    dil.pos.line++;
    out[i] = 0;
    return i;
}

finfo *add_file(char const *filename)
{
    finfo *res = malloc(sizeof(finfo));
    res->fp = fopen(filename, "r");
    if (!res->fp)
        goto out;

    fseek(res->fp, 0, SEEK_END);
    res->fsize = ftell(res->fp);
    rewind(res->fp);

    char buf[MAX_LINE_LENGTH];
    while (fgetline(res->fp, buf, 100))
        if (buf[0] == '!' && res->includes_index < MAX_INCLUDES)
        {
            res->includes[res->includes_index++] = add_file(&buf[1]);
        }
    rewind(res->fp);
out:
    if (!res->fp)
    {
        free(res);
        res = 0;
    }
    return res;
}

size_t f_tree_size(finfo *fi)
{
    size_t res = fi->fsize;
    for (size_t i = 0; i < MAX_INCLUDES; i++)
        if (fi->includes[i])
            res += f_tree_size(fi->includes[i]);
    return res;
}

void free_tree(finfo *fi)
{
    for (size_t i = 0; i < MAX_INCLUDES; i++)
        if (fi->includes[i])
            free_tree(fi->includes[i]);

    fclose(fi->fp);
    free(fi);
}

// read file contents and includes into out
char *read_in(finfo *fi, char *out)
{
    if (!fi || !out || !fi->fp || !fi->fsize)
        return NULL;
    char *tmp = out;
    char ln[MAX_LINE_LENGTH];
    size_t include_index = 0;
    while (fgetline(fi->fp, ln, MAX_LINE_LENGTH))
    {
        if (ln[0] == '!')
            tmp = read_in(fi->includes[include_index++], tmp);
        else if (ln[0] == '#')
            continue; // skip comments
        else
            for (size_t i = 0; ln[i] != 0 && i < MAX_LINE_LENGTH; i++)
                *(tmp++) = ln[i];
    }
    return tmp;
}

//count the span from the first occurrence of f to the last occurrence of l.
//Format: (offset << 32) | span
__int64_t count_span_ftl(char *str, char f, char l)
{
    __int32_t res = -1;
    __int32_t offset = -1;

    char *tmp = str;
    while(*tmp != f && *tmp)
        tmp++;
    if(!*tmp)
    {
        return (offset << 32) | res;
    } 
    offset = (__int32_t) (tmp - str);
    res = 0;
    __int32_t i = 0;
    while(*tmp)
    {
        if(*tmp == l)
            res += i;
        i++;
    }
    return (offset << 32) | res;
}

char *lltoa(long long val, char *out)
{
#define res out
    int loc = 20;
    _Bool neg = 0;
    if ((neg = val < 0))
        val = -val;
    while (val)
    {
        res[loc--] = '0' + val % 10;
        val /= 10;
    }
    if (loc == 20)
        res[loc--] = '0';
    if (neg)
        res[loc--] = '-';

    return &res[loc + 1];
#undef res
}

long long get_var(char *identifier, struct variable *current_frame)
{
    if (!identifier || !current_frame)
        return 0;
    identifier = strtok(identifier, NON_VAR_CHARS);
    for (size_t i = 0; i < MAX_VARIABLES; i++)
        if (current_frame[i].in_use && !strcmp(identifier, current_frame[i].identifier))
            return current_frame[i].value;
    return 0;
}

long long eval_exp(char *expression, struct variable *current_frame)
{
    if (!expression)
        return 0;

    size_t exp_len = strlen(expression);
    size_t len = strcspn(expression, "(");
    while(exp_len != len)
    {

    }

    size_t toklen;

    long long op1, op2;

    // algorithm checks operators in reverse BEDMAS so that the outcome is BEDMAS compliant

    char *token = strtok(expression, "-");
    if ((toklen = strlen(token)) != exp_len)
    {
        op1 = eval_exp(token, current_frame);
        op2 = eval_exp(expression + toklen + 1, current_frame);
        return op1 - op2;
    }

    token = strtok(expression, "+");
    if ((toklen = strlen(token)) != exp_len)
    {
        op1 = eval_exp(token, current_frame);
        op2 = eval_exp(expression + toklen + 1, current_frame);
        return op1 + op2;
    }

    token = strtok(expression, "/");
    if ((toklen = strlen(token)) != exp_len)
    {
        op1 = eval_exp(token, current_frame);
        op2 = eval_exp(expression + toklen + 1, current_frame);
        return op1 / op2;
    }

    token = strtok(expression, "*");
    if ((toklen = strlen(token)) != exp_len)
    {
        op1 = eval_exp(token, current_frame);
        op2 = eval_exp(expression + toklen + 1, current_frame);
        return op1 * op2;
    }

    if (token[0] == '(')
        token++;
    if (token[0] == '$')
        return get_var(&token[1], current_frame);

    return strtoll(token, NULL, 0);
}

char *eval_string(char *expression, struct variable *current_frame)
{
    static char outp[MAX_LINE_LENGTH + 1];
    size_t outp_index = 0;
    char *tmp = expression;
    while (*tmp && tmp - expression < MAX_LINE_LENGTH)
    {
        if (*tmp == '$')
            if (*(tmp + 1) == '$')
            {
                outp[outp_index++] = '$';
                tmp += 2;
                continue;
            }
            else
            {
                char buf[21];
                char *token = strtok(++tmp, " \t");
                char *c = lltoa(get_var(token, current_frame), buf);
                for (; c < buf + 22 && isdigit(*c); c++)
                {
                    outp[outp_index++] = *c;
                } 
                tmp += 1 + strlen(token);
                outp[outp_index++] = ' ';
            }
        else
            outp[outp_index++] = *tmp++;
    }
    outp[outp_index] = 0;
    return outp;
}

int interpret_line(char *line, struct variable *current_frame)
{
    char *token = strtok(line, " \t");
    if (!strcmp(token, "var"))
    {
        // declare a variable
        // expected syntax: var <name> <value>
        //<name> is limited to 20 characters

        size_t i;
        for (i = 0; i < MAX_VARIABLES; i++)
            if (!current_frame[i].in_use)
            {
                current_frame[i].in_use = 1;
                break;
            }
        if (i == MAX_VARIABLES)
        {
            printf("DGL: DIL interpreter: ERROR: cannot create variable. %d variable limit has been reached for the current reference frame.\n%ld: %s\n",
                   MAX_VARIABLES,
                   dil.pos.line - 1,
                   line);
            return -1;
        }

        token = strtok(NULL, " \t");
        if (!token)
        {
            printf("DGL: DIL interpreter: ERROR: incomplete variable declaration.\n%ld: %s\n",
                   dil.pos.line - 1,
                   line);
            return -1;
        }
        if (strlen(token) > 20)
        {
            printf("DGL: DIL interpreter: ERROR: variable name exceeds 20 character limit.\n%ld: %s\n",
                   dil.pos.line - 1,
                   line);
            return -1;
        }

        strcpy(current_frame[i].identifier, token);

        token = strtok(NULL, "");
        if (!token)
        {
            printf("DGL: DIL interpreter: ERROR: incomplete variable declaration. Expected type and value\n%ld: %s\n",
                   dil.pos.line - 1,
                   line);
            return -1;
        }
        current_frame[i].value = eval_exp(token, current_frame);
    }

    return 0;
}

/**
 * @brief Open DIL file, include used files, and return the address of the entire program.
 * @param filename path to the DIL file
 */
char *open_dil(char const *filename)
{
    if (!filename)
        return NULL;

    dil.main_prog = add_file(filename);

    dil.stream = malloc(f_tree_size(dil.main_prog));

    if (read_in(dil.main_prog, dil.stream) == 0)
    {
        free_tree(dil.main_prog);
        return NULL;
    }
    free_tree(dil.main_prog);

    return dil.stream;
}

int run()
{
    int res = 0;
    char line[MAX_LINE_LENGTH];
    struct variable *global_frame = malloc(sizeof(struct variable) * MAX_VARIABLES);
    if (!global_frame)
        return -1;

    while (1)
    {
        size_t line_len = pgetline(line, MAX_LINE_LENGTH);
        if (line_len < 1 || line_len > MAX_LINE_LENGTH)
            break;

        if (line[0] == '>')
        {
            printf("> %s\n", eval_string(&line[1], global_frame));
            continue;
        }

        if (interpret_line(line, global_frame) != 0)
        {
            printf("DGL: DIL interpreter: ERROR: could not interpret line \n%ld: %s\n",
                   dil.pos.line,
                   line);
            res = -1;
            goto out;
        }
    }
out:
    free(global_frame);
    return res;
}