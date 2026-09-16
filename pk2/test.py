#!/usr/bin/env python3
"""
Pruebas unitarias

Uso:
    python3 test.py          # compila y corre todo

"""

import subprocess
import sys
import unittest
from pathlib import Path

SRC = Path(__file__).with_name("nfa_to_dfa.cpp")
BIN = Path(__file__).with_name("sub")


def compilar():
    cmd = ["g++", "-std=c++17", "-O2", "-Wall", "-Wextra", "-o", str(BIN), str(SRC)]
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode != 0:
        sys.exit(f"no compila:\n{p.stderr}")
    if p.stderr.strip():
        print(f"warnings del compilador:\n{p.stderr}")


class PruebaAFD(unittest.TestCase):
    def correr(self, entrada: str) -> str:
        p = subprocess.run(
            [str(BIN)], input=entrada, capture_output=True, text=True, timeout=5
        )
        self.assertEqual(p.returncode, 0, f"el programa falló:\n{p.stderr}")
        return p.stdout.strip()

    def verificar(self, entrada: str, esperado: str):
        self.assertEqual(self.correr(entrada), esperado.strip())

    # ------------------------------------------------------ casos normales ---

    def test_afd_sin_epsilon(self):
        """Un AFN que ya es determinista: el AFD debe ser una copia, 1 a 1."""
        entrada = """
            2 2
            ab
            0
            1 1
            0 a 1
            1 b 1
        """
        self.verificar(entrada, """
alfabeto: ab
estados: 2
inicial: 0
0 {0} a->1 b->-
1* {1} a->- b->1
        """)

    def test_no_determinismo(self):
        """Dos aristas con el mismo símbolo: los destinos se fusionan en un estado."""
        entrada = """
            3 2
            a
            0
            1 2
            0 a 1
            0 a 2
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0 {0} a->1
1* {1,2} a->-
        """)

    def test_dragon_abb(self):
        """Thompson de (a|b)*abb. El AFD canónico tiene 5 estados."""
        entrada = """
            11 13
            ab
            0
            1 10
            0 . 1
            0 . 7
            1 . 2
            1 . 4
            2 a 3
            4 b 5
            3 . 6
            5 . 6
            6 . 1
            6 . 7
            7 a 8
            8 b 9
            9 b 10
            5
            abb
            aabb
            babb
            ab
            -
        """
        self.verificar(entrada, """
alfabeto: ab
estados: 5
inicial: 0
0 {0,1,2,4,7} a->1 b->2
1 {1,2,3,4,6,7,8} a->1 b->3
2 {1,2,4,5,6,7} a->1 b->2
3 {1,2,4,5,6,7,9} a->1 b->4
4* {1,2,4,5,6,7,10} a->1 b->2
"abb": aceptada
"aabb": aceptada
"babb": aceptada
"ab": rechazada
"": rechazada
        """)

    def test_clausura_encadenada(self):
        """Las epsilon se siguen en cadena: e-Closure({0}) tiene que dar {0,1,2}."""
        entrada = """
            4 3
            a
            0
            1 3
            0 . 1
            1 . 2
            2 a 3
            2
            a
            aa
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0 {0,1,2} a->1
1* {3} a->-
"a": aceptada
"aa": rechazada
        """)

    # --------------------------------------------------------- casos borde ---

    def test_un_estado_sin_transiciones(self):
        """El mínimo posible: un estado inicial que además es final."""
        entrada = """
            1 0
            a
            0
            1 0
            2
            -
            a
        """
        self.verificar(entrada, """
alfabeto: a
estados: 1
inicial: 0
0* {0} a->-
"": aceptada
"a": rechazada
        """)

    def test_sin_estados_finales(self):
        """Lenguaje vacío: ningún estado del AFD lleva '*'."""
        entrada = """
            2 1
            ab
            0
            0
            0 a 1
            2
            a
            -
        """
        self.verificar(entrada, """
alfabeto: ab
estados: 2
inicial: 0
0 {0} a->1 b->-
1 {1} a->- b->-
"a": rechazada
"": rechazada
        """)

    def test_ciclo_de_epsilon(self):
        """Ciclo 0->1->2->0 por epsilon: no debe colgarse ni repetir estados."""
        entrada = """
            3 3
            a
            0
            1 2
            0 . 1
            1 . 2
            2 . 0
            1
            -
        """
        self.verificar(entrada, """
alfabeto: a
estados: 1
inicial: 0
0* {0,1,2} a->-
"": aceptada
        """)

    def test_autolazo_epsilon(self):
        """Transición epsilon de un estado a sí mismo: no debe duplicar trabajo."""
        entrada = """
            2 2
            a
            0
            1 1
            0 . 0
            0 a 1
            1
            a
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0 {0} a->1
1* {1} a->-
"a": aceptada
        """)

    def test_transicion_duplicada(self):
        """La misma arista declarada dos veces no cambia el resultado."""
        entrada = """
            2 3
            a
            0
            1 1
            0 a 1
            0 a 1
            0 a 1
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0 {0} a->1
1* {1} a->-
        """)

    def test_estado_muerto(self):
        """Desde {1} no sale nada con 'b': la transición queda como '-'."""
        entrada = """
            2 1
            ab
            0
            1 1
            0 a 1
            3
            a
            ab
            b
        """
        self.verificar(entrada, """
alfabeto: ab
estados: 2
inicial: 0
0 {0} a->1 b->-
1* {1} a->- b->-
"a": aceptada
"ab": rechazada
"b": rechazada
        """)



    def test_todos_finales(self):
        """Si todo estado del AFN es final, todo estado del AFD lleva '*'."""
        entrada = """
            2 1
            a
            0
            2 0 1
            0 a 1
            2
            -
            a
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0* {0} a->1
1* {1} a->-
"": aceptada
"a": aceptada
        """)

    def test_simbolo_fuera_del_alfabeto(self):
        """Una cadena con un símbolo que no está en sigma se rechaza, sin romperse."""
        entrada = """
            2 1
            a
            0
            1 1
            0 a 1
            1
            az
        """
        self.verificar(entrada, """
alfabeto: a
estados: 2
inicial: 0
0 {0} a->1
1* {1} a->-
"az": rechazada
        """)


if __name__ == "__main__":
    compilar()
    unittest.main(verbosity=2)
