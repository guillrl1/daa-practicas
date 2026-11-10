# Diseño y Análisis de Algoritmos (DAA)

![Language](https://img.shields.io/badge/Language-C++-00599C?logo=c%2B%2B&logoColor=white) ![Build](https://img.shields.io/badge/Build-CMake-064F8C?logo=cmake&logoColor=white) ![Status](https://img.shields.io/badge/Status-Academic-green)

Este repositorio contiene las implementaciones, prácticas y ejercicios realizados para la asignatura **Diseño y Análisis de Algoritmos** del grado en Ingeniería Informática en la Universidad de La Laguna.

El objetivo principal es explorar diferentes paradigmas algorítmicos, analizar su complejidad temporal/espacial y comparar su rendimiento en escenarios prácticos.

## 📚 Contenido del Repositorio

Las prácticas están organizadas por temas o paradigmas de resolución:

| Práctica | Tema Principal | Algoritmos Destacados |
| :--- | :--- | :--- |
| **Práctica 1** | Complejidad Computacional| Operaciones de matrices |
| **Práctica 2** | Experimentación y complejidad computacional | RAM |

## 🏗️ Arquitectura del Proyecto

El código sigue principios de diseño limpio y **SOLID** para facilitar la extensión y las pruebas de nuevos algoritmos. Se utiliza el **Patrón Estrategia** para intercambiar implementaciones dinámicamente.

### Estructura de Carpetas

```text
/MiProyectoAlgoritmos
│
├── /include               
│   └── /Algoritmos        <-- Subcarpeta con el nombre de lib/proyecto
│           IAlgoritmo.h   <--- Interfaces (archivos de cabecera)
│           Instancia.h
│           Solucion.h
│
├── /src                   <-- Aquí va la implementación (.cpp)
│       AlgoritmoVoraz.cc
│       AlgoritmoDinamico.cc
│       Instancia.cc
│       main.cc      
│
├── /tests                 
│       TestVoraz.cc
│
├── /build                 <-- Carpeta para compilar (se ignora en git)
│
├── Makefile               <-- Archivo de configuración
├── .gitignore
└── README.md


## Contenido

- [`P01-Complejidad-Computacional`](./P01-Complejidad-Computacional)
- [`P02-RAM`](./P02-RAM)
- [`P03-DyV`](./P03-DyV)
- [`P04-PD`](./P04-PD)
