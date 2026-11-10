# Ejemplos de uso del simulador RAM

## Compilar

```bash
# Debug (con símbolos, warnings activados)
make

# Release (optimizado, para el benchmark)
make BUILD=release
```

El binario se genera en `bin/p01`.

---

## Formato de los ficheros de cinta

**Cinta de entrada** (`*.input`): enteros separados por espacios o saltos de línea.

```
# input_test1.txt — valores que se copiarán a la salida hasta encontrar el 0
3 -8 2 0
```

**Cinta de salida** (`*.output`): se genera automáticamente. Un entero por línea.

---

## Ejemplos de ejecución

### test1.ram — copiar la cinta hasta encontrar un 0

```bash
# Fichero de entrada: 3 -8 2 0
echo "3 -8 2 0" > /tmp/input.txt

./bin/p01 Ejemplos-RAM/test1.ram /tmp/input.txt /tmp/output.txt

# Salida en consola:
# Execution finished. Instructions executed: 16
# Output written to: /tmp/output.txt

cat /tmp/output.txt
# 3
# -8
# 2
```

---

### test2.ram — reconocer si una palabra tiene el mismo número de 1s y 2s

```bash
# Palabra "1 2 1 2 0" → debería reconocerse (salida: 1)
echo "1 2 1 2 0" > /tmp/input.txt
./bin/p01 Ejemplos-RAM/test2.ram /tmp/input.txt /tmp/output.txt
cat /tmp/output.txt
# 1

# Palabra "1 1 2 0" → no se reconoce (salida: 0)
echo "1 1 2 0" > /tmp/input.txt
./bin/p01 Ejemplos-RAM/test2.ram /tmp/input.txt /tmp/output.txt
cat /tmp/output.txt
# 0
```

---

### test3.ram — programa con instrucción ilegal (STORE =2)

```bash
./bin/p01 Ejemplos-RAM/test3.ram /dev/null /tmp/output.txt

# Salida en consola (stderr):
# Runtime error: STORE: immediate operand is not allowed
# Partial output written to: /tmp/output.txt
```

---

### test4.ram — doblar cada elemento de la cinta hasta encontrar un 0

```bash
echo "4 7 3 0" > /tmp/input.txt
./bin/p01 Ejemplos-RAM/test4.ram /tmp/input.txt /tmp/output.txt
cat /tmp/output.txt
# 8
# 14
# 6
```

---

### test5.ram — sumar todos los elementos de la cinta hasta encontrar un 0

```bash
echo "5 10 3 2 0" > /tmp/input.txt
./bin/p01 Ejemplos-RAM/test5.ram /tmp/input.txt /tmp/output.txt
cat /tmp/output.txt
# 20
```

---

### test6.ram — programa con instrucción ilegal (WRITE 0)

```bash
./bin/p01 Ejemplos-RAM/test6.ram /dev/null /tmp/output.txt

# Salida en consola (stderr):
# Runtime error: WRITE: cannot read from register R0
# Partial output written to: /tmp/output.txt
```

---

### test7.ram — guardar valores en registros, multiplicar por 3 y escribir

```bash
echo "4 7 2 0" > /tmp/input.txt
./bin/p01 Ejemplos-RAM/test7.ram /tmp/input.txt /tmp/output.txt
cat /tmp/output.txt
# 12
# 21
# 6
```

---

### insertion_sort.ram — ordenar un array por inserción

```bash
# n=5, array = 4 2 7 1 9
echo "5 4 2 7 1 9" > /tmp/input_sort.txt
./bin/p01 Ejemplos-Sort/insertion_sort.ram /tmp/input_sort.txt /tmp/output.txt
cat /tmp/output.txt
# 1
# 2
# 4
# 7
# 9
```

---

### bubble_sort.ram — ordenar un array con BubbleSort

```bash
echo "5 4 2 7 1 9" > /tmp/input_sort.txt
./bin/p01 Ejemplos-Sort/bubble_sort.ram /tmp/input_sort.txt /tmp/output.txt
cat /tmp/output.txt
# 1
# 2
# 4
# 7
# 9
```

---

## Ejecutar el benchmark completo

```bash
chmod +x benchmark.sh
./benchmark.sh
```

Salida esperada (los contadores varían según el contenido de las cintas):

```
========================================
  RAM Machine Benchmark
========================================

--- Ejemplos-RAM ---
  Ejemplos-RAM/test1                        16 instructions
  Ejemplos-RAM/test2                        28 instructions
  Ejemplos-RAM/test3                        ERROR (exit 1)
  Ejemplos-RAM/test4                        22 instructions
  Ejemplos-RAM/test5                        30 instructions
  Ejemplos-RAM/test6                        ERROR (exit 1)
  Ejemplos-RAM/test7                        45 instructions

--- Ejemplos-Sort ---
  Ejemplos-Sort/bubble_sort                 312 instructions
  Ejemplos-Sort/insertion_sort              187 instructions

========================================
  Results
========================================
  ...

Done.
```

> Los programas `test3` y `test6` se espera que terminen con error:
> son los casos de prueba de instrucciones ilegales del enunciado.