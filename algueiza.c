#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "strutil.h"
#include "hash.h"
#include "abb.h"
#include "heap.h"
#include "cola.h"

#define EXITO 0
#define ERROR 1

#define LONGITUD_MINIMA 1

#define VALOR_AGREGAR_ARCHIVO 0
#define VALOR_VER_TABLERO 1
#define VALOR_INFO_VUELO 2
#define VALOR_PRIORIDAD_VUELOS 3
#define VALOR_BORRAR 4
#define COMANDO_INVALIDO 5

#define INDICE_FLIGHT_NUMBER 0
#define INDICE_PRIORIDAD 5
#define INDICE_FECHA 6

void print_error(char* comando){
    fprintf(stderr, "Error en comando %s\n", comando);
}

bool comando_borrar(char* desde, char* hasta, hash_t* hash, abb_t* abb){

    abb_iter_t* iter = abb_iter_in_crear(abb, desde, hasta, "asc");
    if(!iter) return false;
    cola_t* cola = cola_crear();
    if(!cola) return false;

    while(!abb_iter_in_al_final(iter)){
        char* clave_copia = strdup(abb_iter_in_ver_actual(iter));
        cola_encolar(cola, clave_copia);
        abb_iter_in_avanzar(iter);
    }

    while(!cola_esta_vacia(cola)){
        char* clave_temp = cola_desencolar(cola);
        char** vuelo = abb_borrar(abb, clave_temp);
        if(!vuelo) return false;
        char** aux = hash_borrar(hash, vuelo[INDICE_FLIGHT_NUMBER]);
        if(!aux) return false;
        char* linea = join(vuelo, ' ');
        if(!linea) return false;
        fprintf(stdout, "%s\n", linea);
        free(clave_temp);
        free(linea);
        free_strv(aux);
    }

    abb_iter_in_destruir(iter);
    cola_destruir(cola, NULL);
    fprintf(stdout, "OK\n");
    return true;

}

int funcion_cmp(const void* a, const void* b){

    char** linea_a = (char**)a;
    char** linea_b = (char**)b;
    int prioridad_a = atoi(linea_a[INDICE_PRIORIDAD]);
    int prioridad_b = atoi(linea_b[INDICE_PRIORIDAD]);
    if(prioridad_a > prioridad_b){
        return -1;
    }
    else if(prioridad_a < prioridad_b){
        return 1;
    }
    else{
        if(strcmp(linea_a[INDICE_FLIGHT_NUMBER], linea_b[INDICE_FLIGHT_NUMBER]) > 0) return 1;
        else return -1;
    }

}

void imprimir_prioridad_vuelos(heap_t* heap){
	if(heap_esta_vacio(heap)) return;
	char** linea = heap_desencolar(heap);
	imprimir_prioridad_vuelos(heap);
	fprintf(stdout, "%s - %s\n", linea[INDICE_PRIORIDAD], linea[INDICE_FLIGHT_NUMBER]);
}


bool comando_prioridad_vuelos(char* cantidad, hash_t* hash){

    int k = atoi(cantidad);
    if(k < 1) return false;
    
    heap_t* heap = heap_crear(funcion_cmp);
    if(!heap) return NULL;

    hash_iter_t* iter = hash_iter_crear(hash);
    if(!iter){
        heap_destruir(heap, NULL);
        return NULL;
    }

    size_t contador = 0;
    while(!hash_iter_al_final(iter)){
        char** leido = hash_obtener(hash, hash_iter_ver_actual(iter));
        if(contador < k){
            heap_encolar(heap, leido);
            contador++;
        }
        else if(funcion_cmp(leido, heap_ver_max(heap)) < 0){
            heap_desencolar(heap);
            heap_encolar(heap, leido);
        }
        hash_iter_avanzar(iter);
    }

    imprimir_prioridad_vuelos(heap);
    hash_iter_destruir(iter);
    heap_destruir(heap, NULL);
    fprintf(stdout, "OK\n");
    return true;

}

bool comando_info_vuelo(char* flight_number, hash_t* hash){

	char** vuelo = hash_obtener(hash, flight_number);
	if(!vuelo) return false;
	char* linea = join(vuelo,' ');
	if(!linea) return false;
	fprintf(stdout, "%s\n", linea);
	free(linea);
    fprintf(stdout, "OK\n");
    return true;

}

bool comando_ver_tablero(char* cantidad, char* orden, char* desde, char* hasta, abb_t* abb){

    int k = atoi(cantidad);
    if(k < 1) return false;

	bool validar_orden = (strcmp(orden, "asc") == 0 || strcmp(orden, "desc") == 0);
	if(!validar_orden) return false;

    if(strcmp(desde, hasta) > 0) return false;

    abb_iter_t* iter = abb_iter_in_crear(abb, desde, hasta, orden);
    if(!iter) return false;
    size_t i = 0;

	while(!abb_iter_in_al_final(iter) && i < k){
        const char* clave = abb_iter_in_ver_actual(iter);
        char** linea = abb_obtener(abb, clave);
        printf("%s - %s\n", linea[INDICE_FECHA], linea[INDICE_FLIGHT_NUMBER]);
        abb_iter_in_avanzar(iter);
        i++;
	}

	abb_iter_in_destruir(iter);
    fprintf(stdout, "OK\n");
    return true;

}

void eliminar_fin_de_linea(char* linea){
    char *pos;
    if((pos = strchr(linea, '\n')) != NULL) *pos = '\0';
}

void destruir_dato_wrapper(void* dato){
	free_strv((char**)dato);
}

char* gen_clave_abb(char** strv){

    char* clave[3] = {strv[INDICE_FECHA], strv[INDICE_FLIGHT_NUMBER], NULL};
    return join(clave, ' ');

}

bool comando_agregar_archivo(char* nombre_archivo, hash_t* hash, abb_t* abb){

    FILE* archivo = fopen(nombre_archivo, "r");
    if(!archivo) return false;

    char* linea = NULL;
    size_t len = 0;
    char* clave_abb = NULL;
    char* clave_aux = NULL;

    ssize_t leer = getline(&linea, &len, archivo);
    while(leer != -1){
        eliminar_fin_de_linea(linea);
        char** strv = split(linea, ',');
        if(!strv) return false;
        clave_abb = gen_clave_abb(strv);
        if(!clave_abb) return false;
        if(hash_pertenece(hash, strv[INDICE_FLIGHT_NUMBER])){
            clave_aux = gen_clave_abb(hash_obtener(hash, strv[INDICE_FLIGHT_NUMBER]));
            abb_borrar(abb, clave_aux);
            free(clave_aux);
        }
        abb_guardar(abb, clave_abb, strv);
        hash_guardar(hash, strv[INDICE_FLIGHT_NUMBER], strv);
        free(clave_abb);
        leer = getline(&linea, &len, archivo);
    }

    fclose(archivo);
    free(linea);
    fprintf(stdout, "OK\n");
    return true;

}

void eliminar_fin_de_linea_strv(char** strv, size_t longitud){
    char* pos;
    if((pos = strchr(strv[longitud-1], '\n')) != NULL) *pos = '\0';
}

int check_comando_valido(char* comando){
    
    if((strcmp(comando, "agregar_archivo") == 0)) return VALOR_AGREGAR_ARCHIVO;
    else if((strcmp(comando, "ver_tablero") == 0)) return VALOR_VER_TABLERO;
    else if((strcmp(comando, "info_vuelo") == 0)) return VALOR_INFO_VUELO;
    else if((strcmp(comando, "prioridad_vuelos") == 0)) return VALOR_PRIORIDAD_VUELOS;
    else if((strcmp(comando, "borrar") == 0)) return VALOR_BORRAR;
    else return COMANDO_INVALIDO;

}

void leer_comando(char* linea, hash_t* hash, abb_t* abb){

    if(linea[0] == '\n') return;
    char** strv = split(linea, ' ');
    if(!strv) return;

    size_t longitud = 0;
    for(int i = 0; strv[i] != NULL; i++) longitud++;
    eliminar_fin_de_linea_strv(strv, longitud);

    switch(check_comando_valido(strv[0])){
        case VALOR_AGREGAR_ARCHIVO:
            if(longitud == 2){
                if(!comando_agregar_archivo(strv[1], hash, abb)) print_error(strv[0]);
            }
            else{
                print_error(strv[0]);
            }
            break;
        case VALOR_VER_TABLERO:
            if(longitud == 5){
                if(!comando_ver_tablero(strv[1], strv[2], strv[3], strv[4], abb)) print_error(strv[0]);
            }
            else{
                print_error(strv[0]);
            }
            break;
        case VALOR_INFO_VUELO:
            if(longitud == 2){
                if(!comando_info_vuelo(strv[1], hash)) print_error(strv[0]);
            }
            else{
                print_error(strv[0]);
            }
            break;
        case VALOR_PRIORIDAD_VUELOS:
            if(longitud == 2){
                if(!comando_prioridad_vuelos(strv[1], hash)) print_error(strv[0]);
            }
            else{
                print_error(strv[0]);
            }
            break;
        case VALOR_BORRAR:
            if(longitud == 3){
                if(!comando_borrar(strv[1], strv[2], hash, abb)) print_error(strv[0]);
            }
            else{
                print_error(strv[0]);
            }
            break;
        case COMANDO_INVALIDO:
            print_error(strv[0]);
            break;
    }

    free_strv(strv);

}

int main(int argc, char *argv[]){

    hash_t* hash = hash_crear(destruir_dato_wrapper);
    if(!hash) return ERROR;

    abb_t* abb = abb_crear(strcmp, NULL);
    if(!abb) return ERROR;

    char* linea = NULL;
    size_t len = 0;

    ssize_t leer = getline(&linea, &len, stdin);
    while(leer != -1){
        leer_comando(linea, hash, abb);
        leer = getline(&linea, &len, stdin);
    }

    free(linea);
    hash_destruir(hash);
    abb_destruir(abb);

    return EXITO;

}