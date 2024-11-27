#include "abb.h"
#include "abb_estructura_privada.h"

abb_t *abb_crear(int (*comparador)(void *, void *))
{
	if (comparador == NULL)
		return NULL;

	abb_t *abb = malloc(sizeof(abb_t));
	if (abb == NULL)
		return NULL;
	abb->raiz = NULL;
	abb->nodos = 0;
	abb->comparador = comparador;
	return abb;
}

void abb_destruir_nodos_recursivo(nodo_t *nodo, void (*destructor)(void *))
{
	if (nodo == NULL)
		return;

	abb_destruir_nodos_recursivo(nodo->izq, destructor);
	abb_destruir_nodos_recursivo(nodo->der, destructor);

	if (destructor)
		destructor(nodo->elemento);

	free(nodo);
}
void abb_destruir(abb_t *arbol)
{
	if (arbol == NULL)
		return;

	abb_destruir_nodos_recursivo(arbol->raiz, NULL);
	free(arbol);
}
void abb_destruir_todo(abb_t *abb, void (*destructor)(void *))
{
	if (abb == NULL)
		return;

	abb_destruir_nodos_recursivo(abb->raiz, destructor);
	free(abb);
}

nodo_t *abb_insertar_nodo(nodo_t *nodo, void *elemento,
			  int (*comparador)(void *, void *))
{
	if (nodo == NULL) {
		nodo_t *nuevo_nodo = malloc(sizeof(nodo_t));
		if (!nuevo_nodo)
			return NULL;

		nuevo_nodo->elemento = elemento;
		nuevo_nodo->izq = NULL;
		nuevo_nodo->der = NULL;
		return nuevo_nodo;
	}

	int comparacion = comparador(elemento, nodo->elemento);
	if (comparacion < 0) {
		nodo->izq = abb_insertar_nodo(nodo->izq, elemento, comparador);
	} else {
		nodo->der = abb_insertar_nodo(nodo->der, elemento, comparador);
	}

	return nodo;
}

bool abb_insertar(abb_t *abb, void *elemento)
{
	if (!abb || !abb->comparador)
		return false;

	nodo_t *nuevo_raiz =
		abb_insertar_nodo(abb->raiz, elemento, abb->comparador);
	if (!nuevo_raiz)
		return false;

	abb->raiz = nuevo_raiz;
	abb->nodos++;
	return true;
}

static nodo_t *abb_quitar_nodo(nodo_t *nodo, void *buscado, void **encontrado,
			       int (*comparador)(void *, void *), abb_t *abb)
{
	if (nodo == NULL) {
		if (encontrado)
			*encontrado = NULL;
		return NULL;
	}

	int comparacion = comparador(buscado, nodo->elemento);

	if (comparacion < 0) {
		nodo->izq = abb_quitar_nodo(nodo->izq, buscado, encontrado,
					    comparador, abb);
	} else if (comparacion > 0) {
		nodo->der = abb_quitar_nodo(nodo->der, buscado, encontrado,
					    comparador, abb);
	} else {
		if (encontrado)
			*encontrado = nodo->elemento;

		if (!nodo->izq && !nodo->der) {
			free(nodo);
			abb->nodos--;
			return NULL;
		}

		if (nodo->izq && nodo->der) {
			nodo_t *maximo = nodo->izq;
			while (maximo->der) {
				maximo = maximo->der;
			}
			nodo->elemento = maximo->elemento;
			nodo->izq = abb_quitar_nodo(nodo->izq, maximo->elemento,
						    NULL, comparador, abb);
			return nodo;
		}

		nodo_t *temp = nodo;
		nodo = (nodo->izq) ? nodo->izq : nodo->der;
		free(temp);
		abb->nodos--;
	}

	return nodo;
}

bool abb_quitar(abb_t *abb, void *buscado, void **encontrado)
{
	if (!abb || !abb->comparador)
		return false;

	if (encontrado)
		*encontrado = NULL;

	size_t nodos_antes = abb->nodos;

	abb->raiz = abb_quitar_nodo(abb->raiz, buscado, encontrado,
				    abb->comparador, abb);

	return abb->nodos < nodos_antes;
}

void *abb_buscar_nodo(nodo_t *nodo, void *elemento,
		      int (*comparador)(void *, void *))
{
	if (nodo == NULL)
		return NULL;

	int comparacion = comparador(elemento, nodo->elemento);
	if (comparacion == 0) {
		return nodo->elemento;
	} else if (comparacion < 0) {
		return abb_buscar_nodo(nodo->izq, elemento, comparador);
	} else {
		return abb_buscar_nodo(nodo->der, elemento, comparador);
	}
}

void *abb_obtener(abb_t *abb, void *elemento)
{
	if (!abb || abb->raiz == NULL)
		return NULL;

	return abb_buscar_nodo(abb->raiz, elemento, abb->comparador);
}

size_t abb_cantidad(abb_t *abb)
{
	if (!abb)
		return 0;

	return abb->nodos;
}

static size_t abb_iterar_inorden_aux(nodo_t *nodo, bool (*f)(void *, void *),
				     void *ctx, bool *continuar)
{
	if (!nodo || !(*continuar))
		return 0;

	size_t invocados = 0;

	invocados += abb_iterar_inorden_aux(nodo->izq, f, ctx, continuar);

	if (!(*continuar))
		return invocados;

	if (f(nodo->elemento, ctx)) {
		invocados++;
	} else {
		*continuar = false;
		invocados++;
	}

	invocados += abb_iterar_inorden_aux(nodo->der, f, ctx, continuar);

	return invocados;
}

size_t abb_iterar_inorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !f)
		return 0;

	bool continuar = true;
	size_t invocados = 0;

	invocados += abb_iterar_inorden_aux(abb->raiz, f, ctx, &continuar);

	return invocados;
}

static size_t abb_iterar_preorden_aux(nodo_t *nodo, bool (*f)(void *, void *),
				      void *ctx, bool *continuar)
{
	if (!nodo || !(*continuar))
		return 0;

	size_t invocados = 0;

	if (f(nodo->elemento, ctx)) {
		invocados++;
	} else {
		*continuar = false;
		invocados++;
	}

	if (!(*continuar))
		return invocados;

	invocados += abb_iterar_preorden_aux(nodo->izq, f, ctx, continuar);
	invocados += abb_iterar_preorden_aux(nodo->der, f, ctx, continuar);

	return invocados;
}

size_t abb_iterar_preorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !f)
		return 0;

	bool continuar = true;
	size_t invocados = 0;

	invocados += abb_iterar_preorden_aux(abb->raiz, f, ctx, &continuar);

	return invocados;
}

static size_t abb_iterar_postorden_aux(nodo_t *nodo, bool (*f)(void *, void *),
				       void *ctx, bool *continuar)
{
	if (!nodo || !(*continuar))
		return 0;

	size_t invocados = 0;

	invocados += abb_iterar_postorden_aux(nodo->izq, f, ctx, continuar);

	if (!(*continuar))
		return invocados;

	invocados += abb_iterar_postorden_aux(nodo->der, f, ctx, continuar);

	if (!(*continuar))
		return invocados;

	if (f(nodo->elemento, ctx)) {
		invocados++;
	} else {
		*continuar = false;
		invocados++;
	}

	return invocados;
}

size_t abb_iterar_postorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (!abb || !f)
		return 0;

	bool continuar = true;
	size_t invocados = 0;

	invocados += abb_iterar_postorden_aux(abb->raiz, f, ctx, &continuar);

	return invocados;
}

static size_t abb_vectorizar_inorden_aux(nodo_t *nodo, void **vector,
					 size_t tamaño, size_t *index)
{
	if (!nodo || *index >= tamaño)
		return 0;

	size_t guardados = 0;

	guardados +=
		abb_vectorizar_inorden_aux(nodo->izq, vector, tamaño, index);

	if (*index < tamaño) {
		vector[(*index)++] = nodo->elemento;
		guardados++;
	}

	guardados +=
		abb_vectorizar_inorden_aux(nodo->der, vector, tamaño, index);

	return guardados;
}

size_t abb_vectorizar_inorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !vector)
		return 0;
	size_t index = 0;
	return abb_vectorizar_inorden_aux(abb->raiz, vector, tamaño, &index);
}

static size_t abb_vectorizar_preorden_aux(nodo_t *nodo, void **vector,
					  size_t tamaño, size_t *index)
{
	if (!nodo || *index >= tamaño)
		return 0;

	size_t guardados = 0;

	if (*index < tamaño) {
		vector[(*index)++] = nodo->elemento;
		guardados++;
	}

	guardados +=
		abb_vectorizar_preorden_aux(nodo->izq, vector, tamaño, index);

	guardados +=
		abb_vectorizar_preorden_aux(nodo->der, vector, tamaño, index);

	return guardados;
}

size_t abb_vectorizar_preorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !vector)
		return 0;
	size_t index = 0;
	return abb_vectorizar_preorden_aux(abb->raiz, vector, tamaño, &index);
}

static size_t abb_vectorizar_postorden_aux(nodo_t *nodo, void **vector,
					   size_t tamaño, size_t *index)
{
	if (!nodo || *index >= tamaño)
		return 0;

	size_t guardados = 0;

	guardados +=
		abb_vectorizar_postorden_aux(nodo->izq, vector, tamaño, index);

	guardados +=
		abb_vectorizar_postorden_aux(nodo->der, vector, tamaño, index);

	if (*index < tamaño) {
		vector[(*index)++] = nodo->elemento;
		guardados++;
	}

	return guardados;
}

size_t abb_vectorizar_postorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (!abb || !vector)
		return 0;
	size_t index = 0;
	return abb_vectorizar_postorden_aux(abb->raiz, vector, tamaño, &index);
}