#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FACTOR_REDIMENSION_SUPERIOR 0.7
#define FACTOR_REDIMENSION_MINIMO 0.25
#define AUMENTO_REDIMENSIONAR 2

#define CAPACIDAD_INICIAL 50 

#define CONDICION_REINICIO 2

#define POSICION_INVALIDA -1

// DEFINICION DE LOS TIPOS DE DATOS

typedef enum estado{VACIO, OCUPADO, BORRADO} estado_t;

struct hash_iter{
    const hash_t* hash;
    size_t cursor;
};

typedef struct tabla_hash{
    char* clave;
    void* dato;
    estado_t estado;
}tabla_hash_t;

struct hash{
    size_t borrados;
    size_t capacidad;
    size_t cantidad;
    hash_destruir_dato_t funcion_destruir;
    tabla_hash_t* tabla_hash;
};

// Primitivas PRIVADAS.

void reposicionar_indice(size_t capacidad, size_t* indice, size_t* contador){

    if(*indice == capacidad && *contador < CONDICION_REINICIO){    
        *indice = 0;
        *contador += 1;
    }

}

bool clave_es_igual(tabla_hash_t* tabla, size_t indice, const char* clave){
    return(tabla[indice].estado == OCUPADO && strcmp(tabla[indice].clave, clave) == 0);
}

size_t obtener_posicion_valida(tabla_hash_t* tabla, size_t* indice, estado_t estado, const char* clave, size_t capacidad){

    size_t contador = 0;
    size_t posicion = POSICION_INVALIDA;
    bool encontre = false;

    while(*indice < capacidad){
        switch(estado){
            case VACIO:
                if(tabla[*indice].estado == VACIO){
                    posicion = *indice;
                    encontre = true;
                }
            break;
            case OCUPADO:
                if(clave_es_igual(tabla, *indice, clave)){
                    posicion = *indice;
                    encontre = true;
                }
            break;
            case BORRADO:
            break;
        }
        if(encontre) break;
        *indice += 1;
        reposicionar_indice(capacidad, indice, &contador);
    }

    return posicion;

}

float factor_carga(hash_t* hash){
    return (((float)hash->cantidad + (float)hash->borrados) / (float)hash->capacidad);
}

size_t funcion_hashing(const char *clave, size_t cota){

    size_t hash = 0;
    int c;

    while((c = *clave++)){
        hash = c + (hash<<6) + (hash<<16) - hash;
    }

    return hash % cota;

}

void asignar_valores_tabla(tabla_hash_t* tabla, size_t indice, char* clave, void* dato, estado_t estado){
    tabla[indice].clave = clave;
    tabla[indice].dato = dato;
    tabla[indice].estado = estado;
}


void inicializar_en_vacio(tabla_hash_t* tabla, size_t capacidad){
    for(int i = 0; i < capacidad; i++){
        tabla[i].estado = VACIO;
        tabla[i].dato = NULL;
    }
}

bool insertar_elemento(size_t nuevo_indice, size_t nueva_capacidad, hash_t* hash, int indice_hash, tabla_hash_t* nueva_tabla_hash){

    size_t indice = nuevo_indice;
    size_t posicion = obtener_posicion_valida(nueva_tabla_hash, &indice, VACIO, NULL, nueva_capacidad);
    if(posicion != POSICION_INVALIDA){
        asignar_valores_tabla(nueva_tabla_hash, posicion, hash->tabla_hash[indice_hash].clave, hash->tabla_hash[indice_hash].dato, OCUPADO);
        return true;
    }

    return false;

}

bool redimensionar_tabla_hash(hash_t* hash, size_t nueva_capacidad){

    tabla_hash_t* nueva_tabla_hash = malloc(sizeof(tabla_hash_t)*nueva_capacidad);
    if(!nueva_tabla_hash) return false;

    inicializar_en_vacio(nueva_tabla_hash, nueva_capacidad);

    for(int i = 0; i < hash->capacidad; i++){
        if(hash->tabla_hash[i].estado == OCUPADO){
            if(!insertar_elemento(funcion_hashing(hash->tabla_hash[i].clave,nueva_capacidad), nueva_capacidad, hash, i, nueva_tabla_hash))
                return false;
        }
    }

    hash->borrados = 0;
    hash->capacidad = nueva_capacidad;
    free(hash->tabla_hash);
    hash->tabla_hash = nueva_tabla_hash;
    
    return true;

}

void *borrar_campo_tabla(hash_t* hash, size_t indice){

    hash->tabla_hash[indice].estado = BORRADO;
    free(hash->tabla_hash[indice].clave);
    hash->borrados++;
    hash->cantidad--;
    return hash->tabla_hash[indice].dato;

}

size_t hash_obtener_indice(const hash_t* hash, const char* clave){
    
    if(hash->cantidad == 0)
        return POSICION_INVALIDA;

    size_t indice = funcion_hashing(clave,hash->capacidad);
    size_t posicion = obtener_posicion_valida(hash->tabla_hash, &indice, OCUPADO, clave, hash->capacidad);

    return posicion;

}

size_t siguiente_ocupado(tabla_hash_t* tabla, size_t capacidad, size_t posicion){
    for(size_t i = posicion; i < capacidad; i++)
        if(tabla[i].estado == OCUPADO) return i;
    return capacidad;
}

// Primitivas del HASH.

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){

    hash_t* hash = malloc(sizeof(hash_t));
    if(!hash) return NULL;
    hash->borrados = 0;
    hash->capacidad = CAPACIDAD_INICIAL;
    hash->cantidad = 0;
    hash->funcion_destruir = destruir_dato;
    hash->tabla_hash = malloc(sizeof(tabla_hash_t)*hash->capacidad);
    if(!hash->tabla_hash){
        free(hash);
        return NULL;
    }
    inicializar_en_vacio(hash->tabla_hash, hash->capacidad);
    return hash;

}

bool hash_guardar(hash_t* hash, const char* clave, void* dato){

    if(factor_carga(hash) >= FACTOR_REDIMENSION_SUPERIOR){
        if(!redimensionar_tabla_hash(hash, hash->capacidad*AUMENTO_REDIMENSIONAR)) 
            return false;
    }

    size_t indice = funcion_hashing(clave, hash->capacidad);
    size_t clave_repetida = hash_obtener_indice(hash, clave);
    
    if(clave_repetida != POSICION_INVALIDA){
        if(hash->funcion_destruir != NULL)
            hash->funcion_destruir(hash->tabla_hash[clave_repetida].dato);
        hash->tabla_hash[clave_repetida].dato = dato;
        return true;
    }

    size_t posicion = obtener_posicion_valida(hash->tabla_hash, &indice, VACIO, clave, hash->capacidad);
    if(posicion != POSICION_INVALIDA){
        asignar_valores_tabla(hash->tabla_hash, posicion, strdup(clave), dato, OCUPADO);
        hash->cantidad++;
        return true;
    }

    return false;

}

void* hash_borrar(hash_t* hash, const char* clave){

    if(hash->cantidad == 0){
        return NULL;
    }

    size_t indice = funcion_hashing(clave,hash->capacidad);
    size_t posicion = obtener_posicion_valida(hash->tabla_hash, &indice, OCUPADO, clave, hash->capacidad);
    if(posicion == POSICION_INVALIDA) return NULL;
    void *dato = borrar_campo_tabla(hash, posicion);

    if(factor_carga(hash) <= FACTOR_REDIMENSION_MINIMO && (hash->capacidad > CAPACIDAD_INICIAL))
        redimensionar_tabla_hash(hash, hash->capacidad/AUMENTO_REDIMENSIONAR);

    return dato;

}

void* hash_obtener(const hash_t* hash, const char* clave){
    
    if(hash->cantidad == 0)
        return NULL;

    size_t indice = funcion_hashing(clave,hash->capacidad);
    size_t posicion = obtener_posicion_valida(hash->tabla_hash, &indice, OCUPADO, clave, hash->capacidad);

    if(posicion != POSICION_INVALIDA) return (hash->tabla_hash[indice].dato);
    return NULL;

}

bool hash_pertenece(const hash_t* hash, const char* clave){

    size_t indice = funcion_hashing(clave,hash->capacidad);
    size_t posicion = obtener_posicion_valida(hash->tabla_hash, &indice, OCUPADO, clave, hash->capacidad);

    if(posicion != POSICION_INVALIDA) return true;
    return false;

}

size_t hash_cantidad(const hash_t* hash){
    return hash->cantidad;
}

void hash_destruir(hash_t* hash){

    for(int i = 0; i < hash->capacidad; i++){
        if((hash->tabla_hash[i].estado == OCUPADO) && (hash->funcion_destruir != NULL)){
            hash->funcion_destruir(hash->tabla_hash[i].dato);
        }
        if(hash->tabla_hash[i].estado == OCUPADO)  
            free(hash->tabla_hash[i].clave);
    }

    free(hash->tabla_hash);
    free(hash);

}

// Primitivas del ITERADOR DEL HASH

hash_iter_t *hash_iter_crear(const hash_t* hash) {

    hash_iter_t* iter = malloc(sizeof(hash_iter_t));
    if(iter == NULL) return NULL;
    size_t cursor = siguiente_ocupado(hash->tabla_hash, hash->capacidad, 0);
    iter->cursor = cursor;
    iter->hash = hash;
    return iter;

}

bool hash_iter_avanzar(hash_iter_t* iter){
    if(hash_iter_al_final(iter)) return false;
    iter->cursor = siguiente_ocupado(iter->hash->tabla_hash, iter->hash->capacidad, iter->cursor+1); 
    return true;
}

const char* hash_iter_ver_actual(const hash_iter_t* iter) {
    if(hash_iter_al_final(iter)) return NULL;
    return iter->hash->tabla_hash[iter->cursor].clave;
}

bool hash_iter_al_final(const hash_iter_t* iter) {
    return iter->cursor == iter->hash->capacidad;
}

void hash_iter_destruir(hash_iter_t* iter) {
    free(iter);
}
