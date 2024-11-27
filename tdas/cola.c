#include "cola.h"
#include "lista.h"
#include <stdlib.h>

struct cola {
	Lista *lista;
};

Cola *cola_crear()
{
	Cola *cola = malloc(sizeof(cola));
	if (!cola)
		return NULL;

	cola->lista = lista_crear();
	if (!cola->lista) {
		free(cola);
		return NULL;
	}

	return cola;
}

void cola_destruir(Cola *cola)
{
	if (cola) {
		lista_destruir(cola->lista);
		free(cola);
	}
}
void cola_destruir_todo(Cola *cola, void (*destructor)(void *))
{
	if (cola) {
		lista_destruir_todo(cola->lista, destructor);
		free(cola);
	}
}
size_t cola_cantidad(Cola *cola)
{
	if (!cola)
		return 0;
	return lista_cantidad_elementos(cola->lista);
}
void *cola_frente(Cola *cola)
{
	if (!cola || lista_cantidad_elementos(cola->lista) == 0)
		return NULL;
	void *elemento;
	lista_obtener_elemento(cola->lista, 0, &elemento);
	return elemento;
}
bool cola_encolar(Cola *cola, void *elemento)
{
	if (!cola || !elemento)
		return false;
	return lista_agregar_al_final(cola->lista, elemento);
}
void *cola_desencolar(Cola *cola)
{
	if (!cola || lista_cantidad_elementos(cola->lista) == 0)
		return NULL;
	void *elemento;
	if (lista_quitar_elemento(cola->lista, 0, &elemento)) {
		return elemento;
	}
	return NULL;
}
bool cola_esta_vacía(Cola *cola)
{
	if (!cola)
		return true;
	return lista_cantidad_elementos(cola->lista) == 0;
}