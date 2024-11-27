#include "tablero.h"
#include "tdas/lista.h"
#include <stdlib.h>
#include <stdio.h>

struct tablero {
    size_t filas;
    size_t columnas;
    Lista **celdas; 
};

tablero_t *tablero_crear(size_t filas, size_t columnas) {
    if (filas == 0 || columnas == 0)
        return NULL;

    tablero_t *tablero = malloc(sizeof(tablero_t));
    if (!tablero)
        return NULL;

    tablero->filas = filas;
    tablero->columnas = columnas;
    tablero->celdas = calloc(filas * columnas, sizeof(Lista *));
    if (!tablero->celdas) {
        free(tablero);
        return NULL;
    }

    for (size_t i = 0; i < filas * columnas; i++) {
        tablero->celdas[i] = lista_crear();
        if (!tablero->celdas[i]) {
            for (size_t j = 0; j < i; j++)
                lista_destruir(tablero->celdas[j]);
            free(tablero->celdas);
            free(tablero);
            return NULL;
        }
    }

    return tablero;
}

void tablero_destruir(tablero_t *tablero, void (*destructor)(void *)) {
    if (!tablero)
        return;

    for (size_t i = 0; i < tablero->filas * tablero->columnas; i++) {
        if (destructor) {
            while (!lista_esta_vacia(tablero->celdas[i])) {
                void *elemento = lista_borrar_primero(tablero->celdas[i]);
                destructor(elemento);
            }
        }
        lista_destruir(tablero->celdas[i]); 
    }
    free(tablero->celdas); 
    free(tablero); 
}


bool tablero_agregar(tablero_t *tablero, size_t fila, size_t columna, void *elemento) {
    if (!tablero || fila >= tablero->filas || columna >= tablero->columnas)
        return false;

    size_t posicion = fila * tablero->columnas + columna;
    return lista_insertar(tablero->celdas[posicion], elemento) == 0;
}

void *tablero_obtener(tablero_t *tablero, size_t fila, size_t columna) {
    if (!tablero || fila >= tablero->filas || columna >= tablero->columnas)
        return NULL;

    size_t posicion = fila * tablero->columnas + columna;
    return lista_primero(tablero->celdas[posicion]);
}

void *tablero_eliminar(tablero_t *tablero, size_t fila, size_t columna) {
    if (!tablero || fila >= tablero->filas || columna >= tablero->columnas)
        return NULL;

    size_t posicion = fila * tablero->columnas + columna;
    return lista_borrar_primero(tablero->celdas[posicion]);
}

void tablero_mostrar(tablero_t *tablero, void (*imprimir_celda)(void *)) {
    if (!tablero)
        return;

    for (size_t fila = 0; fila < tablero->filas; fila++) {
        for (size_t columna = 0; columna < tablero->columnas; columna++) {
            size_t posicion = fila * tablero->columnas + columna;
            void *elemento = lista_primero(tablero->celdas[posicion]);

            if (imprimir_celda && elemento) {
                imprimir_celda(elemento); 
            } else if (elemento) {
                printf("X"); 
            } else {
                printf("."); 
            }
        }
        printf("\n"); 
    }
}
