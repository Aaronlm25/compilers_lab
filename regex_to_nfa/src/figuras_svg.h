#ifndef FIGURAS_SVG_H
#define FIGURAS_SVG_H

#include<stddef.h>

typedef struct{
	char* datos;
	size_t len;
	size_t cap;
}cadena;

typedef struct{
	const char* color;
	const char* relleno;
	const char* fuente;
	const char* anclaje;
	double grosor;
	double tamanio;
}figuras;

void iniciaCadena(cadena* c);
void agregaTexto(cadena* c,const char* formato,...);
void liberaCadena(cadena* c);
int escribeCadena(const cadena* c,const char* ruta);

void iniciaFiguras(figuras* f);
void abreSvg(cadena* c,double ancho,double alto);
void cierraSvg(cadena* c);
void rectangulo(const figuras* f,cadena* c,double x,double y,double ancho,double alto);
void linea(const figuras* f,cadena* c,double x1,double y1,double x2,double y2);
void circulo(const figuras* f,cadena* c,double x,double y,double radio);
void texto(const figuras* f,cadena* c,const char* txt,double x,double y);
void triangulo(const figuras* f,cadena* c,double x,double y,double x1,double y1,double x2,double y2);
void curva(const figuras* f,cadena* c,double x1,double y1,double cx,double cy,double x2,double y2);
void punta(const figuras* f,cadena* c,double x,double y,double dx,double dy);
void bucle(const figuras* f,cadena* c,double x,double y,double radio,double alto);
void etiqueta(const figuras* f,cadena* c,const char* txt,double x,double y,const char* color,double tamanio,const char* anclaje);

#endif
