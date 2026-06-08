# **README.md \- ucvsh \- Shell Interactiva para Linux**

Proyecto \#1 de la asignatura **Sistemas Operativos (6004)** \- Universidad Central de Venezuela.

## **🚀 Descripción**

ucvsh es un intérprete de comandos interactivo desarrollado en C para entornos UNIX. El proyecto implementa mecanismos fundamentales del sistema operativo como la gestión del ciclo de vida de procesos, comunicación inter-procesos (pipes), manipulación de descriptores de archivo y sincronización.

## **🛠️ Características Principales**

* **Prompt Personalizado:** Interfaz interactiva para el usuario.  
* **Ejecución de Comandos:** Soporte para comandos simples y con argumentos.  
* **Operadores Lógicos:** Soporte para ;, && y ||.  
* **Redirección de E/S:** Implementación de operadores \>, \>\>, \<.  
* **Tuberías (Pipes):** Comunicación entre procesos mediante el operador |.  
* **Control de Trabajos (Job Control):** Ejecución en segundo plano (&) y comando jobs.  
* **Historial de Comandos:** Persistencia y navegación con flechas del teclado.

## **📋 Requisitos del Entorno**

Para asegurar la compatibilidad y el correcto funcionamiento, se utiliza el siguiente stack:

* **Sistema Operativo:** Ubuntu (vía WSL2 recomendado en Windows).  
* **Compilador:** gcc (Estándar C99).  
* **Herramientas de Depuración:** gdb, valgrind.

## **🏗️ Instalación y Compilación**

1. Clonar el repositorio.  
2. Asegurarse de tener instaladas las herramientas de compilación:  
   `sudo apt update && sudo apt install build-essential`  
3. Compilar el proyecto usando el Makefile incluido:  
   `make`  
4. Ejecutar la shell:  
   `./ucvsh`


## **📄 Licencia**

Este proyecto está bajo la Licencia MIT.