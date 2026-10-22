#!/usr/bin/env bash
# Corre todas las pruebas: las de tests/validas deben aceptarse
# y las de tests/invalidas deben rechazarse.
cd "$(dirname "$0")"
BIN=./pk5
fallos=0

correr() {
	local esperado=$1 archivo=$2
	local salida
	salida=$($BIN "$archivo")
	local codigo=$?
	local obtenido
	if [ $codigo -eq 0 ]; then obtenido=ACEPTADA; else obtenido=RECHAZADA; fi

	local marca=OK
	if [ "$obtenido" != "$esperado" ]; then
		marca=FALLO
		fallos=$((fallos + 1))
	fi

	printf '[%-5s] %-40s %-9s\n' "$marca" "$archivo" "$obtenido"
	printf '        %s\n' "$(echo "$salida" | head -1)"
	if [ "$obtenido" = RECHAZADA ]; then
		printf '        %s\n' "$(echo "$salida" | tail -1)"
	fi
}

for f in tests/validas/*.tboi; do correr ACEPTADA "$f"; done
for f in tests/invalidas/*.tboi; do correr RECHAZADA "$f"; done

echo
if [ $fallos -eq 0 ]; then
	echo "Todas las pruebas pasaron"
else
	echo "$fallos prueba(s) fallaron"
	exit 1
fi
