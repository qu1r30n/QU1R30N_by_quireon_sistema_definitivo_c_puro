//para c
//
/* =============================================================================
 * Proyecto: QU1R30N_by_quireon_sistema_definitivo_c_puro
 * Archivo:  QU1R30N_by_quireon_sistema_definitivo_c_puro.c
 * Autor:    QU1R30N,QUIREON <tu_email@ejemplo.com>
 * Año:      2026
 * =============================================================================
 * Descripción:
 *   HACE TODA LA FUNCIONALIDADES DE UN SISTEMA DE NEGOCIO, CON FUNCIONES DE MENSAJERIA, MANEJO DE ARCHIVOS, Y OPERACIONES DE TEXTO.
 *   ESTE PROGRAMA ES UN SISTEMA DE NEGOCIO QUE PERMITE LEER, ESCRIBIR, EDITAR Y ELIMINAR INVENTARIO COMPRAS VENTAS Y OTROS DATOS DE NEGOCIO, ASI COMO ENVIAR MENSAJES A CONTACTOS Y GRUPOS.
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

const char* GG_caracter_separacion[] = { "|", "°", "¬", "╦", "╝", "╔" };

const char* GG_caracter_separacion_funciones_espesificas[] = { "~", "§", "¶", "╬" };

const char* GG_caracter_para_confirmacion_o_error[] = { "╣", "╠" };

const char* GG_caracter_para_transferencia_entre_archivos[] = { "┴", "■" };

const char* GG_caracter_usadas_por_usuario[] = { ":", "#", "&" };

const char* GG_caracter_para_usar_como_enter_y_nuevo_mensaje[] = { "•", "∆" };

const char* GG_id_programa = "QU1R30N_SISTEMA_DEFINITIVO";

const char* GG_direccion_control_errores_try = "config\\chatbot\\errores_try\\control_errore.txt";

#pragma endregion


// ============================================================================
//  DECLARACIÓN DE FUNCIONES TEX_BASE(PROTOTIPOS)
// ============================================================================
#pragma region "FUNCIONES TEX_BASE"
    char* leerLineaDinamica(FILE* flujo);
    char* modificarColumna(const char* lineaOriginal, int columnaTarget, const char* nuevoValor);
    int   ejecutarEjemplosPrueba(void);
    int   submenu_tex_base(char* parametros_en_texto_a_splitear);
    int   submenu_enlasador_mandar_mensajes(char* parametros_en_texto_a_splitear);
    int   submenu_operaciones_de_texto(char* parametros_en_texto_a_splitear);

    int   leerArchivo(const char* ruta);
    int   escribirLinea(const char* ruta, const char* nuevaLinea);
    int   editarLinea(const char* ruta, int idLinea, const char* nuevoTexto);
    int   editarColumna(const char* ruta, int idLinea, int idColumna, const char* nuevoValor);
    int   eliminarLinea(const char* ruta, int idLinea);
    int   vaciarLinea(const char* ruta, int idLinea);
#pragma endregion
// ============================================================================
//  DECLARACIÓN DE FUNCIONES MENSAJERIA
// ============================================================================

#pragma region "FUNCIONES MENSAJERIA"

    int   mandar_mensje_a_todos(const char* mensaje);
    int   mandar_mensje_a_contacto(const char* mensaje, const char* contactos, int id_opcional);
    int   mandar_mensje_al_primero_que_responda(const char* mensaje_pregunta,
                                            const char* mensaje_de_que_ya_alguien_lo_acepto,
                                            const char* menaje_respuesta_al_quien_lo_logro);
    int   checar_si_hay_mensajes_no_leido(void);
#pragma endregion

// ============================================================================
//  DECLARACIÓN DE FUNCIONES OPERACIONES DE TEXTO
// ============================================================================
#pragma region "FUNCIONES OPERACIONES DE TEXTO"

    char** split(const char* texto, const char* delimitador, int* cantidad);
    void   liberarSplit(char** partes, int cantidad);

#pragma endregion

// ============================================================================
//  FUNCIÓN PRINCIPAL MAIN
// ============================================================================
int main(void) {
    int opcion = 0;
    int resultado = 0;

    do {
        printf("\n=== MENÚ PRINCIPAL ===\n");
        printf("1. comandos_tex_base\n");
        printf("2. enlasador_mandar_mensajes\n");
        printf("3. operaciones_de_texto\n");
        printf("4. Salir\n");
        printf("Seleccione una opción: ");

        char* optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion) {
            case 1: {
                char* parametros = "leer,agregar,editar";
                resultado = submenu_tex_base(parametros);
                printf("[Retorno del submenú]: %d\n", resultado);
                break;
            }

            case 2: {
                char* parametros = "mandar_todos,mandar_contacto,mandar_primero";
                resultado = submenu_enlasador_mandar_mensajes(parametros);
                printf("[Retorno del submenú mensajes]: %d\n", resultado);
                break;
            }

            case 3: {
                char* parametros = "split,modificar_columna,leer_linea";
                resultado = submenu_operaciones_de_texto(parametros);
                printf("[Retorno del submenú texto]: %d\n", resultado);
                break;
            }

            case 4:{
                printf("Saliendo...\n");
                break;
            }
            default:{
                printf("Opción no válida.\n");
                resultado = -2;
                break;
            }
        }

    } while (opcion != 4);

    return resultado;
}

int submenu_tex_base(char* parametros_en_texto_a_splitear) {
    int opcion = 0;
    int idLinea = 0;
    int idColumna = 0;
    int resultado = 0;
    int estado = 0;
    int cantidad = 0;
    char** parametros_espliteados = NULL;

    if (parametros_en_texto_a_splitear != NULL) {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split tex_base] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++) {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    do {
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

        char* optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion) {
            case 1:
                resultado = leerArchivo(NOMBRE_ARCHIVO);
                estado = (resultado == 1) ? 0 : -1;
                break;

            case 2: {
                printf("Ingrese el texto/línea a añadir: ");
                char* texto = leerLineaDinamica(stdin);
                resultado = escribirLinea(NOMBRE_ARCHIVO, texto);
                free(texto);
                estado = (resultado == 1) ? 1 : -3;
                break;
            }

            case 3: {
                printf("Ingrese ID de línea a editar: ");
                char* inId = leerLineaDinamica(stdin);
                idLinea = (inId != NULL) ? atoi(inId) : 0;
                free(inId);

                printf("Ingrese el nuevo contenido completo: ");
                char* texto = leerLineaDinamica(stdin);
                resultado = editarLinea(NOMBRE_ARCHIVO, idLinea, texto);
                free(texto);
                estado = (resultado == 1) ? 2 : -4;
                break;
            }

            case 4: {
                printf("Ingrese ID de línea a editar: ");
                char* inId = leerLineaDinamica(stdin);
                idLinea = (inId != NULL) ? atoi(inId) : 0;
                free(inId);

                printf("Ingrese el número de columna a editar (1, 2, ...): ");
                char* inCol = leerLineaDinamica(stdin);
                idColumna = (inCol != NULL) ? atoi(inCol) : 0;
                free(inCol);

                printf("Ingrese el nuevo valor para esa columna: ");
                char* valor = leerLineaDinamica(stdin);
                resultado = editarColumna(NOMBRE_ARCHIVO, idLinea, idColumna, valor);
                free(valor);
                estado = (resultado == 1) ? 2 : -5;
                break;
            }

            case 5: {
                printf("Ingrese ID de línea a eliminar: ");
                char* inId = leerLineaDinamica(stdin);
                idLinea = (inId != NULL) ? atoi(inId) : 0;
                free(inId);

                resultado = eliminarLinea(NOMBRE_ARCHIVO, idLinea);
                estado = (resultado == 1) ? 3 : -6;
                break;
            }

            case 6: {
                printf("Ingrese ID de línea a vaciar: ");
                char* inId = leerLineaDinamica(stdin);
                idLinea = (inId != NULL) ? atoi(inId) : 0;
                free(inId);

                resultado = vaciarLinea(NOMBRE_ARCHIVO, idLinea);
                estado = (resultado == 1) ? 3 : -7;
                break;
            }

            case 7:
                resultado = ejecutarEjemplosPrueba();
                estado = (resultado == 1) ? 0 : -8;
                break;

            case 8:
                printf("Volviendo al menú principal...\n");
                return estado;

            default:
                printf("Opción no válida.\n");
                estado = -2;
        }

        if (opcion >= 1 && opcion <= 6) {
            printf("[Resultado de operación]: %s (Código %d)\n",
                   (resultado == 1) ? "ÉXITO" : "ERROR", resultado);
        }

        if (opcion == 7) {
            printf("[Resultado de prueba]: %s (Código %d)\n",
                   (resultado == 1) ? "PRUEBAS OK" : "PRUEBAS CON ERRORES", resultado);
        }

    } while (opcion != 8);

    return estado;
}

int submenu_enlasador_mandar_mensajes(char* parametros_en_texto_a_splitear) {
    int opcion = 0;
    int resultado = 0;
    int estado = 0;
    int cantidad = 0;
    char** parametros_espliteados = NULL;

    if (parametros_en_texto_a_splitear != NULL) {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split mensajes] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++) {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    do {
        printf("\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
        printf("1. mandar_mensje_a_todos(mensaje)\n");
        printf("2. mandar_mensje_a_contacto(mensaje, contactos, id_opcional)\n");
        printf("3. mandar_mensje_al_primero_que_responda(mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto, menaje_respuesta_al_quien_lo_logro)\n");
        printf("4. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        char* optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion) {
            case 1: {
                printf("Ingrese el mensaje para todos: ");
                char* mensaje = leerLineaDinamica(stdin);
                resultado = mandar_mensje_a_todos(mensaje);
                free(mensaje);
                estado = resultado;
                break;
            }

            case 2: {
                printf("Ingrese el mensaje: ");
                char* mensaje = leerLineaDinamica(stdin);
                printf("Ingrese la lista de contactos: ");
                char* contactos = leerLineaDinamica(stdin);
                printf("Ingrese id opcional: ");
                char* idStr = leerLineaDinamica(stdin);
                int id_opcional = (idStr != NULL) ? atoi(idStr) : 0;
                free(idStr);

                resultado = mandar_mensje_a_contacto(mensaje, contactos, id_opcional);
                free(mensaje);
                free(contactos);
                estado = resultado;
                break;
            }

            case 3: {
                printf("Ingrese el mensaje de pregunta: ");
                char* pregunta = leerLineaDinamica(stdin);
                printf("Ingrese el mensaje de que alguien ya lo acepto: ");
                char* aceptado = leerLineaDinamica(stdin);
                printf("Ingrese la respuesta al quien lo logro: ");
                char* respuesta = leerLineaDinamica(stdin);

                resultado = mandar_mensje_al_primero_que_responda(pregunta, aceptado, respuesta);
                free(pregunta);
                free(aceptado);
                free(respuesta);
                estado = resultado;
                break;
            }

            case 4:
                printf("Volviendo al menú principal...\n");
                return estado;

            default:
                printf("Opción no válida.\n");
                estado = -2;
                break;
        }

        if (resultado > 0) {
            printf("[Resultado del envío]: %d\n", resultado);
        } else if (resultado < 0) {
            printf("[Error del envío]: %d\n", resultado);
        }

    } while (opcion != 4);

    return estado;
}

int submenu_operaciones_de_texto(char* parametros_en_texto_a_splitear) {
    int opcion = 0;
    int cantidad = 0;
    char** parametros_espliteados = NULL;
    int estado = 0;

    if (parametros_en_texto_a_splitear != NULL) {
        parametros_espliteados = split(parametros_en_texto_a_splitear, ",", &cantidad);
        printf("[split operaciones_texto] elementos: %d\n", cantidad);
        for (int i = 0; i < cantidad; i++) {
            printf("  [%d] %s\n", i, parametros_espliteados[i]);
        }
        liberarSplit(parametros_espliteados, cantidad);
    }

    do {
        printf("\n=== SUBMENÚ operaciones_de_texto ===\n");
        printf("1. split(texto, delimitador)\n");
        printf("2. modificarColumna(linea, columna, nuevoValor)\n");
        printf("3. leerLineaDinamica(FILE*)\n");
        printf("4. Volver al menú principal\n");
        printf("Seleccione una opción: ");

        char* optStr = leerLineaDinamica(stdin);
        opcion = (optStr != NULL) ? atoi(optStr) : 0;
        free(optStr);

        switch (opcion) {
            case 1: {
                printf("Ingrese el texto a partir: ");
                char* texto = leerLineaDinamica(stdin);
                printf("Ingrese el delimitador: ");
                char* delimitador = leerLineaDinamica(stdin);
                int total = 0;
                char** partes = split(texto, delimitador, &total);

                if (partes == NULL) {
                    printf("[ERROR] split: no se pudo separar el texto\n");
                    free(texto);
                    free(delimitador);
                    estado = -10;
                    break;
                }

                printf("[OK] split: partes encontradas = %d\n", total);
                for (int i = 0; i < total; i++) {
                    printf("  parte[%d] = %s\n", i, partes[i]);
                }

                liberarSplit(partes, total);
                free(texto);
                free(delimitador);
                estado = 1;
                break;
            }

            case 2: {
                printf("Ingrese la línea original: ");
                char* linea = leerLineaDinamica(stdin);
                printf("Ingrese la columna a cambiar: ");
                char* colStr = leerLineaDinamica(stdin);
                int columna = (colStr != NULL) ? atoi(colStr) : 0;
                free(colStr);

                printf("Ingrese el nuevo valor: ");
                char* nuevo = leerLineaDinamica(stdin);
                char* resultadoMod = modificarColumna(linea, columna, nuevo);

                if (resultadoMod == NULL) {
                    printf("[ERROR] modificarColumna: no se pudo cambiar la columna\n");
                    free(linea);
                    free(nuevo);
                    estado = -11;
                    break;
                }

                printf("[OK] modificarColumna: %s\n", resultadoMod);
                free(resultadoMod);
                free(linea);
                free(nuevo);
                estado = 2;
                break;
            }

            case 3: {
                printf("Ingrese una línea de texto: ");
                char* linea = leerLineaDinamica(stdin);
                if (linea == NULL) {
                    printf("[ERROR] leerLineaDinamica: no se pudo leer la línea\n");
                    estado = -12;
                    break;
                }

                printf("[OK] leerLineaDinamica: %s\n", linea);
                free(linea);
                estado = 3;
                break;
            }

            case 4:
                printf("Volviendo al menú principal...\n");
                return estado;

            default:
                printf("Opción no válida.\n");
                estado = -2;
                break;
        }

    } while (opcion != 4);

    return estado;
}

// ============================================================================
// 3. DEFINICIÓN DE FUNCIONES
// ============================================================================

char** split(const char* texto, const char* delimitador, int* cantidad) {
    char** partes = NULL;
    char* copia = NULL;
    char* token = NULL;
    int total = 0;

    if (texto == NULL || delimitador == NULL || cantidad == NULL) {
        return NULL;
    }

    copia = (char*)malloc(strlen(texto) + 1);
    if (copia == NULL) {
        return NULL;
    }
    strcpy(copia, texto);

    token = strtok(copia, delimitador);
    while (token != NULL) {
        char** temp = (char**)realloc(partes, (total + 1) * sizeof(char*));
        if (temp == NULL) {
            for (int i = 0; i < total; i++) {
                free(partes[i]);
            }
            free(partes);
            free(copia);
            return NULL;
        }

        partes = temp;
        partes[total] = (char*)malloc(strlen(token) + 1);
        if (partes[total] == NULL) {
            for (int i = 0; i <= total; i++) {
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

void liberarSplit(char** partes, int cantidad) {
    if (partes == NULL) return;

    for (int i = 0; i < cantidad; i++) {
        free(partes[i]);
    }
    free(partes);
}

// Lector dinámico por punteros sin límite estático de bytes
char* leerLineaDinamica(FILE* flujo) {
    size_t capacidad = 16;
    size_t longitud = 0;
    char* cadena = (char*)malloc(capacidad * sizeof(char));

    if (cadena == NULL) return NULL;

    int c;
    while ((c = fgetc(flujo)) != EOF && c != '\n') {
        cadena[longitud++] = (char)c;

        if (longitud >= capacidad) {
            capacidad *= 2;
            char* ptrTemp = (char*)realloc(cadena, capacidad * sizeof(char));
            if (ptrTemp == NULL) {
                free(cadena);
                return NULL;
            }
            cadena = ptrTemp;
        }
    }

    if (longitud == 0 && c == EOF) {
        free(cadena);
        return NULL;
    }

    cadena[longitud] = '\0';
    return cadena;
}

// Auxiliar para reconstruir una línea formateada por delimitador (CSV / TXT)
char* modificarColumna(const char* lineaOriginal, int columnaTarget, const char* nuevoValor) {
    size_t len = strlen(lineaOriginal);
    char* copia = (char*)malloc((len + 1) * sizeof(char));
    if (copia == NULL) return NULL;
    strcpy(copia, lineaOriginal);

    size_t capResultado = len + (nuevoValor ? strlen(nuevoValor) : 0) + 16;
    char* resultado = (char*)malloc(capResultado * sizeof(char));
    if (resultado == NULL) {
        free(copia);
        return NULL;
    }
    resultado[0] = '\0';

    int colActual = 1;
    char* token = strtok(copia, DELIMITADOR);

    while (token != NULL) {
        if (colActual > 1) strcat(resultado, DELIMITADOR);

        if (colActual == columnaTarget) {
            strcat(resultado, (nuevoValor != NULL) ? nuevoValor : "");
        } else {
            strcat(resultado, token);
        }

        token = strtok(NULL, DELIMITADOR);
        colActual++;
    }

    free(copia);
    return resultado;
}

int checar_si_hay_mensajes_no_leido(void) {
    const char* rutas[] = {
        "mensajes_todos.txt",
        "mensajes_contactos.txt",
        "mensajes_primero.txt"
    };
    const int totalRutas = 3;

    for (int i = 0; i < totalRutas; i++) {
        FILE* archivo = fopen(rutas[i], "r");
        if (archivo == NULL) {
            continue;
        }

        int c = fgetc(archivo);
        fclose(archivo);

        if (c != EOF) {
            printf("[OK] checar_si_hay_mensajes_no_leido: hay mensajes en %s\n", rutas[i]);
            return 1;
        }
    }

    printf("[INFO] checar_si_hay_mensajes_no_leido: no hay mensajes no leídos\n");
    return 0;
}

// Ejecuta ejemplos de prueba sobre las operaciones principales
int ejecutarEjemplosPrueba(void) {
    int ok = 1;
    const char* rutaPrueba = "pruebas_demo.txt";
    char* lineaModificada = NULL;

    printf("\n=== EJEMPLOS DE PRUEBA ===\n");

    lineaModificada = modificarColumna("Ana,25,Programador", 2, "30");
    if (lineaModificada == NULL || strcmp(lineaModificada, "Ana,30,Programador") != 0) {
        printf("[ERROR] modificarColumna: resultado inesperado\n");
        ok = 0;
    } else {
        printf("[OK] modificarColumna: %s\n", lineaModificada);
    }
    free(lineaModificada);

    remove(rutaPrueba);
    if (!escribirLinea(rutaPrueba, "Luis,10,Desarrollador") ||
        !escribirLinea(rutaPrueba, "Marta,20,QA")) {
        printf("[ERROR] escribirLinea: no se pudo crear el archivo de prueba\n");
        return 0;
    }

    if (!editarColumna(rutaPrueba, 1, 2, "15")) {
        printf("[ERROR] editarColumna: no se modificó la columna\n");
        ok = 0;
    } else {
        printf("[OK] editarColumna: la primera fila fue actualizada\n");
    }

    if (!eliminarLinea(rutaPrueba, 2)) {
        printf("[ERROR] eliminarLinea: no se pudo borrar la segunda línea\n");
        ok = 0;
    } else {
        printf("[OK] eliminarLinea: la segunda línea fue eliminada\n");
    }

    printf("\n--- ARCHIVO DE PRUEBA RESULTANTE ---\n");
    leerArchivo(rutaPrueba);

    remove(rutaPrueba);

    if (ok) {
        printf("[RESULTADO FINAL] PRUEBAS OK\n");
        return 1;
    }

    printf("[RESULTADO FINAL] PRUEBAS CON ERRORES\n");
    return 0;
}

int mandar_mensje_a_todos(const char* mensaje) {
    if (mensaje == NULL || strlen(mensaje) == 0) {
        printf("[ERROR] mandar_mensje_a_todos: mensaje vacío\n");
        return -10;
    }

    FILE* archivo = fopen("mensajes_todos.txt", "a");
    if (archivo == NULL) {
        printf("[ERROR] mandar_mensje_a_todos: no se pudo abrir el archivo\n");
        return -11;
    }

    fprintf(archivo, "%s\n", mensaje);
    fclose(archivo);

    printf("[OK] mandar_mensje_a_todos: mensaje enviado a todos -> %s\n", mensaje);
    return 1;
}

int mandar_mensje_a_contacto(const char* mensaje, const char* contactos, int id_opcional) {
    if (mensaje == NULL || strlen(mensaje) == 0) {
        printf("[ERROR] mandar_mensje_a_contacto: mensaje vacío\n");
        return -20;
    }

    if (contactos == NULL || strlen(contactos) == 0) {
        printf("[ERROR] mandar_mensje_a_contacto: contactos vacíos\n");
        return -21;
    }

    FILE* archivo = fopen("mensajes_contactos.txt", "a");
    if (archivo == NULL) {
        printf("[ERROR] mandar_mensje_a_contacto: no se pudo abrir el archivo\n");
        return -22;
    }

    fprintf(archivo, "[%d] %s -> %s\n", id_opcional, contactos, mensaje);
    fclose(archivo);

    printf("[OK] mandar_mensje_a_contacto: mensaje enviado a %s con id %d\n", contactos, id_opcional);
    return 2;
}

int mandar_mensje_al_primero_que_responda(const char* mensaje_pregunta,
                                          const char* mensaje_de_que_ya_alguien_lo_acepto,
                                          const char* menaje_respuesta_al_quien_lo_logro) {
    if (mensaje_pregunta == NULL || strlen(mensaje_pregunta) == 0) {
        printf("[ERROR] mandar_mensje_al_primero_que_responda: pregunta vacía\n");
        return -30;
    }

    if (mensaje_de_que_ya_alguien_lo_acepto == NULL || strlen(mensaje_de_que_ya_alguien_lo_acepto) == 0) {
        printf("[ERROR] mandar_mensje_al_primero_que_responda: mensaje de aceptación vacío\n");
        return -31;
    }

    if (menaje_respuesta_al_quien_lo_logro == NULL || strlen(menaje_respuesta_al_quien_lo_logro) == 0) {
        printf("[ERROR] mandar_mensje_al_primero_que_responda: respuesta vacía\n");
        return -32;
    }

    FILE* archivo = fopen("mensajes_primero.txt", "a");
    if (archivo == NULL) {
        printf("[ERROR] mandar_mensje_al_primero_que_responda: no se pudo abrir el archivo\n");
        return -33;
    }

    fprintf(archivo, "%s | %s | %s\n",
            mensaje_pregunta,
            mensaje_de_que_ya_alguien_lo_acepto,
            menaje_respuesta_al_quien_lo_logro);
    fclose(archivo);

    printf("[OK] mandar_mensje_al_primero_que_responda: flujo registrado\n");
    return 3;
}

// Muestra el contenido completo del archivo
int leerArchivo(const char* ruta) {
    if (ruta == NULL) return 0;

    FILE* archivo = fopen(ruta, "r");
    if (archivo == NULL) return 0;

    int numLinea = 1;
    char* linea = NULL;

    printf("\n--- CONTENIDO DE [%s] ---\n", ruta);
    while ((linea = leerLineaDinamica(archivo)) != NULL) {
        printf("%d: %s\n", numLinea++, linea);
        free(linea);
    }
    printf("-----------------------------------\n");

    fclose(archivo);
    return 1;
}

// Añade una nueva fila/línea al final
int escribirLinea(const char* ruta, const char* nuevaLinea) {
    if (ruta == NULL || nuevaLinea == NULL) return 0;

    FILE* archivo = fopen(ruta, "a");
    if (archivo == NULL) return 0;

    fprintf(archivo, "%s\n", nuevaLinea);
    fclose(archivo);
    return 1;
}

// Reemplaza una línea entera según su ID
int editarLinea(const char* ruta, int idLinea, const char* nuevoTexto) {
    if (ruta == NULL || idLinea <= 0) return 0;

    FILE* archivo = fopen(ruta, "r");
    if (archivo == NULL) return 0;

    FILE* temporal = fopen("temp.txt", "w");
    if (temporal == NULL) {
        fclose(archivo);
        return 0;
    }

    int actualLinea = 1;
    int editado = 0;
    char* linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL) {
        if (actualLinea == idLinea) {
            fprintf(temporal, "%s\n", (nuevoTexto != NULL) ? nuevoTexto : "");
            editado = 1;
        } else {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return editado;
}

// Modifica únicamente una columna delimitada dentro de la línea idLinea
int editarColumna(const char* ruta, int idLinea, int idColumna, const char* nuevoValor) {
    if (ruta == NULL || idLinea <= 0 || idColumna <= 0) return 0;

    FILE* archivo = fopen(ruta, "r");
    if (archivo == NULL) return 0;

    FILE* temporal = fopen("temp.txt", "w");
    if (temporal == NULL) {
        fclose(archivo);
        return 0;
    }

    int actualLinea = 1;
    int editado = 0;
    char* linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL) {
        if (actualLinea == idLinea) {
            char* lineaModificada = modificarColumna(linea, idColumna, nuevoValor);
            if (lineaModificada != NULL) {
                fprintf(temporal, "%s\n", lineaModificada);
                free(lineaModificada);
                editado = 1;
            } else {
                fprintf(temporal, "%s\n", linea);
            }
        } else {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return editado;
}

// Elimina la fila idLinea reduciendo el número de registros
int eliminarLinea(const char* ruta, int idLinea) {
    if (ruta == NULL || idLinea <= 0) return 0;

    FILE* archivo = fopen(ruta, "r");
    if (archivo == NULL) return 0;

    FILE* temporal = fopen("temp.txt", "w");
    if (temporal == NULL) {
        fclose(archivo);
        return 0;
    }

    int actualLinea = 1;
    int eliminado = 0;
    char* linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL) {
        if (actualLinea != idLinea) {
            fprintf(temporal, "%s\n", linea);
        } else {
            eliminado = 1;
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return eliminado;
}

// Reemplaza la línea con un renglón en blanco manteniéndola
int vaciarLinea(const char* ruta, int idLinea) {
    if (ruta == NULL || idLinea <= 0) return 0;

    FILE* archivo = fopen(ruta, "r");
    if (archivo == NULL) return 0;

    FILE* temporal = fopen("temp.txt", "w");
    if (temporal == NULL) {
        fclose(archivo);
        return 0;
    }

    int actualLinea = 1;
    int vaciado = 0;
    char* linea = NULL;

    while ((linea = leerLineaDinamica(archivo)) != NULL) {
        if (actualLinea == idLinea) {
            fprintf(temporal, "\n");
            vaciado = 1;
        } else {
            fprintf(temporal, "%s\n", linea);
        }
        actualLinea++;
        free(linea);
    }

    fclose(archivo);
    fclose(temporal);

    remove(ruta);
    rename("temp.txt", ruta);
    return vaciado;
}