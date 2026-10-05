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
 #include <stdarg.h>     /* Argumentos variables */

 #include <stdio.h>      /* Entrada y salida estándar */

 #include <stdlib.h>     /* malloc, free */

 #include <string.h>     /* Manipulación de cadenas */

 #include <locale.h>     /* Configuración regional */

 #include <stddef.h>

 #include <time.h>

 #if defined(_WIN32) || defined(_WIN64)
 #define PLATAFORMA_WINDOWS

 #elif defined(__linux__)
 #define PLATAFORMA_LINUX
 #elif defined(SEMICONDUCTOR)
 #define PLATAFORMA_SEMICONDUCTOR
 /*
     Los microcontroladores tienen memoria limitada,
     por eso utilizaremos un buffer fijo.
 */
 #define CONCATENAR_BUFFER_SIZE 64
 /*PLATAFORMA NO SOPORTADA*/
 #else
 #error "Plataforma no soportada"
 #endif
 #define CONCATENAR_BUFFER_SIZE 64
 #define RESULTADO_BUFFER_SIZE 1024
 #define TAMANO_INICIAL_LINEA 16
 #define NOMBRE_ARCHIVO "datos.txt"
 /*
  * ============================================================================
  * CONFIGURACIÓN DE CONSOLA UTF-8
  * ============================================================================
  */
 #pragma region "CONFIGURACIÓN DE CONSOLA UTF-8"
 static void configurarConsolaUTF8(void)
 {
 	#if defined(PLATAFORMA_WINDOWS)
 	/*
 	 * Windows.
 	 * No incluimos windows.h para mantener esta base
 	 * lo más cercana posible a C estándar.
 	 * Si posteriormente necesitas una configuración
 	 * específica de consola de Windows, puede agregarse
 	 * aquí.
 	 */
 	setlocale(LC_ALL, "");
 	#elif defined(PLATAFORMA_LINUX)
 	/*
 	 * Linux.
 	 */
 	setlocale(LC_ALL, "");
 	#elif defined(PLATAFORMA_SEMICONDUCTOR)
 	/*
 	 * Semiconductor.
 	 * No suponemos que exista una consola.
 	 * Futuro:
 	 *      UART
 	 *      pantalla
 	 *      USB
 	 *      red
 	 *      SPI
 	 *      etc.
 	 */
 	setlocale(LC_ALL, "");
 	#endif
 }
 #pragma endregion
 /* ---------------------------------------------------------------------------
    CAPA DE MEMORIA , ARCHIVOS Y TIEMPO 
    --------------------------------------------------------------------------- */
 #pragma region "CAPA DE MEMORIA, ARCHIVOS Y TIEMPO"
 static void * sistema_memoria_reservar(size_t cantidad);
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad);
 static void sistema_memoria_liberar(void * memoria);
 static FILE * sistema_archivo_abrir(const char * ruta,const char * modo);
 static int sistema_archivo_cerrar(FILE * archivo);
 static int sistema_archivo_eliminar(const char * ruta);
 static int sistema_archivo_renombrar(const char * origen,const char * destino);
 static time_t sistema_tiempo_actual(void);
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE VARIABLES GLOBALES
 // ============================================================================
 #pragma region "FUNCIONES DECLARACIÓN VARIABLES GLOBALES"
 int GG_indice_donde_comensar = 1;
 char * GG_resultados_de_funciones = NULL;
 /*
  * Caracteres utilizados como separadores.
  *
  * IMPORTANTE:
  * Este archivo debe estar guardado en UTF-8.
  */
 const char * GG_caracter_separacion[] = {
 	"|",
 	"°",
 	"¬",
 	"╦",
 	"╝",
 	"╔"
 };
 const char * GG_caracter_separacion_funciones_espesificas[] = {
 	"~",
 	"§",
 	"¶",
 	"╬"
 };
 const char * GG_caracter_para_confirmacion_o_error[] = {
 	"╣",
 	"╠",
 	"⚺",
 	"⚻",
 	"⚼"
 };
 const char * GG_caracter_para_transferencia_entre_archivos[] = {
 	"┴",
 	"■"
 };
 const char * GG_caracter_usadas_por_usuario[] = {
 	":",
 	"#",
 	"&"
 };
 const char * GG_caracter_para_usar_como_enter_y_nuevo_mensaje[] = {
 	"•",
 	"∆"
 };
 const char * GG_id_programa = "QU1R30N_SISTEMA_DEFINITIVO";
 const char * GG_direccion_control_errores_try = "config\\chatbot\\errores_try\\control_errore.txt";
 const char * G_dir_arch_transferencia[] = {
 	/* 0 */
 	"C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\BANDERAS_sis_qu1.TXT",
 	/* 1 - preguntas */
 	"C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\ent_sis_qu1.TXT",
 	/* 2 - respuestas */
 	"C:\\XEROX\\CONFIG\\INF\\QU1R30N_SISTEMA_DEFINITIVO\\sal_sis_qu1.TXT"
 };
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES TEX_BASE
 // ============================================================================
 #pragma region "FUNCIONES TEX_BASE"
 static char * leerLineaDinamica(FILE * flujo, int nivel_de_profundidad);
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
 #pragma region "FUNCIONES MENSAJERÍA"
 char * mandar_mensje_a_todos(const char * mensaje,int nivel_de_profundidad);
 char * mandar_mensje_a_contacto(const char * mensaje,const char * contactos,int id_opcional,int nivel_de_profundidad);
 char * mandar_mensje_al_primero_que_responda(const char * mensaje_pregunta,const char * mensaje_de_que_ya_alguien_lo_acepto,const char * menaje_respuesta_al_quien_lo_logro,int nivel_de_profundidad);
 char * checar_si_hay_mensajes_no_leido(int nivel_de_profundidad);
 #pragma endregion
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE OPERACIONES DE TEXTO
 // ============================================================================
 #pragma region "FUNCIONES OPERACIONES DE TEXTO"
 char ** split(const char * texto,const char * delimitador,int * cantidad,int nivel_de_profundidad);
 void liberarSplit(char ** partes, int cantidad, int nivel_de_profundidad);
 char * join(char ** arreglo, int cantidad,const char * carcter_separacion,int nivel_de_profundidad);
 char * crearResultado(int codigo,const char * informacion,const char * resultado_anterior,const char * funcion_llamante,int nivel_de_profundidad);
 static int concat(char * destino, size_t capacidad,const char * separador,const char * formato, ...);
 #pragma endregion
 // ============================================================================
 // MAIN
 // ============================================================================
 int main(void)
 {
 	configurarConsolaUTF8();
 	printf("%s\n", crearResultado(200, "prueba", NULL, "main", 2));
 	printf("%s\n", crearResultado(200, "prueba", NULL, "main", 2));
 	printf("%s\n", crearResultado(200, "prueba", NULL, "main", 2));
 	int opcion = 0;
 	char * resultado = NULL;
 	do {
 		printf("\n=== MENÚ PRINCIPAL ===\n");
 		printf("1. comandos_tex_base\n");
 		printf("2. enlasador_mandar_mensajes\n");
 		printf("3. operaciones_de_texto\n");
 		printf("4. Salir\n");
 		printf("Seleccione una opción: ");
 		char * optStr = leerLineaDinamica(stdin, 1);
 		if(optStr == NULL)
 		{
 			opcion = 4;
 			free(resultado);
 			resultado = crearResultado(0, "entrada_finalizada", "", "main", 1);
 			break;
 		}
 		opcion = atoi(optStr);
 		free(optStr);
 		switch(opcion)
 		{
 			case 1:
 			{
 				char * parametros = "l";
 				free(resultado);
 				resultado = submenu_tex_base(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				printf("%s\n", resultadoMain);
 				free(resultadoMain);
 				break;
 			}
 			case 2:
 			{
 				char * parametros = "mandar_todos,mandar_contacto,mandar_primero";
 				free(resultado);
 				resultado = submenu_enlasador_mandar_mensajes(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				printf("%s\n", resultadoMain);
 				free(resultadoMain);
 				break;
 			}
 			case 3:
 			{
 				char * parametros = "split,modificar_columna,leer_linea";
 				free(resultado);
 				resultado = submenu_operaciones_de_texto(parametros, 1);
 				char * resultadoMain = crearResultado(1, "", "1", __func__, 1);
 				printf("%s\n", resultadoMain);
 				free(resultadoMain);
 				break;
 			}
 			case 4:
 			{
 				printf("Saliendo...\n");
 				free(resultado);
 				resultado = crearResultado(0, "salida_ok", "", "main", 1);
 				printf("%s\n", resultado);
 				break;
 			}
 			default:
 			{
 				printf("Opción no válida.\n");
 				free(resultado);
 				resultado = crearResultado(-2, "opcion_no_valida", "", "main", 1);
 				printf("%s\n", resultado);
 				break;
 			}
 		}
 	} while(opcion != 4);
 	int codigoFinal = (resultado != NULL) ? atoi(resultado) : -1;
 	free(resultado);
 	return codigoFinal;
 }
 // ============================================================================
 // FUNCIONES SUBMENÚS
 // ============================================================================
 #pragma region "FUNCIONES SUBMENÚS"
 char * submenu_tex_base(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	int opcion = 0;
 	int idLinea = 0;
 	int idColumna = 0;
 	char * resultado = NULL;
 	char * estado = NULL;
 	int cantidad = 0;
 	char ** parametros_espliteados = NULL;
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		printf("[split tex_base] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			printf("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
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
 	char * optStr = leerLineaDinamica(stdin, nivel_de_profundidad);
 	opcion = (optStr != NULL) ? atoi(optStr) : 0;
 	free(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			free(resultado);
 			resultado = leerArchivo(NOMBRE_ARCHIVO, nivel_de_profundidad);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			printf("Ingrese el texto/línea a añadir: ");
 			char * texto = leerLineaDinamica(stdin, nivel_de_profundidad);
 			free(resultado);
 			resultado = escribirLinea(NOMBRE_ARCHIVO, texto, nivel_de_profundidad);
 			free(texto);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			printf("Ingrese ID de línea a editar: ");
 			char * inId = leerLineaDinamica(stdin, nivel_de_profundidad);
 			idLinea = (inId != NULL) ? atoi(inId) : 0;
 			free(inId);
 			printf("Ingrese el nuevo contenido completo: ");
 			char * texto = leerLineaDinamica(stdin, nivel_de_profundidad);
 			free(resultado);
 			resultado = editarLinea(NOMBRE_ARCHIVO, idLinea, texto, nivel_de_profundidad);
 			free(texto);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			printf("Ingrese ID de línea a editar: ");
 			char * inId = leerLineaDinamica(stdin, nivel_de_profundidad);
 			idLinea = (inId != NULL) ? atoi(inId) : 0;
 			free(inId);
 			printf("Ingrese el número de columna a editar (1, 2, ...): ");
 			char * inCol = leerLineaDinamica(stdin, nivel_de_profundidad);
 			idColumna = (inCol != NULL) ? atoi(inCol) : 0;
 			free(inCol);
 			printf("Ingrese el nuevo valor para esa columna: ");
 			char * valor = leerLineaDinamica(stdin, nivel_de_profundidad);
 			free(resultado);
 			resultado = editarColumna(NOMBRE_ARCHIVO, idLinea, idColumna, valor, nivel_de_profundidad);
 			free(valor);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 5:
 		{
 			printf("Ingrese ID de línea a eliminar: ");
 			char * inId = leerLineaDinamica(stdin, nivel_de_profundidad);
 			idLinea = (inId != NULL) ? atoi(inId) : 0;
 			free(inId);
 			free(resultado);
 			resultado = eliminarLinea(NOMBRE_ARCHIVO, idLinea, nivel_de_profundidad);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 6:
 		{
 			printf("Ingrese ID de línea a vaciar: ");
 			char * inId = leerLineaDinamica(stdin, nivel_de_profundidad);
 			idLinea = (inId != NULL) ? atoi(inId) : 0;
 			free(inId);
 			free(resultado);
 			resultado = vaciarLinea(NOMBRE_ARCHIVO, idLinea, nivel_de_profundidad);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 7:
 		{
 			free(resultado);
 			resultado = ejecutarEjemplosPrueba(nivel_de_profundidad);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 8:
 		{
 			printf("Volviendo al menú principal...\n");
 			free(estado);
 			free(resultado);
 			return crearResultado(1, "informacionMain", "1", __func__, 1);
 		}
 		default:
 		{
 			printf("Opción no válida.\n");
 			free(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		printf("%s\n", estado);
 	}
 	free(estado);
 	free(resultado);
 	return crearResultado(1, "informacionMain", "1", __func__, 1);
 }
 char * submenu_enlasador_mandar_mensajes(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	int opcion = 0;
 	char * resultado = NULL;
 	char * estado = NULL;
 	int cantidad = 0;
 	char ** parametros_espliteados = NULL;
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		printf("[split mensajes] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			printf("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
 	printf("\n=== SUBMENÚ enlasador_mandar_mensajes ===\n");
 	printf("1. mandar_mensje_a_todos(mensaje)\n");
 	printf("2. mandar_mensje_a_contacto(mensaje, contactos, id_opcional)\n");
 	printf("3. mandar_mensje_al_primero_que_responda("
 		"mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto, "
 		"menaje_respuesta_al_quien_lo_logro)\n");
 	printf("4. Volver al menú principal\n");
 	printf("Seleccione una opción: ");
 	char * optStr = leerLineaDinamica(stdin, nivel_de_profundidad);
 	opcion = (optStr != NULL) ? atoi(optStr) : 0;
 	free(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			printf("Ingrese el mensaje para todos: ");
 			char * mensaje = leerLineaDinamica(stdin, nivel_de_profundidad);
 			free(resultado);
 			resultado = mandar_mensje_a_todos(mensaje, nivel_de_profundidad);
 			free(mensaje);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			printf("Ingrese el mensaje: ");
 			char * mensaje = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese la lista de contactos: ");
 			char * contactos = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese id opcional: ");
 			char * idStr = leerLineaDinamica(stdin, nivel_de_profundidad);
 			int id_opcional = (idStr != NULL) ? atoi(idStr) : 0;
 			free(idStr);
 			free(resultado);
 			resultado = mandar_mensje_a_contacto(mensaje, contactos, id_opcional, nivel_de_profundidad);
 			free(mensaje);
 			free(contactos);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			printf("Ingrese el mensaje de pregunta: ");
 			char * pregunta = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese el mensaje de que alguien ya lo aceptó: ");
 			char * aceptado = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese la respuesta a quien lo logró: ");
 			char * respuesta = leerLineaDinamica(stdin, nivel_de_profundidad);
 			free(resultado);
 			resultado = mandar_mensje_al_primero_que_responda(pregunta, aceptado, respuesta, nivel_de_profundidad);
 			free(pregunta);
 			free(aceptado);
 			free(respuesta);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			printf("Volviendo al menú principal...\n");
 			free(estado);
 			free(resultado);
 			return crearResultado(1, "informacionMain", "1", __func__, 1);
 		}
 		default:
 		{
 			printf("Opción no válida.\n");
 			free(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		printf("%s\n", estado);
 	}
 	free(estado);
 	free(resultado);
 	return crearResultado(1, "informacionMain", "1", __func__, 1);
 }
 char * submenu_operaciones_de_texto(char * parametros_en_texto_a_splitear, int nivel_de_profundidad)
 {
 	int opcion = 0;
 	int cantidad = 0;
 	char ** parametros_espliteados = NULL;
 	char * estado = NULL;
 	if(parametros_en_texto_a_splitear != NULL)
 	{
 		parametros_espliteados = split(parametros_en_texto_a_splitear, ",", & cantidad, nivel_de_profundidad);
 		printf("[split operaciones_texto] elementos: %d\n", cantidad);
 		for(int i = 0; i < cantidad; i++)
 		{
 			printf("  [%d] %s\n", i, parametros_espliteados[i]);
 		}
 		liberarSplit(parametros_espliteados, cantidad, nivel_de_profundidad);
 	}
 	printf("\n=== SUBMENÚ operaciones_de_texto ===\n");
 	printf("1. split(texto, delimitador)\n");
 	printf("2. modificarColumna(linea, columna, nuevoValor)\n");
 	printf("3. leerLineaDinamica(FILE*)\n");
 	printf("4. Volver al menú principal\n");
 	printf("Seleccione una opción: ");
 	char * optStr = leerLineaDinamica(stdin, nivel_de_profundidad);
 	opcion = (optStr != NULL) ? atoi(optStr) : 0;
 	free(optStr);
 	switch(opcion)
 	{
 		case 1:
 		{
 			printf("Ingrese el texto a partir: ");
 			char * texto = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese el delimitador: ");
 			char * delimitador = leerLineaDinamica(stdin, nivel_de_profundidad);
 			int total = 0;
 			char ** partes = split(texto, delimitador, & total, nivel_de_profundidad);
 			if(partes == NULL)
 			{
 				free(texto);
 				free(delimitador);
 				free(estado);
 				estado = crearResultado(-1, "error_split", "", __func__, 1);
 				break;
 			}
 			for(int i = 0; i < total; i++)
 			{
 				printf("  parte[%d] = %s\n", i, partes[i]);
 			}
 			liberarSplit(partes, total, nivel_de_profundidad);
 			free(texto);
 			free(delimitador);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 2:
 		{
 			printf("Ingrese la línea original: ");
 			char * linea = leerLineaDinamica(stdin, nivel_de_profundidad);
 			printf("Ingrese la columna a cambiar: ");
 			char * colStr = leerLineaDinamica(stdin, nivel_de_profundidad);
 			int columna = (colStr != NULL) ? atoi(colStr) : 0;
 			free(colStr);
 			printf("Ingrese el nuevo valor: ");
 			char * nuevo = leerLineaDinamica(stdin, nivel_de_profundidad);
 			char * resultadoMod = modificarColumna(linea, columna, nuevo, nivel_de_profundidad);
 			if(resultadoMod == NULL)
 			{
 				free(linea);
 				free(nuevo);
 				free(estado);
 				estado = crearResultado(-1, "error_modificar_columna", "", __func__, 1);
 				break;
 			}
 			printf("Resultado: %s\n", resultadoMod);
 			free(resultadoMod);
 			free(linea);
 			free(nuevo);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 3:
 		{
 			printf("Ingrese una línea de texto: ");
 			char * linea = leerLineaDinamica(stdin, nivel_de_profundidad);
 			if(linea == NULL)
 			{
 				free(estado);
 				estado = crearResultado(-1, "error_lectura", "", __func__, 1);
 				break;
 			}
 			printf("Línea recibida: %s\n", linea);
 			free(linea);
 			free(estado);
 			estado = crearResultado(1, "informacionMain", "1", __func__, 1);
 			break;
 		}
 		case 4:
 		{
 			printf("Volviendo al menú principal...\n");
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
 			printf("Opción no válida.\n");
 			free(estado);
 			estado = crearResultado(-2, "opcion_no_valida", "", __func__, 1);
 			break;
 		}
 	}
 	if(estado != NULL)
 	{
 		printf("%s\n", estado);
 	}
 	free(estado);
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
 char ** split(const char *texto, const char *separador, int *cantidad, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
 	if(cantidad == NULL) return NULL;
 	*cantidad = 0;
 	if(texto == NULL || separador == NULL || separador[0] == '\0') return NULL;

 	size_t longitud_texto = strlen(texto);
 	size_t longitud_separador = strlen(separador);
 	size_t capacidad = 8;
 	size_t contador = 0;
 	size_t inicio = 0;
 	size_t posicion = 0;
 	char **partes = sistema_memoria_reservar(capacidad * sizeof(*partes));
 	if(partes == NULL) return NULL;

 	while(posicion <= longitud_texto)
 	{
 		int encontrado = posicion + longitud_separador <= longitud_texto &&
 			strncmp(texto + posicion, separador, longitud_separador) == 0;
 		if(!encontrado && posicion < longitud_texto)
 		{
 			posicion++;
 			continue;
 		}

 		if(contador == capacidad - 1)
 		{
 			if(capacidad > (size_t)-1 / 2 / sizeof(*partes))
 			{
 				liberarSplit(partes, (int)contador, nivel_de_profundidad);
 				return NULL;
 			}
 			size_t nueva_capacidad = capacidad * 2;
 			char **temporal = sistema_memoria_redimensionar(partes, nueva_capacidad * sizeof(*partes));
 			if(temporal == NULL)
 			{
 				liberarSplit(partes, (int)contador, nivel_de_profundidad);
 				return NULL;
 			}
 			partes = temporal;
 			capacidad = nueva_capacidad;
 		}

 		size_t longitud_parte = posicion - inicio;
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

 void liberarSplit(char **partes, int cantidad, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
 	if(partes == NULL) return;
 	for(int i = 0; i < cantidad; i++) sistema_memoria_liberar(partes[i]);
 	sistema_memoria_liberar(partes);
 }
 char * join(char **arreglo, int cantidad, const char *carcter_separacion, int nivel_de_profundidad)
 {
 	(void)nivel_de_profundidad;
 	const char *separador = (carcter_separacion != NULL) ? carcter_separacion : "";
 	size_t longitud_total = 1;
 	size_t longitud_separador = strlen(separador);

 	if(cantidad < 0 || (cantidad > 0 && arreglo == NULL))
 	{
 		return NULL;
 	}

 	for(int i = 0; i < cantidad; i++)
 	{
 		longitud_total += (arreglo[i] != NULL) ? strlen(arreglo[i]) : 0;
 		if(i > 0) longitud_total += longitud_separador;
 	}

 	char *resultado = sistema_memoria_reservar(longitud_total);
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
 char * crearResultado(int codigo,
 	const char * informacion,
 		const char * resultado_anterior,
 			const char * funcion_llamante,
 				int nivel_de_profundidad)
 {
 	const char *separador = GG_caracter_separacion[0];
 	const char *info = (informacion != NULL) ? informacion : "";
 	const char *anterior = (resultado_anterior != NULL) ? resultado_anterior : "";
 	const char *funcion = (funcion_llamante != NULL) ? funcion_llamante : "";
 	int longitud = snprintf(
 		NULL,
 		0,
 		"%d%s%s%s%s%s%s%s%d",
 		codigo,
 		separador,
 		info,
 		separador,
 		anterior,
 		separador,
 		funcion,
 		separador,
 		nivel_de_profundidad
 	);
 	if(longitud < 0) return NULL;

 	char *resultado = sistema_memoria_reservar((size_t)longitud + 1);
 	if(resultado == NULL) return NULL;

 	if(snprintf(
 			resultado,
 			(size_t)longitud + 1,
 			"%d%s%s%s%s%s%s%s%d",
 			codigo,
 			separador,
 			info,
 			separador,
 			anterior,
 			separador,
 			funcion,
 			separador,
 			nivel_de_profundidad
 		) != longitud)
 	{
 		sistema_memoria_liberar(resultado);
 		return NULL;
 	}

 	return resultado;
 }
 
 static char *concatenar(
    const char *formato,
    ...
)
{
    va_list argumentos;
    va_list copia;
    int longitud;
    char *resultado;


    /*
     * Validamos.
     */
    if (formato == NULL)
    {
        return NULL;
    }


    /*
     * Iniciamos argumentos.
     */
    va_start(argumentos, formato);


    /*
     * Hacemos una copia.
     */
    va_copy(copia, argumentos);


    /*
     * Calculamos el tamaño.
     *
     * Si la implementación de C del microcontrolador
     * no soporta esta modalidad de vsnprintf(),
     * esta función puede sustituirse posteriormente
     * por el formateador propio del sistema.
     */
    longitud = vsnprintf(
        NULL,
        0,
        formato,
        copia
    );


    va_end(copia);


    /*
     * Error de formato.
     */
    if (longitud < 0)
    {
        va_end(argumentos);
        return NULL;
    }


    /*
     * Reservamos espacio.
     */
    resultado =
        (char *)sistema_memoria_reservar(
            (size_t)longitud + 1
        );


    if (resultado == NULL)
    {
        va_end(argumentos);
        return NULL;
    }


    /*
     * Escribimos.
     */
    vsnprintf(
        resultado,
        (size_t)longitud + 1,
        formato,
        argumentos
    );


    /*
     * Finalizamos argumentos.
     */
    va_end(argumentos);


    return resultado;
}


/* ============================================================================
   CONCAT
   ============================================================================
 *
 * Agrega texto a un buffer existente respetando su capacidad.
 *
 * Retorna:
 *
 *      0   = correcto
 *     -1   = error
 *
 * ============================================================================
 */

static int concat(
    char *destino,
    size_t capacidad,
    const char *separador,
    const char *formato,
    ...
)
{
    va_list argumentos;
    size_t posicion;
    int resultadoFormato;
    size_t restante;


    /*
     * Validamos.
     */
    if (destino == NULL ||
        capacidad == 0 ||
        formato == NULL)
    {
        return -1;
    }


    /*
     * Buscamos dónde termina el contenido actual.
     */
    posicion = strlen(destino);


    /*
     * Evitamos que strlen() haya encontrado
     * una cadena fuera del buffer.
     */
    if (posicion >= capacidad)
    {
        return -1;
    }


    /*
     * Agregamos separador si existe.
     */
    if (separador != NULL &&
        separador[0] != '\0')
    {
        size_t longitudSeparador =
            strlen(separador);

        restante = capacidad - posicion;

        if (longitudSeparador + 1 > restante)
        {
            return -1;
        }


        memcpy(
            destino + posicion,
            separador,
            longitudSeparador
        );

        posicion += longitudSeparador;

        destino[posicion] = '\0';
    }


    /*
     * Calculamos espacio restante.
     */
    restante = capacidad - posicion;


    /*
     * Argumentos.
     */
    va_start(argumentos, formato);


    /*
     * Formateamos directamente en el buffer.
     */
    resultadoFormato = vsnprintf(
        destino + posicion,
        restante,
        formato,
        argumentos
    );


    /*
     * Cerramos argumentos.
     */
    va_end(argumentos);


    /*
     * Error.
     */
    if (resultadoFormato < 0)
    {
        return -1;
    }


    /*
     * El resultado no cabe.
     */
    if ((size_t)resultadoFormato >= restante)
    {
        return -1;
    }


    return 0;
}



 
 #pragma endregion
 // ============================================================================
 // FUNCIONES OPERACIONES DE TEX_BASE
 // ============================================================================
 #pragma region "FUNCIONES OPERACIONES_DE_TEX_BASE"
 /* ============================================================================
   LEER LINEA DINAMICA
   ============================================================================ */

static char *leerLineaDinamica(
    FILE *flujo,
    int nivel_de_profundidad
)
{
    char *linea;
    size_t capacidad;
    size_t longitud;
    int caracter;


    /*
     * Actualmente el nivel se reserva para
     * el control interno del sistema.
     */
    (void)nivel_de_profundidad;


    /*
     * Validamos.
     */
    if (flujo == NULL)
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
    while ((caracter = fgetc(flujo)) != EOF)
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
     * Si no leímos nada y encontramos EOF,
     * liberamos.
     */
    if (longitud == 0 &&
        caracter == EOF)
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
char * modificarColumna(const char * lineaOriginal,
 	int columnaTarget,
 	const char * nuevoValor,
 		int nivel_de_profundidad)
 {
 	if(lineaOriginal == NULL || nuevoValor == NULL || columnaTarget < 1) return NULL;

 	int cantidad = 0;
 	char **partes = split(lineaOriginal, ",", &cantidad, nivel_de_profundidad);
 	if(partes == NULL) return NULL;
 	if(columnaTarget > cantidad)
 	{
 		liberarSplit(partes, cantidad, nivel_de_profundidad);
 		return NULL;
 	}

 	size_t longitud = strlen(nuevoValor) + 1;
 	sistema_memoria_liberar(partes[columnaTarget - 1]);
 	partes[columnaTarget - 1] = sistema_memoria_reservar(longitud);
 	if(partes[columnaTarget - 1] == NULL)
 	{
 		liberarSplit(partes, cantidad, nivel_de_profundidad);
 		return NULL;
 	}
 	memcpy(partes[columnaTarget - 1], nuevoValor, longitud);

 	char *resultado = join(partes, cantidad, ",", nivel_de_profundidad);
 	liberarSplit(partes, cantidad, nivel_de_profundidad);
 	return resultado;
 }

static int reemplazarArchivoTemporal(const char *ruta)
{
	const char *temporal = "temp.txt";
	const char *respaldo = "temp_qu1ron.bak";
	FILE *existente;

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

static int aplicarOperacionLinea(
	const char *ruta,
	int numeroLinea,
	int operacion,
	const char *contenido,
	int columna,
	int nivel_de_profundidad
)
{
	FILE *archivo;
	FILE *temporal;
	char *linea;
	int encontrada = 0;
	int error = 0;
	int actual = 1;

	if(ruta == NULL || numeroLinea < 1 || (operacion == 0 && contenido == NULL) ||
		(operacion == 3 && (contenido == NULL || columna < 1)) ||
		strcmp(ruta, "temp.txt") == 0 || strcmp(ruta, "temp_qu1ron.bak") == 0)
	{
		return -1;
	}

	archivo = sistema_archivo_abrir(ruta, "r");
	if(archivo == NULL) return -1;

	FILE *temporalExistente = sistema_archivo_abrir("temp.txt", "r");
	if(temporalExistente != NULL)
	{
		sistema_archivo_cerrar(temporalExistente);
		sistema_archivo_cerrar(archivo);
		return -1;
	}

	temporal = sistema_archivo_abrir("temp.txt", "w");
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
				if(fprintf(temporal, "%s\n", contenido) < 0) error = 1;
			}
			else if(operacion == 2)
			{
				if(fputc('\n', temporal) == EOF) error = 1;
			}
			else if(operacion == 3)
			{
				char *modificada = modificarColumna(linea, columna, contenido, nivel_de_profundidad);
				if(modificada == NULL || fprintf(temporal, "%s\n", modificada) < 0) error = 1;
				sistema_memoria_liberar(modificada);
			}
		}
		else if(fprintf(temporal, "%s\n", linea) < 0)
		{
			error = 1;
		}

		sistema_memoria_liberar(linea);
		actual++;
	}

	if(ferror(archivo)) error = 1;
	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
	if(sistema_archivo_cerrar(temporal) != 0) error = 1;

	if(error || !encontrada)
	{
		sistema_archivo_eliminar("temp.txt");
		return error ? -1 : -2;
	}

	if(reemplazarArchivoTemporal(ruta) != 0)
	{
		sistema_archivo_eliminar("temp.txt");
		return -1;
	}
	return 0;
}

 char * leerArchivo(const char * ruta,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL)
 	{
 		return crearResultado(-1, "ruta_invalida", "", __func__, nivel_de_profundidad);
 	}

 	FILE *archivo = sistema_archivo_abrir(ruta, "r");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}

 	int numeroLinea = 1;
 	char *linea;
 	printf("\n--- CONTENIDO DE [%s] ---\n", ruta);
 	while((linea = leerLineaDinamica(archivo, nivel_de_profundidad)) != NULL)
 	{
 		printf("%d: %s\n", numeroLinea++, linea);
 		sistema_memoria_liberar(linea);
 	}

 	int errorLectura = ferror(archivo);
 	int errorCierre = sistema_archivo_cerrar(archivo);
 	printf("-----------------------------------\n");
 	if(errorLectura || errorCierre != 0)
 	{
 		return crearResultado(-1, "error_al_leer_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "lectura_ok", "", __func__, nivel_de_profundidad);
 }
 char * escribirLinea(const char * ruta,
 	const char * nuevaLinea,
 		int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || nuevaLinea == NULL || strcmp(ruta, "temp.txt") == 0 ||
 		strcmp(ruta, "temp_qu1ron.bak") == 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	FILE *archivo = sistema_archivo_abrir(ruta, "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int errorEscritura = fprintf(archivo, "%s\n", nuevaLinea) < 0;
 	if(sistema_archivo_cerrar(archivo) != 0) errorEscritura = 1;
 	if(errorEscritura)
 	{
 		return crearResultado(-1, "error_al_escribir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(1, "escritura_ok", "", __func__, nivel_de_profundidad);
 }
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
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 0, nuevoTexto, 0, nivel_de_profundidad);
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
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 3, nuevoValor, idColumna, nivel_de_profundidad);
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
 char * eliminarLinea(const char * ruta,
 	int idLinea,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 1, NULL, 0, nivel_de_profundidad);
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
 char * vaciarLinea(const char * ruta,
 	int idLinea,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(ruta == NULL || idLinea <= 0)
 	{
 		return crearResultado(-1, "parametros_invalidos", "", __func__, nivel_de_profundidad);
 	}
 	int resultado = aplicarOperacionLinea(ruta, idLinea, 2, NULL, 0, nivel_de_profundidad);
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
 char * checar_si_hay_mensajes_no_leido(int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	const char * rutas[] = {
 		"mensajes_todos.txt",
 		"mensajes_contactos.txt",
 		"mensajes_primero.txt"
 	};
 	for(size_t i = 0; i < sizeof(rutas) / sizeof(rutas[0]); i++)
 	{
 		FILE *archivo = sistema_archivo_abrir(rutas[i], "r");
 		if(archivo == NULL)
 		{
 			continue;
 		}
 		int caracter = fgetc(archivo);
 		int errorLectura = ferror(archivo);
 		int errorCierre = sistema_archivo_cerrar(archivo);
 		if(errorLectura || errorCierre != 0)
 		{
 			return crearResultado(-1, "error_al_consultar_mensajes", "", __func__, nivel_de_profundidad);
 		}
 		if(caracter != EOF)
 		{
 			return crearResultado(1, "hay_mensajes_no_leidos", "", __func__, nivel_de_profundidad);
 		}
 	}
 	return crearResultado(0, "no_hay_mensajes", "", __func__, nivel_de_profundidad);
 }
 char * ejecutarEjemplosPrueba(int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	const char *rutaPrueba = "qu1ron_ejemplos.tmp";
 	FILE *existente = sistema_archivo_abrir(rutaPrueba, "r");
 	if(existente != NULL)
 	{
 		sistema_archivo_cerrar(existente);
 		return crearResultado(-1, "archivo_de_prueba_ya_existe", "", __func__, nivel_de_profundidad);
 	}

 	int exito = 0;
 	char *lineaModificada = modificarColumna("Ana,25,Programador", 2, "30", nivel_de_profundidad);
 	if(lineaModificada == NULL || strcmp(lineaModificada, "Ana,30,Programador") != 0)
 	{
 		sistema_memoria_liberar(lineaModificada);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(lineaModificada);

 	char *resultadoOperacion = escribirLinea(rutaPrueba, "Luis,10,Desarrollador", nivel_de_profundidad);
 	if(resultadoOperacion == NULL || atoi(resultadoOperacion) < 0)
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = escribirLinea(rutaPrueba, "Marta,20,QA", nivel_de_profundidad);
 	if(resultadoOperacion == NULL || atoi(resultadoOperacion) < 0)
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = editarColumna(rutaPrueba, 1, 2, "15", nivel_de_profundidad);
 	if(resultadoOperacion == NULL || atoi(resultadoOperacion) < 0)
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	resultadoOperacion = eliminarLinea(rutaPrueba, 2, nivel_de_profundidad);
 	if(resultadoOperacion == NULL || atoi(resultadoOperacion) < 0)
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);

 	printf("\n--- ARCHIVO DE PRUEBA RESULTANTE ---\n");
 	resultadoOperacion = leerArchivo(rutaPrueba, nivel_de_profundidad);
 	if(resultadoOperacion == NULL || atoi(resultadoOperacion) < 0)
 	{
 		sistema_memoria_liberar(resultadoOperacion);
 		goto limpieza;
 	}
 	sistema_memoria_liberar(resultadoOperacion);
 	exito = 1;

 limpieza:
 	if(sistema_archivo_eliminar(rutaPrueba) != 0 && exito)
 	{
 		return crearResultado(-1, "no_se_pudo_limpiar_archivo_de_prueba", "", __func__, nivel_de_profundidad);
 	}
 	return crearResultado(exito ? 1 : -1, exito ? "pruebas_ok" : "pruebas_fallaron", "", __func__, nivel_de_profundidad);
 }
 char * mandar_mensje_a_todos(const char * mensaje,
 	int nivel_de_profundidad)
 {
 	nivel_de_profundidad++;
 	if(mensaje == NULL || strlen(mensaje) == 0)
 	{
 		return crearResultado(-1, "mensaje_invalido", "", __func__, nivel_de_profundidad);
 	}
 	FILE *archivo = sistema_archivo_abrir("mensajes_todos.txt", "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int error = fprintf(archivo, "%s\n", mensaje) < 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
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
 	FILE *archivo = sistema_archivo_abrir("mensajes_contactos.txt", "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int error = fprintf(archivo, "[%d] %s -> %s\n", id_opcional, contactos, mensaje) < 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
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
 	FILE *archivo = sistema_archivo_abrir("mensajes_primero.txt", "a");
 	if(archivo == NULL)
 	{
 		return crearResultado(-1, "no_se_pudo_abrir_archivo", "", __func__, nivel_de_profundidad);
 	}
 	int error = fprintf(
 		archivo,
 		"%s | %s | %s\n",
 		mensaje_pregunta,
 		mensaje_de_que_ya_alguien_lo_acepto,
 		menaje_respuesta_al_quien_lo_logro
 	) < 0;
 	if(sistema_archivo_cerrar(archivo) != 0) error = 1;
 	if(error) return crearResultado(-1, "error_al_guardar_mensaje", "", __func__, nivel_de_profundidad);
 	return crearResultado(1, "mensaje_enviado", "", __func__, nivel_de_profundidad);
 }
 #pragma endregion
 /* ============================================================================
    MEMORIA
    ============================================================================ */
 #pragma region "MEMORIA"
 static void * sistema_memoria_reservar(size_t cantidad)
 {
 	/*
 	 * Por ahora utilizamos malloc().
 	 * FUTURO:
 	 * En un semiconductor puedes cambiar esto por:
 	 *      arena de memoria
 	 *      pool
 	 *      memoria estática
 	 *      allocator propio
 	 *      memoria RAM del microcontrolador
 	 * La lógica del programa no tendrá que cambiar.
 	 */
 	return malloc(cantidad);
 }
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad)
 {
 	/*
 	 * Por ahora utilizamos realloc().
 	 * FUTURO:
 	 * Se puede sustituir por tu propio administrador
 	 * de memoria.
 	 */
 	return realloc(memoria, cantidad);
 }
 static void sistema_memoria_liberar(void * memoria)
 {
 	/*
 	 * Por ahora utilizamos free().
 	 * FUTURO:
 	 * Se puede sustituir por:
 	 *      core_memory_free()
 	 *      arena_free()
 	 *      pool_free()
 	 * etc.
 	 */
 	free(memoria);
 }
 #pragma endregion
 /* ============================================================================
    ARCHIVOS
    ============================================================================ */
 #pragma region "ARCHIVOS"
 static FILE * sistema_archivo_abrir(const char * ruta,const char * modo)
 {
 		/*Windows / Linux:
      fopen()
 
      Semiconductor:
      FUTURO:

      esta función puede convertirse en acceso a:

          Flash
          EEPROM
          SD
          memoria externa
          sistema de archivos embebido

        Por ahora utilizamos FILE para conservar
        la estructura actual del programa.
        */

        return fopen(ruta, modo);
    }

static int sistema_archivo_cerrar(FILE *archivo)
{
    if (archivo == NULL)
    {
        return -1;
    }

    return fclose(archivo);
}

static int sistema_archivo_eliminar(const char *ruta)
{
    if (ruta == NULL)
    {
        return -1;
    }

    return remove(ruta);
}
static int sistema_archivo_renombrar(const char *origen, const char *destino)
{
    if (origen == NULL || destino == NULL)
    {
        return -1;
    }

    return rename(origen, destino);
}

#pragma endregion
/* ============================================================================
   TIEMPO
   ============================================================================ */
 		#pragma region "TIEMPO"
 		static time_t sistema_tiempo_actual(void)
 		{
 			/*
 			 * FUTURO:
 			 * En un microcontrolador esta función puede obtener
 			 * el tiempo desde:
 			 *      RTC
 			 *      contador
 			 *      sistema operativo
 			 *      red
 			 *      GPS
 			 *      etc.
 			 */
 			return time(NULL);
 		}
 		#pragma endregion