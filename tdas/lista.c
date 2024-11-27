#include "lista.h"
#include <stdlib.h>
#include <stdio.h>

#define VACIO 0
#define ERROR 0
#define ERROR_1 NULL

typedef struct nodo {
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct lista {
	nodo_t *nodo_inicial;
	nodo_t *nodo_final;
	size_t cantidad;
};
struct lista_iterador {
	nodo_t *actual;
	Lista *lista;
};

Lista *lista_crear()
{
	return calloc(1, sizeof(Lista));
}

void lista_destruir(Lista *puntero_lista)
{
	if (puntero_lista == ERROR_1)
		return;

	nodo_t *nodo_actual = puntero_lista->nodo_inicial;
	nodo_t *nodo_siguiente;

	while (nodo_actual != ERROR_1) {
		nodo_siguiente = nodo_actual->siguiente;
		free(nodo_actual);
		nodo_actual = nodo_siguiente;
	}
	free(puntero_lista);
}

void lista_destruir_todo(Lista *puntero_lista, void (*destructor)(void *))
{
	if (puntero_lista == ERROR_1)
		return;
	nodo_t *nodo_actual = puntero_lista->nodo_inicial;
	nodo_t *nodo_siguiente;
	while (nodo_actual != ERROR_1) {
		nodo_siguiente = nodo_actual->siguiente;
		if (destructor != ERROR_1 && nodo_actual->elemento != ERROR_1) {
			destructor(nodo_actual->elemento);
		}
		free(nodo_actual);
		nodo_actual = nodo_siguiente;
	}
	free(puntero_lista);
}

size_t lista_cantidad_elementos(Lista *puntero_lista)
{
	if (puntero_lista == ERROR_1)
		return VACIO;
	return puntero_lista->cantidad;
}
bool lista_agregar_elemento(Lista *lista, size_t posicion, void *cosa)
{
	if (lista == NULL || posicion > lista->cantidad)
		return false;

	nodo_t *nuevo_nodo = malloc(sizeof(nodo_t));
	if (nuevo_nodo == NULL)
		return false;

	nuevo_nodo->elemento = cosa;
	nuevo_nodo->siguiente = NULL;

	if (posicion == 0) {
		nuevo_nodo->siguiente = lista->nodo_inicial;
		lista->nodo_inicial = nuevo_nodo;
		if (lista->cantidad == 0) {
			lista->nodo_final = nuevo_nodo;
		}
	} else {
		nodo_t *nodo_actual = lista->nodo_inicial;
		for (size_t i = 0; i < posicion - 1; i++) {
			nodo_actual = nodo_actual->siguiente;
		}

		nuevo_nodo->siguiente = nodo_actual->siguiente;
		nodo_actual->siguiente = nuevo_nodo;

		if (nuevo_nodo->siguiente == NULL) {
			lista->nodo_final = nuevo_nodo;
		}
	}

	lista->cantidad++;
	return true;
}

bool lista_agregar_al_final(Lista *lista, void *cosa)
{
	if (lista == NULL)
		return false;

	nodo_t *nodo_alfinal = malloc(sizeof(nodo_t));
	if (nodo_alfinal == NULL)
		return false;

	nodo_alfinal->elemento = cosa;
	nodo_alfinal->siguiente = NULL;

	if (lista->cantidad == 0) {
		lista->nodo_inicial = nodo_alfinal;
		lista->nodo_final = nodo_alfinal;
	} else {
		lista->nodo_final->siguiente = nodo_alfinal;
		lista->nodo_final = nodo_alfinal;
	}

	lista->cantidad++;
	return true;
}

bool lista_obtener_elemento(Lista *lista, size_t posicion,
			    void **elemento_encontrado)
{
	if (lista == NULL || posicion >= lista->cantidad) {
		return false;
	}

	nodo_t *nodo_actual = lista->nodo_inicial;
	for (size_t i = 0; i < posicion; i++) {
		nodo_actual = nodo_actual->siguiente;
	}

	if (elemento_encontrado != NULL) {
		*elemento_encontrado = nodo_actual->elemento;
	}

	return true;
}

bool lista_quitar_elemento(Lista *lista, size_t posicion,
			   void **elemento_quitado)
{
	if (lista == NULL || posicion >= lista->cantidad)
		return false;

	nodo_t *nodo_eliminar = lista->nodo_inicial;
	nodo_t *nodo_anterior = NULL;

	if (posicion == 0) {
		lista->nodo_inicial = nodo_eliminar->siguiente;
		if (elemento_quitado) {
			*elemento_quitado = nodo_eliminar->elemento;
		}
		free(nodo_eliminar);
	} else {
		for (size_t i = 0; i < posicion; i++) {
			nodo_anterior = nodo_eliminar;
			nodo_eliminar = nodo_eliminar->siguiente;
		}

		nodo_anterior->siguiente = nodo_eliminar->siguiente;

		if (elemento_quitado) {
			*elemento_quitado = nodo_eliminar->elemento;
		}

		if (nodo_eliminar == lista->nodo_final) {
			lista->nodo_final = nodo_anterior;
		}

		free(nodo_eliminar);
	}

	lista->cantidad--;

	return true;
}

void *lista_buscar_elemento(Lista *lista, void *buscado,
			    int (*comparador)(void *, void *))
{
	if (lista == NULL || comparador == NULL)
		return NULL;

	nodo_t *nodo_actual = lista->nodo_inicial;
	while (nodo_actual != NULL) {
		if (comparador(nodo_actual->elemento, buscado) == VACIO) {
			return nodo_actual->elemento;
		}
		nodo_actual = nodo_actual->siguiente;
	}

	return NULL;
}

size_t lista_iterar_elementos(Lista *lista, bool (*f)(void *, void *),
			      void *ctx)
{
	if ((lista == NULL) || (f == NULL) ||
	    (lista_cantidad_elementos(lista) == VACIO))
		return ERROR;

	int contador = 0;
	nodo_t *auxiliar = lista->nodo_inicial;
	bool avanzar = true;
	while (auxiliar && avanzar) {
		avanzar = f(auxiliar->elemento, ctx);
		contador++;
		auxiliar = auxiliar->siguiente;
	}
	return (size_t)contador;
}

Lista_iterador *lista_iterador_crear(Lista *lista)
{
	if (!lista)
		return ERROR_1;

	Lista_iterador *iterador = malloc(sizeof(Lista_iterador));
	if (!iterador)
		return NULL;
	iterador->actual = lista->nodo_inicial;
	iterador->lista = lista;
	return iterador;
}

bool lista_iterador_hay_siguiente(Lista_iterador *iterador)
{
	if (!iterador) {
		return false;
	}
	return (iterador->actual != NULL);
}

void lista_iterador_avanzar(Lista_iterador *iterador)
{
	if ((!iterador) || (!iterador->actual))
		return;
	iterador->actual = iterador->actual->siguiente;
}

void *lista_iterador_obtener_elemento_actual(Lista_iterador *iterador)
{
	if ((!iterador) || (!iterador->actual))
		return ERROR_1;
	return iterador->actual->elemento;
}

void lista_iterador_destruir(Lista_iterador *iterador)
{
	free(iterador);
}