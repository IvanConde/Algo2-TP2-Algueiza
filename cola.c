#include "cola.h"
#include <stdlib.h>

typedef struct nodo{
    void* dato;
    struct nodo* siguiente;
}nodo_t;

struct cola{
    nodo_t* frente;
    nodo_t* fin;
};

cola_t* cola_crear(void){

    cola_t* cola = malloc(sizeof(cola_t));
    if(cola == NULL) return NULL;
    cola->frente = NULL;
    cola->fin = NULL;
    return cola;

}

void cola_destruir(cola_t *cola, void destruir_dato(void*)){

    while(!cola_esta_vacia(cola)){
        void* dato = cola_desencolar(cola);
        if(destruir_dato != NULL) destruir_dato(dato);
    }
    free(cola);

}

bool cola_esta_vacia(const cola_t *cola){

    return cola->frente == NULL;

}

nodo_t* crear_nodo_cola(void* valor, nodo_t* siguiente){

    nodo_t* nodo_nuevo = malloc(sizeof(nodo_t));
    if(!nodo_nuevo) return NULL;

    nodo_nuevo->siguiente = siguiente;
    nodo_nuevo->dato = valor;

    return nodo_nuevo;

}

bool cola_encolar(cola_t *cola, void* valor){

    nodo_t* nodo_nuevo = crear_nodo_cola(valor, NULL);
    if(!nodo_nuevo) return false;

    if(cola_esta_vacia(cola)){
        cola->frente = nodo_nuevo;
    }
    else{
        cola->fin->siguiente = nodo_nuevo;
    }

    cola->fin = nodo_nuevo;

    return true;

}

void* cola_ver_primero(const cola_t *cola){

    if(cola_esta_vacia(cola)) return NULL;

    return cola->frente->dato;

}

void* cola_desencolar(cola_t *cola){

    if(cola_esta_vacia(cola)) return NULL;

    void* dato = cola->frente->dato;
    nodo_t* aux = cola->frente;

    if(cola->frente == cola->fin){
        cola->frente = NULL;
        cola->fin = NULL;
    }
    else{
        cola->frente = cola->frente->siguiente;
    }

    free(aux);

    return dato;

}