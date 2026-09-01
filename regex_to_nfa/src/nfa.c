#include "nfa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Capacidad inicial del arreglo de estados. */
#define INITIAL_CAPACITY 32

/*
 * Un fragmento de NFA en construccion: el resultado parcial que queda
 * en la pila mientras se aplica el algoritmo de Thompson.
 */
typedef struct
{
    int start;
    int accept;
} fragment;

/*
 * Crea un estado nuevo dentro del automata y devuelve su indice.
 * Duplica la capacidad del arreglo cuando se queda sin espacio.
 *
 * Devuelve NO_STATE si no se pudo reservar memoria.
 *
 * TODO: implementar.
 */
static int new_state(nfa *n, char symbol, int out1, int out2)
{
    (void)n;
    (void)symbol;
    (void)out1;
    (void)out2;

    return NO_STATE;
}

nfa regex_to_nfa(regex r)
{
    nfa n;
    n.states = NULL;
    n.count = 0;
    n.capacity = 0;
    n.start = NO_STATE;
    n.accept = NO_STATE;

    /*
     * Algoritmo de Thompson.
     *
     * Se recorre la expresion en postfijo con una pila de fragmentos:
     *
     *   - simbolo: crea dos estados unidos por una transicion por el simbolo
     *   - concatenacion: desapila dos fragmentos y enlaza el final del
     *     primero con el inicio del segundo mediante epsilon
     *   - union: desapila dos fragmentos, crea un estado inicial que
     *     bifurca hacia ambos y un estado final al que ambos llegan
     *   - cerradura: desapila un fragmento y crea un estado que bifurca
     *     hacia el fragmento y hacia el nuevo estado final
     *
     * Al terminar debe quedar exactamente un fragmento en la pila: ese
     * define el estado inicial y el de aceptacion del automata.
     *
     * TODO: implementar.
     */
    (void)r;
    (void)new_state;

    return n;
}

/*
 * Agrega un estado al conjunto activo junto con todos los estados
 * alcanzables desde el mediante transiciones epsilon.
 *
 * El arreglo visited evita ciclos infinitos con las cerraduras.
 *
 * TODO: implementar.
 */
static void add_state(const nfa *n, int s, int *set, int *set_size, int *visited)
{
    (void)n;
    (void)s;
    (void)set;
    (void)set_size;
    (void)visited;
}

int match_nfa(nfa n, const char *text, size_t len)
{
    /*
     * Simulacion por conjuntos de estados.
     *
     *   1. el conjunto inicial es la cerradura epsilon del estado inicial
     *   2. por cada caracter de la entrada se calcula el conjunto siguiente,
     *      tomando las transiciones que consumen ese caracter y aplicando
     *      de nuevo la cerradura epsilon
     *   3. la cadena se acepta si el estado de aceptacion queda en el
     *      conjunto final
     *
     * TODO: implementar.
     */
    (void)n;
    (void)text;
    (void)len;
    (void)add_state;

    return 0;
}

void free_nfa(nfa *n)
{
    if (n == NULL)
    {
        return;
    }

    free(n->states);

    n->states = NULL;
    n->count = 0;
    n->capacity = 0;
    n->start = NO_STATE;
    n->accept = NO_STATE;
}

/*
 * Serializacion a JSON.
 *
 * Estructura sugerida:
 *
 *   {
 *     "start": 0,
 *     "accept": 3,
 *     "states": [
 *       { "id": 0, "symbol": "a", "out1": 1, "out2": -1 },
 *       ...
 *     ]
 *   }
 *
 * Conviene representar EPSILON como null o como una cadena vacia, y
 * escapar las comillas y contrabarras que puedan aparecer como simbolo.
 *
 * TODO: implementar.
 */
bool save_nfa(const nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}

/*
 * Lectura del formato anterior.
 *
 * TODO: implementar.
 */
bool load_nfa(nfa *n, const char *path)
{
    (void)n;
    (void)path;

    return false;
}