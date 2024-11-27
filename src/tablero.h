#ifndef __TDA_TABLERO_H__
#define __TDA_TABLERO_H__

#include <stdbool.h>
#include <stdlib.h>

typedef struct tablero tablero_t;

/**
 * Crea un tablero con el numero de filas y columnas indicado
 * Inicializa todas las celdas con NULL
 */
tablero_t *tablero_crear(size_t filas, size_t columnas);

/**
 * Destruye el tablero liberando toda la memoria
 * Si se pasa un destructor, lo aplicara a cada elemento de las celdas
 */
void tablero_destruir(tablero_t *tablero, void (*destructor)(void *));

/**
 * Agrega o reemplaza un elemento en una posicion especifica del tablero
 */
bool tablero_agregar(tablero_t *tablero, size_t fila, size_t columna, void *elemento);

/**
 * Obtiene el elemento almacenado en una posicion especifica del tablero
 */
void *tablero_obtener(tablero_t *tablero, size_t fila, size_t columna);

/**
 * Elimina el elemento en una posicion especifica del tablero
 * Devuelve el puntero al elemento eliminado
 */
void *tablero_eliminar(tablero_t *tablero, size_t fila, size_t columna);

/**
 * Imprime el tablero en la consola.
 * Requiere una funcion de callback para definir como se imprime cada celda
 */
void tablero_mostrar(tablero_t *tablero, void (*imprimir_celda)(void *));

#endif // __TDA_TABLERO_H__
