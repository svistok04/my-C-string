#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct string
{
    uint8_t* data;
    uint8_t realLength; // for null-terminator
    uint8_t contentLength; // realLength without null-terminator
}str_t;

// allocates memory for str_t
str_t* stralloc(uint8_t length)
{
    str_t* string = malloc(sizeof(str_t));
    string->data = malloc(length + 1); // adding null terminator so it's + 1
    string->realLength = length + 1;
    string->contentLength = 0;
    return string;
}

// frees the memory of str_t
void strfree(str_t* string)
{
    if (string == NULL)
        return;

    free(string->data);
    free(string);
}

// frees the memory of an array of str_t
void free_str_array(str_t** str_array)
{
    if (str_array == NULL)
        return;

    for (size_t i = 0; str_array[i] != NULL; i++) {
        free(str_array[i]->data);
    //    free(str_array[i]);  // SIGABRT for strsplit function.
    }

    free(str_array);
}

// prints str_t
void strprint(str_t* string, FILE* file)
{
    fwrite(string->data, sizeof(uint8_t), string->contentLength, file);
}

// gets the length of const char*
size_t my_strlen(const char* str)
{
    size_t length = 0;

    while(*str != '\0')
    {
        length++;
        str++;
    }

    return length;
}

// copies from a pointer of an unknown type (actually, const char* or const uint8_t* are expected) to the data of str_t
uint8_t* my_strncpy(uint8_t* dest, const void* src, size_t length)
{
    const uint8_t* source = src;
    for (size_t i = 0; i < length; i++)
    {
        *dest++ = *source++;
    }

    return dest;
}

// adds some content to str_t, i.e. copies to it from some const char*
str_t* my_strdup(const char* str)
{
    str_t* string = stralloc(my_strlen(str));
    my_strncpy(string->data, str, string->realLength);
    string->contentLength = my_strlen(str);

    return string;
}

// concatenates two str_t
str_t* strconcat(str_t* str1, str_t* str2)
{
    str_t* string = stralloc(str1->contentLength + str2->contentLength);
    my_strncpy(string->data, str1->data, str1->contentLength);
    my_strncpy(string->data + str1->contentLength, str2->data, str2->contentLength);
    string->contentLength = str1->contentLength + str2->contentLength;
    return string;
}

// gets a substring from str_t based on the starting index and the length of substring
str_t* my_substr(str_t* str, size_t index, size_t substrLength)
{
    if (index + substrLength > str->realLength)
    {
        fprintf(stdout, "Substring out of range.");
        return NULL;
    }
    str_t* result = stralloc(substrLength);
    my_strncpy(result->data, str->data + index, substrLength);
    result->contentLength = substrLength;

    return result;
}

// compares two str_t. 0 if the same, 1 if the first character that doesn't match has a greater value in str1 than in
// str2, and -1 otherwise.
int16_t my_strcmp(str_t* str1, str_t* str2)
{
    int16_t result = 0;

    size_t cmp_length = (str1->realLength > str2->realLength) ? str2->realLength : str1->realLength;

    const uint8_t* data1 = str1->data;
    const uint8_t* data2 = str2->data;

    for(size_t i = 0; i < cmp_length; i++)
    {
        if (*(data1 + i) > *(data2 + i))
        {
            result = 1;
            break;
        }
        if (*(data1 + i) < *(data2 + i))
        {
            result = -1;
            break;
        }
    }

    return result;
}

// returns a pointer to the first occurrence of const char* word in some str_t.
// TODO: implement additional functionality for replacing a word with another word of a different length, etc.
uint8_t* my_strstr(str_t* str, const char* word)
{
    size_t counter = 0;
    size_t wordSize = my_strlen(word);
    uint8_t* result = NULL;
    uint8_t* data = str->data;
    const char* wordStart = word;
    for (size_t i = 0; i < str->realLength; i++)
    {
        if (*(data++) == *word)
        {
            counter++;
            word++;
            if (counter == wordSize)
            {
                return (uint8_t*)(data - counter);
            }
        } else
        {
            word = wordStart;
            counter = 0;
        }
    }
    return result;
}

// touppers some char
uint8_t my_toupper(uint8_t ch)
{
    return (ch >= 97 && ch <= 122) ? ch - 32 : ch;
}

// tolowers some char
uint8_t my_tolower(uint8_t ch)
{
    return (ch >= 65 && ch <= 90) ? ch + 32 : ch;
}

// touppers some str_t
str_t* strtoupper(str_t* str)
{
    uint8_t* data = str->data;
    for(size_t i = 0; i < str->contentLength; i++)
    {
        *data = my_toupper(*data);
        data++;
    }
    return str;
}

// tolowers some str_t
str_t* strtolower(str_t* str)
{
    uint8_t* data = str->data;
    for(size_t i = 0; i < str->contentLength; i++)
    {
        *data = my_tolower(*data);
        data++;
    }
    return str;
}

// splits a str_t into an array of substrings based on a specified delimiter.
// for ',' chosen, the input "hello,world" will be split into an array containing "hello" and "world".
str_t** strsplit(str_t* str, const uint8_t delimiter)
{
    uint8_t* data = str->data;
    size_t count = 0;
    size_t lengthCnt = 0;
    for(size_t i = 0; i < str->realLength; i++) {
        if (data[i] == delimiter) {
            count++;
        }
    }
    str_t** result = malloc((count + 2) * sizeof(str_t*));
    for (size_t i = 0; i < count + 2; i++)
    {
        result[i] = stralloc(10); // that's not smart lol
    }

    for(size_t i = 0; i < count + 1; i++)
    {
        size_t tmpLength = 0;
        str_t* tmp = result[i];
        uint8_t* tmpData = tmp->data;
        while (*data != delimiter && *data != '\0')
        {
            *(tmpData++) = *(data++);
            lengthCnt++;
            tmpLength++;
        }
        lengthCnt++;
        data++;
        tmp->realLength = tmpLength + 1;
        tmp->contentLength = tmpLength;
        if (lengthCnt >= str->realLength)
        {
            break;
        }
    }

    result[count + 2] = NULL;

    return result;
}

int main(int argc, char** argv) {
    str_t* hello1 = my_strdup("hello");
    str_t* hello2 = my_strdup("hello");
    strprint(hello1, stdout);

    str_t* newLine = my_strdup("\n");
    strprint(newLine, stdout);

    hello2 = strconcat(hello2, my_strdup("priv"));
    strprint(hello2, stdout);
    strprint(newLine, stdout);

    str_t* hello3 = my_strdup("priv");
    strprint(hello3, stdout);
    strprint(newLine, stdout);

    str_t* hellomix = strconcat(hello1, hello3);
    strprint(hellomix, stdout);
    strprint(newLine, stdout);

    str_t* substring = my_substr(hellomix, 3, 5);
    if (substring != NULL) strprint(substring, stdout);
    strprint(newLine, stdout);

    int16_t comparison1 = my_strcmp(hello1, hello1); // hello == hello
    int16_t comparison2 = my_strcmp(hello1, hello3); // hello < priv
    int16_t comparison3 = my_strcmp(hello3, hello1); // priv > hello

    fprintf(stdout, "%d\n%d\n%d\n", comparison1, comparison2, comparison3);

    str_t* str = my_strdup("This is a simple string");
    strprint(str, stdout);
    strprint(newLine, stdout);
    uint8_t* ptr = my_strstr(str, "simple");
    if (ptr != NULL)
    {
        my_strncpy(ptr, "sample", 6);
    }
    strprint(str, stdout);
    strprint(newLine, stdout);

    str_t* UPPERCASE = my_strdup("UPPERCASE");
    strtolower(UPPERCASE);
    strprint(UPPERCASE, stdout);
    strprint(newLine, stdout);

    str_t* lowercase = my_strdup("lowercase");
    strtoupper(lowercase);
    strprint(lowercase, stdout);
    strprint(newLine, stdout);

    str_t* text = my_strdup("hello,hallo,tschuss,bye,poka,privet");
    str_t** wordsSeparated = strsplit(text, ',');
    size_t n = 0;
    while(wordsSeparated[n] != NULL)
    {
        n++;
    }
    for (size_t i = 0; i < n; i++)
    {
        strprint(wordsSeparated[i], stdout);
        strprint(newLine, stdout);
    }

    // testing this dumb bug
    str_t** result = malloc(4 * sizeof(str_t*));
    for (size_t i = 0; i < 3; i++)
    {
        result[i] = stralloc(10);
    }
    result[3] = NULL;
    free_str_array(result);


    strfree(hello1);
    strfree(hello2);
    strfree(hello3);
    strfree(newLine);
    strfree(hellomix);
    if (substring != NULL) strfree(substring);
    strfree(str);
    strfree(UPPERCASE);
    strfree(lowercase);
    strfree(text);
    free_str_array(wordsSeparated); // memory leak: can't dealloc memory to which str_t* points when using
                                            // strsplit function


    return 0;
}
