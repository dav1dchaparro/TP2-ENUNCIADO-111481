#include "pila.h"
#include "lista.h"
#include <stdlib.h>

struct pila {
	Lista *lista;
};

Pila *pila_crear()
{
	Pila *pila = malloc(sizeof(Pila));
	if (!pila)
		return NULL;

	pila->lista = lista_crear();
	if (!pila->lista) {
		free(pila);
		return NULL;
	}

	return pila;
}

void pila_destruir(Pila *pila)
{
	if (!pila)
		return;
	lista_destruir(pila->lista);
	free(pila);
}

void pila_destruir_todo(Pila *pila, void (*f)(void *))
{
	if (!pila)
		return;

	lista_destruir_todo(pila->lista, f);
	free(pila);
}

size_t pila_cantidad(Pila *pila)
{
	if (!pila)
		return 0;
	return lista_cantidad_elementos(pila->lista);
}

void *pila_tope(Pila *pila)
{
	if (!pila || lista_cantidad_elementos(pila->lista) == 0)
		return NULL;

	void *elemento;
	lista_obtener_elemento(pila->lista,
			       lista_cantidad_elementos(pila->lista) - 1,
			       &elemento);
	return elemento;
}

bool pila_apilar(Pila *pila, void *elemento)
{
	if (!pila || !elemento)
		return false;

	return lista_agregar_al_final(pila->lista, elemento);
}

void *pila_desapilar(Pila *pila)
{
	if (!pila || lista_cantidad_elementos(pila->lista) == 0)
		return NULL;

	void *elemento;
	lista_quitar_elemento(pila->lista,
			      lista_cantidad_elementos(pila->lista) - 1,
			      &elemento);
	return elemento;
}

bool pila_esta_vacía(Pila *pila)
{
	if (!pila)
		return true;
	return (pila_cantidad(pila) == 0);
}