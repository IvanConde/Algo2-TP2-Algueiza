#include "heap.h"
#include <stdlib.h>

#define CAPACIDAD_INICIAL 20

#define REDIMENSIONAR_ARRIBA 2
#define REDIMENSIONAR_ABAJO 2
#define CONDICION_REDIMENSIONAR_ABAJO 4

struct heap{
    void** datos;
    size_t cant;
    size_t capacidad;
    cmp_func_t cmp;
};

/* ******************************************************************
 *                    PRIMITIVAS PRIVADAS
 * *****************************************************************/

bool redimensionar_heap(heap_t* heap, size_t nueva_capacidad){

    void** datos_nuevo = realloc(heap->datos, (nueva_capacidad * sizeof(void*)));
    if((nueva_capacidad > 0) && (datos_nuevo == NULL)){
        return false;
    }

    heap->datos = datos_nuevo;
    heap->capacidad = nueva_capacidad;

    return true;

}

size_t obtener_padre(size_t posicion){
    return ((posicion-1)/2);
}

size_t obtener_hijo_der(size_t posicion){
    return (2*posicion+2);
}

size_t obtener_hijo_izq(size_t posicion){
    return (2*posicion+1);
}

void swap(void** x, void** y){
	void* aux = *x;
	*x = *y;
	*y = aux;
}

void upheap(void** datos, size_t posicion, cmp_func_t cmp){
    
    if(posicion == 0) return;
    size_t padre = obtener_padre(posicion);
    if(cmp(datos[padre], datos[posicion]) < 0){
        swap(&datos[padre], &datos[posicion]);
        upheap(datos, padre, cmp);
    }

}

void downheap(void** datos, size_t cantidad, size_t posicion, cmp_func_t cmp){

    if(posicion >= cantidad) return;
    size_t max = posicion;
    size_t izq = obtener_hijo_izq(posicion);
    size_t der = obtener_hijo_der(posicion);

    if(izq < cantidad && cmp(datos[izq], datos[max]) > 0){
        max = izq;
    }
    if(der < cantidad && cmp(datos[der], datos[max]) > 0){
        max = der;
    }
    if(max != posicion){
        swap(&datos[max], &datos[posicion]);
        downheap(datos, cantidad, max, cmp);
    }

}

void heapify(void** datos, size_t cantidad, cmp_func_t cmp){

    if(cantidad == 1) return;
    for(size_t i = (cantidad/2); i > 0; i--){
        downheap(datos, cantidad, i-1, cmp);
    }

}

/* ******************************************************************
 *                    PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t* heap_crear(cmp_func_t cmp){

    heap_t* heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->datos = malloc(sizeof(void*)*CAPACIDAD_INICIAL);
    if(!(heap->datos)){
        free(heap);
        return NULL;
    }
    heap->cant = 0;
    heap->capacidad = CAPACIDAD_INICIAL;
    heap->cmp = cmp;

    return heap;

}

heap_t* heap_crear_arr(void* arreglo[], size_t n, cmp_func_t cmp){

    heap_t* heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->datos = malloc(sizeof(void*)*n);
    if(!(heap->datos)){
        free(heap);
        return NULL;
    }

    for(size_t i = 0; i < n; i++){
        heap->datos[i] = arreglo[i];
    }
    heapify(heap->datos, n, cmp);

    heap->cant = n;
    heap->capacidad = n;
    heap->cmp = cmp;

    return heap;

}

void heap_destruir(heap_t* heap, void destruir_elemento(void* e)){

    while(!heap_esta_vacio(heap)){
        void* dato = heap_desencolar(heap);
        if(destruir_elemento != NULL) destruir_elemento(dato);
    }
    free(heap->datos);
    free(heap);

}

size_t heap_cantidad(const heap_t* heap){
    return heap->cant;
}

bool heap_esta_vacio(const heap_t* heap){
    return (heap->cant == 0);
}

bool heap_encolar(heap_t* heap, void* elem){

    if(heap->cant == heap->capacidad){
        if(!redimensionar_heap(heap, heap->capacidad*REDIMENSIONAR_ARRIBA)){
            return false;
        }
    }

    heap->datos[heap->cant] = elem;
    upheap(heap->datos, heap->cant, heap->cmp);
    heap->cant++;
    return true;

}

void* heap_ver_max(const heap_t* heap){
    if(heap_esta_vacio(heap)) return NULL;
    return heap->datos[0];
}

void* heap_desencolar(heap_t* heap){

    if(heap_esta_vacio(heap)) return NULL;

    void* dato = heap->datos[0];
    heap->datos[0] = heap->datos[(heap->cant)-1];
    heap->cant--;
    downheap(heap->datos, heap->cant, 0, heap->cmp);

    if(((heap->capacidad/CONDICION_REDIMENSIONAR_ABAJO) >= heap->cant) && (heap->capacidad > CAPACIDAD_INICIAL)){
        redimensionar_heap(heap, (heap->capacidad/REDIMENSIONAR_ABAJO));
    }

    return dato;

}

/* ******************************************************************
 *                    PRIMITIVA DEL HEAPSORT
 * *****************************************************************/

void heap_sort(void* elementos[], size_t cant, cmp_func_t cmp){

    heapify(elementos, cant, cmp);

    for(size_t i = cant - 1; i > 0; i--){
        swap(&elementos[0], &elementos[i]);
        downheap(elementos, i, 0, cmp);
    }

}