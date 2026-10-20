 /*
 * Guía rápida de lectura:
 * - El código común contiene menús, operaciones de texto, CRUD y mensajería.
 * - sistema_memoria_*, sistema_archivo_* y sistema_consola_* son fronteras
 *   entre ese código común y el backend de cada plataforma.
 * - Los resultados de las operaciones siguen el formato:
 *   "codigo|informacion|resultado_anterior|funcion|profundidad".
 *   Ejemplo de éxito: "1|escritura_ok||escribirLinea|1".
 * - Los comentarios con "Ejemplo" muestran datos hipotéticos para explicar
 *   el flujo; no son valores fijos usados por el programa.
 */
 /* para c puro
  * =============================================================================
  * Proyecto: QU1R30N_by_quireon_sistema_definitivo_c_puro
  * Archivo:  QU1R30N_by_quireon_sistema_definitivo_c_puro.c
  * Autor:    QU1R30N,QUIREON <tu_email@ejemplo.com>
  * Año:      2026
  * =============================================================================
  * Descripción:
  *   Hace las funcionalidades de un sistema de negocio, con funciones de
  *   mensajería, manejo de archivos y operaciones de texto.
  *
  *   Este programa permite leer, escribir, editar y eliminar inventario,
  *   compras, ventas y otros datos de negocio, así como enviar mensajes
  *   a contactos y grupos.
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
 #include <stdarg.h>     /* Permite recibir argumentos variables en el formateador común. */

#if !defined(SEMICONDUCTOR) || defined(PIC16F)
 #include <stdio.h>
#endif

#if !defined(SEMICONDUCTOR)
 #include <locale.h>
#endif

 #include <stdlib.h>     /* EXIT_SUCCESS/EXIT_FAILURE; malloc/free solo en el backend de escritorio. */

 #include <string.h>     /* strlen, memcpy, strcmp y otras operaciones portables de texto. */

 #include <stddef.h>     /* size_t y NULL. */

 #include <limits.h>     /* INT_MIN, INT_MAX y CHAR_BIT para conversiones con límites seguros. */



/*
 * Selección del backend. SEMICONDUCTOR tiene prioridad aunque el compilador
 * también defina macros del host; esto facilita revisar los stubs desde PC.
 */
 #pragma region plataforma
 #if defined(SEMICONDUCTOR)
 #define PLATAFORMA_SEMICONDUCTOR
 /*
     Los microcontroladores tienen memoria limitada,
     por eso utilizaremos un buffer fijo.
 */
 #define CONCATENAR_BUFFER_SIZE 64
 #elif defined(_WIN32) || defined(_WIN64)
 
 #define PLATAFORMA_WINDOWS

 #elif defined(__linux__)
 #define PLATAFORMA_LINUX
 /*PLATAFORMA NO SOPORTADA*/
 #else
 #error "Plataforma no soportada"
 #endif
#pragma endregion plataforma

#pragma region RUTAS_DE_ARCHIVOS

#if defined(PLATAFORMA_WINDOWS)
/* Rutas relativas o físicas elegidas para el backend de Windows. */
#define RUTA_CONTROL_ERRORES_TRY "config\\chatbot\\errores_try\\control_errore.txt"
#define RUTA_TRANSFERENCIA_BANDERAS "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\BANDERAS_sis_qu1.TXT"
#define RUTA_TRANSFERENCIA_PREGUNTAS "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\ent_sis_qu1.TXT"
#define RUTA_TRANSFERENCIA_RESPUESTAS "C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\sal_sis_qu1.TXT"
#elif defined(PLATAFORMA_LINUX)
/* Equivalentes para Linux; las rutas se mantienen agrupadas aquí. */
#define RUTA_CONTROL_ERRORES_TRY "config/chatbot/errores_try/control_errore.txt"
#define RUTA_TRANSFERENCIA_BANDERAS "config/QU1R30N_SISTEMA_DEFINITIVO/BANDERAS_sis_qu1.TXT"
#define RUTA_TRANSFERENCIA_PREGUNTAS "config/QU1R30N_SISTEMA_DEFINITIVO/ent_sis_qu1.TXT"
#define RUTA_TRANSFERENCIA_RESPUESTAS "config/QU1R30N_SISTEMA_DEFINITIVO/sal_sis_qu1.TXT"
#elif defined(PLATAFORMA_SEMICONDUCTOR)
/* Identificadores lógicos; el stub aún no los conecta a un medio físico. */
#define RUTA_CONTROL_ERRORES_TRY "control_errores_try"
#define RUTA_TRANSFERENCIA_BANDERAS "banderas_sis_qu1"
#define RUTA_TRANSFERENCIA_PREGUNTAS "entrada_sis_qu1"
#define RUTA_TRANSFERENCIA_RESPUESTAS "salida_sis_qu1"
#endif

/* Archivos de apoyo usados por las operaciones comunes de la aplicación. */
#define RUTA_ARCHIVO_TEMPORAL "temp.txt"
#define RUTA_ARCHIVO_RESPALDO "temp_qu1ron.bak"
#define RUTA_ARCHIVO_PRUEBAS "qu1ron_ejemplos.tmp"
#define RUTA_MENSAJES_TODOS "mensajes_todos.txt"
#define RUTA_MENSAJES_CONTACTOS "mensajes_contactos.txt"
#define RUTA_MENSAJES_PRIMERO "mensajes_primero.txt"

/* Archivo que usa el menú de operaciones básicas de texto. */
 #define NOMBRE_ARCHIVO "datos.txt"

#pragma endregion RUTAS_DE_ARCHIVOS

 /*
  * Tamaño de arranque, no límite máximo: las líneas largas se amplían mediante
  * la capa de memoria. Ejemplo: una línea de 80 caracteres crece desde 16.
  */
 #define TAMANO_INICIAL_LINEA 16

 /*
  * ============================================================================
  * CONFIGURACIÓN DE CONSOLA UTF-8
  * ============================================================================
  */
/*
 * Activa la configuración regional de escritorio para mostrar texto UTF-8.
 * En el backend semiconductor no se llama setlocale ni se presupone una consola.
 */
 static void configurarConsolaUTF8(void)
 {
	#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
	setlocale(LC_ALL, "");
	#endif
 }
 /* ---------------------------------------------------------------------------
    CAPA DE MEMORIA, ARCHIVOS Y CONSOLA
    --------------------------------------------------------------------------- */
#pragma region CAPA DE MEMORIA, ARCHIVOS Y CONSOLA

/*
 * Contrato de fin de lectura compartido por consola y archivos.
 * Las implementaciones de escritorio traducen EOF a este valor.
 */
typedef struct SistemaArchivo SistemaArchivo;
static char * leerLineaDinamica(SistemaArchivo * flujo, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char * leerLineaConsola(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 
 #define SISTEMA_ARCHIVO_FIN_LECTURA (-1)
/* La aplicación superior no ve FILE*, malloc ni los periféricos de consola. */
 static void * sistema_memoria_reservar(size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static void sistema_memoria_liberar(void * memoria, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static SistemaArchivo * sistema_archivo_abrir(const char * ruta,const char * modo, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_cerrar(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_eliminar(const char * ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_renombrar(const char * origen,const char * destino, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 
 static int sistema_archivo_leer_caracter(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_archivo_hay_error(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_consola_leer_caracter(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_consola_escribir_caracter(int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_consola_escribir_texto(const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int sistema_consola_escribir_formato(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *formato, ...);
 
 #pragma endregion CAPA DE MEMORIA, ARCHIVOS Y CONSOLA

 // ============================================================================
 // IDENTIDAD, SEPARADORES Y CONFIGURACIÓN GLOBAL
 // ============================================================================
#pragma region IDENTIDAD, SEPARADORES Y CONFIGURACIÓN GLOBAL
 /* Índice inicial usado por la lógica general; actualmente empieza en 1. */
 int GG_indice_donde_comensar = 1;
/* Buffer global reservado para resultados si un flujo lo necesita. */
 char * GG_resultados_de_funciones = NULL;
 /*
  * Caracteres utilizados como separadores.
  *
  * IMPORTANTE:
  * Este archivo debe estar guardado en UTF-8.
  */
/*
 * Separadores de los protocolos de texto. [0] ("|") delimita campos de
 * crearResultado; los restantes quedan disponibles para protocolos futuros.
 */
 const char * GG_caracter_separacion[] = {
 	"|",
 	"°",
 	"¬",
 	"╦",
 	"╝",
 	"╔"
 };
/* Delimitadores reservados para operaciones específicas. */
 const char * GG_caracter_separacion_funciones_espesificas[] = {
 	"~",
 	"§",
 	"¶",
 	"╬"
 };
/* Marcadores simbólicos de confirmación o error usados por la aplicación. */
 const char * GG_caracter_para_confirmacion_o_error[] = {
 	"╣",
 	"╠",
 	"⚺",
 	"⚻",
 	"⚼"
 };
/* Marcadores usados al intercambiar datos entre archivos. */
 const char * GG_caracter_para_transferencia_entre_archivos[] = {
 	"┴",
 	"■"
 };
/* Símbolos que pueden aparecer en entradas escritas por el usuario. */
 const char * GG_caracter_usadas_por_usuario[] = {
 	":",
 	"#",
 	"&"
 };
/* Símbolos visuales asignados a acciones de entrada/mensaje. */
 const char * GG_caracter_para_usar_como_enter_y_nuevo_mensaje[] = {
 	"•",
 	"∆"
 };
/* Identidad textual del programa y sus rutas configuradas por plataforma. */
 const char * GG_id_programa = "QU1R30N_SISTEMA_DEFINITIVO";
 const char * GG_direccion_control_errores_try = RUTA_CONTROL_ERRORES_TRY;
 const char * G_dir_arch_transferencia[] = {
 	/* 0 */
 	RUTA_TRANSFERENCIA_BANDERAS,
 	/* 1 - preguntas */
 	RUTA_TRANSFERENCIA_PREGUNTAS,
 	/* 2 - respuestas */
 	RUTA_TRANSFERENCIA_RESPUESTAS
 };
 
 #pragma endregion IDENTIDAD, SEPARADORES Y CONFIGURACIÓN GLOBAL
 
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES TEX_BASE
 // ============================================================================
 #pragma region DECLARACIÓN DE FUNCIONES TEX_BASE
 
 /*
  * API de texto y archivos usada por el menú. Cada char* devuelto es memoria
  * propiedad del llamador y debe liberarse con sistema_memoria_liberar().
  */
 char * modificarColumna(const char * lineaOriginal,int columnaTarget,const char * nuevoValor,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * leerArchivo(const char * ruta,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * escribirLinea(const char * ruta,const char * nuevaLinea,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * editarLinea(const char * ruta,int idLinea,const char * nuevoTexto,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * editarColumna(const char * ruta,int idLinea,int idColumna,const char * nuevoValor,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * eliminarLinea(const char * ruta,int idLinea,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * vaciarLinea(const char * ruta,int idLinea,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 
 #pragma endregion DECLARACIÓN DE FUNCIONES TEX_BASE
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE MENSAJERÍA
 // ============================================================================
 #pragma region DECLARACIÓN DE FUNCIONES DE MENSAJERÍA
 /* API de mensajería: resultados con el formato definido por crearResultado(). */
 char * mandar_mensje_a_todos(const char * mensaje,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * mandar_mensje_a_contacto(const char * mensaje,const char * contactos,int id_opcional,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * mandar_mensje_al_primero_que_responda(const char * mensaje_pregunta,const char * mensaje_de_que_ya_alguien_lo_acepto,const char * menaje_respuesta_al_quien_lo_logro,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * checar_si_hay_mensajes_no_leido(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 #pragma endregion DECLARACIÓN DE FUNCIONES DE MENSAJERÍA
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 // ============================================================================
 #pragma region DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 /*
  * Primitivas de cadenas y resultados. Las cadenas devueltas dinámicamente
  * pertenecen al llamador; split() se libera con liberarSplit().
  */
 char ** split(const char * texto,const char * delimitador,int * cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 void liberarSplit(char ** partes, int cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char * join(char ** arreglo, int cantidad,const char * carcter_separacion,char *error_o_datos_a_retornar,int nivel_de_profundidad);
  char * concatenar(const char * formato, ...);
  static char *enteroATexto(int valor, char *buffer, size_t capacidad);
 
  static int textoAEntero(const char *texto, int *resultado, char *error_o_datos_a_retornar,int nivel_de_profundidad);


 #pragma endregion DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 
// ============================================================================
 // DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 // ============================================================================
 #pragma region FUNCIONES_DE_DEPURACION

 char * ejecutarEjemplosPrueba(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 
 char * crearResultado(int codigo,const char * informacion,const char * resultado_anterior,const char * funcion_llamante,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 void imprimirMensaje_para_depurar(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *format, ...);
 void imprimirMensaje_para_depurar_arreglo(char *error_o_datos_a_retornar,int nivel_de_profundidad, char **contenido, const char *texto, int total);
 static int submenu_pruebas_auxiliares(char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada);
 static int leerCodigoResultado(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *texto, int *codigo);
 static int resultadoTieneError(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *texto);

 #pragma endregion FUNCIONES_DE_DEPURACION

 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE SUBMENÚS
 // ============================================================================
 #pragma region FUNCIONES_SUBMENUS
 char * submenu_tex_base(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada);
 char * submenu_enlasador_mandar_mensajes(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada);
 char * submenu_operaciones_de_texto(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada);
 #pragma endregion FUNCIONES_SUBMENUS
 
 // ============================================================================
 // MAIN
 // ============================================================================

 /*
  * Menú interactivo para probar las opciones de los submenús.
  * Devuelve al llamador el resultado codificado de la operación seleccionada.
  */
 char* prueba(int nivel_de_profundidad);
char* prueba(int nivel_de_profundidad)
 {
	
	nivel_de_profundidad++;
	int opcion = 0;
	int opcionSubmenu = 0;
	const char *comando = NULL;

	char *error_o_datos_a_retornar ="";
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "\n=== MENÚ DE PRUEBAS ===\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "1. Probar comandos de texto y archivos\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "2. Probar envío y consulta de mensajes\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "3. Probar operaciones de texto\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "4. Probar funciones auxiliares e infraestructura\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "5. Volver al menú principal\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "Seleccione una opción: ");

	char *entrada = leerLineaConsola(error_o_datos_a_retornar,nivel_de_profundidad);
	if(entrada == NULL){return crearResultado(-1, "no_se_pudo_leer_opcion", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}

	int entradaValida = textoAEntero(entrada, &opcion,error_o_datos_a_retornar,  nivel_de_profundidad);
	sistema_memoria_liberar(entrada,error_o_datos_a_retornar,  nivel_de_profundidad);
	if(!entradaValida){return crearResultado(-2, "opcion_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}

	switch(opcion)
	{
		case 1:
		{
			comando = "tex_base";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ comandos_tex_base ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Leer todo el archivo\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Añadir nueva línea\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Editar línea completa por ID\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Editar columna específica de una línea\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Eliminar línea por ID\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Vaciar línea por ID\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Ejecutar ejemplos de prueba\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "8. Volver al menú anterior\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
			char *entradaSubmenu = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
			if(entradaSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 8)
			{
				return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 2:
		{
			comando = "mandar_mensajes";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Enviar mensaje a todos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Enviar mensaje a un contacto\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Enviar mensaje al primero que responda\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Consultar si hay mensajes no leídos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Volver al menú anterior\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
			char *entradaSubmenu = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
			if(entradaSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 5)
			{
				return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 3:
		{
			comando = "operaciones_de_texto";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ operaciones_de_texto ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. split(texto, delimitador)\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. modificarColumna(linea, columna, nuevoValor)\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. leer una línea de texto\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Volver al menú anterior\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
			char *entradaSubmenu = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
			if(entradaSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 4)
			{
				return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 4:
		{
			comando = "pruebas_auxiliares";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ funciones auxiliares ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Probar split, liberarSplit y join\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Probar conversiones y lectura de resultados\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Probar crearResultado\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Probar reserva, redimensionamiento y liberación\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Probar primitivas de archivos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Probar consola, configuración UTF-8 y depuración\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Volver al menú de pruebas\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
			char *entradaSubmenu = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
			if(entradaSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 7)
			{
				return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 5:
			{
				char *salida = concatenar(
					"%s%s%s%s%s%s%d%s%s",
					GG_id_programa,
					GG_caracter_para_transferencia_entre_archivos[0],
					"prueba",
					GG_caracter_para_transferencia_entre_archivos[1],
					"salir",
					GG_caracter_para_transferencia_entre_archivos[1],
					0,
					GG_caracter_para_transferencia_entre_archivos[1],
					"INFORMACION_ESPEJO_NO_SE_MODIFICA"
				);
				return salida != NULL
					? salida
					: crearResultado(-1, "error_al_concatenar_opcion", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
		default:
			return crearResultado(-2, "opcion_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	char *comandoConOpcion = concatenar(
		"%s%s%d",
		comando,
		GG_caracter_para_transferencia_entre_archivos[1],
		opcionSubmenu
	);
	if(comandoConOpcion == NULL)
	{
		return crearResultado(-1, "error_al_concatenar_opcion", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	char *resultado = concatenar(
		"%s%s%s%s%s%s%s",
		GG_id_programa,
		GG_caracter_para_transferencia_entre_archivos[0],
		"prueba",
		GG_caracter_para_transferencia_entre_archivos[1],
		comandoConOpcion,
		GG_caracter_para_transferencia_entre_archivos[1],
		"INFORMACION_ESPEJO_NO_SE_MODIFICA"
	);
	sistema_memoria_liberar(comandoConOpcion, error_o_datos_a_retornar, 0);
	if(resultado == NULL){return crearResultado(-1, "error_al_concatenar_opcion", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}
	return resultado;
 }

 /*
  * Punto de entrada: muestra el menú y distribuye las opciones.
  */
int main(void)
{
	configurarConsolaUTF8();
	char *error_o_datos_a_retornar = NULL;

	while (1)
	{
		char *resultado = prueba(0);
		imprimirMensaje_para_depurar(error_o_datos_a_retornar, 0, "Resultado de prueba: %s\n", resultado);
		int codigo = -1;
		if(resultado == NULL) return -1;

		int cantidadPartesResultado = 0;
		char **partesResultado = split(
			resultado,
			GG_caracter_para_transferencia_entre_archivos[1],
			&cantidadPartesResultado,
			error_o_datos_a_retornar,
			0
		);
		if(partesResultado == NULL || cantidadPartesResultado < 2)
		{
			liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return -1;
		}
		const char *nombreFuncion = partesResultado[1];
		int opcionSubmenu = 0;
		if(cantidadPartesResultado < 4 ||
			!textoAEntero(partesResultado[2], &opcionSubmenu, error_o_datos_a_retornar, 0))
		{
			liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return -1;
		}

		if(nombreFuncion != NULL && strcmp(nombreFuncion, "salir") == 0)
		{
			liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return 0;
		}
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "tex_base") == 0 && opcionSubmenu != 8)
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = submenu_tex_base(
				"leer,escribir,editar,eliminar,vaciar,ejemplos",
				error_o_datos_a_retornar,
				1,
				opcionSubmenu
			);
		}
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "mandar_mensajes") == 0 && opcionSubmenu != 5)
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = submenu_enlasador_mandar_mensajes(
				"mandar_todos,mandar_contacto,mandar_primero",
				error_o_datos_a_retornar,
				1,
				opcionSubmenu
			);
		}
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "operaciones_de_texto") == 0 && opcionSubmenu != 4)
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = submenu_operaciones_de_texto(
				"split,modificar_columna,leer_linea",
				error_o_datos_a_retornar,
				1,
				opcionSubmenu
			);
		}
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "pruebas_auxiliares") == 0 && opcionSubmenu != 7)
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = crearResultado(
				submenu_pruebas_auxiliares(error_o_datos_a_retornar, 1, opcionSubmenu) == 0 ? 1 : -1,
				"pruebas_auxiliares",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else
		{
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, 1);
		}

		liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
		if(resultado == NULL) return -1;
		int resultadoValido = leerCodigoResultado(error_o_datos_a_retornar, 0, resultado, &codigo);
		sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
		if(!resultadoValido) return -1;
		if(codigo <= 0) return codigo;
	}
}
 


 // ============================================================================
 // FUNCIONES SUBMENÚS
 // ============================================================================
#pragma region FUNCIONES SUBMENÚS

 /*
 * Menú de lectura y CRUD del archivo datos.txt.
 * Ejemplo: una entrada "3" solicita ID y texto y reemplaza la línea 3.
 * Devuelve una cadena de resultado codificada; el llamador debe liberarla.
 */
 char * submenu_tex_base(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada)
 {
 	nivel_de_profundidad++;
 	int opcion = opcion_seleccionada;     /* Menú principal ya pudo seleccionar la acción. */
 	int idLinea = 0;                      /* Número de línea (base 1), por ejemplo 2. */
 	int idColumna = 0;                    /* Columna de CSV (base 1), por ejemplo 3. */
 	char * resultado = NULL;              /* Resultado de la última operación del archivo. */
 	char * estado = NULL;                 /* Estado que se presenta y luego se libera. */
 	int cantidad = 0;                     /* Cantidad de parámetros tras split(). */
 	char ** parametros_espliteados = NULL;/* Elementos temporales derivados del parámetro. */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "[split tex_base] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}
 	if(opcion == 0)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ comandos_tex_base ===\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Leer todo el archivo\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Añadir nueva línea\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Editar línea completa por ID\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Editar columna específica de una línea\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Eliminar línea por ID\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Vaciar línea por ID\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Ejecutar ejemplos de prueba\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "8. Volver al menú anterior\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
 		char *optStr = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 		if(!textoAEntero(optStr, &opcion,error_o_datos_a_retornar,  nivel_de_profundidad)) opcion = 0;
 		sistema_memoria_liberar(optStr,error_o_datos_a_retornar,  0);
 	}
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = leerArchivo(NOMBRE_ARCHIVO,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el texto/línea a añadir: ");
 			char * texto = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = escribirLinea(NOMBRE_ARCHIVO, texto,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese ID de línea a editar: ");
 			char * inId = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea,error_o_datos_a_retornar,  nivel_de_profundidad)) idLinea = 0;
 			sistema_memoria_liberar(inId,error_o_datos_a_retornar,  0);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el nuevo contenido completo: ");
 			char * texto = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = editarLinea(NOMBRE_ARCHIVO, idLinea, texto,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 4:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese ID de línea a editar: ");
 			char * inId = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea,error_o_datos_a_retornar,  nivel_de_profundidad)) idLinea = 0;
 			sistema_memoria_liberar(inId,error_o_datos_a_retornar,  0);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el número de columna a editar (1, 2, ...): ");
 			char * inCol = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(!textoAEntero(inCol, &idColumna,error_o_datos_a_retornar,  nivel_de_profundidad)) idColumna = 0;
 			sistema_memoria_liberar(inCol,error_o_datos_a_retornar,  0);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el nuevo valor para esa columna: ");
 			char * valor = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = editarColumna(NOMBRE_ARCHIVO, idLinea, idColumna, valor,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(valor,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 5:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese ID de línea a eliminar: ");
 			char * inId = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea,error_o_datos_a_retornar,  nivel_de_profundidad)) idLinea = 0;
 			sistema_memoria_liberar(inId,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = eliminarLinea(NOMBRE_ARCHIVO, idLinea,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 6:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese ID de línea a vaciar: ");
 			char * inId = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea,error_o_datos_a_retornar,  nivel_de_profundidad)) idLinea = 0;
 			sistema_memoria_liberar(inId,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = vaciarLinea(NOMBRE_ARCHIVO, idLinea,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 7:
 		{
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = ejecutarEjemplosPrueba(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 8:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Volviendo al menú anterior...\n");
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Opción no válida.\n");
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(-2, "opcion_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "%s\n", estado);
 	}
 	sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
 	sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
	return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Menú para guardar mensajes en los archivos configurados.
 * Ejemplo: "Hola" a todos agrega la línea "Hola" al archivo de mensajes.
 * Cada resultado devuelto usa el protocolo crearResultado() y pertenece al llamador.
 */
 char * submenu_enlasador_mandar_mensajes(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada)
 {
	nivel_de_profundidad++;
	int opcion = opcion_seleccionada;      /* Acción elegida en el menú de pruebas cuando es distinto de cero. */
 	char * resultado = NULL;               /* Resultado de mensajería, por ejemplo "1|mensaje_enviado|...". */
 	char * estado = NULL;                  /* Estado del menú, mostrado y liberado al terminar. */
 	int cantidad = 0;                      /* Elementos del parámetro informativo separado por comas. */
 	char ** parametros_espliteados = NULL; /* Partes temporales; se liberan con liberarSplit(). */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "[split mensajes] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}
 	if(opcion == 0)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. mandar_mensje_a_todos(mensaje, error_o_datos_a_retornar)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. mandar_mensje_a_contacto(mensaje, contactos, id_opcional, error_o_datos_a_retornar)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. mandar_mensje_al_primero_que_responda("
 			"mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto, "
 			"menaje_respuesta_al_quien_lo_logro, error_o_datos_a_retornar)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Consultar si hay mensajes no leídos\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Volver al menú anterior\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
 		char *optStr = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 		if(!textoAEntero(optStr, &opcion,error_o_datos_a_retornar,  nivel_de_profundidad)) opcion = 0;
 		sistema_memoria_liberar(optStr,error_o_datos_a_retornar,  0);
 	}
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el mensaje para todos: ");
 			char * mensaje = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = mandar_mensje_a_todos(mensaje,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(mensaje,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el mensaje: ");
 			char * mensaje = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese la lista de contactos: ");
 			char * contactos = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese id opcional: ");
 			char * idStr = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			int id_opcional = 0;
 			textoAEntero(idStr, &id_opcional,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(idStr,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = mandar_mensje_a_contacto(mensaje, contactos, id_opcional,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(mensaje,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(contactos,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el mensaje de pregunta: ");
 			char * pregunta = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el mensaje de que alguien ya lo aceptó: ");
 			char * aceptado = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese la respuesta a quien lo logró: ");
 			char * respuesta = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
 			resultado = mandar_mensje_al_primero_que_responda(pregunta, aceptado, respuesta,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(pregunta,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(aceptado,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(respuesta,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
		case 4:
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = checar_si_hay_mensajes_no_leido(error_o_datos_a_retornar, nivel_de_profundidad);
			if(resultado == NULL)
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
					"No se pudo crear el resultado de la consulta.\n"
				);
			}
			else
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "%s\n", resultado);
				int codigoConsulta = 0;
				if(leerCodigoResultado(error_o_datos_a_retornar, nivel_de_profundidad, resultado, &codigoConsulta))
				{
					if(codigoConsulta > 0)
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Hay mensajes sin leer.\n");
					}
					else if(codigoConsulta == 0)
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "No hay mensajes sin leer.\n");
					}
					else
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Falló la consulta de mensajes.\n");
					}
				}
			}
			break;
		}
 		case 5:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Volviendo al menú anterior...\n");
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
				return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Opción no válida.\n");
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(-2, "opcion_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "%s\n", estado);
 	}
 	sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
 	sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
	return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Menú de demostración de split(), modificarColumna() y lectura de consola.
 * Ejemplo: "a,b,c", columna 2 y "B" produce "a,B,c".
 */
 char * submenu_operaciones_de_texto(const char * parametros_en_texto_a_splitear, char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada)
 {
	nivel_de_profundidad++;
	int opcion = opcion_seleccionada;      /* Acción elegida en el menú de pruebas cuando es distinto de cero. */
 	int cantidad = 0;                      /* Número de argumentos de ejemplo divididos. */
 	char ** parametros_espliteados = NULL; /* Argumentos temporales que se liberan al mostrarlos. */
 	char * estado = NULL;                  /* Resultado de estado mostrado al finalizar la opción. */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "[split operaciones_texto] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}
 	if(opcion == 0)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ operaciones_de_texto ===\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. split(texto, delimitador)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. modificarColumna(linea, columna, nuevoValor, error_o_datos_a_retornar)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. leerLineaDinamica(consola, error_o_datos_a_retornar)\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Volver al menú anterior\n");
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
 		char *optStr = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 		if(!textoAEntero(optStr, &opcion,error_o_datos_a_retornar,  nivel_de_profundidad)) opcion = 0;
 		sistema_memoria_liberar(optStr,error_o_datos_a_retornar,  0);
 	}
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el texto a partir: ");
 			char * texto = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el delimitador: ");
 			char * delimitador = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			int total = 0; /* Número de segmentos obtenidos del texto ingresado. */
 			char ** partes = split(texto, delimitador, & total,error_o_datos_a_retornar,  nivel_de_profundidad);
 			if(partes == NULL)
 			{
 				sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
 				sistema_memoria_liberar(delimitador,error_o_datos_a_retornar,  0);
 				sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
				estado = crearResultado(-1, "error_split", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 				break;
 			}
 			for(int i = 0; i < total; i++)
 			{
 				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "  parte[%d] = %s\n", i, partes[i]);
 			}
 			liberarSplit(partes, total,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(delimitador,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese la línea original: ");
 			char * linea = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese la columna a cambiar: ");
 			char * colStr = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			int columna = 0;
 			textoAEntero(colStr, &columna,error_o_datos_a_retornar,  nivel_de_profundidad);
 			sistema_memoria_liberar(colStr,error_o_datos_a_retornar,  0);
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese el nuevo valor: ");
 			char * nuevo = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			char * resultadoMod = modificarColumna(linea, columna, nuevo,error_o_datos_a_retornar,  nivel_de_profundidad); /* Cadena nueva, p. ej. "a,B,c". */
 			if(resultadoMod == NULL)
 			{
 				sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
 				sistema_memoria_liberar(nuevo,error_o_datos_a_retornar,  0);
 				sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
				estado = crearResultado(-1, "error_modificar_columna", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 				break;
 			}
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Resultado: %s\n", resultadoMod);
 			sistema_memoria_liberar(resultadoMod,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(nuevo,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Ingrese una línea de texto: ");
 			char * linea = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
 			if(linea == NULL)
 			{
 				sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
				estado = crearResultado(-1, "error_lectura", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 				break;
 			}
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Línea recibida: %s\n", linea);
 			sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 		case 4:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Volviendo al menú anterior...\n");
 			if(estado != NULL)
 			{
 				char * tmp = estado;
 				estado = NULL;
 				return tmp;
 			}
			return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Opción no válida.\n");
 			sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
			estado = crearResultado(-2, "opcion_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "%s\n", estado);
 	}
 	sistema_memoria_liberar(estado,error_o_datos_a_retornar,  0);
	return crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
 
 #pragma endregion FUNCIONES SUBMENÚS

 // ============================================================================
 // FUNCIONES OPERACIONES DE TEXTO
 // ============================================================================
 #pragma region FUNCIONES OPERACIONES DE TEXTO
 
 /* ============================================================================
    SPLIT
    ============================================================================
  *
  * Divide un texto utilizando un separador.
  *
  * NO utiliza strtok().
  *
  * Esto permite que la función sea más controlable
  * y evita depender del estado interno de strtok().
  *
  * Ejemplo:
  *
  *      texto:
  *
  *          "uno|dos|tres"
  *
  *      separador:
  *
  *          "|"
  *
  *      resultado:
  *
  *          partes[0] = "uno"
  *          partes[1] = "dos"
  *          partes[2] = "tres"
  *
  *      cantidad = 3
  *
  * ============================================================================
  */
/*
 * Divide texto por cada aparición completa del separador y reserva cada parte.
 * Ejemplo: split("rojo||azul", "||") produce ["rojo", "azul"], cantidad=2.
 * También conserva campos vacíos: split("a,,b", ",") produce ["a", "", "b"].
 * La lista termina en NULL; quien la recibe libera con liberarSplit().
 */
 char ** split(const char *texto, const char *separador, int *cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	nivel_de_profundidad++;
 	if(cantidad == NULL) return NULL;
 	*cantidad = 0;
 	if(texto == NULL || separador == NULL || separador[0] == '\0') return NULL;

 	size_t longitud_texto = strlen(texto);             /* Bytes de entrada, sin contar '\0'. */
 	size_t longitud_separador = strlen(separador);     /* El delimitador puede tener varios bytes. */
 	size_t capacidad = 8;                              /* Ranuras iniciales del vector de partes. */
 	size_t contador = 0;                               /* Partes completas ya guardadas. */
 	size_t inicio = 0;                                 /* Primer byte del campo actual. */
 	size_t posicion = 0;                               /* Byte examinado buscando el delimitador. */
 	char **partes = sistema_memoria_reservar(capacidad * sizeof(*partes),error_o_datos_a_retornar,  nivel_de_profundidad); /* Vector dinámico. */
 	if(partes == NULL) return NULL;

 	while(posicion <= longitud_texto)
 	{
 		int encontrado = longitud_separador <= longitud_texto - posicion &&
 			strncmp(texto + posicion, separador, longitud_separador) == 0;
 		if(!encontrado && posicion < longitud_texto)
 		{
 			posicion++;
 			continue;
 		}

 		if(contador >= (size_t)INT_MAX)
 		{
 			liberarSplit(partes, (int)contador,error_o_datos_a_retornar,  nivel_de_profundidad);
 			return NULL;
 		}

 		if(contador == capacidad - 1)
 		{
 			if(capacidad > (size_t)-1 / 2 / sizeof(*partes))
 			{
 				liberarSplit(partes, (int)contador,error_o_datos_a_retornar,  nivel_de_profundidad);
 				return NULL;
 			}
 			size_t nueva_capacidad = capacidad * 2; /* Duplica ranuras para amortizar realocaciones. */
 			char **temporal = sistema_memoria_redimensionar(partes, nueva_capacidad * sizeof(*partes),error_o_datos_a_retornar,  nivel_de_profundidad);
 			if(temporal == NULL)
 			{
 				liberarSplit(partes, (int)contador,error_o_datos_a_retornar,  nivel_de_profundidad);
 				return NULL;
 			}
 			partes = temporal;
 			capacidad = nueva_capacidad;
 		}

 		size_t longitud_parte = posicion - inicio; /* Bytes del campo, incluso si el campo está vacío. */
 		partes[contador] = sistema_memoria_reservar(longitud_parte + 1,error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(partes[contador] == NULL)
 		{
 			liberarSplit(partes, (int)contador,error_o_datos_a_retornar,  nivel_de_profundidad);
 			return NULL;
 		}
 		memcpy(partes[contador], texto + inicio, longitud_parte);
 		partes[contador][longitud_parte] = '\0';
 		contador++;

 		if(posicion == longitud_texto) break;
 		posicion += longitud_separador;
 		inicio = posicion;
 	}

 	partes[contador] = NULL;
 	*cantidad = (int)contador;
 	return partes;
 }

/*
 * Libera todas las cadenas devueltas por split() y después el vector.
 * Ejemplo: para ["a","b"] libera primero partes[0], partes[1] y el vector.
 */
 void liberarSplit(char **partes, int cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	nivel_de_profundidad++;
 	if(partes == NULL) return;
 	for(int i = 0; i < cantidad; i++) sistema_memoria_liberar(partes[i],error_o_datos_a_retornar,  nivel_de_profundidad);
 	sistema_memoria_liberar(partes,error_o_datos_a_retornar,  nivel_de_profundidad);
 }
/*
 * Une cantidad elementos con un separador y reserva la cadena resultante.
 * Ejemplo: ["rojo","azul"], 2, "|" produce "rojo|azul".
 * Los elementos NULL se tratan como cadenas vacías; el resultado se libera con
 * sistema_memoria_liberar().
 */
 char * join(char **arreglo, int cantidad, const char *carcter_separacion, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	nivel_de_profundidad++;
 	const char *separador = (carcter_separacion != NULL) ? carcter_separacion : ""; /* NULL significa sin separador. */
 	size_t longitud_total = 1;                  /* Incluye desde el inicio el byte final '\0'. */
 	size_t longitud_separador = strlen(separador); /* Bytes insertados entre elementos. */

 	if(cantidad < 0 || (cantidad > 0 && arreglo == NULL))
 	{
 		return NULL;
 	}

 	for(int i = 0; i < cantidad; i++)
 	{
 		size_t longitud_elemento = (arreglo[i] != NULL) ? strlen(arreglo[i]) : 0;
 		if(longitud_elemento > (size_t)-1 - longitud_total) return NULL;
 		longitud_total += longitud_elemento;
 		if(i > 0)
 		{
 			if(longitud_separador > (size_t)-1 - longitud_total) return NULL;
 			longitud_total += longitud_separador;
 		}
 	}

 	char *resultado = sistema_memoria_reservar(longitud_total,error_o_datos_a_retornar,  nivel_de_profundidad); /* Buffer exacto que devuelve la función. */
 	if(resultado == NULL) return NULL;

 	char *destino = resultado;
 	for(int i = 0; i < cantidad; i++)
 	{
 		const char *elemento = (arreglo[i] != NULL) ? arreglo[i] : "";
 		if(i > 0)
 		{
 			memcpy(destino, separador, longitud_separador);
 			destino += longitud_separador;
 		}

 		size_t longitud_elemento = strlen(elemento);
 		memcpy(destino, elemento, longitud_elemento);
 		destino += longitud_elemento;
 	}

 	*destino = '\0';
 	return resultado;
 }

/*
 * Construye una cadena con formato printf limitado a %s, %d y %%.
 * El resultado dinámico pertenece al llamador y se libera con
 * sistema_memoria_liberar(). Los argumentos de cadena NULL se tratan como "".
 */
 char * concatenar(const char *formato, ...)
 {
 	if(formato == NULL) return NULL;

 	size_t longitud_total = 0;
 	va_list argumentos;
 	va_start(argumentos, formato);

 	for(const char *cursor = formato; *cursor != '\0'; cursor++)
 	{
 		size_t longitud_segmento = 1;
 		if(*cursor == '%')
 		{
 			cursor++;
 			if(*cursor == '\0') goto error_formato_concatenar;

 			if(*cursor == 's')
 			{
 				const char *texto = va_arg(argumentos, const char *);
 				longitud_segmento = texto != NULL ? strlen(texto) : 0;
 			}
 			else if(*cursor == 'd')
 			{
 				char numero[sizeof(int) * CHAR_BIT + 2];
 				if(enteroATexto(va_arg(argumentos, int), numero, sizeof(numero)) == NULL)
 					goto error_formato_concatenar;
 				longitud_segmento = strlen(numero);
 			}
 			else if(*cursor != '%')
 			{
 				goto error_formato_concatenar;
 			}
 		}

 		if(longitud_segmento > (size_t)-1 - longitud_total - 1)
 			goto error_formato_concatenar;
 		longitud_total += longitud_segmento;
 	}
 	va_end(argumentos);

 	char *resultado = sistema_memoria_reservar(longitud_total + 1, NULL, 0);
 	if(resultado == NULL) return NULL;

 	char *destino = resultado;
 	va_start(argumentos, formato);
 	for(const char *cursor = formato; *cursor != '\0'; cursor++)
 	{
 		if(*cursor != '%')
 		{
 			*destino++ = *cursor;
 			continue;
 		}

 		cursor++;
 		if(*cursor == 's')
 		{
 			const char *texto = va_arg(argumentos, const char *);
 			if(texto != NULL)
 			{
 				size_t longitud_texto = strlen(texto);
 				memcpy(destino, texto, longitud_texto);
 				destino += longitud_texto;
 			}
 		}
 		else if(*cursor == 'd')
 		{
 			char numero[sizeof(int) * CHAR_BIT + 2];
 			if(enteroATexto(va_arg(argumentos, int), numero, sizeof(numero)) == NULL)
 			{
 				va_end(argumentos);
 				sistema_memoria_liberar(resultado, NULL, 0);
 				return NULL;
 			}
 			size_t longitud_numero = strlen(numero);
 			memcpy(destino, numero, longitud_numero);
 			destino += longitud_numero;
 		}
 		else
 		{
 			*destino++ = '%';
 		}
 	}
 	va_end(argumentos);
 	*destino = '\0';
 	return resultado;

error_formato_concatenar:
 	va_end(argumentos);
 	return NULL;
 }
/*
 * Convierte un int a decimal sin usar printf/snprintf.
 * Ejemplos: 42 -> "42"; -7 -> "-7"; INT_MIN también se maneja sin negarlo
 * directamente, lo que evita desbordar el entero con signo.
 * Devuelve buffer o NULL si el buffer no alcanza.
 */
 static char *enteroATexto(int valor, char *buffer, size_t capacidad)
 {
 	unsigned int magnitud; /* Valor absoluto representado sin signo. */
 	size_t longitud = 0;    /* Dígitos escritos en orden inverso. */
 	int negativo = valor < 0; /* Indica si se agrega '-' al invertir. */

 	if(buffer == NULL || capacidad < 2) return NULL;
 	magnitud = negativo ? 0u - (unsigned int)valor : (unsigned int)valor;

 	do
 	{
 		if(longitud + (size_t)negativo + 1 >= capacidad) return NULL;
 		buffer[longitud++] = (char)('0' + magnitud % 10u);
 		magnitud /= 10u;
 	} while(magnitud != 0);

 	if(negativo) buffer[longitud++] = '-';
 	buffer[longitud] = '\0';

 	for(size_t inicio = 0, fin = longitud - 1; inicio < fin; inicio++, fin--)
 	{
 		char temporal = buffer[inicio];
 		buffer[inicio] = buffer[fin];
 		buffer[fin] = temporal;
 	}

 	return buffer;
 }

/*
 * Lee un entero decimal desde un tramo no necesariamente terminado en '\0'.
 * Ejemplos: " -12 " -> éxito y -12; "12x" o un número fuera de int -> fallo.
 * Devuelve 1 si se convirtió todo el tramo; 0 si hay sintaxis inválida/rango.
 */
 static int textoAEnteroN(const char *texto, size_t longitud, int *resultado)
 {
 	size_t posicion = 0;       /* Byte actual; avanza sobre espacios, signo y dígitos. */
 	unsigned int magnitud = 0; /* Acumulador protegido antes de cada multiplicación por 10. */
 	unsigned int limite;       /* INT_MAX positivo o |INT_MIN| negativo. */
 	int negativo = 0;          /* Signo leído, inicialmente positivo. */
 	int hayDigito = 0;         /* Evita aceptar entradas vacías o solo un signo. */

 	if(texto == NULL || resultado == NULL) return 0;
 	while(posicion < longitud &&
 		(texto[posicion] == ' ' || texto[posicion] == '\t' ||
 		 texto[posicion] == '\n' || texto[posicion] == '\r' ||
 		 texto[posicion] == '\f' || texto[posicion] == '\v'))
 	{
 		posicion++;
 	}

 	if(posicion < longitud && (texto[posicion] == '-' || texto[posicion] == '+'))
 	{
 		negativo = texto[posicion] == '-';
 		posicion++;
 	}

 	limite = (unsigned int)INT_MAX + (unsigned int)negativo;
 	while(posicion < longitud && texto[posicion] >= '0' && texto[posicion] <= '9')
 	{
 		unsigned int digito = (unsigned int)(texto[posicion] - '0');
 		if(magnitud > (limite - digito) / 10u) return 0;
 		magnitud = magnitud * 10u + digito;
 		hayDigito = 1;
 		posicion++;
 	}

 	if(!hayDigito) return 0;
 	while(posicion < longitud &&
 		(texto[posicion] == ' ' || texto[posicion] == '\t' ||
 		 texto[posicion] == '\n' || texto[posicion] == '\r' ||
 		 texto[posicion] == '\f' || texto[posicion] == '\v'))
 	{
 		posicion++;
 	}
 	if(posicion != longitud) return 0;

 	if(negativo)
 	{
 		*resultado = magnitud == (unsigned int)INT_MAX + 1u
 			? INT_MIN
 			: -(int)magnitud;
 	}
 	else
 	{
 		*resultado = (int)magnitud;
 	}
 	return 1;
 }

/* Atajo para cadenas C: strlen() obtiene longitud y delega validación/rango. */
 static int textoAEntero(const char *texto, int *resultado, /* #sym:textoAEntero */ char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	return texto != NULL
 		? textoAEnteroN(texto, strlen(texto), resultado)
 		: 0;
 }

/*
 * Formateador pequeño de la consola común: admite %s, %d y %% únicamente.
 * Ejemplo: ("línea %d: %s\n", 2, "Ana") escribe "línea 2: Ana".
 * Devuelve 0 al escribir todo y -1 si el formato o backend falla.
 */
 static int sistema_consola_escribir_formato(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *formato, ...)
 {
	nivel_de_profundidad++;
 	va_list argumentos;
 	const char *cursor;
 	const char *inicio;
 	int error = 0;

 	(void)nivel_de_profundidad;
 	if(formato == NULL) return -1;

 	va_start(argumentos, formato);
 	cursor = formato;
 	inicio = formato;

 	while(*cursor != '\0' && !error)
 	{
 		char numero[sizeof(int) * CHAR_BIT + 2]; /* Espacio suficiente para signo, dígitos y '\0'. */
 		const char *texto;                        /* Siguiente argumento para una conversión %s. */

 		if(*cursor != '%')
 		{
 			cursor++;
 			continue;
 		}

 		while(inicio < cursor)
 		{
 			if(sistema_consola_escribir_caracter((unsigned char)*inicio++,error_o_datos_a_retornar,  nivel_de_profundidad) != 0)
 			{
 				error = 1;
 				break;
 			}
 		}
 		if(error) break;
 		cursor++;
 		if(*cursor == '\0')
 		{
 			error = 1;
 			break;
 		}

 		if(*cursor == 's')
 		{
 			texto = va_arg(argumentos, const char *);
 			if(texto == NULL || sistema_consola_escribir_texto(texto,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 		}
 		else if(*cursor == 'd')
 		{
 			if(enteroATexto(va_arg(argumentos, int), numero, sizeof(numero)) == NULL ||
 				sistema_consola_escribir_texto(numero,error_o_datos_a_retornar,  nivel_de_profundidad) != 0)
 			{
 				error = 1;
 			}
 		}
 		else if(*cursor == '%')
 		{
 			if(sistema_consola_escribir_caracter('%',error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 		}
 		else
 		{
 			error = 1;
 		}

 		cursor++;
 		inicio = cursor;
 	}

 	if(!error && sistema_consola_escribir_texto(inicio,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 	va_end(argumentos);
 	return error ? -1 : 0;
 }

/* Escribe un entero decimal en archivo como texto; 0 significa éxito. */
 static int sistema_archivo_escribir_entero(SistemaArchivo *archivo, int valor, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	char buffer[sizeof(int) * CHAR_BIT + 2];
 	if(enteroATexto(valor, buffer, sizeof(buffer)) == NULL) return -1;
 	return sistema_archivo_escribir_texto(archivo, buffer,error_o_datos_a_retornar,  nivel_de_profundidad);
 }

/* Escribe texto seguido de '\n'; -1 señala que falló cualquiera de los dos pasos. */
 static int escribirLineaArchivo(SistemaArchivo *archivo, const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	if(texto == NULL || sistema_archivo_escribir_texto(archivo, texto,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) return -1;
 	return sistema_archivo_escribir_caracter(archivo, '\n',error_o_datos_a_retornar,  nivel_de_profundidad);
 }


 #pragma endregion FUNCIONES OPERACIONES DE TEXTO
 
 // ============================================================================
 // FUNCIONES OPERACIONES DE TEX_BASE
 // ============================================================================
 #pragma region FUNCIONES OPERACIONES_DE_TEX_BASE
 
 /* ============================================================================
   LEER LINEA DINAMICA
   ============================================================================ */

/*
 * Implementación compartida de lectura de línea.
 * leerCaracter(contexto) oculta el origen: archivo, consola o futuro UART.
 * La cadena devuelta se reserva dinámicamente y el llamador la libera.
 * Ejemplo: bytes 'h','o','l','a','\n' -> cadena "hola".
 */
static char *leerLineaDesde(
    int (*leerCaracter)(void *contexto, char *error_o_datos_a_retornar,int nivel_de_profundidad),
    void *contexto,
    char *error_o_datos_a_retornar,int nivel_de_profundidad
)
{
    char *linea;       /* Buffer devuelto si la lectura termina con una línea válida. */
    size_t capacidad;  /* Bytes reservados; crece al duplicarse cuando la línea no cabe. */
    size_t longitud;   /* Bytes leídos, sin contar el '\0' de terminación. */
    int caracter;      /* Byte leído como int o SISTEMA_ARCHIVO_FIN_LECTURA. */


	nivel_de_profundidad++;


    /*
     * Validamos.
     */
    if (leerCaracter == NULL)
    {
        return NULL;
    }


    /*
     * Capacidad inicial.
     */
    capacidad = TAMANO_INICIAL_LINEA;


    /*
     * Reservamos memoria.
     */
    linea =
        (char *)sistema_memoria_reservar(capacidad,error_o_datos_a_retornar,  nivel_de_profundidad);


    if (linea == NULL)
    {
        return NULL;
    }


    longitud = 0;


    /*
     * Leemos carácter por carácter.
     */
    while ((caracter = leerCaracter(contexto, error_o_datos_a_retornar, nivel_de_profundidad)) != SISTEMA_ARCHIVO_FIN_LECTURA)
    {
        /*
         * Final de línea.
         */
        if (caracter == '\n')
        {
            break;
        }


        /*
         * Necesitamos más espacio.
         */
        if (longitud + 1 >= capacidad)
        {
            char *temporal;


            if (capacidad > (size_t)-1 / 2)
            {
                sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
                return NULL;
            }
            capacidad *= 2;


            temporal =
                (char *)sistema_memoria_redimensionar(
                    linea,
                    capacidad,error_o_datos_a_retornar, 
                    nivel_de_profundidad
                );


            if (temporal == NULL)
            {
                sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
                return NULL;
            }


            linea = temporal;
        }


        /*
         * Guardamos el carácter.
         */
        linea[longitud] =
            (char)caracter;


        longitud++;
    }


    /*
     * Si no leímos nada y terminó la lectura,
     * liberamos.
     */
    if (longitud == 0 &&
        caracter == SISTEMA_ARCHIVO_FIN_LECTURA)
    {
        sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
        return NULL;
    }


    /*
     * Terminador.
     */
    linea[longitud] = '\0';


    return linea;
}

static int leerCaracterArchivo(void *contexto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	/* El contexto es SistemaArchivo*; el backend traduce su EOF al valor común. */
	return sistema_archivo_leer_caracter((SistemaArchivo *)contexto,error_o_datos_a_retornar,  nivel_de_profundidad);
}

static int leerCaracterConsola(void *contexto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	/* La consola no necesita objeto de contexto en la API actual. */
	(void)contexto;
	return sistema_consola_leer_caracter(error_o_datos_a_retornar, nivel_de_profundidad);
}

/* Adaptador público interno: lee una línea desde un archivo ya abierto. */
static char *leerLineaDinamica(SistemaArchivo *flujo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	nivel_de_profundidad++;
	(void)nivel_de_profundidad;
	if(flujo == NULL) return NULL;
	return leerLineaDesde(leerCaracterArchivo, flujo, error_o_datos_a_retornar, nivel_de_profundidad);
}

/* Adaptador interno: misma lógica de línea, pero leyendo desde la consola. */
static char *leerLineaConsola(char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	nivel_de_profundidad++;
	(void)nivel_de_profundidad;
	return leerLineaDesde(leerCaracterConsola, NULL, error_o_datos_a_retornar, nivel_de_profundidad);
}

/*
 * Sustituye una columna (numeración desde 1) de una fila delimitada por comas.
 * Ejemplo: ("Ana,25,QA", 2, "30") -> "Ana,30,QA".
 * Devuelve una nueva cadena reservada o NULL si la fila/columna no es válida.
 */
char * modificarColumna(const char * lineaOriginal,
 	int columnaTarget,
 	const char * nuevoValor,
 		char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	nivel_de_profundidad++;
 	if(lineaOriginal == NULL || nuevoValor == NULL || columnaTarget < 1) return NULL;

 	int cantidad = 0; /* Columnas encontradas en la fila, por ejemplo 3. */
 	char **partes = split(lineaOriginal, ",", &cantidad,error_o_datos_a_retornar,  nivel_de_profundidad); /* Campos editables. */
 	if(partes == NULL) return NULL;
 	if(columnaTarget > cantidad)
 	{
 		liberarSplit(partes, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 		return NULL;
 	}

 	size_t longitud = strlen(nuevoValor) + 1; /* Incluye el '\0' que memcpy también copiará. */
 	sistema_memoria_liberar(partes[columnaTarget - 1],error_o_datos_a_retornar,  nivel_de_profundidad);
 	partes[columnaTarget - 1] = sistema_memoria_reservar(longitud,error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(partes[columnaTarget - 1] == NULL)
 	{
 		liberarSplit(partes, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 		return NULL;
 	}
 	memcpy(partes[columnaTarget - 1], nuevoValor, longitud);

 	char *resultado = join(partes, cantidad, ",",error_o_datos_a_retornar,  nivel_de_profundidad); /* Reconstruye la fila CSV. */
 	liberarSplit(partes, cantidad,error_o_datos_a_retornar,  nivel_de_profundidad);
 	return resultado;
 }

/*
 * Sustituye un archivo mediante temporal y respaldo para evitar editarlo in situ.
 * Flujo de ejemplo: datos.txt -> temp_qu1ron.bak; temp.txt -> datos.txt;
 * finalmente elimina el respaldo. Devuelve 0 si completa todos los pasos.
 */
static int reemplazarArchivoTemporal(const char *ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	const char *temporal = RUTA_ARCHIVO_TEMPORAL; /* Archivo nuevo construido aparte. */
	const char *respaldo = RUTA_ARCHIVO_RESPALDO; /* Copia temporal del archivo original. */
	SistemaArchivo *existente;                    /* Permite comprobar si ya existe el respaldo. */

	if(ruta == NULL || strcmp(ruta, temporal) == 0 || strcmp(ruta, respaldo) == 0) return -1;

	existente = sistema_archivo_abrir(respaldo, "r",error_o_datos_a_retornar,  nivel_de_profundidad);
	if(existente != NULL)
	{
		sistema_archivo_cerrar(existente,error_o_datos_a_retornar,  nivel_de_profundidad);
		return -1;
	}

	if(sistema_archivo_renombrar(ruta, respaldo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) return -1;
	if(sistema_archivo_renombrar(temporal, ruta,error_o_datos_a_retornar,  nivel_de_profundidad) != 0)
	{
		sistema_archivo_renombrar(respaldo, ruta,error_o_datos_a_retornar,  nivel_de_profundidad);
		return -1;
	}

	return sistema_archivo_eliminar(respaldo,error_o_datos_a_retornar,  nivel_de_profundidad);
}

/*
 * Núcleo del CRUD por línea; operacion: 0=reemplazar, 1=eliminar,
 * 2=vaciar y 3=modificar columna. Reescribe el archivo a un temporal y luego
 * lo sustituye. Devuelve 0=éxito, -1=error y -2=línea solicitada inexistente.
 */
static int aplicarOperacionLinea(
	const char *ruta,
	int numeroLinea,
	int operacion,
	const char *contenido,
	int columna,
	char *error_o_datos_a_retornar,int nivel_de_profundidad
)
{
	nivel_de_profundidad++;
	(void)nivel_de_profundidad;
	SistemaArchivo *archivo;          /* Original abierto para lectura. */
	SistemaArchivo *temporal;         /* Archivo de salida con cambios aplicados. */
	char *linea;                      /* Línea actual; se libera en cada iteración. */
	int encontrada = 0;               /* Indica si numeroLinea apareció en el archivo. */
	int error = 0;                    /* Acumula fallo de lectura, escritura o cierre. */
	int actual = 1;                   /* Contador de línea, empezando por 1. */

	if(ruta == NULL || numeroLinea < 1 || (operacion == 0 && contenido == NULL) ||
		(operacion == 3 && (contenido == NULL || columna < 1)) ||
		strcmp(ruta, RUTA_ARCHIVO_TEMPORAL) == 0 ||
		strcmp(ruta, RUTA_ARCHIVO_RESPALDO) == 0)
	{
		return -1;
	}

	archivo = sistema_archivo_abrir(ruta, "r",error_o_datos_a_retornar,  nivel_de_profundidad);
	if(archivo == NULL) return -1;

	/* Evita truncar un temporal ajeno que ya existiera antes de esta operación. */
	SistemaArchivo *temporalExistente = sistema_archivo_abrir(RUTA_ARCHIVO_TEMPORAL, "r",error_o_datos_a_retornar,  nivel_de_profundidad);
	if(temporalExistente != NULL)
	{
		sistema_archivo_cerrar(temporalExistente,error_o_datos_a_retornar,  nivel_de_profundidad);
		sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
		return -1;
	}

	temporal = sistema_archivo_abrir(RUTA_ARCHIVO_TEMPORAL, "w",error_o_datos_a_retornar,  nivel_de_profundidad);
	if(temporal == NULL)
	{
		sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
		return -1;
	}

	while((linea = leerLineaDinamica(archivo,error_o_datos_a_retornar,  nivel_de_profundidad)) != NULL)
	{
		if(actual == numeroLinea)
		{
			encontrada = 1;
			if(operacion == 0)
			{
				if(escribirLineaArchivo(temporal, contenido,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
			}
			else if(operacion == 2)
			{
				if(sistema_archivo_escribir_caracter(temporal, '\n',error_o_datos_a_retornar,  nivel_de_profundidad) < 0) error = 1;
			}
			else if(operacion == 3)
			{
							/* La copia modificada se libera tras escribirla o ante error. */
							char * modificada = modificarColumna(linea, columna, contenido,error_o_datos_a_retornar,  nivel_de_profundidad);
				if(modificada == NULL || escribirLineaArchivo(temporal, modificada,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
				sistema_memoria_liberar(modificada,error_o_datos_a_retornar,  0);
			}
		}
		else if(escribirLineaArchivo(temporal, linea,error_o_datos_a_retornar,  nivel_de_profundidad) != 0)
		{
			error = 1;
		}

		sistema_memoria_liberar(linea,error_o_datos_a_retornar,  0);
		actual++;
	}

	if(sistema_archivo_hay_error(archivo,error_o_datos_a_retornar,  nivel_de_profundidad)) error = 1;
	if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
	if(sistema_archivo_cerrar(temporal,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;

	if(error || !encontrada)
	{
		sistema_archivo_eliminar(RUTA_ARCHIVO_TEMPORAL,error_o_datos_a_retornar,  nivel_de_profundidad);
		return error ? -1 : -2;
	}

	if(reemplazarArchivoTemporal(ruta,error_o_datos_a_retornar,  nivel_de_profundidad) != 0)
	{
		sistema_archivo_eliminar(RUTA_ARCHIVO_TEMPORAL,error_o_datos_a_retornar,  nivel_de_profundidad);
		return -1;
	}
	return 0;
}

/*
 * Muestra todas las líneas de ruta numeradas desde 1.
 * Ejemplo: "Ana\nLuis\n" se presenta como "1: Ana" y "2: Luis".
 * Retorna un resultado estructurado; ruta no se modifica.
 */
 char * leerArchivo(const char * ruta,
 	char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	nivel_de_profundidad++;
 	if(ruta == NULL)
 	{
 		return crearResultado(-1, "ruta_invalida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "r",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int numeroLinea = 1; /* Número que se muestra junto a cada línea. */
 	char *linea;          /* Línea dinámica leída; se libera inmediatamente tras mostrarla. */
 	sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "\n--- CONTENIDO DE [%s] ---\n", ruta);
 	while((linea = leerLineaDinamica(archivo,error_o_datos_a_retornar,  nivel_de_profundidad)) != NULL)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "%d: %s\n", numeroLinea++, linea);
 		sistema_memoria_liberar(linea,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}
 	int errorLectura = sistema_archivo_hay_error(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
 	int errorCierre = sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
 	sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "-----------------------------------\n");
 	if(errorLectura || errorCierre != 0)
 	{
 		return crearResultado(-1, "error_al_leer_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}
 	return crearResultado(1, "lectura_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Agrega nuevaLinea al final del archivo y termina el registro con '\n'.
 * Ejemplo: escribirLinea("datos.txt","Ana,25,QA", error_o_datos_a_retornar) añade "Ana,25,QA\n".
 */
 char * escribirLinea(const char * ruta,
 	const char * nuevaLinea,
 		char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevaLinea == NULL ||
		strcmp(ruta, RUTA_ARCHIVO_TEMPORAL) == 0 ||
		strcmp(ruta, RUTA_ARCHIVO_RESPALDO) == 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "a",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int errorEscritura = escribirLineaArchivo(archivo, nuevaLinea,error_o_datos_a_retornar,  nivel_de_profundidad) != 0; /* Se confirma también el cierre. */
 	if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) errorEscritura = 1;
 	if(errorEscritura)
 	{
 		return crearResultado(-1, "error_al_escribir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	return crearResultado(1, "escritura_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }

 /* Reemplaza por completo la línea idLinea; el ID comienza en 1. */
 char * editarLinea(const char * ruta,
 	int idLinea,
 	const char * nuevoTexto,
 		char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevoTexto == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 0, nuevoTexto, 0,error_o_datos_a_retornar,  nivel_de_profundidad); /* 0=reemplazar. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	return crearResultado(1, "edicion_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }

 /*
 * Cambia una columna de una línea delimitada por comas.
 * Ejemplo: línea 1, columna 2, "30" convierte "Ana,25,QA" en "Ana,30,QA".
 */
 char * editarColumna(const char * ruta,
 	int idLinea,
 	int idColumna,
 	const char * nuevoValor,
 		char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevoValor == NULL || idLinea <= 0 || idColumna <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 3, nuevoValor, idColumna,error_o_datos_a_retornar,  nivel_de_profundidad); /* 3=columna. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_o_columna_no_encontrada", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	return crearResultado(1, "columna_editada_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }

 /* Elimina el registro idLinea; las líneas posteriores cambian de número. */
 char * eliminarLinea(const char * ruta,
 	int idLinea,
 	char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 1, NULL, 0,error_o_datos_a_retornar,  nivel_de_profundidad); /* 1=omitir línea. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	return crearResultado(1, "eliminacion_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }

 /* Conserva la posición idLinea, pero escribe una línea vacía en su lugar. */
 char * vaciarLinea(const char * ruta,
 	int idLinea,
 	char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 2, NULL, 0,error_o_datos_a_retornar,  nivel_de_profundidad); /* 2=línea vacía. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	return crearResultado(1, "vaciado_ok", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }

 #pragma endregion FUNCIONES OPERACIONES_DE_TEX_BASE
 
 // ============================================================================
 // FUNCIONES OPERACIONES DE MENSAJERÍA
 // ============================================================================

 #pragma region FUNCIONES_OPERACIONES_DE_MENSAJERIA

 /*
 * Comprueba si alguno de los tres archivos de mensajes contiene al menos un byte.
 * Devuelve código 1 si encuentra contenido, 0 si todos están vacíos/no existen,
 * o -1 si un archivo abierto da error de lectura o cierre.
 */
 char * checar_si_hay_mensajes_no_leido(char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	/* Orden de consulta: todos, contactos y mensajes dirigidos al primero. */
 	const char * rutas[] = {
 		RUTA_MENSAJES_TODOS,
 		RUTA_MENSAJES_CONTACTOS,
 		RUTA_MENSAJES_PRIMERO
 	};
 	for(size_t i = 0; i < sizeof(rutas) / sizeof(rutas[0]); i++)
 	{
 		SistemaArchivo *archivo = sistema_archivo_abrir(rutas[i], "r",error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(archivo == NULL)
 		{
 			continue;
 		}
 		int caracter = sistema_archivo_leer_caracter(archivo,error_o_datos_a_retornar,  nivel_de_profundidad); /* Un byte basta para saber si hay contenido. */
 		int errorLectura = sistema_archivo_hay_error(archivo,error_o_datos_a_retornar,  nivel_de_profundidad); /* Distingue EOF de un fallo real. */
 		int errorCierre = sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);     /* El cierre también puede fallar. */
 		if(errorLectura || errorCierre != 0)
 		{
 			return crearResultado(-1, "error_al_consultar_mensajes", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 		if(caracter != SISTEMA_ARCHIVO_FIN_LECTURA)
 		{
 			return crearResultado(1, "hay_mensajes_no_leidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 	}
 	return crearResultado(0, "no_hay_mensajes", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Ejecuta una secuencia CRUD sobre un archivo temporal y lo elimina al final.
 * Ejemplo: crea dos filas, cambia la edad de Luis, elimina la fila de Marta,
 * muestra el archivo restante y limpia el archivo de prueba.
 */
 char * ejecutarEjemplosPrueba(char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	const char *rutaPrueba = RUTA_ARCHIVO_PRUEBAS;
 	SistemaArchivo *existente = sistema_archivo_abrir(rutaPrueba, "r",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(existente != NULL)
 	{
 		sistema_archivo_cerrar(existente,error_o_datos_a_retornar,  nivel_de_profundidad);
 		return crearResultado(-1, "archivo_de_prueba_ya_existe", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}

 	int exito = 1; /* Se pone en 0 al primer fallo y entonces se omiten pruebas restantes. */
 	char *lineaModificada = modificarColumna("Ana,25,Programador", 2, "30",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(lineaModificada == NULL || strcmp(lineaModificada, "Ana,30,Programador") != 0)
 	{
 		exito = 0;
 	}
 	sistema_memoria_liberar(lineaModificada,error_o_datos_a_retornar,  nivel_de_profundidad);

 	char *resultadoOperacion = NULL; /* Cada operación devuelve un resultado que se libera aquí. */
 	if(exito)
 	{
 		resultadoOperacion = escribirLinea(rutaPrueba, "Luis,10,Desarrollador",error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, resultadoOperacion)) exito = 0;
 		sistema_memoria_liberar(resultadoOperacion,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}

 	if(exito)
 	{
 		resultadoOperacion = escribirLinea(rutaPrueba, "Marta,20,QA",error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, resultadoOperacion)) exito = 0;
 		sistema_memoria_liberar(resultadoOperacion,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}

 	if(exito)
 	{
 		resultadoOperacion = editarColumna(rutaPrueba, 1, 2, "15",error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, resultadoOperacion)) exito = 0;
 		sistema_memoria_liberar(resultadoOperacion,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}

 	if(exito)
 	{
 		resultadoOperacion = eliminarLinea(rutaPrueba, 2,error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, resultadoOperacion)) exito = 0;
 		sistema_memoria_liberar(resultadoOperacion,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}

 	if(exito)
 	{
 		sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "\n--- ARCHIVO DE PRUEBA RESULTANTE ---\n");
 		resultadoOperacion = leerArchivo(rutaPrueba,error_o_datos_a_retornar,  nivel_de_profundidad);
 		if(resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, resultadoOperacion)) exito = 0;
 		sistema_memoria_liberar(resultadoOperacion,error_o_datos_a_retornar,  nivel_de_profundidad);
 	}

 	{
 		SistemaArchivo *archivoPrueba = sistema_archivo_abrir(rutaPrueba, "r",error_o_datos_a_retornar,  nivel_de_profundidad); /* NULL si el temporal no existe. */
 		int errorLimpieza = 0; /* Se activa ante fallo al cerrar o borrar el archivo. */

 		if(archivoPrueba != NULL)
 		{
 			if(sistema_archivo_cerrar(archivoPrueba,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) errorLimpieza = 1;
 			if(sistema_archivo_eliminar(rutaPrueba,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) errorLimpieza = 1;
 		}
 		else if(exito)
 		{
 			errorLimpieza = 1;
 		}

 		if(errorLimpieza)
 		{
 			return crearResultado(-1, "no_se_pudo_limpiar_archivo_de_prueba", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 		}
 	}
 	return crearResultado(exito ? 1 : -1, exito ? "pruebas_ok" : "pruebas_fallaron", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Agrega una línea al archivo de mensajes para todos.
 * Ejemplo: "Reunión a las 10" se guarda seguido de un salto de línea.
 */
 char * mandar_mensje_a_todos(const char * mensaje,
 	char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje == NULL || strlen(mensaje) == 0)
 	{
 		return crearResultado(-1, "mensaje_invalido", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_TODOS, "a",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int error = escribirLineaArchivo(archivo, mensaje,error_o_datos_a_retornar,  nivel_de_profundidad) != 0;
 	if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Guarda un mensaje junto con su ID opcional y la lista de contactos.
 * Ejemplo: id=7, contactos="Ana,Luis", mensaje="Hola" produce
 * "[7] Ana,Luis -> Hola".
 */
 char * mandar_mensje_a_contacto(const char * mensaje,
 	const char * contactos,
 		int id_opcional,
 		char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje == NULL || strlen(mensaje) == 0)
 	{
 		return crearResultado(-1, "mensaje_invalido", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(contactos == NULL || strlen(contactos) == 0)
 	{
 		return crearResultado(-1, "contactos_invalidos", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_CONTACTOS, "a",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
	int error =
		sistema_archivo_escribir_texto(archivo, "[",error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_entero(archivo, id_opcional,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_texto(archivo, "] ",error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_texto(archivo, contactos,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_texto(archivo, " -> ",error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_texto(archivo, mensaje,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
		sistema_archivo_escribir_caracter(archivo, '\n',error_o_datos_a_retornar,  nivel_de_profundidad) != 0;
 	if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
/*
 * Registra pregunta, aviso de aceptación y respuesta final en una sola línea.
 * Ejemplo: "¿Quién?", "Yo voy", "Gracias" ->
 * "¿Quién? | Yo voy | Gracias".
 */
 char * mandar_mensje_al_primero_que_responda(const char * mensaje_pregunta,
 	const char * mensaje_de_que_ya_alguien_lo_acepto,
 		const char * menaje_respuesta_al_quien_lo_logro,
 			char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje_pregunta == NULL || strlen(mensaje_pregunta) == 0)
 	{
 		return crearResultado(-1, "mensaje_pregunta_invalido", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(mensaje_de_que_ya_alguien_lo_acepto == NULL || strlen(mensaje_de_que_ya_alguien_lo_acepto) == 0)
 	{
 		return crearResultado(-1, "mensaje_aceptado_invalido", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	if(menaje_respuesta_al_quien_lo_logro == NULL || strlen(menaje_respuesta_al_quien_lo_logro) == 0)
 	{
 		return crearResultado(-1, "mensaje_respuesta_invalido", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_PRIMERO, "a",error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	}
 	int error =
 		sistema_archivo_escribir_texto(archivo, mensaje_pregunta,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
 		sistema_archivo_escribir_texto(archivo, " | ",error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
 		sistema_archivo_escribir_texto(archivo, mensaje_de_que_ya_alguien_lo_acepto,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
 		sistema_archivo_escribir_texto(archivo, " | ",error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
 		sistema_archivo_escribir_texto(archivo, menaje_respuesta_al_quien_lo_logro,error_o_datos_a_retornar,  nivel_de_profundidad) != 0 ||
 		sistema_archivo_escribir_caracter(archivo, '\n',error_o_datos_a_retornar,  nivel_de_profundidad) != 0;
 	if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
 }
 #pragma endregion FUNCIONES_OPERACIONES_DE_MENSAJERIA

 /* ============================================================================
    MEMORIA
    ============================================================================ */
#pragma region FUNCIONES_MEMORIA

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)

/*
 * Backend de escritorio: delega en el heap estándar.
 * Ejemplo de propiedad: reservar(20) devuelve un bloque de 20 bytes que debe
 * terminar en sistema_memoria_liberar() o redimensionarse con la misma capa.
 */
/* Solicita cantidad bytes; NULL indica que no se pudo reservar. */
 static void * sistema_memoria_reservar(size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	return malloc(cantidad);
 }
/* Cambia el tamaño de un bloque existente; conserva su contenido si tiene éxito. */
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	return realloc(memoria, cantidad);
 }
/* Libera un bloque obtenido con reservar/redimensionar; NULL es seguro en free(). */
 static void sistema_memoria_liberar(void * memoria, /* #sym:sistema_memoria_liberar */ char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	free(memoria);
 }

#elif defined(PLATAFORMA_SEMICONDUCTOR)

/*
 * Sustituir estas operaciones con el administrador fijo de memoria del PIC16F.
 * Hasta entonces, las reservas fallan explícitamente en esta plataforma.
 */
/* Stub deliberadamente fallido: aún no existe un pool físico configurado. */
static void * sistema_memoria_reservar(size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)cantidad;
	(void)nivel_de_profundidad;
	return NULL;
}

/* Stub de redimensionamiento; devuelve NULL hasta integrar el allocator embebido. */
static void * sistema_memoria_redimensionar(void *memoria, size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)memoria;
	(void)cantidad;
	(void)nivel_de_profundidad;
	return NULL;
}

/* Stub de liberación: no hay bloque asignado mientras reservar() falle. */
static void sistema_memoria_liberar(void *memoria, /* #sym:sistema_memoria_liberar */ char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)memoria;
	(void)nivel_de_profundidad;
}

#endif

#pragma endregion FUNCIONES_MEMORIA

 /* ============================================================================
    ARCHIVOS
    ============================================================================ */
#pragma region FUNCIONES_ARCHIVOS


#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
 /*
  * Implementación escritorio del tipo opaco. FILE* queda confinado a esta capa;
  * el código común solo conserva y entrega SistemaArchivo*.
  */
  struct SistemaArchivo
 {
  	FILE *flujo;
	int caracterPendiente;
	int tieneCaracterPendiente;
  };

/*
 * Abre una ruta ("datos.txt") con modo estándar ("r", "a", "w").
 * Devuelve un manejador opaco o NULL; si falla la reserva del manejador,
 * cierra el FILE* para no dejar recursos abiertos.
 */
 static SistemaArchivo *sistema_archivo_abrir(const char *ruta, const char *modo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
 	if(ruta == NULL || modo == NULL) return NULL;
 	FILE *flujo = fopen(ruta, modo);
 	if(flujo == NULL) return NULL;

 	SistemaArchivo *archivo = sistema_memoria_reservar(sizeof(*archivo),error_o_datos_a_retornar,  nivel_de_profundidad);
 	if(archivo == NULL)
 	{
 		fclose(flujo);
 		return NULL;
 	}
 	archivo->flujo = flujo;
	archivo->caracterPendiente = 0;
	archivo->tieneCaracterPendiente = 0;
 	return archivo;
 }

/* Cierra el flujo y libera el objeto; devuelve 0 si fclose tuvo éxito. */
static int sistema_archivo_cerrar(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	if(archivo == NULL) return -1;

 	int resultado = fclose(archivo->flujo);
 	sistema_memoria_liberar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
 	return resultado;
}

/* Elimina una ruta; por ejemplo, borra el temporal luego de una edición. */
static int sistema_archivo_eliminar(const char *ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (ruta == NULL) ? -1 : remove(ruta);
}

/* Cambia el nombre de origen a destino; 0 significa que el backend lo logró. */
static int sistema_archivo_renombrar(const char *origen, const char *destino, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (origen == NULL || destino == NULL) ? -1 : rename(origen, destino);
}

/*
 * Lee un byte como int o devuelve SISTEMA_ARCHIVO_FIN_LECTURA al alcanzar EOF.
 * Esta conversión evita exponer el valor EOF al código común.
 */
static int sistema_archivo_leer_caracter(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(archivo == NULL || archivo->flujo == NULL) return SISTEMA_ARCHIVO_FIN_LECTURA;

	int caracter;
	if(archivo->tieneCaracterPendiente)
	{
		caracter = archivo->caracterPendiente;
		archivo->tieneCaracterPendiente = 0;
	}
	else
	{
		caracter = fgetc(archivo->flujo);
	}

	if(caracter == EOF) return SISTEMA_ARCHIVO_FIN_LECTURA;
	if(caracter == '\r')
	{
		int siguiente = fgetc(archivo->flujo);
		if(siguiente != '\n' && siguiente != EOF)
		{
			archivo->caracterPendiente = siguiente;
			archivo->tieneCaracterPendiente = 1;
		}
		return '\n';
	}
	return caracter;
}

/* Escribe un byte; 0=éxito, -1=flujo inválido o fallo de escritura. */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(archivo == NULL || archivo->flujo == NULL) return -1;
	return (fputc(caracter, archivo->flujo) == EOF) ? -1 : 0;
}

/* Consulta el indicador de error del flujo; 0=sin error, distinto de 0=error. */
static int sistema_archivo_hay_error(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (archivo == NULL || archivo->flujo == NULL) ? 1 : ferror(archivo->flujo);
}

/* Escribe todos los bytes de una cadena (sin su '\0'); 0=éxito, -1=fallo. */
static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(archivo == NULL || archivo->flujo == NULL || texto == NULL) return -1;
	size_t longitud = strlen(texto);
	return fwrite(texto, 1, longitud, archivo->flujo) == longitud ? 0 : -1;
}


#elif defined(PLATAFORMA_SEMICONDUCTOR)

/*
 * Stub embebido: reemplazar cada operación cuando se elija el controlador
 * (Flash, EEPROM, SD u otro). Los errores son explícitos, nunca éxito simulado.
 */
/* Todavía no puede asociar una ruta lógica con un dispositivo físico. */
static SistemaArchivo *sistema_archivo_abrir(const char *ruta, const char *modo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)ruta;
	(void)modo;
	(void)nivel_de_profundidad;
	return NULL;
}

/* No hay manejador real que cerrar en el stub actual. */
static int sistema_archivo_cerrar(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return -1;
}

/* El borrado requiere la política y soporte del medio que se seleccione. */
static int sistema_archivo_eliminar(const char *ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)ruta;
	(void)nivel_de_profundidad;
	return -1;
}

/* El renombrado debe implementarse según lo que permita el almacenamiento. */
static int sistema_archivo_renombrar(const char *origen, const char *destino, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)origen;
	(void)destino;
	(void)nivel_de_profundidad;
	return -1;
}

/* Indica fin de entrada porque el stub no está conectado a un lector físico. */
static int sistema_archivo_leer_caracter(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* El stub no acepta escrituras hasta contar con backend real. */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)caracter;
	(void)nivel_de_profundidad;
	return -1;
}

/* El stub no persiste texto hasta contar con backend real. */
static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)texto;
	(void)nivel_de_profundidad;
	return -1;
}

/* Mientras el backend no exista, toda consulta de error debe ser conservadora. */
static int sistema_archivo_hay_error(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return 1;
}
#endif

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
/* Backends de consola de escritorio; aquí, y solo aquí, se usa stdio. */
static int sistema_consola_leer_caracter(char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	static int caracterPendiente;
	static int tieneCaracterPendiente;
	int caracter;

	if(tieneCaracterPendiente)
	{
		caracter = caracterPendiente;
		tieneCaracterPendiente = 0;
	}
	else
	{
		caracter = getchar();
	}

	if(caracter == EOF) return SISTEMA_ARCHIVO_FIN_LECTURA;
	if(caracter == '\r')
	{
		int siguiente = getchar();
		if(siguiente != '\n' && siguiente != EOF)
		{
			caracterPendiente = siguiente;
			tieneCaracterPendiente = 1;
		}
		return '\n';
	}
	return caracter;
}

/* Escribe un carácter a salida estándar; 0=éxito y -1=fallo. */
static int sistema_consola_escribir_caracter(int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return fputc(caracter, stdout) == EOF ? -1 : 0;
}

/* Escribe una cadena completa y vacía la salida para que los prompts aparezcan. */
static int sistema_consola_escribir_texto(const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(texto == NULL || fputs(texto, stdout) == EOF || fflush(stdout) != 0) return -1;
	return 0;
}
#elif defined(PLATAFORMA_SEMICONDUCTOR)
/* Reemplazar con lectura del periférico elegido, por ejemplo UART. */
static int sistema_consola_leer_caracter(char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* Stub de salida de byte: no reporta éxito sin un periférico real. */
static int sistema_consola_escribir_caracter(int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)caracter;
	(void)nivel_de_profundidad;
	return -1;
}

/* Stub de salida de texto; lo implementará el backend de consola PIC16F. */
static int sistema_consola_escribir_texto(const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)texto;
	(void)nivel_de_profundidad;
	return -1;
}
#endif

#pragma endregion FUNCIONES_ARCHIVOS

#pragma region FUNCIONES_DE_DEPURACION

/*
 * Agrega el código de estado actual al inicio de la traza acumulada.
 *
 * Separadores según la profundidad:
 *   0 -> '|'
 *   1 -> '°'
 *   2 -> '¬'
 *
 * Ejemplo:
 *   crearResultado(-4, 2, ...) -> "¬-4"
 *   crearResultado( 2, 1, "¬-4", ..., error_o_datos_a_retornar) -> "°2¬-4"
 *   crearResultado( 0, 0, "°2¬-4", ..., error_o_datos_a_retornar) -> "|0°2¬-4"
 *
 * Los códigos >= 0 representan estados sin error fatal.
 * Los códigos < 0 representan errores.
 *
 * Devuelve una cadena nueva que pertenece al llamador.
 * Devuelve NULL si falla una conversión, el tamaño o la reserva.
 */
char *crearResultado(
    int codigo,
    const char *informacion,
    const char *resultado_anterior,
    const char *funcion_llamante,
    char *error_o_datos_a_retornar,int nivel_de_profundidad
)
{
    /*
     * Ajusta GG_NUM_SEPARADORES al número real de elementos
     * de GG_caracter_separacion.
     */
    const size_t cantidadSeparadores =
        sizeof(GG_caracter_separacion) /
        sizeof(GG_caracter_separacion[0]);

    if (
        nivel_de_profundidad < 0 ||
        (size_t)nivel_de_profundidad >= cantidadSeparadores
    )
    {
        return NULL;
    }

    const char *separador =
        GG_caracter_separacion[nivel_de_profundidad];

    if (separador == NULL)
    {
        return NULL;
    }

    /*
     * La información y el nombre de la función se conservan
     * como argumentos por compatibilidad con la interfaz actual.
     * La traza solo contiene separador, código y resultado anterior.
     */
    (void)informacion;
    (void)funcion_llamante;

    const char *anterior =
        (resultado_anterior != NULL)
            ? resultado_anterior
            : "";

    char codigoTexto[sizeof(int) * CHAR_BIT + 2];

    if (
        enteroATexto(
            codigo,
            codigoTexto,
            sizeof(codigoTexto)
        ) == NULL
    )
    {
        return NULL;
    }

    /*
     * Calcula el espacio necesario para:
     * separador + código + traza anterior + '\0'.
     */
    const size_t longitudSeparador = strlen(separador);
    const size_t longitudCodigo = strlen(codigoTexto);
    const size_t longitudAnterior = strlen(anterior);

    size_t longitudTotal = 1;

    if (
        longitudSeparador > (size_t)-1 - longitudTotal
    )
    {
        return NULL;
    }
    longitudTotal += longitudSeparador;

    if (
        longitudCodigo > (size_t)-1 - longitudTotal
    )
    {
        return NULL;
    }
    longitudTotal += longitudCodigo;

    if (
        longitudAnterior > (size_t)-1 - longitudTotal
    )
    {
        return NULL;
    }
    longitudTotal += longitudAnterior;

    /* Reserva una cadena independiente. */
    char *resultado = sistema_memoria_reservar(longitudTotal,error_o_datos_a_retornar,  nivel_de_profundidad);

    if (resultado == NULL)
    {
        return NULL;
    }

    /* Construye: separador + código actual + traza anterior. */
    char *destino = resultado;

    memcpy(destino, separador, longitudSeparador);
    destino += longitudSeparador;

    memcpy(destino, codigoTexto, longitudCodigo);
    destino += longitudCodigo;

    memcpy(destino, anterior, longitudAnterior);
    destino += longitudAnterior;

    *destino = '\0';

    return resultado;
}




/*
 * Imprime un mensaje de depuración con formato printf.
 * En PIC16F, el mensaje se limita al tamaño del buffer.
 */
void imprimirMensaje_para_depurar(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *format, ...)
{
	(void)nivel_de_profundidad;
	if(format == NULL) return;

	va_list args;
	va_start(args, format);

#ifdef PIC16F
	char buffer[80];
	int longitud = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	if(longitud < 0)
	{
		printf("[error al formatear mensaje de depuracion]\n");
		return;
	}
	printf("%s", buffer);
#elif defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
	vprintf(format, args);
	va_end(args);
#else
	va_end(args);
	sistema_consola_escribir_texto(
		"[formato de depuracion no disponible en este backend]\n",error_o_datos_a_retornar, 
		nivel_de_profundidad
	);
#endif
}

/*
 * Imprime un arreglo de cadenas. Si total <= 0, contenido debe terminar
 * en NULL; de lo contrario se imprimen exactamente total elementos.
 */
void imprimirMensaje_para_depurar_arreglo(
	char *error_o_datos_a_retornar,int nivel_de_profundidad,
	char **contenido,
	const char *texto,
	int total
)
{
	(void)nivel_de_profundidad;
	const char *prefijo = (texto != NULL) ? texto : "celda";

	if(contenido == NULL)
	{
#if defined(PIC16F) || defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
		printf("%s[0]: (null)\n", prefijo);
#else
		sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "%s[0]: (null)\n", prefijo);
#endif
		return;
	}

	if(total > 0)
	{
		for(int i = 0; i < total; ++i)
		{
			const char *valor =
				(contenido[i] != NULL) ? contenido[i] : "(null)";

#ifdef PIC16F
			char buffer[120];
			int longitud = snprintf(
				buffer,
				sizeof(buffer),
				"\n%s[%d]: %s",
				prefijo,
				i,
				valor
			);
			if(longitud >= 0)
			{
				printf("%s", buffer);
			}
			else
			{
				printf("[error al formatear elemento de depuracion]\n");
			}
#elif defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
			printf("\n%s[%d]: %s", prefijo, i, valor);
#else
			sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, 
				"\n%s[%d]: %s",
				prefijo,
				i,
				valor
			);
#endif
		}
		return;
	}

	int i = 0;
	while(contenido[i] != NULL)
	{
		const char *valor = contenido[i];

#ifdef PIC16F
		char buffer[120];
		int longitud = snprintf(
			buffer,
			sizeof(buffer),
			"%s[%d]: %s\n",
			prefijo,
			i,
			valor
		);
		if(longitud >= 0)
		{
			printf("%s", buffer);
		}
		else
		{
			printf("[error al formatear elemento de depuracion]\n");
		}
#elif defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
		printf("%s[%d]: %s\n", prefijo, i, valor);
#else
		sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, 
			"%s[%d]: %s\n",
			prefijo,
			i,
			valor
		);
#endif
		++i;
	}

	if(i == 0)
	{
#if defined(PIC16F) || defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
		printf("%s[0]: (null)\n", prefijo);
#else
		sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "%s[0]: (null)\n", prefijo);
#endif
	}
}

/*
 * Menú de pruebas manuales para primitivas y funciones auxiliares.
 * Las pruebas de archivos se limitan a nombres temporales de esta prueba.
 */
static int submenu_pruebas_auxiliares(char *error_o_datos_a_retornar,int nivel_de_profundidad,int opcion_seleccionada)
{
	nivel_de_profundidad++;
	int opcion = opcion_seleccionada;

	for(;;)
	{
		if(opcion == 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== FUNCIONES AUXILIARES ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Probar split, liberarSplit y join\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Probar conversiones y lectura de resultados\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Probar crearResultado\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Probar reserva, redimensionamiento y liberación\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Probar primitivas de archivos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Probar consola, configuración UTF-8 y depuración\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Volver al menú de pruebas\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");

			char *entrada = leerLineaConsola(error_o_datos_a_retornar, 1);
			if(entrada == NULL) return -1;
			int entradaValida = textoAEntero(entrada, &opcion,error_o_datos_a_retornar,  1);
			sistema_memoria_liberar(entrada,error_o_datos_a_retornar,  0);
			if(!entradaValida) opcion = 0;
		}

		switch(opcion)
		{
			case 1:
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Texto para dividir: ");
				char *texto = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Separador: ");
				char *separador = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(texto == NULL || separador == NULL)
				{
					sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(separador,error_o_datos_a_retornar,  0);
					return -1;
				}

				int cantidad = 0;
				char **partes = split(texto, separador, &cantidad,error_o_datos_a_retornar,  1);
				if(partes == NULL)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "split fallo.\n");
					sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(separador,error_o_datos_a_retornar,  0);
					break;
				}

				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Elementos: %d\n", cantidad);
				imprimirMensaje_para_depurar_arreglo(error_o_datos_a_retornar, nivel_de_profundidad, partes, "parte", cantidad);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\nSeparador para unir: ");
				char *separadorUnion = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(separadorUnion == NULL)
				{
					liberarSplit(partes, cantidad,error_o_datos_a_retornar,  1);
					sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(separador,error_o_datos_a_retornar,  0);
					return -1;
				}

				char *unido = join(partes, cantidad, separadorUnion,error_o_datos_a_retornar,  1);
				if(unido == NULL)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "join fallo.\n");
				}
				else
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Resultado de join: %s\n", unido);
				}
				sistema_memoria_liberar(unido,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(separadorUnion,error_o_datos_a_retornar,  0);
				liberarSplit(partes, cantidad,error_o_datos_a_retornar,  1);
				sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(separador,error_o_datos_a_retornar,  0);
				break;
			}
			case 2:
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Entero decimal para convertir: ");
				char *entradaEntero = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(entradaEntero == NULL) return -1;

				int valor = 0;
				if(!textoAEntero(entradaEntero, &valor,error_o_datos_a_retornar,  1))
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "No es un entero válido.\n");
				}
				else
				{
					char buffer[sizeof(int) * CHAR_BIT + 2];
					if(enteroATexto(valor, buffer, sizeof(buffer)) == NULL)
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "enteroATexto fallo.\n");
					}
					else
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "enteroATexto: %s\n", buffer);
					}
					int valorPorTramo = 0;
					if(textoAEnteroN(
						entradaEntero,
						strlen(entradaEntero),
						&valorPorTramo
					))
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
							"textoAEnteroN: %d\n",
							valorPorTramo
						);
					}
				}
				sistema_memoria_liberar(entradaEntero,error_o_datos_a_retornar,  0);

				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Traza para analizar: ");
				char *traza = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(traza == NULL) return -1;
				int codigo = 0;
				if(leerCodigoResultado(error_o_datos_a_retornar, nivel_de_profundidad, traza, &codigo))
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Código=%d; resultadoTieneError=%d\n",
						codigo,
						resultadoTieneError(error_o_datos_a_retornar, nivel_de_profundidad, traza)
					);
				}
				else
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"leerCodigoResultado no pudo analizar la traza.\n"
					);
				}
				sistema_memoria_liberar(traza,error_o_datos_a_retornar,  0);
				break;
			}
			case 3:
			{
				int codigo = 0;
				int profundidad = 0;
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Código entero: ");
				char *codigoTexto = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Información (si aplica): ");
				char *informacion = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Resultado anterior (vacío si ninguno): ");
				char *anterior = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Nombre de función (si aplica): ");
				char *funcion = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Nivel de profundidad: ");
				char *nivelTexto = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(codigoTexto == NULL || informacion == NULL || anterior == NULL ||
					funcion == NULL || nivelTexto == NULL)
				{
					sistema_memoria_liberar(codigoTexto,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(informacion,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(anterior,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(funcion,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(nivelTexto,error_o_datos_a_retornar,  0);
					return -1;
				}

				if(!textoAEntero(codigoTexto, &codigo,error_o_datos_a_retornar,  1) ||
					!textoAEntero(nivelTexto, &profundidad,error_o_datos_a_retornar,  1))
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Código o profundidad inválidos.\n"
					);
				}
				else
				{
					char *resultado = crearResultado(
						codigo,
						informacion,
						anterior,
						funcion,
						error_o_datos_a_retornar,
						profundidad
					);
					if(resultado == NULL)
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
							"crearResultado devolvió NULL.\n"
						);
					}
					else
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
							"Resultado creado: %s\n",
							resultado
						);
					}
					sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
				}
				sistema_memoria_liberar(codigoTexto,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(informacion,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(anterior,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(funcion,error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(nivelTexto,error_o_datos_a_retornar,  0);
				break;
			}
			case 4:
			{
				int tamano = 0;
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Tamaño inicial (>0): ");
				char *tamanoTexto = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(tamanoTexto == NULL) return -1;
				if(!textoAEntero(tamanoTexto, &tamano,error_o_datos_a_retornar,  1) || tamano <= 0)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Tamaño inválido.\n");
					sistema_memoria_liberar(tamanoTexto,error_o_datos_a_retornar,  0);
					break;
				}
				sistema_memoria_liberar(tamanoTexto,error_o_datos_a_retornar,  0);

				unsigned char *bloque = sistema_memoria_reservar((size_t)tamano,error_o_datos_a_retornar,  0);
				if(bloque == NULL)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Reserva no disponible en este backend.\n"
					);
					break;
				}

				memset(bloque, 0x5A, (size_t)tamano);
				unsigned char *redimensionado = sistema_memoria_redimensionar(
					bloque,
					(size_t)tamano + 1,error_o_datos_a_retornar, 
					0
				);
				if(redimensionado == NULL)
				{
					sistema_memoria_liberar(bloque,error_o_datos_a_retornar,  0);
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Redimensionamiento no disponible o fallido.\n"
					);
					break;
				}
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
					"Bloque redimensionado; bytes iniciales conservados=%s\n",
					(redimensionado[0] == 0x5A) ? "sí" : "no"
				);
				sistema_memoria_liberar(redimensionado,error_o_datos_a_retornar,  0);
				break;
			}
			case 5:
			{
				const char *rutaOriginal = "qu1ron_backend_test.tmp";
				const char *rutaRenombrada = "qu1ron_backend_test_renamed.tmp";
				SistemaArchivo *preexistenteOriginal =
					sistema_archivo_abrir(rutaOriginal, "r",error_o_datos_a_retornar,  0);
				SistemaArchivo *preexistenteRenombrado =
					sistema_archivo_abrir(rutaRenombrada, "r",error_o_datos_a_retornar,  0);
				if(preexistenteOriginal != NULL || preexistenteRenombrado != NULL)
				{
					if(preexistenteOriginal != NULL)
					{
						sistema_archivo_cerrar(preexistenteOriginal,error_o_datos_a_retornar,  0);
					}
					if(preexistenteRenombrado != NULL)
					{
						sistema_archivo_cerrar(preexistenteRenombrado,error_o_datos_a_retornar,  0);
					}
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Los archivos temporales de prueba ya existen; no se modificaron.\n"
					);
					break;
				}
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Texto que se escribirá: ");
				char *texto = leerLineaConsola(error_o_datos_a_retornar, 1);
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Entero que se agregará: ");
				char *enteroTexto = leerLineaConsola(error_o_datos_a_retornar, 1);
				int entero = 0;
				if(texto == NULL || enteroTexto == NULL ||
					!textoAEntero(enteroTexto, &entero,error_o_datos_a_retornar,  1))
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
						"Entradas inválidas; no se ejecutó la prueba de archivo.\n"
					);
					sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
					sistema_memoria_liberar(enteroTexto,error_o_datos_a_retornar,  0);
					break;
				}
				sistema_memoria_liberar(enteroTexto,error_o_datos_a_retornar,  0);

				SistemaArchivo *archivo = sistema_archivo_abrir(
					rutaOriginal,
					"w",error_o_datos_a_retornar, 
					0
				);
				int archivoOriginalCreado = archivo != NULL;
				int archivoRenombradoCreado = 0;
				int error = archivo == NULL;
				if(!error)
				{
					error = sistema_archivo_escribir_texto(archivo, texto,error_o_datos_a_retornar,  0) != 0 ||
						sistema_archivo_escribir_caracter(archivo, '|',error_o_datos_a_retornar,  0) != 0 ||
						sistema_archivo_escribir_entero(archivo, entero,error_o_datos_a_retornar,  0) != 0 ||
						escribirLineaArchivo(archivo, "",error_o_datos_a_retornar,  0) != 0;
					if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  0) != 0) error = 1;
				}
				if(!error &&
					sistema_archivo_renombrar(rutaOriginal, rutaRenombrada,error_o_datos_a_retornar,  0) != 0)
				{
					error = 1;
				}
				else if(!error)
				{
					archivoOriginalCreado = 0;
					archivoRenombradoCreado = 1;
				}

				if(!error)
				{
					archivo = sistema_archivo_abrir(rutaRenombrada, "r",error_o_datos_a_retornar,  0);
					if(archivo == NULL)
					{
						error = 1;
					}
					else
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Contenido leído: ");
						int caracter;
						while((caracter = sistema_archivo_leer_caracter(archivo,error_o_datos_a_retornar,  0)) !=
							SISTEMA_ARCHIVO_FIN_LECTURA)
						{
							if(sistema_consola_escribir_caracter(caracter,error_o_datos_a_retornar,  0) != 0)
							{
								error = 1;
								break;
							}
						}
						if(sistema_archivo_hay_error(archivo,error_o_datos_a_retornar,  0)) error = 1;
						if(sistema_archivo_cerrar(archivo,error_o_datos_a_retornar,  0) != 0) error = 1;
					}
				}

				if(archivoOriginalCreado &&
					sistema_archivo_eliminar(rutaOriginal,error_o_datos_a_retornar,  0) != 0)
				{
					error = 1;
				}
				if(archivoRenombradoCreado &&
					sistema_archivo_eliminar(rutaRenombrada,error_o_datos_a_retornar,  0) != 0)
				{
					error = 1;
				}
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, 
					"\nPrueba de primitivas de archivo: %s\n",
					error ? "fallida" : "correcta"
				);
				sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
				break;
			}
			case 6:
			{
				configurarConsolaUTF8();
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Texto para imprimir: ");
				char *texto = leerLineaConsola(error_o_datos_a_retornar, 1);
				if(texto == NULL) return -1;
				sistema_consola_escribir_caracter('[',error_o_datos_a_retornar,  0);
				sistema_consola_escribir_texto(texto,error_o_datos_a_retornar,  0);
				sistema_consola_escribir_caracter(']',error_o_datos_a_retornar,  0);
				sistema_consola_escribir_caracter('\n',error_o_datos_a_retornar,  0);
				imprimirMensaje_para_depurar(error_o_datos_a_retornar, 
					0,
					"Formato de depuración: texto=%s longitud=%d\n",
					texto,
					(int)strlen(texto)
				);
				char *elementos[] = { texto, NULL };
				imprimirMensaje_para_depurar_arreglo(error_o_datos_a_retornar, nivel_de_profundidad, elementos, "dato", 1);
				sistema_consola_escribir_caracter('\n',error_o_datos_a_retornar,  0);
				sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
				break;
			}
			case 7:
				return 0;
			default:
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Opción no válida.\n");
				break;
		}
		if(opcion_seleccionada != 0) return 0;
		opcion = 0;
	}
}



/*
 * Extrae el código inicial de una traza, con o sin separador inicial.
 * Ejemplos: "°-2¬1" y "-2|error" guardan -2 y devuelven 1.
 */
  static int leerCodigoResultado(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *texto, int *codigo)
 {
	const char *inicio;
	const char *fin;
	int separadorEncontrado = 0;
	(void)nivel_de_profundidad;
 	(void)nivel_de_profundidad;

	if(texto == NULL || codigo == NULL) return 0;

	inicio = texto;
	for(size_t i = 0;
		i < sizeof(GG_caracter_separacion) / sizeof(GG_caracter_separacion[0]);
		i++)
	{
		size_t longitud = strlen(GG_caracter_separacion[i]);
		if(strncmp(inicio, GG_caracter_separacion[i], longitud) == 0)
		{
			inicio += longitud;
			break;
		}
	}

	fin = inicio;
	if(*fin == '-' || *fin == '+') fin++;
	while(*fin >= '0' && *fin <= '9') fin++;
	if(fin == inicio || (fin == inicio + 1 &&
		(*inicio == '-' || *inicio == '+')))
	{
		return 0;
	}

	if(*fin != '\0')
	{
		for(size_t i = 0;
			i < sizeof(GG_caracter_separacion) / sizeof(GG_caracter_separacion[0]);
			i++)
		{
			size_t longitud = strlen(GG_caracter_separacion[i]);
			if(strncmp(fin, GG_caracter_separacion[i], longitud) == 0)
			{
				separadorEncontrado = 1;
				break;
			}
		}
		if(!separadorEncontrado) return 0;
	}

	return textoAEnteroN(inicio, (size_t)(fin - inicio), codigo);
 }

/* Interpreta como error un código negativo o una cadena de resultado inválida. */
 static int resultadoTieneError(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *texto)
 {
 	int codigo;
 	return !leerCodigoResultado(error_o_datos_a_retornar, nivel_de_profundidad, texto, &codigo) || codigo < 0;
 }



#pragma endregion FUNCIONES_DE_DEPURACION