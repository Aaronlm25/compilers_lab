#include"grafico_nfa.h"

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const double RADIO=24;
const double SEPX=118;
const double SEPY=96;
const double MARGEN=122;
const double MARGENTXT=30;
const double MARGENSUP=118;
const double MARGENINF=56;

const char* CFONDO="white";
const char* CESTADO="#334155";
const char* CACEPTA="#15803d";
const char* CARISTA="#94a3b8";
const char* CTEXTO="#0f172a";
const char* CTENUE="#94a3b8";

void marcaRegresos(const nfa* n,int v,int* color,int* marcas){
	color[v]=1;
	for(int s=0;s<2;s++){
		int ng=(s==0)?n->states[v].out1:n->states[v].out2;
		if(ng==NO_STATE){
			continue;
		}
		if(color[ng]==1){
			marcas[v*2+s]=1;
		}else if(color[ng]==0){
			marcaRegresos(n,ng,color,marcas);
		}
	}
	color[v]=2;
}

void calculaCapas(const nfa* n,const int* marcas,int* capas){
	for(int i=0;i<n->count;i++){
		capas[i]=0;
	}
	for(int vuelta=0;vuelta<n->count;vuelta++){
		int cambio=0;
		for(int v=0;v<n->count;v++){
			for(int s=0;s<2;s++){
				int ng=(s==0)?n->states[v].out1:n->states[v].out2;
				if(ng==NO_STATE||marcas[v*2+s]){
					continue;
				}
				if(capas[ng]<capas[v]+1){
					capas[ng]=capas[v]+1;
					cambio=1;
				}
			}
		}
		if(!cambio){
			break;
		}
	}
}

int iniciaGrafico(grafico* g,nfa automata){
	g->automata=automata;
	g->pos=NULL;
	g->regreso=NULL;
	g->radio=RADIO;
	g->ancho=0;
	g->alto=0;
	if(automata.states==NULL||automata.count<=0){
		return 1;
	}
	g->pos=calloc((size_t)automata.count,sizeof(coordenadas));
	g->regreso=calloc((size_t)automata.count*2,sizeof(int));
	int* color=calloc((size_t)automata.count,sizeof(int));
	int* capas=malloc((size_t)automata.count*sizeof(int));
	int* ocupa=calloc((size_t)automata.count+1,sizeof(int));
	if(g->pos==NULL||g->regreso==NULL||color==NULL||capas==NULL||ocupa==NULL){
		free(color);
		free(capas);
		free(ocupa);
		liberaGrafico(g);
		return 1;
	}
	if(automata.start!=NO_STATE){
		marcaRegresos(&automata,automata.start,color,g->regreso);
	}
	for(int i=0;i<automata.count;i++){
		if(color[i]==0){
			marcaRegresos(&automata,i,color,g->regreso);
		}
	}
	calculaCapas(&automata,g->regreso,capas);
	int maxCapa=0;
	int maxRanuras=0;
	for(int i=0;i<automata.count;i++){
		if(capas[i]>maxCapa){
			maxCapa=capas[i];
		}
	}
	for(int i=0;i<automata.count;i++){
		g->pos[i].capa=capas[i];
		g->pos[i].ranura=ocupa[capas[i]];
		ocupa[capas[i]]++;
		if(ocupa[capas[i]]>maxRanuras){
			maxRanuras=ocupa[capas[i]];
		}
	}
	g->ancho=MARGEN*2+maxCapa*SEPX;
	g->alto=MARGENSUP+MARGENINF+(maxRanuras-1)*SEPY+RADIO*2;
	double centro=MARGENSUP+(g->alto-MARGENSUP-MARGENINF)/2;
	for(int i=0;i<automata.count;i++){
		int enCapa=ocupa[g->pos[i].capa];
		g->pos[i].x=MARGEN+g->pos[i].capa*SEPX;
		g->pos[i].y=centro+(g->pos[i].ranura-(enCapa-1)/2.0)*SEPY;
	}
	free(color);
	free(capas);
	free(ocupa);
	return 0;
}

void liberaGrafico(grafico* g){
	if(g==NULL){
		return;
	}
	free(g->pos);
	free(g->regreso);
	g->pos=NULL;
	g->regreso=NULL;
}

void escapa(char* dst,size_t n,char c){
	const char* rep=NULL;
	if(c=='<'){
		rep="&lt;";
	}else if(c=='>'){
		rep="&gt;";
	}else if(c=='&'){
		rep="&amp;";
	}else if(c=='\''){
		rep="&apos;";
	}else if(c=='"'){
		rep="&quot;";
	}
	if(rep!=NULL){
		snprintf(dst,n,"%s",rep);
	}else if(c==EPSILON){
		snprintf(dst,n,"&#949;");
	}else{
		snprintf(dst,n,"%c",c);
	}
}

void escapaTexto(char* dst,size_t n,const char* src){
	size_t j=0;
	for(size_t i=0;src[i]!='\0';i++){
		char uno[16];
		escapa(uno,sizeof(uno),src[i]);
		size_t largo=strlen(uno);
		if(j+largo+1>n){
			break;
		}
		memcpy(dst+j,uno,largo);
		j+=largo;
	}
	dst[j]='\0';
}

void dibujaArista(const grafico* g,cadena* c,figuras* f,int origen,int destino,int deRegreso,char simbolo){
	double x1=g->pos[origen].x;
	double y1=g->pos[origen].y;
	double x2=g->pos[destino].x;
	double y2=g->pos[destino].y;
	int mismaCapa=g->pos[origen].capa==g->pos[destino].capa;
	int salto=abs(g->pos[destino].capa-g->pos[origen].capa)>1;
	int esCurva=deRegreso||mismaCapa||salto||fabs(y1-y2)>1;
	f->color=CARISTA;
	f->grosor=2;
	char txt[16];
	escapa(txt,sizeof(txt),simbolo);
	if(!esCurva){
		double dx=x2-x1;
		double dy=y2-y1;
		double largo=sqrt(dx*dx+dy*dy);
		if(largo<1e-9){
			return;
		}
		double ux=dx/largo;
		double uy=dy/largo;
		linea(f,c,x1+g->radio*ux,y1+g->radio*uy,x2-g->radio*ux,y2-g->radio*uy);
		punta(f,c,x2-g->radio*ux,y2-g->radio*uy,ux,uy);
		etiqueta(f,c,txt,(x1+x2)/2,(y1+y2)/2-10,CTEXTO,17,"middle");
		return;
	}
	double cx;
	double cy;
	if(deRegreso||mismaCapa){
		double dist=fabs(x2-x1)+fabs(y2-y1);
		cx=(x1+x2)/2;
		cy=fmin(y1,y2)-(g->radio*2+dist*0.22);
	}else{
		double dx=x2-x1;
		double dy=y2-y1;
		double largo=sqrt(dx*dx+dy*dy);
		double desp=salto?34:16;
		cx=(x1+x2)/2-(dy/largo)*desp;
		cy=(y1+y2)/2+(dx/largo)*desp;
	}
	double sx=cx-x1;
	double sy=cy-y1;
	double largoSal=sqrt(sx*sx+sy*sy);
	double ex=x2-cx;
	double ey=y2-cy;
	double largoEnt=sqrt(ex*ex+ey*ey);
	if(largoSal<1e-9||largoEnt<1e-9){
		return;
	}
	double ix=x1+g->radio*(sx/largoSal);
	double iy=y1+g->radio*(sy/largoSal);
	double fx=x2-g->radio*(ex/largoEnt);
	double fy=y2-g->radio*(ey/largoEnt);
	curva(f,c,ix,iy,cx,cy,fx,fy);
	punta(f,c,fx,fy,ex/largoEnt,ey/largoEnt);
	etiqueta(f,c,txt,0.25*ix+0.5*cx+0.25*fx,0.25*iy+0.5*cy+0.25*fy-8,CTEXTO,17,"middle");
}

void dibujaBucle(const grafico* g,cadena* c,figuras* f,int v,char simbolo){
	double x=g->pos[v].x;
	double y=g->pos[v].y;
	f->color=CARISTA;
	f->grosor=2;
	bucle(f,c,x,y,g->radio,g->radio*2.6);
	punta(f,c,x+g->radio*0.6,y-g->radio*0.8,0.6,0.8);
	char txt[16];
	escapa(txt,sizeof(txt),simbolo);
	etiqueta(f,c,txt,x,y-g->radio*2.9,CTEXTO,17,"middle");
}

void dibujaEstado(const grafico* g,cadena* c,figuras* f,int v){
	double x=g->pos[v].x;
	double y=g->pos[v].y;
	int acepta=v==g->automata.accept;
	f->grosor=2.2;
	f->color=acepta?CACEPTA:CESTADO;
	f->relleno=CFONDO;
	circulo(f,c,x,y,g->radio);
	if(acepta){
		f->relleno="none";
		circulo(f,c,x,y,g->radio-5);
	}
	char num[16];
	snprintf(num,sizeof(num),"%d",v);
	etiqueta(f,c,num,x,y+6,CTEXTO,17,"middle");
	if(v==g->automata.start){
		f->color=CESTADO;
		f->grosor=2.2;
		linea(f,c,x-g->radio-34,y,x-g->radio,y);
		punta(f,c,x-g->radio,y,1,0);
		etiqueta(f,c,"inicio",x-g->radio-36,y-9,CTENUE,13,"end");
	}
}

void dibujaNfa(const grafico* g,cadena* c,const char* expresion){
	figuras f;
	iniciaFiguras(&f);
	abreSvg(c,g->ancho,g->alto);
	f.color=CFONDO;
	f.relleno=CFONDO;
	f.grosor=0;
	rectangulo(&f,c,0,0,g->ancho,g->alto);
	if(expresion!=NULL){
		char esc[1200];
		char txt[1240];
		escapaTexto(esc,sizeof(esc),expresion);
		snprintf(txt,sizeof(txt),"regex:  %s",esc);
		etiqueta(&f,c,txt,MARGENTXT,38,CTEXTO,19,"start");
	}
	for(int v=0;v<g->automata.count;v++){
		for(int s=0;s<2;s++){
			int ng=(s==0)?g->automata.states[v].out1:g->automata.states[v].out2;
			if(ng==NO_STATE){
				continue;
			}
			char simbolo=(s==0)?g->automata.states[v].symbol:EPSILON;
			if(ng==v){
				dibujaBucle(g,c,&f,v,simbolo);
			}else{
				dibujaArista(g,c,&f,v,ng,g->regreso[v*2+s],simbolo);
			}
		}
	}
	for(int v=0;v<g->automata.count;v++){
		dibujaEstado(g,c,&f,v);
	}
	cierraSvg(c);
}

int escribeNfa(const grafico* g,const char* ruta,const char* expresion){
	cadena c;
	iniciaCadena(&c);
	dibujaNfa(g,&c,expresion);
	int res=escribeCadena(&c,ruta);
	liberaCadena(&c);
	return res;
}
