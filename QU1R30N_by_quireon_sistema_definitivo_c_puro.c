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
#if defined(__linux__) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 199309L
#endif

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

 #include <float.h>      /* FLT_MAX para validar conversiones decimales a float. */

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
#include <time.h>
#endif

#if defined(__linux__)
#include <errno.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#elif defined(__linux__)
#include <sys/stat.h>
#endif

#if defined(__XC)
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif
#include <xc.h>
#endif


 /*
  * Tamaño de arranque, no límite máximo: las líneas largas se amplían mediante
  * la capa de memoria. Ejemplo: una línea de 80 caracteres crece desde 16.
  */
 #define TAMANO_INICIAL_LINEA 16


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

/*
 * Catálogo navegable del subsistema de negocio. El formato de resultado del
 * programa sigue siendo el de crearResultado(); este catálogo no lo reemplaza.
 */
typedef struct
{
	const char *nombre;
	const char *ruta;
	const char *columnas;
} GG_SeccionNegocio;

static const GG_SeccionNegocio GG_secciones_negocio[] = {
	{"Configuración", "CONFIG/CONF/CONFIGURACION.TXT", "dato_de_configuracion°descripcion_de_configuracion"},
	{"Productos / inventario", "CONFIG/INF/INVENTARIO/INVENTARIO.TXT",
		"_00_ID°_01_PRODUCTO°_02_CONTENIDO°_03_TIPO_MEDIDA°_04_PRECIO_VENTA°_05_COD_BARRAS°_06_CANTIDAD°_07_COSTO_COMP°_08_PROVEDOR°_09_GRUPO°_10_CANT_X_PAQUET°_11_ES_PAQUETE°_12_CODBAR_PAQUETE_E_ID°_13_COD_BAR_INDIVIDUAL_ES_PAQ_E_ID°_14_LIGAR_PROD_SAB°_15_IMPUESTOS°_16_INGREDIENTES°_17_CADUCIDAD°_18_ULTIMO_MOV°_19_SUCUR_VENT°_20_CLAF_PROD°_21_DIR_IMG_INTER°_22_DIR_IMG_COMP°_23_INFO_EXTRA°_24_PROCESO_CREAR°_25_DIR_VID_PROC_CREAR°_26_TIEMPO_FABRICACION°_27_INDICES_DIA_REGISTRO_PRODUC_VENDIDO°_28_INDICES_MES_REGISTRO_PRODUC_VENDIDO°_29_INDICES_AÑO_REGISTRO_PRODUC_VENDIDO°_30_ULTIMA_VENTA°_31_INDICES_TOTAL_REGISTRO_PRODUC_VENDIDO°_32_NO_PONER_NADA"},
	{"Productos no inventariados", "CONFIG/INF/INVENTARIO/COSAS_NO_ESTABAN.TXT", "_00_ID°_01_COD_BAR°_02_NOMBRE"},
	{"Proveedores", "CONFIG/INF/DAT/PROVEDORES.TXT",
		"_00_ID_EMPRESA°_01_NOMBRE_EMPRESA°_02_NOMBRE_ENCARGADO°_03_DIRECCIÓN_EMPRESA°_04_CIUDAD_EMPRESA°_05_ESTADO_EMPRESA°_06_CÓDIGO_POSTAL°_07_PAÍS°_08_CORREO_ELECTRÓNICO°_09_TELÉFONO_ENCARGADO°_10_TELEFONO_EMPRESA°_11_TIPO_DE_PROVEEDOR°_12_PRODUCTOS_SERVICIOS_SUMINISTRADOS°_13_CUENTA_BANCO°_14_UBICACIÓN_(GPS)°_15_NOTAS°_16_RECORDATORIO°_17_ACTIVO_O_NO_ACTIVO°_18_CALIFICACIÓN°_19_COMENTARIOS°_20_SUCURSALES_QUE_LE_COMPRAN°_21_DINERO_A_COMPRARLE°_22_DIAS_DE_PREVENTA°_23_DIAS_DE_ENTREGA"},
	{"Afiliados", "CONFIG/AFILIADOS/AFILIADOS_UNIFICADO.TXT",
		"_00_ID°_01_NOMBRE°_02_APELLIDO_PATERNO°_03_APELLIDO_MATERNO°_04_FECHA_DE_NACIMIENTO°_05_GÉNERO°_06_DIRECCIÓN°_07_CIUDAD°_08_ESTADO_PROVINCIA°_09_CÓDIGO_POSTAL°_10_PAÍS°_11_CORREO_ELECTRÓNICO°_12_TELÉFONO°_13_FECHA_DE_INGRESO°_14_SUELDO°_15_CARGO°_16_ESTADO_DE_CURS_APRENDIS_E°_17_SUPERVISOR°_18_NOTAS°_19_IDS_DE_AFILIADOS°_20_FECHA_DE_TERMINACIÓN°_21_MOTIVO_DE_TERMINACIÓN°_22_HORAS_TRABAJADAS°_23_EVALUACIONES_DE_DESEMPEÑO°_24_HABILIDADES_Y_CERTIFICACIONES°_25_IDIOMAS°_26_FECHA_DE_ÚLTIMA_PROMOCIÓN°_27_ID_DEPARTAMENTO°_28_HISTORIAL_DE_CAPACITACIÓN°_29_ÚLTIMO_AUMENTO_DE_SALARIO°_30_TIPO_EMPLEADO°_31_RANGO_CALIF°_32_ID_PATROCINADOR°_33_ID_ENCARGADO°_34_PUNTOS_A_P°_35_REG_PUNTOS_DIA°_36_REG_PUNTOS_MES°_37_REG_PUNTOS_AÑO°_38_REG_PUNTOS_TOTAL°_39_NIVEL°_40_FECHA_ASIGNACION°_40_ESTADO_DEL_USUARIO"},
	{"Sucursales", "CONFIG/INF/DAT/SUCUR.TXT",
		"_00_ID°_01_NOM_ID_SUCUR°_02_NOMBRE_SUCUR°_03_NOMBRE_ENCARGADO°_04_DIRECCIÓN_SUCUR°_05_CIUDAD_SUCUR°_06_ESTADO_SUCUR°_07_CÓDIGO_POSTAL°_08_PAÍS°_09_CORREO_ELECTRÓNICO°_10_TELÉFONO_ENCARGADO°_11_TELEFONO_SUCUR°_12_TIPO_DE_SUCUR°_13_PRODUCTOS_SERVICIOS°_14_CUENTA_BANCO°_15_UBICACIÓN_(GPS)°_16_NOTAS°_17_RECORDATORIO°_18_ACTIVO_O_NO_ACTIVO°_19_HORA_ABRIR_CERRAR"},
	{"Registros diarios", "CONFIG/INF/REGISTROS/FECHAS/AAAA/AAAAMM/AAAAMMDD_REGISTRO.TXT",
		"_00_ID°_01_HORA°_02_OPERACION°_03_TOTAL_IMPUESTO°_04_CÓDIGO_PRECIO°_05_COMENTARIO°_06_TOTAL_VENTA°_07_TOTAL_COSTO_COMP°_08_TOTAL_IMPUESTOS°_09_TOTAL_DEDUSIBLES°_10_PLATAFORMA"},
	{"Registros mensuales", "CONFIG/INF/REGISTROS/FECHAS/AAAA/AAAAMM_REGISTRO.TXT",
		"_00_ID°_01_DIA°_02_OPERACION°_03_TOTAL_IMPUESTO°_04_COMENTARIO°_05_TOTAL_VENTA°_06_TOTAL_COSTO_COMP°_07_TOTAL_IMPUESTOS°_08_TOTAL_GANANCIA"},
	{"Registros anuales", "CONFIG/INF/REGISTROS/FECHAS/AAAA_REGISTRO.TXT",
		"_00_ID°_01_MES°_02_OPERACION°_03_TOTAL_IMPUESTO°_04_COMENTARIO°_05_TOTAL_VENTA°_06_TOTAL_COSTO_COMP°_07_TOTAL_IMPUESTOS°_08_TOTAL_GANANCIA"},
	{"Registro acumulado", "CONFIG/INF/REGISTROS/ACUMULADO_REGISTRO.TXT",
		"_00_ID°_01_AÑO°_02_OPERACION°_03_TOTAL_IMPUESTO°_04_COMENTARIO°_05_TOTAL_VENTA°_06_TOTAL_COSTO_COMP°_07_TOTAL_IMPUESTOS°_08_TOTAL_GANANCIA"},
	{"Productos vendidos (día)", "CONFIG/INF/REGISTROS/FECHAS/AAAAMMDD_PRODUC_REGISTRO.TXT",
		"_00_ID°_01_NOMBRE_PRODUCTO°_02_CANTIDAD°_03_COD_BAR°_04_PROVEDORES°_05_HISTORIAL°_06_RANKING°_07_PROMEDIO°_08_VECES_SUPERA_PROMEDIO°_09_USO_MULTIPLE°_10_TIPO_PRODUCTO°_11_NIVEL_NECESIDAD"},
	{"Productos vendidos (mes)", "CONFIG/INF/REGISTROS/FECHAS/AAAAMM_PRODUC_REGISTRO.TXT",
		"_00_ID°_01_NOMBRE_PRODUCTO°_02_CANTIDAD°_03_COD_BAR°_04_PROVEDORES°_05_HISTORIAL°_06_RANKING°_07_PROMEDIO°_08_VECES_SUPERA_PROMEDIO°_09_USO_MULTIPLE°_10_TIPO_PRODUCTO°_11_NIVEL_NECESIDAD"},
	{"Productos vendidos (año)", "CONFIG/INF/REGISTROS/FECHAS/AAAA_PRODUC_REGISTRO.TXT",
		"_00_ID°_01_NOMBRE_PRODUCTO°_02_CANTIDAD°_03_COD_BAR°_04_PROVEDORES°_05_HISTORIAL°_06_RANKING°_07_PROMEDIO°_08_VECES_SUPERA_PROMEDIO°_09_USO_MULTIPLE°_10_TIPO_PRODUCTO°_11_NIVEL_NECESIDAD"},
	{"Productos vendidos (acumulado)", "CONFIG/INF/REGISTROS/ACUMULADO_PRODUC_REGISTRO.TXT",
		"_00_ID°_01_NOMBRE_PRODUCTO°_02_CANTIDAD°_03_COD_BAR°_04_PROVEDORES°_05_HISTORIAL°_06_RANKING°_07_PROMEDIO°_08_VECES_SUPERA_PROMEDIO°_09_USO_MULTIPLE°_10_TIPO_PRODUCTO°_11_NIVEL_NECESIDAD"},
	{"Impuestos", "CONFIG/INF/IMPUESTOS/IMPUESTOS.TXT", "_00_ID°_01_IMPUESTO°_02_PORCENTAGE°_03_DESCRIPCION°_04_INFO_EXTRA°_05_TIPO_IMPUESTO"},
	{"Deducibles", "CONFIG/INF/IMPUESTOS/DEDUSIBLES.TXT", "_00_ID°_01_FECHA°_02_MONTO°_03_DESCRIPCION°_04_PROVEDOR°_05_ARCHIVO_FACTURA°_06_FOLIO"},
	{"Herramientas / tipos de medida", "CONFIG/INF/INVENTARIO/TIPOS_DE_MEDIDA.TXT", "_00_ID°_01_COD_BAR"},
	{"Trabajos por día", "CONFIG/INF/DAT/TRABAJOS_POR_DIA.TXT",
		"_00_ID°_01_DIA°_02_ID_TRABAJADOR°_03_HECHO_O_NO°_04_CANTIDAD°_05_COSTO_COMP°_06_ID_QUIENLOISO°_07_ID_PROGRAMA"},
	{NULL, NULL, NULL}
};

static const GG_SeccionNegocio GG_archivos[] = {
	{"Espacios", "espacios/archivo_espacios.txt", "ID|Usuario|Contraseña|Directorio_Archivo_permisos_usuarios|Nivel"},
	{"Entrada del sistema", RUTA_TRANSFERENCIA_PREGUNTAS, "ID_DESTINO┴ID_ORIGEN■COMANDO■INFORMACION_ESPEJO_NO_SE_MODIFICA"},
	{"Salida del sistema", RUTA_TRANSFERENCIA_RESPUESTAS, "ID_DESTINO┴ID_ORIGEN■COMANDO■INFORMACION_ESPEJO_NO_SE_MODIFICA"},
	{"Control de errores", RUTA_CONTROL_ERRORES_TRY, NULL},
	{"Registros de ventas", "registros_ventas.txt", NULL},
	{"Registros de compras", "registros_compras.txt", NULL},
	{NULL, NULL, NULL}
};

char *GG_ultimo_retorno_estandar = NULL;
static char *GG_acumulador_retorno_procesos = NULL;

/* El getter devuelve una vista válida hasta la siguiente llamada a crearResultado(). */
static int actualizar_ultimo_retorno(const char *texto);
/* Acumula el texto de cada proceso usando el separador de transferencia actual. */
int acumular_retorno_proceso(const char *texto);
void limpiar_retorno_procesos(void);
const char *obtener_ultimo_retorno_formateado(void);

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
 char *construir_retorno_estandar(int codigo, const char *separador, const char *mensaje, const char *datos_extra);
 char *variable_string(const char *format, ...);
 int split_con_numero_de_celdas(const char *txt, const char *sep, char ***salida);
 void free_split_con_numero_de_celdas(char **arreglo);
 int texto_a_int_seguro(const char *texto, int *var_a_retornar);
 int texto_a_float_seguro(const char *texto, float *var_a_retornar);
 int concatenar_formato_separado_por_variable(char **destino, const char *separador, const char *formato, ...);
 int concatenar_formato(char *destino, size_t capacidad, const char *separador, const char *formato, ...);
 char *join_paresido_simple(char caracter_union_filas, char **texto, int n_texto, const char *columnas_extraer, const char *caracter_union_columnas);
 char *joineada_paraesida_y_quitador_de_extremos(const char *data, int restar_cuantas, int restar_primera_celda);
 char *Trimend_paresido(const char *texto);
 char *concatenacion_filas_de_un_archivo(const char *ruta_archivo, int poner_num_fila);
 char *concatenacion_filas_de_un_arreglo(char **arreglo, int n_arreglo, int poner_num_fila);
 char *concatenacion_caracter_separacion(const char *texto_actual, const char *texto_agregar, const char *separador);
 char *generar_folio(const char *formato_fecha_hora);
 int desfragmentar_direccion(const char *direccion, char **retorna_directorios, char **retorna_nom_arch, char **retorna_extencion);
 char *ReemplazarCaracteres_de_texto_arreglo(const char *info, char **caracteres_sep, int n_sep, char **caracteres_sustitucion);
 char *ReemplazarCaracteres_de_texto_string(const char *info, const char *sep, const char *sust);
 char *busqueda_profunda_string(const char *texto, const char *columnas_recorrer, const char *comparar);
 char *busqueda_profunda_comparacion_final_string(const char *texto, const char *columnas_recorrer, const char *comparar);
 char *busqueda_con_YY_profunda_texto_id_archivo(const char *texto, const char *columnas_recorrer, const char *comparaciones);
 char *editar_incr_string_funcion_recursiva(const char *texto, const char *columnas_recorrer, const char *info_sustituir, const char *edit_0_increm_1);
 char *editar_inc_agregar_edicion_profunda_multiple_comparacion_final_string(const char *texto, const char *indices_editar, const char *info_editar, const char *comparacion, const char *edit_0_increm_1);
 char *ARR_FUN_SOLO_TEXTO_editar_inc_agregar_edicion_profunda_multiple(const char *datos);
 char *editar_inc_agregar_edicion_profunda_multiple_comparacion_MULTIPLE_A_CHECAR(const char *texto, const char *indices_editar, const char *comparacion_con_edicion, const char *edit_0_increm_1);
 char *editar_inc_edicion_profunda_multiple_string(const char *texto, const char *indices_editar, const char *info_editar, const char *edit_0_increm_1);
 char *editar_inc_edicion_profunda_multiple_AL_FINAL_string(const char *texto, const char *indices_editar, const char *info_editar, const char *edit_0_increm_1);
 char *recorrer_caracter_separacion(const char *contenidoFila, const char *izquierda_o_derecha, int numero_veses);
 char *recorrer_caracter_separacion_funciones_espesificas(const char *contenidoFila, const char *izquierda_o_derecha, int numero_veses);
  int crearDirectorio(const char *ruta);
  int crearArchivo(const char *ruta, const char *cabecera);
  char **leer_archivo(const char *ruta, int *n_lineas_out);
  void guardar_archivo(const char *ruta, char **lineas, int n_lineas);
  void free_lineas(char **lineas, int n_lineas);
  void agregar_fila(const char *ruta, const char *fila);
  int existe_archivo(const char *ruta);
  void generar_ruta_archivo(const char *ruta, long id, long cant_por_arch, char *salida, size_t tam_salida);
  char *leer_info_dividida(const char *ruta);
  char *agregar_info_dividida(const char *direccion, const char *agregando, const char *nom_columnas);
  char *leer_todo_info_dividida(const char *ruta);
  char *agregar_sino_existe_info_dividida(const char *direccion, long col_comp, const char *comparar, const char *texto_agr, const char *nom_cols);
  char *seleccionar_id_info_dividida(const char *direccion, const char *id);
  char *seleccionar_id_info_dividida_extrae(const char *direccion, const char *id);
  char *editar_id_info_dividida(const char *direccion, const char *id, const char *info_a_comparar, const char *col_comparar, const char *nuevos_datos, int col_editar);
  char *editar_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *vals_str);
  char *incrementa_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *cants_str);
  char *incrementa_celda_id_info_dividida_copia_si_cero(const char *direccion, const char *id, const char *cols_str, const char *cants_str, const char *cols_fuente_str);
  char *incrementa_celda_busqueda_info_dividida(const char *direccion, const char *dato_a_buscar, int col_buscar, const char *cols_inc_str, const char *cants_inc_str, const char *agregar_fallback);
  char *buscar_info_dividida(const char *direccion, const char *dato_buscar, int col_buscar, int id_hint);
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
 void delay_ms(unsigned int ms);
 void fechaActual(char *buffer, const char *formato);
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
	char *accionSubmenu = NULL;
	char *comandoConcatenado = NULL;

	char *error_o_datos_a_retornar ="";
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "\n=== MENÚ DE PRUEBAS ===\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "1. Probar comandos de texto y archivos\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "2. Probar envío y consulta de mensajes\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "3. Probar operaciones de texto\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "4. Probar funciones auxiliares e infraestructura\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "5. Navegar el catálogo del negocio\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "6. Volver al menú principal\n");
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
			{return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}
			
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 8)
			{return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}

			switch (opcionSubmenu)
			{
				case 1:
					accionSubmenu = concatenar("%s%s%s", "leerArchivo", GG_caracter_separacion[2], NOMBRE_ARCHIVO);
					break;
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s", "escribirLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "registro_sintetico");
					break;
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s", "editarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					if(accionSubmenu != NULL)
					{
						char *accionCompleta = concatenar("%s%s%s", accionSubmenu, GG_caracter_separacion[2], "texto_editado_sintetico");
						sistema_memoria_liberar(accionSubmenu, error_o_datos_a_retornar, 0);
						accionSubmenu = accionCompleta;
					}
					break;
				case 4:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "editarColumna", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1", GG_caracter_separacion[2], "1");
					if(accionSubmenu != NULL)
					{
						char *accionCompleta = concatenar("%s%s%s", accionSubmenu, GG_caracter_separacion[2], "valor_sintetico");
						sistema_memoria_liberar(accionSubmenu, error_o_datos_a_retornar, 0);
						accionSubmenu = accionCompleta;
					}
					break;
				case 5:
					accionSubmenu = concatenar("%s%s%s%s%s", "eliminarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					break;
				case 6:
					accionSubmenu = concatenar("%s%s%s%s%s", "vaciarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					break;
				case 7:
					accionSubmenu = concatenar("%s", "ejecutarEjemplosPrueba");
					break;
				case 8:
					accionSubmenu = concatenar("%s", "volver");
					break;
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
			switch(opcionSubmenu)
			{
				case 1:
					accionSubmenu = concatenar("%s%s%s", "mandar_mensje_a_todos", GG_caracter_separacion[2], "mensaje_sintetico");
					break;
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "mandar_mensje_a_contacto", GG_caracter_separacion[2], "mensaje_sintetico", GG_caracter_separacion[2], "contacto_sintetico", GG_caracter_separacion[2], "1");
					break;
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "mandar_mensje_al_primero_que_responda", GG_caracter_separacion[2], "pregunta_sintetica", GG_caracter_separacion[2], "aceptacion_sintetica", GG_caracter_separacion[2], "respuesta_sintetica");
					break;
				case 4:
					accionSubmenu = concatenar("%s", "checar_si_hay_mensajes_no_leido");
					break;
				case 5:
					accionSubmenu = concatenar("%s", "volver");
					break;
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
			switch(opcionSubmenu)
			{
				case 1:
					accionSubmenu = concatenar("%s%s%s%s%s", "split", GG_caracter_separacion[2], "uno,dos,tres", GG_caracter_separacion[2], ",");
					break;
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "modificarColumna", GG_caracter_separacion[2], "uno,dos,tres", GG_caracter_separacion[2], "2", GG_caracter_separacion[2], "valor_sintetico");
					break;
				case 3:
					accionSubmenu = concatenar("%s", "leerLineaConsola");
					break;
				case 4:
					accionSubmenu = concatenar("%s", "volver");
					break;
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
			switch(opcionSubmenu)
			{
				case 1:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "probar_split_join", GG_caracter_separacion[2], "uno,dos", GG_caracter_separacion[2], ",", GG_caracter_separacion[2], "-");
					break;
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s", "probar_conversiones", GG_caracter_separacion[2], "42", GG_caracter_separacion[2], "42|prueba");
					break;
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s%s%s%s%s", "probar_crearResultado", GG_caracter_separacion[2], "1", GG_caracter_separacion[2], "prueba_sintetica", GG_caracter_separacion[2], "", GG_caracter_separacion[2], "prueba", GG_caracter_separacion[2], "1");
					break;
				case 4:
					accionSubmenu = concatenar("%s%s%s", "probar_memoria", GG_caracter_separacion[2], "16");
					break;
				case 5:
					accionSubmenu = concatenar("%s%s%s%s%s", "probar_archivos", GG_caracter_separacion[2], "texto_sintetico", GG_caracter_separacion[2], "42");
					break;
				case 6:
					accionSubmenu = concatenar("%s%s%s", "probar_consola", GG_caracter_separacion[2], "texto_sintetico");
					break;
				case 7:
					accionSubmenu = concatenar("%s", "volver");
					break;
			}
			break;
		}
		case 5:
		{
			comando = "catalogo_negocio";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== CATÁLOGO DEL NEGOCIO ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Productos e inventario\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Proveedores y afiliados\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Sucursales\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Registros de ventas y productos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Impuestos, deducibles y trabajos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Configuración y archivos generales\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Volver\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una sección: ");
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
			accionSubmenu = concatenar("%s%s%d", "ver", GG_caracter_separacion[2], opcionSubmenu);
			break;
		}
		case 6:
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

	if(accionSubmenu == NULL)
	{
		return crearResultado(-2, "accion_submenu_no_definida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}
	comandoConcatenado = concatenar(
		"%s%s%s",
		comando,
		GG_caracter_separacion_funciones_espesificas[0],
		accionSubmenu
	);
	sistema_memoria_liberar(accionSubmenu, error_o_datos_a_retornar, 0);
	if(comandoConcatenado == NULL)
	{
		return crearResultado(-1, "error_al_concatenar_comando", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}
	

	char *resultado = concatenar(
		"%s%s%s%s%s%s%d%s%s",
		GG_id_programa,
		GG_caracter_para_transferencia_entre_archivos[0],
		"prueba",
		GG_caracter_para_transferencia_entre_archivos[1],
		comandoConcatenado,
		GG_caracter_para_transferencia_entre_archivos[1],
		opcionSubmenu,
		GG_caracter_para_transferencia_entre_archivos[1],
		"INFORMACION_ESPEJO_NO_SE_MODIFICA"
	);
	sistema_memoria_liberar(comandoConcatenado, error_o_datos_a_retornar, 0);
	if(resultado == NULL){return crearResultado(-1, "error_al_concatenar_opcion", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);}
	return resultado;
 }

static void mostrar_seccion_negocio(int seccion, char *error_o_datos_a_retornar, int nivel_de_profundidad)
{
	const int cantidadSecciones = (int)(sizeof(GG_secciones_negocio) / sizeof(GG_secciones_negocio[0])) - 1;
	const int cantidadArchivos = (int)(sizeof(GG_archivos) / sizeof(GG_archivos[0])) - 1;
	int inicio = 0;
	int fin = 0;

	switch(seccion)
	{
		case 1: inicio = 1; fin = 3; break;
		case 2: inicio = 3; fin = 5; break;
		case 3: inicio = 5; fin = 6; break;
		case 4: inicio = 6; fin = 14; break;
		case 5: inicio = 14; fin = cantidadSecciones; break;
		case 6:
			sistema_consola_escribir_formato(
				error_o_datos_a_retornar,
				nivel_de_profundidad,
				"%s\n  Archivo: %s\n",
				GG_secciones_negocio[0].nombre,
				GG_secciones_negocio[0].ruta
			);
			sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "\n=== ARCHIVOS GENERALES ===\n");
			for(int i = 0; i < cantidadArchivos; i++)
			{
				sistema_consola_escribir_formato(
					error_o_datos_a_retornar,
					nivel_de_profundidad,
					"%s: %s\n",
					GG_archivos[i].nombre,
					GG_archivos[i].ruta
				);
			}
			return;
		case 7:
			return;
		default:
			return;
	}

	sistema_consola_escribir_formato(error_o_datos_a_retornar, nivel_de_profundidad, "\n=== SECCIÓN DEL NEGOCIO ===\n");
	for(int i = inicio; i < fin && i < cantidadSecciones; i++)
	{
		sistema_consola_escribir_formato(
			error_o_datos_a_retornar,
			nivel_de_profundidad,
			"%s\n  Archivo: %s\n",
			GG_secciones_negocio[i].nombre,
			GG_secciones_negocio[i].ruta
		);
	}
}

/*
 * Garantiza que exista NOMBRE_ARCHIVO sin borrar su contenido si ya existe.
 */
static char * inicio(char *error_o_datos_a_retornar, int nivel_de_profundidad)
{
	SistemaArchivo *archivo = sistema_archivo_abrir(NOMBRE_ARCHIVO,"a",error_o_datos_a_retornar,nivel_de_profundidad);

	
	if(archivo == NULL)
	{return crearResultado(-1, "no_se_pudo_crear_archivo_inicial", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad + 1);}

	if(sistema_archivo_cerrar(archivo, error_o_datos_a_retornar, nivel_de_profundidad) != 0)
	{return crearResultado(-1, "no_se_pudo_cerrar_archivo_inicial", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad + 1);}

	return crearResultado(1, "archivo_inicial_listo", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad + 1);
}

 /*
  * Punto de entrada: muestra el menú y distribuye las opciones.
  */
int main(void)
{
	configurarConsolaUTF8();
	char *error_o_datos_a_retornar = NULL;
	char *resultadoInicio = inicio(error_o_datos_a_retornar, 0);
	int codigoInicio = -1;
	if(resultadoInicio == NULL ||
		!leerCodigoResultado(error_o_datos_a_retornar, 0, resultadoInicio, &codigoInicio))
	{
		sistema_memoria_liberar(resultadoInicio, error_o_datos_a_retornar, 0);
		return -1;
	}
	sistema_memoria_liberar(resultadoInicio, error_o_datos_a_retornar, 0);
	if(codigoInicio < 0)
	{
		return codigoInicio;
	}

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
		int cantidadPartesComando = 0;
		char **partesComando = split(
			partesResultado[1],
			GG_caracter_separacion_funciones_espesificas[0],
			&cantidadPartesComando,
			error_o_datos_a_retornar,
			0
		);
		if(partesComando == NULL || cantidadPartesComando < 1)
		{
			liberarSplit(partesComando, cantidadPartesComando, error_o_datos_a_retornar, 0);
			liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return -1;
		}
		const char *nombreFuncion = partesComando[0];
		int opcionSubmenu = 0;
		if(cantidadPartesResultado < 4 ||
			!textoAEntero(partesResultado[2], &opcionSubmenu, error_o_datos_a_retornar, 0))
		{
			liberarSplit(partesComando, cantidadPartesComando, error_o_datos_a_retornar, 0);
			liberarSplit(partesResultado, cantidadPartesResultado, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return -1;
		}

		if(nombreFuncion != NULL && strcmp(nombreFuncion, "salir") == 0)
		{
			liberarSplit(partesComando, cantidadPartesComando, error_o_datos_a_retornar, 0);
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
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "catalogo_negocio") == 0)
		{
			mostrar_seccion_negocio(opcionSubmenu, error_o_datos_a_retornar, 1);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(1, "catalogo_negocio", "", __func__, error_o_datos_a_retornar, 1);
		}
		else
		{
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(1, "informacionMain", "1", __func__, error_o_datos_a_retornar, 1);
		}

		liberarSplit(partesComando, cantidadPartesComando, error_o_datos_a_retornar, 0);
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
 * Formatea una cadena nueva con memoria de la capa común.
 * El llamador debe liberar el resultado con sistema_memoria_liberar().
 */
char *variable_string(const char *format, ...)
{
#if defined(PLATAFORMA_SEMICONDUCTOR) && !defined(PIC16F)
	(void)format;
	return NULL;
#else
	if(format == NULL) return NULL;

	va_list argumentos;
	va_list argumentos_medicion;
	va_start(argumentos, format);
	va_copy(argumentos_medicion, argumentos);
	int longitud = vsnprintf(NULL, 0, format, argumentos_medicion);
	va_end(argumentos_medicion);
	if(longitud < 0)
	{
		va_end(argumentos);
		return NULL;
	}

	char *buffer = sistema_memoria_reservar((size_t)longitud + 1, NULL, 0);
	if(buffer == NULL)
	{
		va_end(argumentos);
		return NULL;
	}

	int escritos = vsnprintf(buffer, (size_t)longitud + 1, format, argumentos);
	va_end(argumentos);
	if(escritos != longitud)
	{
		sistema_memoria_liberar(buffer, NULL, 0);
		return NULL;
	}
	return buffer;
#endif
}

/*
 * Construye el retorno estándar sin cambiar el formato de traza de crearResultado().
 * El resultado pertenece al llamador y se libera con sistema_memoria_liberar().
 */
char *construir_retorno_estandar(
	int codigo,
	const char *separador,
	const char *mensaje,
	const char *datos_extra
)
{
	if(separador == NULL)
	{
		separador = GG_caracter_para_confirmacion_o_error[0];
	}
	if(separador == NULL) return NULL;

	return variable_string(
		"%d%s%s%s%s",
		codigo,
		separador,
		mensaje != NULL ? mensaje : "sin_mensaje",
		separador,
		datos_extra != NULL ? datos_extra : "sin_datos"
	);
}

/*
 * Adaptador compatible con la interfaz anterior de split.
 * Devuelve el número de celdas o -1 si no pudo dividir.
 */
int split_con_numero_de_celdas(const char *txt, const char *sep, char ***salida)
{
	if(salida == NULL) return -1;
	*salida = NULL;

	int cantidad = 0;
	char **partes = split(txt, sep, &cantidad, NULL, 0);
	if(partes == NULL) return -1;

	*salida = partes;
	return cantidad;
}

void free_split_con_numero_de_celdas(char **arreglo)
{
	if(arreglo == NULL) return;

	int cantidad = 0;
	while(arreglo[cantidad] != NULL)
	{
		if(cantidad == INT_MAX)
		{
			return;
		}
		cantidad++;
	}
	liberarSplit(arreglo, cantidad, NULL, 0);
}

/* Devuelve 0 si convierte todo el texto y -1 si el valor no es válido. */
int texto_a_int_seguro(const char *texto, int *var_a_retornar)
{
	return textoAEntero(texto, var_a_retornar, NULL, 0) ? 0 : -1;
}

typedef struct
{
	char *datos;
	size_t longitud;
	size_t capacidad;
} GG_CadenaDinamica;

static void gg_cadena_iniciar(GG_CadenaDinamica *cadena)
{
	cadena->datos = sistema_memoria_reservar(1, NULL, 0);
	cadena->longitud = 0;
	cadena->capacidad = cadena->datos != NULL ? 1 : 0;
	if(cadena->datos != NULL) cadena->datos[0] = '\0';
}

static int gg_cadena_agregar_n(GG_CadenaDinamica *cadena, const char *texto, size_t longitud)
{
	if(cadena == NULL || (texto == NULL && longitud > 0)) return 0;
	if(cadena->datos == NULL) return 0;
	if(longitud > (size_t)-1 - cadena->longitud - 1) return 0;

	size_t requerida = cadena->longitud + longitud + 1;
	if(requerida > cadena->capacidad)
	{
		size_t nueva_capacidad = cadena->capacidad;
		while(nueva_capacidad < requerida)
		{
			if(nueva_capacidad > (size_t)-1 / 2)
			{
				nueva_capacidad = requerida;
				break;
			}
			nueva_capacidad *= 2;
		}
		char *temporal = sistema_memoria_redimensionar(cadena->datos, nueva_capacidad, NULL, 0);
		if(temporal == NULL) return 0;
		cadena->datos = temporal;
		cadena->capacidad = nueva_capacidad;
	}

	if(longitud > 0) memcpy(cadena->datos + cadena->longitud, texto, longitud);
	cadena->longitud += longitud;
	cadena->datos[cadena->longitud] = '\0';
	return 1;
}

static int gg_cadena_agregar(GG_CadenaDinamica *cadena, const char *texto)
{
	return texto != NULL
		? gg_cadena_agregar_n(cadena, texto, strlen(texto))
		: 1;
}

static void gg_cadena_liberar(GG_CadenaDinamica *cadena)
{
	if(cadena == NULL) return;
	sistema_memoria_liberar(cadena->datos, NULL, 0);
	cadena->datos = NULL;
	cadena->longitud = 0;
	cadena->capacidad = 0;
}

static char *gg_cadena_entregar(GG_CadenaDinamica *cadena)
{
	if(cadena == NULL) return NULL;
	char *resultado = cadena->datos;
	cadena->datos = NULL;
	cadena->longitud = 0;
	cadena->capacidad = 0;
	return resultado;
}

static char *gg_formatear_argumentos(const char *formato, va_list argumentos)
{
#if defined(PLATAFORMA_SEMICONDUCTOR) && !defined(PIC16F)
	(void)formato;
	(void)argumentos;
	return NULL;
#else
	if(formato == NULL) return NULL;
	va_list copia;
	va_copy(copia, argumentos);
	int longitud = vsnprintf(NULL, 0, formato, copia);
	va_end(copia);
	if(longitud < 0) return NULL;

	char *resultado = sistema_memoria_reservar((size_t)longitud + 1, NULL, 0);
	if(resultado == NULL) return NULL;
	va_copy(copia, argumentos);
	int escritos = vsnprintf(resultado, (size_t)longitud + 1, formato, copia);
	va_end(copia);
	if(escritos != longitud)
	{
		sistema_memoria_liberar(resultado, NULL, 0);
		return NULL;
	}
	return resultado;
#endif
}

static int gg_indices_desde_texto(const char *texto, const char *separador, int **indices_salida, int *cantidad_salida)
{
	if(texto == NULL || separador == NULL || separador[0] == '\0' ||
		indices_salida == NULL || cantidad_salida == NULL) return 0;
	*indices_salida = NULL;
	*cantidad_salida = 0;

	int cantidad = 0;
	char **partes = split(texto, separador, &cantidad, NULL, 0);
	if(partes == NULL) return 0;
	if((size_t)cantidad > (size_t)-1 / sizeof(int))
	{
		liberarSplit(partes, cantidad, NULL, 0);
		return 0;
	}

	int *indices = sistema_memoria_reservar((size_t)cantidad * sizeof(*indices), NULL, 0);
	if(indices == NULL && cantidad > 0)
	{
		liberarSplit(partes, cantidad, NULL, 0);
		return 0;
	}
	for(int i = 0; i < cantidad; ++i)
	{
		if(texto_a_int_seguro(partes[i], &indices[i]) != 0 || indices[i] < 0)
		{
			sistema_memoria_liberar(indices, NULL, 0);
			liberarSplit(partes, cantidad, NULL, 0);
			return 0;
		}
	}
	liberarSplit(partes, cantidad, NULL, 0);
	*indices_salida = indices;
	*cantidad_salida = cantidad;
	return 1;
}

int texto_a_float_seguro(const char *texto, float *var_a_retornar)
{
	if(texto == NULL || var_a_retornar == NULL || texto[0] == '\0') return -1;

	const char *cursor = texto;
	int negativo = 0;
	if(*cursor == '-' || *cursor == '+')
	{
		negativo = *cursor == '-';
		cursor++;
	}

	double valor = 0.0;
	int digitos = 0;
	while(*cursor >= '0' && *cursor <= '9')
	{
		valor = valor * 10.0 + (double)(*cursor - '0');
		if(valor > FLT_MAX) return -1;
		digitos++;
		cursor++;
	}
	if(*cursor == '.')
	{
		double escala = 0.1;
		cursor++;
		while(*cursor >= '0' && *cursor <= '9')
		{
			valor += (double)(*cursor - '0') * escala;
			if(valor > FLT_MAX) return -1;
			escala *= 0.1;
			digitos++;
			cursor++;
		}
	}
	if(digitos == 0 || *cursor != '\0') return -1;

	*var_a_retornar = (float)(negativo ? -valor : valor);
	return 0;
}

int concatenar_formato_separado_por_variable(char **destino, const char *separador, const char *formato, ...)
{
	if(destino == NULL || formato == NULL) return -1;

	GG_CadenaDinamica formato_expandido;
	gg_cadena_iniciar(&formato_expandido);
	if(formato_expandido.datos == NULL) return -1;

	size_t longitud_formato = strlen(formato);
	for(size_t i = 0; i < longitud_formato; ++i)
	{
		if(formato[i] != '%')
		{
			if(!gg_cadena_agregar_n(&formato_expandido, formato + i, 1)) goto error_formato_expandido;
			continue;
		}
		if(i + 1 >= longitud_formato) goto error_formato_expandido;
		if(!gg_cadena_agregar_n(&formato_expandido, formato + i, 1)) goto error_formato_expandido;
		i++;
		if(!gg_cadena_agregar_n(&formato_expandido, formato + i, 1)) goto error_formato_expandido;
		if(formato[i] == '%') continue;

		while(i + 1 < longitud_formato &&
			strchr("diuoxXfFeEgGaAcspn", formato[i]) == NULL)
		{
			i++;
			if(!gg_cadena_agregar_n(&formato_expandido, formato + i, 1)) goto error_formato_expandido;
		}
		if(strchr("diuoxXfFeEgGaAcspn", formato[i]) == NULL) goto error_formato_expandido;
		if(separador != NULL && separador[0] != '\0' &&
			!gg_cadena_agregar(&formato_expandido, separador)) goto error_formato_expandido;
	}

	va_list argumentos;
	va_start(argumentos, formato);
	char *nuevo_texto = gg_formatear_argumentos(formato_expandido.datos, argumentos);
	va_end(argumentos);
	gg_cadena_liberar(&formato_expandido);
	if(nuevo_texto == NULL) return -1;

	GG_CadenaDinamica combinado;
	gg_cadena_iniciar(&combinado);
	if(combinado.datos == NULL)
	{
		sistema_memoria_liberar(nuevo_texto, NULL, 0);
		return -1;
	}
	if(*destino != NULL && !gg_cadena_agregar(&combinado, *destino)) goto error_combinado;
	if(!gg_cadena_agregar(&combinado, nuevo_texto)) goto error_combinado;
	sistema_memoria_liberar(nuevo_texto, NULL, 0);
	sistema_memoria_liberar(*destino, NULL, 0);
	*destino = gg_cadena_entregar(&combinado);
	return *destino != NULL ? 0 : -1;

error_combinado:
	sistema_memoria_liberar(nuevo_texto, NULL, 0);
	gg_cadena_liberar(&combinado);
	return -1;
error_formato_expandido:
	gg_cadena_liberar(&formato_expandido);
	return -1;
}

int concatenar_formato(char *destino, size_t capacidad, const char *separador, const char *formato, ...)
{
if(destino == NULL || capacidad == 0 || formato == NULL ||
	memchr(destino, '\0', capacidad) == NULL) return -1;
va_list argumentos;
va_start(argumentos, formato);
char *nuevo_texto = gg_formatear_argumentos(formato, argumentos);
	va_end(argumentos);
	if(nuevo_texto == NULL) return -1;

	size_t longitud = strlen(destino);
	size_t longitud_nuevo = strlen(nuevo_texto);
	size_t longitud_separador = separador != NULL ? strlen(separador) : 0;
	if(longitud >= capacidad ||
		longitud_nuevo > capacidad - longitud - 1 ||
		longitud_separador > capacidad - longitud - longitud_nuevo - 1)
	{
		sistema_memoria_liberar(nuevo_texto, NULL, 0);
		return -1;
	}
	memcpy(destino + longitud, nuevo_texto, longitud_nuevo);
	longitud += longitud_nuevo;
	if(longitud_separador > 0)
	{
		memcpy(destino + longitud, separador, longitud_separador);
		longitud += longitud_separador;
	}
	destino[longitud] = '\0';
	sistema_memoria_liberar(nuevo_texto, NULL, 0);
	return 0;
}

static const char *gg_separador_columnas(void)
{
	return GG_caracter_separacion[1] != NULL ? GG_caracter_separacion[1] : "|";
}

static const char *gg_separador_filas(void)
{
	return GG_caracter_separacion[0] != NULL ? GG_caracter_separacion[0] : "|";
}

static char *gg_unir_partes(char **partes, int cantidad, const char *separador)
{
	return join(partes, cantidad, separador, NULL, 0);
}

char *join_paresido_simple(
	char caracter_union_filas,
	char **texto,
	int n_texto,
	const char *columnas_extraer,
	const char *caracter_union_columnas
)
{
	if(n_texto < 0 || (n_texto > 0 && texto == NULL)) return NULL;
	char separador_filas[2] = {caracter_union_filas, '\0'};
	const char *separador_columnas = caracter_union_columnas != NULL
		? caracter_union_columnas
		: gg_separador_columnas();
	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL) return NULL;

	if(columnas_extraer == NULL || columnas_extraer[0] == '\0')
	{
		int escritas = 0;
		for(int i = 0; i < n_texto; ++i)
		{
			if(texto[i] == NULL) continue;
			if(escritas > 0 && !gg_cadena_agregar(&salida, separador_filas)) goto error_join_paresido;
			if(!gg_cadena_agregar(&salida, texto[i])) goto error_join_paresido;
			escritas++;
		}
		return gg_cadena_entregar(&salida);
	}

	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(columnas_extraer, separador_columnas, &indices, &cantidad_indices))
		goto error_join_paresido;

	int filas_escritas = 0;
	for(int i = 0; i < n_texto; ++i)
	{
		if(texto[i] == NULL) continue;
		int cantidad_partes = 0;
		char **partes = split(texto[i], separador_columnas, &cantidad_partes, NULL, 0);
		if(partes == NULL)
		{
			sistema_memoria_liberar(indices, NULL, 0);
			goto error_join_paresido;
		}
		GG_CadenaDinamica fila;
		gg_cadena_iniciar(&fila);
		if(fila.datos == NULL)
		{
			liberarSplit(partes, cantidad_partes, NULL, 0);
			sistema_memoria_liberar(indices, NULL, 0);
			goto error_join_paresido;
		}
		int columnas_escritas = 0;
		for(int j = 0; j < cantidad_indices; ++j)
		{
			int indice = indices[j];
			if(indice >= cantidad_partes) continue;
			if(columnas_escritas > 0 && !gg_cadena_agregar(&fila, separador_columnas))
			{
				gg_cadena_liberar(&fila);
				liberarSplit(partes, cantidad_partes, NULL, 0);
				sistema_memoria_liberar(indices, NULL, 0);
				goto error_join_paresido;
			}
			if(!gg_cadena_agregar(&fila, partes[indice]))
			{
				gg_cadena_liberar(&fila);
				liberarSplit(partes, cantidad_partes, NULL, 0);
				sistema_memoria_liberar(indices, NULL, 0);
				goto error_join_paresido;
			}
			columnas_escritas++;
		}
		if(columnas_escritas == 0)
		{
			gg_cadena_liberar(&fila);
			liberarSplit(partes, cantidad_partes, NULL, 0);
			continue;
		}
		if(filas_escritas > 0 && !gg_cadena_agregar(&salida, separador_filas))
		{
			gg_cadena_liberar(&fila);
			liberarSplit(partes, cantidad_partes, NULL, 0);
			sistema_memoria_liberar(indices, NULL, 0);
			goto error_join_paresido;
		}
		if(!gg_cadena_agregar(&salida, fila.datos))
		{
			gg_cadena_liberar(&fila);
			liberarSplit(partes, cantidad_partes, NULL, 0);
			sistema_memoria_liberar(indices, NULL, 0);
			goto error_join_paresido;
		}
		if(columnas_escritas > 0) filas_escritas++;
		gg_cadena_liberar(&fila);
		liberarSplit(partes, cantidad_partes, NULL, 0);
	}

	sistema_memoria_liberar(indices, NULL, 0);
	return gg_cadena_entregar(&salida);

error_join_paresido:
	gg_cadena_liberar(&salida);
	return NULL;
}

char *joineada_paraesida_y_quitador_de_extremos(const char *data, int restar_cuantas, int restar_primera_celda)
{
	if(data == NULL || restar_cuantas < 0) return NULL;
	int cantidad = 0;
	char **partes = split(data, gg_separador_filas(), &cantidad, NULL, 0);
	if(partes == NULL) return NULL;

	int inicio = restar_primera_celda ? restar_cuantas : 0;
	int fin = restar_primera_celda ? cantidad : cantidad - restar_cuantas;
	if(inicio > cantidad) inicio = cantidad;
	if(fin < inicio) fin = inicio;
	char *resultado = gg_unir_partes(partes + inicio, fin - inicio, gg_separador_filas());
	liberarSplit(partes, cantidad, NULL, 0);
	return resultado;
}

char *Trimend_paresido(const char *texto)
{
	if(texto == NULL) return NULL;
	int cantidad = 0;
	char **partes = split(texto, gg_separador_filas(), &cantidad, NULL, 0);
	if(partes == NULL) return NULL;

	int cantidad_total = cantidad;
	while(cantidad > 0 && partes[cantidad - 1][0] == '\0') cantidad--;
	char *resultado = gg_unir_partes(partes, cantidad, gg_separador_filas());
	liberarSplit(partes, cantidad_total, NULL, 0);
	return resultado;
}

char *concatenacion_filas_de_un_archivo(const char *ruta_archivo, int poner_num_fila)
{
	if(ruta_archivo == NULL) return NULL;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta_archivo, "r", NULL, 0);
	if(archivo == NULL) return NULL;

	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL)
	{
		sistema_archivo_cerrar(archivo, NULL, 0);
		return NULL;
	}

	int num_fila = 0;
	int error = 0;
	char *linea;
	while((linea = leerLineaDinamica(archivo, NULL, 0)) != NULL)
	{
		if(salida.longitud > 0 && !gg_cadena_agregar(&salida, gg_separador_filas()))
		{
			sistema_memoria_liberar(linea, NULL, 0);
			error = 1;
			break;
		}
		if(poner_num_fila)
		{
			char numero[sizeof(int) * CHAR_BIT + 2];
			char prefijo[sizeof(numero) + 2];
			if(enteroATexto(num_fila, numero, sizeof(numero)) == NULL)
			{
				sistema_memoria_liberar(linea, NULL, 0);
				error = 1;
				break;
			}
			size_t longitud_numero = strlen(numero);
			memcpy(prefijo, numero, longitud_numero);
			memcpy(prefijo + longitud_numero, ") ", 3);
			if(!gg_cadena_agregar(&salida, prefijo))
			{
				sistema_memoria_liberar(linea, NULL, 0);
				error = 1;
				break;
			}
		}
		if(!gg_cadena_agregar(&salida, linea))
		{
			sistema_memoria_liberar(linea, NULL, 0);
			error = 1;
			break;
		}
		num_fila++;
		sistema_memoria_liberar(linea, NULL, 0);
	}
	if(sistema_archivo_hay_error(archivo, NULL, 0)) error = 1;
	if(sistema_archivo_cerrar(archivo, NULL, 0) != 0) error = 1;
	if(error)
	{
		gg_cadena_liberar(&salida);
		return NULL;
	}
	return gg_cadena_entregar(&salida);
}

char *concatenacion_filas_de_un_arreglo(char **arreglo, int n_arreglo, int poner_num_fila)
{
	if(n_arreglo < 0 || (n_arreglo > 0 && arreglo == NULL)) return NULL;
	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL) return NULL;

	int agregadas = 0;
	for(int i = 0; i < n_arreglo; ++i)
	{
		if(arreglo[i] == NULL) continue;
		if(agregadas > 0 && !gg_cadena_agregar(&salida, gg_separador_filas())) goto error_concatenar_filas;
		if(poner_num_fila)
		{
			char numero[sizeof(int) * CHAR_BIT + 2];
			char prefijo[sizeof(numero) + 2];
			if(enteroATexto(i, numero, sizeof(numero)) == NULL) goto error_concatenar_filas;
			size_t longitud_numero = strlen(numero);
			memcpy(prefijo, numero, longitud_numero);
			memcpy(prefijo + longitud_numero, ") ", 3);
			if(!gg_cadena_agregar(&salida, prefijo)) goto error_concatenar_filas;
		}
		if(!gg_cadena_agregar(&salida, arreglo[i])) goto error_concatenar_filas;
		agregadas++;
	}
	return gg_cadena_entregar(&salida);

error_concatenar_filas:
	gg_cadena_liberar(&salida);
	return NULL;
}

char *concatenacion_caracter_separacion(const char *texto_actual, const char *texto_agregar, const char *separador)
{
	if(texto_agregar == NULL) return NULL;
	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL) return NULL;
	if(texto_actual != NULL && texto_actual[0] != '\0')
	{
		if(!gg_cadena_agregar(&salida, texto_actual) ||
			(separador != NULL && !gg_cadena_agregar(&salida, separador)))
		{
			gg_cadena_liberar(&salida);
			return NULL;
		}
	}
	if(!gg_cadena_agregar(&salida, texto_agregar))
	{
		gg_cadena_liberar(&salida);
		return NULL;
	}
	return gg_cadena_entregar(&salida);
}

char *generar_folio(const char *formato_fecha_hora)
{
#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
	time_t ahora = time(NULL);
	struct tm *hora_local = localtime(&ahora);
	if(hora_local == NULL) return NULL;
	char buffer[256];
	const char *formato = formato_fecha_hora != NULL ? formato_fecha_hora : "%y%m%d%H%M%S";
	size_t longitud = strftime(buffer, sizeof(buffer), formato, hora_local);
	if(longitud == 0) return NULL;
	char *folio = sistema_memoria_reservar(longitud + 1, NULL, 0);
	if(folio != NULL) memcpy(folio, buffer, longitud + 1);
	return folio;
#else
	(void)formato_fecha_hora;
	return NULL;
#endif
}

int desfragmentar_direccion(
	const char *direccion,
	char **retorna_directorios,
	char **retorna_nom_arch,
	char **retorna_extencion
)
{
	if(direccion == NULL || retorna_directorios == NULL ||
		retorna_nom_arch == NULL || retorna_extencion == NULL) return -1;
	*retorna_directorios = NULL;
	*retorna_nom_arch = NULL;
	*retorna_extencion = NULL;

	const char *nombre = direccion;
	for(const char *cursor = direccion; *cursor != '\0'; ++cursor)
	{
		if(*cursor == '/' || *cursor == '\\') nombre = cursor + 1;
	}
	const char *separador_ruta = nombre > direccion ? nombre - 1 : NULL;
	const char *punto = strrchr(nombre, '.');
	if(punto == nombre) punto = NULL;
	size_t longitud_directorio = separador_ruta != NULL ? (size_t)(separador_ruta - direccion) : 0;
	size_t longitud_nombre = (size_t)((punto != NULL ? punto : direccion + strlen(direccion)) - nombre);
	size_t longitud_extension = punto != NULL ? strlen(punto + 1) : 0;

	char *directorio = sistema_memoria_reservar(longitud_directorio + 1, NULL, 0);
	char *nombre_archivo = sistema_memoria_reservar(longitud_nombre + 1, NULL, 0);
	char *extension = sistema_memoria_reservar(longitud_extension + 1, NULL, 0);
	if(directorio == NULL || nombre_archivo == NULL || extension == NULL)
	{
		sistema_memoria_liberar(directorio, NULL, 0);
		sistema_memoria_liberar(nombre_archivo, NULL, 0);
		sistema_memoria_liberar(extension, NULL, 0);
		return -1;
	}
	if(longitud_directorio > 0) memcpy(directorio, direccion, longitud_directorio);
	directorio[longitud_directorio] = '\0';
	memcpy(nombre_archivo, nombre, longitud_nombre);
	nombre_archivo[longitud_nombre] = '\0';
	if(longitud_extension > 0) memcpy(extension, punto + 1, longitud_extension);
	extension[longitud_extension] = '\0';
	*retorna_directorios = directorio;
	*retorna_nom_arch = nombre_archivo;
	*retorna_extencion = extension;
	return 0;
}

static char *gg_reemplazar_texto(const char *texto, const char *buscar, const char *reemplazo)
{
	if(texto == NULL || buscar == NULL || reemplazo == NULL || buscar[0] == '\0') return NULL;
	size_t longitud_buscar = strlen(buscar);
	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL) return NULL;

	const char *cursor = texto;
	const char *encontrado;
	while((encontrado = strstr(cursor, buscar)) != NULL)
	{
		if(!gg_cadena_agregar_n(&salida, cursor, (size_t)(encontrado - cursor)) ||
			!gg_cadena_agregar(&salida, reemplazo))
		{
			gg_cadena_liberar(&salida);
			return NULL;
		}
		cursor = encontrado + longitud_buscar;
	}
	if(!gg_cadena_agregar(&salida, cursor))
	{
		gg_cadena_liberar(&salida);
		return NULL;
	}
	return gg_cadena_entregar(&salida);
}

char *ReemplazarCaracteres_de_texto_arreglo(
	const char *info,
	char **caracteres_sep,
	int n_sep,
	char **caracteres_sustitucion
)
{
	if(info == NULL || n_sep < 0 || (n_sep > 0 && (caracteres_sep == NULL || caracteres_sustitucion == NULL)))
		return NULL;
	char *resultado = variable_string("%s", info);
	if(resultado == NULL) return NULL;
	for(int i = 0; i < n_sep; ++i)
	{
		if(caracteres_sep[i] == NULL || caracteres_sustitucion[i] == NULL ||
			caracteres_sep[i][0] == '\0') continue;
		char *reemplazado = gg_reemplazar_texto(resultado, caracteres_sep[i], caracteres_sustitucion[i]);
		if(reemplazado == NULL)
		{
			sistema_memoria_liberar(resultado, NULL, 0);
			return NULL;
		}
		sistema_memoria_liberar(resultado, NULL, 0);
		resultado = reemplazado;
	}
	return resultado;
}

char *ReemplazarCaracteres_de_texto_string(const char *info, const char *sep, const char *sust)
{
	if(info == NULL) return NULL;
	if(sep == NULL || sust == NULL || sep[0] == '\0') return variable_string("%s", info);
	return gg_reemplazar_texto(info, sep, sust);
}

static int gg_indices_coinciden(char **campos, int cantidad_campos, const int *indices, int cantidad_indices, const char *valor)
{
	if(campos == NULL || indices == NULL || valor == NULL) return 0;
	for(int i = 0; i < cantidad_indices; ++i)
	{
		if(indices[i] < cantidad_campos && strcmp(campos[indices[i]], valor) == 0) return 1;
	}
	return 0;
}

static int gg_fila_cumple_comparaciones(const char *fila, const char *comparaciones)
{
	if(fila == NULL || comparaciones == NULL) return 0;
	int cantidad_condiciones = 0;
	char **condiciones = split(comparaciones, "|", &cantidad_condiciones, NULL, 0);
	if(condiciones == NULL) return 0;
	int cantidad_campos = 0;
	char **campos = split(fila, gg_separador_columnas(), &cantidad_campos, NULL, 0);
	if(campos == NULL)
	{
		liberarSplit(condiciones, cantidad_condiciones, NULL, 0);
		return 0;
	}

	int cumple = 1;
	for(int i = 0; i < cantidad_condiciones && cumple; ++i)
	{
		char *dos_puntos = strchr(condiciones[i], ':');
		if(dos_puntos == NULL)
		{
			cumple = 0;
			break;
		}
		*dos_puntos = '\0';
		int indice = -1;
		if(texto_a_int_seguro(condiciones[i], &indice) != 0 ||
			indice < 0 || indice >= cantidad_campos ||
			strcmp(campos[indice], dos_puntos + 1) != 0)
		{
			cumple = 0;
		}
	}
	liberarSplit(campos, cantidad_campos, NULL, 0);
	liberarSplit(condiciones, cantidad_condiciones, NULL, 0);
	return cumple;
}

char *busqueda_profunda_string(const char *texto, const char *columnas_recorrer, const char *comparar)
{
	if(texto == NULL || columnas_recorrer == NULL || comparar == NULL) return NULL;
	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(columnas_recorrer, "|", &indices, &cantidad_indices)) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL)
	{
		sistema_memoria_liberar(indices, NULL, 0);
		return NULL;
	}

	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL)
	{
		liberarSplit(filas, cantidad_filas, NULL, 0);
		sistema_memoria_liberar(indices, NULL, 0);
		return NULL;
	}
	int encontradas = 0;
	for(int i = 0; i < cantidad_filas; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(filas[i], gg_separador_columnas(), &cantidad_campos, NULL, 0);
		if(campos == NULL) goto error_busqueda;
		int coincide = gg_indices_coinciden(campos, cantidad_campos, indices, cantidad_indices, comparar);
		liberarSplit(campos, cantidad_campos, NULL, 0);
		if(!coincide) continue;
		if(encontradas > 0 && !gg_cadena_agregar(&salida, gg_separador_filas())) goto error_busqueda;
		if(!gg_cadena_agregar(&salida, filas[i])) goto error_busqueda;
		encontradas++;
	}
	liberarSplit(filas, cantidad_filas, NULL, 0);
	sistema_memoria_liberar(indices, NULL, 0);
	return gg_cadena_entregar(&salida);

error_busqueda:
	liberarSplit(filas, cantidad_filas, NULL, 0);
	sistema_memoria_liberar(indices, NULL, 0);
	gg_cadena_liberar(&salida);
	return NULL;
}

char *busqueda_profunda_comparacion_final_string(const char *texto, const char *columnas_recorrer, const char *comparar)
{
	if(texto == NULL || columnas_recorrer == NULL || comparar == NULL) return NULL;
	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(columnas_recorrer, "|", &indices, &cantidad_indices)) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL)
	{
		sistema_memoria_liberar(indices, NULL, 0);
		return NULL;
	}
	const char *ultima = NULL;
	for(int i = 0; i < cantidad_filas; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(filas[i], gg_separador_columnas(), &cantidad_campos, NULL, 0);
		if(campos == NULL)
		{
			liberarSplit(filas, cantidad_filas, NULL, 0);
			sistema_memoria_liberar(indices, NULL, 0);
			return NULL;
		}
		if(gg_indices_coinciden(campos, cantidad_campos, indices, cantidad_indices, comparar))
			ultima = filas[i];
		liberarSplit(campos, cantidad_campos, NULL, 0);
	}
	char *resultado = ultima != NULL ? variable_string("%s", ultima) : variable_string("%s", "");
	liberarSplit(filas, cantidad_filas, NULL, 0);
	sistema_memoria_liberar(indices, NULL, 0);
	return resultado;
}

char *busqueda_con_YY_profunda_texto_id_archivo(const char *texto, const char *columnas_recorrer, const char *comparaciones)
{
	(void)columnas_recorrer;
	if(texto == NULL || comparaciones == NULL) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL) return NULL;
	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL)
	{
		liberarSplit(filas, cantidad_filas, NULL, 0);
		return NULL;
	}
	int encontradas = 0;
	for(int i = 0; i < cantidad_filas; ++i)
	{
		if(!gg_fila_cumple_comparaciones(filas[i], comparaciones)) continue;
		if(encontradas > 0 && !gg_cadena_agregar(&salida, gg_separador_filas())) goto error_busqueda_yy;
		if(!gg_cadena_agregar(&salida, filas[i])) goto error_busqueda_yy;
		encontradas++;
	}
	liberarSplit(filas, cantidad_filas, NULL, 0);
	return gg_cadena_entregar(&salida);

error_busqueda_yy:
	liberarSplit(filas, cantidad_filas, NULL, 0);
	gg_cadena_liberar(&salida);
	return NULL;
}

static int gg_editar_campos(char **campos, int cantidad_campos, const int *indices, int cantidad_indices, const char *valor, int incrementar, const char *comparar)
{
	if(campos == NULL || (cantidad_indices > 0 && indices == NULL)) return 0;
	for(int i = 0; i < cantidad_indices; ++i)
	{
		int indice = indices[i];
		if(indice >= cantidad_campos) continue;
		if(comparar != NULL && strcmp(campos[indice], comparar) != 0) continue;

		char nuevo_valor[sizeof(int) * CHAR_BIT + 2];
		const char *texto_nuevo = valor != NULL ? valor : "";
		if(incrementar)
		{
			int actual = 0;
			if(texto_a_int_seguro(campos[indice], &actual) != 0 || actual == INT_MAX) return 0;
			if(enteroATexto(actual + 1, nuevo_valor, sizeof(nuevo_valor)) == NULL) return 0;
			texto_nuevo = nuevo_valor;
		}
		char *copia = variable_string("%s", texto_nuevo);
		if(copia == NULL) return 0;
		sistema_memoria_liberar(campos[indice], NULL, 0);
		campos[indice] = copia;
	}
	return 1;
}

static char *gg_reconstruir_fila(char **campos, int cantidad_campos)
{
	return join(campos, cantidad_campos, gg_separador_columnas(), NULL, 0);
}

static char *gg_editar_filas(
	const char *texto,
	const int *indices,
	int cantidad_indices,
	const char *valor,
	int incrementar,
	int fila_objetivo,
	const char *comparar_campos
)
{
	if(texto == NULL || cantidad_indices < 0) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL) return NULL;

	GG_CadenaDinamica salida;
	gg_cadena_iniciar(&salida);
	if(salida.datos == NULL)
	{
		liberarSplit(filas, cantidad_filas, NULL, 0);
		return NULL;
	}

	for(int i = 0; i < cantidad_filas; ++i)
	{
		if(i > 0 && !gg_cadena_agregar(&salida, gg_separador_filas())) goto error_editar_filas;
		if((fila_objetivo == -1 || fila_objetivo == i) && cantidad_indices > 0)
		{
			int cantidad_campos = 0;
			char **campos = split(filas[i], gg_separador_columnas(), &cantidad_campos, NULL, 0);
			if(campos == NULL) goto error_editar_filas;
			if(!gg_editar_campos(campos, cantidad_campos, indices, cantidad_indices, valor, incrementar, comparar_campos))
			{
				liberarSplit(campos, cantidad_campos, NULL, 0);
				goto error_editar_filas;
			}
			char *fila_editada = gg_reconstruir_fila(campos, cantidad_campos);
			liberarSplit(campos, cantidad_campos, NULL, 0);
			if(fila_editada == NULL) goto error_editar_filas;
			int agregada = gg_cadena_agregar(&salida, fila_editada);
			sistema_memoria_liberar(fila_editada, NULL, 0);
			if(!agregada) goto error_editar_filas;
		}
		else if(!gg_cadena_agregar(&salida, filas[i]))
		{
			goto error_editar_filas;
		}
	}
	liberarSplit(filas, cantidad_filas, NULL, 0);
	return gg_cadena_entregar(&salida);

error_editar_filas:
	liberarSplit(filas, cantidad_filas, NULL, 0);
	gg_cadena_liberar(&salida);
	return NULL;
}

char *editar_incr_string_funcion_recursiva(
	const char *texto,
	const char *columnas_recorrer,
	const char *info_sustituir,
	const char *edit_0_increm_1
)
{
	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(columnas_recorrer, "|", &indices, &cantidad_indices)) return NULL;
	int incrementar = edit_0_increm_1 != NULL && strcmp(edit_0_increm_1, "1") == 0;
	char *resultado = gg_editar_filas(texto, indices, cantidad_indices, info_sustituir, incrementar, -1, NULL);
	sistema_memoria_liberar(indices, NULL, 0);
	return resultado;
}

char *editar_inc_edicion_profunda_multiple_string(
	const char *texto,
	const char *indices_editar,
	const char *info_editar,
	const char *edit_0_increm_1
)
{
	return editar_incr_string_funcion_recursiva(texto, indices_editar, info_editar, edit_0_increm_1);
}

char *editar_inc_edicion_profunda_multiple_AL_FINAL_string(
	const char *texto,
	const char *indices_editar,
	const char *info_editar,
	const char *edit_0_increm_1
)
{
	if(texto == NULL) return NULL;
	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(indices_editar, "|", &indices, &cantidad_indices)) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL)
	{
		sistema_memoria_liberar(indices, NULL, 0);
		return NULL;
	}
	int incrementar = edit_0_increm_1 != NULL && strcmp(edit_0_increm_1, "1") == 0;
	char *resultado = gg_editar_filas(
		texto, indices, cantidad_indices, info_editar, incrementar,
		cantidad_filas > 0 ? cantidad_filas - 1 : -1, NULL
	);
	liberarSplit(filas, cantidad_filas, NULL, 0);
	sistema_memoria_liberar(indices, NULL, 0);
	return resultado;
}

char *editar_inc_agregar_edicion_profunda_multiple_comparacion_final_string(
	const char *texto,
	const char *indices_editar,
	const char *info_editar,
	const char *comparacion,
	const char *edit_0_increm_1
)
{
	if(texto == NULL || comparacion == NULL) return NULL;
	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(indices_editar, "|", &indices, &cantidad_indices)) return NULL;
	int cantidad_filas = 0;
	char **filas = split(texto, gg_separador_filas(), &cantidad_filas, NULL, 0);
	if(filas == NULL)
	{
		sistema_memoria_liberar(indices, NULL, 0);
		return NULL;
	}
	int objetivo = -2;
	for(int i = 0; i < cantidad_filas; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(filas[i], gg_separador_columnas(), &cantidad_campos, NULL, 0);
		if(campos == NULL)
		{
			liberarSplit(filas, cantidad_filas, NULL, 0);
			sistema_memoria_liberar(indices, NULL, 0);
			return NULL;
		}
		if(gg_indices_coinciden(campos, cantidad_campos, indices, cantidad_indices, comparacion))
			objetivo = i;
		liberarSplit(campos, cantidad_campos, NULL, 0);
	}
	int incrementar = edit_0_increm_1 != NULL && strcmp(edit_0_increm_1, "1") == 0;
	char *resultado = gg_editar_filas(texto, indices, cantidad_indices, info_editar, incrementar, objetivo, NULL);
	liberarSplit(filas, cantidad_filas, NULL, 0);
	sistema_memoria_liberar(indices, NULL, 0);
	return resultado;
}

char *ARR_FUN_SOLO_TEXTO_editar_inc_agregar_edicion_profunda_multiple(const char *datos)
{
	if(datos == NULL) return NULL;
	int cantidad = 0;
	char **partes = split(datos, "|", &cantidad, NULL, 0);
	if(partes == NULL) return NULL;
	if(cantidad < 3)
	{
		liberarSplit(partes, cantidad, NULL, 0);
		return variable_string("%s", "");
	}
	char *resultado = editar_incr_string_funcion_recursiva(
		partes[0],
		partes[1],
		partes[2],
		cantidad > 3 ? partes[3] : "0"
	);
	liberarSplit(partes, cantidad, NULL, 0);
	return resultado;
}

char *editar_inc_agregar_edicion_profunda_multiple_comparacion_MULTIPLE_A_CHECAR(
	const char *texto,
	const char *indices_editar,
	const char *comparacion_con_edicion,
	const char *edit_0_increm_1
)
{
	if(texto == NULL || indices_editar == NULL || comparacion_con_edicion == NULL) return NULL;
	char *separador = strchr(comparacion_con_edicion, ',');
	char *comparar = NULL;
	const char *valor_editar = "";
	if(separador != NULL)
	{
		size_t longitud_comparar = (size_t)(separador - comparacion_con_edicion);
		comparar = sistema_memoria_reservar(longitud_comparar + 1, NULL, 0);
		if(comparar == NULL) return NULL;
		memcpy(comparar, comparacion_con_edicion, longitud_comparar);
		comparar[longitud_comparar] = '\0';
		valor_editar = separador + 1;
	}
	else
	{
		comparar = variable_string("%s", comparacion_con_edicion);
		if(comparar == NULL) return NULL;
	}

	int *indices = NULL;
	int cantidad_indices = 0;
	if(!gg_indices_desde_texto(indices_editar, "|", &indices, &cantidad_indices))
	{
		sistema_memoria_liberar(comparar, NULL, 0);
		return NULL;
	}
	int incrementar = edit_0_increm_1 != NULL && strcmp(edit_0_increm_1, "1") == 0;
	char *resultado = gg_editar_filas(
		texto, indices, cantidad_indices, valor_editar, incrementar, -1, comparar
	);
	sistema_memoria_liberar(indices, NULL, 0);
	sistema_memoria_liberar(comparar, NULL, 0);
	return resultado;
}

char *recorrer_caracter_separacion(const char *contenidoFila, const char *izquierda_o_derecha, int numero_veses)
{
	if(contenidoFila == NULL || numero_veses < 0) return NULL;
	int cantidad = 0;
	char **partes = split(contenidoFila, gg_separador_filas(), &cantidad, NULL, 0);
	if(partes == NULL) return NULL;
	int izquierda = izquierda_o_derecha != NULL && strcmp(izquierda_o_derecha, "izq") == 0;
	int inicio = izquierda ? numero_veses : 0;
	int fin = izquierda ? cantidad : cantidad - numero_veses;
	if(inicio > cantidad) inicio = cantidad;
	if(fin < inicio) fin = inicio;
	char *resultado = gg_unir_partes(partes + inicio, fin - inicio, gg_separador_filas());
	liberarSplit(partes, cantidad, NULL, 0);
	return resultado;
}

char *recorrer_caracter_separacion_funciones_espesificas(
	const char *contenidoFila,
	const char *izquierda_o_derecha,
	int numero_veses
)
{
	if(contenidoFila == NULL || numero_veses < 0) return NULL;
	const char *separador = GG_caracter_separacion_funciones_espesificas[0];
	if(separador == NULL || separador[0] == '\0') separador = gg_separador_filas();
	int cantidad = 0;
	char **partes = split(contenidoFila, separador, &cantidad, NULL, 0);
	if(partes == NULL) return NULL;
	int izquierda = izquierda_o_derecha != NULL && strcmp(izquierda_o_derecha, "izq") == 0;
	int inicio = izquierda ? numero_veses : 0;
	int fin = izquierda ? cantidad : cantidad - numero_veses;
	if(inicio > cantidad) inicio = cantidad;
	if(fin < inicio) fin = inicio;
	char *resultado = gg_unir_partes(partes + inicio, fin - inicio, separador);
	liberarSplit(partes, cantidad, NULL, 0);
	return resultado;
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

/* ============================================================================
   ALMACENAMIENTO DIVIDIDO
   ============================================================================ */
#pragma region FUNCIONES_INFO_DIVIDIDA

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)

static char *gg_buscar_ultimo_separador_ruta(char *ruta)
{
	char *barra = strrchr(ruta, '/');
	char *barra_inversa = strrchr(ruta, '\\');
	if(barra == NULL) return barra_inversa;
	if(barra_inversa == NULL) return barra;
	return barra > barra_inversa ? barra : barra_inversa;
}

static int gg_es_directorio(const char *ruta)
{
#if defined(PLATAFORMA_WINDOWS)
	struct _stat estado;
	return _stat(ruta, &estado) == 0 && (estado.st_mode & _S_IFDIR) != 0;
#else
	struct stat estado;
	return stat(ruta, &estado) == 0 && S_ISDIR(estado.st_mode);
#endif
}

static int gg_crear_directorio_uno(const char *ruta)
{
#if defined(PLATAFORMA_WINDOWS)
	if(_mkdir(ruta) == 0 || gg_es_directorio(ruta)) return 0;
#else
	if(mkdir(ruta, 0777) == 0 || gg_es_directorio(ruta)) return 0;
#endif
	return -1;
}

int crearDirectorio(const char *ruta)
{
	if(ruta == NULL || ruta[0] == '\0') return -1;
	char *copia = variable_string("%s", ruta);
	if(copia == NULL) return -1;

	for(char *cursor = copia; *cursor != '\0'; ++cursor)
	{
#if defined(PLATAFORMA_WINDOWS)
		if(*cursor == '/') *cursor = '\\';
		if(*cursor != '\\') continue;
		if(cursor == copia || (cursor == copia + 2 && copia[1] == ':')) continue;
#else
		if(*cursor == '\\') *cursor = '/';
		if(*cursor != '/' || cursor == copia) continue;
#endif
		char separador = *cursor;
		*cursor = '\0';
		int error = copia[0] != '\0' && gg_crear_directorio_uno(copia) != 0;
		*cursor = separador;
		if(error)
		{
			sistema_memoria_liberar(copia, NULL, 0);
			return -1;
		}
	}

	int resultado = gg_crear_directorio_uno(copia);
	sistema_memoria_liberar(copia, NULL, 0);
	return resultado;
}

static int gg_crear_directorio_padre(const char *ruta)
{
	char *copia = variable_string("%s", ruta);
	if(copia == NULL) return -1;
	char *separador = gg_buscar_ultimo_separador_ruta(copia);
	if(separador == NULL)
	{
		sistema_memoria_liberar(copia, NULL, 0);
		return 0;
	}
#if defined(PLATAFORMA_WINDOWS)
	if(separador == copia + 2 && copia[1] == ':')
	{
		sistema_memoria_liberar(copia, NULL, 0);
		return 0;
	}
#endif
	if(separador == copia)
	{
		separador[1] = '\0';
	}
	else
	{
		*separador = '\0';
	}
	int resultado = crearDirectorio(copia);
	sistema_memoria_liberar(copia, NULL, 0);
	return resultado;
}

int existe_archivo(const char *ruta)
{
	if(ruta == NULL) return 0;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "r", NULL, 0);
	if(archivo == NULL) return 0;
	return sistema_archivo_cerrar(archivo, NULL, 0) == 0;
}

int crearArchivo(const char *ruta, const char *cabecera)
{
	if(ruta == NULL || ruta[0] == '\0') return -1;
	if(existe_archivo(ruta)) return 1;
	if(gg_crear_directorio_padre(ruta) != 0) return -1;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "w", NULL, 0);
	if(archivo == NULL) return -1;
	int error = cabecera != NULL && escribirLineaArchivo(archivo, cabecera, NULL, 0) != 0;
	if(sistema_archivo_cerrar(archivo, NULL, 0) != 0) error = 1;
	return error ? -1 : 0;
}

void free_lineas(char **lineas, int n_lineas)
{
	if(lineas == NULL) return;
	for(int i = 0; i < n_lineas; ++i)
		sistema_memoria_liberar(lineas[i], NULL, 0);
	sistema_memoria_liberar(lineas, NULL, 0);
}

char **leer_archivo(const char *ruta, int *n_lineas_out)
{
	if(n_lineas_out == NULL) return NULL;
	*n_lineas_out = 0;
	if(ruta == NULL) return NULL;

	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "r", NULL, 0);
	if(archivo == NULL) return NULL;

	int capacidad = 8;
	int cantidad = 0;
	char **lineas = sistema_memoria_reservar((size_t)capacidad * sizeof(*lineas), NULL, 0);
	if(lineas == NULL)
	{
		sistema_archivo_cerrar(archivo, NULL, 0);
		return NULL;
	}

	char *linea;
	while((linea = leerLineaDinamica(archivo, NULL, 0)) != NULL)
	{
		if(cantidad == capacidad)
		{
			if(capacidad > INT_MAX / 2)
			{
				sistema_memoria_liberar(linea, NULL, 0);
				free_lineas(lineas, cantidad);
				sistema_archivo_cerrar(archivo, NULL, 0);
				return NULL;
			}
			int nueva_capacidad = capacidad * 2;
			char **temporal = sistema_memoria_redimensionar(
				lineas, (size_t)nueva_capacidad * sizeof(*lineas), NULL, 0);
			if(temporal == NULL)
			{
				sistema_memoria_liberar(linea, NULL, 0);
				free_lineas(lineas, cantidad);
				sistema_archivo_cerrar(archivo, NULL, 0);
				return NULL;
			}
			lineas = temporal;
			capacidad = nueva_capacidad;
		}
		lineas[cantidad++] = linea;
	}

	int error = sistema_archivo_hay_error(archivo, NULL, 0);
	if(sistema_archivo_cerrar(archivo, NULL, 0) != 0) error = 1;
	if(error)
	{
		free_lineas(lineas, cantidad);
		return NULL;
	}
	*n_lineas_out = cantidad;
	return lineas;
}

static int gg_guardar_archivo(const char *ruta, char **lineas, int n_lineas)
{
	if(ruta == NULL || n_lineas < 0 || (n_lineas > 0 && lineas == NULL)) return -1;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "w", NULL, 0);
	if(archivo == NULL) return -1;
	int error = 0;
	for(int i = 0; i < n_lineas; ++i)
	{
		if(lineas[i] == NULL || escribirLineaArchivo(archivo, lineas[i], NULL, 0) != 0)
		{
			error = 1;
			break;
		}
	}
	if(sistema_archivo_hay_error(archivo, NULL, 0)) error = 1;
	if(sistema_archivo_cerrar(archivo, NULL, 0) != 0) error = 1;
	return error ? -1 : 0;
}

void guardar_archivo(const char *ruta, char **lineas, int n_lineas)
{
	(void)gg_guardar_archivo(ruta, lineas, n_lineas);
}

static int gg_agregar_fila(const char *ruta, const char *fila)
{
	if(ruta == NULL || fila == NULL) return -1;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "a", NULL, 0);
	if(archivo == NULL) return -1;
	int error = escribirLineaArchivo(archivo, fila, NULL, 0) != 0;
	if(sistema_archivo_hay_error(archivo, NULL, 0)) error = 1;
	if(sistema_archivo_cerrar(archivo, NULL, 0) != 0) error = 1;
	return error ? -1 : 0;
}

void agregar_fila(const char *ruta, const char *fila)
{
	(void)gg_agregar_fila(ruta, fila);
}

void generar_ruta_archivo(const char *ruta, long id, long cant_por_arch, char *salida, size_t tam_salida)
{
	if(salida == NULL || tam_salida == 0) return;
	salida[0] = '\0';
	if(cant_por_arch <= 0) return;

	char *id_texto = variable_string("%ld", id);
	char *cantidad_texto = variable_string("%ld", cant_por_arch);
	if(id_texto == NULL || cantidad_texto == NULL)
	{
		sistema_memoria_liberar(id_texto, NULL, 0);
		sistema_memoria_liberar(cantidad_texto, NULL, 0);
		return;
	}

	GG_CadenaDinamica construida;
	gg_cadena_iniciar(&construida);
	if(construida.datos == NULL) goto terminar_ruta_archivo;

	const char *separador = "\\";
#if defined(PLATAFORMA_LINUX)
	separador = "/";
#endif
	if(ruta != NULL && ruta[0] != '\0')
	{
		size_t largo = strlen(ruta);
		if(!gg_cadena_agregar(&construida, ruta)) goto error_ruta_archivo;
		if(ruta[largo - 1] != '/' && ruta[largo - 1] != '\\' &&
			!gg_cadena_agregar(&construida, separador)) goto error_ruta_archivo;
	}

	size_t largo_id = strlen(id_texto);
	size_t largo_cantidad = strlen(cantidad_texto);
	size_t ceros = largo_cantidad > 0 ? largo_cantidad - 1 : 0;
	long potencia = cant_por_arch;
	for(size_t digito = 2; digito <= largo_id; ++digito)
	{
		char *segmento = variable_string("%ld%s", potencia, separador);
		if(segmento == NULL || !gg_cadena_agregar(&construida, segmento))
		{
			sistema_memoria_liberar(segmento, NULL, 0);
			goto error_ruta_archivo;
		}
		sistema_memoria_liberar(segmento, NULL, 0);
		if(potencia > LONG_MAX / cant_por_arch) goto error_ruta_archivo;
		potencia *= cant_por_arch;
	}

	size_t prefijo = largo_id > ceros ? largo_id - ceros : 0;
	if(!gg_cadena_agregar_n(&construida, id_texto, prefijo)) goto error_ruta_archivo;
	for(size_t i = 0; i < ceros; ++i)
		if(!gg_cadena_agregar(&construida, "0")) goto error_ruta_archivo;
	if(!gg_cadena_agregar(&construida, ".txt")) goto error_ruta_archivo;
	if(construida.longitud < tam_salida)
		memcpy(salida, construida.datos, construida.longitud + 1);
	goto terminar_ruta_archivo;

error_ruta_archivo:
	gg_cadena_liberar(&construida);
	goto terminar_ruta_archivo;
terminar_ruta_archivo:
	if(construida.datos != NULL) gg_cadena_liberar(&construida);
	sistema_memoria_liberar(id_texto, NULL, 0);
	sistema_memoria_liberar(cantidad_texto, NULL, 0);
}

static char *gg_carpeta_data(const char *ruta_metadata)
{
	char *copia = variable_string("%s", ruta_metadata);
	if(copia == NULL) return NULL;
	char *separador = gg_buscar_ultimo_separador_ruta(copia);
	char *punto = strrchr(copia, '.');
	if(punto != NULL && (separador == NULL || punto > separador)) *punto = '\0';
	char *resultado = variable_string("%s_DAT", copia);
	sistema_memoria_liberar(copia, NULL, 0);
	return resultado;
}

static int gg_parse_long_info(const char *texto, long *valor)
{
	if(texto == NULL || valor == NULL || texto[0] == '\0') return 0;
	char *fin = NULL;
	long convertido = strtol(texto, &fin, 10);
	if(fin == texto || *fin != '\0') return 0;
	*valor = convertido;
	return 1;
}

static int gg_leer_config_metadata(const char *ruta, long *id_total, long *cantidad_por_archivo)
{
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL) return -1;
	long id = 0;
	long por_archivo = 100;
	for(int i = 0; i < cantidad; ++i)
	{
		char **partes = NULL;
		int n = split_con_numero_de_celdas(lineas[i], GG_caracter_separacion[0], &partes);
		if(n >= 2 && partes != NULL)
		{
			if(strcmp(partes[0], "ID_TOT") == 0)
			{
				long convertido = 0;
				if(gg_parse_long_info(partes[1], &convertido) && convertido >= 0) id = convertido;
			}
			else if(strcmp(partes[0], "CANT_POR_ARCH") == 0)
			{
				long convertido = 0;
				if(gg_parse_long_info(partes[1], &convertido) && convertido > 0) por_archivo = convertido;
			}
		}
		if(partes != NULL) free_split_con_numero_de_celdas(partes);
	}
	free_lineas(lineas, cantidad);
	if(id_total != NULL) *id_total = id;
	if(cantidad_por_archivo != NULL) *cantidad_por_archivo = por_archivo;
	return 0;
}

static int gg_leer_columnas_metadata(const char *ruta, char **columnas_out)
{
	if(columnas_out == NULL) return -1;
	*columnas_out = NULL;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL) return -1;
	for(int i = 0; i < cantidad; ++i)
	{
		char **partes = NULL;
		int n = split_con_numero_de_celdas(lineas[i], GG_caracter_separacion[0], &partes);
		if(n >= 2 && partes != NULL && strcmp(partes[0], "COLUMNAS") == 0)
		{
			*columnas_out = variable_string("%s", partes[1]);
			free_split_con_numero_de_celdas(partes);
			free_lineas(lineas, cantidad);
			return *columnas_out != NULL ? 0 : -1;
		}
		if(partes != NULL) free_split_con_numero_de_celdas(partes);
	}
	free_lineas(lineas, cantidad);
	return -1;
}

static int gg_guardar_metadata_id(const char *ruta, long nuevo_id)
{
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL) return -1;
	char *texto_id = variable_string("%ld", nuevo_id);
	if(texto_id == NULL)
	{
		free_lineas(lineas, cantidad);
		return -1;
	}
	int encontrado = 0;
	for(int i = 0; i < cantidad; ++i)
	{
		char **partes = NULL;
		int n = split_con_numero_de_celdas(lineas[i], GG_caracter_separacion[0], &partes);
		if(n >= 2 && partes != NULL && strcmp(partes[0], "ID_TOT") == 0)
		{
			char *nueva_linea = variable_string("ID_TOT%s%s", GG_caracter_separacion[0], texto_id);
			if(nueva_linea == NULL)
			{
				free_split_con_numero_de_celdas(partes);
				sistema_memoria_liberar(texto_id, NULL, 0);
				free_lineas(lineas, cantidad);
				return -1;
			}
			sistema_memoria_liberar(lineas[i], NULL, 0);
			lineas[i] = nueva_linea;
			encontrado = 1;
		}
		if(partes != NULL) free_split_con_numero_de_celdas(partes);
	}
	sistema_memoria_liberar(texto_id, NULL, 0);
	int resultado = encontrado ? gg_guardar_archivo(ruta, lineas, cantidad) : -1;
	free_lineas(lineas, cantidad);
	return resultado;
}

static char *gg_ruta_data_por_id(const char *ruta_metadata, long id, long cantidad_por_archivo)
{
	char *carpeta = gg_carpeta_data(ruta_metadata);
	if(carpeta == NULL) return NULL;
	size_t capacidad = strlen(carpeta) + 512;
	char *ruta = sistema_memoria_reservar(capacidad, NULL, 0);
	if(ruta != NULL)
	{
		generar_ruta_archivo(carpeta, id, cantidad_por_archivo, ruta, capacidad);
		if(ruta[0] == '\0')
		{
			sistema_memoria_liberar(ruta, NULL, 0);
			ruta = NULL;
		}
	}
	sistema_memoria_liberar(carpeta, NULL, 0);
	return ruta;
}

static char *gg_reconstruir_fila_info(char **celdas, int cantidad)
{
	char *resultado = NULL;
	for(int i = 0; i < cantidad; ++i)
	{
		const char *valor = celdas[i] != NULL ? celdas[i] : "";
		int estado = i == 0
			? concatenar_formato_separado_por_variable(&resultado, NULL, "%s", valor)
			: concatenar_formato_separado_por_variable(&resultado, NULL, "%s%s", GG_caracter_separacion[0], valor);
		if(estado < 0)
		{
			sistema_memoria_liberar(resultado, NULL, 0);
			return NULL;
		}
	}
	return resultado != NULL ? resultado : variable_string("");
}

static char *gg_info_respuesta(int codigo, const char *mensaje)
{
	return variable_string("%d%s%s", codigo, GG_caracter_para_confirmacion_o_error[0], mensaje);
}

static char *gg_info_respuesta_fila(int codigo, const char *fila)
{
	return variable_string("%d%s%s", codigo, GG_caracter_para_confirmacion_o_error[0], fila != NULL ? fila : "");
}

static int gg_crear_metadata_info(const char *ruta, const char *columnas)
{
	if(ruta == NULL || ruta[0] == '\0') return -1;
	char *cabecera = variable_string(
		"tipo_info%sinfo\nID_TOT%s0\nCOLUMNAS%s%s\nCANT_POR_ARCH%s100",
		GG_caracter_separacion[0], GG_caracter_separacion[0],
		GG_caracter_separacion[0], columnas != NULL ? columnas : "",
		GG_caracter_separacion[0]);
	if(cabecera == NULL) return -1;
	int resultado = crearArchivo(ruta, cabecera);
	sistema_memoria_liberar(cabecera, NULL, 0);
	return resultado;
}

char *leer_info_dividida(const char *ruta)
{
	if(ruta == NULL) return NULL;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL) return NULL;
	GG_CadenaDinamica resultado;
	gg_cadena_iniciar(&resultado);
	if(resultado.datos == NULL)
	{
		free_lineas(lineas, cantidad);
		return NULL;
	}
	for(int i = 0; i < cantidad; ++i)
	{
		if(i > 0 && !gg_cadena_agregar(&resultado, "\n")) goto error_leer_info;
		if(!gg_cadena_agregar(&resultado, lineas[i])) goto error_leer_info;
	}
	free_lineas(lineas, cantidad);
	return gg_cadena_entregar(&resultado);

error_leer_info:
	free_lineas(lineas, cantidad);
	gg_cadena_liberar(&resultado);
	return NULL;
}

char *agregar_info_dividida(const char *direccion, const char *agregando, const char *nom_columnas)
{
	if(direccion == NULL || direccion[0] == '\0') return gg_info_respuesta(0, "parametros_invalidos");
	if(!existe_archivo(direccion) && gg_crear_metadata_info(direccion, nom_columnas) < 0)
		return gg_info_respuesta(0, "error_no_pudo_crear_metadata");

	char *columnas_metadata = NULL;
	const char *columnas = nom_columnas;
	if(columnas == NULL)
	{
		if(gg_leer_columnas_metadata(direccion, &columnas_metadata) != 0)
			return gg_info_respuesta(0, "error_no_pudo_leer_metadata");
		columnas = columnas_metadata;
	}

	long id_total = 0;
	long por_archivo = 100;
	if(gg_leer_config_metadata(direccion, &id_total, &por_archivo) != 0 || id_total == LONG_MAX)
	{
		sistema_memoria_liberar(columnas_metadata, NULL, 0);
		return gg_info_respuesta(0, "error_no_pudo_leer_metadata");
	}

	char *ruta_data = gg_ruta_data_por_id(direccion, id_total + 1, por_archivo);
	if(ruta_data == NULL)
	{
		sistema_memoria_liberar(columnas_metadata, NULL, 0);
		return gg_info_respuesta(0, "error_de_memoria");
	}

	char *columnas_archivo = ReemplazarCaracteres_de_texto_string(
		columnas,
		GG_caracter_separacion[1], GG_caracter_separacion[0]);
	if(columnas_archivo == NULL)
	{
		sistema_memoria_liberar(columnas_metadata, NULL, 0);
		sistema_memoria_liberar(ruta_data, NULL, 0);
		return gg_info_respuesta(0, "error_de_memoria");
	}
	int estado_archivo = crearArchivo(ruta_data, columnas_archivo);
	sistema_memoria_liberar(columnas_archivo, NULL, 0);
	sistema_memoria_liberar(columnas_metadata, NULL, 0);
	if(estado_archivo < 0)
	{
		sistema_memoria_liberar(ruta_data, NULL, 0);
		return gg_info_respuesta(0, "error_no_pudo_crear_archivo_de_datos");
	}

	char *fila = variable_string("%ld%s%s", id_total + 1, GG_caracter_separacion[0], agregando != NULL ? agregando : "");
	if(fila == NULL || gg_agregar_fila(ruta_data, fila) != 0 ||
		gg_guardar_metadata_id(direccion, id_total + 1) != 0)
	{
		sistema_memoria_liberar(fila, NULL, 0);
		sistema_memoria_liberar(ruta_data, NULL, 0);
		return gg_info_respuesta(0, "error_al_guardar");
	}
	char *respuesta = gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	return respuesta;
}

char *leer_todo_info_dividida(const char *ruta)
{
	long id_total = 0;
	long por_archivo = 100;
	if(ruta == NULL || gg_leer_config_metadata(ruta, &id_total, &por_archivo) != 0) return NULL;
	char *resultado = NULL;
	char *ruta_anterior = NULL;
	long registros_agregados = 0;
	for(long id = 1; id <= id_total && registros_agregados < id_total; ++id)
	{
		char *ruta_data = gg_ruta_data_por_id(ruta, id, por_archivo);
		if(ruta_data == NULL) goto error_leer_todo;
		if(ruta_anterior != NULL && strcmp(ruta_anterior, ruta_data) == 0)
		{
			sistema_memoria_liberar(ruta_data, NULL, 0);
			continue;
		}
		sistema_memoria_liberar(ruta_anterior, NULL, 0);
		ruta_anterior = variable_string("%s", ruta_data);
		if(ruta_anterior == NULL)
		{
			sistema_memoria_liberar(ruta_data, NULL, 0);
			goto error_leer_todo;
		}

		int cantidad = 0;
		char **lineas = leer_archivo(ruta_data, &cantidad);
		sistema_memoria_liberar(ruta_data, NULL, 0);
		if(lineas == NULL) goto error_leer_todo;
		for(int fila = cantidad > 0 ? 1 : 0; fila < cantidad && registros_agregados < id_total; ++fila)
		{
			if(resultado != NULL &&
				concatenar_formato_separado_por_variable(&resultado, NULL, "%s%s",
					GG_caracter_separacion_funciones_espesificas[3], lineas[fila]) < 0)
			{
				free_lineas(lineas, cantidad);
				goto error_leer_todo;
			}
			if(resultado == NULL &&
				concatenar_formato_separado_por_variable(&resultado, NULL, "%s", lineas[fila]) < 0)
			{
				free_lineas(lineas, cantidad);
				goto error_leer_todo;
			}
			++registros_agregados;
		}
		free_lineas(lineas, cantidad);
	}
	sistema_memoria_liberar(ruta_anterior, NULL, 0);
	return resultado != NULL ? resultado : variable_string("");

error_leer_todo:
	sistema_memoria_liberar(ruta_anterior, NULL, 0);
	sistema_memoria_liberar(resultado, NULL, 0);
	return NULL;
}

static int gg_buscar_fila_por_id(const char *ruta_metadata, const char *id, char **ruta_data_out, int *indice_out, char **fila_out)
{
	if(ruta_data_out != NULL) *ruta_data_out = NULL;
	if(indice_out != NULL) *indice_out = -1;
	if(fila_out != NULL) *fila_out = NULL;
	if(ruta_metadata == NULL || id == NULL || id[0] == '\0') return -1;
	long id_numero = 0;
	if(!gg_parse_long_info(id, &id_numero) || id_numero < 0) return -1;
	long por_archivo = 100;
	if(gg_leer_config_metadata(ruta_metadata, NULL, &por_archivo) != 0) return -1;
	char *ruta_data = gg_ruta_data_por_id(ruta_metadata, id_numero, por_archivo);
	if(ruta_data == NULL) return -1;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta_data, &cantidad);
	if(lineas == NULL)
	{
		sistema_memoria_liberar(ruta_data, NULL, 0);
		return -1;
	}
	int encontrado = -1;
	for(int i = 0; i < cantidad; ++i)
	{
		char **celdas = NULL;
		int n = split_con_numero_de_celdas(lineas[i], GG_caracter_separacion[0], &celdas);
		if(n > 0 && celdas != NULL && strcmp(celdas[0], id) == 0)
		{
			encontrado = i;
			if(fila_out != NULL) *fila_out = variable_string("%s", lineas[i]);
			break;
		}
		if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
	}
	free_lineas(lineas, cantidad);
	if(encontrado < 0 || (fila_out != NULL && *fila_out == NULL))
	{
		sistema_memoria_liberar(ruta_data, NULL, 0);
		return -1;
	}
	if(ruta_data_out != NULL) *ruta_data_out = ruta_data;
	else sistema_memoria_liberar(ruta_data, NULL, 0);
	if(indice_out != NULL) *indice_out = encontrado;
	return 0;
}

char *seleccionar_id_info_dividida(const char *direccion, const char *id)
{
	char *fila = NULL;
	if(gg_buscar_fila_por_id(direccion, id, NULL, NULL, &fila) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	return respuesta;
}

char *seleccionar_id_info_dividida_extrae(const char *direccion, const char *id)
{
	char *fila = NULL;
	char *ruta_data = NULL;
	int indice = -1;
	if(gg_buscar_fila_por_id(direccion, id, &ruta_data, &indice, &fila) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = variable_string("1%s%s%s%s%s%d",
		GG_caracter_para_confirmacion_o_error[0], fila,
		GG_caracter_para_confirmacion_o_error[0], ruta_data,
		GG_caracter_para_confirmacion_o_error[0], indice);
	sistema_memoria_liberar(fila, NULL, 0);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	return respuesta;
}

static int gg_editar_campos_en_archivo(const char *ruta_data, int indice_fila, char **indices, char **valores, int cantidad)
{
	int n_lineas = 0;
	char **lineas = leer_archivo(ruta_data, &n_lineas);
	if(lineas == NULL || indice_fila < 0 || indice_fila >= n_lineas)
	{
		free_lineas(lineas, n_lineas);
		return -1;
	}
	char **celdas = NULL;
	int n_celdas = split_con_numero_de_celdas(lineas[indice_fila], GG_caracter_separacion[0], &celdas);
	if(n_celdas <= 0 || celdas == NULL)
	{
		free_lineas(lineas, n_lineas);
		return -1;
	}
	for(int i = 0; i < cantidad; ++i)
	{
		int columna = -1;
		if(indices == NULL || valores == NULL || texto_a_int_seguro(indices[i], &columna) != 0 ||
			columna < 0 || columna >= n_celdas) continue;
		char *nuevo = variable_string("%s", valores[i]);
		if(nuevo == NULL)
		{
			free_split_con_numero_de_celdas(celdas);
			free_lineas(lineas, n_lineas);
			return -1;
		}
		sistema_memoria_liberar(celdas[columna], NULL, 0);
		celdas[columna] = nuevo;
	}
	char *nueva_fila = gg_reconstruir_fila_info(celdas, n_celdas);
	free_split_con_numero_de_celdas(celdas);
	if(nueva_fila == NULL)
	{
		free_lineas(lineas, n_lineas);
		return -1;
	}
	sistema_memoria_liberar(lineas[indice_fila], NULL, 0);
	lineas[indice_fila] = nueva_fila;
	int resultado = gg_guardar_archivo(ruta_data, lineas, n_lineas);
	free_lineas(lineas, n_lineas);
	return resultado;
}

char *editar_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *vals_str)
{
	char *ruta_data = NULL;
	int indice = -1;
	if(gg_buscar_fila_por_id(direccion, id, &ruta_data, &indice, NULL) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	char **indices = NULL;
	char **valores = NULL;
	int n_indices = split_con_numero_de_celdas(cols_str, GG_caracter_separacion_funciones_espesificas[4], &indices);
	int n_valores = split_con_numero_de_celdas(vals_str, GG_caracter_separacion_funciones_espesificas[4], &valores);
	int n = n_indices < n_valores ? n_indices : n_valores;
	int resultado = n > 0 && indices != NULL && valores != NULL
		? gg_editar_campos_en_archivo(ruta_data, indice, indices, valores, n) : -1;
	if(indices != NULL) free_split_con_numero_de_celdas(indices);
	if(valores != NULL) free_split_con_numero_de_celdas(valores);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	return gg_info_respuesta(resultado == 0 ? 1 : 0,
		resultado == 0 ? "informacion_editada" : "no_se_encontro_informacion");
}

char *editar_id_info_dividida(const char *direccion, const char *id, const char *info_a_comparar, const char *col_comparar, const char *nuevos_datos, int col_editar)
{
	char *ruta_data = NULL;
	int indice = -1;
	char *fila = NULL;
	if(gg_buscar_fila_por_id(direccion, id, &ruta_data, &indice, &fila) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	int coincide = 1;
	if(info_a_comparar != NULL && info_a_comparar[0] != '\0' &&
		col_comparar != NULL && col_comparar[0] != '\0')
	{
		int columna = -1;
		char **celdas = NULL;
		int n = split_con_numero_de_celdas(fila, GG_caracter_separacion[0], &celdas);
		coincide = texto_a_int_seguro(col_comparar, &columna) == 0 &&
			n > columna && columna >= 0 && celdas != NULL &&
			strcmp(celdas[columna], info_a_comparar) == 0;
		if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
	}
	int resultado = -1;
	if(coincide)
	{
		char *columna_texto = variable_string("%d", col_editar);
		char *valor_texto = variable_string("%s", nuevos_datos != NULL ? nuevos_datos : "");
		if(columna_texto != NULL && valor_texto != NULL)
		{
			char *indices[] = {columna_texto};
			char *valores[] = {valor_texto};
			resultado = gg_editar_campos_en_archivo(ruta_data, indice, indices, valores, 1);
		}
		sistema_memoria_liberar(columna_texto, NULL, 0);
		sistema_memoria_liberar(valor_texto, NULL, 0);
	}
	sistema_memoria_liberar(fila, NULL, 0);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	return gg_info_respuesta(resultado == 0 ? 1 : 0,
		resultado == 0 ? "informacion_editada" : "no_se_encontro_informacion");
}

static int gg_incrementar_campos(const char *direccion, const char *id, const char *indices_texto, const char *cantidades_texto, int copia_si_vacio, const char *fuentes_texto, char **fila_resultado)
{
	if(fila_resultado != NULL) *fila_resultado = NULL;
	char *ruta_data = NULL;
	int indice = -1;
	char *fila = NULL;
	if(gg_buscar_fila_por_id(direccion, id, &ruta_data, &indice, &fila) != 0) return -1;
	char **indices = NULL;
	char **cantidades = NULL;
	char **fuentes = NULL;
	int n_indices = split_con_numero_de_celdas(indices_texto, GG_caracter_separacion_funciones_espesificas[4], &indices);
	int n_cantidades = split_con_numero_de_celdas(cantidades_texto, GG_caracter_separacion_funciones_espesificas[4], &cantidades);
	int n_fuentes = copia_si_vacio
		? split_con_numero_de_celdas(fuentes_texto != NULL ? fuentes_texto : "", GG_caracter_separacion_funciones_espesificas[4], &fuentes)
		: 0;
	char **celdas = NULL;
	int n_celdas = split_con_numero_de_celdas(fila, GG_caracter_separacion[0], &celdas);
	int n = n_indices < n_cantidades ? n_indices : n_cantidades;
	int hubo_deficit = 0;
	int bultos = 0;
	int error = n <= 0 || n_celdas <= 0 || indices == NULL || cantidades == NULL || celdas == NULL;
	for(int i = 0; !error && i < n; ++i)
	{
		int columna = -1;
		char *fin_numero = NULL;
		double incremento = strtod(cantidades[i], &fin_numero);
		if(texto_a_int_seguro(indices[i], &columna) != 0 || columna < 0 || columna >= n_celdas ||
			fin_numero == cantidades[i] || *fin_numero != '\0')
			continue;
		fin_numero = NULL;
		double actual = strtod(celdas[columna] != NULL ? celdas[columna] : "0", &fin_numero);
		if(fin_numero == celdas[columna] || (fin_numero != NULL && *fin_numero != '\0')) actual = 0.0;
		double total = actual + incremento;
		if(copia_si_vacio && (total <= 0.0 || celdas[columna] == NULL || celdas[columna][0] == '\0'))
		{
			hubo_deficit = 1;
			if(total < 0.0) bultos = (int)(-total) + 1;
			if(i < n_fuentes && fuentes != NULL)
			{
				int fuente = -1;
				if(texto_a_int_seguro(fuentes[i], &fuente) == 0 && fuente >= 0 && fuente < n_celdas)
				{
					char *copia = variable_string("%s", celdas[fuente] != NULL ? celdas[fuente] : "");
					if(copia == NULL) { error = 1; break; }
					sistema_memoria_liberar(celdas[columna], NULL, 0);
					celdas[columna] = copia;
				}
			}
		}
		char *nuevo = variable_string("%.17g", total);
		if(nuevo == NULL) { error = 1; break; }
		sistema_memoria_liberar(celdas[columna], NULL, 0);
		celdas[columna] = nuevo;
	}
	char *nueva_fila = NULL;
	if(!error) nueva_fila = gg_reconstruir_fila_info(celdas, n_celdas);
	if(nueva_fila == NULL) error = 1;
	if(!error)
	{
		int n_filas = 0;
		char **lineas = leer_archivo(ruta_data, &n_filas);
		if(lineas == NULL || indice >= n_filas) error = 1;
		else
		{
			sistema_memoria_liberar(lineas[indice], NULL, 0);
			lineas[indice] = variable_string("%s", nueva_fila);
			if(lineas[indice] == NULL || gg_guardar_archivo(ruta_data, lineas, n_filas) != 0) error = 1;
			free_lineas(lineas, n_filas);
		}
	}
	if(fila_resultado != NULL && !error) *fila_resultado = variable_string("%s", nueva_fila);
	sistema_memoria_liberar(nueva_fila, NULL, 0);
	if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
	if(indices != NULL) free_split_con_numero_de_celdas(indices);
	if(cantidades != NULL) free_split_con_numero_de_celdas(cantidades);
	if(fuentes != NULL) free_split_con_numero_de_celdas(fuentes);
	sistema_memoria_liberar(fila, NULL, 0);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	if(error) return -1;
	return hubo_deficit ? bultos + 2 : 1;
}

char *incrementa_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *cants_str)
{
	char *fila = NULL;
	int resultado = gg_incrementar_campos(direccion, id, cols_str, cants_str, 0, NULL, &fila);
	if(resultado < 0) return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	return respuesta;
}

char *incrementa_celda_id_info_dividida_copia_si_cero(const char *direccion, const char *id, const char *cols_str, const char *cants_str, const char *cols_fuente_str)
{
	char *fila = NULL;
	int resultado = gg_incrementar_campos(direccion, id, cols_str, cants_str, 1, cols_fuente_str, &fila);
	if(resultado < 0) return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = resultado > 1
		? variable_string("2%s%s%s%d", GG_caracter_para_confirmacion_o_error[0], fila,
			GG_caracter_para_confirmacion_o_error[0], resultado - 2)
		: gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	return respuesta;
}

char *agregar_sino_existe_info_dividida(const char *direccion, long col_comp, const char *comparar, const char *texto_agr, const char *nom_cols)
{
	if(direccion == NULL || comparar == NULL || col_comp < 0)
		return gg_info_respuesta(0, "parametros_invalidos");
	long id_total = 0;
	long por_archivo = 100;
	if(!existe_archivo(direccion) && gg_crear_metadata_info(direccion, nom_cols) < 0)
		return gg_info_respuesta(0, "error_no_pudo_crear_metadata");
	if(gg_leer_config_metadata(direccion, &id_total, &por_archivo) != 0)
		return gg_info_respuesta(0, "error_no_pudo_leer_metadata");
	for(long id = 1; id <= id_total; ++id)
	{
		char *fila = NULL;
		char *texto_id = variable_string("%ld", id);
		if(texto_id == NULL) return gg_info_respuesta(0, "error_de_memoria");
		int encontrado = gg_buscar_fila_por_id(direccion, texto_id, NULL, NULL, &fila) == 0;
		sistema_memoria_liberar(texto_id, NULL, 0);
		if(!encontrado) continue;
		char **celdas = NULL;
		int n = split_con_numero_de_celdas(fila, GG_caracter_separacion[0], &celdas);
		int coincide = n > col_comp && celdas != NULL && strcmp(celdas[col_comp], comparar) == 0;
		if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
		if(coincide)
		{
			char *respuesta = variable_string("0%sya_existe%s%s",
				GG_caracter_para_confirmacion_o_error[0], GG_caracter_para_confirmacion_o_error[0], fila);
			sistema_memoria_liberar(fila, NULL, 0);
			return respuesta;
		}
		sistema_memoria_liberar(fila, NULL, 0);
	}
	return agregar_info_dividida(direccion, texto_agr, nom_cols);
}

char *buscar_info_dividida(const char *direccion, const char *dato_buscar, int col_buscar, int id_hint)
{
	if(direccion == NULL || dato_buscar == NULL || col_buscar < 0)
		return gg_info_respuesta(0, "parametros_invalidos");
	long id_total = 0;
	long por_archivo = 100;
	if(gg_leer_config_metadata(direccion, &id_total, &por_archivo) != 0)
		return gg_info_respuesta(0, "error_al_leer_metadata");
	if(id_hint >= 1 && id_hint <= id_total)
	{
		char *fila = NULL;
		char *texto_id = variable_string("%d", id_hint);
		if(texto_id != NULL && gg_buscar_fila_por_id(direccion, texto_id, NULL, NULL, &fila) == 0)
		{
			char **celdas = NULL;
			int n = split_con_numero_de_celdas(fila, GG_caracter_separacion[0], &celdas);
			if(n > col_buscar && celdas != NULL && strcmp(celdas[col_buscar], dato_buscar) == 0)
			{
				char *respuesta = variable_string("1%s%s%s%d", GG_caracter_para_confirmacion_o_error[0], fila,
					GG_caracter_para_confirmacion_o_error[0], id_hint);
				if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
				sistema_memoria_liberar(fila, NULL, 0);
				sistema_memoria_liberar(texto_id, NULL, 0);
				return respuesta;
			}
			if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
			sistema_memoria_liberar(fila, NULL, 0);
		}
		sistema_memoria_liberar(texto_id, NULL, 0);
	}
	for(long id = 1; id <= id_total; ++id)
	{
		char *fila = NULL;
		char *texto_id = variable_string("%ld", id);
		if(texto_id == NULL) return gg_info_respuesta(0, "error_de_memoria");
		if(gg_buscar_fila_por_id(direccion, texto_id, NULL, NULL, &fila) != 0)
		{
			sistema_memoria_liberar(texto_id, NULL, 0);
			continue;
		}
		sistema_memoria_liberar(texto_id, NULL, 0);
		char **celdas = NULL;
		int n = split_con_numero_de_celdas(fila, GG_caracter_separacion[0], &celdas);
		if(n > col_buscar && celdas != NULL && strcmp(celdas[col_buscar], dato_buscar) == 0)
		{
			char *respuesta = variable_string("1%s%s%s%ld",
				GG_caracter_para_confirmacion_o_error[0], fila,
				GG_caracter_para_confirmacion_o_error[0], id);
			if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
			sistema_memoria_liberar(fila, NULL, 0);
			return respuesta;
		}
		if(celdas != NULL) free_split_con_numero_de_celdas(celdas);
		sistema_memoria_liberar(fila, NULL, 0);
	}
	return gg_info_respuesta(0, "no_encontro_el_producto");
}

char *incrementa_celda_busqueda_info_dividida(const char *direccion, const char *dato_a_buscar, int col_buscar, const char *cols_inc_str, const char *cants_inc_str, const char *agregar_fallback)
{
	char *busqueda = buscar_info_dividida(direccion, dato_a_buscar, col_buscar, -1);
	if(busqueda != NULL && busqueda[0] == '1')
	{
		char **partes = NULL;
		int n = split_con_numero_de_celdas(busqueda, GG_caracter_para_confirmacion_o_error[0], &partes);
		if(partes != NULL)
		{
			char *resultado = n > 2
				? incrementa_celda_id_info_dividida(direccion, partes[2], cols_inc_str, cants_inc_str)
				: gg_info_respuesta(0, "no_se_encontro_informacion");
			free_split_con_numero_de_celdas(partes);
			sistema_memoria_liberar(busqueda, NULL, 0);
			return resultado;
		}
	}
	sistema_memoria_liberar(busqueda, NULL, 0);
	if(agregar_fallback != NULL && agregar_fallback[0] != '\0')
		return agregar_info_dividida(direccion, agregar_fallback, NULL);
	return gg_info_respuesta(0, "no_se_encontro_informacion");
}

#else

int crearDirectorio(const char *ruta) { (void)ruta; return -1; }
int crearArchivo(const char *ruta, const char *cabecera) { (void)ruta; (void)cabecera; return -1; }
char **leer_archivo(const char *ruta, int *n_lineas_out) { (void)ruta; if(n_lineas_out != NULL) *n_lineas_out = 0; return NULL; }
void guardar_archivo(const char *ruta, char **lineas, int n_lineas) { (void)ruta; (void)lineas; (void)n_lineas; }
void free_lineas(char **lineas, int n_lineas) { (void)lineas; (void)n_lineas; }
void agregar_fila(const char *ruta, const char *fila) { (void)ruta; (void)fila; }
int existe_archivo(const char *ruta) { (void)ruta; return 0; }
void generar_ruta_archivo(const char *ruta, long id, long cantidad, char *salida, size_t capacidad) { (void)ruta; (void)id; (void)cantidad; if(salida != NULL && capacidad > 0) salida[0] = '\0'; }
char *leer_info_dividida(const char *ruta) { (void)ruta; return NULL; }
char *agregar_info_dividida(const char *direccion, const char *agregando, const char *nom_columnas) { (void)direccion; (void)agregando; (void)nom_columnas; return NULL; }
char *leer_todo_info_dividida(const char *ruta) { (void)ruta; return NULL; }
char *agregar_sino_existe_info_dividida(const char *direccion, long col, const char *comparar, const char *fila, const char *columnas) { (void)direccion; (void)col; (void)comparar; (void)fila; (void)columnas; return NULL; }
char *seleccionar_id_info_dividida(const char *direccion, const char *id) { (void)direccion; (void)id; return NULL; }
char *seleccionar_id_info_dividida_extrae(const char *direccion, const char *id) { (void)direccion; (void)id; return NULL; }
char *editar_id_info_dividida(const char *direccion, const char *id, const char *comparar, const char *columna, const char *nuevo, int editar) { (void)direccion; (void)id; (void)comparar; (void)columna; (void)nuevo; (void)editar; return NULL; }
char *editar_celda_id_info_dividida(const char *direccion, const char *id, const char *cols, const char *vals) { (void)direccion; (void)id; (void)cols; (void)vals; return NULL; }
char *incrementa_celda_id_info_dividida(const char *direccion, const char *id, const char *cols, const char *cants) { (void)direccion; (void)id; (void)cols; (void)cants; return NULL; }
char *incrementa_celda_id_info_dividida_copia_si_cero(const char *direccion, const char *id, const char *cols, const char *cants, const char *fuentes) { (void)direccion; (void)id; (void)cols; (void)cants; (void)fuentes; return NULL; }
char *incrementa_celda_busqueda_info_dividida(const char *direccion, const char *dato, int col, const char *cols, const char *cants, const char *fallback) { (void)direccion; (void)dato; (void)col; (void)cols; (void)cants; (void)fallback; return NULL; }
char *buscar_info_dividida(const char *direccion, const char *dato, int col, int hint) { (void)direccion; (void)dato; (void)col; (void)hint; return NULL; }

#endif

#pragma endregion FUNCIONES_INFO_DIVIDIDA
 
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

	if(!actualizar_ultimo_retorno(resultado))
	{
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, nivel_de_profundidad);
		return NULL;
	}
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
 * Pausa la ejecución usando el mecanismo disponible en la plataforma.
 */
void delay_ms(unsigned int ms)
{
#if defined(_WIN32) || defined(_WIN64)
	Sleep((DWORD)ms);
#elif defined(__linux__)
	struct timespec espera;
	espera.tv_sec = (time_t)(ms / 1000U);
	espera.tv_nsec = (long)(ms % 1000U) * 1000000L;
	while(nanosleep(&espera, &espera) != 0 && errno == EINTR)
	{
	}
#elif defined(__XC)
	while(ms > 0U)
	{
		__delay_ms(1);
		--ms;
	}
#else
	(void)ms;
#endif
}

/*
 * Escribe la fecha en un buffer de al menos 20 bytes.
 * El formato se usa en escritorio; el backend embebido usa una fecha simulada.
 */
void fechaActual(char *buffer, const char *formato)
{
	if(buffer == NULL)
	{
		return;
	}

#if defined(_WIN32) || defined(_WIN64) || defined(__linux__)
	if(formato == NULL)
	{
		buffer[0] = '\0';
		return;
	}

	time_t ahora = time(NULL);
	struct tm *hora_local = localtime(&ahora);
	if(hora_local == NULL || strftime(buffer, 20, formato, hora_local) == 0)
	{
		buffer[0] = '\0';
	}
#elif defined(__XC) || defined(PLATAFORMA_SEMICONDUCTOR)
	const char fecha_simulada[] = "01/01/2000 00:00";
	memcpy(buffer, fecha_simulada, sizeof(fecha_simulada));
	(void)formato;
#else
	buffer[0] = '\0';
	(void)formato;
#endif
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

 static int actualizar_ultimo_retorno(const char *texto)
 {
 	if(texto == NULL) return 0;
 	size_t longitud = strlen(texto);
 	if(longitud == (size_t)-1) return 0;

 	char *copia = sistema_memoria_reservar(longitud + 1, NULL, 0);
 	if(copia == NULL) return 0;
 	memcpy(copia, texto, longitud + 1);

 	sistema_memoria_liberar(GG_ultimo_retorno_estandar, NULL, 0);
 	GG_ultimo_retorno_estandar = copia;
 	return 1;
 }

 int acumular_retorno_proceso(const char *texto)
 {
 	if(texto == NULL || texto[0] == '\0') return 0;

 	const char *separador = GG_caracter_para_transferencia_entre_archivos[1];
 	size_t longitudActual = GG_acumulador_retorno_procesos != NULL
 		? strlen(GG_acumulador_retorno_procesos)
 		: 0;
 	size_t longitudTexto = strlen(texto);
 	size_t longitudSeparador = longitudActual > 0 ? strlen(separador) : 0;
 	if(longitudActual > (size_t)-1 - longitudSeparador ||
 		longitudActual + longitudSeparador > (size_t)-1 - longitudTexto ||
 		longitudActual + longitudSeparador + longitudTexto == (size_t)-1)
 	{
 		return -1;
 	}

 	size_t longitudTotal = longitudActual + longitudSeparador + longitudTexto + 1;
 	char *acumulado = sistema_memoria_redimensionar(
 		GG_acumulador_retorno_procesos,
 		longitudTotal,
 		NULL,
 		0
 	);
 	if(acumulado == NULL) return -1;

 	GG_acumulador_retorno_procesos = acumulado;
 	if(longitudSeparador > 0)
 	{
 		memcpy(acumulado + longitudActual, separador, longitudSeparador);
 		longitudActual += longitudSeparador;
 	}
 	memcpy(acumulado + longitudActual, texto, longitudTexto + 1);
 	return 1;
 }

 void limpiar_retorno_procesos(void)
 {
 	sistema_memoria_liberar(GG_acumulador_retorno_procesos, NULL, 0);
 	GG_acumulador_retorno_procesos = NULL;
 }

 const char *obtener_ultimo_retorno_formateado(void)
 {
 	return GG_ultimo_retorno_estandar;
 }



 #pragma endregion FUNCIONES_DE_DEPURACION