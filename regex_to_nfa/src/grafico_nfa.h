#ifndef GRAFICO_NFA_H
#define GRAFICO_NFA_H

#include"figuras_svg.h"
#include"nfa.h"

typedef struct{
	double x;
	double y;
	int capa;
	int ranura;
}coordenadas;

typedef struct{
	nfa automata;
	coordenadas* pos;
	int* regreso;
	double ancho;
	double alto;
	double radio;
}grafico;

int iniciaGrafico(grafico* g,nfa automata);
void liberaGrafico(grafico* g);
void dibujaNfa(const grafico* g,cadena* c,const char* expresion);
int escribeNfa(const grafico* g,const char* ruta,const char* expresion);

#endif
