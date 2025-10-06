### Creado por: Mateo Úsuga Vargas y Diego Andrés Peña Aroca usando CLion
## Video explicativo: https://youtu.be/oXngzKO6KfM
# Práctica 2 – Huffman Canónico

Este proyecto implementa en C++ el **algoritmo de Huffman** y su variante **Huffman Canónico** para compresión de texto.  
El programa recibe una cadena de al menos 30 caracteres y produce:

- Tabla de frecuencias de cada símbolo.
- Construcción del árbol de Huffman.
- Códigos de Huffman **no-canónicos** y sus longitudes.
- Códigos **canónicos** derivados de las longitudes.
- Visualización textual del árbol en preorden.
- Bitstream comprimido (primeros 128 bits y versión en hexadecimal).
- Cálculo de reducción lograda respecto al tamaño original.

La práctica corresponde al curso **Estructuras de Datos y Algoritmos (2025-2)**.

---

##  Archivos del proyecto

- `main.cpp` → Implementación completa de Huffman + Huffman Canónico.
- `README.md` → Este archivo, con instrucciones y explicación.

---

## ⚙️ Compilación y ejecución

### Con g++ (Linux/Mac/Windows con MinGW):
```bash
g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o huffman
./huffman
