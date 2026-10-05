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

#if !defined(SEMICONDUCTOR)
 #include <stdio.h>
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

 /*
  * Tamaño de arranque, no límite máximo: las líneas largas se amplían mediante
  * la capa de memoria. Ejemplo: una línea de 80 caracteres crece desde 16.
  */
 #define TAMANO_INICIAL_LINEA 16
/* Archivo que usa el menú de operaciones básicas de texto. */
 #define NOMBRE_ARCHIVO "datos.txt"
 /*
  * ============================================================================
  * CONFIGURACIÓN DE CONSOLA UTF-8
  * ============================================================================
  */
#pragma region "CONFIGURACIÓN DE CONSOLA UTF-8"
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
 #pragma endregion
 /* ---------------------------------------------------------------------------
    CAPA DE MEMORIA, ARCHIVOS Y CONSOLA
    --------------------------------------------------------------------------- */
 #pragma region "CAPA DE MEMORIA, ARCHIVOS Y CONSOLA"
/*
 * Contrato de fin de lectura compartido por consola y archivos.
 * Las implementaciones de escritorio traducen EOF a este valor.
 */
 typedef struct SistemaArchivo SistemaArchivo;
 #define SISTEMA_ARCHIVO_FIN_LECTURA (-1)
/* La aplicación superior no ve FILE*, malloc ni los periféricos de consola. */
 static void * sistema_memoria_reservar(size_t cantidad);
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad);
 static void sistema_memoria_liberar(void * memoria);
 static SistemaArchivo * sistema_archivo_abrir(const char * ruta,const char * modo);
 static int sistema_archivo_cerrar(SistemaArchivo *archivo);
 static int sistema_archivo_eliminar(const char * ruta);
 static int sistema_archivo_renombrar(const char * origen,const char * destino);
 static int textoAEntero(const char *texto, int *resultado);
 static int leerCodigoResultado(const char *texto, int *codigo);
 static int resultadoTieneError(const char *texto);
 static int sistema_archivo_leer_caracter(SistemaArchivo *archivo);
 static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter);
 static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto);
 static int sistema_archivo_hay_error(SistemaArchivo *archivo);
 static int sistema_consola_leer_caracter(void);
 static int sistema_consola_escribir_caracter(int caracter);
 static int sistema_consola_escribir_texto(const char *texto);
 static int sistema_consola_escribir_formato(const char *formato, ...);
 #pragma endregion
 // ============================================================================
 // IDENTIDAD, SEPARADORES Y CONFIGURACIÓN GLOBAL
 // ============================================================================
 #pragma region "FUNCIONES DECLARACIÓN VARIABLES GLOBALES"
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
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES TEX_BASE
 // ============================================================================
 /*
  * API de texto y archivos usada por el menú. Cada char* devuelto es memoria
  * propiedad del llamador y debe liberarse con sistema_memoria_liberar().
  */
 #pragma region "FUNCIONES TEX_BASE"
 static char * leerLineaDinamica(SistemaArchivo * flujo, int nivel_de_profundidad);
 static char * leerLineaConsola(int nivel_de_profundidad);
 char * modificarColumna(const char * lineaOriginal,int columnaTarget,const char * nuevoValor,int nivel_de_profundidad);
 char * ejecutarEjemplosPrueba(int nivel_de_profundidad);
 char * submenu_tex_base(char * parametros_en_texto_a_splitear, int nivel_de_profundidad);
 char * submenu_enlasador_mandar_mensajes(char * parametros_en_texto_a_splitear, int nivel_de_profundidad);
 char * submenu_operaciones_de_texto(char * parametros_en_texto_a_splitear, int nivel_de_profundidad);
 char * leerArchivo(const char * ruta,int nivel_de_profundidad);
 char * escribirLinea(const char * ruta,const char * nuevaLinea,int nivel_de_profundidad);
 char * editarLinea(const char * ruta,int idLinea,const char * nuevoTexto,int nivel_de_profundidad);
 char * editarColumna(const char * ruta,int idLinea,int idColumna,const char * nuevoValor,int nivel_de_profundidad);
 char * eliminarLinea(const char * ruta,int idLinea,int nivel_de_profundidad);
 char * vaciarLinea(const char * ruta,int idLinea,int nivel_de_profundidad);
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE MENSAJERÍA
 // ============================================================================
 /* API de mensajería: resultados con el formato definido por crearResultado(). */
 #pragma region "FUNCIONES MENSAJERÍA"
 char * mandar_mensje_a_todos(const char * mensaje,int nivel_de_profundidad);
 char * mandar_mensje_a_contacto(const char * mensaje,const char * contactos,int id_opcional,int nivel_de_profundidad);
 char * mandar_mensje_al_primero_que_responda(const char * mensaje_pregunta,const char * mensaje_de_que_ya_alguien_lo_acepto,const char * menaje_respuesta_al_quien_lo_logro,int nivel_de_profundidad);
 char * checar_si_hay_mensajes_no_leido(int nivel_de_profundidad);
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 // ============================================================================
 /*
  * Primitivas de cadenas y resultados. Las cadenas devueltas dinámicamente
  * pertenecen al llamador; split() se libera con liberarSplit().
  */
 #pragma region "FUNCIONES OPERACIONES DE TEXTO"
 char ** split(const char * texto,const char * delimitador,int * cantidad,int nivel_de_profundidad);
 void liberarSplit(char ** partes, int cantidad, int nivel_de_profundidad);
 char * join(char ** arreglo, int cantidad,const char * carcter_separacion,int nivel_de_profundidad);
 char * crearResultado(int codigo,const char * informacion,const char * resultado_anterior,const char * funcion_llamante,int nivel_de_profundidad);
 #pragma endregion
 int prueba(void);
 // ============================================================================
 // MAIN
 // ============================================================================
/*
  * Punto de entrada de escritorio: muestra el menú y distribuye las opciones.
  * En SEMICONDUCTOR ejecuta directamente prueba(), para no depender del menú
  * que requiere consola interactiva y memoria dinámica.
  */
  int main(void)
 {
  	configurarConsolaUTF8();
 #if defined(PLATAFORMA_SEMICONDUCTOR)
 	return prueba() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
 #else
 	for(int i = 0; i < 3; i++)
 	{
 		char *resultadoInicial = crearResultado(200, "prueba", NULL, "main", 2);
 		if(resultadoInicial == NULL)
 		{
 			sistema_consola_escribir_formato("No se pudo crear el resultado inicial.\n");
 			return EXIT_FAILURE;
 		}
 		sistema_consola_escribir_formato("%s\n", resultadoInicial);
 		sistema_memoria_liberar(resultadoInicial);
 	}
 	int opcion = 0;             /* Opción numérica elegida en el menú principal. */
 	char * resultado = NULL;    /* Último resultado codificado; se libera antes de reemplazarlo. */
 	do {
 		sistema_consola_escribir_formato("\n=== MENÚ PRINCIPAL ===\n");
 		sistema_consola_escribir_formato("1. comandos_tex_base\n");
 		sistema_consola_escribir_formato("2. enlasador_mandar_mensajes\n");
 		sistema_consola_escribir_formato("3. operaciones_de_texto\n");
 		sistema_consola_escribir_formato("4. Salir\n");
		sistema_consola_escribir_formato("5. Ejecutar prueba del sistema\n");
 		sistema_consola_escribir_formato("Seleccione una opción: ");
 		char * optStr = leerLineaConsola(1);
 		if(optStr == NULL)
 		{
 			opcion = 4;
 			sistema_memoria_liberar(resultado);
 			resultado = crearResultado(0, "entrada_finalizada", "", "main", 1);
 			break;
 		}
 		if(!textoAEntero(optStr, &opcion)) opcion = 0;
 		sistema_memoria_liberar(optStr);
 		switch(opcion)
 		{
 			case 1:
 			{
 				char * parametros = "l";
 				sistema_memoria_liberar(resultado);
 				resultado = submenu_tex_base(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				sistema_consola_escribir_formato("%s\n", resultadoMain);
 				sistema_memoria_liberar(resultadoMain);
 				break;
 			}
 			case 2:
 			{
 				char * parametros = "mandar_todos,mandar_contacto,mandar_primero";
 				sistema_memoria_liberar(resultado);
 				resultado = submenu_enlasador_mandar_mensajes(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				sistema_consola_escribir_formato("%s\n", resultadoMain);
 				sistema_memoria_liberar(resultadoMain);
 				break;
 			}
 			case 3:
 			{
 				char * parametros = "split,modificar_columna,leer_linea";
 				sistema_memoria_liberar(resultado);
 				resultado = submenu_operaciones_de_texto(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				sistema_consola_escribir_formato("%s\n", resultadoMain);
 				sistema_memoria_liberar(resultadoMain);
 				break;
 			}
 			case 4:
 			{
 				sistema_consola_escribir_formato("Saliendo...\n");
 				sistema_memoria_liberar(resultado);
 				resultado = crearResultado(0, "salida_ok", "", "main", 1);
 				sistema_consola_escribir_formato("%s\n", resultado);
 				break;
 			}
			case 5:
			{
				prueba();
				break;
			}
 			default:
 			{
 				sistema_consola_escribir_formato("Opción no válida.\n");
 				sistema_memoria_liberar(resultado);
 				resultado = crearResultado(-2, "opcion_no_valida", "", "main", 1);
 				sistema_consola_escribir_formato("%s\n", resultado);
 				break;
 			}
 		}
 	} while(opcion != 4);
 	int codigoFinal = -1;
 	leerCodigoResultado(resultado, &codigoFinal);
 	sistema_memoria_liberar(resultado);
 	return codigoFinal;
#endif
 }
 
 /*
  * Prueba mínima de la interfaz abstracta de consola.
  * Ejemplo: se recibe "Ana" y se escribe "Hola Ana".
  * El límite es de 32 bytes (un carácter UTF-8 puede ocupar varios bytes).
  * Devuelve 0 si la interacción se completa; -1 ante EOF, nombre vacío/largo
  * o error de salida.
  */
 int prueba(void)
 {
 	enum { TAMANO_NOMBRE_PRUEBA = 32 };
 	char nombre[TAMANO_NOMBRE_PRUEBA + 1]; /* Entrada local, más el '\0' final. */
 	size_t longitud = 0;                    /* Bytes del nombre ya almacenados. */
 	int caracter;                           /* Último byte leído o FIN_LECTURA. */
 	int nombreDemasiadoLargo = 0;           /* Se activa si llegan más de 32 bytes. */

 	if(sistema_consola_escribir_texto(
 		"\n=== PRUEBA DEL SISTEMA ===\n\nEscribe tu nombre:\n> ") != 0)
 	{
 		return -1;
 	}

 	while((caracter = sistema_consola_leer_caracter()) != SISTEMA_ARCHIVO_FIN_LECTURA)
 	{
 		/* Enter termina la entrada; CR también cubre terminales con retorno de carro. */
 		if(caracter == '\n' || caracter == '\r') break;
 		if(longitud < TAMANO_NOMBRE_PRUEBA)
 		{
 			nombre[longitud++] = (char)caracter;
 		}
 		else
 		{
 			nombreDemasiadoLargo = 1;
 		}
 	}

 	if(caracter == SISTEMA_ARCHIVO_FIN_LECTURA || nombreDemasiadoLargo || longitud == 0)
 	{
 		sistema_consola_escribir_texto(
 			nombreDemasiadoLargo
 				? "\nEl nombre es demasiado largo para esta prueba.\n"
 				: "\nNo se pudo leer un nombre desde la consola.\n"
 		);
 		return -1;
 	}

 	nombre[longitud] = '\0';

 	if(sistema_consola_escribir_texto("\nHola ") != 0 ||
 		sistema_consola_escribir_texto(nombre) != 0 ||
 		sistema_consola_escribir_texto(
 			"\n\nPrueba completada correctamente.\n") != 0)
 	{
 		return -1;
 	}

 	return 0;
 }

 // ============================================================================
 // FUNCIONES SUBMENÚS
 // ============================================================================
 #pragma region "FUNCIONES SUBMENÚS"
/*
 * Menú de lectura y CRUD del archivo datos.txt.
 * Ejemplo: una entrada "3" solicita ID y texto y reemplaza la línea 3.
 * Devuelve una cadena de resultado codificada; el llamador debe liberarla.
 */
 char * submenu_tex_base(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	int opcion = 0;                       /* Opción seleccionada en este submenú. */
 	int idLinea = 0;                      /* Número de línea (base 1), por ejemplo 2. */
 	int idColumna = 0;                    /* Columna de CSV (base 1), por ejemplo 3. */
 	char * resultado = NULL;              /* Resultado de la última operación del archivo. */
 	char * estado = NULL;                 /* Estado que se presenta y luego se libera. */
 	int cantidad = 0;                     /* Cantidad de parámetros tras split(). */
 	char ** parametros_espliteados = NULL;/* Elementos temporales derivados del parámetro. */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		sistema_consola_escribir_formato("[split tex_base] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
 	sistema_consola_escribir_formato("\n=== SUBMENÚ comandos_tex_base ===\n");
 	sistema_consola_escribir_formato("1. Leer todo el archivo\n");
 	sistema_consola_escribir_formato("2. Añadir nueva línea\n");
 	sistema_consola_escribir_formato("3. Editar línea completa por ID\n");
 	sistema_consola_escribir_formato("4. Editar columna específica de una línea\n");
 	sistema_consola_escribir_formato("5. Eliminar línea por ID\n");
 	sistema_consola_escribir_formato("6. Vaciar línea por ID\n");
 	sistema_consola_escribir_formato("7. Ejecutar ejemplos de prueba\n");
 	sistema_consola_escribir_formato("8. Volver al menú principal\n");
 	sistema_consola_escribir_formato("Seleccione una opción: ");
 	char * optStr = leerLineaConsola(nivel_de_profundidad);
 	if(!textoAEntero(optStr, &opcion)) opcion = 0;
 	sistema_memoria_liberar(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_memoria_liberar(resultado);
 			resultado = leerArchivo(NOMBRE_ARCHIVO, nivel_de_profundidad);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato("Ingrese el texto/línea a añadir: ");
 			char * texto = leerLineaConsola(nivel_de_profundidad);
 			sistema_memoria_liberar(resultado);
 			resultado = escribirLinea(NOMBRE_ARCHIVO, texto, nivel_de_profundidad);
 			sistema_memoria_liberar(texto);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato("Ingrese ID de línea a editar: ");
 			char * inId = leerLineaConsola(nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea)) idLinea = 0;
 			sistema_memoria_liberar(inId);
 			sistema_consola_escribir_formato("Ingrese el nuevo contenido completo: ");
 			char * texto = leerLineaConsola(nivel_de_profundidad);
 			sistema_memoria_liberar(resultado);
 			resultado = editarLinea(NOMBRE_ARCHIVO, idLinea, texto, nivel_de_profundidad);
 			sistema_memoria_liberar(texto);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			sistema_consola_escribir_formato("Ingrese ID de línea a editar: ");
 			char * inId = leerLineaConsola(nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea)) idLinea = 0;
 			sistema_memoria_liberar(inId);
 			sistema_consola_escribir_formato("Ingrese el número de columna a editar (1, 2, ...): ");
 			char * inCol = leerLineaConsola(nivel_de_profundidad);
 			if(!textoAEntero(inCol, &idColumna)) idColumna = 0;
 			sistema_memoria_liberar(inCol);
 			sistema_consola_escribir_formato("Ingrese el nuevo valor para esa columna: ");
 			char * valor = leerLineaConsola(nivel_de_profundidad);
 			sistema_memoria_liberar(resultado);
 			resultado = editarColumna(NOMBRE_ARCHIVO, idLinea, idColumna, valor, nivel_de_profundidad);
 			sistema_memoria_liberar(valor);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 5:
 		{
 			sistema_consola_escribir_formato("Ingrese ID de línea a eliminar: ");
 			char * inId = leerLineaConsola(nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea)) idLinea = 0;
 			sistema_memoria_liberar(inId);
 			sistema_memoria_liberar(resultado);
 			resultado = eliminarLinea(NOMBRE_ARCHIVO, idLinea, nivel_de_profundidad);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 6:
 		{
 			sistema_consola_escribir_formato("Ingrese ID de línea a vaciar: ");
 			char * inId = leerLineaConsola(nivel_de_profundidad);
 			if(!textoAEntero(inId, &idLinea)) idLinea = 0;
 			sistema_memoria_liberar(inId);
 			sistema_memoria_liberar(resultado);
 			resultado = vaciarLinea(NOMBRE_ARCHIVO, idLinea, nivel_de_profundidad);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 7:
 		{
 			sistema_memoria_liberar(resultado);
 			resultado = ejecutarEjemplosPrueba(nivel_de_profundidad);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 8:
 		{
 			sistema_consola_escribir_formato("Volviendo al menú principal...\n");
 			sistema_memoria_liberar(estado);
 			sistema_memoria_liberar(resultado);
 			return crearResultado(1, "informacionMain", "1", __func__, 1);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato("Opción no válida.\n");
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato("%s\n", estado);
 	}
 	sistema_memoria_liberar(estado);
 	sistema_memoria_liberar(resultado);
 	return crearResultado(1, "informacionMain", "1", __func__, 1);
 }
/*
 * Menú para guardar mensajes en los archivos configurados.
 * Ejemplo: "Hola" a todos agrega la línea "Hola" al archivo de mensajes.
 * Cada resultado devuelto usa el protocolo crearResultado() y pertenece al llamador.
 */
 char * submenu_enlasador_mandar_mensajes(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	int opcion = 0;                        /* Acción elegida: 1=todos, 2=contactos, 3=primero, 4=volver. */
 	char * resultado = NULL;               /* Resultado de mensajería, por ejemplo "1|mensaje_enviado|...". */
 	char * estado = NULL;                  /* Estado del menú, mostrado y liberado al terminar. */
 	int cantidad = 0;                      /* Elementos del parámetro informativo separado por comas. */
 	char ** parametros_espliteados = NULL; /* Partes temporales; se liberan con liberarSplit(). */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		sistema_consola_escribir_formato("[split mensajes] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
 	sistema_consola_escribir_formato("\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
 	sistema_consola_escribir_formato("1. mandar_mensje_a_todos(mensaje)\n");
 	sistema_consola_escribir_formato("2. mandar_mensje_a_contacto(mensaje, contactos, id_opcional)\n");
 	sistema_consola_escribir_formato("3. mandar_mensje_al_primero_que_responda("
 		"mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto, "
 		"menaje_respuesta_al_quien_lo_logro)\n");
 	sistema_consola_escribir_formato("4. Volver al menú principal\n");
 	sistema_consola_escribir_formato("Seleccione una opción: ");
 	char * optStr = leerLineaConsola(nivel_de_profundidad);
 	if(!textoAEntero(optStr, &opcion)) opcion = 0;
 	sistema_memoria_liberar(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_consola_escribir_formato("Ingrese el mensaje para todos: ");
 			char * mensaje = leerLineaConsola(nivel_de_profundidad);
 			sistema_memoria_liberar(resultado);
 			resultado = mandar_mensje_a_todos(mensaje, nivel_de_profundidad);
 			sistema_memoria_liberar(mensaje);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato("Ingrese el mensaje: ");
 			char * mensaje = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese la lista de contactos: ");
 			char * contactos = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese id opcional: ");
 			char * idStr = leerLineaConsola(nivel_de_profundidad);
 			int id_opcional = 0;
 			textoAEntero(idStr, &id_opcional);
 			sistema_memoria_liberar(idStr);
 			sistema_memoria_liberar(resultado);
 			resultado = mandar_mensje_a_contacto(mensaje, contactos, id_opcional, nivel_de_profundidad);
 			sistema_memoria_liberar(mensaje);
 			sistema_memoria_liberar(contactos);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato("Ingrese el mensaje de pregunta: ");
 			char * pregunta = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese el mensaje de que alguien ya lo aceptó: ");
 			char * aceptado = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese la respuesta a quien lo logró: ");
 			char * respuesta = leerLineaConsola(nivel_de_profundidad);
 			sistema_memoria_liberar(resultado);
 			resultado = mandar_mensje_al_primero_que_responda(pregunta, aceptado, respuesta, nivel_de_profundidad);
 			sistema_memoria_liberar(pregunta);
 			sistema_memoria_liberar(aceptado);
 			sistema_memoria_liberar(respuesta);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			sistema_consola_escribir_formato("Volviendo al menú principal...\n");
 			sistema_memoria_liberar(estado);
 			sistema_memoria_liberar(resultado);
 			return crearResultado(1, "informacionMain", "1", __func__, 1);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato("Opción no válida.\n");
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato("%s\n", estado);
 	}
 	sistema_memoria_liberar(estado);
 	sistema_memoria_liberar(resultado);
 	return crearResultado(1, "informacionMain", "1", __func__, 1);
 }
/*
 * Menú de demostración de split(), modificarColumna() y lectura de consola.
 * Ejemplo: "a,b,c", columna 2 y "B" produce "a,B,c".
 */
 char * submenu_operaciones_de_texto(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	int opcion = 0;                        /* 1=split, 2=editar columna, 3=leer consola, 4=volver. */
 	int cantidad = 0;                      /* Número de argumentos de ejemplo divididos. */
 	char ** parametros_espliteados = NULL; /* Argumentos temporales que se liberan al mostrarlos. */
 	char * estado = NULL;                  /* Resultado de estado mostrado al finalizar la opción. */
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		sistema_consola_escribir_formato("[split operaciones_texto] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			sistema_consola_escribir_formato("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
 	sistema_consola_escribir_formato("\n=== SUBMENÚ operaciones_de_texto ===\n");
 	sistema_consola_escribir_formato("1. split(texto, delimitador)\n");
 	sistema_consola_escribir_formato("2. modificarColumna(linea, columna, nuevoValor)\n");
 	sistema_consola_escribir_formato("3. leerLineaDinamica(consola)\n");
 	sistema_consola_escribir_formato("4. Volver al menú principal\n");
 	sistema_consola_escribir_formato("Seleccione una opción: ");
 	char * optStr = leerLineaConsola(nivel_de_profundidad);
 	if(!textoAEntero(optStr, &opcion)) opcion = 0;
 	sistema_memoria_liberar(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			sistema_consola_escribir_formato("Ingrese el texto a partir: ");
 			char * texto = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese el delimitador: ");
 			char * delimitador = leerLineaConsola(nivel_de_profundidad);
 			int total = 0; /* Número de segmentos obtenidos del texto ingresado. */
 			char ** partes = split(texto, delimitador, & total, nivel_de_profundidad);
 			if(partes == NULL)
 			{
 				sistema_memoria_liberar(texto);
 				sistema_memoria_liberar(delimitador);
 				sistema_memoria_liberar(estado);
 				estado = crearResultado(-1, "error_split", "", __func__, 1);
 				break;
 			}
 			for(int i = 0; i < total; i++)
 			{
 				sistema_consola_escribir_formato("  parte[%d] = %s\n", i, partes[i]);
 			}
 			liberarSplit(partes, total, nivel_de_profundidad);
 			sistema_memoria_liberar(texto);
 			sistema_memoria_liberar(delimitador);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			sistema_consola_escribir_formato("Ingrese la línea original: ");
 			char * linea = leerLineaConsola(nivel_de_profundidad);
 			sistema_consola_escribir_formato("Ingrese la columna a cambiar: ");
 			char * colStr = leerLineaConsola(nivel_de_profundidad);
 			int columna = 0;
 			textoAEntero(colStr, &columna);
 			sistema_memoria_liberar(colStr);
 			sistema_consola_escribir_formato("Ingrese el nuevo valor: ");
 			char * nuevo = leerLineaConsola(nivel_de_profundidad);
 			char * resultadoMod = modificarColumna(linea, columna, nuevo, nivel_de_profundidad); /* Cadena nueva, p. ej. "a,B,c". */
 			if(resultadoMod == NULL)
 			{
 				sistema_memoria_liberar(linea);
 				sistema_memoria_liberar(nuevo);
 				sistema_memoria_liberar(estado);
 				estado = crearResultado(-1, "error_modificar_columna", "", __func__, 1);
 				break;
 			}
 			sistema_consola_escribir_formato("Resultado: %s\n", resultadoMod);
 			sistema_memoria_liberar(resultadoMod);
 			sistema_memoria_liberar(linea);
 			sistema_memoria_liberar(nuevo);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			sistema_consola_escribir_formato("Ingrese una línea de texto: ");
 			char * linea = leerLineaConsola(nivel_de_profundidad);
 			if(linea == NULL)
 			{
 				sistema_memoria_liberar(estado);
 				estado = crearResultado(-1, "error_lectura", "", __func__, 1);
 				break;
 			}
 			sistema_consola_escribir_formato("Línea recibida: %s\n", linea);
 			sistema_memoria_liberar(linea);
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			sistema_consola_escribir_formato("Volviendo al menú principal...\n");
 			if(estado != NULL)
 			{
 				char * tmp = estado;
 				estado = NULL;
 				return tmp;
 			}
 			return crearResultado(1, "informacionMain", "1", __func__, 1);
 		}
 		default:
 		{
 			sistema_consola_escribir_formato("Opción no válida.\n");
 			sistema_memoria_liberar(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		sistema_consola_escribir_formato("%s\n", estado);
 	}
 	sistema_memoria_liberar(estado);
 	return crearResultado(1, "informacionMain", "1", __func__, 1);
 }
 #pragma endregion
 // ============================================================================
 // FUNCIONES OPERACIONES DE TEXTO
 // ============================================================================
 #pragma region "FUNCIONES OPERACIONES DE TEXTO"
 
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
 char ** split(const char *texto, const char *separador, int *cantidad, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
 	if(cantidad == NULL) return NULL;
 	*cantidad = 0;
 	if(texto == NULL || separador == NULL || separador[0] == '\0') return NULL;

 	size_t longitud_texto = strlen(texto);             /* Bytes de entrada, sin contar '\0'. */
 	size_t longitud_separador = strlen(separador);     /* El delimitador puede tener varios bytes. */
 	size_t capacidad = 8;                              /* Ranuras iniciales del vector de partes. */
 	size_t contador = 0;                               /* Partes completas ya guardadas. */
 	size_t inicio = 0;                                 /* Primer byte del campo actual. */
 	size_t posicion = 0;                               /* Byte examinado buscando el delimitador. */
 	char **partes = sistema_memoria_reservar(capacidad * sizeof(*partes)); /* Vector dinámico. */
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
 			liberarSplit(partes, (int)contador, nivel_de_profundidad);
 			return NULL;
 		}

 		if(contador == capacidad - 1)
 		{
 			if(capacidad > (size_t)-1 / 2 / sizeof(*partes))
 			{
 				liberarSplit(partes, (int)contador, nivel_de_profundidad);
 				return NULL;
 			}
 			size_t nueva_capacidad = capacidad * 2; /* Duplica ranuras para amortizar realocaciones. */
 			char **temporal = sistema_memoria_redimensionar(partes, nueva_capacidad * sizeof(*partes));
 			if(temporal == NULL)
 			{
 				liberarSplit(partes, (int)contador, nivel_de_profundidad);
 				return NULL;
 			}
 			partes = temporal;
 			capacidad = nueva_capacidad;
 		}

 		size_t longitud_parte = posicion - inicio; /* Bytes del campo, incluso si el campo está vacío. */
 		if(longitud_parte == (size_t)-1)
 		{
 			liberarSplit(partes, (int)contador, nivel_de_profundidad);
 			return NULL;
 		}
 		partes[contador] = sistema_memoria_reservar(longitud_parte + 1);
 		if(partes[contador] == NULL)
 		{
 			liberarSplit(partes, (int)contador, nivel_de_profundidad);
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
 void liberarSplit(char **partes, int cantidad, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
 	if(partes == NULL) return;
 	for(int i = 0; i < cantidad; i++) sistema_memoria_liberar(partes[i]);
 	sistema_memoria_liberar(partes);
 }
/*
 * Une cantidad elementos con un separador y reserva la cadena resultante.
 * Ejemplo: ["rojo","azul"], 2, "|" produce "rojo|azul".
 * Los elementos NULL se tratan como cadenas vacías; el resultado se libera con
 * sistema_memoria_liberar().
 */
 char * join(char **arreglo, int cantidad, const char *carcter_separacion, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
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

 	char *resultado = sistema_memoria_reservar(longitud_total); /* Buffer exacto que devuelve la función. */
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
 static int textoAEntero(const char *texto, int *resultado)
 {
 	return texto != NULL
 		? textoAEnteroN(texto, strlen(texto), resultado)
 		: 0;
 }

/*
 * Extrae el primer campo del protocolo de resultado.
 * Ejemplo: "-2|error||editarLinea|3" guarda -2 y devuelve 1.
 */
 static int leerCodigoResultado(const char *texto, int *codigo)
 {
 	const char *separador;

 	if(texto == NULL || codigo == NULL) return 0;
 	separador = strchr(texto, GG_caracter_separacion[0][0]);
 	return separador != NULL
 		? textoAEnteroN(texto, (size_t)(separador - texto), codigo)
 		: 0;
 }

/* Interpreta como error un código negativo o una cadena de resultado inválida. */
 static int resultadoTieneError(const char *texto)
 {
 	int codigo;
 	return !leerCodigoResultado(texto, &codigo) || codigo < 0;
 }

/*
 * Formateador pequeño de la consola común: admite %s, %d y %% únicamente.
 * Ejemplo: ("línea %d: %s\n", 2, "Ana") escribe "línea 2: Ana".
 * Devuelve 0 al escribir todo y -1 si el formato o backend falla.
 */
 static int sistema_consola_escribir_formato(const char *formato, ...)
 {
 	va_list argumentos;
 	const char *cursor;
 	const char *inicio;

 	if(formato == NULL) return -1;

 	va_start(argumentos, formato);
 	cursor = formato;
 	inicio = formato;

 	while(*cursor != '\0')
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
 			if(sistema_consola_escribir_caracter((unsigned char)*inicio++) != 0) goto error;
 		}
 		cursor++;
 		if(*cursor == '\0') goto error;

 		if(*cursor == 's')
 		{
 			texto = va_arg(argumentos, const char *);
 			if(texto == NULL || sistema_consola_escribir_texto(texto) != 0) goto error;
 		}
 		else if(*cursor == 'd')
 		{
 			if(enteroATexto(va_arg(argumentos, int), numero, sizeof(numero)) == NULL ||
 				sistema_consola_escribir_texto(numero) != 0)
 			{
 				goto error;
 			}
 		}
 		else if(*cursor == '%')
 		{
 			if(sistema_consola_escribir_caracter('%') != 0) goto error;
 		}
 		else
 		{
 			goto error;
 		}

 		cursor++;
 		inicio = cursor;
 	}

 	if(sistema_consola_escribir_texto(inicio) != 0) goto error;
 	va_end(argumentos);
 	return 0;

 error:
 	va_end(argumentos);
 	return -1;
 }

/* Escribe un entero decimal en archivo como texto; 0 significa éxito. */
 static int sistema_archivo_escribir_entero(SistemaArchivo *archivo, int valor)
 {
 	char buffer[sizeof(int) * CHAR_BIT + 2];
 	if(enteroATexto(valor, buffer, sizeof(buffer)) == NULL) return -1;
 	return sistema_archivo_escribir_texto(archivo, buffer);
 }

/* Escribe texto seguido de '\n'; -1 señala que falló cualquiera de los dos pasos. */
 static int escribirLineaArchivo(SistemaArchivo *archivo, const char *texto)
 {
 	if(texto == NULL || sistema_archivo_escribir_texto(archivo, texto) != 0) return -1;
 	return sistema_archivo_escribir_caracter(archivo, '\n');
 }

/*
 * Construye una cadena de estado propiedad del llamador.
 * Ejemplo: (1,"escritura_ok","","escribirLinea",1) produce
 * "1|escritura_ok||escribirLinea|1".
 * codigo >= 0 representa éxito/estado; código negativo representa error.
 * Devuelve NULL ante desbordamiento de tamaño o fallo de reserva.
 */
  char * crearResultado(int codigo,
 	const char * informacion,
 		const char * resultado_anterior,
 			const char * funcion_llamante,
 				int nivel_de_profundidad)
 {
 	const char *separador = GG_caracter_separacion[0]; /* "|" separa los cinco campos. */
 	const char *info = (informacion != NULL) ? informacion : ""; /* Texto descriptivo o vacío. */
 	const char *anterior = (resultado_anterior != NULL) ? resultado_anterior : ""; /* Campo encadenado. */
 	const char *funcion = (funcion_llamante != NULL) ? funcion_llamante : ""; /* Origen del resultado. */
 	char codigoTexto[sizeof(int) * CHAR_BIT + 2];
 	char profundidadTexto[sizeof(int) * CHAR_BIT + 2];
 	const char *partes[9]; /* Secuencia alternada para ensamblar los cinco campos. */

 	if(enteroATexto(codigo, codigoTexto, sizeof(codigoTexto)) == NULL ||
 		enteroATexto(nivel_de_profundidad, profundidadTexto, sizeof(profundidadTexto)) == NULL)
 	{
 		return NULL;
 	}

 	partes[0] = codigoTexto;
 	partes[1] = separador;
 	partes[2] = info;
 	partes[3] = separador;
 	partes[4] = anterior;
 	partes[5] = separador;
 	partes[6] = funcion;
 	partes[7] = separador;
 	partes[8] = profundidadTexto;

 	size_t longitudTotal = 1; /* El byte final '\0' también requiere espacio. */
 	for(size_t i = 0; i < sizeof(partes) / sizeof(partes[0]); i++)
 	{
 		size_t longitudParte = strlen(partes[i]);
 		if(longitudParte > (size_t)-1 - longitudTotal) return NULL;
 		longitudTotal += longitudParte;
 	}

 	char *resultado = sistema_memoria_reservar(longitudTotal); /* Memoria que el llamador debe liberar. */
 	if(resultado == NULL) return NULL;

 	char *destino = resultado; /* Cursor de escritura que avanza a través del buffer. */
 	for(size_t i = 0; i < sizeof(partes) / sizeof(partes[0]); i++)
 	{
 		size_t longitudParte = strlen(partes[i]);
 		memcpy(destino, partes[i], longitudParte);
 		destino += longitudParte;
 	}
 	*destino = '\0';
 	return resultado;
 }
 
  #pragma endregion
 // ============================================================================
 // FUNCIONES OPERACIONES DE TEX_BASE
 // ============================================================================
 #pragma region "FUNCIONES OPERACIONES_DE_TEX_BASE"
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
    int (*leerCaracter)(void *contexto),
    void *contexto,
    int nivel_de_profundidad
)
{
    char *linea;       /* Buffer devuelto si la lectura termina con una línea válida. */
    size_t capacidad;  /* Bytes reservados; crece al duplicarse cuando la línea no cabe. */
    size_t longitud;   /* Bytes leídos, sin contar el '\0' de terminación. */
    int caracter;      /* Byte leído como int o SISTEMA_ARCHIVO_FIN_LECTURA. */


    /*
     * Actualmente el nivel se reserva para
     * el control interno del sistema.
     */
    (void)nivel_de_profundidad;


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
        (char *)sistema_memoria_reservar(capacidad);


    if (linea == NULL)
    {
        return NULL;
    }


    longitud = 0;


    /*
     * Leemos carácter por carácter.
     */
    while ((caracter = leerCaracter(contexto)) != SISTEMA_ARCHIVO_FIN_LECTURA)
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
                sistema_memoria_liberar(linea);
                return NULL;
            }
            capacidad *= 2;


            temporal =
                (char *)sistema_memoria_redimensionar(
                    linea,
                    capacidad
                );


            if (temporal == NULL)
            {
                sistema_memoria_liberar(linea);
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
        sistema_memoria_liberar(linea);
        return NULL;
    }


    /*
     * Terminador.
     */
    linea[longitud] = '\0';


    return linea;
}

static int leerCaracterArchivo(void *contexto)
{
	/* El contexto es SistemaArchivo*; el backend traduce su EOF al valor común. */
	return sistema_archivo_leer_caracter((SistemaArchivo *)contexto);
}

static int leerCaracterConsola(void *contexto)
{
	/* La consola no necesita objeto de contexto en la API actual. */
	(void)contexto;
	return sistema_consola_leer_caracter();
}

/* Adaptador público interno: lee una línea desde un archivo ya abierto. */
static char *leerLineaDinamica(SistemaArchivo *flujo, int nivel_de_profundidad)
{
	if(flujo == NULL) return NULL;
	return leerLineaDesde(leerCaracterArchivo, flujo, nivel_de_profundidad);
}

/* Adaptador interno: misma lógica de línea, pero leyendo desde la consola. */
static char *leerLineaConsola(int nivel_de_profundidad)
{
	return leerLineaDesde(leerCaracterConsola, NULL, nivel_de_profundidad);
}

/*
 * Sustituye una columna (numeración desde 1) de una fila delimitada por comas.
 * Ejemplo: ("Ana,25,QA", 2, "30") -> "Ana,30,QA".
 * Devuelve una nueva cadena reservada o NULL si la fila/columna no es válida.
 */
char * modificarColumna(const char * lineaOriginal,
 	int columnaTarget,
 	const char * nuevoValor,
 		int nivel_de_profundidad)
 {
 	if(lineaOriginal == NULL || nuevoValor == NULL || columnaTarget < 1) return NULL;

 	int cantidad = 0; /* Columnas encontradas en la fila, por ejemplo 3. */
 	char **partes = split(lineaOriginal, ",", &cantidad, nivel_de_profundidad); /* Campos editables. */
 	if(partes == NULL) return NULL;
 	if(columnaTarget > cantidad)
 	{
 		liberarSplit(partes, cantidad, nivel_de_profundidad);
 		return NULL;
 	}

 	size_t longitud = strlen(nuevoValor) + 1; /* Incluye el '\0' que memcpy también copiará. */
 	sistema_memoria_liberar(partes[columnaTarget - 1]);
 	partes[columnaTarget - 1] = sistema_memoria_reservar(longitud);
 	if(partes[columnaTarget - 1] == NULL)
 	{
 		liberarSplit(partes, cantidad, nivel_de_profundidad);
 		return NULL;
 	}
 	memcpy(partes[columnaTarget - 1], nuevoValor, longitud);

 	char *resultado = join(partes, cantidad, ",", nivel_de_profundidad); /* Reconstruye la fila CSV. */
 	liberarSplit(partes, cantidad, nivel_de_profundidad);
 	return resultado;
 }

/*
 * Sustituye un archivo mediante temporal y respaldo para evitar editarlo in situ.
 * Flujo de ejemplo: datos.txt -> temp_qu1ron.bak; temp.txt -> datos.txt;
 * finalmente elimina el respaldo. Devuelve 0 si completa todos los pasos.
 */
static int reemplazarArchivoTemporal(const char *ruta)
{
	const char *temporal = RUTA_ARCHIVO_TEMPORAL; /* Archivo nuevo construido aparte. */
	const char *respaldo = RUTA_ARCHIVO_RESPALDO; /* Copia temporal del archivo original. */
	SistemaArchivo *existente;                    /* Permite comprobar si ya existe el respaldo. */

	if(ruta == NULL || strcmp(ruta, temporal) == 0 || strcmp(ruta, respaldo) == 0) return -1;

	existente = sistema_archivo_abrir(respaldo, "r");
	if(existente != NULL)
	{
		sistema_archivo_cerrar(existente);
		return -1;
	}

	if(sistema_archivo_renombrar(ruta, respaldo) != 0) return -1;
	if(sistema_archivo_renombrar(temporal, ruta) != 0)
	{
		sistema_archivo_renombrar(respaldo, ruta);
		return -1;
	}

	return sistema_archivo_eliminar(respaldo);
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
	int nivel_de_profundidad
)
{
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

	archivo = sistema_archivo_abrir(ruta, "r");
	if(archivo == NULL) return -1;

	/* Evita truncar un temporal ajeno que ya existiera antes de esta operación. */
	SistemaArchivo *temporalExistente = sistema_archivo_abrir(RUTA_ARCHIVO_TEMPORAL, "r");
	if(temporalExistente != NULL)
	{
		sistema_archivo_cerrar(temporalExistente);
		sistema_archivo_cerrar(archivo);
		return -1;
	}

	temporal = sistema_archivo_abrir(RUTA_ARCHIVO_TEMPORAL, "w");
	if(temporal == NULL)
	{
		sistema_archivo_cerrar(archivo);
		return -1;
	}

	while((linea = leerLineaDinamica(archivo, nivel_de_profundidad)) != NULL)
	{
		if(actual == numeroLinea)
		{
			encontrada = 1;
			if(operacion == 0)
			{
				if(escribirLineaArchivo(temporal, contenido) != 0) error = 1;
			}
			else if(operacion == 2)
			{
				if(sistema_archivo_escribir_caracter(temporal, '\n') < 0) error = 1;
			}
			else if(operacion == 3)
			{
							/* La copia modificada se libera tras escribirla o ante error. */
							char * modificada = modificarColumna(linea, columna, contenido, nivel_de_profundidad);
				if(modificada == NULL || escribirLineaArchivo(temporal, modificada) != 0) error = 1;
				sistema_memoria_liberar(modificada);
			}
		}
		else if(escribirLineaArchivo(temporal, linea) != 0)
		{
			error = 1;
		}

		sistema_memoria_liberar(linea);
		actual++;
	}

	if(sistema_archivo_hay_error(archivo)) error = 1;
	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
	if(sistema_archivo_cerrar(temporal) != 0) error = 1;

	if(error || !encontrada)
	{
		sistema_archivo_eliminar(RUTA_ARCHIVO_TEMPORAL);
		return error ? -1 : -2;
	}

	if(reemplazarArchivoTemporal(ruta) != 0)
	{
		sistema_archivo_eliminar(RUTA_ARCHIVO_TEMPORAL);
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
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL)
 	{
 		return crearResultado(-1, "ruta_invalida", "", __func__, nivel_de_profundidad);
 	}

 	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "r");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}

 	int numeroLinea = 1; /* Número que se muestra junto a cada línea. */
 	char *linea;          /* Línea dinámica leída; se libera inmediatamente tras mostrarla. */
 	sistema_consola_escribir_formato("\n--- CONTENIDO DE [%s] ---\n", ruta);
 	while((linea = leerLineaDinamica(archivo, nivel_de_profundidad)) != NULL)
 	{
 		sistema_consola_escribir_formato("%d: %s\n", numeroLinea++, linea);
 		sistema_memoria_liberar(linea);
 	}

 	int errorLectura = sistema_archivo_hay_error(archivo);
 	int errorCierre = sistema_archivo_cerrar(archivo);
 	sistema_consola_escribir_formato("-----------------------------------\n");
 	if(errorLectura || errorCierre != 0)
 	{
 		return crearResultado(-1, "error_al_leer_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "lectura_ok", "", __func__, nivel_de_profundidad);
 }
/*
 * Agrega nuevaLinea al final del archivo y termina el registro con '\n'.
 * Ejemplo: escribirLinea("datos.txt","Ana,25,QA") añade "Ana,25,QA\n".
 */
 char * escribirLinea(const char * ruta,
 	const char * nuevaLinea,
 		int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevaLinea == NULL ||
		strcmp(ruta, RUTA_ARCHIVO_TEMPORAL) == 0 ||
		strcmp(ruta, RUTA_ARCHIVO_RESPALDO) == 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int errorEscritura = escribirLineaArchivo(archivo, nuevaLinea) != 0; /* Se confirma también el cierre. */
 	if(sistema_archivo_cerrar(archivo) != 0) errorEscritura = 1;
 	if(errorEscritura)
 	{
 		return crearResultado(-1, "error_al_escribir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "escritura_ok", "", __func__, nivel_de_profundidad);
 }
/* Reemplaza por completo la línea idLinea; el ID comienza en 1. */
 char * editarLinea(const char * ruta,
 	int idLinea,
 	const char * nuevoTexto,
 		int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevoTexto == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 0, nuevoTexto, 0, nivel_de_profundidad); /* 0=reemplazar. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "edicion_ok", "", __func__, nivel_de_profundidad);
 }
/*
 * Cambia una columna de una línea delimitada por comas.
 * Ejemplo: línea 1, columna 2, "30" convierte "Ana,25,QA" en "Ana,30,QA".
 */
 char * editarColumna(const char * ruta,
 	int idLinea,
 	int idColumna,
 	const char * nuevoValor,
 		int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevoValor == NULL || idLinea <= 0 || idColumna <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 3, nuevoValor, idColumna, nivel_de_profundidad); /* 3=columna. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_o_columna_no_encontrada", "", __func__, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "columna_editada_ok", "", __func__, nivel_de_profundidad);
 }
/* Elimina el registro idLinea; las líneas posteriores cambian de número. */
 char * eliminarLinea(const char * ruta,
 	int idLinea,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 1, NULL, 0, nivel_de_profundidad); /* 1=omitir línea. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "eliminacion_ok", "", __func__, nivel_de_profundidad);
 }
/* Conserva la posición idLinea, pero escribe una línea vacía en su lugar. */
 char * vaciarLinea(const char * ruta,
 	int idLinea,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 2, NULL, 0, nivel_de_profundidad); /* 2=línea vacía. */
 	if(resultado == -2)
 	{
 		return crearResultado(-1, "linea_no_encontrada", "", __func__, nivel_de_profundidad);
 	}
 	if(resultado != 0)
 	{
 		return crearResultado(-1, "no_se_pudo_actualizar_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "vaciado_ok", "", __func__, nivel_de_profundidad);
 }
 #pragma endregion
 // ============================================================================
 // FUNCIONES OPERACIONES DE MENSAJERÍA
 // ============================================================================
 #pragma region "FUNCIONES OPERACIONES_DE_MENSAJERIA"
/*
 * Comprueba si alguno de los tres archivos de mensajes contiene al menos un byte.
 * Devuelve código 1 si encuentra contenido, 0 si todos están vacíos/no existen,
 * o -1 si un archivo abierto da error de lectura o cierre.
 */
 char * checar_si_hay_mensajes_no_leido(int nivel_de_profundidad)
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
 		SistemaArchivo *archivo = sistema_archivo_abrir(rutas[i], "r");
 		if(archivo == NULL)
 		{
 			continue;
 		}
 		int caracter = sistema_archivo_leer_caracter(archivo); /* Un byte basta para saber si hay contenido. */
 		int errorLectura = sistema_archivo_hay_error(archivo); /* Distingue EOF de un fallo real. */
 		int errorCierre = sistema_archivo_cerrar(archivo);     /* El cierre también puede fallar. */
 		if(errorLectura || errorCierre != 0)
 		{
 			return crearResultado(-1, "error_al_consultar_mensajes", "", __func__, nivel_de_profundidad);
 		}
 		if(caracter != SISTEMA_ARCHIVO_FIN_LECTURA)
 		{
 			return crearResultado(1, "hay_mensajes_no_leidos", "", __func__, nivel_de_profundidad);
 		}
 	}
 	return crearResultado(0, "no_hay_mensajes", "", __func__, nivel_de_profundidad);
 }
/*
 * Ejecuta una secuencia CRUD sobre un archivo temporal y lo elimina al final.
 * Ejemplo: crea dos filas, cambia la edad de Luis, elimina la fila de Marta,
 * muestra el archivo restante y limpia el archivo de prueba.
 */
 char * ejecutarEjemplosPrueba(int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	const char *rutaPrueba = RUTA_ARCHIVO_PRUEBAS;
 	SistemaArchivo *existente = sistema_archivo_abrir(rutaPrueba, "r");
 	if(existente != NULL)
 	{
 		sistema_archivo_cerrar(existente);
 		return crearResultado(-1, "archivo_de_prueba_ya_existe", "", __func__, nivel_de_profundidad);
 	}

 	int exito = 0; /* Solo pasa a 1 después de que toda la secuencia termina bien. */
 	char *lineaModificada = modificarColumna("Ana,25,Programador", 2, "30", nivel_de_profundidad);
 	if(lineaModificada == NULL || strcmp(lineaModificada, "Ana,30,Programador") != 0)
 	{
 		sistema_memoria_liberar(lineaModificada);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(lineaModificada);

 	char *resultadoOperacion = escribirLinea(rutaPrueba, "Luis,10,Desarrollador", nivel_de_profundidad); /* Resultado codificado. */
 	if(resultadoTieneError(resultadoOperacion))
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = escribirLinea(rutaPrueba, "Marta,20,QA", nivel_de_profundidad);
 	if(resultadoTieneError(resultadoOperacion))
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = editarColumna(rutaPrueba, 1, 2, "15", nivel_de_profundidad);
 	if(resultadoTieneError(resultadoOperacion))
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = eliminarLinea(rutaPrueba, 2, nivel_de_profundidad);
 	if(resultadoTieneError(resultadoOperacion))
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	sistema_consola_escribir_formato("\n--- ARCHIVO DE PRUEBA RESULTANTE ---\n");
 	resultadoOperacion = leerArchivo(rutaPrueba, nivel_de_profundidad);
 	if(resultadoTieneError(resultadoOperacion))
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);
 	exito = 1;

 limpieza:
 	{
 		SistemaArchivo *archivoPrueba = sistema_archivo_abrir(rutaPrueba, "r"); /* NULL si el temporal no existe. */
 		int errorLimpieza = 0; /* Se activa ante fallo al cerrar o borrar el archivo. */

 		if(archivoPrueba != NULL)
 		{
 			if(sistema_archivo_cerrar(archivoPrueba) != 0) errorLimpieza = 1;
 			if(sistema_archivo_eliminar(rutaPrueba) != 0) errorLimpieza = 1;
 		}
 		else if(exito)
 		{
 			errorLimpieza = 1;
 		}

 		if(errorLimpieza)
 		{
 			return crearResultado(-1, "no_se_pudo_limpiar_archivo_de_prueba", "", __func__, nivel_de_profundidad);
 		}
 	}
 	return crearResultado(exito ? 1 : -1, exito ? "pruebas_ok" : "pruebas_fallaron", "", __func__, nivel_de_profundidad);
 }
/*
 * Agrega una línea al archivo de mensajes para todos.
 * Ejemplo: "Reunión a las 10" se guarda seguido de un salto de línea.
 */
 char * mandar_mensje_a_todos(const char * mensaje,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje == NULL || strlen(mensaje) == 0)
 	{
 		return crearResultado(-1, "mensaje_invalido", "", __func__, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_TODOS, "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int error = escribirLineaArchivo(archivo, mensaje) != 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
/*
 * Guarda un mensaje junto con su ID opcional y la lista de contactos.
 * Ejemplo: id=7, contactos="Ana,Luis", mensaje="Hola" produce
 * "[7] Ana,Luis -> Hola".
 */
 char * mandar_mensje_a_contacto(const char * mensaje,
 	const char * contactos,
 		int id_opcional,
 		int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje == NULL || strlen(mensaje) == 0)
 	{
 		return crearResultado(-1, "mensaje_invalido", "", __func__, nivel_de_profundidad);
 	}
 	if(contactos == NULL || strlen(contactos) == 0)
 	{
 		return crearResultado(-1, "contactos_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_CONTACTOS, "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
	int error =
		sistema_archivo_escribir_texto(archivo, "[") != 0 ||
		sistema_archivo_escribir_entero(archivo, id_opcional) != 0 ||
		sistema_archivo_escribir_texto(archivo, "] ") != 0 ||
		sistema_archivo_escribir_texto(archivo, contactos) != 0 ||
		sistema_archivo_escribir_texto(archivo, " -> ") != 0 ||
		sistema_archivo_escribir_texto(archivo, mensaje) != 0 ||
		sistema_archivo_escribir_caracter(archivo, '\n') != 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
/*
 * Registra pregunta, aviso de aceptación y respuesta final en una sola línea.
 * Ejemplo: "¿Quién?", "Yo voy", "Gracias" ->
 * "¿Quién? | Yo voy | Gracias".
 */
 char * mandar_mensje_al_primero_que_responda(const char * mensaje_pregunta,
 	const char * mensaje_de_que_ya_alguien_lo_acepto,
 		const char * menaje_respuesta_al_quien_lo_logro,
 			int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje_pregunta == NULL || strlen(mensaje_pregunta) == 0)
 	{
 		return crearResultado(-1, "mensaje_pregunta_invalido", "", __func__, nivel_de_profundidad);
 	}
 	if(mensaje_de_que_ya_alguien_lo_acepto == NULL || strlen(mensaje_de_que_ya_alguien_lo_acepto) == 0)
 	{
 		return crearResultado(-1, "mensaje_aceptado_invalido", "", __func__, nivel_de_profundidad);
 	}
 	if(menaje_respuesta_al_quien_lo_logro == NULL || strlen(menaje_respuesta_al_quien_lo_logro) == 0)
 	{
 		return crearResultado(-1, "mensaje_respuesta_invalido", "", __func__, nivel_de_profundidad);
 	}
 	SistemaArchivo *archivo = sistema_archivo_abrir(RUTA_MENSAJES_PRIMERO, "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int error =
 		sistema_archivo_escribir_texto(archivo, mensaje_pregunta) != 0 ||
 		sistema_archivo_escribir_texto(archivo, " | ") != 0 ||
 		sistema_archivo_escribir_texto(archivo, mensaje_de_que_ya_alguien_lo_acepto) != 0 ||
 		sistema_archivo_escribir_texto(archivo, " | ") != 0 ||
 		sistema_archivo_escribir_texto(archivo, menaje_respuesta_al_quien_lo_logro) != 0 ||
 		sistema_archivo_escribir_caracter(archivo, '\n') != 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
 #pragma endregion
 /* ============================================================================
    MEMORIA
    ============================================================================ */
 #pragma region "MEMORIA"
#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)

/*
 * Backend de escritorio: delega en el heap estándar.
 * Ejemplo de propiedad: reservar(20) devuelve un bloque de 20 bytes que debe
 * terminar en sistema_memoria_liberar() o redimensionarse con la misma capa.
 */
/* Solicita cantidad bytes; NULL indica que no se pudo reservar. */
 static void * sistema_memoria_reservar(size_t cantidad)
 {
 	return malloc(cantidad);
 }
/* Cambia el tamaño de un bloque existente; conserva su contenido si tiene éxito. */
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad)
 {
 	return realloc(memoria, cantidad);
 }
/* Libera un bloque obtenido con reservar/redimensionar; NULL es seguro en free(). */
 static void sistema_memoria_liberar(void * memoria)
 {
 	free(memoria);
 }

#elif defined(PLATAFORMA_SEMICONDUCTOR)

/*
 * Sustituir estas operaciones con el administrador fijo de memoria del PIC16F.
 * Hasta entonces, las reservas fallan explícitamente en esta plataforma.
 */
/* Stub deliberadamente fallido: aún no existe un pool físico configurado. */
static void * sistema_memoria_reservar(size_t cantidad)
{
	(void)cantidad;
	return NULL;
}

/* Stub de redimensionamiento; devuelve NULL hasta integrar el allocator embebido. */
static void * sistema_memoria_redimensionar(void *memoria, size_t cantidad)
{
	(void)memoria;
	(void)cantidad;
	return NULL;
}

/* Stub de liberación: no hay bloque asignado mientras reservar() falle. */
static void sistema_memoria_liberar(void *memoria)
{
	(void)memoria;
}

#endif
 #pragma endregion
 /* ============================================================================
    ARCHIVOS
    ============================================================================ */
 #pragma region "ARCHIVOS"
#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
 /*
  * Implementación escritorio del tipo opaco. FILE* queda confinado a esta capa;
  * el código común solo conserva y entrega SistemaArchivo*.
  */
  struct SistemaArchivo
 {
  	FILE *flujo;
  };

/*
 * Abre una ruta ("datos.txt") con modo estándar ("r", "a", "w").
 * Devuelve un manejador opaco o NULL; si falla la reserva del manejador,
 * cierra el FILE* para no dejar recursos abiertos.
 */
 static SistemaArchivo *sistema_archivo_abrir(const char *ruta, const char *modo)
{
 	if(ruta == NULL || modo == NULL) return NULL;
 	FILE *flujo = fopen(ruta, modo);
 	if(flujo == NULL) return NULL;

 	SistemaArchivo *archivo = sistema_memoria_reservar(sizeof(*archivo));
 	if(archivo == NULL)
 	{
 		fclose(flujo);
 		return NULL;
 	}
 	archivo->flujo = flujo;
 	return archivo;
 }

/* Cierra el flujo y libera el objeto; devuelve 0 si fclose tuvo éxito. */
 static int sistema_archivo_cerrar(SistemaArchivo *archivo)
 {
 	if(archivo == NULL) return -1;

 	int resultado = fclose(archivo->flujo);
 	sistema_memoria_liberar(archivo);
 	return resultado;
}

/* Elimina una ruta; por ejemplo, borra el temporal luego de una edición. */
static int sistema_archivo_eliminar(const char *ruta)
{
	return (ruta == NULL) ? -1 : remove(ruta);
}

/* Cambia el nombre de origen a destino; 0 significa que el backend lo logró. */
static int sistema_archivo_renombrar(const char *origen, const char *destino)
{
	return (origen == NULL || destino == NULL) ? -1 : rename(origen, destino);
}

/*
 * Lee un byte como int o devuelve SISTEMA_ARCHIVO_FIN_LECTURA al alcanzar EOF.
 * Esta conversión evita exponer el valor EOF al código común.
 */
static int sistema_archivo_leer_caracter(SistemaArchivo *archivo)
{
	if(archivo == NULL || archivo->flujo == NULL) return SISTEMA_ARCHIVO_FIN_LECTURA;

	int caracter = fgetc(archivo->flujo);
	return (caracter == EOF) ? SISTEMA_ARCHIVO_FIN_LECTURA : caracter;
}

/* Escribe un byte; 0=éxito, -1=flujo inválido o fallo de escritura. */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter)
{
	if(archivo == NULL || archivo->flujo == NULL) return -1;
	return (fputc(caracter, archivo->flujo) == EOF) ? -1 : 0;
}

/* Consulta el indicador de error del flujo; 0=sin error, distinto de 0=error. */
static int sistema_archivo_hay_error(SistemaArchivo *archivo)
{
	return (archivo == NULL || archivo->flujo == NULL) ? 1 : ferror(archivo->flujo);
}

/* Escribe todos los bytes de una cadena (sin su '\0'); 0=éxito, -1=fallo. */
static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto)
{
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
static SistemaArchivo *sistema_archivo_abrir(const char *ruta, const char *modo)
{
	(void)ruta;
	(void)modo;
	return NULL;
}

/* No hay manejador real que cerrar en el stub actual. */
static int sistema_archivo_cerrar(SistemaArchivo *archivo)
{
	(void)archivo;
	return -1;
}

/* El borrado requiere la política y soporte del medio que se seleccione. */
static int sistema_archivo_eliminar(const char *ruta)
{
	(void)ruta;
	return -1;
}

/* El renombrado debe implementarse según lo que permita el almacenamiento. */
static int sistema_archivo_renombrar(const char *origen, const char *destino)
{
	(void)origen;
	(void)destino;
	return -1;
}

/* Indica fin de entrada porque el stub no está conectado a un lector físico. */
static int sistema_archivo_leer_caracter(SistemaArchivo *archivo)
{
	(void)archivo;
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* El stub no acepta escrituras hasta contar con backend real. */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter)
{
	(void)archivo;
	(void)caracter;
	return -1;
}

/* El stub no persiste texto hasta contar con backend real. */
static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto)
{
	(void)archivo;
	(void)texto;
	return -1;
}

/* Mientras el backend no exista, toda consulta de error debe ser conservadora. */
static int sistema_archivo_hay_error(SistemaArchivo *archivo)
{
	(void)archivo;
	return 1;
}
#endif

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
/* Backends de consola de escritorio; aquí, y solo aquí, se usa stdio. */
static int sistema_consola_leer_caracter(void)
{
	int caracter = getchar();
	return caracter == EOF ? SISTEMA_ARCHIVO_FIN_LECTURA : caracter;
}

/* Escribe un carácter a salida estándar; 0=éxito y -1=fallo. */
static int sistema_consola_escribir_caracter(int caracter)
{
	return fputc(caracter, stdout) == EOF ? -1 : 0;
}

/* Escribe una cadena completa y vacía la salida para que los prompts aparezcan. */
static int sistema_consola_escribir_texto(const char *texto)
{
	if(texto == NULL || fputs(texto, stdout) == EOF || fflush(stdout) != 0) return -1;
	return 0;
}
#elif defined(PLATAFORMA_SEMICONDUCTOR)
/* Reemplazar con lectura del periférico elegido, por ejemplo UART. */
static int sistema_consola_leer_caracter(void)
{
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* Stub de salida de byte: no reporta éxito sin un periférico real. */
static int sistema_consola_escribir_caracter(int caracter)
{
	(void)caracter;
	return -1;
}

/* Stub de salida de texto; lo implementará el backend de consola PIC16F. */
static int sistema_consola_escribir_texto(const char *texto)
{
	(void)texto;
	return -1;
}
#endif

#pragma endregion