#include "pila.h"
#include <stdlib.h>

#define CAPACIDAD_INICIAL 10
#define CANTIDAD_INICIAL 0

#define REDIMENSIONAR_ARRIBA 2
#define REDIMENSIONAR_ABAJO 2
#define CONDICION_REDIMENSIONAR_ABAJO 4

/* Definición del struct pila proporcionado por la cátedra.
 */

struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

bool redimensionar_pila(pila_t* pila, size_t nueva_capacidad){

    void** datos_nuevo = realloc(pila->datos, (nueva_capacidad * sizeof(void*)));
    if((nueva_capacidad > 0) && (datos_nuevo == NULL)){
        return false;
    }

    pila->datos = datos_nuevo;
    pila->capacidad = nueva_capacidad;

    return true;

}

pila_t* pila_crear(void){

    pila_t* pila = malloc(sizeof(pila_t));
    if(pila == NULL){
        return NULL;
    }

    pila->datos = malloc(sizeof(void*)*CAPACIDAD_INICIAL);
    if(pila->datos == NULL){
        free(pila);
        return NULL;
    }

    pila->capacidad = CAPACIDAD_INICIAL;
    pila->cantidad = CANTIDAD_INICIAL;

    return pila;

}

void pila_destruir(pila_t *pila){

    free(pila->datos);
    free(pila);

}

bool pila_esta_vacia(const pila_t *pila){

    return (pila->cantidad == 0);

}

bool pila_apilar(pila_t *pila, void* valor){

    if(pila->capacidad == pila->cantidad){
        if(!redimensionar_pila(pila, pila->capacidad*REDIMENSIONAR_ARRIBA)){
            return false;
        }
    }

    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;

    return true;

}

void* pila_ver_tope(const pila_t *pila){

    if(pila_esta_vacia(pila)){
        return NULL;
    }

    return pila->datos[pila->cantidad-1];

}

void* pila_desapilar(pila_t *pila){

    if(pila_esta_vacia(pila)){
        return NULL;
    }

    void* valor_cantidad_anterior = pila_ver_tope(pila);
    pila->cantidad--;

    if(((pila->capacidad/CONDICION_REDIMENSIONAR_ABAJO) >= pila->cantidad) && (pila->capacidad > CAPACIDAD_INICIAL)){
        redimensionar_pila(pila, (pila->capacidad/REDIMENSIONAR_ABAJO));
    }

    return valor_cantidad_anterior;

}

// ...