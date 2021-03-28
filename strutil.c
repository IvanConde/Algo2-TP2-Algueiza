#include <string.h>
#include <stdlib.h>
#include "strutil.h"

size_t contar_separadores(const char* str, char sep){

    size_t cantidad = 0;
    for(int i = 0; i < strlen(str); i++){
        if (str[i] == sep){
            cantidad++;
        }
    }
    return cantidad;

}

char* substr(const char *str, size_t n){

    char* cadena = malloc(sizeof(char)*(n+1));
    if(!cadena) return NULL;
    strncpy(cadena, str, n);
    cadena[n] = '\0';
    return cadena;

}

char** split(const char *str, char sep){

    size_t largo_str = strlen(str);

    if(sep == '\0'){
        char** strv = malloc(sizeof(char*)*2);
        if(!strv) return NULL;
        strv[0] = substr(str, largo_str);
        strv[1] = NULL;
        return strv;
    }

    size_t cantidad = contar_separadores(str, sep);
    size_t pos_actual = -1;
    size_t pos_anterior = 0;
    size_t contador = 0;

    char **strv = malloc(sizeof(char*) * (cantidad+2));
    if(!strv) return NULL;

    for(size_t i = 0; i <= largo_str; i++){
        if(str[i] == sep || str[i] == '\0'){
            pos_anterior = pos_actual;
            pos_anterior++;
            pos_actual = i;
            strv[contador] = substr(str+pos_anterior, pos_actual-pos_anterior);
            contador++;
        }
    }   
    strv[contador] = NULL;

    return strv;

}

char* join(char **strv, char sep){

    size_t pos_actual = 0;
    size_t len_caracteres = 0;
    size_t i = 0;

    while(strv[i]){
        len_caracteres += strlen(strv[i]);
        i++;
    }
    size_t largo_str = len_caracteres + i;

    char *str = malloc(sizeof(char) * (largo_str+1));
    if(!str) return NULL;

    for(i = 0; strv[i] != NULL; i++){

        if(sep == '\0' && pos_actual > 0){
            strcpy(str + pos_actual - i, strv[i]);
        }
        else{
            strcpy(str + pos_actual, strv[i]);
        }

        pos_actual += (strlen(strv[i]) + 1);

        if(strv[i+1]){
            str[pos_actual-1] = sep;
        }

    }
    str[largo_str] = '\0';

    return str;

}

void free_strv(char *strv[]){

    for(int i = 0; strv[i] != NULL; i++){
        free(strv[i]);
    }
    free(strv);

}