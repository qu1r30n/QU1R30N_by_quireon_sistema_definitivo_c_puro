#define main quireon_program_main
#include "QU1R30N_by_quireon_sistema_definitivo_c_puro.c"
#undef main

#include <assert.h>

static int result_code(char *result)
{
    assert(result != NULL);
    int code = atoi(result);
    sistema_memoria_liberar(result);
    return code;
}

int main(void)
{
    const char *path = "file_layer_smoke_unique.tmp";
    FILE *existing = sistema_archivo_abrir(path, "r");
    assert(existing == NULL);

    assert(result_code(escribirLinea(path, "uno,rojo", 0)) == 1);
    assert(result_code(escribirLinea(path, "dos,azul", 0)) == 1);
    assert(result_code(editarColumna(path, 1, 2, "verde", 0)) == 1);

    FILE *file = sistema_archivo_abrir(path, "r");
    assert(file != NULL);
    char *first = leerLineaDinamica(file, 0);
    char *second = leerLineaDinamica(file, 0);
    assert(first != NULL && strcmp(first, "uno,verde") == 0);
    assert(second != NULL && strcmp(second, "dos,azul") == 0);
    sistema_memoria_liberar(first);
    sistema_memoria_liberar(second);
    assert(sistema_archivo_leer_caracter(file) == EOF);
    assert(!sistema_archivo_hay_error(file));
    assert(sistema_archivo_cerrar(file) == 0);

    assert(result_code(eliminarLinea(path, 1, 0)) == 1);
    assert(result_code(leerArchivo(path, 0)) == 1);
    assert(sistema_archivo_eliminar(path) == 0);

    char *initial = crearResultado(200, "test", NULL, "main", 1);
    assert(initial != NULL);
    sistema_memoria_liberar(initial);
    return 0;
}
