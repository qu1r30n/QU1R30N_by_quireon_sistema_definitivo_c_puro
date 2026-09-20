// para c
//
/* =============================================================================
 * Proyecto: QU1R30N_by_quireon_sistema_definitivo_c_puro
 * Archivo:  QU1R30N_by_quireon_sistema_definitivo_c_puro.c
 * Autor:    QU1R30N,QUIREON <tu_email@ejemplo.com>
 * Año:      2026
 * =============================================================================
 * Descripción:
 *   HACE TODA LA FUNCIONALIDADES DE UN SISTEMA DE NEGOCIO, CON FUNCIONES DE MENSAJERIA, MANEJO DE ARCHIVOS, Y
 * OPERACIONES DE TEXTO. ESTE PROGRAMA ES UN SISTEMA DE NEGOCIO QUE PERMITE LEER, ESCRIBIR, EDITAR Y ELIMINAR INVENTARIO
 * COMPRAS VENTAS Y OTROS DATOS DE NEGOCIO, ASI COMO ENVIAR MENSAJES A CONTACTOS Y GRUPOS.
 * =============================================================================
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://apache.org
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOMBRE_ARCHIVO "datos.txt"
#define DELIMITADOR ","

// ============================================================================
//  DECLARACIÓN VARIABLES GLOBALES
// ============================================================================
#pragma region "FUNCIONES DECLARACION VARIABLES GLOBALES"

int GG_indice_donde_comensar = 1;

const char *GG_caracter_separacion[] = {"|", "°", "¬", "╦", "╝", "╔"};

const char *GG_caracter_separacion_funciones_espesificas[] = {"~", "§", "¶", "╬"};

const char *GG_caracter_para_confirmacion_o_error[] = {"╣", "╠", "⚺", "⚻", "⚼"};

const char *GG_caracter_para_transferencia_entre_archivos[] = {"┴", "■"};

const char *GG_caracter_usadas_por_usuario[] = {":", "#", "&"};

const char *GG_caracter_para_usar_como_enter_y_nuevo_mensaje[] = {"•", "∆"};

const char *GG_id_programa = "QU1R30N_SISTEMA_DEFINITIVO";

const char *GG_direccion_control_errores_try = "config\\chatbot\\errores_try\\control_errore.txt";

const char *G_dir_arch_transferencia[] = {
    /*0*/ "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\BANDERAS_sis_qu1.TXT",
    /*1*/ "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\ent_sis_qu1.TXT", // preguntas
    /*2*/ "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\sal_sis_qu1.TXT"  // respuestas
};

#pragma endregion

// ============================================================================
//  DECLARACIÓN DE FUNCIONES TEX_BASE(PROTOTIPOS)
// ============================================================================
#pragma region "FUNCIONES TEX_BASE"
char *leerLineaDinamica(FILE *flujo);
char *modificarColumna(const char *lineaOriginal, int columnaTarget, const char *nuevoValor);
char *ejecutarEjemplosPrueba(void);
char *submenu_tex_base(char *parametros_en_texto_a_splitear);
char *submenu_enlasador_mandar_mensajes(char *parametros_en_texto_a_splitear);
char *submenu_operaciones_de_texto(char *parametros_en_texto_a_splitear);

char *leerArchivo(const char *ruta);
char *escribirLinea(const char *ruta, const char *nuevaLinea);
char *editarLinea(const char *ruta, int idLinea, const char *nuevoTexto);
char *editarColumna(const char *ruta, int idLinea, int idColumna, const char *nuevoValor);
char *eliminarLinea(const char *ruta, int idLinea);
char *vaciarLinea(const char *ruta, int idLinea);
#pragma endregion
// ============================================================================
//  DECLARACIÓN DE FUNCIONES MENSAJERIA
// ============================================================================

#pragma region "FUNCIONES MENSAJERIA"

char *mandar_mensje_a_todos(const char *mensaje);
char *mandar_mensje_a_contacto(const char *mensaje, const char *contactos, int id_opcional);
char *mandar_mensje_al_primero_que_responda(const char *mensaje_pregunta,
                                            const char *mensaje_de_que_ya_alguien_lo_acepto,
                                            const char *menaje_respuesta_al_quien_lo_logro);
char *checar_si_hay_mensajes_no_leido(void);
#pragma endregion

// ============================================================================
//  DECLARACIÓN DE FUNCIONES OPERACIONES DE TEXTO
// ============================================================================
#pragma region "FUNCIONES OPERACIONES DE TEXTO"

char **split(const char *texto, const char *delimitador, int *cantidad);
void liberarSplit(char **partes, int cantidad);
char *join(char **arreglo, int cantidad, const char *carcter_separacion);
char *crearResultado(int codigo, const char *separador, const char *informacion);
int codigoResultado(const char *resultado);

char *extraccion_del_resultado(const char *resultado);

#pragma endregion

// ============================================================================
int main(void)
{
    int opcion = 0;
    char *resultado = NULL;

    do
    {
        printf("\n=== MENÚ PRINCIPAL ===\n");
        printf("1. comandos_tex_base\n");
        printf("2. enlasador_mandar_mensajes\n");
        printf("3. operaciones_de_texto\n");
        printf("4. Salir\n");
        printf("Seleccione una opción: ");

        char *optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion)
        {
        case 1:
        {
            char *parametros = "l";
            resultado = submenu_tex_base(parametros);
            char *informacionMain = extraccion_del_resultado(resultado);
            char *resultadoMain = crearResultado(codigoResultado(resultado),
                                                 GG_caracter_para_confirmacion_o_error[0], informacionMain);
            printf("%s\n", resultadoMain);
            free(informacionMain);
            free(resultado);
            free(resultadoMain);
            break;
        }

        case 2:
        {
            char *parametros = "mandar_todos,mandar_contacto,mandar_primero";
            resultado = submenu_enlasador_mandar_mensajes(parametros);
            char *informacionMain = extraccion_del_resultado(resultado);
            char *resultadoMain = crearResultado(codigoResultado(resultado),
                                                 GG_caracter_para_confirmacion_o_error[0], informacionMain);
            printf("%s\n", resultadoMain);
            free(informacionMain);
            free(resultado);
            free(resultadoMain);
            break;
        }

        case 3:
        {
            char *parametros = "split,modificar_columna,leer_linea";
            resultado = submenu_operaciones_de_texto(parametros);
            char *informacionMain = extraccion_del_resultado(resultado);
            char *resultadoMain = crearResultado(codigoResultado(resultado),
                                                 GG_caracter_para_confirmacion_o_error[0], informacionMain);
            printf("%s\n", resultadoMain);
            free(informacionMain);
            free(resultado);
            free(resultadoMain);
            break;
        }

        case 4:
        {
            printf("Saliendo...\n");
            free(resultado);
            resultado = crearResultado(0, GG_caracter_para_confirmacion_o_error[0], "salida_ok");
            printf("%s\n", resultado);
            break;
        }
        default:
        {
            printf("Opción no válida.\n");
            resultado = crearResultado(-2, GG_caracter_para_confirmacion_o_error[0], "opcion_no_valida");
            break;
        }
        }

    } while (opcion != 4);

    int codigoFinal = codigoResultado(resultado);
    free(resultado);
    return codigoFinal;
}

// ============================================================================
//  FUNCIÓN SUBMENÚS
// ============================================================================

char *submenu_tex_base(char *parametros_en_texto_a_splitear)
{
    int opcion = 0;
    int idLinea = 0;
    int idColumna = 0;
    char *resultado = NULL;
    char *estado = NULL;
    int cantidad = 0;
    char **parametros_espliteados = NULL;

    if (parametros_en_texto_a_splitear != NULL)
    {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split tex_base] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++)
        {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    {
        printf("\n=== SUBMENÚ comandos_tex_base ===\n");
        printf("1. Leer todo el archivo\n");
        printf("2. Añadir nueva línea\n");
        printf("3. Editar línea completa por ID\n");
        printf("4. Editar columna específica de una línea\n");
        printf("5. Eliminar línea por ID\n");
        printf("6. Vaciar línea por ID\n");
        printf("7. Ejecutar ejemplos de prueba\n");
        printf("8. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        char *optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion)
        {
        case 1:
            free(resultado);
            resultado = leerArchivo(NOMBRE_ARCHIVO);
            free(estado);
            char *informacionLeer = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacionLeer);
            free(informacionLeer);
            break;

        case 2:
        {
            printf("Ingrese el texto/línea a añadir: ");
            char *texto = leerLineaDinamica(stdin);
            free(resultado);
            resultado = escribirLinea(NOMBRE_ARCHIVO, texto);
            free(texto);
            free(estado);
            char *informacionPruebas = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacionPruebas);
            free(informacionPruebas);
            break;
        }

        case 3:
        {
            printf("Ingrese ID de línea a editar: ");
            char *inId = leerLineaDinamica(stdin);
            idLinea = (inId != NULL) ? atoi(inId) : 0;
            free(inId);

            printf("Ingrese el nuevo contenido completo: ");
            char *texto = leerLineaDinamica(stdin);
            free(resultado);
            resultado = editarLinea(NOMBRE_ARCHIVO, idLinea, texto);
            free(texto);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 4:
        {
            printf("Ingrese ID de línea a editar: ");
            char *inId = leerLineaDinamica(stdin);
            idLinea = (inId != NULL) ? atoi(inId) : 0;
            free(inId);

            printf("Ingrese el número de columna a editar (1, 2, ...): ");
            char *inCol = leerLineaDinamica(stdin);
            idColumna = (inCol != NULL) ? atoi(inCol) : 0;
            free(inCol);

            printf("Ingrese el nuevo valor para esa columna: ");
            char *valor = leerLineaDinamica(stdin);
            free(resultado);
            resultado = editarColumna(NOMBRE_ARCHIVO, idLinea, idColumna, valor);
            free(valor);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 5:
        {
            printf("Ingrese ID de línea a eliminar: ");
            char *inId = leerLineaDinamica(stdin);
            idLinea = (inId != NULL) ? atoi(inId) : 0;
            free(inId);

            free(resultado);
            resultado = eliminarLinea(NOMBRE_ARCHIVO, idLinea);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 6:
        {
            printf("Ingrese ID de línea a vaciar: ");
            char *inId = leerLineaDinamica(stdin);
            idLinea = (inId != NULL) ? atoi(inId) : 0;
            free(inId);

            free(resultado);
            resultado = vaciarLinea(NOMBRE_ARCHIVO, idLinea);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 7:
            free(resultado);
            resultado = ejecutarEjemplosPrueba();
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;

        case 8:
            printf("Volviendo al menú principal...\n");
            return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "volver");

        default:
            printf("Opción no válida.\n");
            free(estado);
            estado = crearResultado(-2, GG_caracter_para_confirmacion_o_error[1], "opcion_no_valida");
        }

        if (opcion >= 1 && opcion <= 6)
        {
            printf("%s\n", estado);
        }

        if (opcion == 7)
        {
            printf("%s\n", estado);
        }

    }
    free(resultado);
    return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "sin_resultado");
}

char *submenu_enlasador_mandar_mensajes(char *parametros_en_texto_a_splitear)
{
    int opcion = 0;
    char *resultado = NULL;
    char *estado = NULL;
    int cantidad = 0;
    char **parametros_espliteados = NULL;

    if (parametros_en_texto_a_splitear != NULL)
    {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split mensajes] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++)
        {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    {
        printf("\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
        printf("1. mandar_mensje_a_todos(mensaje)\n");
        printf("2. mandar_mensje_a_contacto(mensaje, contactos, id_opcional)\n");
        printf("3. mandar_mensje_al_primero_que_responda(mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto, "
               "menaje_respuesta_al_quien_lo_logro)\n");
        printf("4. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        char *optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion)
        {
        case 1:
        {
            printf("Ingrese el mensaje para todos: ");
            char *mensaje = leerLineaDinamica(stdin);
            free(resultado);
            resultado = mandar_mensje_a_todos(mensaje);
            free(mensaje);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 2:
        {
            printf("Ingrese el mensaje: ");
            char *mensaje = leerLineaDinamica(stdin);
            printf("Ingrese la lista de contactos: ");
            char *contactos = leerLineaDinamica(stdin);
            printf("Ingrese id opcional: ");
            char *idStr = leerLineaDinamica(stdin);
            int id_opcional = (idStr != NULL) ? atoi(idStr) : 0;
            free(idStr);

            free(resultado);
            resultado = mandar_mensje_a_contacto(mensaje, contactos, id_opcional);
            free(mensaje);
            free(contactos);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 3:
        {
            printf("Ingrese el mensaje de pregunta: ");
            char *pregunta = leerLineaDinamica(stdin);
            printf("Ingrese el mensaje de que alguien ya lo acepto: ");
            char *aceptado = leerLineaDinamica(stdin);
            printf("Ingrese la respuesta al quien lo logro: ");
            char *respuesta = leerLineaDinamica(stdin);

            free(resultado);
            resultado = mandar_mensje_al_primero_que_responda(pregunta, aceptado, respuesta);
            free(pregunta);
            free(aceptado);
            free(respuesta);
            free(estado);
            char *informacion = extraccion_del_resultado(resultado);
            estado = crearResultado(codigoResultado(resultado), GG_caracter_para_confirmacion_o_error[1], informacion);
            free(informacion);
            break;
        }

        case 4:
            printf("Volviendo al menú principal...\n");
            return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "volver");

        default:
            printf("Opción no válida.\n");
            free(estado);
            estado = crearResultado(-2, GG_caracter_para_confirmacion_o_error[1], "opcion_no_valida");
            break;
        }

        printf("%s\n", estado);

    }
    free(resultado);
    return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "sin_resultado");
}

char *submenu_operaciones_de_texto(char *parametros_en_texto_a_splitear)
{
    int opcion = 0;
    int cantidad = 0;
    char **parametros_espliteados = NULL;
    char *estado = NULL;

    if (parametros_en_texto_a_splitear != NULL)
    {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split operaciones_texto] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++)
        {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    {
        printf("\n=== SUBMENÚ operaciones_de_texto ===\n");
        printf("1. split(texto, delimitador)\n");
        printf("2. modificarColumna(linea, columna, nuevoValor)\n");
        printf("3. leerLineaDinamica(FILE*)\n");
        printf("4. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        char *optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion)
        {
        case 1:
        {
            printf("Ingrese el texto a partir: ");
            char *texto = leerLineaDinamica(stdin);
            printf("Ingrese el delimitador: ");
            char *delimitador = leerLineaDinamica(stdin);
            int total = 0;
            char **partes = split(texto, delimitador, &total);

            if (partes == NULL)
            {
                free(texto);
                free(delimitador);
                free(estado);
                estado = crearResultado(-10, GG_caracter_para_confirmacion_o_error[1], "split_no_pudo_separar_texto");
                break;
            }

            for (int i = 0; i < total; i++)
            {
                printf("  parte[%d] = %s\n", i, partes[i]);
            }

            liberarSplit(partes, total);
            free(texto);
            free(delimitador);
            free(estado);
            estado = crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "split_ok");
            break;
        }

        case 2:
        {
            printf("Ingrese la línea original: ");
            char *linea = leerLineaDinamica(stdin);
            printf("Ingrese la columna a cambiar: ");
            char *colStr = leerLineaDinamica(stdin);
            int columna = (colStr != NULL) ? atoi(colStr) : 0;
            free(colStr);

            printf("Ingrese el nuevo valor: ");
            char *nuevo = leerLineaDinamica(stdin);
            char *resultadoMod = modificarColumna(linea, columna, nuevo);

            if (resultadoMod == NULL)
            {
                free(linea);
                free(nuevo);
                free(estado);
                estado = crearResultado(-11, GG_caracter_para_confirmacion_o_error[1],
                                        "modificarColumna_no_pudo_cambiar_columna");
                break;
            }

            free(resultadoMod);
            free(linea);
            free(nuevo);
            free(estado);
            estado = crearResultado(2, GG_caracter_para_confirmacion_o_error[1], "modificarColumna_ok");
            break;
        }

        case 3:
        {
            printf("Ingrese una línea de texto: ");
            char *linea = leerLineaDinamica(stdin);
            if (linea == NULL)
            {
                free(estado);
                estado = crearResultado(-12, GG_caracter_para_confirmacion_o_error[1],
                                        "leerLineaDinamica_no_pudo_leer_linea");
                break;
            }

            free(linea);
            free(estado);
            estado = crearResultado(3, GG_caracter_para_confirmacion_o_error[1], "leerLineaDinamica_ok");
            break;
        }

        case 4:
            printf("Volviendo al menú principal...\n");
            return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "volver");

        default:
            printf("Opción no válida.\n");
            free(estado);
            estado = crearResultado(-2, GG_caracter_para_confirmacion_o_error[1], "opcion_no_valida");
            break;
        }

    }
    return estado != NULL ? estado : crearResultado(0, GG_caracter_para_confirmacion_o_error[1], "sin_resultado");
}

// ============================================================================
// FUNCIONES OPERACIONES DE TEXTO
// ============================================================================

char **split(const char *texto, const char *delimitador, int *cantidad)
{
    char **partes = NULL;
    char *copia = NULL;
    char *token = NULL;
    int total = 0;

    if (texto == NULL || delimitador == NULL || cantidad == NULL)
    {
        return NULL;
    }

    copia = (char *)malloc(strlen(texto) + 1);
    if (copia == NULL)
    {
        return NULL;
    }
    strcpy(copia, texto);

    token = strtok(copia, delimitador);
    while (token != NULL)
    {
        char **temp = (char **)realloc(partes, (total + 1) * sizeof(char *));
        if (temp == NULL)
        {
            for (int i = 0; i < total; i++)
            {
                free(partes[i]);
            }
            free(partes);
            free(copia);
            return NULL;
        }

        partes = temp;
        partes[total] = (char *)malloc(strlen(token) + 1);
        if (partes[total] == NULL)
        {
            for (int i = 0; i <= total; i++)
            {
                free(partes[i]);
            }
            free(partes);
            free(copia);
            return NULL;
        }

        strcpy(partes[total], token);
        total++;
        token = strtok(NULL, delimitador);
    }

    free(copia);
    *cantidad = total;
    return partes;
}

void liberarSplit(char **partes, int cantidad)
{
    if (partes == NULL)
        return;

    for (int i = 0; i < cantidad; i++)
    {
        free(partes[i]);
    }
    free(partes);
}

char *join(char **arreglo, int cantidad, const char *carcter_separacion)
{
    const char *separador = (carcter_separacion != NULL) ? carcter_separacion : "";
    size_t longitud_total = 1;
    size_t longitud_separador = strlen(separador);
    char *resultado = NULL;
    char *destino = NULL;

    if (cantidad < 0 || (cantidad > 0 && arreglo == NULL))
    {
        return NULL;
    }

    for (int i = 0; i < cantidad; i++)
    {
        longitud_total += (arreglo[i] != NULL) ? strlen(arreglo[i]) : 0;
        if (i > 0)
        {
            longitud_total += longitud_separador;
        }
    }

    resultado = (char *)malloc(longitud_total);
    if (resultado == NULL)
    {
        return NULL;
    }

    destino = resultado;
    for (int i = 0; i < cantidad; i++)
    {
        const char *elemento = (arreglo[i] != NULL) ? arreglo[i] : "";
        const char *separador_actual = (i > 0) ? separador : "";

        sprintf(destino, "%s%s", separador_actual, elemento);
        destino += strlen(destino);
    }

    if (cantidad == 0)
    {
        resultado[0] = '\0';
    }

    return resultado;
}

char *crearResultado(int codigo, const char *separador, const char *informacion)
{
    const char *separadorSeguro = (separador != NULL) ? separador : "";
    const char *informacionSegura = (informacion != NULL) ? informacion : "";
    size_t longitud = (size_t)snprintf(NULL, 0, "%d%s%s", codigo, separadorSeguro, informacionSegura) + 1;
    char *resultado = (char *)malloc(longitud);

    if (resultado != NULL)
    {
        sprintf(resultado, "%d%s%s", codigo, separadorSeguro, informacionSegura);
    }

    return resultado;
}

int codigoResultado(const char *resultado)
{
    return (resultado != NULL) ? atoi(resultado) : -1;
}

char *extraccion_del_resultado(const char *resultado)
{
    const char *separadores[] = {GG_caracter_para_confirmacion_o_error[0], GG_caracter_para_confirmacion_o_error[1],
                                 GG_caracter_para_confirmacion_o_error[2], GG_caracter_para_confirmacion_o_error[3],
                                 GG_caracter_para_confirmacion_o_error[4]};
    const char *separadorEncontrado = NULL;
    const char *inicioInformacion = NULL;
    char *informacion = NULL;

    if (resultado == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < 5; i++)
    {
        const char *encontrado = strstr(resultado, separadores[i]);
        if (encontrado != NULL && (separadorEncontrado == NULL || encontrado < separadorEncontrado))
        {
            separadorEncontrado = encontrado;
            inicioInformacion = encontrado + strlen(separadores[i]);
        }
    }

    if (separadorEncontrado == NULL)
    {
        return NULL;
    }

    informacion = (char *)malloc(strlen(inicioInformacion) + 1);
    if (informacion == NULL)
    {
        return NULL;
    }
    sprintf(informacion, "%s", inicioInformacion);

    return informacion;
}

// ============================================================================
// FUNCIONES OPERACIONES DE TEX_BASE
// ============================================================================

// Lector dinámico por punteros sin límite estático de bytes
char *leerLineaDinamica(FILE *flujo)
{
    size_t capacidad = 16;
    size_t longitud = 0;
    char *cadena = (char *)malloc(capacidad * sizeof(char));

    if (cadena == NULL)
        return NULL;

    int c;
    while ((c = fgetc(flujo)) != EOF && c != '\n')
    {
        cadena[longitud++] = (char)c;

        if (longitud >= capacidad)
        {
            capacidad *= 2;
            char *ptrTemp = (char *)realloc(cadena, capacidad * sizeof(char));
            if (ptrTemp == NULL)
            {
                free(cadena);
                return NULL;
            }
            cadena = ptrTemp;
        }
    }

    if (longitud == 0 && c == EOF)
    {
        free(cadena);
        return NULL;
    }

    cadena[longitud] = '\0';
    return cadena;
}

// Auxiliar para reconstruir una línea formateada por delimitador (CSV / TXT)
char *modificarColumna(const char *lineaOriginal, int columnaTarget, const char *nuevoValor)
{
    size_t len = strlen(lineaOriginal);
    char *copia = (char *)malloc((len + 1) * sizeof(char));
    if (copia == NULL)
        return NULL;
    strcpy(copia, lineaOriginal);

    size_t capResultado = len + (nuevoValor ? strlen(nuevoValor) : 0) + 16;
    char *resultado = (char *)malloc(capResultado * sizeof(char));
    if (resultado == NULL)
    {
        free(copia);
        return NULL;
    }
    resultado[0] = '\0';

    int colActual = 1;
    size_t posicion = 0;
    char *token = strtok(copia, DELIMITADOR);

    while (token != NULL)
    {
        if (colActual > 1)
            posicion += (size_t)sprintf(resultado + posicion, "%s", DELIMITADOR);

        if (colActual == columnaTarget)
        {
            posicion += (size_t)sprintf(resultado + posicion, "%s", (nuevoValor != NULL) ? nuevoValor : "");
        }
        else
        {
            posicion += (size_t)sprintf(resultado + posicion, "%s", token);
        }

        token = strtok(NULL, DELIMITADOR);
        colActual++;
    }

    free(copia);
    return resultado;
}

// Muestra el contenido completo del archivo
char *leerArchivo(const char *ruta)
{
    if (ruta == NULL)
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "leerArchivo_ruta_nula");

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "leerArchivo_no_pudo_abrir_archivo");

    int numLinea = 1;
    char *linea = NULL;

    printf("\n--- CONTENIDO DE [%s] ---\n", ruta);
    while ((linea = leerLineaDinamica(archivo)) != NULL)
    {
        printf("%d: %s\n", numLinea++, linea);
        free(linea);
    }
    printf("-----------------------------------\n");

    fclose(archivo);
    return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "leerArchivo_ok");
}

// Añade una nueva fila/línea al final
char *escribirLinea(const char *ruta, const char *nuevaLinea)
{
    if (ruta == NULL || nuevaLinea == NULL)
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "escribirLinea_parametro_nulo");

    FILE *archivo = fopen(ruta, "a");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "escribirLinea_no_pudo_abrir_archivo");

    fprintf(archivo, "%s\n", nuevaLinea);
    fclose(archivo);
    return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "escribirLinea_ok");
}

// Reemplaza una línea entera según su ID
char *editarLinea(const char *ruta, int idLinea, const char *nuevoTexto)
{
    if (ruta == NULL || idLinea <= 0)
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "editarLinea_parametro_invalido");

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "editarLinea_no_pudo_abrir_archivo");

    FILE *temporal = fopen("temp.txt", "w");
    if (temporal == NULL)
    {
        fclose(archivo);
        return crearResultado(-3, GG_caracter_para_confirmacion_o_error[2], "editarLinea_no_pudo_crear_temporal");
    }

    int actualLinea = 1;
    int editado = 0;
    char *linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL)
    {
        if (actualLinea == idLinea)
        {
            fprintf(temporal, "%s\n", (nuevoTexto != NULL) ? nuevoTexto : "");
            editado = 1;
        }
        else
        {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return crearResultado(editado ? 0 : -4, GG_caracter_para_confirmacion_o_error[2],
                          editado ? "editarLinea_ok" : "editarLinea_id_no_encontrado");
}

// Modifica únicamente una columna delimitada dentro de la línea idLinea

char *editarColumna(const char *ruta, int idLinea, int idColumna, const char *nuevoValor)
{

    if (ruta == NULL || idLinea <= 0 || idColumna <= 0)
    {
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "editarColumna_parametro_invalido");
    }

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "editarColumna_no_pudo_abrir_archivo");

    FILE *temporal = fopen("temp.txt", "w");
    if (temporal == NULL)
    {
        fclose(archivo);
        return crearResultado(-3, GG_caracter_para_confirmacion_o_error[2], "editarColumna_no_pudo_crear_temporal");
    }

    int actualLinea = 1;
    int editado = 0;
    char *linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL)
    {
        if (actualLinea == idLinea)
        {
            char *lineaModificada = modificarColumna(linea, idColumna, nuevoValor);
            if (lineaModificada != NULL)
            {
                fprintf(temporal, "%s\n", lineaModificada);
                free(lineaModificada);
                editado = 1;
            }
            else
            {
                fprintf(temporal, "%s\n", linea);
            }
        }
        else
        {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return crearResultado(editado ? 0 : -4, GG_caracter_para_confirmacion_o_error[2],
                          editado ? "editarColumna_ok" : "editarColumna_id_no_encontrado");
}

// Elimina la fila idLinea reduciendo el número de registros
char *eliminarLinea(const char *ruta, int idLinea)
{
    if (ruta == NULL || idLinea <= 0)
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "eliminarLinea_parametro_invalido");

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "eliminarLinea_no_pudo_abrir_archivo");

    FILE *temporal = fopen("temp.txt", "w");
    if (temporal == NULL)
    {
        fclose(archivo);
        return crearResultado(-3, GG_caracter_para_confirmacion_o_error[2], "eliminarLinea_no_pudo_crear_temporal");
    }

    int actualLinea = 1;
    int eliminado = 0;
    char *linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL)
    {
        if (actualLinea != idLinea)
        {
            fprintf(temporal, "%s\n", linea);
        }
        else
        {
            eliminado = 1;
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return crearResultado(eliminado ? 0 : -4, GG_caracter_para_confirmacion_o_error[2],
                          eliminado ? "eliminarLinea_ok" : "eliminarLinea_id_no_encontrado");
}

// Reemplaza la línea con un renglón en blanco manteniéndola
char *vaciarLinea(const char *ruta, int idLinea)
{
    if (ruta == NULL || idLinea <= 0)
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "vaciarLinea_parametro_invalido");

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL)
        return crearResultado(-2, GG_caracter_para_confirmacion_o_error[2], "vaciarLinea_no_pudo_abrir_archivo");

    FILE *temporal = fopen("temp.txt", "w");
    if (temporal == NULL)
    {
        fclose(archivo);
        return crearResultado(-3, GG_caracter_para_confirmacion_o_error[2], "vaciarLinea_no_pudo_crear_temporal");
    }

    int actualLinea = 1;
    int vaciado = 0;
    char *linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL)
    {
        if (actualLinea == idLinea)
        {
            fprintf(temporal, "\n");
            vaciado = 1;
        }
        else
        {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return crearResultado(vaciado ? 0 : -4, GG_caracter_para_confirmacion_o_error[2],
                          vaciado ? "vaciarLinea_ok" : "vaciarLinea_id_no_encontrado");
}

// ============================================================================
// FUNCIONES OPERACIONES DE MENSAJERIA
// ============================================================================

char *checar_si_hay_mensajes_no_leido(void)
{
    const char *rutas[] = {"mensajes_todos.txt", "mensajes_contactos.txt", "mensajes_primero.txt"};
    const int totalRutas = 3;

    for (int i = 0; i < totalRutas; i++)
    {
        FILE *archivo = fopen(rutas[i], "r");
        if (archivo == NULL)
        {
            continue;
        }

        int c = fgetc(archivo);
        fclose(archivo);

        if (c != EOF)
        {
            return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "hay_mensajes_no_leidos");
        }
    }

    return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "no_hay_mensajes_no_leidos");
}

// Ejecuta ejemplos de prueba sobre las operaciones principales
char *ejecutarEjemplosPrueba(void)
{
    int ok = 1;
    const char *rutaPrueba = "pruebas_demo.txt";
    char *lineaModificada = NULL;

    printf("\n=== EJEMPLOS DE PRUEBA ===\n");

    lineaModificada = modificarColumna("Ana,25,Programador", 2, "30");
    if (lineaModificada == NULL || strcmp(lineaModificada, "Ana,30,Programador") != 0)
    {
        ok = 0;
    }
    free(lineaModificada);

    remove(rutaPrueba);
    char *resultadoOperacion = escribirLinea(rutaPrueba, "Luis,10,Desarrollador");
    int codigoOperacion = codigoResultado(resultadoOperacion);
    free(resultadoOperacion);
    if (codigoOperacion < 0)
    {
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "escribirLinea_no_pudo_crear_archivo");
    }

    resultadoOperacion = escribirLinea(rutaPrueba, "Marta,20,QA");
    codigoOperacion = codigoResultado(resultadoOperacion);
    free(resultadoOperacion);
    if (codigoOperacion < 0)
    {
        return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "escribirLinea_no_pudo_crear_archivo");
    }

    resultadoOperacion = editarColumna(rutaPrueba, 1, 2, "15");
    codigoOperacion = codigoResultado(resultadoOperacion);
    free(resultadoOperacion);
    if (codigoOperacion < 0)
    {
        ok = 0;
    }

    resultadoOperacion = eliminarLinea(rutaPrueba, 2);
    codigoOperacion = codigoResultado(resultadoOperacion);
    free(resultadoOperacion);
    if (codigoOperacion < 0)
    {
        ok = 0;
    }

    printf("\n--- ARCHIVO DE PRUEBA RESULTANTE ---\n");
    resultadoOperacion = leerArchivo(rutaPrueba);
    free(resultadoOperacion);

    remove(rutaPrueba);

    if (ok)
    {
        return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "pruebas_ok");
    }

    return crearResultado(-1, GG_caracter_para_confirmacion_o_error[2], "pruebas_con_errores");
}

char *mandar_mensje_a_todos(const char *mensaje)
{
    if (mensaje == NULL || strlen(mensaje) == 0)
    {
        return crearResultado(-10, GG_caracter_para_confirmacion_o_error[2], "mandar_mensje_a_todos_mensaje_vacio");
    }

    FILE *archivo = fopen("mensajes_todos.txt", "a");
    if (archivo == NULL)
    {
        return crearResultado(-11, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_a_todos_no_pudo_abrir_archivo");
    }

    fprintf(archivo, "%s\n", mensaje);
    fclose(archivo);

    return crearResultado(0, GG_caracter_para_confirmacion_o_error[2], "mandar_mensje_a_todos_ok");
}

char *mandar_mensje_a_contacto(const char *mensaje, const char *contactos, int id_opcional)
{
    if (mensaje == NULL || strlen(mensaje) == 0)
    {
        return crearResultado(-20, GG_caracter_para_confirmacion_o_error[2], "mandar_mensje_a_contacto_mensaje_vacio");
    }

    if (contactos == NULL || strlen(contactos) == 0)
    {
        return crearResultado(-21, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_a_contacto_contactos_vacios");
    }

    FILE *archivo = fopen("mensajes_contactos.txt", "a");
    if (archivo == NULL)
    {
        return crearResultado(-22, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_a_contacto_no_pudo_abrir_archivo");
    }

    fprintf(archivo, "[%d] %s -> %s\n", id_opcional, contactos, mensaje);
    fclose(archivo);

    return crearResultado(2, GG_caracter_para_confirmacion_o_error[2], "mandar_mensje_a_contacto_ok");
}

char *mandar_mensje_al_primero_que_responda(const char *mensaje_pregunta,
                                            const char *mensaje_de_que_ya_alguien_lo_acepto,
                                            const char *menaje_respuesta_al_quien_lo_logro)
{
    if (mensaje_pregunta == NULL || strlen(mensaje_pregunta) == 0)
    {
        return crearResultado(-30, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_al_primero_que_responda_pregunta_vacia");
    }

    if (mensaje_de_que_ya_alguien_lo_acepto == NULL || strlen(mensaje_de_que_ya_alguien_lo_acepto) == 0)
    {
        return crearResultado(-31, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_al_primero_que_responda_aceptacion_vacia");
    }

    if (menaje_respuesta_al_quien_lo_logro == NULL || strlen(menaje_respuesta_al_quien_lo_logro) == 0)
    {
        return crearResultado(-32, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_al_primero_que_responda_respuesta_vacia");
    }

    FILE *archivo = fopen("mensajes_primero.txt", "a");
    if (archivo == NULL)
    {
        return crearResultado(-33, GG_caracter_para_confirmacion_o_error[2],
                              "mandar_mensje_al_primero_que_responda_no_pudo_abrir_archivo");
    }

    fprintf(archivo, "%s | %s | %s\n", mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto,
            menaje_respuesta_al_quien_lo_logro);
    fclose(archivo);

    return crearResultado(3, GG_caracter_para_confirmacion_o_error[2], "mandar_mensje_al_primero_que_responda_ok");
}
