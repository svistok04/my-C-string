#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct string
{
    uint8_t* data;
    uint8_t length; // for null-terminator
    uint8_t howMuch; // length without null-terminator
}str_t;

str_t* string_create(uint8_t length)
{
    str_t* string = malloc(sizeof(str_t));
    string->data = malloc(length + 1); //adding null terminator
    string->length = length + 1;
    string->howMuch = 0;
    return string;
}

void string_free(str_t* string)
{
    free(string->data);
    free(string);
}

void string_print(str_t* string, FILE* file)
{
    fwrite(string->data, sizeof(uint8_t), string->howMuch, file);
}

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

void* my_memcpy(uint8_t* dest, const void* src, size_t length)
{
    const uint8_t* source = src;
    for (size_t i = 0; i < length; i++)
    {
        *dest++ = *source++;
    }

    return dest;
}

str_t* string_dup(const char* str)
{
    str_t* string = string_create(my_strlen(str));
    my_memcpy(string->data, str, string->length);
    string->howMuch = my_strlen(str);

    return string;
}

str_t* string_concat(str_t* str1, str_t* str2)
{
    str_t* string = string_create(str1->howMuch + str2->howMuch);
    my_memcpy(string->data, str1->data, str1->howMuch);
    my_memcpy(string->data + str1->howMuch, str2->data, str2->howMuch);
    string->howMuch = str1->howMuch + str2->howMuch;
    return string;
}

int main(int argc, char** argv) {
    str_t* hello = string_dup("hello");
    string_print(hello, stdout);
    str_t* newLine = string_dup("\n");
    string_print(newLine, stdout);
    str_t* hello2x = string_concat(hello, hello);
    string_print(hello2x, stdout);

    string_free(hello);
    string_free(newLine);
    string_free(hello2x);
    return 0;
}
