#include"figuras_svg.h"

#include<math.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const size_t CAPINI=4096;
const char* RECTANGULO="<rect x='%.1f' y='%.1f' width='%.1f' height='%.1f' style=\"fill:%s; stroke:%s; stroke-width:%.1f;\" />\n";
const char* LINEA="<line x1='%.1f' y1='%.1f' x2='%.1f' y2='%.1f' stroke='%s' stroke-width='%.1f' />\n";
const char* CIRCULO="<circle cx='%.1f' cy='%.1f' r='%.1f' stroke='%s' stroke-width='%.1f' fill='%s' />\n";
const char* TEXTO="<text fill='%s' font-family='%s' font-size='%.1f' x='%.1f' y='%.1f' text-anchor='%s'>%s</text>\n";
const char* POLIGONO="<polygon points='%.1f,%.1f %.1f,%.1f %.1f,%.1f' fill='%s' />\n";
const char* CURVA="<path d='M %.1f,%.1f Q %.1f,%.1f %.1f,%.1f' fill='none' stroke='%s' stroke-width='%.1f' />\n";
const char* BUCLE="<path d='M %.1f,%.1f C %.1f,%.1f %.1f,%.1f %.1f,%.1f' fill='none' stroke='%s' stroke-width='%.1f' />\n";
const char* ABRESVG="<?xml version='1.0' encoding='UTF-8' ?>\n<svg xmlns='http://www.w3.org/2000/svg' width='%.1f' height='%.1f' viewBox='0 0 %.1f %.1f'>\n";

void iniciaCadena(cadena* c){
	c->datos=NULL;
	c->len=0;
	c->cap=0;
}

void liberaCadena(cadena* c){
	free(c->datos);
	iniciaCadena(c);
}

int reserva(cadena* c,size_t extra){
	if(c->len+extra+1<=c->cap){
		return 1;
	}
	size_t nueva=(c->cap==0)?CAPINI:c->cap;
	while(c->len+extra+1>nueva){
		nueva*=2;
	}
	char* mayor=realloc(c->datos,nueva);
	if(mayor==NULL){
		return 0;
	}
	c->datos=mayor;
	c->cap=nueva;
	return 1;
}

void agregaTexto(cadena* c,const char* formato,...){
	va_list args;
	va_list copia;
	va_start(args,formato);
	va_copy(copia,args);
	int faltan=vsnprintf(NULL,0,formato,copia);
	va_end(copia);
	if(faltan>0&&reserva(c,(size_t)faltan)){
		vsnprintf(c->datos+c->len,(size_t)faltan+1,formato,args);
		c->len+=(size_t)faltan;
	}
	va_end(args);
}

int escribeCadena(const cadena* c,const char* ruta){
	FILE* archivo=fopen(ruta,"w");
	if(archivo==NULL){
		return 1;
	}
	if(c->datos!=NULL){
		fwrite(c->datos,1,c->len,archivo);
	}
	fclose(archivo);
	return 0;
}

void iniciaFiguras(figuras* f){
	f->color="black";
	f->relleno="black";
	f->fuente="sans-serif";
	f->anclaje="middle";
	f->grosor=3;
	f->tamanio=20;
}

void abreSvg(cadena* c,double ancho,double alto){
	agregaTexto(c,ABRESVG,ancho,alto,ancho,alto);
}

void cierraSvg(cadena* c){
	agregaTexto(c,"</svg>\n");
}

void rectangulo(const figuras* f,cadena* c,double x,double y,double ancho,double alto){
	agregaTexto(c,RECTANGULO,x,y,ancho,alto,f->relleno,f->color,f->grosor);
}

void linea(const figuras* f,cadena* c,double x1,double y1,double x2,double y2){
	agregaTexto(c,LINEA,x1,y1,x2,y2,f->color,f->grosor);
}

void circulo(const figuras* f,cadena* c,double x,double y,double radio){
	agregaTexto(c,CIRCULO,x,y,radio,f->color,f->grosor,f->relleno);
}

void texto(const figuras* f,cadena* c,const char* txt,double x,double y){
	agregaTexto(c,TEXTO,f->color,f->fuente,f->tamanio,x,y,f->anclaje,txt);
}

void triangulo(const figuras* f,cadena* c,double x,double y,double x1,double y1,double x2,double y2){
	agregaTexto(c,POLIGONO,x,y,x1,y1,x2,y2,f->color);
}

void curva(const figuras* f,cadena* c,double x1,double y1,double cx,double cy,double x2,double y2){
	agregaTexto(c,CURVA,x1,y1,cx,cy,x2,y2,f->color,f->grosor);
}

void punta(const figuras* f,cadena* c,double x,double y,double dx,double dy){
	double largo=sqrt(dx*dx+dy*dy);
	if(largo<1e-9){
		return;
	}
	double ux=dx/largo;
	double uy=dy/largo;
	double px=-uy;
	double py=ux;
	double base=11;
	double ala=5;
	triangulo(f,c,x,y,x-base*ux+ala*px,y-base*uy+ala*py,x-base*ux-ala*px,y-base*uy-ala*py);
}

void bucle(const figuras* f,cadena* c,double x,double y,double radio,double alto){
	agregaTexto(c,BUCLE,x-radio*0.6,y-radio*0.8,x-radio*1.6,y-alto,x+radio*1.6,y-alto,x+radio*0.6,y-radio*0.8,f->color,f->grosor);
}

void etiqueta(const figuras* f,cadena* c,const char* txt,double x,double y,const char* color,double tamanio,const char* anclaje){
	agregaTexto(c,TEXTO,color,f->fuente,tamanio,x,y,anclaje,txt);
}
