#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define CAPACIDAD_MINIMA 3
#define MAX_FACTOR_CARGA 0.75
#define VACIO 0

typedef struct casillero {
	void *elemento;
	char *clave;
	struct casillero *siguiente;
} casillero_t;

struct hash {
	casillero_t **tabla;
	size_t ocupado;
	size_t cant_elementos;
	size_t capacidad_inicial;
};
/*
*post: me devuelve un numero en base a la clave 
*/
uint32_t funcion_hash(const char *key)
{
	uint32_t hash, i;
	for (hash = i = 0; i < strlen(key); ++i) {
		hash += (unsigned char)key[i];

		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/*
*post: Realiza el respectivo rehash al hash pasado por parametro, duplicando su capacidad
*/
hash_t *rehash(hash_t *hash)
{
	hash_t *hash_nuevo = hash_crear(hash->capacidad_inicial * 2);
	if (!hash_nuevo)
		return NULL;
	void *anterior = NULL;

	for (int i = 0; i < hash->capacidad_inicial; i++) {
		casillero_t *auxiliar = hash->tabla[i];
		while (auxiliar) {
			hash_insertar(hash_nuevo, auxiliar->clave,
				      auxiliar->elemento, &anterior);
			auxiliar = auxiliar->siguiente;
		}
	}
	hash_t temporal = *hash;
	*hash = *hash_nuevo;
	*hash_nuevo = temporal;
	hash_destruir(hash_nuevo);
	return hash;
}
hash_t *hash_crear(size_t capacidad_inicial)
{
	hash_t *hash = malloc(sizeof(hash_t));
	if (!hash)
		return NULL;
	if (capacidad_inicial <= CAPACIDAD_MINIMA)
		hash->capacidad_inicial = CAPACIDAD_MINIMA;
	else
		hash->capacidad_inicial = capacidad_inicial;
	hash->cant_elementos = VACIO;
	hash->ocupado = VACIO;
	hash->tabla = calloc(hash->capacidad_inicial, sizeof(casillero_t));
	if (!hash->tabla) {
		free(hash);
		return NULL;
	}
	return hash;
}
size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return VACIO;
	return hash->cant_elementos;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if ((!hash))
		return;

	for (int i = 0; i < hash->capacidad_inicial; i++) {
		while (hash->tabla[i]) {
			casillero_t *auxiliar = hash->tabla[i]->siguiente;
			if (destructor)
				destructor(hash->tabla[i]->elemento);
			free(hash->tabla[i]->clave);
			free(hash->tabla[i]);
			hash->tabla[i] = auxiliar;
			hash->cant_elementos--;
		}
		free(hash->tabla[i]);
	}
	free(hash->tabla);
	free(hash);
}

/*
pre:-
*post: me devuelve un puntero reservado con malloc que contiene la copia de la clave
*/
char *duplicar_clave(const char *clave)
{
	if (!clave)
		return NULL;

	char *copia = malloc(strlen(clave) + 1);
	if (!copia)
		return NULL;
	strcpy(copia, clave);

	return copia;
}

/*
*post: Creo un casillero ya inicializado con su par "clave-valor"
*/
casillero_t *creo_casillero(const char *clave, void *elemento)
{
	casillero_t *nuevo_casillero = calloc(1, sizeof(casillero_t));
	if (!nuevo_casillero)
		return NULL;

	nuevo_casillero->clave = duplicar_clave(clave);
	if (!nuevo_casillero->clave)
		return NULL;
	nuevo_casillero->elemento = elemento;
	return nuevo_casillero;
}

/**
 * Inserta un casillero con el par "clave-valor" en la tabla de hash cuando no hay ninguno 
 * en la posición especificada de la tabla.
 *
 * Precondicion: `hash` y `clave` son validos. 
 * Postcondicion: Se crea un nuevo casillero en la posicion especificada y se actualizan
 * los contadores de elementos ocupados y totales.
 */
bool inserto_en_primera_posicion(hash_t *hash, size_t posicion, char *clave,
				 void *valor, void **encontrado)
{
	char *clave_copiada = malloc(strlen(clave) + 1);
	if (!clave_copiada)
		return false;

	memcpy(clave_copiada, clave, strlen(clave) + 1);

	casillero_t *nuevo_casillero = creo_casillero(clave_copiada, valor);
	if (!nuevo_casillero) {
		free(clave_copiada);
		return false;
	}

	if (encontrado)
		*encontrado = NULL;

	hash->tabla[posicion] = nuevo_casillero;
	hash->ocupado++;
	hash->cant_elementos++;

	return true;
}
///

/*
* post: inserta un casillero con el par "clave-valor" en el hash cuando hay un 
* casillero incializado en esa posicion de la tabla 
*/
hash_t *inserto_en_lista(hash_t *hash, casillero_t *posicion_anterior,
			 const char *clave, void *elemento, void **anterior)
{
	casillero_t *nuevo_casillero = creo_casillero(clave, elemento);
	if (!nuevo_casillero)
		return NULL;

	if (anterior)
		*anterior = NULL;
	posicion_anterior->siguiente = nuevo_casillero;
	hash->cant_elementos++;
	return hash;
}
bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (!hash || !clave)
		return false;
	if ((double)hash->ocupado / (double)hash->capacidad_inicial >
	    MAX_FACTOR_CARGA) {
		if (!rehash(hash))
			return false;
	}

	size_t posicion =
		((size_t)funcion_hash(clave)) % hash->capacidad_inicial;

	if (!hash->tabla[posicion]) {
		return inserto_en_primera_posicion(hash, posicion, clave, valor,
						   encontrado);
	}

	casillero_t *actual = hash->tabla[posicion];
	casillero_t *anterior = NULL;

	while (actual) {
		if (strcmp(actual->clave, clave) == 0) {
			if (encontrado)
				*encontrado = actual->elemento;
			actual->elemento = valor;
			return true;
		}
		anterior = actual;
		actual = actual->siguiente;
	}

	return inserto_en_lista(hash, anterior, clave, valor, encontrado);
}

void *hash_buscar(hash_t *hash, char *clave)
{
	if ((!hash) || (!clave))
		return NULL;

	size_t posicion = (size_t)funcion_hash(clave) % hash->capacidad_inicial;

	if (!hash->tabla[posicion])
		return NULL;

	casillero_t *auxiliar = hash->tabla[posicion];
	while ((auxiliar) && (strcmp(auxiliar->clave, clave) != 0)) {
		auxiliar = auxiliar->siguiente;
	}

	if (!auxiliar)
		return NULL;
	return auxiliar->elemento;
}

bool hash_contiene(hash_t *hash, char *clave)
{
	if ((!hash) || (!clave))
		return false;

	size_t posicion = (size_t)funcion_hash(clave) % hash->capacidad_inicial;

	if (!hash->tabla[posicion])
		return false;

	casillero_t *auxiliar = hash->tabla[posicion];
	while ((auxiliar) && (strcmp(auxiliar->clave, clave) != 0))
		auxiliar = auxiliar->siguiente;

	if (!auxiliar)
		return false;
	return true;
}

/*
* post: quita el casillero del hash y retorna el elemento que se ha extraido
*/
void *quito_de_lista(hash_t *hash, casillero_t *auxiliar, casillero_t *anterior)
{
	void *elemento_extraido = auxiliar->elemento;
	anterior->siguiente = auxiliar->siguiente;
	hash->cant_elementos--;
	free((char *)auxiliar->clave);
	free(auxiliar);
	return elemento_extraido;
}

/*
* post: quita de la primera posicion de la tabla del hash retornando el elemento que se ha extraido
*/
void *quito_de_primera_posicion(hash_t *hash, size_t posicion)
{
	casillero_t *primero = hash->tabla[posicion];
	void *elemento_extraido = primero->elemento;
	hash->tabla[posicion] = primero->siguiente;
	hash->cant_elementos--;
	hash->ocupado--;
	free((char *)primero->clave);
	free(primero);
	return elemento_extraido;
}

void *hash_quitar(hash_t *hash, char *clave)
{
	if ((!hash) || (!clave))
		return NULL;

	size_t posicion = (size_t)funcion_hash(clave) % hash->capacidad_inicial;
	casillero_t *anterior = NULL;
	casillero_t *auxiliar = hash->tabla[posicion];

	while (auxiliar && strcmp(auxiliar->clave, clave) != 0) {
		anterior = auxiliar;
		auxiliar = auxiliar->siguiente;
	}

	if (!auxiliar)
		return NULL;

	if (anterior) {
		return quito_de_lista(hash, auxiliar, anterior);
	}
	return quito_de_primera_posicion(hash, posicion);
}

size_t hash_iterar(hash_t *hash, bool (*f)(char *clave, void *valor, void *aux),
		   void *ctx)
{
	if ((!hash) || (!f))
		return 0;

	int i = 0;
	size_t contador = 0;
	bool sigue_iterando = true;

	while ((i < hash->capacidad_inicial) && (sigue_iterando)) {
		casillero_t *auxiliar = hash->tabla[i];
		if (auxiliar) {
			while (auxiliar && (sigue_iterando)) {
				contador++;
				if (f(auxiliar->clave, auxiliar->elemento,
				      ctx) == false) {
					sigue_iterando = false;
				}
				auxiliar = auxiliar->siguiente;
			}
		}
		i++;
	}

	return contador;
}