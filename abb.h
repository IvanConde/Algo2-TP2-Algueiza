#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

// Crea el abb.
// Post: devuelve un abb vacío.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda un elemento en el abb, si la clave ya se encuentra en la estructura, la reemplaza. De no poder guardarlo devuelve false.
// Pre: La estructura abb fue inicializada.
// Post: Se almacenó el par (clave, dato).
bool abb_guardar(abb_t* arbol, const char* clave, void *dato);

// Borra un elemento del abb y devuelve el dato asociado. Devuelve NULL si el dato no estaba.
// Pre: La estructura abb fue inicializada.
// Post: El elemento fue borrado de la estructura y se lo devolvió, en el caso de que estuviera guardado.
void* abb_borrar(abb_t* arbol, const char* clave);

// Obtiene el valor de un elemento del abb, si la clave no se encuentra devuelve NULL.
// Pre: La estructura abb fue inicializada.
void* abb_obtener(const abb_t* arbol, const char* clave);

// Determina si clave pertenece o no al abb.
// Pre: La estructura abb fue inicializada.
bool abb_pertenece(const abb_t* arbol, const char* clave);

// Devuelve la cantidad de elementos del abb.
// Pre: La estructura abb fue inicializada.
size_t abb_cantidad(abb_t* arbol);

// Destruye la estructura liberando la memoria pedida y llamando a la función destruir para cada par (clave, dato).
// Pre: La estructura abb fue inicializada.
// Post: La estructura abb fue destruida.
void abb_destruir(abb_t* arbol);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador.
// Pre: el abb fue creado.
// Post: devuelve un iterador inicializado y en la raiz del abb.
abb_iter_t* abb_iter_in_crear(const abb_t* arbol, const char* desde, const char* hasta, const char* orden );

// Avanza el iterador al siguiente elemento del abb.
// Pre: el iterador fue creado.
// Post: el iterador avanzó de posicion.
bool abb_iter_in_avanzar(abb_iter_t* iter);

// Devuelve clave actual, esa clave no se puede modificar ni liberar.
// Pre: el iterador fue creado.
// Post: se devolvió la clave actual del abb, cuando no está vacío.
const char* abb_iter_in_ver_actual(const abb_iter_t* iter);

// Devuelve verdadero si el iterador esta al final del abb.
// Pre: el iterador fue creado.
// Post: se devuelve true si el iterador esta al final del abb, caso contrario falso.
bool abb_iter_in_al_final(const abb_iter_t* iter);

// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: se destruyo el iterador.
void abb_iter_in_destruir(abb_iter_t* iter);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Itera todo el abb, excepto en caso de corte, provida siguiendo las operaciones indicadas segun la funcion visitar.
// Pre: El abb fue creado, visitar es una funcion capaz de realizar lo que el usuario tenga intencionado,
// extra es opcional segun la funcion visitar.
// Post: la funcion itero por todo el abb, excepto en caso de corte, realizando las operaciones indicadas segun 
// la funcion visitar e interactuando con la variable en extra si es el caso como es debido.
void abb_in_order(abb_t* arbol, bool visitar(const char*, void*, void*), void* extra);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en abb.h
// (esto es, las pruebas no pueden acceder a los miembros del struct abb).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_abb_alumno(void);

#endif // ABB_H