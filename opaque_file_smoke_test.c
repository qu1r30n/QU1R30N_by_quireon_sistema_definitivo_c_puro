#define main quireon_program_main
#include "QU1R30N_by_quireon_sistema_definitivo_c_puro.c"
#undef main

#include <assert.h>

static int result_code(char *result)
{
    assert(result != NULL);
    int code = atoi(result);
    sistema_memoria_liberar(result, 0);
    return code;
}

int main(void)
{
    const char *path = "opaque_file_smoke_unique.tmp";
    SistemaArchivo *existing = sistema_archivo_abrir(path, "r", 0);
    assert(existing == NULL);

    assert(result_code(escribirLinea(path, "uno,rojo", 0)) == 1);
    assert(result_code(escribirLinea(path, "dos,azul", 0)) == 1);
    assert(result_code(editarColumna(path, 1, 2, "verde", 0)) == 1);

    SistemaArchivo *file = sistema_archivo_abrir(path, "r", 0);
    assert(file != NULL);
    char *first = leerLineaDinamica(file, 0);
    char *second = leerLineaDinamica(file, 0);
    assert(first != NULL && strcmp(first, "uno,verde") == 0);
    assert(second != NULL && strcmp(second, "dos,azul") == 0);
    sistema_memoria_liberar(first, 0);
    sistema_memoria_liberar(second, 0);
    assert(leerLineaDinamica(file, 0) == NULL);
    assert(!sistema_archivo_hay_error(file, 0));
    assert(sistema_archivo_cerrar(file, 0) == 0);

    assert(result_code(eliminarLinea(path, 1, 0)) == 1);
    assert(result_code(leerArchivo(path, 0)) == 1);
    assert(sistema_archivo_eliminar(path, 0) == 0);

    SistemaArchivo *console = sistema_archivo_entrada_estandar();
    assert(console != NULL);
    assert(sistema_archivo_cerrar(console, 0) == 0);
    return 0;
}
