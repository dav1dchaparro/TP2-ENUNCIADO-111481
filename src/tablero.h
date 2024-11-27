#ifndef __TDA_TABLERO_H__
#define __TDA_TABLERO_H__

#include <stdbool.h>
#include <string.h>

typedef struct tablero tablero_t;

/**
 * Crea un tablero 
 */
tablero_t *tablero_crear(size_t filas, size_t columnas);

/**
 * Libera lo asignado a tablero
 */
void tablero_destruir(tablero_t *tablero);

/**
 * Agrega un elemento al tablero en la posicion indicada
 */
bool tablero_agregar(tablero_t *tablero, size_t fila, size_t columna, void *elemento);

/**
 * Obtiene el elemento en una posición especifica
 */
void *tablero_obtener(tablero_t *tablero, size_t fila, size_t columna);

/**
 * Elimina el elemento en una posicion especifica
 */
void *tablero_eliminar(tablero_t *tablero, size_t fila, size_t columna);

/**
 * Imprime el tablero en la consola

 */
void tablero_mostrar(tablero_t *tablero);

#endif // __TDA_TABLERO_H__
