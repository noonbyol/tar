#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

int _strlen(char*s)
{
    int i;
    for(i = 0; s[i]; i++);
    return i;
}


void making_null(char*s, int n)
{
    for(int i = 0; i < n; i++)
        s[i] = '\0';
}

int sum_of_ascii(char* p1){
    int sum = 0;
    for(int i = 0; i < _strlen(p1); i++)
        sum += (int)p1[i];
    return sum;
}

int _strcmp(char s1[], char s2[]) 
{
    int i;
    for(i = 0; s1[i] != '\0' && s2[i] != '\0'; i++)
        if (s1[i] != s2[i])
            return s1[i] - s2[i];
    return s1[i] - s2[i];
}

    void _strcpy(char*s1, char*s2)
    {
        for(int i = 0; s2[i]; i++)
            s1[i] = s2[i];
    }

    void _strcat(char* a, char* b){
        int j = 0;
        for(int i = _strlen(a); j < _strlen(b); i++, j++)
            a[i] = b[j];
    }


void err_no(int fd, char*s)
{
    if(fd == -1)
    {
        write(2, "my_tar: ", 8);
        write(2, s, _strlen(s));
        write(2, ": Cannot stat: No such file or directory\n", 41);
    }
}