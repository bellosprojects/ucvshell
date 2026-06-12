# ucvsh - Shell para Sistemas Operativos

## Estructura del proyecto

- `include/` - Headers públicos
- `src/` - Implementaciones
- `bin/` - Ejecutable (generado)
- `obj/` - Objetos (generado)

## Compilación

```bash
make
```

## Advertencia

Para el correcto manejo de las señales, es importar ejecutar el codigo manualmente con el comando

```bash
./bin/ucvsh
```

Y no con

```bash
make run
```