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
 #include <errno.h>      /* errno para validar conversiones numericas del modulo de afiliados. */
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
/*
 * Propósito: habilitar la configuración regional del entorno de escritorio.
 * Entrada: no recibe parámetros.
 * Salida y efectos: no devuelve valor; en Windows/Linux llama setlocale(LC_ALL, "").
 * Entrada de ejemplo: configurarConsolaUTF8().
 * Salida de ejemplo: sin valor de retorno; queda activa la configuración regional disponible.
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
/* Ruta de sesión activa para registrar ventas ocurridas durante el conteo. */
 static char *GG_ruta_sesion_inventario_activa = NULL;
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
	/*
	 * RELACION_PROYECTO_TABLA_ID_TIPO usa:
	 * TIPO_RELACION¬PROYECTO/TABLA¬ID, con TIPO_RELACION igual a
	 * PATROCINADOR o ENCARGADO. Cada registro guarda una sola relacion.
	 */
	{"Afiliados", "CONFIG/AFILIADOS/AFILIADOS_UNIFICADO.TXT",
		"_00_ID°_01_NOMBRE°_02_APELLIDO_PATERNO°_03_APELLIDO_MATERNO°_04_FECHA_DE_NACIMIENTO°_05_GÉNERO°_06_DIRECCIÓN°_07_CIUDAD°_08_ESTADO_PROVINCIA°_09_CÓDIGO_POSTAL°_10_PAÍS°_11_CORREO_ELECTRÓNICO°_12_TELÉFONO°_13_FECHA_DE_INGRESO°_14_SUELDO°_15_CARGO°_16_ESTADO_DE_CURS_APRENDIS_E°_17_SUPERVISOR°_18_NOTAS°_19_IDS_DE_AFILIADOS°_20_FECHA_DE_TERMINACIÓN°_21_MOTIVO_DE_TERMINACIÓN°_22_HORAS_TRABAJADAS°_23_EVALUACIONES_DE_DESEMPEÑO°_24_HABILIDADES_Y_CERTIFICACIONES°_25_IDIOMAS°_26_FECHA_DE_ÚLTIMA_PROMOCIÓN°_27_ID_DEPARTAMENTO°_28_HISTORIAL_DE_CAPACITACIÓN°_29_ÚLTIMO_AUMENTO_DE_SALARIO°_30_TIPO_EMPLEADO°_31_RANGO_CALIF°_32_RELACION_PROYECTO_TABLA_ID_TIPO°_33_PUNTOS_RECIBIDOS_TOTALES°_34_REG_PUNTOS_DIA°_35_REG_PUNTOS_MES°_36_REG_PUNTOS_AÑO°_37_REG_PUNTOS_TOTAL°_38_NIVEL°_39_FECHA_ASIGNACION°_40_ESTADO_DEL_USUARIO°_41_PUNTOS_DE_PROYECTO_A_RECIBIR°_42_ID_HORIZONTAL°_43_TIPO_AFILIADO"},
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
 int monitoreo_archivo_entrada(char ***retorno_comando, int *retorno_numero_lineas);
 void respuesta(const char *info, const char *programa_enviar, const char *info_espejo);
 int datos_recibidos_a_procesar_y_borrar(char ***retorno_comando, int *retorno_numero_lineas);
 int finalizar_comando_procesado(const char *linea_original, int estado_ejecucion);
 int cambiar_id_programa_al_siguiente(char **usuarios, int total_usuarios);
 int quitar_id_prog_del_archivo(void);
 int checar_numero_de_direccion_de_archivo_atras_actual_adelante(
   int posicion_bandera, int numero_actual_posicion, int arr_devolver[3]);
 #pragma endregion DECLARACIÓN DE FUNCIONES DE MENSAJERÍA
 // ============================================================================
 // DECLARACIÓN DE FUNCIONES DE PERMISOS
 // ============================================================================
 #pragma region DECLARACIÓN DE FUNCIONES DE PERMISOS
 int checar_permiso(
	 int nivel_minimo,
	 const char *ruta_archivo,
	 const char *id_de_espacio,
	 const char *usuario,
	 const char *contrasena,
	 char **retorna_direccion_espacio_negocio,
	 int *retornar_nivel);

 int checar_permiso_negocios(
	 int nivel_minimo,
	 const char *ruta_archivo,
	 const char *id_de_espacio,
	 const char *usuario,
	 const char *contrasena,
	 int *retornar_nivel);
#pragma endregion DECLARACIÓN DE FUNCIONES DE PERMISOS

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
 void agregar_texto_a_arreglo(char ***arreglo, const char *texto);
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
  int crear_espacios(char *nom_espacio, char *usuario, char *contrasena, char **retorno_nombre_completo_espacio);
  int crear_archivos_base_negocio(const char *id_espacio);
  int creacion_de_estructura_base(char *nombre_de_structura, char *direccion_del_espacio);
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
 static int probar_almacenamiento_dividido(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 int submenu_pruebas_proceso_mul(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_espacios(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_proceso_mul(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_ventas(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_compras(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_inventario(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_proveedores(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static char *pruebas_recopilar_analisis_compras(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_espacios(int opcion,char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_proceso_mul(int opcion,char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_ventas(char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_compras(char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_inventario(int opcion,char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_proveedores(int opcion,char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 static int ejecutar_prueba_analisis_compras(int opcion,char **argumentos,int cantidad,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *registro_unificado_cod3_r_(const char *dir_arch_afiliados,const char *dir_arch_niveles,const char *id_enc,const char *tabla_enc,const char *datos,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *compras(const char *direccion_archivo,char **productos,int cantidad_productos,const char *proveedores,const char *sucursales,double porcentaje_ganancia,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *ventas(const char *direccion_archivo,const char *codigos,const char *cantidades,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_agregar_producto(const char *direccion_archivo,const char *producto,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_agrega_si_no_existe(const char *direccion_archivo,const char *producto,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_buscar(const char *direccion_archivo,const char *codigo_barras,const char *id_producto,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_sacar_info_productos_unitario_del_paquete(const char *direccion_archivo,const char *codigo_barras,const char *id_producto,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_archivos_inicio_hacer_inventario(const char *direccion_registro,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_registrar_movimiento_conteo(const char *direccion_registro,const char *codigo_barras,const char *nombre,const char *cantidad,const char *fecha,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_hacer_inventario(const char *direccion_archivo,const char *direccion_registro,const char *informacion,const char *fecha,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *inventario_dar_el_inventario_string_caracter_sep(const char *direccion_archivo,const char *separador,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *registro_provedores_cod3_r_(const char *direccion_archivo,const char *datos,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *extraer_nombre_provedores(const char *direccion_archivo,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *extraer_nombre_provedores_y_dinero(const char *direccion_archivo,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *existe_informacion(const char *direccion_archivo,const char *informacion,int columna,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *prediccion_arreglo_compra(char **todo_el_ranking,int cantidad_filas,int columna_historial,int columna_ranking,int columna_promedio_compra,int columna_veces_supera_promedio,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *prediccion_archivo_compra(const char *direccion_ranking,int columna_historial,int columna_ranking,int columna_promedio_compra,int columna_veces_supera_promedio,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 char *calculo_pred_compra(char **ranking,int cantidad_filas,int columna_ranking,int columna_promedio_compra,char *error_o_datos_a_retornar,int nivel_de_profundidad);
 int posicion_rango_nesecidad(double promedio_compra,double cantidad_inventario);
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
 int submenu_pruebas_espacios(char *error_o_datos_a_retornar,int nivel_de_profundidad);
 #pragma endregion FUNCIONES_SUBMENUS
 
 // ============================================================================
 // MAIN
 // ============================================================================

 /*
  * Menú interactivo para probar las opciones de los submenús.
  * Devuelve al llamador el resultado codificado de la operación seleccionada.
  */
 char* prueba(int nivel_de_profundidad);
/*
* Propósito: mostrar el menú de pruebas y codificar la acción elegida.
* Entrada: nivel_de_profundidad indica la profundidad para registrar el resultado.
* Salida y efectos: devuelve una cadena asignada con el protocolo de transferencia o un resultado de error; recoge opciones y parámetros, y delega toda ejecución al despachador de main().
* Entrada de ejemplo: prueba(0), con entrada de consola "8".
* Salida de ejemplo: "QU1R30N_SISTEMA_DEFINITIVO┴prueba■salir■0■INFORMACION_ESPEJO_NO_SE_MODIFICA".
*/
char* prueba(int nivel_de_profundidad)
 {
	
	nivel_de_profundidad++;
	int opcion = 0;
	int opcionSubmenu = 0;
	const char *comando = NULL;
	char *accionSubmenu = NULL;
	char *comandoConcatenado = NULL;

	char *error_o_datos_a_retornar ="";
	/*
	 * Menú principal de pruebas:
	 * 1 = comandos y archivo datos.txt; ejemplo: entrada "1", después "2"
	 *     agrega "registro_sintetico" y despacha tex_base.
	 * 2 = mensajería; ejemplo: "2", luego "1" intenta enviar el mensaje
	 *     sintético a todos los contactos configurados.
	 * 3 = operaciones de texto; ejemplo: "3", luego "1" divide
	 *     "uno,dos,tres" por coma.
	 * 4 = auxiliares/infraestructura; ejemplo: "4", luego "7" solicita tres
	 *     textos y muestra el arreglo resultante.
	 * 5 = catálogo; ejemplo: "5", luego "1" imprime las rutas de inventario.
	 * 6 = pregunta la opción y los parámetros de espacios; main ejecuta la acción.
	 * 7 = pregunta la opción y los parámetros de afiliados; main ejecuta la acción.
	 * 8 = pregunta los parámetros de venta; main ejecuta la actualización de existencias.
	 * 9 = pregunta los parámetros de compra; main ejecuta la actualización del inventario.
	 * 10 = pregunta las opciones y parámetros de productos e inventario; main ejecuta la operación.
	 * 11 = preguntar las opciones y campos de proveedores; main ejecuta la operación.
	 * 12 = analizar ranking y predecir compras; main calcula sin alterar el ranking.
	 * 13 = volver; genera el comando "salir" para que main retorne al menú previo.
	 * La entrada recibida aquí siempre es el número del menú principal.
	 */
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "\n=== MENÚ DE PRUEBAS ===\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "1. Probar comandos de texto y archivos\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "2. Probar envío y consulta de mensajes\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "3. Probar operaciones de texto\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "4. Probar funciones auxiliares e infraestructura\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "5. Navegar el catálogo del negocio\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "6. Probar espacios, permisos y estructuras\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "7. Probar proceso de afiliados y comisiones (provisional)\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "8. Registrar una venta y descontar inventario\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "9. Registrar compra y aumentar inventario\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "10. Probar productos y proceso de inventario\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "11. Probar proveedores\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "12. Analizar ranking y predecir compras\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar,0, "13. Volver al menú principal\n");
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
			/*
			 * Submenú tex_base: la entrada "1" lee datos.txt; "2" agrega
			 * "registro_sintetico"; "3" edita la línea 1; "4" cambia la
			 * columna 1 de esa línea; "5" la elimina; "6" la vacía;
			 * "7" ejecuta ejemplos; "8" vuelve. Las opciones 2-6 alteran
			 * el archivo configurado, por lo que se debe usar con cuidado.
			 * Ejemplo: principal=1, submenú=2 -> se despacha
			 * escribirLinea(datos.txt, "registro_sintetico").
			 */
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
				/* Entrada 1: se recibe la opción y se despacha la lectura de NOMBRE_ARCHIVO. */
				case 1:
					accionSubmenu = concatenar("%s%s%s", "leerArchivo", GG_caracter_separacion[2], NOMBRE_ARCHIVO);
					break;
				/* Entrada 2: agrega una fila de ejemplo; posible efecto: datos.txt termina con esa fila. */
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s", "escribirLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "registro_sintetico");
					break;
				/* Entrada 3: reemplaza el contenido de la línea 1 por texto_editado_sintetico. */
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s", "editarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					if(accionSubmenu != NULL)
					{
						char *accionCompleta = concatenar("%s%s%s", accionSubmenu, GG_caracter_separacion[2], "texto_editado_sintetico");
						sistema_memoria_liberar(accionSubmenu, error_o_datos_a_retornar, 0);
						accionSubmenu = accionCompleta;
					}
					break;
				/* Entrada 4: cambia la columna 1 de la línea 1 a valor_sintetico. */
				case 4:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "editarColumna", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1", GG_caracter_separacion[2], "1");
					if(accionSubmenu != NULL)
					{
						char *accionCompleta = concatenar("%s%s%s", accionSubmenu, GG_caracter_separacion[2], "valor_sintetico");
						sistema_memoria_liberar(accionSubmenu, error_o_datos_a_retornar, 0);
						accionSubmenu = accionCompleta;
					}
					break;
				/* Entrada 5: solicita eliminar la línea con ID 1 del archivo. */
				case 5:
					accionSubmenu = concatenar("%s%s%s%s%s", "eliminarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					break;
				/* Entrada 6: solicita vaciar el contenido de la línea con ID 1. */
				case 6:
					accionSubmenu = concatenar("%s%s%s%s%s", "vaciarLinea", GG_caracter_separacion[2], NOMBRE_ARCHIVO, GG_caracter_separacion[2], "1");
					break;
				/* Entrada 7: ejecuta los ejemplos predefinidos y muestra sus resultados. */
				case 7:
					accionSubmenu = concatenar("%s", "ejecutarEjemplosPrueba");
					break;
				/* Entrada 8: no ejecuta una operación; retorna al menú anterior. */
				case 8:
					accionSubmenu = concatenar("%s", "volver");
					break;
			}

			break;
		}
		case 2:
		{
			/*
			 * Submenú de mensajería: "1" intenta enviar un mensaje a todos;
			 * "2" lo dirige al contacto sintético con identificador 1;
			 * "3" consulta al primero que responda; "4" revisa mensajes no
			 * leídos; "5" vuelve. Los ejemplos usan mensajes/contactos
			 * sintéticos y dependen de que el backend y los contactos existan.
			 * Ejemplo: principal=2, submenú=2 -> mensaje_sintetico se dirige
			 * a contacto_sintetico, ID 1.
			 */
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
				/* Entrada 1: despacha el mensaje sintético a todos los contactos. */
				case 1:
					accionSubmenu = concatenar("%s%s%s", "mandar_mensje_a_todos", GG_caracter_separacion[2], "mensaje_sintetico");
					break;
				/* Entrada 2: despacha mensaje, contacto e ID de ejemplo. */
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "mandar_mensje_a_contacto", GG_caracter_separacion[2], "mensaje_sintetico", GG_caracter_separacion[2], "contacto_sintetico", GG_caracter_separacion[2], "1");
					break;
				/* Entrada 3: envía pregunta sintética y espera respuesta/aceptación. */
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "mandar_mensje_al_primero_que_responda", GG_caracter_separacion[2], "pregunta_sintetica", GG_caracter_separacion[2], "aceptacion_sintetica", GG_caracter_separacion[2], "respuesta_sintetica");
					break;
				/* Entrada 4: consulta el estado de mensajes no leídos. */
				case 4:
					accionSubmenu = concatenar("%s", "checar_si_hay_mensajes_no_leido");
					break;
				/* Entrada 5: retorna sin enviar ni consultar mensajes. */
				case 5:
					accionSubmenu = concatenar("%s", "volver");
					break;
			}
			break;
		}
		case 3:
		{
			/*
			 * Submenú de texto: "1" divide "uno,dos,tres" por ","; "2"
			 * sustituye la columna 2 de esa línea; "3" lee texto adicional
			 * desde consola; "4" vuelve.
			 * Ejemplo: principal=3, submenú=1 -> split recibe el texto
			 * "uno,dos,tres" y el delimitador ","; se esperan tres celdas.
			 */
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
				/* Entrada 1: resultado esperado de ejemplo: ["uno", "dos", "tres"]. */
				case 1:
					accionSubmenu = concatenar("%s%s%s%s%s", "split", GG_caracter_separacion[2], "uno,dos,tres", GG_caracter_separacion[2], ",");
					break;
				/* Entrada 2: cambia la segunda columna por valor_sintetico si la fila es válida. */
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "modificarColumna", GG_caracter_separacion[2], "uno,dos,tres", GG_caracter_separacion[2], "2", GG_caracter_separacion[2], "valor_sintetico");
					break;
				/* Entrada 3: el submenu pide una línea y devuelve el texto recibido. */
				case 3:
					accionSubmenu = concatenar("%s", "leerLineaConsola");
					break;
				/* Entrada 4: vuelve al menú principal sin procesar texto. */
				case 4:
					accionSubmenu = concatenar("%s", "volver");
					break;
			}
			break;
		}		
		case 4:
		{
			/*
			 * Submenú de auxiliares: "1" divide y vuelve a unir texto; "2"
			 * convierte enteros/analiza resultados; "3" forma una respuesta;
			 * "4" prueba memoria; "5" prueba archivos temporales; "6" prueba
			 * consola y depuración; "7" agrega tres textos a un arreglo;
			 * "8" prueba almacenamiento dividido con confirmación "SI";
			 * "9" vuelve.
			 * Ejemplo: principal=4, submenú=7, entradas "uno", "dos", "tres"
			 * -> se espera imprimir esos tres elementos en orden.
			 */
			comando = "pruebas_auxiliares";
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "\n=== SUBMENÚ funciones auxiliares ===\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "1. Probar split, liberarSplit y join\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "2. Probar conversiones y lectura de resultados\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "3. Probar crearResultado\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "4. Probar reserva, redimensionamiento y liberación\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "5. Probar primitivas de archivos\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "6. Probar consola, configuración UTF-8 y depuración\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Probar agregar texto a un arreglo dinámico\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "8. Probar almacenamiento dividido\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "9. Volver al menú de pruebas\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");
			char *entradaSubmenu = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
			if(entradaSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudo_leer_opcion_submenu", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			int entradaSubmenuValida = textoAEntero(entradaSubmenu, &opcionSubmenu, error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_memoria_liberar(entradaSubmenu, error_o_datos_a_retornar, 0);
			if(!entradaSubmenuValida || opcionSubmenu < 1 || opcionSubmenu > 9)
			{
				return crearResultado(-2, "opcion_submenu_no_valida", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			switch(opcionSubmenu)
			{
				/* Entrada 1: ejemplo "uno,dos" con "," y "-", resultado "uno-dos". */
				case 1:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s", "probar_split_join", GG_caracter_separacion[2], "uno,dos", GG_caracter_separacion[2], ",", GG_caracter_separacion[2], "-");
					break;
				/* Entrada 2: convierte 42 y analiza la traza sintética "42|prueba". */
				case 2:
					accionSubmenu = concatenar("%s%s%s%s%s", "probar_conversiones", GG_caracter_separacion[2], "42", GG_caracter_separacion[2], "42|prueba");
					break;
				/* Entrada 3: crea una serialización sintética con código 1 e información de prueba. */
				case 3:
					accionSubmenu = concatenar("%s%s%s%s%s%s%s%s%s%s%s", "probar_crearResultado", GG_caracter_separacion[2], "1", GG_caracter_separacion[2], "prueba_sintetica", GG_caracter_separacion[2], "", GG_caracter_separacion[2], "prueba", GG_caracter_separacion[2], "1");
					break;
				/* Entrada 4: pide al helper reservar 16 bytes, ampliarlos y liberarlos. */
				case 4:
					accionSubmenu = concatenar("%s%s%s", "probar_memoria", GG_caracter_separacion[2], "16");
					break;
				/* Entrada 5: pide texto y entero; crea, lee y borra archivos temporales. */
				case 5:
					accionSubmenu = concatenar("%s%s%s%s%s", "probar_archivos", GG_caracter_separacion[2], "texto_sintetico", GG_caracter_separacion[2], "42");
					break;
				/* Entrada 6: pide texto y lo imprime entre corchetes, además de depurarlo. */
				case 6:
					accionSubmenu = concatenar("%s%s%s", "probar_consola", GG_caracter_separacion[2], "texto_sintetico");
					break;
				/* Entrada 7: el helper pide tres textos y confirma el arreglo creado. */
				case 7:
					accionSubmenu = concatenar("%s", "probar_agregar_texto_a_arreglo");
					break;
				/* Entrada 8: el helper pide ruta/columnas/fila y confirma escritura con "SI". */
				case 8:
					accionSubmenu = concatenar("%s", "probar_almacenamiento_dividido");
					break;
				/* Entrada 9: vuelve sin ejecutar pruebas. */
				case 9:
					accionSubmenu = concatenar("%s", "volver");
					break;
			}
			break;
		}
		case 5:
		{
			/*
			 * Catálogo: las entradas 1-5 muestran grupos de rutas de negocio;
			 * la 6 muestra configuración y archivos generales; la 7 vuelve.
			 * Ejemplo: principal=5, sección=1 -> imprime Productos e inventario
			 * junto con las rutas configuradas, sin modificar esos archivos.
			 */
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
			comando = "pruebas_espacios";
			accionSubmenu = pruebas_recopilar_espacios(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_espacios", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 7:
		{
			comando = "pruebas_proceso_mul";
			accionSubmenu = pruebas_recopilar_proceso_mul(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_afiliados", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 8:
		{
			comando = "pruebas_ventas";
			accionSubmenu = pruebas_recopilar_ventas(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_venta", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 9:
		{
			comando = "pruebas_compras";
			accionSubmenu = pruebas_recopilar_compras(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_compra", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 10:
		{
			comando = "pruebas_inventario";
			accionSubmenu = pruebas_recopilar_inventario(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_inventario", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 11:
		{
			comando = "pruebas_proveedores";
			accionSubmenu = pruebas_recopilar_proveedores(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_proveedores", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 12:
		{
			comando = "pruebas_analisis_compras";
			accionSubmenu = pruebas_recopilar_analisis_compras(
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(accionSubmenu == NULL)
			{
				return crearResultado(-1, "no_se_pudieron_recopilar_parametros_de_analisis_de_compras", "", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
			}
			break;
		}
		case 13:
			{
				/* Entrada 13 del menú principal: se recibe la orden de salir y main vuelve al menú anterior. */
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

/**
 * Ejecuta pruebas interactivas de creación de espacios, permisos y estructuras.
 * Entradas: el contexto de error y profundidad se reenvía a la consola; los
 * nombres, rutas y credenciales se solicitan durante la ejecución. Las opciones
 * que crean archivos reales exigen confirmación escribiendo "SI".
 * Salida: 0 si se completa o cancela una prueba; -1 por entrada inválida,
 * lectura fallida o error en la operación probada.
 * Entrada de ejemplo: elegir 1, escribir "ventas", "ana", "clave" y confirmar "SI".
 * Salida de ejemplo: crea el espacio real, muestra su nombre con folio y retorna 0.
 */
int submenu_pruebas_espacios(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	/*
	 * Menú de espacios:
	 * 1 recibe nombre, administrador y contraseña; después pide "SI".
	 *    Ejemplo: ventas / ana / clave / SI -> crea un espacio con folio.
	 * 2 recibe ruta de permisos, ID, usuario, contraseña y nivel mínimo.
	 *    Ejemplo: nivel mínimo 2 -> muestra PERMITIDO o DENEGADO y el nivel.
	 * 3 recibe directorio o ID/nombre (no un archivo .txt) y, con "SI",
	 *    crea los archivos base de negocio.
	 * 4 recibe nombre de estructura y ruta del espacio; con "SI", crea
	 *    la estructura base de inventario.
	 * 5 vuelve sin escribir archivos.
	 */
	char *entradas[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
	char *nombre_creado = NULL;
	char *direccion_negocio = NULL;
	int opcion = 0;
	int cantidad_entradas = 0;
	int resultado = -1;

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PRUEBAS DE ESPACIOS Y PERMISOS ===\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"1. Crear un espacio real y sus archivos base\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"2. Verificar permisos de un usuario\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"3. Crear archivos base de negocio\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"4. Crear estructura base de inventario\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"5. Volver\nSeleccione una opción: ");

	entradas[0] = leerLineaConsola(
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(entradas[0] == NULL ||
		!textoAEntero(entradas[0], &opcion,
			error_o_datos_a_retornar, nivel_de_profundidad))
		goto terminar_prueba_espacios;
	sistema_memoria_liberar(entradas[0], error_o_datos_a_retornar, 0);
	entradas[0] = NULL;

	if(opcion == 5)
	{
		resultado = 0;
		goto terminar_prueba_espacios;
	}
	if(opcion < 1 || opcion > 4)
		goto terminar_prueba_espacios;

	const char *prompts_crear[] = {
		"Nombre del nuevo espacio: ",
		"Usuario administrador: ",
		"Contraseña (se escribe visible): "
	};
	const char *prompts_permiso[] = {
		"Ruta del archivo de permisos: ",
		"ID/nombre del espacio: ",
		"Usuario: ",
		"Contraseña (se escribe visible): ",
		"Nivel mínimo requerido (entero): "
	};
	const char *prompts_archivos[] = {
		"Directorio del espacio o ID/nombre (no el archivo .txt): "
	};
	const char *prompts_estructura[] = {
		"Nombre descriptivo de la estructura: ",
		"Ruta del archivo del espacio (por ejemplo, espacios/ventas.txt): "
	};
	const char **prompts = NULL;
	switch(opcion)
	{
		/* Opción 1: datos esperados nombre/usuario/contraseña; éxito imprime el nombre foliado creado. */
		case 1:
			prompts = prompts_crear;
			cantidad_entradas = 3;
			break;
		case 2:
			prompts = prompts_permiso;
			cantidad_entradas = 5;
			break;
		case 3:
			prompts = prompts_archivos;
			cantidad_entradas = 1;
			break;
		case 4:
			prompts = prompts_estructura;
			cantidad_entradas = 2;
			break;
	}

	for(int i = 0; i < cantidad_entradas; ++i)
	{
		sistema_consola_escribir_formato(
			error_o_datos_a_retornar, 0, "%s", prompts[i]);
		entradas[i] = leerLineaConsola(
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(entradas[i] == NULL)
			goto terminar_prueba_espacios;
	}

	if(opcion == 1 || opcion == 3 || opcion == 4)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Esta operación escribirá en las rutas reales configuradas.\n"
			"Escriba SI para continuar: ");
		entradas[cantidad_entradas] = leerLineaConsola(
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(entradas[cantidad_entradas] == NULL)
			goto terminar_prueba_espacios;
		if(strcmp(entradas[cantidad_entradas], "SI") != 0 &&
			strcmp(entradas[cantidad_entradas], "si") != 0)
		{
			sistema_consola_escribir_formato(
				error_o_datos_a_retornar, 0, "Operación cancelada.\n");
			resultado = 0;
			goto terminar_prueba_espacios;
		}
	}

	switch(opcion)
	{
		case 1:
		{
			int estado = crear_espacios(
				entradas[0], entradas[1], entradas[2], &nombre_creado);
			if(estado == 0)
			{
				sistema_consola_escribir_formato(
					error_o_datos_a_retornar, 0,
					"Espacio creado: %s\n",
					nombre_creado != NULL ? nombre_creado : "(nombre no devuelto)");
				resultado = 0;
			}
			else
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
					"No se pudo crear el espacio. Revisa los datos y la ruta configurada.\n");
			}
			break;
		}
		/* Opción 2: valida las mismas credenciales por ambos métodos y muestra niveles/ruta. */
		case 2:
		{
			int nivel_minimo = 0;
			int nivel_usuario = -1;
			int nivel_negocios = -1;
			int permiso = -1;
			int permiso_negocios = -1;
			if(texto_a_int_seguro(entradas[4], &nivel_minimo) != 0)
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
					"El nivel mínimo debe ser un entero válido.\n");
				break;
			}
			permiso = checar_permiso(
				nivel_minimo,
				entradas[0],
				entradas[1],
				entradas[2],
				entradas[3],
				&direccion_negocio,
				&nivel_usuario);
			permiso_negocios = checar_permiso_negocios(
				nivel_minimo,
				entradas[0],
				entradas[1],
				entradas[2],
				entradas[3],
				&nivel_negocios);
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"checar_permiso: %s; nivel: %d; dirección: %s\n"
				"checar_permiso_negocios: %s; nivel: %d\n",
				permiso == 0 ? "PERMITIDO" :
					(permiso == 1 ? "DENEGADO" : "ERROR"),
				nivel_usuario,
				direccion_negocio != NULL ? direccion_negocio : "(sin dirección)",
				permiso_negocios == 0 ? "PERMITIDO" :
					(permiso_negocios == 1 ? "DENEGADO" : "ERROR"),
				nivel_negocios);
			if(permiso >= 0 && permiso_negocios >= 0)
				resultado = 0;
			break;
		}
		/* Opción 3: crea los archivos base para el espacio seleccionado si los datos son válidos. */
		case 3:
		{
			int estado = crear_archivos_base_negocio(entradas[0]);
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				estado == 0
					? "Archivos base de negocio creados o ya existentes.\n"
					: "No se pudieron crear todos los archivos base.\n");
			if(estado == 0)
				resultado = 0;
			break;
		}
		/* Opción 4: crea estructura de inventario usando el nombre y la ruta proporcionados. */
		case 4:
		{
			int estado = creacion_de_estructura_base(entradas[0], entradas[1]);
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				estado == 0
					? "Estructura base de inventario creada.\n"
					: "No se pudo crear la estructura base de inventario.\n");
			if(estado == 0)
				resultado = 0;
			break;
		}
	}

terminar_prueba_espacios:
	for(size_t i = 0; i < sizeof(entradas) / sizeof(entradas[0]); ++i)
		sistema_memoria_liberar(entradas[i], error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(nombre_creado, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(direccion_negocio, error_o_datos_a_retornar, 0);
	return resultado;
}

/*
 * Propósito: mostrar en consola una sección del catálogo del negocio.
 * Entradas: seccion selecciona el grupo (1–6); error_o_datos_a_retornar es el contexto de error de consola; nivel_de_profundidad identifica la profundidad del mensaje.
 * Salida y efectos: no devuelve valor; imprime los nombres y rutas correspondientes, excepto para seccion 7 o valores no reconocidos, que no imprimen.
 * Entrada de ejemplo: mostrar_seccion_negocio(7, NULL, 0).
 * Salida de ejemplo: sin valor de retorno ni texto en consola.
 */
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
/*
 * Propósito: asegurar que exista el archivo inicial sin truncar su contenido.
 * Entradas: error_o_datos_a_retornar aporta contexto para errores; nivel_de_profundidad identifica la llamada.
 * Salida y efectos: devuelve una cadena de resultado asignada y abre/cierra datos.txt en modo append, creándolo si falta.
 * Entrada de ejemplo: inicio(NULL, 0), con acceso de escritura permitido.
 * Salida de ejemplo: "°1"; datos.txt existe y su contenido previo se conserva.
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
/*
 * Propósito: inicializar la aplicación y despachar el menú interactivo.
 * Entrada: no recibe parámetros; consume opciones desde la consola.
 * Salida y efectos: devuelve 0 al elegir salir, o un código negativo ante error; prepara datos.txt y escribe mensajes en consola.
 * Entrada de ejemplo: main() con stdin="6\n" y acceso de escritura a datos.txt.
 * Salida de ejemplo: devuelve 0 después de mostrar el menú de pruebas y recibir la opción de salida.
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
		else if(nombreFuncion != NULL && strcmp(nombreFuncion, "pruebas_auxiliares") == 0 && opcionSubmenu != 9)
		{
			sistema_memoria_liberar(resultado,error_o_datos_a_retornar,  0);
			resultado = crearResultado(
				opcionSubmenu == 8
					? probar_almacenamiento_dividido(error_o_datos_a_retornar, 1)
					: submenu_pruebas_auxiliares(error_o_datos_a_retornar, 1, opcionSubmenu),
				"pruebas_auxiliares",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_espacios") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_espacios(
					opcionPrueba, argumentos + 1, cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_pruebas_de_espacios" : "pruebas_de_espacios_completadas",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_proceso_mul") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_proceso_mul(
					opcionPrueba, argumentos + 1, cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_pruebas_de_afiliados" : "pruebas_de_afiliados_completadas",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_ventas") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0 &&
				(opcionPrueba == 0 || opcionPrueba == 1)
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_ventas(
					argumentos + 1, cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_prueba_de_ventas" : "prueba_de_ventas_completada",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_compras") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0 &&
				(opcionPrueba == 0 || opcionPrueba == 1)
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_compras(
					argumentos + 1, cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_prueba_de_compras" : "prueba_de_compras_completada",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_inventario") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0 &&
				opcionPrueba >= 0 && opcionPrueba <= 7
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_inventario(
					opcionPrueba, argumentos + 1, cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_pruebas_de_inventario" : "pruebas_de_inventario_completadas",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_proveedores") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0 &&
				opcionPrueba >= 0 && opcionPrueba <= 3
				? (opcionPrueba == 0 ? 0 : ejecutar_prueba_proveedores(
					opcionPrueba, argumentos + 1,
					cantidadArgumentos - 1,
					error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_pruebas_de_proveedores" : "pruebas_de_proveedores_completadas",
				"",
				__func__,
				error_o_datos_a_retornar,
				1
			);
		}
		else if(nombreFuncion != NULL &&
			strcmp(nombreFuncion, "pruebas_analisis_compras") == 0)
		{
			int cantidadArgumentos = 0;
			int opcionPrueba = 0;
			char **argumentos = cantidadPartesComando > 1
				? split(partesComando[1], GG_caracter_separacion[2],
					&cantidadArgumentos, error_o_datos_a_retornar, 1)
				: NULL;
			int estado = argumentos != NULL && cantidadArgumentos > 0 &&
				texto_a_int_seguro(argumentos[0], &opcionPrueba) == 0 &&
				opcionPrueba >= 0 && opcionPrueba <= 2
				? (opcionPrueba == 0 ? 0 :
					ejecutar_prueba_analisis_compras(
						opcionPrueba, argumentos + 1,
						cantidadArgumentos - 1,
						error_o_datos_a_retornar, 1))
				: -1;
			liberarSplit(argumentos, cantidadArgumentos,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			resultado = crearResultado(
				estado < 0 ? -1 : 1,
				estado < 0 ? "error_en_analisis_de_compras" : "analisis_de_compras_completado",
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
/*
 * Propósito: ejecutar o mostrar las opciones CRUD del archivo datos.txt.
 * Entradas: parametros_en_texto_a_splitear es una lista informativa separada por comas; error_o_datos_a_retornar es el contexto de error; nivel_de_profundidad marca la traza; opcion_seleccionada elige una acción (0 solicita menú, 8 vuelve).
 * Salida y efectos: devuelve una cadena de resultado asignada y puede leer/escribir datos.txt o solicitar datos por consola.
 * Entrada de ejemplo: submenu_tex_base("leer,escribir", NULL, 1, 8).
 * Salida de ejemplo: "¬11"; no modifica el archivo en la opción de volver.
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
/*
 * Propósito: mostrar y despachar las opciones de envío y consulta de mensajes.
 * Entradas: parametros_en_texto_a_splitear contiene opciones informativas separadas por comas; error_o_datos_a_retornar aporta contexto de error; nivel_de_profundidad marca la traza; opcion_seleccionada elige la acción (0 solicita menú, 5 vuelve).
 * Salida y efectos: devuelve una cadena de resultado asignada; las acciones de envío pueden agregar mensajes a los archivos configurados y usar la consola.
 * Entrada de ejemplo: submenu_enlasador_mandar_mensajes("todos,contacto", NULL, 1, 5).
 * Salida de ejemplo: "¬11"; no escribe mensajes al elegir volver.
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
/*
 * Propósito: despachar ejemplos interactivos de split, modificación de columna y lectura de texto.
 * Entradas: parametros_en_texto_a_splitear contiene opciones informativas separadas por comas; error_o_datos_a_retornar aporta contexto de error; nivel_de_profundidad marca la traza; opcion_seleccionada elige la opción (0 solicita menú, 4 vuelve).
 * Salida y efectos: devuelve una cadena de resultado asignada; puede leer la consola y mostrar el resultado de la operación elegida.
 * Entrada de ejemplo: submenu_operaciones_de_texto("split,modificar", NULL, 1, 4).
 * Salida de ejemplo: "¬11"; no solicita datos al elegir volver.
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
/*
 * Propósito: dividir texto por todas las apariciones del separador, conservando campos vacíos.
 * Entradas: texto es la cadena a dividir; separador es el delimitador no vacío; cantidad recibe el número de partes; error_o_datos_a_retornar y nivel_de_profundidad dan contexto de memoria.
 * Salida y efectos: devuelve un arreglo terminado en NULL, liberable con liberarSplit(); si cantidad no es NULL, lo pone en 0 antes de validar texto/separador y en el total al completar; no altera texto.
 * Entrada de ejemplo: split("a,,b", ",", &cantidad, NULL, 0), con cantidad inicialmente 99.
 * Salida de ejemplo: retorna {"a", "", "b", NULL} y cantidad pasa de 99 a 3.
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
/*
 * Propósito: crear una cadena nueva aplicando un formato printf.
 * Entradas: format contiene los especificadores; los argumentos variables deben corresponder en tipo y orden a esos especificadores.
 * Salida y efectos: devuelve memoria nueva que el llamador libera con sistema_memoria_liberar(); en SEMICONDUCTOR sin PIC16F devuelve NULL.
 * Entrada de ejemplo: variable_string("%s:%d", "id", 7) en escritorio.
 * Salida de ejemplo: retorna la cadena asignada "id:7".
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
/*
 * Propósito: construir un retorno compacto con código, mensaje y datos.
 * Entradas: codigo es el estado numérico; separador delimita campos (NULL usa el marcador global); mensaje y datos_extra son sus textos, con valores predeterminados si son NULL.
 * Salida y efectos: devuelve una cadena nueva que debe liberarse con sistema_memoria_liberar(); no modifica los argumentos.
 * Entrada de ejemplo: construir_retorno_estandar(1, NULL, "ok", NULL).
 * Salida de ejemplo: retorna "1╣ok╣sin_datos".
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
/*
 * Propósito: adaptar split() a la interfaz que devuelve la cantidad como retorno.
 * Entradas: txt es el texto a dividir; sep es el delimitador; salida recibe el arreglo creado.
 * Salida y efectos: devuelve el número de celdas o -1; inicializa *salida en NULL y, si tiene éxito, transfiere al llamador el arreglo que debe liberar.
 * Entrada de ejemplo: salida=NULL; split_con_numero_de_celdas("rojo,azul", ",", &salida).
 * Salida de ejemplo: retorna 2 y salida pasa de NULL a {"rojo", "azul", NULL}.
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

/*
 * Propósito: liberar un arreglo de cadenas terminado en NULL.
 * Entrada: arreglo es el vector previamente obtenido con el adaptador de split.
 * Salida y efectos: no devuelve valor; libera sus cadenas y el vector, pero no puede cambiar la variable del llamador.
 * Entrada de ejemplo: free_split_con_numero_de_celdas(arreglo), donde arreglo contiene {"x", NULL}.
 * Salida de ejemplo: memoria liberada; la variable arreglo conserva su valor anterior y debe dejarse de usar o ponerse a NULL.
 */
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

/*
 * Propósito: copiar texto al final de un vector dinámico terminado en NULL.
 * Entradas: arreglo apunta al vector (NULL si está vacío); texto es la cadena que se copia.
 * Salida y efectos: no devuelve valor; puede reasignar *arreglo y reserva una nueva copia del texto.
 * Entrada de ejemplo: con lista=NULL, agregar_texto_a_arreglo(&lista, "uno").
 * Salida de ejemplo: lista pasa de NULL a {"uno", NULL}.
 */
void agregar_texto_a_arreglo(char ***arreglo, const char *texto)
{
	if(arreglo == NULL || texto == NULL)
		return;

	size_t cantidad_actual = 0;
	if(*arreglo != NULL)
	{
		while((*arreglo)[cantidad_actual] != NULL)
		{
			if(cantidad_actual >= (size_t)-1 / sizeof(**arreglo) - 2)
			{
				imprimirMensaje_para_depurar(NULL, 0,
					"[agregar_texto_a_arreglo] el arreglo excede el tamaño representable\n");
				return;
			}
			cantidad_actual++;
		}
	}

	size_t largo_texto = strlen(texto);
	if(largo_texto == (size_t)-1)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[agregar_texto_a_arreglo] el texto excede el tamaño representable\n");
		return;
	}

	char *texto_nuevo = sistema_memoria_reservar(largo_texto + 1, NULL, 0);
	if(texto_nuevo == NULL)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[agregar_texto_a_arreglo] no se pudo reservar el texto\n");
		return;
	}
	memcpy(texto_nuevo, texto, largo_texto + 1);

	size_t elementos = cantidad_actual + 2;
	if(elementos > (size_t)-1 / sizeof(**arreglo))
	{
		sistema_memoria_liberar(texto_nuevo, NULL, 0);
		imprimirMensaje_para_depurar(NULL, 0,
			"[agregar_texto_a_arreglo] el arreglo excede el tamaño representable\n");
		return;
	}

	char **nuevo_arreglo = sistema_memoria_redimensionar(
		*arreglo, elementos * sizeof(**arreglo), NULL, 0);
	if(nuevo_arreglo == NULL)
	{
		sistema_memoria_liberar(texto_nuevo, NULL, 0);
		imprimirMensaje_para_depurar(NULL, 0,
			"[agregar_texto_a_arreglo] no se pudo ampliar el arreglo\n");
		return;
	}

	nuevo_arreglo[cantidad_actual] = texto_nuevo;
	nuevo_arreglo[cantidad_actual + 1] = NULL;
	*arreglo = nuevo_arreglo;
}

/* Devuelve 0 si convierte todo el texto y -1 si el valor no es válido. */
/*
 * Propósito: convertir texto a entero mediante el conversor validado del programa.
 * Entradas: texto contiene el entero decimal completo; var_a_retornar recibe el valor convertido.
 * Salida y efectos: devuelve 0 si la conversión es válida o -1 si falla; solo escribe el puntero de salida en una conversión válida.
 * Entrada de ejemplo: texto="42", var_a_retornar apunta inicialmente a 5.
 * Salida de ejemplo: texto_a_int_seguro("42", &valor) devuelve 0 y valor pasa de 5 a 42.
 */
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

/*
 * Propósito: inicializar una cadena dinámica vacía.
 * Entrada: cadena apunta a la estructura que recibirá los datos, longitud y capacidad.
 * Salida y efectos: no devuelve valor; reserva un byte para la cadena vacía o deja datos=NULL y capacidad=0 si falla.
 * Entrada de ejemplo: gg_cadena_iniciar(&c), con c sin inicializar.
 * Salida de ejemplo: en caso de reserva exitosa, c queda con datos="", longitud=0 y capacidad=1.
 */
static void gg_cadena_iniciar(GG_CadenaDinamica *cadena)
{
	cadena->datos = sistema_memoria_reservar(1, NULL, 0);
	cadena->longitud = 0;
	cadena->capacidad = cadena->datos != NULL ? 1 : 0;
	if(cadena->datos != NULL) cadena->datos[0] = '\0';
}

/*
 * Propósito: añadir una cantidad exacta de bytes a una cadena dinámica.
 * Entradas: cadena es la estructura inicializada; texto apunta a los bytes; longitud indica cuántos copiar.
 * Salida y efectos: devuelve 1 al agregar o 0 ante argumento inválido, falta de memoria o desbordamiento; amplía y actualiza la cadena.
 * Entrada de ejemplo: c ya contiene ""; gg_cadena_agregar_n(&c, "sol", 3).
 * Salida de ejemplo: devuelve 1 y c.datos pasa a "sol", con longitud=3.
 */
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

/*
 * Propósito: añadir una cadena C completa a una cadena dinámica.
 * Entradas: cadena es la estructura inicializada; texto es el texto a concatenar (NULL se trata como agregado vacío).
 * Salida y efectos: devuelve el resultado de la operación de agregado; puede ampliar la memoria de cadena.
 * Entrada de ejemplo: c.datos="sol"; gg_cadena_agregar(&c, " y luna").
 * Salida de ejemplo: devuelve 1 y c.datos queda "sol y luna".
 */
static int gg_cadena_agregar(GG_CadenaDinamica *cadena, const char *texto)
{
	return texto != NULL
		? gg_cadena_agregar_n(cadena, texto, strlen(texto))
		: 1;
}

/*
 * Propósito: liberar y reiniciar una cadena dinámica.
 * Entrada: cadena apunta a una estructura que puede contener memoria asignada.
 * Salida y efectos: no devuelve valor; libera datos y deja datos=NULL, longitud=0 y capacidad=0.
 * Entrada de ejemplo: c.datos="sol"; gg_cadena_liberar(&c).
 * Salida de ejemplo: c pasa de {"sol", 3, capacidad} a {NULL, 0, 0}.
 */
static void gg_cadena_liberar(GG_CadenaDinamica *cadena)
{
	if(cadena == NULL) return;
	sistema_memoria_liberar(cadena->datos, NULL, 0);
	cadena->datos = NULL;
	cadena->longitud = 0;
	cadena->capacidad = 0;
}

/*
 * Propósito: transferir al llamador la propiedad del búfer de una cadena dinámica.
 * Entrada: cadena apunta a la estructura cuyo búfer se entrega.
 * Salida y efectos: devuelve el puntero al búfer y reinicia la estructura sin liberar ese búfer.
 * Entrada de ejemplo: c.datos="sol"; resultado=gg_cadena_entregar(&c).
 * Salida de ejemplo: resultado apunta a "sol" y c pasa a {NULL, 0, 0}.
 */
static char *gg_cadena_entregar(GG_CadenaDinamica *cadena)
{
	if(cadena == NULL) return NULL;
	char *resultado = cadena->datos;
	cadena->datos = NULL;
	cadena->longitud = 0;
	cadena->capacidad = 0;
	return resultado;
}

/*
 * Propósito: aplicar formato printf a una lista va_list y reservar el resultado.
 * Entradas: formato define los especificadores; argumentos contiene los valores correspondientes y debe estar inicializado por el llamador.
 * Salida y efectos: devuelve una cadena nueva para liberar con sistema_memoria_liberar(); en SEMICONDUCTOR sin PIC16F devuelve NULL.
 * Entrada de ejemplo: va_list con "%s-%d" y los valores "id", 7, recibido desde una función variádica.
 * Salida de ejemplo: en un backend con vsnprintf retorna "id-7".
 */
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

/*
 * Propósito: convertir una lista textual de índices no negativos en un arreglo de int.
 * Entradas: texto contiene los índices separados; separador es no vacío; indices_salida recibe el arreglo; cantidad_salida recibe su longitud.
 * Salida y efectos: devuelve 1 si todos los campos son enteros no negativos o 0 si falla; inicializa las salidas a NULL y 0 antes de procesar.
 * Entrada de ejemplo: indices=NULL y cantidad=9; gg_indices_desde_texto("1|3", "|", &indices, &cantidad).
 * Salida de ejemplo: devuelve 1, indices pasa de NULL a {1, 3} y cantidad de 9 a 2.
 */
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

/*
 * Propósito: convertir una cadena decimal simple en float, validando todo su contenido.
 * Entradas: texto contiene signo opcional y dígitos con punto decimal opcional; var_a_retornar recibe el valor.
 * Salida y efectos: devuelve 0 si convierte o -1 si el texto está vacío, mal formado o excede FLT_MAX; escribe la salida solo al tener éxito.
 * Entrada de ejemplo: texto="-2.5", salida inicialmente 0.0f.
 * Salida de ejemplo: texto_a_float_seguro("-2.5", &valor) devuelve 0 y valor pasa de 0.0f a -2.5f.
 */
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

/*
 * Propósito: formatear texto y anexarlo a una cadena asignada dinámicamente.
 * Entradas: destino apunta a la cadena existente o NULL; separador se agrega después de cada conversión del formato (NULL o vacío no agrega texto); formato y argumentos definen el fragmento.
 * Salida y efectos: devuelve 0 al reemplazar *destino por la cadena combinada o -1 ante error; libera la cadena anterior al éxito.
 * Entrada de ejemplo: destino=NULL; concatenar_formato_separado_por_variable(&destino, NULL, "id%d", 7).
 * Salida de ejemplo: devuelve 0 y destino pasa de NULL a "id7".
 */
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

/*
 * Propósito: anexar texto formateado y, opcionalmente, un separador a un búfer existente.
 * Entradas: destino es el búfer terminado en NUL; capacidad es su tamaño total; separador se agrega al final si no es NULL; formato y argumentos definen el texto.
 * Salida y efectos: devuelve 0 si anexa todo o -1 si los argumentos son inválidos, falla el formato o no cabe; modifica destino solo al éxito.
 * Entrada de ejemplo: destino="id:", capacidad=16; concatenar_formato(destino, 16, "|", "%d", 7).
 * Salida de ejemplo: devuelve 0 y destino pasa de "id:" a "id:7|".
 */
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

/*
 * Propósito: obtener el separador configurado para columnas o su alternativa.
 * Entrada: no recibe parámetros.
 * Salida y efectos: devuelve una vista al separador global de columnas, o "|" si el valor global es NULL.
 * Entrada de ejemplo: gg_separador_columnas() con GG_caracter_separacion[1]="°".
 * Salida de ejemplo: retorna el puntero a "°"; no modifica estado.
 */
static const char *gg_separador_columnas(void)
{
	return GG_caracter_separacion[1] != NULL ? GG_caracter_separacion[1] : "|";
}

/*
 * Propósito: obtener el separador configurado para filas o su alternativa.
 * Entrada: no recibe parámetros.
 * Salida y efectos: devuelve una vista al separador global de filas, o "|" si el valor global es NULL.
 * Entrada de ejemplo: gg_separador_filas() con GG_caracter_separacion[0]="|".
 * Salida de ejemplo: retorna el puntero a "|"; no modifica estado.
 */
static const char *gg_separador_filas(void)
{
	return GG_caracter_separacion[0] != NULL ? GG_caracter_separacion[0] : "|";
}

/*
 * Propósito: unir una cantidad de cadenas mediante un separador.
 * Entradas: partes es el arreglo de cadenas; cantidad indica cuántas leer; separador se intercala entre ellas.
 * Salida y efectos: devuelve una cadena asignada por join() o NULL si falla; no modifica las partes.
 * Entrada de ejemplo: con partes={"a", "b"}, gg_unir_partes(partes, 2, "|").
 * Salida de ejemplo: retorna "a|b".
 */
static char *gg_unir_partes(char **partes, int cantidad, const char *separador)
{
	return join(partes, cantidad, separador, NULL, 0);
}

/*
 * Propósito: unir filas, opcionalmente extrayendo columnas indicadas por índices.
 * Entradas: caracter_union_filas separa filas; texto y n_texto describen las filas; columnas_extraer lista índices de columnas o es NULL/vacía para filas completas; caracter_union_columnas define el separador de celdas o usa el global.
 * Salida y efectos: devuelve una cadena nueva unida o NULL si los argumentos/formato de índices fallan; no modifica entradas.
 * Entrada de ejemplo: join_paresido_simple(';', (char *[]){"uno", "dos"}, 2, NULL, NULL).
 * Salida de ejemplo: retorna "uno;dos".
 */
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

/*
 * Propósito: quitar un número de filas iniciales o finales y unir las restantes.
 * Entradas: data contiene filas separadas por el separador global; restar_cuantas indica cuántas quitar; restar_primera_celda selecciona quitar desde el inicio (distinto de cero) o el final (cero).
 * Salida y efectos: devuelve una cadena nueva o NULL si no puede dividir/reservar; no modifica data.
 * Entrada de ejemplo: joineada_paraesida_y_quitador_de_extremos("a|b|c", 1, 1).
 * Salida de ejemplo: retorna "b|c".
 */
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

/*
 * Propósito: eliminar campos vacíos al final de una cadena dividida en filas.
 * Entrada: texto contiene campos separados por el separador global de filas.
 * Salida y efectos: devuelve una cadena nueva sin campos finales vacíos, o NULL si falla; no cambia texto.
 * Entrada de ejemplo: Trimend_paresido("a|b||").
 * Salida de ejemplo: retorna "a|b".
 */
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

/*
 * Propósito: leer todas las líneas de un archivo y unirlas en una cadena.
 * Entradas: ruta_archivo identifica el archivo existente; poner_num_fila indica si se antepone a cada línea su índice desde cero.
 * Salida y efectos: devuelve las líneas unidas por el separador global de filas, o NULL ante error; abre y cierra el archivo sin modificarlo.
 * Entrada de ejemplo: archivo con líneas "a" y "b"; concatenacion_filas_de_un_archivo("datos.txt", 0).
 * Salida de ejemplo: retorna "a|b".
 */
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

/*
 * Propósito: unir las cadenas no NULL de un arreglo, opcionalmente prefijándolas con su índice.
 * Entradas: arreglo contiene hasta n_arreglo cadenas; n_arreglo es la cantidad de elementos; poner_num_fila activa el prefijo "índice) ".
 * Salida y efectos: devuelve una cadena nueva o NULL si falla; no modifica el arreglo.
 * Entrada de ejemplo: arreglo={"a", "b"}; concatenacion_filas_de_un_arreglo(arreglo, 2, 1).
 * Salida de ejemplo: retorna "0) a|1) b".
 */
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

/*
 * Propósito: anexar un texto a otro e intercalar un separador cuando el primero no está vacío.
 * Entradas: texto_actual es el prefijo opcional; texto_agregar es obligatorio; separador se intercala si existe prefijo y no es NULL.
 * Salida y efectos: devuelve una cadena nueva o NULL si falla; no modifica los argumentos.
 * Entrada de ejemplo: concatenacion_caracter_separacion("uno", "dos", "|").
 * Salida de ejemplo: retorna "uno|dos".
 */
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

/*
 * Propósito: generar una cadena con la fecha/hora local usando strftime.
 * Entrada: formato_fecha_hora es un formato strftime; NULL usa "%y%m%d%H%M%S".
 * Salida y efectos: devuelve una cadena asignada o NULL si la plataforma no está soportada, falla el reloj/formato o no cabe; consulta la hora local sin cambiarla.
 * Entrada de ejemplo: generar_folio("%Y").
 * Salida de ejemplo: devuelve el año local de cuatro dígitos (por ejemplo, "2026"); el valor exacto depende del reloj del sistema en la llamada.
 */
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

/*
 * Propósito: separar una ruta en directorio, nombre base y extensión.
 * Entradas: direccion es la ruta; cada puntero de salida debe apuntar a un char* que recibirá una cadena asignada.
 * Salida y efectos: devuelve 0 si completa las tres salidas o -1 ante argumentos inválidos/fallo de memoria; inicializa las salidas en NULL y no modifica la ruta.
 * Entrada de ejemplo: las tres salidas son NULL; desfragmentar_direccion("C:\\datos\\inventario.txt", &dir, &nombre, &ext).
 * Salida de ejemplo: devuelve 0; dir pasa a "C:\\datos", nombre a "inventario" y ext a "txt".
 */
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

/*
 * Propósito: sustituir todas las coincidencias no solapadas de una subcadena.
 * Entradas: texto es la cadena original; buscar debe ser no vacío; reemplazo es el texto sustituto, incluso si es vacío.
 * Salida y efectos: devuelve una cadena nueva o NULL ante entradas inválidas/fallo de memoria; no modifica los argumentos.
 * Entrada de ejemplo: gg_reemplazar_texto("casa y casa", "casa", "hogar").
 * Salida de ejemplo: retorna "hogar y hogar".
 */
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

/*
 * Propósito: aplicar en secuencia varias sustituciones de subcadenas.
 * Entradas: info es el texto fuente; caracteres_sep y caracteres_sustitucion contienen pares de búsqueda/reemplazo; n_sep indica el número de pares.
 * Salida y efectos: devuelve una copia transformada o NULL si falla; omite pares nulos o con búsqueda vacía y no modifica los arreglos.
 * Entrada de ejemplo: info="a-b-c", búsqueda={"-"}, reemplazo={"|"}, n_sep=1.
 * Salida de ejemplo: retorna "a|b|c".
 */
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

/*
 * Propósito: sustituir en una cadena todas las apariciones de un separador por otro texto.
 * Entradas: info es el texto fuente; sep es la subcadena buscada; sust es el reemplazo.
 * Salida y efectos: devuelve una cadena nueva; si sep/sust son NULL o sep está vacío, devuelve una copia de info; info NULL produce NULL.
 * Entrada de ejemplo: ReemplazarCaracteres_de_texto_string("ana-rosa", "-", " ").
 * Salida de ejemplo: retorna "ana rosa".
 */
char *ReemplazarCaracteres_de_texto_string(const char *info, const char *sep, const char *sust)
{
	if(info == NULL) return NULL;
	if(sep == NULL || sust == NULL || sep[0] == '\0') return variable_string("%s", info);
	return gg_reemplazar_texto(info, sep, sust);
}

/*
 * Comprueba si algún campo indicado coincide exactamente con valor.
 * Entradas: campos/cantidad_campos describen la fila; indices/cantidad_indices
 * señalan campos (índices desde cero); valor es el texto buscado.
 * Salida y efectos: 1 si hay coincidencia, 0 si no; no modifica los argumentos.
 * Entrada de ejemplo: campos={"Ana","QA"}, indices={1}, valor="QA".
 * Salida de ejemplo: 1.
 */
static int gg_indices_coinciden(char **campos, int cantidad_campos, const int *indices, int cantidad_indices, const char *valor)
{
	if(campos == NULL || indices == NULL || valor == NULL) return 0;
	for(int i = 0; i < cantidad_indices; ++i)
	{
		if(indices[i] < cantidad_campos && strcmp(campos[indices[i]], valor) == 0) return 1;
	}
	return 0;
}

/*
 * Evalúa en una fila todas las condiciones codificadas como índice:valor.
 * Entradas: fila es la fila separada por columnas; comparaciones contiene
 * condiciones separadas por "|" con índices de columna desde cero.
 * Salida y efectos: 1 si todas coinciden, 0 ante fallo o alguna discrepancia.
 * Entrada de ejemplo: fila="Ana,30", comparaciones="0:Ana|1:30".
 * Salida de ejemplo: 1.
 */
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

/*
 * Devuelve todas las filas cuyo campo indicado coincide con comparar.
 * Entradas: texto contiene filas y columnas; columnas_recorrer lista índices
 * de columna desde cero separados por "|"; comparar es el valor exacto.
 * Salida y efectos: cadena dinámica de filas coincidentes o NULL; no altera texto.
 * Entrada de ejemplo: ("Ana,30|Luis,25", "1", "30").
 * Salida de ejemplo: cadena dinámica "Ana,30" (liberable con sistema_memoria_liberar).
 */
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

/*
 * Obtiene la última fila cuyo campo en cualquiera de los índices coincide.
 * Entradas: texto son filas, columnas_recorrer enumera índices desde cero con
 * "|", y comparar es el valor a buscar.
 * Salida y efectos: copia dinámica de la última fila coincidente, "" si no hay
 * coincidencia, o NULL ante error; no modifica texto.
 * Entrada de ejemplo: ("Ana,30|Luis,30", "1", "30").
 * Salida de ejemplo: cadena dinámica "Luis,30".
 */
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

/*
 * Filtra filas aplicando a cada una las condiciones índice:valor.
 * Entradas: texto contiene filas; columnas_recorrer se ignora en esta
 * implementación; comparaciones usa "0:valor|1:valor" con índices desde cero.
 * Salida y efectos: filas coincidentes unidas por el separador de filas, NULL
 * ante error; no modifica texto.
 * Entrada de ejemplo: ("Ana,30|Luis,25", "", "1:30").
 * Salida de ejemplo: cadena dinámica "Ana,30".
 */
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

/*
 * Reemplaza o incrementa los campos seleccionados, opcionalmente filtrando valor.
 * Entradas: campos y cantidad_campos son la fila editable; indices y su cantidad
 * eligen columnas desde cero; valor es el reemplazo; incrementar!=0 suma uno;
 * comparar, si no es NULL, exige coincidencia previa.
 * Salida y efectos: 1 al completar, 0 ante error; libera y sustituye los campos.
 * Entrada de ejemplo: campos={"Ana","25"}, indices={1}, valor="30", incrementar=0.
 * Salida de ejemplo: devuelve 1 y campos queda {"Ana","30"}.
 */
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

/*
 * Une campos en una fila usando el separador de columnas configurado.
 * Entradas: campos es el vector de textos y cantidad_campos su longitud.
 * Salida y efectos: cadena dinámica con la fila o NULL si falla; no altera campos.
 * Entrada de ejemplo: ({"Ana","30","QA"}, 3).
 * Salida de ejemplo: cadena dinámica "Ana,30,QA".
 */
static char *gg_reconstruir_fila(char **campos, int cantidad_campos)
{
	return join(campos, cantidad_campos, gg_separador_columnas(), NULL, 0);
}

/*
 * Edita las columnas indicadas en una o varias filas y reconstruye el texto.
 * Entradas: texto es la tabla; indices/cantidad_indices eligen columnas; valor
 * es el reemplazo; incrementar!=0 suma uno; fila_objetivo es índice de fila
 * desde cero o -1 para todas; comparar_campos filtra campos si no es NULL.
 * Salida y efectos: tabla dinámica editada o NULL; no modifica texto original.
 * Entrada de ejemplo: ("Ana,25|Luis,30", {1}, 1, "31", 0, 1, NULL).
 * Salida de ejemplo: cadena dinámica "Ana,25|Luis,31".
 */
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

/*
 * Edita las columnas seleccionadas en todas las filas, o las incrementa en uno.
 * Entradas: texto es la tabla; columnas_recorrer lista índices desde cero con
 * "|"; info_sustituir es el nuevo texto; edit_0_increm_1 igual a "1" activa suma.
 * Salida y efectos: tabla nueva o NULL ante error; conserva texto de entrada.
 * Entrada de ejemplo: ("Ana,25", "1", "30", "0").
 * Salida de ejemplo: cadena dinámica "Ana,30".
 */
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

/*
 * Adaptador público que delega la edición múltiple a la rutina recursiva.
 * Entradas: texto es la tabla; indices_editar lista columnas; info_editar es el
 * reemplazo; edit_0_increm_1 igual a "1" activa incremento en vez de reemplazo.
 * Salida y efectos: tabla dinámica resultante o NULL; no modifica texto.
 * Entrada de ejemplo: ("Ana,25", "1", "26", "0").
 * Salida de ejemplo: cadena dinámica "Ana,26".
 */
char *editar_inc_edicion_profunda_multiple_string(
	const char *texto,
	const char *indices_editar,
	const char *info_editar,
	const char *edit_0_increm_1
)
{
	return editar_incr_string_funcion_recursiva(texto, indices_editar, info_editar, edit_0_increm_1);
}

/*
 * Edita únicamente la última fila, reemplazando o incrementando sus columnas.
 * Entradas: texto es la tabla; indices_editar enumera columnas; info_editar es
 * el valor; edit_0_increm_1 igual a "1" activa incremento.
 * Salida y efectos: copia dinámica de la tabla editada o NULL; texto se conserva.
 * Entrada de ejemplo: ("Ana,25|Luis,30", "1", "31", "0").
 * Salida de ejemplo: cadena dinámica "Ana,25|Luis,31".
 */
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

/*
 * Edita la última fila con coincidencia en las columnas elegidas.
 * Entradas: texto es la tabla; indices_editar selecciona columnas; info_editar
 * es el valor nuevo; comparacion es el valor de búsqueda; edit_0_increm_1 activa
 * incremento cuando vale "1".
 * Salida y efectos: tabla dinámica actualizada o NULL; no altera texto original.
 * Entrada de ejemplo: ("Ana,25|Luis,25", "1", "30", "25", "0").
 * Salida de ejemplo: cadena dinámica "Ana,25|Luis,30".
 */
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

/*
 * Decodifica una solicitud compacta y aplica una edición a la tabla incluida.
 * Entrada: datos contiene texto|índices|reemplazo|indicador_incremento.
 * Salida y efectos: tabla dinámica editada; con menos de tres campos devuelve
 * "" y ante error NULL; no modifica datos.
 * Entrada de ejemplo: "Ana,25|1|30|0".
 * Salida de ejemplo: cadena dinámica "Ana,30".
 */
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

/*
 * Edita las columnas seleccionadas en filas cuyo campo coincida con un valor.
 * Entradas: texto es la tabla; indices_editar lista índices; comparacion_con_edicion
 * es "valor_buscar,valor_nuevo" (o solo el valor buscado, que edita a vacío);
 * edit_0_increm_1 igual a "1" activa incremento.
 * Salida y efectos: nueva tabla o NULL; el original no se modifica.
 * Entrada de ejemplo: ("Ana,25|Luis,30", "1", "25,26", "0").
 * Salida de ejemplo: cadena dinámica "Ana,26|Luis,30".
 */
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

/*
 * Recorta del inicio ("izq") o del final un número de campos separados por filas.
 * Entradas: contenidoFila es el texto separado por el delimitador de filas;
 * izquierda_o_derecha selecciona el lado; numero_veses es cuántos campos quitar.
 * Salida y efectos: texto dinámico restante o NULL si falla; no modifica entrada.
 * Entrada de ejemplo: ("a|b|c", "izq", 1).
 * Salida de ejemplo: cadena dinámica "b|c".
 */
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

/*
 * Recorta campos usando el delimitador específico configurado, con fallback de filas.
 * Entradas: contenidoFila es el texto; izquierda_o_derecha es "izq" para quitar
 * desde el inicio o cualquier otro valor para quitar desde el final; numero_veses
 * indica cuántos elementos recortar.
 * Salida y efectos: cadena dinámica restante o NULL; no modifica contenidoFila.
 * Entrada de ejemplo: ("a~b~c", "izq", 1), con separador específico "~".
 * Salida de ejemplo: cadena dinámica "b~c".
 */
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
/*
 * Libera cada cadena de un split y después el vector recibido.
 * Entradas: partes es el vector reservado por split; cantidad es su longitud;
 * error_o_datos_a_retornar y nivel_de_profundidad se reenvían al liberador.
 * Salida y efectos: no retorna valor; libera todo el almacenamiento de partes.
 * Entrada de ejemplo: partes={"a","b"}, cantidad=2.
 * Salida de ejemplo: ambos textos y el vector quedan liberados; partes no se reutiliza.
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
/*
 * Concatena elementos de un vector, intercalando el separador indicado.
 * Entradas: arreglo contiene cantidad cadenas; carcter_separacion es el delimitador
 * (NULL equivale a vacío); error_o_datos_a_retornar y nivel_de_profundidad se usan
 * en la reserva. Elementos NULL se tratan como "".
 * Salida y efectos: cadena dinámica nueva o NULL si argumentos/reserva fallan.
 * Entrada de ejemplo: ({"rojo","azul"}, 2, "|", NULL, 0).
 * Salida de ejemplo: cadena dinámica "rojo|azul".
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
/*
 * Construye una cadena dinámica con formato limitado a %s, %d y %%.
 * Entradas: formato es la plantilla y los argumentos variádicos aportan valores
 * en el mismo orden; las cadenas NULL se convierten en vacías.
 * Salida y efectos: resultado reservado para el llamador o NULL ante formato
 * inválido/error de memoria; no imprime nada.
 * Entrada de ejemplo: concatenar("id=%d %s", 7, "Ana").
 * Salida de ejemplo: cadena dinámica "id=7 Ana".
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
/*
 * Convierte un entero a decimal dentro del buffer proporcionado.
 * Entradas: valor es el entero; buffer apunta al destino; capacidad incluye
 * espacio para el terminador NUL.
 * Salida y efectos: devuelve buffer y lo llena, o NULL si no cabe/está invalidado.
 * Entrada de ejemplo: valor=42, buffer inicialmente "", capacidad=8.
 * Salida de ejemplo: devuelve el mismo buffer, ahora contiene "42".
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
/*
 * Convierte un tramo decimal completo, permitiendo espacios alrededor.
 * Entradas: texto y longitud definen el tramo; resultado apunta al entero destino.
 * Salida y efectos: 1 en conversión válida y escribe resultado; 0 ante sintaxis,
 * rango o punteros inválidos; en fallo no se garantiza cambiar el destino.
 * Entrada de ejemplo: texto=" -12 ", longitud=5, resultado inicialmente 99.
 * Salida de ejemplo: devuelve 1 y *resultado pasa de 99 a -12.
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
/*
 * Convierte una cadena C decimal delegando la validación al lector por longitud.
 * Entradas: texto es la cadena terminada en NUL; resultado recibe el entero;
 * error_o_datos_a_retornar y nivel_de_profundidad se ignoran por compatibilidad.
 * Salida y efectos: 1 si convierte toda la cadena, 0 ante NULL o dato inválido.
 * Entrada de ejemplo: texto="42", resultado inicialmente 0.
 * Salida de ejemplo: devuelve 1 y *resultado pasa de 0 a 42.
 */
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
/*
 * Escribe en la consola texto con conversiones limitadas a %s, %d y %%.
 * Entradas: error_o_datos_a_retornar y nivel_de_profundidad se reenvían al
 * backend; formato y argumentos variádicos determinan el texto.
 * Salida y efectos: escribe en consola; devuelve 0 si todo se escribe o -1
 * ante formato no admitido, argumentos/canal inválidos o fallo de E/S.
 * Entrada de ejemplo: sistema_consola_escribir_formato(NULL,0,"n=%d",7).
 * Salida de ejemplo: escribe "n=7" y devuelve 0 si la consola acepta los bytes.
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
/*
 * Escribe en un archivo el valor entero convertido a texto decimal.
 * Entradas: archivo es el flujo abierto; valor es el entero;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: añade los dígitos al flujo; devuelve 0 si escribe o -1
 * si falla la conversión o la escritura.
 * Entrada de ejemplo: archivo abierto para escritura, valor=42.
 * Salida de ejemplo: el flujo recibe "42" y la función devuelve 0 si no hay error.
 */
 static int sistema_archivo_escribir_entero(SistemaArchivo *archivo, int valor, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
 	char buffer[sizeof(int) * CHAR_BIT + 2];
 	if(enteroATexto(valor, buffer, sizeof(buffer)) == NULL) return -1;
 	return sistema_archivo_escribir_texto(archivo, buffer,error_o_datos_a_retornar,  nivel_de_profundidad);
 }

/* Escribe texto seguido de '\n'; -1 señala que falló cualquiera de los dos pasos. */
/*
 * Escribe un texto y después un salto de línea en el archivo abierto.
 * Entradas: archivo es el flujo destino; texto no debe ser NULL;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: modifica el archivo; devuelve 0 si ambas escrituras tienen
 * éxito y -1 ante error.
 * Entrada de ejemplo: flujo vacío abierto para escritura, texto="Ana".
 * Salida de ejemplo: el flujo contiene "Ana\n" y la función devuelve 0.
 */
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
/*
 * Lee caracteres mediante una función callback hasta LF o fin de flujo.
 * Entradas: leerCaracter obtiene cada byte usando contexto; contexto identifica
 * la fuente; error_o_datos_a_retornar y nivel_de_profundidad se reenvían.
 * Salida y efectos: cadena dinámica sin LF, o NULL si no hay bytes/error; no
 * modifica el contexto por sí misma.
 * Entrada de ejemplo: callback que entrega 'h','i','\n', luego EOF; contexto=NULL.
 * Salida de ejemplo: devuelve una cadena reservada "hi".
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

/*
 * Adaptador de callback que lee un carácter de un SistemaArchivo.
 * Entradas: contexto debe apuntar al flujo abierto;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: devuelve el byte leído o SISTEMA_ARCHIVO_FIN_LECTURA; avanza
 * la posición del archivo.
 * Entrada de ejemplo: contexto señala un archivo cuyo próximo byte es 'A'.
 * Salida de ejemplo: devuelve 'A' y el cursor del archivo avanza un byte.
 */
static int leerCaracterArchivo(void *contexto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	/* El contexto es SistemaArchivo*; el backend traduce su EOF al valor común. */
	return sistema_archivo_leer_caracter((SistemaArchivo *)contexto,error_o_datos_a_retornar,  nivel_de_profundidad);
}

/*
 * Adaptador de callback que lee un carácter desde la consola.
 * Entradas: contexto se ignora en la implementación actual;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: devuelve el carácter leído o el marcador de fin; consume
 * entrada estándar/consola.
 * Entrada de ejemplo: con una consola que entrega primero 'S', contexto=NULL.
 * Salida de ejemplo: devuelve 'S' y ese carácter queda consumido.
 */
static int leerCaracterConsola(void *contexto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	/* La consola no necesita objeto de contexto en la API actual. */
	(void)contexto;
	return sistema_consola_leer_caracter(error_o_datos_a_retornar, nivel_de_profundidad);
}

/* Adaptador público interno: lee una línea desde un archivo ya abierto. */
/*
 * Lee una línea desde un archivo previamente abierto.
 * Entradas: flujo es el SistemaArchivo;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: devuelve cadena dinámica sin LF, o NULL al EOF sin datos
 * o ante error; avanza el flujo.
 * Entrada de ejemplo: flujo contiene como siguiente línea "hola\n".
 * Salida de ejemplo: devuelve la cadena reservada "hola".
 */
static char *leerLineaDinamica(SistemaArchivo *flujo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	nivel_de_profundidad++;
	(void)nivel_de_profundidad;
	if(flujo == NULL) return NULL;
	return leerLineaDesde(leerCaracterArchivo, flujo, error_o_datos_a_retornar, nivel_de_profundidad);
}

/* Adaptador interno: misma lógica de línea, pero leyendo desde la consola. */
/*
 * Lee desde la consola una línea completa sin incluir el salto de línea.
 * Entradas: error_o_datos_a_retornar y nivel_de_profundidad se reenvían a las
 * rutinas de lectura y memoria.
 * Salida y efectos: devuelve cadena dinámica o NULL si no hay caracteres/error;
 * consume los caracteres de consola leídos.
 * Entrada de ejemplo: la consola recibe "hola" seguido de Enter.
 * Salida de ejemplo: devuelve la cadena reservada "hola".
 */
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
/*
 * Sustituye una columna numerada desde uno en una fila separada por comas.
 * Entradas: lineaOriginal es la fila; columnaTarget es el índice desde uno;
 * nuevoValor es el reemplazo; error_o_datos_a_retornar es el puntero de
 * diagnóstico reenviado y nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: fila nueva dinámica o NULL ante entrada/columna inválida;
 * no altera la cadena original.
 * Entrada de ejemplo: modificarColumna("Ana,25,QA",2,"30",NULL,0).
 * Salida de ejemplo: cadena dinámica "Ana,30,QA".
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
/*
 * Reemplaza ruta por el archivo temporal, usando un respaldo intermedio.
 * Entradas: ruta es el archivo original; error_o_datos_a_retornar es el puntero
 * de diagnóstico reenviado y nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: modifica/renombra archivos; devuelve 0 si todo concluye,
 * -1 si algún paso falla, intentando restaurar el original.
 * Entrada de ejemplo: ruta="datos.txt", original y temporal existentes, respaldo ausente.
 * Salida de ejemplo: sustituye datos.txt por el temporal, elimina respaldo y devuelve 0.
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
/*
 * Reescribe una línea mediante reemplazo (0), borrado (1), vaciado (2) o
 * cambio de columna (3), con staging a archivo temporal.
 * Entradas: ruta es el archivo; numeroLinea empieza en uno; operacion selecciona
 * acción; contenido aporta reemplazo; columna (para acción 3) empieza en uno;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada; ambos se envían a E/S.
 * Salida y efectos: modifica el archivo; devuelve 0, -1 por error o -2 si falta línea.
 * Entrada de ejemplo: archivo "a\nb\n", línea=2, operación=0, contenido="B".
 * Salida de ejemplo: archivo queda "a\nB\n" y devuelve 0 si E/S concluye.
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
/*
 * Imprime en consola las líneas de un archivo numeradas desde uno.
 * Entradas: ruta identifica el archivo; error_o_datos_a_retornar es el puntero
 * de diagnóstico reenviado y nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: no modifica ruta; imprime el contenido y devuelve la
 * serialización de crearResultado (separador de nivel más código).
 * Entrada de ejemplo: leerArchivo("datos.txt",NULL,0), con archivo "Ana\n".
 * Salida de ejemplo: imprime "1: Ana" y devuelve "°1" al usar nivel 0.
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
/*
 * Añade una línea al final de un archivo, incluyendo LF.
 * Entradas: ruta es el archivo destino; nuevaLinea es el contenido; los
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada; ambos se reenvían a E/S.
 * Salida y efectos: modifica el archivo y devuelve la serialización de
 * crearResultado (separador de nivel más código).
 * Entrada de ejemplo: escribirLinea("datos.txt","Ana,25",NULL,0), archivo existente.
 * Salida de ejemplo: archivo termina con "Ana,25\n" y devuelve "°1" al usar nivel 0.
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

/*
 * Reemplaza completamente la línea indicada, usando numeración desde uno.
 * Entradas: ruta es el archivo; idLinea es el número de línea; nuevoTexto es
 * el reemplazo; error_o_datos_a_retornar es el puntero de diagnóstico reenviado
 * y nivel_de_profundidad identifica el nivel de llamada; ambos se reenvían a E/S.
 * Salida y efectos: reescribe el archivo y devuelve la serialización de
 * crearResultado (separador de nivel más código).
 * Entrada de ejemplo: archivo "a\nb\n", editarLinea("datos.txt",2,"B",NULL,0).
 * Salida de ejemplo: archivo queda "a\nB\n" y devuelve "°1" al usar nivel 0.
 */
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
 * Entradas: ruta es el archivo; idLinea e idColumna empiezan en uno; nuevoValor
 * reemplaza el campo; error_o_datos_a_retornar es el puntero de diagnóstico
 * reenviado y nivel_de_profundidad identifica el nivel de llamada; ambos se reenvían a E/S.
 * Salida y efectos: reescribe el archivo y devuelve la serialización de
 * crearResultado (separador de nivel más código).
 * Entrada de ejemplo: archivo "Ana,25,QA\n", editarColumna("datos.txt",1,2,"30",NULL,0).
 * Salida de ejemplo: archivo queda "Ana,30,QA\n" y devuelve "°1" al usar nivel 0.
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

/*
 * Elimina físicamente la línea indicada; las siguientes pasan a ocupar índices previos.
 * Entradas: ruta es el archivo; idLinea empieza en uno;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: reescribe el archivo sin esa línea y devuelve la serialización
 * de crearResultado (separador de nivel más código).
 * Entrada de ejemplo: archivo "a\nb\n", eliminarLinea("datos.txt",1,NULL,0).
 * Salida de ejemplo: archivo queda "b\n" y devuelve "°1" al usar nivel 0.
 */
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

/*
 * Conserva el número de línea sustituyendo su contenido por una línea vacía.
 * Entradas: ruta es el archivo; idLinea empieza en uno;
 * error_o_datos_a_retornar es el puntero de diagnóstico reenviado y
 * nivel_de_profundidad identifica el nivel de llamada.
 * Salida y efectos: modifica el archivo y devuelve la serialización de
 * crearResultado (separador de nivel más código).
 * Entrada de ejemplo: archivo "a\nb\n", vaciarLinea("datos.txt",1,NULL,0).
 * Salida de ejemplo: archivo queda "\nb\n" y devuelve "°1" al usar nivel 0.
 */
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

/*
 * Busca el último separador de directorio ('/' o '\\') dentro de la ruta.
 * Entradas: ruta es una cadena terminada en NUL que no se modifica.
 * Salida y efectos: puntero a la última barra o NULL si no existe.
 * Entrada de ejemplo: ruta="a/b\\c.txt".
 * Salida de ejemplo: devuelve un puntero a la barra invertida antes de "c.txt".
 */
static char *gg_buscar_ultimo_separador_ruta(char *ruta)
{
	char *barra = strrchr(ruta, '/');
	char *barra_inversa = strrchr(ruta, '\\');
	if(barra == NULL) return barra_inversa;
	if(barra_inversa == NULL) return barra;
	return barra > barra_inversa ? barra : barra_inversa;
}

/*
 * Comprueba si una ruta existente designa un directorio en la plataforma activa.
 * Entrada: ruta es el camino a consultar.
 * Salida y efectos: 1 si es directorio, 0 si no existe/no es directorio; no modifica ruta.
 * Entrada de ejemplo: ruta="carpeta_existente", que apunta a un directorio.
 * Salida de ejemplo: devuelve 1.
 */
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

/*
 * Crea un único directorio, considerando éxito si ya existía como directorio.
 * Entrada: ruta es el nombre/camino del directorio.
 * Salida y efectos: crea el directorio si es posible; devuelve 0 en éxito o -1.
 * Entrada de ejemplo: ruta="datos/nuevo", con "datos" existente y "nuevo" ausente.
 * Salida de ejemplo: crea "datos/nuevo" y devuelve 0.
 */
static int gg_crear_directorio_uno(const char *ruta)
{
#if defined(PLATAFORMA_WINDOWS)
	if(_mkdir(ruta) == 0 || gg_es_directorio(ruta)) return 0;
#else
	if(mkdir(ruta, 0777) == 0 || gg_es_directorio(ruta)) return 0;
#endif
	return -1;
}

/*
 * Crea recursivamente los componentes de una ruta de directorio.
 * Entrada: ruta es el camino que se creará; se normalizan separadores por plataforma.
 * Salida y efectos: crea directorios faltantes; devuelve 0 en éxito o -1 ante error.
 * Entrada de ejemplo: crearDirectorio("datos/2026/ventas").
 * Salida de ejemplo: crea esos componentes si faltan y devuelve 0.
 */
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

/*
 * Crea el directorio padre de la ruta de archivo proporcionada.
 * Entrada: ruta es el camino del archivo cuya carpeta padre se requiere.
 * Salida y efectos: crea el padre si hay separador; devuelve 0 en éxito o -1.
 * Entrada de ejemplo: gg_crear_directorio_padre("datos/ventas.txt").
 * Salida de ejemplo: crea "datos" si falta y devuelve 0.
 */
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

/*
 * Comprueba si puede abrirse y cerrarse una ruta como archivo.
 * Entrada: ruta es el camino consultado.
 * Salida y efectos: devuelve 1 si abre y cierra sin error, 0 en caso contrario;
 * no modifica el contenido.
 * Entrada de ejemplo: existe_archivo("datos.txt"), archivo legible existente.
 * Salida de ejemplo: devuelve 1.
 */
int existe_archivo(const char *ruta)
{
	if(ruta == NULL) return 0;
	SistemaArchivo *archivo = sistema_archivo_abrir(ruta, "r", NULL, 0);
	if(archivo == NULL) return 0;
	return sistema_archivo_cerrar(archivo, NULL, 0) == 0;
}

/*
 * Crea un archivo nuevo, creando primero sus directorios padre si hacen falta.
 * Entradas: ruta es el destino; cabecera, si no es NULL, se escribe como primera
 * línea; no reemplaza un archivo ya existente.
 * Salida y efectos: crea archivo/cabecera; devuelve 0 creado, 1 ya existía, -1 error.
 * Entrada de ejemplo: crearArchivo("datos/nuevo.txt","id,nombre"), destino ausente.
 * Salida de ejemplo: crea el archivo con "id,nombre\n" y devuelve 0.
 */
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

/*
 * Libera las n_lineas cadenas y luego el vector retornado por leer_archivo.
 * Entradas: lineas es el vector (puede ser NULL); n_lineas es la cantidad válida.
 * Salida y efectos: no retorna valor; libera todo el almacenamiento recibido.
 * Entrada de ejemplo: lineas={"a","b"}, n_lineas=2.
 * Salida de ejemplo: los dos textos y el vector quedan liberados.
 */
void free_lineas(char **lineas, int n_lineas)
{
	if(lineas == NULL) return;
	for(int i = 0; i < n_lineas; ++i)
		sistema_memoria_liberar(lineas[i], NULL, 0);
	sistema_memoria_liberar(lineas, NULL, 0);
}

/*
 * Lee un archivo completo en un vector de cadenas, una por línea.
 * Entradas: ruta es el archivo; n_lineas_out apunta al contador de salida.
 * Salida y efectos: devuelve vector dinámico o NULL; inicializa el contador a
 * cero y, en éxito, lo establece al número de líneas; el llamador usa free_lineas.
 * Entrada de ejemplo: ruta="datos.txt" con "a\nb\n", *n_lineas_out inicialmente 99.
 * Salida de ejemplo: devuelve {"a","b"} y *n_lineas_out pasa de 99 a 2.
 */
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

/*
 * Sobrescribe un archivo con las cadenas dadas, cada una como una línea.
 * Entradas: ruta es destino; lineas contiene n_lineas textos; n_lineas debe ser
 * no negativo; no se permite vector NULL si hay líneas.
 * Salida y efectos: trunca/escribe el archivo; devuelve 0 en éxito o -1 en error.
 * Entrada de ejemplo: gg_guardar_archivo("datos.txt",{"a","b"},2).
 * Salida de ejemplo: archivo queda "a\nb\n" y devuelve 0 si E/S concluye.
 */
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

/*
 * Interfaz void que guarda las líneas delegando en gg_guardar_archivo.
 * Entradas: ruta es destino; lineas/n_lineas describen los textos a escribir.
 * Salida y efectos: sobrescribe el archivo cuando la operación tiene éxito;
 * descarta el código de error del auxiliar y no devuelve valor.
 * Entrada de ejemplo: guardar_archivo("datos.txt",{"a","b"},2).
 * Salida de ejemplo: datos.txt contiene "a\nb\n" si la E/S concluye.
 */
void guardar_archivo(const char *ruta, char **lineas, int n_lineas)
{
	(void)gg_guardar_archivo(ruta, lineas, n_lineas);
}

/*
 * Añade una fila al final del archivo, seguida de LF.
 * Entradas: ruta es el archivo; fila es el contenido que se agrega.
 * Salida y efectos: modifica el archivo; devuelve 0 si escribe/cierra bien o -1.
 * Entrada de ejemplo: archivo termina en "a\n", gg_agregar_fila("datos.txt","b").
 * Salida de ejemplo: termina en "a\nb\n" y devuelve 0 si E/S concluye.
 */
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

/*
 * Añade una fila al archivo mediante el auxiliar y descarta su estado de retorno.
 * Entradas: ruta es el archivo; fila es el texto a añadir.
 * Salida y efectos: agrega fila más LF si se puede; no devuelve estado.
 * Entrada de ejemplo: agregar_fila("datos.txt","b"), archivo termina en "a\n".
 * Salida de ejemplo: archivo termina en "a\nb\n" si la operación tiene éxito.
 */
void agregar_fila(const char *ruta, const char *fila)
{
	(void)gg_agregar_fila(ruta, fila);
}

/*
 * Concatena una base y una ruta relativa usando '/' salvo si base ya termina
 * en separador.
 * Entradas: base es el directorio; relativa es el componente a añadir.
 * Salida y efectos: nueva cadena dinámica o NULL; no modifica entradas.
 * Entrada de ejemplo: ("datos/espacio", "clientes.txt").
 * Salida de ejemplo: cadena dinámica "datos/espacio/clientes.txt".
 */
static char *gg_unir_ruta_espacio(const char *base, const char *relativa)
{
	if(base == NULL || relativa == NULL) return NULL;
	size_t longitud = strlen(base);
	const char *separador = longitud > 0 &&
		(base[longitud - 1] == '/' || base[longitud - 1] == '\\') ? "" : "/";
	return variable_string("%s%s%s", base, separador, relativa);
}

/*
 * Valida textos serializables como campos de espacio y, opcionalmente, nombres.
 * Entradas: texto es el campo; validar_nombre!=0 exige nombre no vacío, distinto
 * de "."/".." y sin separadores de ruta; todo texto rechaza '|', LF y CR.
 * Salida y efectos: 1 válido, 0 inválido; no cambia texto.
 * Entrada de ejemplo: gg_texto_campo_valido_espacio("ventas",1).
 * Salida de ejemplo: devuelve 1.
 */
static int gg_texto_campo_valido_espacio(const char *texto, int validar_nombre)
{
	if(texto == NULL || (validar_nombre && texto[0] == '\0')) return 0;
	if(validar_nombre && (strcmp(texto, ".") == 0 || strcmp(texto, "..") == 0)) return 0;
	for(const char *cursor = texto; *cursor != '\0'; ++cursor)
	{
		if(*cursor == '|' || *cursor == '\n' || *cursor == '\r' ||
			(validar_nombre && (*cursor == '/' || *cursor == '\\')))
			return 0;
	}
	return 1;
}

/*
 * Crea bajo el espacio el conjunto de archivos y encabezados de negocio configurados.
 * Entrada: id_espacio es una ruta completa o un identificador relativo al catálogo.
 * Salida y efectos: crea los archivos faltantes de GG_secciones_negocio; devuelve
 * 0 si no hubo errores y -1 si alguno no pudo crearse.
 * Entrada de ejemplo: crear_archivos_base_negocio("datos/ventas").
 * Salida de ejemplo: crea allí las secciones configuradas y devuelve 0 si la E/S funciona.
 */
int crear_archivos_base_negocio(const char *id_espacio)
{
	if(id_espacio == NULL || id_espacio[0] == '\0') return -1;

	char *base = NULL;
	if(strchr(id_espacio, '/') != NULL || strchr(id_espacio, '\\') != NULL)
	{
		base = variable_string("%s", id_espacio);
	}
	else
	{
		char *directorio_espacios = variable_string("%s", GG_archivos[0].ruta);
		if(directorio_espacios == NULL) return -1;
		char *separador = gg_buscar_ultimo_separador_ruta(directorio_espacios);
		if(separador == NULL)
		{
			sistema_memoria_liberar(directorio_espacios, NULL, 0);
			return -1;
		}
		*separador = '\0';
		base = gg_unir_ruta_espacio(directorio_espacios, id_espacio);
		sistema_memoria_liberar(directorio_espacios, NULL, 0);
	}
	if(base == NULL) return -1;

	int errores = 0;
	for(size_t i = 0; GG_secciones_negocio[i].nombre != NULL; ++i)
	{
		char *ruta = gg_unir_ruta_espacio(base, GG_secciones_negocio[i].ruta);
		if(ruta == NULL)
		{
			errores++;
			break;
		}
		if(crearArchivo(ruta, GG_secciones_negocio[i].columnas) < 0)
		{
			imprimirMensaje_para_depurar(NULL, 0,
				"[crear_archivos_base_negocio] no se pudo crear: %s\n", ruta);
			errores++;
		}
		sistema_memoria_liberar(ruta, NULL, 0);
	}
	sistema_memoria_liberar(base, NULL, 0);
	return errores == 0 ? 0 : -1;
}

/*
 * Crea un espacio con su archivo maestro, directorio y archivos base de negocio.
 * Entradas: nom_espacio es un nombre válido; usuario/contrasena son campos sin
 * separadores de registro; retorno_nombre_completo_espacio es opcional y recibe
 * el nombre creado (se inicializa a NULL).
 * Salida y efectos: crea y registra archivos; devuelve 0 o -1. El folio usa la
 * hora actual, por lo que el nombre exacto depende del momento de ejecución.
 * Entrada de ejemplo: ("ventas","ana","clave",&nombre), nombre inicialmente NULL.
 * Salida de ejemplo: en éxito devuelve 0 y *nombre pasa a "<folio_actual>_ventas".
 */
int crear_espacios(char *nom_espacio, char *usuario, char *contrasena,
	char **retorno_nombre_completo_espacio)
{
	if(retorno_nombre_completo_espacio != NULL)
		*retorno_nombre_completo_espacio = NULL;
	if(!gg_texto_campo_valido_espacio(nom_espacio, 1) ||
		!gg_texto_campo_valido_espacio(usuario, 0) ||
		!gg_texto_campo_valido_espacio(contrasena, 0))
		return -1;

	char *folio = generar_folio("%Y%m%d%H%M%S");
	if(folio == NULL) return -1;

	char *nombre_retorno = NULL;
	char *nombre = variable_string("%s_%s", folio, nom_espacio);
	char *ruta_maestra = variable_string("%s", GG_archivos[0].ruta);
	char *directorio_base = ruta_maestra != NULL
		? variable_string("%s", ruta_maestra) : NULL;
	if(nombre == NULL || ruta_maestra == NULL || directorio_base == NULL)
		goto error_crear_espacio;

	char *separador = gg_buscar_ultimo_separador_ruta(directorio_base);
	if(separador == NULL) goto error_crear_espacio;
	*separador = '\0';

	char *directorio = gg_unir_ruta_espacio(directorio_base, nombre);
	char *nombre_archivo = variable_string("%s.txt", nombre);
	if(directorio == NULL || nombre_archivo == NULL)
	{
		sistema_memoria_liberar(directorio, NULL, 0);
		sistema_memoria_liberar(nombre_archivo, NULL, 0);
		goto error_crear_espacio;
	}
	char *ruta_espacio = gg_unir_ruta_espacio(directorio, nombre_archivo);
	sistema_memoria_liberar(nombre_archivo, NULL, 0);
	if(ruta_espacio == NULL)
	{
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}
	if(existe_archivo(ruta_espacio) || gg_es_directorio(directorio) ||
		crearDirectorio(directorio) != 0)
	{
		sistema_memoria_liberar(ruta_espacio, NULL, 0);
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}

	char *contenido = variable_string("%s\n%s|%s|%s||1",
		GG_archivos[0].columnas, nombre, usuario, contrasena);
	if(contenido == NULL || crearArchivo(ruta_espacio, contenido) < 0)
	{
		sistema_memoria_liberar(contenido, NULL, 0);
		sistema_memoria_liberar(ruta_espacio, NULL, 0);
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}
	sistema_memoria_liberar(contenido, NULL, 0);

	if(retorno_nombre_completo_espacio != NULL)
	{
		nombre_retorno = variable_string("%s", nombre);
		if(nombre_retorno == NULL)
		{
			sistema_memoria_liberar(ruta_espacio, NULL, 0);
			sistema_memoria_liberar(directorio, NULL, 0);
			goto error_crear_espacio;
		}
	}

	if(crear_archivos_base_negocio(directorio) != 0)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[crear_espacios] error al crear los archivos base del espacio: %s\n", nombre);
		sistema_memoria_liberar(ruta_espacio, NULL, 0);
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}

	int estado_maestro = crearArchivo(ruta_maestra, GG_archivos[0].columnas);
	if(estado_maestro < 0)
	{
		sistema_memoria_liberar(ruta_espacio, NULL, 0);
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}
	char *linea_maestra = variable_string("%s|%s|%s|%s|2",
		nombre, usuario, contrasena, ruta_espacio);
	if(linea_maestra == NULL || gg_agregar_fila(ruta_maestra, linea_maestra) != 0)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[crear_espacios] no se pudo registrar el espacio en: %s\n", ruta_maestra);
		sistema_memoria_liberar(linea_maestra, NULL, 0);
		sistema_memoria_liberar(ruta_espacio, NULL, 0);
		sistema_memoria_liberar(directorio, NULL, 0);
		goto error_crear_espacio;
	}
	sistema_memoria_liberar(linea_maestra, NULL, 0);

	if(retorno_nombre_completo_espacio != NULL)
	{
		*retorno_nombre_completo_espacio = nombre_retorno;
		nombre_retorno = NULL;
	}

	imprimirMensaje_para_depurar(NULL, 0,
		"[crear_espacios] espacio creado: %s\n", directorio);
	sistema_memoria_liberar(ruta_espacio, NULL, 0);
	sistema_memoria_liberar(directorio, NULL, 0);
	sistema_memoria_liberar(folio, NULL, 0);
	sistema_memoria_liberar(nombre, NULL, 0);
	sistema_memoria_liberar(ruta_maestra, NULL, 0);
	sistema_memoria_liberar(directorio_base, NULL, 0);
	return 0;

error_crear_espacio:
	sistema_memoria_liberar(nombre_retorno, NULL, 0);
	sistema_memoria_liberar(folio, NULL, 0);
	sistema_memoria_liberar(nombre, NULL, 0);
	sistema_memoria_liberar(ruta_maestra, NULL, 0);
	sistema_memoria_liberar(directorio_base, NULL, 0);
	return -1;
}

/**
 * Deriva y crea el archivo base de inventario asociado a la dirección del espacio.
 * Entradas: nombre_de_structura sirve para el mensaje de diagnóstico;
 * direccion_del_espacio determina la ruta y el nombre del archivo base.
 * Salida y efectos: crea <nombre_del_archivo_espacio>_inventario.txt con columnas
 * configuradas; devuelve 0 en éxito o -1 ante entrada, conversión o E/S inválida.
 * Entrada de ejemplo: ("inventario","datos/ventas.txt").
 * Salida de ejemplo: intenta crear "datos/ventas_inventario.txt" y devuelve 0 si se crea.
 */
int creacion_de_estructura_base(char *nombre_de_structura, char *direccion_del_espacio)
{
	if(nombre_de_structura == NULL || nombre_de_structura[0] == '\0' ||
		direccion_del_espacio == NULL || direccion_del_espacio[0] == '\0')
		return -1;

	char *directorios = NULL;
	char *nombre_archivo_espacio = NULL;
	char *extension = NULL;
	char *nombre_archivo_base = NULL;
	char *columnas = NULL;
	char *ruta_archivo_base = NULL;
	int resultado = -1;

	if(desfragmentar_direccion(direccion_del_espacio, &directorios,
		&nombre_archivo_espacio, &extension) != 0 ||
		nombre_archivo_espacio == NULL || nombre_archivo_espacio[0] == '\0')
		goto terminar_creacion_estructura;

	nombre_archivo_base = variable_string("%s_inventario.txt", nombre_archivo_espacio);
	columnas = gg_reemplazar_texto(GG_secciones_negocio[1].columnas,
		GG_caracter_separacion[1], GG_caracter_separacion[0]);
	if(nombre_archivo_base == NULL || columnas == NULL)
		goto terminar_creacion_estructura;

	if(directorios != NULL && directorios[0] != '\0')
	{
		ruta_archivo_base = gg_unir_ruta_espacio(directorios, nombre_archivo_base);
	}
	else if(direccion_del_espacio[0] == '/' || direccion_del_espacio[0] == '\\')
	{
		ruta_archivo_base = variable_string("%c%s",
			direccion_del_espacio[0], nombre_archivo_base);
	}
	else
	{
		ruta_archivo_base = variable_string("%s", nombre_archivo_base);
	}
	if(ruta_archivo_base == NULL) goto terminar_creacion_estructura;

	if(crearArchivo(ruta_archivo_base, columnas) < 0)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[creacion_de_estructura_base] no se pudo crear: %s\n", ruta_archivo_base);
		goto terminar_creacion_estructura;
	}

	imprimirMensaje_para_depurar(NULL, 0,
		"Creacion de estructura base: %s (%s)\n",
		nombre_de_structura, ruta_archivo_base);
	resultado = 0;

terminar_creacion_estructura:
	sistema_memoria_liberar(directorios, NULL, 0);
	sistema_memoria_liberar(nombre_archivo_espacio, NULL, 0);
	sistema_memoria_liberar(extension, NULL, 0);
	sistema_memoria_liberar(nombre_archivo_base, NULL, 0);
	sistema_memoria_liberar(columnas, NULL, 0);
	sistema_memoria_liberar(ruta_archivo_base, NULL, 0);
	return resultado;
}

/**
 * Construye la ruta de un archivo de datos según ID y cantidad por archivo.
 * Entradas: ruta es carpeta base opcional; id y cant_por_arch determinan
 * subcarpetas y ceros; salida apunta al buffer; tam_salida es su capacidad.
 * Salida y efectos: escribe ruta terminada en NUL si cabe; la vacía al iniciar
 * y la deja vacía con argumentos/tamaño/valores inválidos; no devuelve estado.
 * Entrada de ejemplo: ruta="data", id=12, cant_por_arch=10, salida inicialmente "?", capacidad=128.
 * Salida de ejemplo: en Windows escribe "data\\10\\10.txt"; en Linux "data/10/10.txt".
 */
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

/**
 * Deriva el nombre de carpeta de datos quitando extensión del archivo metadata.
 * Entrada: ruta_metadata es la ruta de metadata.
 * Salida y efectos: cadena dinámica con sufijo "_DAT" o NULL; no modifica entrada.
 * Entrada de ejemplo: gg_carpeta_data("datos/info.txt").
 * Salida de ejemplo: cadena dinámica "datos/info_DAT".
 */
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

/**
 * Convierte una cadena decimal completa a long.
 * Entradas: texto contiene el número; valor apunta al destino.
 * Salida y efectos: devuelve 1 y escribe valor si toda la cadena es decimal
 * dentro de rango; devuelve 0 en caso contrario.
 * Entrada de ejemplo: texto="123", *valor inicialmente 0.
 * Salida de ejemplo: devuelve 1 y *valor pasa de 0 a 123.
 */
static int gg_parse_long_info(const char *texto, long *valor)
{
	if(texto == NULL || valor == NULL || texto[0] == '\0') return 0;
	char *fin = NULL;
	long convertido = strtol(texto, &fin, 10);
	if(fin == texto || *fin != '\0') return 0;
	*valor = convertido;
	return 1;
}

/**
 * Lee de metadata el ID total y la capacidad por archivo, con valores por defecto.
 * Entradas: ruta es el archivo; id_total y cantidad_por_archivo son salidas
 * opcionales para ID_TOT y CANT_POR_ARCH, respectivamente.
 * Salida y efectos: devuelve 0 al leer; escribe los punteros no NULL (defaults
 * 0 y 100 si no hay entradas válidas), o -1 si no puede abrir/leer.
 * Entrada de ejemplo: archivo con "ID_TOT|12\nCANT_POR_ARCH|50\n", salidas 0 y 0.
 * Salida de ejemplo: devuelve 0; *id_total pasa a 12 y *cantidad_por_archivo a 50.
 */
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

/**
 * Busca la entrada COLUMNAS en metadata y devuelve una copia dinámica de su valor.
 * Entradas: ruta es el archivo metadata; columnas_out apunta al puntero de salida.
 * Salida y efectos: inicializa *columnas_out a NULL; devuelve 0 con cadena
 * asignada si encuentra COLUMNAS, o -1 ante error/ausencia.
 * Entrada de ejemplo: archivo incluye "COLUMNAS|id,nombre", *columnas_out inicialmente "x".
 * Salida de ejemplo: devuelve 0 y *columnas_out pasa de "x" a una copia de "id,nombre".
 */
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

/**
 * Sustituye el valor de ID_TOT en metadata y guarda nuevamente el archivo.
 * Entradas: ruta es metadata; nuevo_id es el valor decimal que se persistirá.
 * Salida y efectos: reescribe metadata si encuentra ID_TOT; devuelve 0 en éxito
 * o -1 si falla lectura, falta la clave, o no se puede guardar.
 * Entrada de ejemplo: metadata contiene "ID_TOT|4", nuevo_id=5.
 * Salida de ejemplo: la línea queda "ID_TOT|5" y devuelve 0 si E/S concluye.
 */
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

/**
 * Calcula una ruta de archivo de datos derivada de metadata y un ID.
 * Entradas: ruta_metadata define la carpeta base; id y cantidad_por_archivo
 * determinan el particionado numérico.
 * Salida y efectos: devuelve una ruta dinámica o NULL si no se puede construir;
 * no modifica metadata.
 * Entrada de ejemplo: ("datos/info.txt",12,10).
 * Salida de ejemplo: Windows "datos/info_DAT\\10\\10.txt"; Linux "datos/info_DAT/10/10.txt".
 */
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

/**
 * Serializa celdas usando el separador de campo configurado para información.
 * Entradas: celdas es el vector de valores; cantidad es su longitud; NULL se
 * representa como campo vacío.
 * Salida y efectos: cadena dinámica o NULL ante error; no modifica celdas.
 * Entrada de ejemplo: gg_reconstruir_fila_info({"1","Ana"},2).
 * Salida de ejemplo: cadena dinámica "1|Ana" con el separador predeterminado.
 */
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

/**
 * Serializa un código y un mensaje con el marcador de confirmación/error.
 * Entradas: codigo es el estado; mensaje es el texto del estado.
 * Salida y efectos: devuelve cadena dinámica "código╣mensaje" con el marcador
 * configurado, o NULL ante fallo de reserva; no cambia argumentos.
 * Entrada de ejemplo: gg_info_respuesta(0,"no_encontrado").
 * Salida de ejemplo: cadena dinámica "0╣no_encontrado".
 */
static char *gg_info_respuesta(int codigo, const char *mensaje)
{
	return variable_string("%d%s%s", codigo, GG_caracter_para_confirmacion_o_error[0], mensaje);
}

/**
 * Serializa código y fila usando el marcador de confirmación/error.
 * Entradas: codigo es el estado; fila es el texto (NULL equivale a vacío).
 * Salida y efectos: cadena dinámica con formato "código╣fila" o NULL si falla.
 * Entrada de ejemplo: gg_info_respuesta_fila(1,"1|Ana").
 * Salida de ejemplo: cadena dinámica "1╣1|Ana".
 */
static char *gg_info_respuesta_fila(int codigo, const char *fila)
{
	return variable_string("%d%s%s", codigo, GG_caracter_para_confirmacion_o_error[0], fila != NULL ? fila : "");
}

/**
 * Crea un archivo metadata inicial con contador 0, columnas y capacidad 100.
 * Entradas: ruta es destino; columnas es el encabezado (NULL equivale a vacío).
 * Salida y efectos: crea el archivo si no existe; devuelve 0 creado, 1 existente
 * según crearArchivo, o -1 ante error.
 * Entrada de ejemplo: gg_crear_metadata_info("datos/info.txt","id|nombre"), ruta ausente.
 * Salida de ejemplo: crea metadata con ID_TOT=0 y CANT_POR_ARCH=100; devuelve 0.
 */
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

/**
 * Lee un archivo completo y une sus líneas con LF en una cadena.
 * Entrada: ruta identifica el archivo.
 * Salida y efectos: devuelve texto dinámico o NULL ante error; libera internamente
 * el vector de líneas y no modifica el archivo.
 * Entrada de ejemplo: leer_info_dividida("datos.txt"), contenido "a\nb\n".
 * Salida de ejemplo: cadena dinámica "a\nb".
 */
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

/**
 * Agrega un registro a la información particionada y aumenta ID_TOT en metadata.
 * Entradas: direccion es la ruta de metadata; agregando es el contenido de fila
 * (NULL equivale a vacío); nom_columnas es encabezado opcional para crear metadata.
 * Salida y efectos: crea/actualiza metadata y archivo de datos; devuelve respuesta
 * dinámica con código y fila agregada, o código/mensaje de error.
 * Entrada de ejemplo: metadata vacía/ausente, agregar_info_dividida("datos/info.txt","Ana","id|nombre").
 * Salida de ejemplo: crea el registro "1|Ana" y devuelve "1╣1|Ana" si E/S concluye.
 */
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

/**
 * Lee los registros de todos los archivos de datos particionados por metadata.
 * Entrada: ruta identifica metadata, cuyo contador y tamaño de partición guían
 * la lectura; omite la primera línea de encabezado de cada archivo de datos.
 * Salida y efectos: devuelve filas unidas por el separador específico configurado,
 * "" si no hay registros, o NULL ante error; no modifica los archivos.
 * Entrada de ejemplo: metadata ID_TOT=1 y archivo de datos con encabezado y "1|Ana".
 * Salida de ejemplo: cadena dinámica "1|Ana".
 */
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

/**
 * Busca una fila por su ID en el archivo de datos indicado por la metadata.
 * Entradas: ruta_metadata es la ruta de metadata; id es el ID textual; ruta_data_out,
 * indice_out y fila_out son salidas opcionales para ruta del bloque, índice de línea
 * y copia de la fila. Devuelve 0 al hallar la fila y -1 ante error o ausencia.
 * Las salidas solicitadas se inicializan como NULL, -1 y NULL; el llamador libera
 * las cadenas recibidas.
 * Entrada de ejemplo: ruta_metadata="productos", id="7",
 * ruta_data_out=NULL, indice_out=-1, fila_out=NULL, con fila almacenada "7|Manzana|3".
 * Salida de ejemplo: retorna 0; ruta_data_out pasa de NULL a la ruta del bloque
 * asignada, indice_out de -1 al índice de la línea y fila_out de NULL a
 * "7|Manzana|3" (si se pidieron las tres salidas).
 */
/**
 * Localiza en el archivo de partición la fila cuyo primer campo coincide con ID.
 * Entradas: ruta_metadata e id identifican registro; ruta_data_out, indice_out
 * y fila_out son salidas opcionales para ruta, índice de línea desde cero y copia.
 * Salida y efectos: inicializa salidas a NULL/-1; devuelve 0 y rellena las
 * solicitadas al encontrarla, o -1 si id/datos/fila no existen.
 * Entrada de ejemplo: ID "1" ocupa la línea 1; salidas (NULL,-1,NULL) inicialmente.
 * Salida de ejemplo: devuelve 0; índice=1 si antes hay encabezado y fila_out="1|Ana";
 * la ruta de datos queda asignada (su formato depende de plataforma).
 */
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

/**
 * Devuelve una respuesta de protocolo con la fila cuyo ID coincide.
 * Entradas: direccion es la ruta de metadata y id es el ID textual.
 * Salida: cadena nueva con estado 1 y fila, o estado 0 si no se encuentra;
 * no modifica los archivos.
 * Entrada de ejemplo: seleccionar_id_info_dividida("productos", "7"),
 * cuando la fila es "7|Manzana|3".
 * Salida de ejemplo: "1╣7|Manzana|3" (╣ es el separador de confirmación configurado).
 */
/**
 * Busca un ID y devuelve su fila serializada en una respuesta de confirmación.
 * Entradas: direccion es metadata; id es el identificador de registro.
 * Salida y efectos: cadena dinámica "1╣fila" si existe o "0╣no_se_encontro_informacion";
 * no modifica los archivos.
 * Entrada de ejemplo: direccion con fila "1|Ana", id="1".
 * Salida de ejemplo: cadena dinámica "1╣1|Ana".
 */
char *seleccionar_id_info_dividida(const char *direccion, const char *id)
{
	char *fila = NULL;
	if(gg_buscar_fila_por_id(direccion, id, NULL, NULL, &fila) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	return respuesta;
}

/**
 * Recupera una fila junto con la ruta del bloque y el índice donde reside.
 * Entradas: direccion es la metadata e id identifica la fila.
 * Salida: cadena asignada con estado, fila, ruta e índice separados por ╣;
 * en ausencia de la fila devuelve respuesta de estado 0.
 * Entrada de ejemplo: seleccionar_id_info_dividida_extrae("productos", "7"),
 * si existe "7|Manzana|3" en el índice 2.
 * Salida de ejemplo: "1╣7|Manzana|3╣<ruta-del-bloque>╣2"; la ruta concreta
 * depende de la configuración de bloques del archivo.
 */
/**
 * Busca un ID y devuelve fila, ruta de partición e índice de línea en la respuesta.
 * Entradas: direccion es metadata; id es el identificador.
 * Salida y efectos: respuesta dinámica "1╣fila╣ruta╣índice" si existe, o código
 * 0 y mensaje de ausencia; solo consulta archivos.
 * Entrada de ejemplo: fila "1|Ana" en primera línea de datos tras el encabezado, id="1".
 * Salida de ejemplo: "1╣1|Ana╣<ruta_de_datos>╣1"; la ruta depende de plataforma.
 */
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

/**
 * Sustituye varias celdas de una fila y persiste el archivo de datos.
 * Entradas: ruta_data identifica el archivo, indice_fila la línea, indices
 * contiene índices de columna textuales, valores sus reemplazos y cantidad
 * el número de pares. Devuelve 0 si guarda la fila o -1 si falla; los pares
 * con índice inválido se omiten.
 * Entrada de ejemplo: fila "7|Manzana|3", índice 0, indices={"2"},
 * valores={"4"}, cantidad=1.
 * Salida de ejemplo: retorna 0 y la fila persistida queda "7|Manzana|4".
 */
/**
 * Actualiza en disco celdas concretas de una fila del archivo de datos.
 * Entradas: ruta_data identifica archivo; indice_fila es índice de línea desde
 * cero; indices son índices de columna en texto; valores son sus reemplazos;
 * cantidad indica pares a procesar.
 * Salida y efectos: reescribe el archivo si la fila puede reconstruirse; devuelve
 * 0 en éxito o -1 ante error/índice de fila inválido.
 * Entrada de ejemplo: fila "1|Ana|25", índice_fila=0, indices={"2"}, valores={"30"}.
 * Salida de ejemplo: fila queda "1|Ana|30" y devuelve 0 si E/S concluye.
 */
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

/**
 * Edita varias celdas de la fila identificada y devuelve el estado de la operación.
 * Entradas: direccion es metadata, id la fila, cols_str y vals_str listas paralelas
 * de columnas y valores, separadas por el delimitador específico configurado.
 * Salida: respuesta de protocolo 1 si se guardó o 0 si no se pudo editar.
 * Entrada de ejemplo: editar_celda_id_info_dividida(NULL,"7","2","4").
 * Salida de ejemplo: "0╣no_se_encontro_informacion"; no modifica archivos.
 * Para una dirección existente no puede precisarse el resultado: el cuerpo usa
 * Las listas paralelas usan GG_caracter_separacion_funciones_espesificas[0].
 */
/**
 * Edita uno o varios campos de la fila identificada por ID.
 * Entradas: direccion e id localizan el registro; cols_str enumera columnas;
 * vals_str enumera reemplazos en el delimitador específico de la API.
 * Salida y efectos: modifica el archivo de datos y devuelve respuesta dinámica
 * de éxito/ausencia/error; la interpretación exacta de las listas depende del
 * separador configurado para esta API.
 * Entrada de ejemplo: metadata válida sin fila con ID "99"; cols_str="1", vals_str="Ana".
 * Salida de ejemplo: devuelve "0╣no_se_encontro_informacion" y no edita datos.
 */
char *editar_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *vals_str)
{
	char *ruta_data = NULL;
	int indice = -1;
	if(gg_buscar_fila_por_id(direccion, id, &ruta_data, &indice, NULL) != 0)
		return gg_info_respuesta(0, "no_se_encontro_informacion");
	char **indices = NULL;
	char **valores = NULL;
	int n_indices = split_con_numero_de_celdas(cols_str, GG_caracter_separacion_funciones_espesificas[0], &indices);
	int n_valores = split_con_numero_de_celdas(vals_str, GG_caracter_separacion_funciones_espesificas[0], &valores);
	int n = n_indices < n_valores ? n_indices : n_valores;
	int resultado = n > 0 && indices != NULL && valores != NULL
		? gg_editar_campos_en_archivo(ruta_data, indice, indices, valores, n) : -1;
	if(indices != NULL) free_split_con_numero_de_celdas(indices);
	if(valores != NULL) free_split_con_numero_de_celdas(valores);
	sistema_memoria_liberar(ruta_data, NULL, 0);
	return gg_info_respuesta(resultado == 0 ? 1 : 0,
		resultado == 0 ? "informacion_editada" : "no_se_encontro_informacion");
}

/**
 * Edita una columna por ID, opcionalmente exigiendo que otra columna coincida.
 * Entradas: direccion e id localizan la fila; info_a_comparar y col_comparar
 * son la condición opcional; nuevos_datos es el valor y col_editar su columna.
 * Salida: respuesta 1 si la escritura tuvo éxito, o 0 si no coincidió o falló.
 * Entrada de ejemplo: editar_id_info_dividida("productos","7","Manzana","1","Pera",1),
 * con la fila "7|Manzana|3".
 * Salida de ejemplo: "1╣informacion_editada"; la fila pasa a "7|Pera|3".
 */
/**
 * Edita una columna de la fila identificada, opcionalmente condicionada por otra.
 * Entradas: direccion/id localizan fila; info_a_comparar y col_comparar, si ambos
 * no vacíos, exigen valor esperado en la columna indicada desde cero; nuevos_datos
 * es el reemplazo (NULL equivale a vacío); col_editar es la columna desde cero.
 * Salida y efectos: persiste el cambio si hay coincidencia; devuelve respuesta
 * dinámica con código 1/0 y estado.
 * Entrada de ejemplo: fila "1|Ana|25", id="1", comparar="25", col="2", nuevo="30", editar=2.
 * Salida de ejemplo: fila queda "1|Ana|30" y respuesta "1╣informacion_editada".
 */
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

/**
 * Suma cantidades a varias columnas numéricas y persiste la fila resultante.
 * Entradas: direccion e id localizan la fila; indices_texto y cantidades_texto
 * son listas paralelas; copia_si_vacio activa el modo de copia/deficit;
 * fuentes_texto da las columnas fuente opcionales; fila_resultado recibe una
 * copia de la fila escrita. Devuelve -1 ante fallo; en modo copia también
 * puede devolver 1 + bultos cuando hubo déficit.
 * Entrada de ejemplo: direccion=NULL, id="7", indices="2", cantidades="2",
 * copia_si_vacio=0, fuentes_texto=NULL y fila_resultado apuntando a NULL.
 * Salida de ejemplo: retorna -1 y fila_resultado permanece NULL.
 * Para una dirección existente no puede precisarse el resultado: el cuerpo usa
 * Las listas paralelas usan GG_caracter_separacion_funciones_espesificas[0].
 */
/**
 * Suma cantidades a campos numéricos de una fila y guarda la fila actualizada.
 * Entradas: direccion/id localizan registro; indices_texto y cantidades_texto
 * son listas paralelas de columnas y sumandos; copia_si_vacio activa copia desde
 * fuentes_texto (lista de columnas fuente) cuando hay déficit; fila_resultado
 * es una salida opcional con copia de la fila final.
 * Salida y efectos: inicializa *fila_resultado a NULL; modifica el archivo y
 * devuelve 1 al sumar sin déficit, bultos+2 con déficit o -1 ante error.
 * Entrada de ejemplo: metadata válida sin fila con ID "99"; salida de fila NULL.
 * Salida de ejemplo: devuelve -1 y *fila_resultado permanece NULL.
 */
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
	int n_indices = split_con_numero_de_celdas(indices_texto, GG_caracter_separacion_funciones_espesificas[0], &indices);
	int n_cantidades = split_con_numero_de_celdas(cantidades_texto, GG_caracter_separacion_funciones_espesificas[0], &cantidades);
	int n_fuentes = copia_si_vacio
		? split_con_numero_de_celdas(fuentes_texto != NULL ? fuentes_texto : "", GG_caracter_separacion_funciones_espesificas[0], &fuentes)
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

/**
 * Incrementa las columnas indicadas de la fila identificada, sin modo de copia.
 * Entradas: direccion e id localizan la fila; cols_str y cants_str contienen
 * índices y cantidades paralelos. Salida: respuesta de protocolo con fila nueva
 * en éxito o estado 0 si no se encontró/pudo actualizar.
 * Entrada de ejemplo: incrementa_celda_id_info_dividida(NULL,"7","2","2").
 * Salida de ejemplo: "0╣no_se_encontro_informacion".
 * Las columnas y cantidades usan GG_caracter_separacion_funciones_espesificas[0].
 */
char *incrementa_celda_id_info_dividida(const char *direccion, const char *id, const char *cols_str, const char *cants_str)
{
	char *fila = NULL;
	int resultado = gg_incrementar_campos(direccion, id, cols_str, cants_str, 0, NULL, &fila);
	if(resultado < 0) return gg_info_respuesta(0, "no_se_encontro_informacion");
	char *respuesta = gg_info_respuesta_fila(1, fila);
	sistema_memoria_liberar(fila, NULL, 0);
	return respuesta;
}

/**
 * Incrementa columnas permitiendo copiar una columna fuente ante valor vacío o déficit.
 * Entradas: direccion e id localizan la fila; cols_str/cants_str son columnas y
 * cantidades; cols_fuente_str indica fuentes de copia paralelas. Devuelve una
 * respuesta con fila actualizada; si hay déficit, el estado 2 incluye el número
 * de bultos requeridos como tercer campo.
 * Entrada de ejemplo: incrementa_celda_id_info_dividida_copia_si_cero(NULL,"7","1","2","2").
 * Salida de ejemplo: "0╣no_se_encontro_informacion".
 * Las columnas y cantidades usan GG_caracter_separacion_funciones_espesificas[0].
 */
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

/**
 * Agrega una fila solo cuando no existe otra con el mismo valor en una columna.
 * Entradas: direccion es la metadata; col_comp es el índice de comparación;
 * comparar es el valor buscado; texto_agr es la fila por agregar y nom_cols
 * los encabezados iniciales opcionales. Salida: respuesta de duplicado o la
 * respuesta de agregar_info_dividida; puede crear metadata y modificar datos.
 * Entrada de ejemplo: columna 1, comparar="Manzana", fila nueva="Pera|4",
 * con "Manzana" ya presente en la columna 1.
 * Salida de ejemplo: respuesta con código 0, "ya_existe" y la fila encontrada;
 * no se agrega la fila duplicada.
 */
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

/**
 * Busca el primer valor exacto en una columna, probando antes un ID sugerido.
 * Entradas: direccion es metadata, dato_buscar el valor, col_buscar su columna
 * e id_hint un ID opcional para acelerar la búsqueda (1..total).
 * Salida: respuesta con fila e ID en éxito o código 0 si no hay coincidencia.
 * Entrada de ejemplo: buscar_info_dividida("productos","Manzana",1,7),
 * con ID 7 y fila "7|Manzana|3".
 * Salida de ejemplo: "1╣7|Manzana|3╣7".
 */
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

/**
 * Busca un valor y, si existe, incrementa sus columnas; si no, agrega fallback.
 * Entradas: direccion es metadata; dato_a_buscar y col_buscar definen la búsqueda;
 * cols_inc_str/cants_inc_str son columnas y cantidades; agregar_fallback es la
 * fila opcional que se agrega al no hallar coincidencia. Devuelve la respuesta
 * de la actualización, del agregado o de ausencia.
 * Entrada de ejemplo: incrementa_celda_busqueda_info_dividida(NULL,"Manzana",1,"2","2",NULL).
 * Salida de ejemplo: "0╣no_se_encontro_informacion".
 * Si encuentra fila, las columnas/cantidades se separan con el primer delimitador específico.
 */
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

/**
 * Valida credenciales contra el archivo de espacios y obtiene el nivel autorizado.
 * Entradas: nivel_minimo es el umbral requerido; ruta_archivo identifica el registro;
 * id_de_espacio, usuario y contrasena identifican la credencial. Las salidas
 * opcionales reciben la dirección del negocio y el nivel almacenado. Devuelve 0
 * cuando nivel_usuario <= nivel_minimo, 1 si existe pero no alcanza el umbral,
 * y -1 ante datos/archivo inválidos o error. Inicializa las salidas a NULL y -1.
 * Entrada de ejemplo: una fila de archivo "esp1|ana|clave|/negocio|2" y una llamada
 * con nivel_minimo=2, ruta_archivo="usuarios", id_de_espacio="esp1",
 * usuario="ana", contrasena="clave", dirección=NULL y nivel=-1.
 * Salida de ejemplo: retorna 0; la dirección pasa de NULL a "/negocio" y el nivel
 * de -1 a 2.
 */
static int gg_checar_permiso_archivo(
	int nivel_minimo,
	const char *ruta_archivo,
	const char *id_de_espacio,
	const char *usuario,
	const char *contrasena,
	char **retorna_direccion_espacio_negocio,
	int *retornar_nivel)
{
	if(retorna_direccion_espacio_negocio != NULL)
		*retorna_direccion_espacio_negocio = NULL;
	if(retornar_nivel != NULL)
		*retornar_nivel = -1;

	if(ruta_archivo == NULL || id_de_espacio == NULL ||
		usuario == NULL || contrasena == NULL)
		return -1;

	imprimirMensaje_para_depurar(NULL, 0,
		"[checar_permiso] ruta='%s', id='%s', usuario='%s', nivel_minimo=%d\n",
		ruta_archivo, id_de_espacio, usuario, nivel_minimo);

	if(!existe_archivo(ruta_archivo))
		return -1;

	int n_lineas = 0;
	char **lineas = leer_archivo(ruta_archivo, &n_lineas);
	if(lineas == NULL)
		return -1;

	int resultado = 1;
	const char *separador = GG_caracter_separacion[0];
	if(separador == NULL || separador[0] == '\0')
	{
		free_lineas(lineas, n_lineas);
		return -1;
	}

	for(int i = 1; i < n_lineas; ++i)
	{
		if(lineas[i] == NULL)
			continue;

		char **columnas = NULL;
		int n_columnas = split_con_numero_de_celdas(
			lineas[i], separador, &columnas);
		if(n_columnas < 0)
		{
			free_lineas(lineas, n_lineas);
			return -1;
		}
		if(n_columnas < 5 || columnas == NULL)
		{
			free_split_con_numero_de_celdas(columnas);
			continue;
		}

		if(strcmp(columnas[0], id_de_espacio) == 0 &&
			strcmp(columnas[1], usuario) == 0 &&
			strcmp(columnas[2], contrasena) == 0)
		{
			int nivel_usuario = 0;
			if(texto_a_int_seguro(columnas[4], &nivel_usuario) != 0)
			{
				free_split_con_numero_de_celdas(columnas);
				free_lineas(lineas, n_lineas);
				return -1;
			}

			char *direccion = NULL;
			if(retorna_direccion_espacio_negocio != NULL)
			{
				direccion = variable_string("%s", columnas[3]);
				if(direccion == NULL)
				{
					free_split_con_numero_de_celdas(columnas);
					free_lineas(lineas, n_lineas);
					return -1;
				}
			}

			if(retorna_direccion_espacio_negocio != NULL)
				*retorna_direccion_espacio_negocio = direccion;
			if(retornar_nivel != NULL)
				*retornar_nivel = nivel_usuario;
			resultado = nivel_usuario <= nivel_minimo ? 0 : 1;
			free_split_con_numero_de_celdas(columnas);
			break;
		}

		free_split_con_numero_de_celdas(columnas);
	}

	free_lineas(lineas, n_lineas);
	return resultado;
}

/**
 * Interfaz pública para comprobar credenciales y recuperar la dirección del negocio.
 * Entradas: nivel_minimo es el umbral; ruta_archivo, id_de_espacio, usuario y
 * contrasena seleccionan la cuenta; los dos últimos argumentos son salidas opcionales
 * para dirección y nivel. Devuelve el estado 0/1/-1 de gg_checar_permiso_archivo.
 * Entrada de ejemplo: nivel_minimo=2, fila almacenada "esp1|ana|clave|/negocio|2",
 * salidas inicialmente direccion=NULL y nivel=-1.
 * Salida de ejemplo: retorna 0; direccion pasa a "/negocio" y nivel a 2.
 */
int checar_permiso(
	int nivel_minimo,
	const char *ruta_archivo,
	const char *id_de_espacio,
	const char *usuario,
	const char *contrasena,
	char **retorna_direccion_espacio_negocio,
	int *retornar_nivel)
{
	return gg_checar_permiso_archivo(
		nivel_minimo,
		ruta_archivo,
		id_de_espacio,
		usuario,
		contrasena,
		retorna_direccion_espacio_negocio,
		retornar_nivel);
}

/**
 * Comprueba permisos de negocio sin solicitar la dirección asociada.
 * Entradas: nivel_minimo es el umbral; ruta_archivo, id_de_espacio, usuario y
 * contrasena identifican la cuenta; retornar_nivel recibe el nivel encontrado.
 * Devuelve 0 si cumple, 1 si el nivel es insuficiente o -1 ante error.
 * Entrada de ejemplo: archivo con "esp1|ana|clave|/negocio|2", umbral 2,
 * retornar_nivel inicialmente -1.
 * Salida de ejemplo: retorna 0 y retornar_nivel cambia de -1 a 2.
 */
int checar_permiso_negocios(
	int nivel_minimo,
	const char *ruta_archivo,
	const char *id_de_espacio,
	const char *usuario,
	const char *contrasena,
	int *retornar_nivel)
{
	return gg_checar_permiso_archivo(
		nivel_minimo,
		ruta_archivo,
		id_de_espacio,
		usuario,
		contrasena,
		NULL,
		retornar_nivel);
}

#else

/**
 * Stub no implementado para crear un directorio en la plataforma sin backend.
 * Entrada: ruta es el nombre solicitado. Salida: siempre -1, sin cambios de sistema.
 * Entrada de ejemplo: crearDirectorio("datos").
 * Salida de ejemplo: -1.
 */
int crearDirectorio(const char *ruta) { (void)ruta; return -1; }
/**
 * Stub de creación de archivo: recibe ruta y cabecera, pero no escribe en disco.
 * Entrada de ejemplo: crearArchivo("datos.csv", "id|nombre").
 * Salida de ejemplo: -1.
 */
int crearArchivo(const char *ruta, const char *cabecera) { (void)ruta; (void)cabecera; return -1; }
/**
 * Stub de lectura; ruta indica el archivo y n_lineas_out es salida opcional.
 * Inicializa el contador a cero y no devuelve un arreglo.
 * Entrada de ejemplo: n_lineas_out=5; leer_archivo("datos.csv", &n_lineas_out).
 * Salida de ejemplo: NULL y n_lineas_out cambia de 5 a 0.
 */
char **leer_archivo(const char *ruta, int *n_lineas_out) { (void)ruta; if(n_lineas_out != NULL) *n_lineas_out = 0; return NULL; }
/**
 * Stub de persistencia: acepta ruta, arreglo y cantidad, sin escribirlos.
 * Entrada de ejemplo: guardar_archivo("datos.csv", {"1|Ana"}, 1).
 * Salida de ejemplo: no retorna valor y no produce efectos.
 */
void guardar_archivo(const char *ruta, char **lineas, int n_lineas) { (void)ruta; (void)lineas; (void)n_lineas; }
/**
 * Stub de liberación de un arreglo de líneas. Entrada: lineas y cantidad.
 * Salida: ninguna; no libera memoria en esta plataforma.
 * Entrada de ejemplo: free_lineas(NULL, 0). Salida de ejemplo: retorna void.
 */
void free_lineas(char **lineas, int n_lineas) { (void)lineas; (void)n_lineas; }
/**
 * Stub de agregado de fila. Entradas: ruta destino y texto de fila.
 * Salida: ninguna y ningún cambio persistente.
 * Entrada de ejemplo: agregar_fila("datos.csv", "1|Ana"). Salida de ejemplo: void.
 */
void agregar_fila(const char *ruta, const char *fila) { (void)ruta; (void)fila; }
/**
 * Stub de existencia de archivo; recibe la ruta y reporta que no existe.
 * Entrada de ejemplo: existe_archivo("datos.csv"). Salida de ejemplo: 0.
 */
int existe_archivo(const char *ruta) { (void)ruta; return 0; }
/**
 * Stub de construcción de ruta; ruta, id y cantidad se ignoran, salida se vacía
 * cuando es válida y capacidad es mayor que cero.
 * Entrada de ejemplo: salida="x", capacidad=8; generar_ruta_archivo("datos",2,4,salida,8).
 * Salida de ejemplo: salida pasa de "x" a "".
 */
void generar_ruta_archivo(const char *ruta, long id, long cantidad, char *salida, size_t capacidad) { (void)ruta; (void)id; (void)cantidad; if(salida != NULL && capacidad > 0) salida[0] = '\0'; }
/**
 * Stub de alta de espacio; acepta nombre, usuario, contraseña y salida opcional.
 * Inicializa retorno a NULL y devuelve -1 sin crear registros.
 * Entrada de ejemplo: retorno apunta a "x"; crear_espacios("Tienda","ana","clave",&retorno).
 * Salida de ejemplo: -1 y retorno cambia de "x" a NULL.
 */
int crear_espacios(char *nombre, char *usuario, char *contrasena, char **retorno) { (void)nombre; (void)usuario; (void)contrasena; if(retorno != NULL) *retorno = NULL; return -1; }
/**
 * Stub de creación de archivos base; id_espacio identifica el negocio.
 * Entrada de ejemplo: crear_archivos_base_negocio("esp1"). Salida de ejemplo: -1.
 */
int crear_archivos_base_negocio(const char *id_espacio) { (void)id_espacio; return -1; }
/**
 * Stub de creación de estructura; nombre y direccion se ignoran.
 * Entrada de ejemplo: creacion_de_estructura_base("Tienda","/tienda").
 * Salida de ejemplo: -1.
 */
int creacion_de_estructura_base(char *nombre, char *direccion) { (void)nombre; (void)direccion; return -1; }
/**
 * Stub de lectura de información dividida. Entrada: ruta de metadata.
 * Salida: siempre NULL, sin acceso a almacenamiento.
 * Entrada de ejemplo: leer_info_dividida("productos"). Salida de ejemplo: NULL.
 */
char *leer_info_dividida(const char *ruta) { (void)ruta; return NULL; }
/**
 * Stub de agregado a metadata dividida. Entradas: dirección, fila y columnas.
 * Salida: siempre NULL; no crea metadata ni escribe la fila.
 * Entrada de ejemplo: agregar_info_dividida("productos","Manzana|3","nombre|stock").
 * Salida de ejemplo: NULL.
 */
char *agregar_info_dividida(const char *direccion, const char *agregando, const char *nom_columnas) { (void)direccion; (void)agregando; (void)nom_columnas; return NULL; }
/**
 * Stub que debería concatenar todos los bloques de una ruta.
 * Entrada de ejemplo: leer_todo_info_dividida("productos"). Salida de ejemplo: NULL.
 */
char *leer_todo_info_dividida(const char *ruta) { (void)ruta; return NULL; }
/**
 * Stub de agregado condicional; recibe metadata, columna/valor de comparación,
 * fila nueva y encabezados. Salida: NULL, sin examinar ni escribir datos.
 * Entrada de ejemplo: agregar_sino_existe_info_dividida("productos",1,"A","B|2","nombre|stock").
 * Salida de ejemplo: NULL.
 */
char *agregar_sino_existe_info_dividida(const char *direccion, long col, const char *comparar, const char *fila, const char *columnas) { (void)direccion; (void)col; (void)comparar; (void)fila; (void)columnas; return NULL; }
/**
 * Stub de selección por ID. Entradas: metadata e ID textual.
 * Salida: NULL porque este backend aún no implementa lectura.
 * Entrada de ejemplo: seleccionar_id_info_dividida("productos","7").
 * Salida de ejemplo: NULL.
 */
char *seleccionar_id_info_dividida(const char *direccion, const char *id) { (void)direccion; (void)id; return NULL; }
/**
 * Stub de selección con datos de ubicación. Entradas: metadata e ID textual.
 * Salida: NULL, sin modificar almacenamiento.
 * Entrada de ejemplo: seleccionar_id_info_dividida_extrae("productos","7").
 * Salida de ejemplo: NULL.
 */
char *seleccionar_id_info_dividida_extrae(const char *direccion, const char *id) { (void)direccion; (void)id; return NULL; }
/**
 * Stub de edición condicional por ID; recibe ruta, ID, condición, columna, valor y
 * columna a editar. Salida: NULL, sin leer ni modificar archivos.
 * Entrada de ejemplo: editar_id_info_dividida("productos","7","A","1","B",1).
 * Salida de ejemplo: NULL.
 */
char *editar_id_info_dividida(const char *direccion, const char *id, const char *comparar, const char *columna, const char *nuevo, int editar) { (void)direccion; (void)id; (void)comparar; (void)columna; (void)nuevo; (void)editar; return NULL; }
/**
 * Stub de edición de celdas; cols y vals describen los reemplazos.
 * Entrada de ejemplo: editar_celda_id_info_dividida("productos","7","2","4").
 * Salida de ejemplo: NULL, sin cambios en datos.
 */
char *editar_celda_id_info_dividida(const char *direccion, const char *id, const char *cols, const char *vals) { (void)direccion; (void)id; (void)cols; (void)vals; return NULL; }
/**
 * Stub de incremento por ID; cols y cants son listas de columnas y cantidades.
 * Entrada de ejemplo: incrementa_celda_id_info_dividida("productos","7","2","1").
 * Salida de ejemplo: NULL.
 */
char *incrementa_celda_id_info_dividida(const char *direccion, const char *id, const char *cols, const char *cants) { (void)direccion; (void)id; (void)cols; (void)cants; return NULL; }
/**
 * Stub de incremento con fuentes para copia; acepta metadata, ID, columnas,
 * cantidades y fuentes. Salida: NULL, sin persistir valores.
 * Entrada de ejemplo: incrementa_celda_id_info_dividida_copia_si_cero("productos","7","1","2","2").
 * Salida de ejemplo: NULL.
 */
char *incrementa_celda_id_info_dividida_copia_si_cero(const char *direccion, const char *id, const char *cols, const char *cants, const char *fuentes) { (void)direccion; (void)id; (void)cols; (void)cants; (void)fuentes; return NULL; }
/**
 * Stub de búsqueda seguida de incremento o agregado fallback.
 * Entrada: metadata, valor y columna de búsqueda, columnas/cantidades, fallback.
 * Entrada de ejemplo: incrementa_celda_busqueda_info_dividida("productos","A",1,"2","1","A|1").
 * Salida de ejemplo: NULL; no se consulta ni modifica almacenamiento.
 */
char *incrementa_celda_busqueda_info_dividida(const char *direccion, const char *dato, int col, const char *cols, const char *cants, const char *fallback) { (void)direccion; (void)dato; (void)col; (void)cols; (void)cants; (void)fallback; return NULL; }
/**
 * Stub de búsqueda por valor; recibe metadata, valor, columna e ID sugerido.
 * Entrada de ejemplo: buscar_info_dividida("productos","A",1,7).
 * Salida de ejemplo: NULL.
 */
char *buscar_info_dividida(const char *direccion, const char *dato, int col, int hint) { (void)direccion; (void)dato; (void)col; (void)hint; return NULL; }
/**
 * Stub de comprobación de permisos; entradas identifican umbral, archivo, espacio
 * y credenciales; las salidas opcionales de dirección/nivel se limpian.
 * Entrada de ejemplo: dirección="x", nivel=3; llamada con credenciales no nulas.
 * Salida de ejemplo: retorna -1; dirección pasa de "x" a NULL y nivel de 3 a -1.
 */
int checar_permiso(
	int nivel_minimo,
	const char *ruta_archivo,
	const char *id_de_espacio,
	const char *usuario,
	const char *contrasena,
	char **retorna_direccion_espacio_negocio,
	int *retornar_nivel)
{
	(void)nivel_minimo;
	(void)ruta_archivo;
	(void)id_de_espacio;
	(void)usuario;
	(void)contrasena;
	if(retorna_direccion_espacio_negocio != NULL)
		*retorna_direccion_espacio_negocio = NULL;
	if(retornar_nivel != NULL)
		*retornar_nivel = -1;
	return -1;
}
/**
 * Stub de comprobación de permisos de negocio; recibe umbral, archivo, espacio,
 * usuario, contraseña y salida opcional de nivel.
 * Entrada de ejemplo: retornar_nivel=3; llamada con argumentos de texto válidos.
 * Salida de ejemplo: retorna -1 y retornar_nivel pasa de 3 a -1.
 */
int checar_permiso_negocios(
	int nivel_minimo,
	const char *ruta_archivo,
	const char *id_de_espacio,
	const char *usuario,
	const char *contrasena,
	int *retornar_nivel)
{
	(void)nivel_minimo;
	(void)ruta_archivo;
	(void)id_de_espacio;
	(void)usuario;
	(void)contrasena;
	if(retornar_nivel != NULL)
		*retornar_nivel = -1;
	return -1;
}

#endif

#pragma endregion FUNCIONES_INFO_DIVIDIDA
 
 // ============================================================================
 // FUNCIONES OPERACIONES DE MENSAJERÍA
 // ============================================================================

 #pragma region FUNCIONES_OPERACIONES_DE_MENSAJERIA

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)

/**
 * Copia en memoria la ruta configurada para un archivo de intercambio.
 * Entradas: indice_archivo selecciona una entrada de GG_archivos; ruta_out debe
 * apuntar a un char* de salida. Inicializa *ruta_out a NULL; devuelve 0 si asigna
 * la ruta, -3 para índice/salida inválidos y -1 ante fallo de memoria.
 * Entrada de ejemplo: indice_archivo=1, ruta_out inicialmente NULL.
 * Salida de ejemplo: retorna 0 y ruta_out pasa de NULL a una copia de
 * GG_archivos[1].ruta, que el llamador debe liberar.
 */
static int construir_ruta_transferencia(int indice_archivo, char **ruta_out)
{
	if(ruta_out == NULL) return -3;
	*ruta_out = NULL;
	size_t cantidad_archivos = sizeof(GG_archivos) / sizeof(GG_archivos[0]) - 1;
	if(indice_archivo < 0 || (size_t)indice_archivo >= cantidad_archivos ||
		GG_archivos[indice_archivo].ruta == NULL)
		return -3;
	*ruta_out = variable_string("%s", GG_archivos[indice_archivo].ruta);
	return *ruta_out != NULL ? 0 : -1;
}

/**
 * Valida que una línea de intercambio esté dirigida a este programa y extrae su comando.
 * Entradas: linea_transferencia contiene destino/origen y campos separados por los
 * marcadores configurados; comando_out es salida obligatoria. Se inicializa a NULL.
 * Devuelve 0 si extrae, -2 si la línea está vacía/mal formada o va a otro destino,
 * -3 si falta el puntero de salida y -1 si falla la memoria.
 * Entrada de ejemplo: "QU1R30N_SISTEMA_DEFINITIVO■otro┴listar┴info", con
 * comando_out inicialmente NULL.
 * Salida de ejemplo: retorna 0 y comando_out pasa de NULL a "listar".
 */
static int extraer_comando_de_linea_transferencia(
	const char *linea_transferencia, char **comando_out)
{
	if(comando_out == NULL) return -3;
	*comando_out = NULL;
	if(linea_transferencia == NULL || linea_transferencia[0] == '\0') return -2;

	char **partes = NULL;
	int cantidad = split_con_numero_de_celdas(
		linea_transferencia, GG_caracter_para_transferencia_entre_archivos[0], &partes);
	if(cantidad < 3 || partes == NULL)
	{
		free_split_con_numero_de_celdas(partes);
		return -2;
	}

	char **destino_origen = NULL;
	int cantidad_destino_origen = split_con_numero_de_celdas(
		partes[0], GG_caracter_para_transferencia_entre_archivos[1], &destino_origen);
	int es_para_este_programa = cantidad_destino_origen >= 1 &&
		destino_origen != NULL && destino_origen[0] != NULL &&
		strcmp(destino_origen[0], GG_id_programa) == 0;
	free_split_con_numero_de_celdas(destino_origen);
	if(!es_para_este_programa || partes[1] == NULL || partes[1][0] == '\0')
	{
		free_split_con_numero_de_celdas(partes);
		return -2;
	}

	*comando_out = variable_string("%s", partes[1]);
	free_split_con_numero_de_celdas(partes);
	return *comando_out != NULL ? 0 : -1;
}

/**
 * Elimina la primera línea exactamente igual a la indicada y vuelve a guardar el archivo.
 * Entradas: ruta identifica el archivo y linea_a_quitar el texto completo a retirar.
 * Devuelve el estado de guardado, -2 si no se pudo leer o no había coincidencia,
 * -3 por argumentos nulos y -1 por fallo de memoria.
 * Entrada de ejemplo: archivo "entrada" con líneas "A" y "B"; quitar "A".
 * Salida de ejemplo: retorna 0 y el archivo queda con la línea "B".
 */
static int quitar_linea_exacta_del_archivo(const char *ruta, const char *linea_a_quitar)
{
	if(ruta == NULL || linea_a_quitar == NULL) return -3;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL) return -2;

	size_t elementos = cantidad > 0 ? (size_t)cantidad : 1;
	if(elementos > (size_t)-1 / sizeof(char *))
	{
		free_lineas(lineas, cantidad);
		return -1;
	}
	char **resultado = sistema_memoria_reservar(elementos * sizeof(*resultado), NULL, 0);
	if(resultado == NULL)
	{
		free_lineas(lineas, cantidad);
		return -1;
	}

	int conservadas = 0;
	int eliminado = 0;
	for(int i = 0; i < cantidad; ++i)
	{
		if(!eliminado && lineas[i] != NULL && strcmp(lineas[i], linea_a_quitar) == 0)
		{
			eliminado = 1;
			continue;
		}
		resultado[conservadas] = variable_string("%s", lineas[i] ? lineas[i] : "");
		if(resultado[conservadas] == NULL)
		{
			free_lineas(resultado, conservadas);
			free_lineas(lineas, cantidad);
			return -1;
		}
		++conservadas;
	}

	int estado = eliminado ? gg_guardar_archivo(ruta, resultado, conservadas) : -2;
	free_lineas(resultado, conservadas);
	free_lineas(lineas, cantidad);
	return estado;
}

/**
 * Escribe una respuesta en el archivo de salida de intercambio.
 * Entradas: info es el contenido principal; programa_enviar es el destinatario
 * (si es NULL/vacío usa "NEXOPORTALARCANO"); info_espejo agrega el campo espejo.
 * Efectos: agrega una fila con destinatario, ID local, info y espejo; no retorna
 * estado y omite la escritura si ambos textos están vacíos.
 * Entrada de ejemplo: respuesta("ok","PANEL","eco").
 * Salida de ejemplo: el archivo de salida recibe la fila
 * "PANEL■QU1R30N_SISTEMA_DEFINITIVO┴ok┴eco" (si creación/escritura tiene éxito).
 */
void respuesta(const char *info, const char *programa_enviar, const char *info_espejo)
{
	const char *programa = programa_enviar != NULL && programa_enviar[0] != '\0'
		? programa_enviar : "NEXOPORTALARCANO";
	const char *texto_info = info != NULL ? info : "";
	const char *texto_espejo = info_espejo != NULL ? info_espejo : "";
	if(texto_info[0] == '\0' && texto_espejo[0] == '\0') return;

	char *ruta = NULL;
	if(construir_ruta_transferencia(2, &ruta) != 0)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[respuesta] no se pudo construir la ruta del archivo de salida\n");
		return;
	}
	if(crearArchivo(ruta, GG_archivos[2].columnas) < 0)
	{
		imprimirMensaje_para_depurar(NULL, 0,
			"[respuesta] no se pudo crear el archivo: %s\n", ruta);
		sistema_memoria_liberar(ruta, NULL, 0);
		return;
	}

	char *linea = variable_string("%s%s%s%s%s%s%s",
		programa,
		GG_caracter_para_transferencia_entre_archivos[1],
		GG_id_programa,
		GG_caracter_para_transferencia_entre_archivos[0],
		texto_info,
		GG_caracter_para_transferencia_entre_archivos[0],
		texto_espejo);
	if(linea == NULL || gg_agregar_fila(ruta, linea) != 0)
		imprimirMensaje_para_depurar(NULL, 0,
			"[respuesta] no se pudo escribir en el archivo: %s\n", ruta);
	sistema_memoria_liberar(linea, NULL, 0);
	sistema_memoria_liberar(ruta, NULL, 0);
}

/**
 * Lee el archivo de entrada y reúne las líneas de intercambio dirigidas al programa.
 * Entradas: retorno_comando y retorno_numero_lineas son salidas obligatorias; se
 * inicializan a NULL y 0. Devuelve 1 con al menos una línea válida, 0 si no hay
 * comandos válidos y -1 ante error. El llamador libera el arreglo y sus cadenas.
 * Entrada de ejemplo: puntero de arreglo NULL y contador 0; el archivo contiene
 * una línea válida "QU1R30N_SISTEMA_DEFINITIVO■otro┴listar┴info".
 * Salida de ejemplo: retorna 1; arreglo pasa de NULL a un arreglo cuyo elemento
 * 0 es esa línea original, y contador de 0 a 1.
 */
int monitoreo_archivo_entrada(char ***retorno_comando, int *retorno_numero_lineas)
{
	if(retorno_comando == NULL || retorno_numero_lineas == NULL) return -1;
	*retorno_comando = NULL;
	*retorno_numero_lineas = 0;
	char *ruta = NULL;
	if(construir_ruta_transferencia(1, &ruta) != 0) return -1;
	int cantidad_lineas = 0;
	char **lineas = leer_archivo(ruta, &cantidad_lineas);
	sistema_memoria_liberar(ruta, NULL, 0);
	if(lineas == NULL) return -1;

	char **comandos = NULL;
	int cantidad_comandos = 0;
	for(int i = GG_indice_donde_comensar; i < cantidad_lineas; ++i)
	{
		if(lineas[i] == NULL || lineas[i][0] == '\0') continue;
		char *comando = NULL;
		int estado = extraer_comando_de_linea_transferencia(lineas[i], &comando);
		if(estado == -2) continue;
		if(estado != 0)
		{
			free_lineas(comandos, cantidad_comandos);
			free_lineas(lineas, cantidad_lineas);
			return -1;
		}
		sistema_memoria_liberar(comando, NULL, 0);

		if(cantidad_comandos == INT_MAX ||
			(size_t)(cantidad_comandos + 1) > (size_t)-1 / sizeof(*comandos))
		{
			free_lineas(comandos, cantidad_comandos);
			free_lineas(lineas, cantidad_lineas);
			return -1;
		}
		char **temporal = sistema_memoria_redimensionar(
			comandos, (size_t)(cantidad_comandos + 1) * sizeof(*comandos), NULL, 0);
		if(temporal == NULL)
		{
			free_lineas(comandos, cantidad_comandos);
			free_lineas(lineas, cantidad_lineas);
			return -1;
		}
		comandos = temporal;
		comandos[cantidad_comandos] = variable_string("%s", lineas[i]);
		if(comandos[cantidad_comandos] == NULL)
		{
			free_lineas(comandos, cantidad_comandos);
			free_lineas(lineas, cantidad_lineas);
			return -1;
		}
		++cantidad_comandos;
	}
	free_lineas(lineas, cantidad_lineas);
	if(cantidad_comandos == 0)
	{
		sistema_memoria_liberar(comandos, NULL, 0);
		return 0;
	}
	*retorno_comando = comandos;
	*retorno_numero_lineas = cantidad_comandos;
	return 1;
}

/**
 * Inicializa las salidas y delega la lectura de comandos al monitor de entrada.
 * Entradas: retorno_comando y retorno_numero_lineas son salidas obligatorias.
 * Devuelve exactamente el estado del monitor: 1 con datos, 0 sin datos o -1 en error.
 * Entrada de ejemplo: arreglo=NULL, contador=0 y una línea válida en el archivo.
 * Salida de ejemplo: retorna 1; arreglo pasa de NULL al arreglo de líneas y contador
 * de 0 a la cantidad encontrada (por ejemplo, 1).
 */
int datos_recibidos_a_procesar_y_borrar(
	char ***retorno_comando, int *retorno_numero_lineas)
{
	if(retorno_comando == NULL || retorno_numero_lineas == NULL) return -1;
	*retorno_comando = NULL;
	*retorno_numero_lineas = 0;
	return monitoreo_archivo_entrada(retorno_comando, retorno_numero_lineas);
}

/**
 * Quita del archivo de entrada un comando procesado y archiva los fallidos.
 * Entradas: linea_original es la línea exacta recibida; estado_ejecucion negativo
 * significa fallo y provoca además agregarla al archivo de errores.
 * Devuelve el resultado de quitar la línea, -3 por línea nula/vacía o -1 ante error.
 * Entrada de ejemplo: línea="destino┴listar┴x", estado_ejecucion=0, y línea presente.
 * Salida de ejemplo: retorna 0 y la línea deja de estar en el archivo de entrada;
 * el archivo de errores no se modifica.
 */
int finalizar_comando_procesado(const char *linea_original, int estado_ejecucion)
{
	if(linea_original == NULL || linea_original[0] == '\0') return -3;
	char *ruta_entrada = NULL;
	if(construir_ruta_transferencia(1, &ruta_entrada) != 0) return -1;

	if(estado_ejecucion < 0)
	{
		char *ruta_errores = NULL;
		if(construir_ruta_transferencia(3, &ruta_errores) != 0)
		{
			sistema_memoria_liberar(ruta_entrada, NULL, 0);
			return -1;
		}
		int creado = crearArchivo(ruta_errores, GG_archivos[3].columnas);
		int guardado = creado < 0 ? -1 : gg_agregar_fila(ruta_errores, linea_original);
		sistema_memoria_liberar(ruta_errores, NULL, 0);
		if(guardado != 0)
		{
			sistema_memoria_liberar(ruta_entrada, NULL, 0);
			return -1;
		}
	}

	int resultado = quitar_linea_exacta_del_archivo(ruta_entrada, linea_original);
	sistema_memoria_liberar(ruta_entrada, NULL, 0);
	return resultado;
}

/**
 * Reemplaza el ID local en la fila de control por el siguiente usuario disponible.
 * Entradas: usuarios es el arreglo con IDs/destinos y total_usuarios su tamaño.
 * Devuelve 1 si actualiza el archivo, 0 si no aplica/no hay destino, o el error
 * de guardado (incluido -1 al fallar operaciones previas).
 * Entrada de ejemplo: usuarios[0]=GG_id_programa, usuarios[1]="PANEL",
 * total_usuarios=2, y archivo de entrada con fila de control iniciada en "0".
 * Salida de ejemplo: retorna 1 y el primer campo de la fila de control pasa a "PANEL".
 */
int cambiar_id_programa_al_siguiente(char **usuarios, int total_usuarios)
{
	if(usuarios == NULL || total_usuarios <= GG_indice_donde_comensar) return 0;
	if(usuarios[0] == NULL || strcmp(usuarios[0], GG_id_programa) != 0) return 0;

	int indice_nuevo = GG_indice_donde_comensar;
	for(int i = GG_indice_donde_comensar; i < total_usuarios; ++i)
	{
		if(usuarios[i] != NULL && strcmp(usuarios[i], GG_id_programa) == 0)
		{
			indice_nuevo = i >= total_usuarios - 1
				? GG_indice_donde_comensar : i + 1;
			break;
		}
	}
	if(usuarios[indice_nuevo] == NULL || usuarios[indice_nuevo][0] == '\0') return 0;

	char *ruta = NULL;
	if(construir_ruta_transferencia(1, &ruta) != 0) return -1;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL)
	{
		sistema_memoria_liberar(ruta, NULL, 0);
		return -1;
	}
	int encontrado = 0;
	for(int i = 0; i < cantidad && !encontrado; ++i)
	{
		char **campos = NULL;
		int total_campos = split_con_numero_de_celdas(
			lineas[i], GG_caracter_separacion[0], &campos);
		if(total_campos > 0 && campos != NULL && strcmp(campos[0], "0") == 0)
		{
			char *nuevo = variable_string("%s", usuarios[indice_nuevo]);
			if(nuevo == NULL)
			{
				free_split_con_numero_de_celdas(campos);
				free_lineas(lineas, cantidad);
				sistema_memoria_liberar(ruta, NULL, 0);
				return -1;
			}
			sistema_memoria_liberar(campos[0], NULL, 0);
			campos[0] = nuevo;
			char *actualizada = gg_reconstruir_fila_info(campos, total_campos);
			free_split_con_numero_de_celdas(campos);
			if(actualizada == NULL)
			{
				free_lineas(lineas, cantidad);
				sistema_memoria_liberar(ruta, NULL, 0);
				return -1;
			}
			sistema_memoria_liberar(lineas[i], NULL, 0);
			lineas[i] = actualizada;
			encontrado = 1;
		}
		else
		{
			free_split_con_numero_de_celdas(campos);
		}
	}
	int guardado = encontrado ? gg_guardar_archivo(ruta, lineas, cantidad) : 0;
	free_lineas(lineas, cantidad);
	sistema_memoria_liberar(ruta, NULL, 0);
	return guardado == 0 && encontrado ? 1 : guardado;
}

/**
 * Elimina del archivo de entrada la línea que contiene el ID local del programa.
 * Entradas: ninguna; busca el ID configurado en los campos de cada línea.
 * Devuelve el estado de borrar la línea, o -1 por fallo y -2 si no aparece.
 * Entrada de ejemplo: archivo con una línea que contiene GG_id_programa.
 * Salida de ejemplo: retorna el resultado de aplicarOperacionLinea; ante éxito es 0.
 */
int quitar_id_prog_del_archivo(void)
{
	char *ruta = NULL;
	if(construir_ruta_transferencia(1, &ruta) != 0) return -1;
	int cantidad = 0;
	char **lineas = leer_archivo(ruta, &cantidad);
	if(lineas == NULL)
	{
		sistema_memoria_liberar(ruta, NULL, 0);
		return -1;
	}
	const char *separadores[] = {
		GG_caracter_separacion[0],
		GG_caracter_para_transferencia_entre_archivos[0],
		GG_caracter_para_transferencia_entre_archivos[1]
	};
	int resultado = 0;
	int encontrado = 0;
	for(int i = 0; i < cantidad && !encontrado; ++i)
	{
		for(size_t s = 0; s < sizeof(separadores) / sizeof(separadores[0]); ++s)
		{
			char **campos = NULL;
			int n = split_con_numero_de_celdas(lineas[i], separadores[s], &campos);
			for(int j = 0; j < n && campos != NULL; ++j)
			{
				if(campos[j] != NULL && strcmp(campos[j], GG_id_programa) == 0)
				{
					resultado = aplicarOperacionLinea(ruta, i + 1, 1, NULL, 0, NULL, 0);
					encontrado = 1;
					break;
				}
			}
			free_split_con_numero_de_celdas(campos);
			if(encontrado) break;
		}
	}
	free_lineas(lineas, cantidad);
	sistema_memoria_liberar(ruta, NULL, 0);
	return encontrado ? resultado : -2;
}

/**
 * Calcula tres índices de archivo: anterior, actual y siguiente con vuelta de ciclo.
 * Entradas: posicion_bandera determina el índice inicial al dar la vuelta;
 * numero_actual_posicion es el índice actual; arr_devolver es salida de 3 enteros.
 * Devuelve 0 y escribe el arreglo, o -1 si la salida es NULL o hay desbordamiento.
 * Entrada de ejemplo: bandera=0, actual=1, arr_devolver={-1,-1,-1}.
 * Salida de ejemplo: retorna 0 y el arreglo pasa a { 2, 1, 0 } según las fórmulas
 * actuales (atras=1-3, adelante se reemplaza por bandera).
 */
int checar_numero_de_direccion_de_archivo_atras_actual_adelante(
	int posicion_bandera, int numero_actual_posicion, int arr_devolver[3])
{
	if(arr_devolver == NULL) return -1;
	long adelante = (long)numero_actual_posicion + 3;
	long atras = (long)numero_actual_posicion - 3;
	if(adelante >= 3) adelante = posicion_bandera;
	if(atras < 0) atras = 2L + posicion_bandera;
	if(adelante < INT_MIN || adelante > INT_MAX ||
		atras < INT_MIN || atras > INT_MAX) return -1;
	arr_devolver[0] = (int)atras;
	arr_devolver[1] = numero_actual_posicion;
	arr_devolver[2] = (int)adelante;
	return 0;
}

#else

/**
 * Stub no operativo del monitor de entrada en la plataforma sin sistema de archivos.
 * Entradas: punteros opcionales de arreglo y cantidad; si no son NULL se limpian.
 * Devuelve siempre 0 y no lee dispositivos.
 * Entrada de ejemplo: arreglo apuntando a un valor previo y contador=4.
 * Salida de ejemplo: retorna 0, arreglo pasa a NULL y contador a 0.
 */
int monitoreo_archivo_entrada(char ***retorno_comando, int *retorno_numero_lineas)
{
	if(retorno_comando != NULL) *retorno_comando = NULL;
	if(retorno_numero_lineas != NULL) *retorno_numero_lineas = 0;
	return 0;
}

/**
 * Stub de salida de mensajes para una plataforma sin backend de intercambio.
 * Entradas: info, programa_enviar e info_espejo se aceptan pero se ignoran.
 * Efectos/salida: no escribe nada y retorna void.
 * Entrada de ejemplo: respuesta("ok","PANEL","eco"). Salida de ejemplo: void;
 * ningún archivo ni salida se modifica.
 */
void respuesta(const char *info, const char *programa_enviar, const char *info_espejo)
{
	(void)info;
	(void)programa_enviar;
	(void)info_espejo;
}

/**
 * Stub que informa que no hay comandos entrantes en esta plataforma.
 * Entradas: salidas opcionales retorno_comando y retorno_numero_lineas se limpian.
 * Entrada de ejemplo: arreglo no NULL y contador=3.
 * Salida de ejemplo: retorna 0; arreglo pasa a NULL y contador a 0.
 */
int datos_recibidos_a_procesar_y_borrar(char ***retorno_comando, int *retorno_numero_lineas)
{
	if(retorno_comando != NULL) *retorno_comando = NULL;
	if(retorno_numero_lineas != NULL) *retorno_numero_lineas = 0;
	return 0;
}

/**
 * Stub de finalización: línea y estado se ignoran porque no hay almacenamiento.
 * Entrada de ejemplo: finalizar_comando_procesado("comando", -1).
 * Salida de ejemplo: 0, sin archivar ni borrar nada.
 */
int finalizar_comando_procesado(const char *linea_original, int estado_ejecucion)
{
	(void)linea_original;
	(void)estado_ejecucion;
	return 0;
}

/**
 * Stub de rotación del ID de programa; ignora la lista y su cantidad.
 * Entrada de ejemplo: cambiar_id_programa_al_siguiente(NULL, 0).
 * Salida de ejemplo: 0, sin modificar archivos.
 */
int cambiar_id_programa_al_siguiente(char **usuarios, int total_usuarios)
{
	(void)usuarios;
	(void)total_usuarios;
	return 0;
}

/**
 * Stub de eliminación del ID local; no consulta archivos ni cambia estado.
 * Entrada de ejemplo: quitar_id_prog_del_archivo(). Salida de ejemplo: 0.
 */
int quitar_id_prog_del_archivo(void) { return 0; }

/**
 * Stub de cálculo de índices de archivos; conserva el índice actual y pone
 * los extremos a cero. Entrada: posición bandera (ignorada), posición actual y
 * arreglo de tres enteros de salida. Devuelve -1 si el arreglo es NULL; si no,
 * devuelve 0.
 * Entrada de ejemplo: bandera=2, actual=1, arreglo={-1,-1,-1}.
 * Salida de ejemplo: retorna 0 y el arreglo pasa a {0,1,0}.
 */
int checar_numero_de_direccion_de_archivo_atras_actual_adelante(
	int posicion_bandera, int numero_actual_posicion, int arr_devolver[3])
{
	(void)posicion_bandera;
	if(arr_devolver == NULL) return -1;
	arr_devolver[0] = 0;
	arr_devolver[1] = numero_actual_posicion;
	arr_devolver[2] = 0;
	return 0;
}

#endif

 /*
 * Comprueba si alguno de los tres archivos de mensajes contiene al menos un byte.
 * Devuelve código 1 si encuentra contenido, 0 si todos están vacíos/no existen,
 * o -1 si un archivo abierto da error de lectura o cierre.
 */
/**
 * Comprueba si hay al menos un byte en alguno de los tres archivos de mensajes.
 * Entradas: error_o_datos_a_retornar acumula detalles de error y nivel_de_profundidad
 * determina la profundidad de la respuesta. Devuelve una cadena de resultado nueva:
 * código 1 si encuentra contenido, 0 si no hay mensajes o -1 ante error de lectura/
 * cierre; los archivos se abren y cierran, pero no se modifican.
 * Entrada de ejemplo: error=NULL, nivel=0 y los tres archivos vacíos o inexistentes.
 * Salida de ejemplo: devuelve "°0" (el nivel se incrementa antes de crear el resultado).
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
/**
 * Ejecuta una demostración CRUD en el archivo temporal de pruebas y lo limpia al final.
 * Entradas: error_o_datos_a_retornar recibe errores; nivel_de_profundidad indica
 * profundidad de resultado. Efectos: crea/edita/borra datos temporales e imprime
 * el contenido de prueba; devuelve una cadena de resultado con éxito o error.
 * Entrada de ejemplo: error=NULL, nivel=0, ruta temporal inexistente y operaciones
 * de archivo disponibles.
 * Salida de ejemplo: devuelve "°1" cuando todas las operaciones y la limpieza tienen
 * éxito; el archivo temporal se elimina antes de retornar.
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
/**
 * Agrega un mensaje no vacío al archivo común de mensajes.
 * Entradas: mensaje es el texto; error_o_datos_a_retornar acumula fallos y
 * nivel_de_profundidad fija la profundidad de la respuesta. Efectos: agrega una
 * línea y cierra el archivo; devuelve una cadena de estado asignada.
 * Entrada de ejemplo: mandar_mensje_a_todos("Reunión a las 10",NULL,0),
 * con archivo de mensajes disponible.
 * Salida de ejemplo: devuelve "°1" y agrega "Reunión a las 10\n" al archivo.
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
/**
 * Agrega un mensaje dirigido a una lista de contactos con un ID asociado.
 * Entradas: mensaje y contactos deben ser no vacíos; id_opcional se escribe entre
 * corchetes; error_o_datos_a_retornar y nivel registran contexto. Efectos: anexa
 * una línea al archivo de contactos y devuelve una cadena de estado.
 * Entrada de ejemplo: mensaje="Hola", contactos="Ana,Luis", id_opcional=7,
 * error=NULL, nivel=0, con escritura correcta.
 * Salida de ejemplo: devuelve "°1" y anexa "[7] Ana,Luis -> Hola\n".
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
/**
 * Registra una pregunta de respuesta prioritaria, el aviso de aceptación y la respuesta final.
 * Entradas: mensaje_pregunta, mensaje_de_que_ya_alguien_lo_acepto y
 * menaje_respuesta_al_quien_lo_logro deben ser no vacíos; error/nivel son contexto.
 * Efectos: añade una línea al archivo correspondiente; devuelve una cadena de estado.
 * Entrada de ejemplo: "¿Quién?", "Yo voy", "Gracias", error=NULL, nivel=0,
 * con el archivo disponible para escritura.
 * Salida de ejemplo: devuelve "°1" y anexa "¿Quién? | Yo voy | Gracias\n".
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
/**
 * Reserva un bloque dinámico con malloc en Windows/Linux.
 * Entradas: cantidad es el tamaño en bytes; error_o_datos_a_retornar y
 * nivel_de_profundidad forman parte de la interfaz común (el nivel se ignora).
 * Salida: dirección del bloque o NULL; el llamador debe liberarlo.
 * Entrada de ejemplo: sistema_memoria_reservar(16,NULL,0).
 * Salida de ejemplo: puntero a un bloque de 16 bytes, o NULL si malloc no puede reservarlo.
 */
 static void * sistema_memoria_reservar(size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	return malloc(cantidad);
 }
/* Cambia el tamaño de un bloque existente; conserva su contenido si tiene éxito. */
/**
 * Redimensiona un bloque mediante realloc en Windows/Linux.
 * Entradas: memoria es el bloque original (o NULL), cantidad el nuevo tamaño;
 * los otros argumentos son contexto común y el nivel se ignora. Salida: nuevo
 * puntero o NULL; si tiene éxito conserva el contenido previo hasta el menor tamaño.
 * Entrada de ejemplo: memoria apunta a 8 bytes con "abc"; sistema_memoria_redimensionar(memoria,16,NULL,0).
 * Salida de ejemplo: devuelve un puntero al bloque de 16 bytes, cuyo contenido empieza por "abc";
 * el puntero puede diferir del original.
 */
 static void * sistema_memoria_redimensionar(void * memoria, size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
 {
	(void)nivel_de_profundidad;
 	return realloc(memoria, cantidad);
 }
/* Libera un bloque obtenido con reservar/redimensionar; NULL es seguro en free(). */
/**
 * Libera un bloque obtenido por la capa de memoria usando free.
 * Entradas: memoria es el bloque o NULL; los argumentos de error/profundidad son
 * compatibles con la interfaz y la profundidad se ignora. Efecto: el bloque deja
 * de estar reservado; no retorna valor.
 * Entrada de ejemplo: sistema_memoria_liberar(p,NULL,0), con p reservado.
 * Salida de ejemplo: void; p queda liberado y no debe reutilizarse.
 */
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
/**
 * Stub de reserva para el backend embebido aún sin pool de memoria.
 * Entradas: cantidad es el tamaño solicitado; error y profundidad son contexto
 * (la profundidad se ignora). Salida: siempre NULL, sin efecto.
 * Entrada de ejemplo: sistema_memoria_reservar(16,NULL,0). Salida de ejemplo: NULL.
 */
static void * sistema_memoria_reservar(size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)cantidad;
	(void)nivel_de_profundidad;
	return NULL;
}

/* Stub de redimensionamiento; devuelve NULL hasta integrar el allocator embebido. */
/**
 * Stub de redimensionamiento embebido; memoria y cantidad se ignoran.
 * Entrada de ejemplo: sistema_memoria_redimensionar(p,32,NULL,0).
 * Salida de ejemplo: NULL, sin modificar ni mover el bloque señalado por p.
 */
static void * sistema_memoria_redimensionar(void *memoria, size_t cantidad, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)memoria;
	(void)cantidad;
	(void)nivel_de_profundidad;
	return NULL;
}

/* Stub de liberación: no hay bloque asignado mientras reservar() falle. */
/**
 * Stub de liberación embebida; acepta un bloque y contexto, pero no libera memoria.
 * Entrada de ejemplo: sistema_memoria_liberar(NULL,NULL,0). Salida de ejemplo: void.
 */
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
/**
 * Abre un archivo local con el modo estándar solicitado y crea un manejador opaco.
 * Entradas: ruta y modo ("r", "a", "w", etc.); error y profundidad se pasan a la
 * capa de memoria (el nivel no altera fopen). Salida: manejador o NULL; ante fallo
 * de reserva cierra el flujo ya abierto.
 * Entrada de ejemplo: sistema_archivo_abrir("datos.txt","r",NULL,0), con archivo existente.
 * Salida de ejemplo: devuelve un manejador no NULL listo para leer; el llamador lo cierra.
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
/**
 * Cierra el flujo local y libera su manejador.
 * Entradas: archivo es el manejador abierto; error/profundidad son contexto de memoria.
 * Devuelve el resultado de fclose o -1 si archivo es NULL; siempre libera un manejador válido.
 * Entrada de ejemplo: archivo abierto correctamente en lectura.
 * Salida de ejemplo: retorna 0 si fclose tiene éxito y el manejador queda liberado.
 */
static int sistema_archivo_cerrar(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	if(archivo == NULL) return -1;

 	int resultado = fclose(archivo->flujo);
 	sistema_memoria_liberar(archivo,error_o_datos_a_retornar,  nivel_de_profundidad);
 	return resultado;
}

/* Elimina una ruta; por ejemplo, borra el temporal luego de una edición. */
/**
 * Elimina el archivo indicado en el backend de escritorio.
 * Entradas: ruta identifica el archivo; error y profundidad son contexto común.
 * Salida: 0 si remove lo elimina, -1 por ruta NULL o error del sistema.
 * Entrada de ejemplo: sistema_archivo_eliminar("temporal.txt",NULL,0), con archivo existente.
 * Salida de ejemplo: 0 y el archivo deja de existir.
 */
static int sistema_archivo_eliminar(const char *ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (ruta == NULL) ? -1 : remove(ruta);
}

/* Cambia el nombre de origen a destino; 0 significa que el backend lo logró. */
/**
 * Renombra un archivo local.
 * Entradas: origen es la ruta actual, destino la nueva; error y profundidad se ignoran.
 * Salida: resultado de rename (0 cuando se completa, -1 ante argumentos inválidos/error).
 * Entrada de ejemplo: origen="a.txt" existente y destino="b.txt".
 * Salida de ejemplo: retorna 0 y el archivo queda con nombre "b.txt".
 */
static int sistema_archivo_renombrar(const char *origen, const char *destino, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (origen == NULL || destino == NULL) ? -1 : rename(origen, destino);
}

/*
 * Lee un byte como int o devuelve SISTEMA_ARCHIVO_FIN_LECTURA al alcanzar EOF.
 * Esta conversión evita exponer el valor EOF al código común.
 */
/**
 * Lee un carácter del flujo, normalizando CR/CRLF a '\n'.
 * Entradas: archivo es el manejador; error/profundidad forman parte de la interfaz.
 * Salida: byte como int o SISTEMA_ARCHIVO_FIN_LECTURA si no hay flujo/alcanzó EOF.
 * Entrada de ejemplo: flujo con "A" al inicio y llamada con ese manejador.
 * Salida de ejemplo: retorna 'A'; al leer después del final retorna el sentinela EOF propio.
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
/**
 * Escribe un byte en un flujo local abierto.
 * Entradas: archivo es el manejador y caracter el valor a escribir; error/profundidad
 * son contexto común. Devuelve 0 si fputc tiene éxito y -1 ante flujo inválido/fallo.
 * Entrada de ejemplo: archivo abierto en modo "w", caracter='X'.
 * Salida de ejemplo: retorna 0 y se escribe el byte X.
 */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(archivo == NULL || archivo->flujo == NULL) return -1;
	return (fputc(caracter, archivo->flujo) == EOF) ? -1 : 0;
}

/* Consulta el indicador de error del flujo; 0=sin error, distinto de 0=error. */
/**
 * Consulta el indicador de error del flujo.
 * Entrada: archivo es el manejador; error/profundidad son contexto común.
 * Devuelve 0 sin error, distinto de 0 si el flujo es inválido o ferror está activo.
 * Entrada de ejemplo: manejador recién abierto, sin operaciones fallidas.
 * Salida de ejemplo: 0.
 */
static int sistema_archivo_hay_error(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return (archivo == NULL || archivo->flujo == NULL) ? 1 : ferror(archivo->flujo);
}

/* Escribe todos los bytes de una cadena (sin su '\0'); 0=éxito, -1=fallo. */
/**
 * Escribe todos los bytes de una cadena en el flujo.
 * Entradas: archivo es el manejador y texto la cadena; error/profundidad son contexto.
 * Devuelve 0 si se escribió toda la cadena o -1 por argumento/flujo inválido o fallo.
 * Entrada de ejemplo: flujo abierto en escritura y texto="Hola".
 * Salida de ejemplo: retorna 0 y el flujo recibe los cuatro bytes "Hola".
 */
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
/**
 * Stub embebido de apertura: no existe aún correspondencia entre ruta y dispositivo.
 * Entradas: ruta, modo y contexto común; se ignoran.
 * Entrada de ejemplo: sistema_archivo_abrir("datos.txt","r",NULL,0).
 * Salida de ejemplo: NULL y ningún recurso abierto.
 */
static SistemaArchivo *sistema_archivo_abrir(const char *ruta, const char *modo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)ruta;
	(void)modo;
	(void)nivel_de_profundidad;
	return NULL;
}

/* No hay manejador real que cerrar en el stub actual. */
/**
 * Stub embebido de cierre; archivo y contexto se ignoran porque no hay manejador real.
 * Entrada de ejemplo: sistema_archivo_cerrar(NULL,NULL,0). Salida de ejemplo: -1.
 */
static int sistema_archivo_cerrar(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return -1;
}

/* El borrado requiere la política y soporte del medio que se seleccione. */
/**
 * Stub embebido de eliminación; ruta y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_eliminar("datos.txt",NULL,0).
 * Salida de ejemplo: -1; no se elimina ningún archivo.
 */
static int sistema_archivo_eliminar(const char *ruta, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)ruta;
	(void)nivel_de_profundidad;
	return -1;
}

/* El renombrado debe implementarse según lo que permita el almacenamiento. */
/**
 * Stub embebido de renombrado; origen, destino y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_renombrar("a","b",NULL,0).
 * Salida de ejemplo: -1; no se renombra nada.
 */
static int sistema_archivo_renombrar(const char *origen, const char *destino, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)origen;
	(void)destino;
	(void)nivel_de_profundidad;
	return -1;
}

/* Indica fin de entrada porque el stub no está conectado a un lector físico. */
/**
 * Stub embebido de lectura de carácter: no hay periférico de entrada conectado.
 * Entrada: manejador y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_leer_caracter(NULL,NULL,0).
 * Salida de ejemplo: SISTEMA_ARCHIVO_FIN_LECTURA.
 */
static int sistema_archivo_leer_caracter(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* El stub no acepta escrituras hasta contar con backend real. */
/**
 * Stub embebido de escritura de carácter; manejador, byte y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_escribir_caracter(NULL,'X',NULL,0).
 * Salida de ejemplo: -1, sin escribir.
 */
static int sistema_archivo_escribir_caracter(SistemaArchivo *archivo, int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)caracter;
	(void)nivel_de_profundidad;
	return -1;
}

/* El stub no persiste texto hasta contar con backend real. */
/**
 * Stub embebido de escritura de texto; manejador, cadena y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_escribir_texto(NULL,"Hola",NULL,0).
 * Salida de ejemplo: -1; no se escribe texto.
 */
static int sistema_archivo_escribir_texto(SistemaArchivo *archivo, const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)texto;
	(void)nivel_de_profundidad;
	return -1;
}

/* Mientras el backend no exista, toda consulta de error debe ser conservadora. */
/**
 * Reporta error mientras el backend embebido siga sin implementación.
 * Entrada: manejador y contexto se ignoran.
 * Entrada de ejemplo: sistema_archivo_hay_error(NULL,NULL,0). Salida de ejemplo: 1.
 */
static int sistema_archivo_hay_error(SistemaArchivo *archivo, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)archivo;
	(void)nivel_de_profundidad;
	return 1;
}
#endif

#if defined(PLATAFORMA_WINDOWS) || defined(PLATAFORMA_LINUX)
/* Backends de consola de escritorio; aquí, y solo aquí, se usa stdio. */
/**
 * Lee un carácter de la consola en Windows/Linux y normaliza CR/CRLF a '\n'.
 * Entradas: error_o_datos_a_retornar y nivel_de_profundidad son contexto común
 * (el nivel se ignora). Devuelve el byte leído o SISTEMA_ARCHIVO_FIN_LECTURA al EOF.
 * Entrada de ejemplo: stdin entrega 'A'. Salida de ejemplo: devuelve 'A' (65).
 */
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
/**
 * Escribe un carácter en stdout.
 * Entradas: caracter es el byte; error/profundidad son contexto común y se ignoran.
 * Devuelve 0 si fputc tiene éxito o -1 si falla.
 * Entrada de ejemplo: sistema_consola_escribir_caracter('X',NULL,0).
 * Salida de ejemplo: devuelve 0 y stdout recibe "X" (sin salto de línea).
 */
static int sistema_consola_escribir_caracter(int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return fputc(caracter, stdout) == EOF ? -1 : 0;
}

/* Escribe una cadena completa y vacía la salida para que los prompts aparezcan. */
/**
 * Escribe una cadena completa en stdout y fuerza el vaciado del búfer.
 * Entradas: texto es la cadena; error/profundidad son contexto común.
 * Devuelve 0 si escritura y fflush tienen éxito; -1 si texto es NULL o hay error.
 * Entrada de ejemplo: sistema_consola_escribir_texto("Hola",NULL,0).
 * Salida de ejemplo: devuelve 0 y stdout recibe "Hola".
 */
static int sistema_consola_escribir_texto(const char *texto, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	if(texto == NULL || fputs(texto, stdout) == EOF || fflush(stdout) != 0) return -1;
	return 0;
}
#elif defined(PLATAFORMA_SEMICONDUCTOR)
/* Reemplazar con lectura del periférico elegido, por ejemplo UART. */
/**
 * Stub embebido de lectura de consola; error/profundidad se ignoran.
 * Entrada de ejemplo: sistema_consola_leer_caracter(NULL,0).
 * Salida de ejemplo: SISTEMA_ARCHIVO_FIN_LECTURA, al no existir periférico conectado.
 */
static int sistema_consola_leer_caracter(char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)nivel_de_profundidad;
	return SISTEMA_ARCHIVO_FIN_LECTURA;
}

/* Stub de salida de byte: no reporta éxito sin un periférico real. */
/**
 * Stub embebido de salida de carácter; caracter y nivel se ignoran.
 * Entrada de ejemplo: sistema_consola_escribir_caracter('X',NULL,0).
 * Salida de ejemplo: -1, sin enviar el byte a un periférico.
 */
static int sistema_consola_escribir_caracter(int caracter, char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	(void)caracter;
	(void)nivel_de_profundidad;
	return -1;
}

/* Stub de salida de texto; lo implementará el backend de consola PIC16F. */
/**
 * Stub embebido de salida de texto; texto y nivel se ignoran.
 * Entrada de ejemplo: sistema_consola_escribir_texto("Hola",NULL,0).
 * Salida de ejemplo: -1, no se escribe porque aún no hay backend.
 */
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
/**
 * Construye y registra una traza anteponiendo el separador de nivel y el código.
 * Entradas: codigo es el estado; informacion y funcion_llamante se conservan por
 * compatibilidad pero no se incorporan; resultado_anterior es la traza a anexar
 * (NULL equivale a vacío); error_o_datos_a_retornar se pasa al allocator; nivel
 * selecciona el separador. Salida: cadena nueva o NULL si nivel, conversión,
 * reserva o registro del último retorno fallan. El llamador libera la cadena.
 * Entrada de ejemplo: codigo=0, informacion="ok", resultado_anterior="",
 * funcion_llamante="demo", nivel_de_profundidad=0.
 * Salida de ejemplo: devuelve "|0" y actualiza el último retorno a "|0".
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
/**
 * Emite en consola un mensaje con formato printf; PIC limita el texto al buffer.
 * Entradas: error_o_datos_a_retornar y nivel son contexto; format y los argumentos
 * variables definen el texto. Efecto: imprime, sin valor de retorno; format NULL
 * no produce salida.
 * Entrada de ejemplo: imprimirMensaje_para_depurar(NULL,0,"ID=%d\n",7).
 * Salida de ejemplo: stdout recibe "ID=7\n" en Windows/Linux.
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
/**
 * Imprime elementos de un arreglo de cadenas, con prefijo e índices.
 * Entradas: error y nivel son contexto; contenido es el arreglo; texto es prefijo
 * opcional (por defecto "celda"); total>0 fija el número de elementos; con total<=0
 * se requiere terminador NULL. Efecto: imprime y no modifica el arreglo.
 * Entrada de ejemplo: contenido={"uno","dos",NULL}, texto="parte", total=2.
 * Salida de ejemplo: stdout muestra "\nparte[0]: uno\nparte[1]: dos".
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
/**
 * Suspende la ejecución el intervalo indicado usando el mecanismo de la plataforma.
 * Entrada: ms es la duración en milisegundos. Efecto: pausa el proceso; no retorna dato.
 * Entrada de ejemplo: delay_ms(1). Salida de ejemplo: void; en Windows/Linux/PIC
 * espera aproximadamente 1 ms (la precisión depende del sistema operativo/reloj).
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
/**
 * Escribe la fecha/hora actual formateada en buffer; PIC usa una fecha fija simulada.
 * Entradas: buffer debe tener al menos 20 bytes; formato es el patrón strftime en
 * escritorio (en PIC se ignora). Efecto: escribe la fecha terminada en NUL; si
 * buffer es NULL no hace nada y ante formato inválido/escritura fallida lo vacía.
 * Entrada de ejemplo: en PIC, buffer con capacidad >=20 y formato="%Y".
 * Salida de ejemplo: buffer recibe exactamente "01/01/2000 00:00"; en escritorio,
 * una máscara "%Y-%m-%d" produce la fecha local del día de ejecución.
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
/**
 * Ejecuta el menú interactivo de pruebas auxiliares según la opción seleccionada.
 * Entradas: error_o_datos_a_retornar acumula errores de consola; nivel es la
 * profundidad de traza; opcion_seleccionada elige la acción inicial (0 muestra menú).
 * Efectos: puede leer consola, imprimir resultados y crear/eliminar archivos de prueba;
 * devuelve -1 ante fallo de entrada o de una prueba seleccionada, y el estado de
 * la acción/menú en los demás casos.
 * Entrada de ejemplo: opcion_seleccionada=0, el usuario escribe "7" en el menú.
 * Salida de ejemplo: retorna 0 y vuelve al menú superior; imprime el menú y la
 * solicitud de opción antes de leer el "7".
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
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "7. Probar agregar texto a un arreglo dinámico\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "8. Probar almacenamiento dividido\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "9. Volver al menú de pruebas\n");
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0, "Seleccione una opción: ");

			char *entrada = leerLineaConsola(error_o_datos_a_retornar, 1);
			if(entrada == NULL) return -1;
			int entradaValida = textoAEntero(entrada, &opcion,error_o_datos_a_retornar,  1);
			sistema_memoria_liberar(entrada,error_o_datos_a_retornar,  0);
			if(!entradaValida) opcion = 0;
		}

		switch(opcion)
		{
			/*
			 * Opción 1: pide texto, separador y separador de unión.
			 * Ejemplo: "uno,dos", "," y "-".
			 * Resultado posible: muestra dos partes y "uno-dos"; luego libera
			 * las partes creadas por split.
			 */
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
			/*
			 * Opción 2: convierte un entero y analiza una traza.
			 * Ejemplo: entero "42", traza "°-2¬1".
			 * Resultado posible: enteroATexto muestra "42" y se informa código
			 * -2; el estado de error depende del código interpretado.
			 */
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
			/*
			 * Opción 3: pide código, información, resultado previo, función y nivel.
			 * Ejemplo: 1 / "prueba_sintetica" / vacío / "prueba" / 1.
			 * Resultado posible: imprime la cadena codificada que construye
			 * crearResultado; el formato concreto usa los separadores globales.
			 */
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
			/*
			 * Opción 4: pide un tamaño positivo y prueba reservar, ampliar y liberar.
			 * Ejemplo: tamaño "16".
			 * Resultado posible: informa si los bytes iniciales se conservaron;
			 * el backend puede declarar memoria no disponible.
			 */
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
			/*
			 * Opción 5: pide texto e entero; crea un archivo temporal, lo renombra,
			 * vuelve a leerlo y lo elimina.
			 * Ejemplo: "hola" y "42" -> contenido leído "hola|42".
			 * Si los nombres temporales ya existen, la prueba no los modifica.
			 */
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
			/*
			 * Opción 6: pide texto y prueba impresión UTF-8 y depuración.
			 * Ejemplo: "hola" -> imprime [hola] y reporta longitud 4.
			 */
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
			/*
			 * Opción 7: recibe tres líneas y agrega una copia de cada una al arreglo.
			 * Ejemplo: "uno", "dos", "tres" -> imprime esos valores en índices 0..2.
			 */
			case 7:
			{
				char **arreglo = NULL;
				int correcto = 1;
				for(int i = 0; i < 3; i++)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
						"Ingrese el texto %d que se agregará al arreglo: ", i + 1);
					char *texto = leerLineaConsola(error_o_datos_a_retornar, 1);
					if(texto == NULL)
					{
						correcto = 0;
						break;
					}

					agregar_texto_a_arreglo(&arreglo, texto);
					int cantidad_agregada = 0;
					if(arreglo != NULL)
					{
						while(arreglo[cantidad_agregada] != NULL)
						{
							cantidad_agregada++;
						}
					}
					if(cantidad_agregada != i + 1 ||
						strcmp(arreglo[i], texto) != 0)
					{
						correcto = 0;
					}
					sistema_memoria_liberar(texto,error_o_datos_a_retornar,  0);
					if(!correcto) break;
				}

				if(correcto)
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
						"Elementos agregados:\n");
					for(int i = 0; i < 3; i++)
					{
						sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
							"[%d] %s\n", i, arreglo[i]);
					}
				}
				else
				{
					sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
						"No se pudo completar la prueba del arreglo dinámico.\n");
				}

				if(arreglo != NULL)
				{
					for(int i = 0; arreglo[i] != NULL; i++)
					{
						sistema_memoria_liberar(arreglo[i],error_o_datos_a_retornar,  0);
					}
					sistema_memoria_liberar(arreglo,error_o_datos_a_retornar,  0);
				}
				break;
			}
			/*
			 * Opción 8: solicita ruta metadata, encabezado, registro y confirmación.
			 * Ejemplo: encabezado "id|nombre|cantidad", fila "Ana|3", "SI".
			 * Resultado posible: crea el registro 1|Ana|3, lo consulta y verifica
			 * que un segundo agregado con el mismo ID se reconozca como existente.
			 */
			case 8:
				return probar_almacenamiento_dividido(error_o_datos_a_retornar, 1) == 1 ? 0 : -1;
			/* Opción 9: vuelve al menú de pruebas sin ejecutar una operación. */
			case 9:
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
 * Prueba el almacenamiento dividido con una ruta y un registro elegidos por el usuario.
 * La escritura solo se ejecuta después de confirmar "SI".
 */
static int probar_almacenamiento_dividido(char *error_o_datos_a_retornar,int nivel_de_profundidad)
{
	char *ruta = NULL;
	char *columnas = NULL;
	char *datos = NULL;
	char *confirmacion = NULL;
	char *respuesta_agregar = NULL;
	char *metadata = NULL;
	char *registros = NULL;
	char *respuesta_seleccion = NULL;
	char *respuesta_extrae = NULL;
	char *respuesta_busqueda = NULL;
	char *respuesta_sino_existe = NULL;
	char *valor_busqueda = NULL;
	char *entrada_columna = NULL;
	char **partes_respuesta = NULL;
	char **celdas_fila = NULL;
	int cantidad_celdas = 0;
	int columna_busqueda = 0;
	int resultado = -1;

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Ruta del archivo metadata (use una carpeta existente): ");
	ruta = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Encabezado de columnas, incluyendo el ID (ejemplo: id|nombre|cantidad): ");
	columnas = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Datos del nuevo registro sin el ID (ejemplo: Ana|3): ");
	datos = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	if(ruta == NULL || columnas == NULL || datos == NULL)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"No se pudieron leer los parámetros de almacenamiento.\n");
		goto limpiar;
	}

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Esta prueba creará o modificará archivos. Escriba SI para continuar: ");
	confirmacion = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	if(confirmacion == NULL || strcmp(confirmacion, "SI") != 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Prueba cancelada; no se modificaron archivos.\n");
		goto limpiar;
	}

	respuesta_agregar = agregar_info_dividida(ruta, datos, columnas);
	if(respuesta_agregar == NULL ||
		split_con_numero_de_celdas(
			respuesta_agregar,
			GG_caracter_para_confirmacion_o_error[0],
			&partes_respuesta) < 2 ||
		partes_respuesta == NULL ||
		strcmp(partes_respuesta[0], "1") != 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"No se pudo agregar el registro: %s\n",
			respuesta_agregar != NULL ? respuesta_agregar : "(sin respuesta)");
		goto limpiar;
	}
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Registro agregado: %s\n", partes_respuesta[1]);

	cantidad_celdas = split_con_numero_de_celdas(
		partes_respuesta[1], GG_caracter_separacion[0], &celdas_fila);
	if(cantidad_celdas < 2 || celdas_fila[0] == NULL)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"La fila agregada no tiene un formato válido.\n");
		goto limpiar;
	}

	metadata = leer_info_dividida(ruta);
	registros = leer_todo_info_dividida(ruta);
	if(metadata == NULL || registros == NULL)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"No se pudieron leer la metadata o los registros.\n");
		goto limpiar;
	}
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Metadata:\n%s\nRegistros:\n%s\n", metadata, registros);

	respuesta_seleccion = seleccionar_id_info_dividida(ruta, celdas_fila[0]);
	respuesta_extrae = seleccionar_id_info_dividida_extrae(ruta, celdas_fila[0]);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Consulta por ID: %s\nConsulta con ubicación del bloque: %s\n",
		respuesta_seleccion != NULL ? respuesta_seleccion : "(falló)",
		respuesta_extrae != NULL ? respuesta_extrae : "(falló)");
	if(respuesta_seleccion == NULL || respuesta_extrae == NULL)
	{
		goto limpiar;
	}

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Índice de columna para buscar (0 es el ID): ");
	entrada_columna = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	if(entrada_columna == NULL ||
		!textoAEntero(entrada_columna, &columna_busqueda,
			error_o_datos_a_retornar, nivel_de_profundidad) ||
		columna_busqueda < 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Índice de columna no válido.\n");
		goto limpiar;
	}
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Valor que se buscará en esa columna: ");
	valor_busqueda = leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
	if(valor_busqueda == NULL)
	{
		goto limpiar;
	}

	respuesta_busqueda = buscar_info_dividida(
		ruta, valor_busqueda, columna_busqueda, 0);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado de búsqueda: %s\n",
		respuesta_busqueda != NULL ? respuesta_busqueda : "(falló)");
	if(respuesta_busqueda == NULL)
	{
		goto limpiar;
	}

	respuesta_sino_existe = agregar_sino_existe_info_dividida(
		ruta, 0, celdas_fila[0], datos, columnas);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Comprobación de agregar solo si no existe (mismo ID): %s\n",
		respuesta_sino_existe != NULL ? respuesta_sino_existe : "(falló)");
	if(respuesta_sino_existe != NULL)
	{
		resultado = 1;
	}

limpiar:
	if(partes_respuesta != NULL) free_split_con_numero_de_celdas(partes_respuesta);
	if(celdas_fila != NULL) free_split_con_numero_de_celdas(celdas_fila);
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(columnas, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(datos, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(confirmacion, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta_agregar, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(metadata, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(registros, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta_seleccion, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta_extrae, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta_busqueda, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta_sino_existe, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(valor_busqueda, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(entrada_columna, error_o_datos_a_retornar, 0);
	return resultado;
}

/*
 * Extrae el código inicial de una traza, con o sin separador inicial.
 * Ejemplos: "°-2¬1" y "-2|error" guardan -2 y devuelven 1.
 */
  /**
   * Extrae el código entero inicial de una traza y admite un separador opcional.
   * Entradas: error_o_datos_a_retornar y nivel son contexto (el nivel se ignora);
   * texto es la traza; codigo apunta a la salida. Devuelve 1 al analizar y 0 si
   * los argumentos o el formato no son válidos.
   * Entrada de ejemplo: texto="°-2¬1" y *codigo=0.
   * Salida de ejemplo: devuelve 1 y *codigo pasa de 0 a -2.
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

/**
 * Indica si una cadena de resultado es inválida o contiene un código negativo.
 * Entradas: error_o_datos_a_retornar y nivel son contexto; texto es la traza.
 * Devuelve 1 para error/forma inválida y 0 para código válido no negativo.
 * Entrada de ejemplo: resultadoTieneError(NULL,0,"°-2").
 * Salida de ejemplo: 1; con "°0" la salida sería 0.
 */
 static int resultadoTieneError(char *error_o_datos_a_retornar,int nivel_de_profundidad, const char *texto)
 {
 	int codigo;
 	return !leerCodigoResultado(error_o_datos_a_retornar, nivel_de_profundidad, texto, &codigo) || codigo < 0;
 }

/**
 * Guarda una copia independiente del texto como último retorno estándar global.
 * Entrada: texto es la cadena a copiar. Devuelve 1 si la copia se registra o 0
 * si texto es NULL/no se puede reservar; sustituye y libera el valor anterior.
 * Entrada de ejemplo: actualizar_ultimo_retorno("°1"), con memoria disponible.
 * Salida de ejemplo: devuelve 1 y GG_ultimo_retorno_estandar pasa a contener "°1".
 */
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

 /**
  * Añade un texto no vacío al acumulador global de retornos de proceso.
  * Entrada: texto es el segmento a anexar. Devuelve 1 si lo agrega, 0 si es NULL/
  * vacío y -1 ante desbordamiento o fallo de redimensionamiento. Entre segmentos
  * usa el marcador de transferencia configurado.
  * Entrada de ejemplo: acumulador="primero", acumular_retorno_proceso("segundo").
  * Salida de ejemplo: devuelve 1 y el acumulador queda "primero■segundo".
  */
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

 /**
  * Libera y reinicia el acumulador global de retornos de proceso.
  * Entrada: ninguna. Efecto: libera el bloque y deja el puntero global en NULL.
  * Entrada de ejemplo: GG_acumulador_retorno_procesos="uno■dos".
  * Salida de ejemplo: void; GG_acumulador_retorno_procesos pasa a NULL.
  */
 void limpiar_retorno_procesos(void)
 {
 	sistema_memoria_liberar(GG_acumulador_retorno_procesos, NULL, 0);
 	GG_acumulador_retorno_procesos = NULL;
 }

 /**
  * Obtiene el puntero global al último resultado estándar formateado.
  * Entrada: ninguna. Salida: puntero de solo lectura gestionado por el módulo;
  * puede ser NULL antes de que se registre un resultado.
  * Entrada de ejemplo: tras crearResultado(1,...,nivel=0).
  * Salida de ejemplo: devuelve un puntero a "|1" (no liberar desde el llamador).
  */
 const char *obtener_ultimo_retorno_formateado(void)
 {
 	return GG_ultimo_retorno_estandar;
 }



 #pragma endregion FUNCIONES_DE_DEPURACION

/*
 * MODULO PROVISIONAL proceso_mul
 * Estas funciones se dejaron al final para que sus contratos y reglas de
 * negocio puedan revisarse antes de considerarlos definitivos.
 */
typedef struct proceso_mul
{
	const char *separador_columnas;
	const char *separador_relacion;
	const char *columnas_afiliados;
} proceso_mul;

static char * pm_copiar_texto(const char *texto)
{
	if(texto == NULL) texto = "";
	size_t longitud = strlen(texto) + 1;
	char *copia = sistema_memoria_reservar(longitud, NULL, 0);
	if(copia != NULL) memcpy(copia, texto, longitud);
	return copia;
}

static int pm_convertir_double(const char *texto, double *salida)
{
	if(texto == NULL || texto[0] == '\0' || salida == NULL) return 0;
	char *fin = NULL;
	errno = 0;
	double valor = strtod(texto, &fin);
	if(errno != 0 || fin == texto || *fin != '\0') return 0;
	*salida = valor;
	return 1;
}

static char *pm_formatear_double(double valor)
{
	return variable_string("%.15g", valor);
}

static int pm_texto_sin_separadores(const char *texto, const char *separador)
{
	if(texto == NULL || separador == NULL || separador[0] == '\0') return 0;
	return strstr(texto, separador) == NULL &&
		strchr(texto, '\n') == NULL && strchr(texto, '\r') == NULL;
}

static void pm_reemplazar_campo(char **campos, int indice, const char *valor)
{
	if(campos == NULL || indice < 0 || indice >= 44) return;
	sistema_memoria_liberar(campos[indice], NULL, 0);
	campos[indice] = pm_copiar_texto(valor);
}

void proceso_mul_inicializar(proceso_mul *p)
{
	if(p == NULL) return;
	p->separador_columnas = GG_caracter_separacion[1];
	p->separador_relacion = GG_caracter_separacion[2];
	p->columnas_afiliados = GG_secciones_negocio[4].columnas;
}

static int pm_obtener_fila_afiliado(
	const proceso_mul *p,
	const char *ruta,
	const char *id_afiliado,
	const char *relacion,
	char **fila_salida,
	int *numero_linea_salida,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(fila_salida != NULL) *fila_salida = NULL;
	if(numero_linea_salida != NULL) *numero_linea_salida = 0;
	if(p == NULL || ruta == NULL || id_afiliado == NULL ||
		fila_salida == NULL || numero_linea_salida == NULL) return -1;

	SistemaArchivo *archivo = sistema_archivo_abrir(
		ruta, "r", error_o_datos_a_retornar, nivel_de_profundidad);
	if(archivo == NULL) return -1;

	char *fila_base = NULL;
	int linea_base = 0;
	int linea_actual = 0;
	int estado = 0;
	char *linea = NULL;
	while((linea = leerLineaDinamica(
		archivo, error_o_datos_a_retornar, nivel_de_profundidad)) != NULL)
	{
		linea_actual++;
		if(linea_actual == 1)
		{
			if(strcmp(linea, p->columnas_afiliados) != 0) estado = -2;
			sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
			if(estado == -2) break;
			continue;
		}

		int cantidad = 0;
		char **campos = split(
			linea, p->separador_columnas, &cantidad,
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(campos == NULL)
		{
			sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
			estado = -1;
			break;
		}
		if(cantidad == 44 && strcmp(campos[0], id_afiliado) == 0)
		{
			int coincidencia_relacion = relacion != NULL &&
				strcmp(campos[32], relacion) == 0;
			if(coincidencia_relacion)
			{
				*fila_salida = pm_copiar_texto(linea);
				*numero_linea_salida = linea_actual;
				estado = *fila_salida != NULL ? 1 : -1;
				liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
				break;
			}
			if(fila_base == NULL)
			{
				fila_base = pm_copiar_texto(linea);
				linea_base = linea_actual;
				if(fila_base == NULL) estado = -1;
			}
		}
		else if(strcmp(campos[0], id_afiliado) == 0 && cantidad != 44)
		{
			estado = -2;
			liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
			break;
		}
		liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
		if(estado == -1) break;
	}

	if(estado == 0 && linea_actual == 0) estado = -2;
	if(sistema_archivo_hay_error(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0) estado = -1;
	if(sistema_archivo_cerrar(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0) estado = -1;

	if(estado < 0)
	{
		sistema_memoria_liberar(*fila_salida, error_o_datos_a_retornar, 0);
		*fila_salida = NULL;
		sistema_memoria_liberar(fila_base, error_o_datos_a_retornar, 0);
		return estado;
	}

	if(estado == 0 && fila_base != NULL)
	{
		*fila_salida = fila_base;
		*numero_linea_salida = linea_base;
		fila_base = NULL;
		estado = 2;
	}
	sistema_memoria_liberar(fila_base, error_o_datos_a_retornar, 0);
	return estado;
}

/*
 * Guarda una relacion por fila, copiando los campos personales existentes del
 * afiliado. La fila debe usar el esquema actual de 44 columnas.
 */
char *registro_unificado_cod3_r(
	proceso_mul *p,
	const char *ruta_afiliados,
	const char *id_afiliado,
	const char *tipo_relacion,
	const char *proyecto_tabla,
	const char *id_relacionado,
	const char *puntos_por_recibir,
	const char *puntos_recibidos_totales,
	const char *nivel,
	const char *id_horizontal,
	const char *tipo_afiliado,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(p == NULL || ruta_afiliados == NULL || id_afiliado == NULL ||
		tipo_relacion == NULL || proyecto_tabla == NULL ||
		id_relacionado == NULL || puntos_por_recibir == NULL ||
		puntos_recibidos_totales == NULL || nivel == NULL ||
		id_horizontal == NULL || tipo_afiliado == NULL)
		return crearResultado(-1, "parametros_invalidos", "", __func__,
			error_o_datos_a_retornar, nivel_de_profundidad);

	const char *campos_validar[] = {
		id_afiliado, tipo_relacion, proyecto_tabla, id_relacionado,
		puntos_por_recibir, puntos_recibidos_totales, nivel,
		id_horizontal, tipo_afiliado
	};
	for(size_t i = 0; i < sizeof(campos_validar) / sizeof(campos_validar[0]); ++i)
	{
		if(!pm_texto_sin_separadores(campos_validar[i], p->separador_columnas))
			return crearResultado(-1, "campo_contiene_separador_de_columnas",
				"", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}
	if(!pm_texto_sin_separadores(tipo_relacion, p->separador_relacion) ||
		!pm_texto_sin_separadores(proyecto_tabla, p->separador_relacion) ||
		!pm_texto_sin_separadores(id_relacionado, p->separador_relacion))
		return crearResultado(-1, "relacion_contiene_separador_reservado", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	if(strcmp(tipo_relacion, "PATROCINADOR") != 0 &&
		strcmp(tipo_relacion, "ENCARGADO") != 0)
		return crearResultado(-1, "tipo_relacion_no_valido",
			"Use PATROCINADOR o ENCARGADO.", __func__,
			error_o_datos_a_retornar, nivel_de_profundidad);

	if(!existe_archivo(ruta_afiliados))
		return crearResultado(-1, "archivo_afiliados_no_existe",
			"Primero crea la tabla y el registro personal del afiliado.",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);

	char *relacion = variable_string(
		"%s%s%s%s%s",
		tipo_relacion, p->separador_relacion,
		proyecto_tabla, p->separador_relacion, id_relacionado);
	if(relacion == NULL)
		return crearResultado(-1, "sin_memoria_para_relacion", "", __func__,
			error_o_datos_a_retornar, nivel_de_profundidad);

	char *fila_existente = NULL;
	int numero_linea = 0;
	int estado = pm_obtener_fila_afiliado(
		p, ruta_afiliados, id_afiliado, relacion,
		&fila_existente, &numero_linea,
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(estado < 0)
	{
		sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
		return crearResultado(-1,
			estado == -2 ? "esquema_afiliados_incompatible_o_id_inexistente"
				: "error_al_leer_afiliados",
			"", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}
	if(estado == 0)
	{
		sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
		return crearResultado(-1, "id_afiliado_no_encontrado",
			"Debe existir primero el registro personal del afiliado.",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	int cantidad = 0;
	char **campos = split(
		fila_existente, p->separador_columnas, &cantidad,
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(campos == NULL || cantidad != 44)
	{
		liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(fila_existente, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
		return crearResultado(-1, "fila_afiliado_no_tiene_44_columnas", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	pm_reemplazar_campo(campos, 32, relacion);
	pm_reemplazar_campo(campos, 33, puntos_recibidos_totales);
	pm_reemplazar_campo(campos, 38, nivel);
	pm_reemplazar_campo(campos, 41, puntos_por_recibir);
	pm_reemplazar_campo(campos, 42, id_horizontal);
	pm_reemplazar_campo(campos, 43, tipo_afiliado);
	char *fila_nueva = join(
		campos, cantidad, p->separador_columnas,
		error_o_datos_a_retornar, nivel_de_profundidad);
	liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(fila_existente, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
	if(fila_nueva == NULL)
		return crearResultado(-1, "no_se_pudo_formar_fila_afiliado", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);

	char *resultado_operacion = estado == 1
		? editarLinea(ruta_afiliados, numero_linea, fila_nueva,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: escribirLinea(ruta_afiliados, fila_nueva,
			error_o_datos_a_retornar, nivel_de_profundidad);
	int error = resultadoTieneError(
		error_o_datos_a_retornar, nivel_de_profundidad, resultado_operacion);
	char *resultado = crearResultado(
		error ? -1 : 1,
		error ? "no_se_pudo_guardar_relacion" :
			(estado == 1 ? "relacion_actualizada" : "relacion_agregada"),
		"", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_memoria_liberar(resultado_operacion, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(fila_nueva, error_o_datos_a_retornar, 0);
	return resultado;
}

char *dinero_de_venta(
	const char *cantidad,
	const char *comision,
	const char *tipo)
{
	double venta = 0.0, valor_comision = 0.0;
	if(!pm_convertir_double(cantidad, &venta) ||
		!pm_convertir_double(comision, &valor_comision))
		return NULL;
	double pago = tipo != NULL && strcmp(tipo, "DINERO") == 0
		? valor_comision : venta * valor_comision / 100.0;
	return pm_formatear_double(pago);
}

char *entrada_dinero_simple(
	const char *cantidad,
	const char *comisiones,
	const char *tipo,
	const char *separador)
{
	if(separador == NULL || separador[0] == '\0') return NULL;
	int cantidad_comisiones = 0;
	char **porcentajes = split(comisiones, separador, &cantidad_comisiones, NULL, 0);
	if(porcentajes == NULL) return NULL;
	char **pagos = sistema_memoria_reservar(
		((size_t)cantidad_comisiones + 1) * sizeof(*pagos), NULL, 0);
	if(pagos == NULL)
	{
		liberarSplit(porcentajes, cantidad_comisiones, NULL, 0);
		return NULL;
	}
	memset(pagos, 0, ((size_t)cantidad_comisiones + 1) * sizeof(*pagos));
	int correcto = 1;
	for(int i = 0; i < cantidad_comisiones; ++i)
	{
		pagos[i] = dinero_de_venta(cantidad, porcentajes[i], tipo);
		if(pagos[i] == NULL) { correcto = 0; break; }
	}
	char *resultado = correcto
		? join(pagos, cantidad_comisiones, separador, NULL, 0)
		: NULL;
	liberarSplit(pagos, cantidad_comisiones, NULL, 0);
	liberarSplit(porcentajes, cantidad_comisiones, NULL, 0);
	return resultado;
}

static char *pm_buscar_patrocinador(
	const char *ruta,
	const char *id_usuario,
	const char *proyecto,
	const char *separador_columnas,
	const char *separador_relacion,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	SistemaArchivo *archivo = sistema_archivo_abrir(
		ruta, "r", error_o_datos_a_retornar, nivel_de_profundidad);
	if(archivo == NULL) return NULL;
	char *patrocinador = NULL;
	char *linea = NULL;
	int numero_linea = 0;
	while((linea = leerLineaDinamica(
		archivo, error_o_datos_a_retornar, nivel_de_profundidad)) != NULL)
	{
		if(numero_linea++ == 0)
		{
			sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
			continue;
		}
		int cantidad = 0;
		char **campos = split(linea, separador_columnas, &cantidad,
			error_o_datos_a_retornar, nivel_de_profundidad);
		sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
		if(campos == NULL) break;
		if(cantidad == 44 && strcmp(campos[0], id_usuario) == 0)
		{
			int partes_relacion = 0;
			char **partes = split(campos[32], separador_relacion,
				&partes_relacion, error_o_datos_a_retornar, nivel_de_profundidad);
			if(partes != NULL && partes_relacion == 3 &&
				strcmp(partes[0], "PATROCINADOR") == 0 &&
				strcmp(partes[1], proyecto) == 0)
			{
				patrocinador = pm_copiar_texto(partes[2]);
				liberarSplit(partes, partes_relacion, error_o_datos_a_retornar, 0);
				liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
				break;
			}
			liberarSplit(partes, partes_relacion, error_o_datos_a_retornar, 0);
		}
		liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
	}
	if(sistema_archivo_hay_error(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0) patrocinador = NULL;
	if(sistema_archivo_cerrar(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0)
	{
		sistema_memoria_liberar(patrocinador, error_o_datos_a_retornar, 0);
		patrocinador = NULL;
	}
	return patrocinador;
}

static char *pm_extraer_cadena_patrocinadores(
	const char *ruta,
	const char *id_usuario,
	const char *proyecto,
	int profundidad,
	const char *separador_retorno,
	const proceso_mul *p,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(profundidad <= 0) return pm_copiar_texto("");
	char *patrocinador = pm_buscar_patrocinador(
		ruta, id_usuario, proyecto, p->separador_columnas,
		p->separador_relacion, error_o_datos_a_retornar, nivel_de_profundidad);
	if(patrocinador == NULL || patrocinador[0] == '\0') return patrocinador;
	char *siguiente = pm_extraer_cadena_patrocinadores(
		ruta, patrocinador, proyecto, profundidad - 1, separador_retorno,
		p, error_o_datos_a_retornar, nivel_de_profundidad);
	char *resultado = siguiente != NULL && siguiente[0] != '\0'
		? variable_string("%s%s%s", patrocinador, separador_retorno, siguiente)
		: pm_copiar_texto(patrocinador);
	sistema_memoria_liberar(patrocinador, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(siguiente, error_o_datos_a_retornar, 0);
	return resultado;
}

char *extraer_patrosinadores_funcion_recursiva(
	const char *ruta,
	const char *id_usuario,
	const char *proyecto,
	int cantidad,
	const char *separador_retorno,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(ruta == NULL || id_usuario == NULL || proyecto == NULL ||
		cantidad < 0 || cantidad > 100)
		return NULL;
	proceso_mul p;
	proceso_mul_inicializar(&p);
	if(separador_retorno == NULL || separador_retorno[0] == '\0')
		separador_retorno = GG_caracter_separacion[0];
	return pm_extraer_cadena_patrocinadores(
		ruta, id_usuario, proyecto, cantidad, separador_retorno, &p,
		error_o_datos_a_retornar, nivel_de_profundidad);
}

char *calc_din_por_enc_y_total(
	const char *dinero,
	const char *porcentajes_simples,
	const char *porcentajes_complejos,
	const char *comision_venta_compleja,
	const char *separador_porcentajes)
{
	double importe = 0.0, comision_compleja = 0.0, total = 0.0;
	if(!pm_convertir_double(dinero, &importe) ||
		!pm_convertir_double(comision_venta_compleja, &comision_compleja) ||
		separador_porcentajes == NULL || separador_porcentajes[0] == '\0')
		return NULL;

	const char *listas[2] = { porcentajes_simples, porcentajes_complejos };
	char *pagos[2] = { NULL, NULL };
	for(int grupo = 0; grupo < 2; ++grupo)
	{
		int cantidad = 0;
		char **porcentajes = split(listas[grupo], separador_porcentajes,
			&cantidad, NULL, 0);
		if(porcentajes == NULL) goto fallo_calc_din;
		char **montos = sistema_memoria_reservar(
			((size_t)cantidad + 1) * sizeof(*montos), NULL, 0);
		if(montos == NULL)
		{
			liberarSplit(porcentajes, cantidad, NULL, 0);
			goto fallo_calc_din;
		}
		memset(montos, 0, ((size_t)cantidad + 1) * sizeof(*montos));
		int correcto = 1;
		for(int i = 0; i < cantidad; ++i)
		{
			double porcentaje = 0.0;
			if(!pm_convertir_double(porcentajes[i], &porcentaje))
			{
				correcto = 0;
				break;
			}
			double pago = grupo == 0
				? importe * porcentaje / 100.0
				: importe * comision_compleja / 100.0 * porcentaje / 100.0;
			total += pago;
			montos[i] = pm_formatear_double(pago);
			if(montos[i] == NULL) { correcto = 0; break; }
		}
		if(correcto) pagos[grupo] = join(montos, cantidad,
			separador_porcentajes, NULL, 0);
		liberarSplit(montos, cantidad, NULL, 0);
		liberarSplit(porcentajes, cantidad, NULL, 0);
		if(!correcto || pagos[grupo] == NULL) goto fallo_calc_din;
	}

	char *total_texto = pm_formatear_double(total);
	char *resultado = total_texto != NULL
		? variable_string("%s%s%s%s%s",
			pagos[0], GG_caracter_separacion_funciones_espesificas[1],
			pagos[1], GG_caracter_separacion_funciones_espesificas[1],
			total_texto)
		: NULL;
	sistema_memoria_liberar(total_texto, NULL, 0);
	sistema_memoria_liberar(pagos[0], NULL, 0);
	sistema_memoria_liberar(pagos[1], NULL, 0);
	return resultado;

fallo_calc_din:
	sistema_memoria_liberar(pagos[0], NULL, 0);
	sistema_memoria_liberar(pagos[1], NULL, 0);
	return NULL;
}

char **acumulador_de_strings(
	const char *texto,
	int veces,
	const char *separador_entrada,
	const char *separador_salida)
{
	if(texto == NULL || veces < 0) return NULL;
	if(separador_entrada == NULL || separador_entrada[0] == '\0')
		separador_entrada = GG_caracter_separacion[0];
	int cantidad = 0;
	char **partes = split(texto, separador_entrada, &cantidad, NULL, 0);
	if(partes == NULL) return NULL;
	char **resultado = sistema_memoria_reservar(
		((size_t)cantidad + 1) * sizeof(*resultado), NULL, 0);
	if(resultado == NULL)
	{
		liberarSplit(partes, cantidad, NULL, 0);
		return NULL;
	}
	memset(resultado, 0, ((size_t)cantidad + 1) * sizeof(*resultado));
	for(int i = 0; i < cantidad; ++i)
	{
		resultado[i] = pm_copiar_texto("");
		for(int repeticion = 0; repeticion < veces; ++repeticion)
		{
			char *nuevo = variable_string(
				"%s%s%s",
				resultado[i],
				repeticion > 0 && separador_salida != NULL
					? separador_salida : "",
				partes[i]);
			if(nuevo == NULL)
			{
				liberarSplit(resultado, cantidad, NULL, 0);
				liberarSplit(partes, cantidad, NULL, 0);
				return NULL;
			}
			sistema_memoria_liberar(resultado[i], NULL, 0);
			resultado[i] = nuevo;
		}
		if(resultado[i] == NULL)
		{
			liberarSplit(resultado, cantidad, NULL, 0);
			liberarSplit(partes, cantidad, NULL, 0);
			return NULL;
		}
	}
	liberarSplit(partes, cantidad, NULL, 0);
	return resultado;
}

static char *pm_pedir_texto(
	const char *solicitud,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	sistema_consola_escribir_formato(
		error_o_datos_a_retornar, nivel_de_profundidad, "%s", solicitud);
	return leerLineaConsola(error_o_datos_a_retornar, nivel_de_profundidad);
}

static int pm_pedir_entero(
	const char *solicitud,
	int *valor,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	char *texto = pm_pedir_texto(solicitud, error_o_datos_a_retornar,
		nivel_de_profundidad);
	if(texto == NULL) return 0;
	int correcto = textoAEntero(texto, valor, error_o_datos_a_retornar,
		nivel_de_profundidad);
	sistema_memoria_liberar(texto, error_o_datos_a_retornar, 0);
	return correcto;
}

static int pm_long_a_texto(long valor, char *buffer, size_t capacidad)
{
	if(buffer == NULL || capacidad == 0) return 0;
	unsigned long magnitud = valor < 0
		? (unsigned long)(-(valor + 1)) + 1UL
		: (unsigned long)valor;
	char invertido[sizeof(unsigned long) * CHAR_BIT];
	size_t cantidad = 0;
	do
	{
		invertido[cantidad++] = (char)('0' + magnitud % 10UL);
		magnitud /= 10UL;
	} while(magnitud != 0 && cantidad < sizeof(invertido));
	size_t signo = valor < 0 ? 1U : 0U;
	if(cantidad + signo + 1 > capacidad) return 0;
	size_t indice = 0;
	if(signo) buffer[indice++] = '-';
	while(cantidad > 0) buffer[indice++] = invertido[--cantidad];
	buffer[indice] = '\0';
	return 1;
}

static int pm_siguiente_id_afiliado(
	const char *ruta,
	const char *cabecera,
	long *id_siguiente,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(ruta == NULL || cabecera == NULL || id_siguiente == NULL) return 0;
	if(crearArchivo(ruta, cabecera) < 0) return 0;

	SistemaArchivo *archivo = sistema_archivo_abrir(
		ruta, "r", error_o_datos_a_retornar, nivel_de_profundidad);
	if(archivo == NULL) return 0;

	char *linea = leerLineaDinamica(
		archivo, error_o_datos_a_retornar, nivel_de_profundidad);
	int valido = linea != NULL && strcmp(linea, cabecera) == 0;
	sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
	long maximo = 0;
	while(valido && (linea = leerLineaDinamica(
		archivo, error_o_datos_a_retornar, nivel_de_profundidad)) != NULL)
	{
		int cantidad_campos = 0;
		char **campos = split(linea, GG_caracter_separacion[1],
			&cantidad_campos, error_o_datos_a_retornar, nivel_de_profundidad);
		sistema_memoria_liberar(linea, error_o_datos_a_retornar, 0);
		if(campos == NULL || cantidad_campos != 44)
		{
			liberarSplit(campos, cantidad_campos, error_o_datos_a_retornar, 0);
			valido = 0;
			break;
		}

		char *fin = NULL;
		errno = 0;
		long id_actual = strtol(campos[0], &fin, 10);
		if(errno != 0 || fin == campos[0] || *fin != '\0' || id_actual <= 0)
		{
			liberarSplit(campos, cantidad_campos, error_o_datos_a_retornar, 0);
			valido = 0;
			break;
		}
		if(id_actual > maximo) maximo = id_actual;
		liberarSplit(campos, cantidad_campos, error_o_datos_a_retornar, 0);
	}

	if(sistema_archivo_hay_error(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0) valido = 0;
	if(sistema_archivo_cerrar(archivo, error_o_datos_a_retornar,
		nivel_de_profundidad) != 0) valido = 0;
	if(!valido || maximo == LONG_MAX) return 0;
	*id_siguiente = maximo + 1;
	return 1;
}

static int pm_agregar_usuario_afiliado(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta_predeterminada =
		"CONFIG/AFILIADOS/AFILIADOS_UNIFICADO.TXT";
	char *ruta = pm_pedir_texto(
		"Ruta de AFILIADOS_UNIFICADO.TXT (Enter usa la ruta configurada): ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(ruta == NULL) return -1;
	if(ruta[0] == '\0')
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		ruta = pm_copiar_texto(ruta_predeterminada);
		if(ruta == NULL) return -1;
	}

	int cantidad_columnas = 0;
	char **nombres_columnas = split(
		GG_secciones_negocio[4].columnas, GG_caracter_separacion[1],
		&cantidad_columnas, error_o_datos_a_retornar, nivel_de_profundidad);
	if(nombres_columnas == NULL || cantidad_columnas != 44)
	{
		liberarSplit(nombres_columnas, cantidad_columnas,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"El esquema configurado de afiliados no tiene 44 columnas.\n");
		return -1;
	}

	char **campos = sistema_memoria_reservar(
		(size_t)cantidad_columnas * sizeof(*campos),
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(campos == NULL)
	{
		liberarSplit(nombres_columnas, cantidad_columnas,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		return -1;
	}
	for(int i = 0; i < cantidad_columnas; ++i)
	{
		campos[i] = pm_copiar_texto("");
		if(campos[i] == NULL)
		{
			for(int j = 0; j < i; ++j)
				sistema_memoria_liberar(campos[j], error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
			liberarSplit(nombres_columnas, cantidad_columnas,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
			return -1;
		}
	}

	int correcto = 1;
	for(int i = 1; i < cantidad_columnas; ++i)
	{
		char *solicitud = variable_string(
			"Valor para %s (Enter lo deja vacio): ", nombres_columnas[i]);
		if(solicitud == NULL)
		{
			correcto = 0;
			break;
		}
		char *valor = pm_pedir_texto(
			solicitud, error_o_datos_a_retornar, nivel_de_profundidad);
		sistema_memoria_liberar(solicitud, error_o_datos_a_retornar, 0);
		if(valor == NULL ||
			strstr(valor, GG_caracter_separacion[1]) != NULL ||
			strchr(valor, '\r') != NULL || strchr(valor, '\n') != NULL)
		{
			sistema_memoria_liberar(valor, error_o_datos_a_retornar, 0);
			correcto = 0;
			break;
		}
		sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
		campos[i] = valor;
	}
	if(correcto && campos[1][0] == '\0')
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"El campo NOMBRE es obligatorio.\n");
		correcto = 0;
	}

	char *confirmacion = NULL;
	if(correcto)
	{
		confirmacion = pm_pedir_texto(
			"Se agregara un usuario al archivo. Escriba SI para confirmar: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		correcto = confirmacion != NULL &&
			(strcmp(confirmacion, "SI") == 0 || strcmp(confirmacion, "si") == 0);
	}

	long id_siguiente = 0;
	if(correcto)
	{
		correcto = pm_siguiente_id_afiliado(
			ruta, GG_secciones_negocio[4].columnas, &id_siguiente,
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(!correcto)
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"No se pudo validar el encabezado o las filas existentes.\n");
	}

	if(correcto)
	{
		char id_texto[sizeof(long) * CHAR_BIT + 2];
		if(!pm_long_a_texto(id_siguiente, id_texto, sizeof(id_texto)))
			correcto = 0;
		else
		{
			sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
			campos[0] = pm_copiar_texto(id_texto);
			correcto = campos[0] != NULL;
		}
	}

	char *fila = correcto
		? join(campos, cantidad_columnas, GG_caracter_separacion[1],
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(correcto && fila == NULL) correcto = 0;
	char *resultado_escritura = NULL;
	if(correcto)
	{
		resultado_escritura = escribirLinea(
			ruta, fila, error_o_datos_a_retornar, nivel_de_profundidad);
		correcto = resultado_escritura != NULL &&
			!resultadoTieneError(
				error_o_datos_a_retornar, nivel_de_profundidad,
				resultado_escritura);
	}

	if(correcto)
	{
		char id_asignado[sizeof(long) * CHAR_BIT + 2];
		if(!pm_long_a_texto(id_siguiente, id_asignado, sizeof(id_asignado)))
			correcto = 0;
		else
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Usuario agregado. ID asignado: %s\n", id_asignado);
	}
	else if(confirmacion != NULL &&
		strcmp(confirmacion, "SI") != 0 && strcmp(confirmacion, "si") != 0)
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Operación cancelada; no se agregó el usuario.\n");

	sistema_memoria_liberar(resultado_escritura, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	for(int i = 0; i < cantidad_columnas; ++i)
		sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(confirmacion, error_o_datos_a_retornar, 0);
	liberarSplit(nombres_columnas, cantidad_columnas,
		error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

int submenu_pruebas_proceso_mul(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PROCESO DE AFILIADOS ===\n"
		"1. Agregar usuario nuevo a AFILIADOS_UNIFICADO\n"
		"2. Registrar/actualizar relacion de afiliado\n"
		"3. Calcular comision de una venta\n"
		"4. Calcular comisiones simples\n"
		"5. Buscar cadena de patrocinadores\n"
		"6. Calcular pagos simples, complejos y total\n"
		"7. Repetir elementos de una lista de texto\n"
		"8. Volver\n");
	if(!pm_pedir_entero("Seleccione una opcion: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad)) return -1;

	if(opcion == 8) return 0;
	if(opcion < 1 || opcion > 7) return -1;

	if(opcion == 1)
		return pm_agregar_usuario_afiliado(
			error_o_datos_a_retornar, nivel_de_profundidad);

	if(opcion == 2)
	{
		proceso_mul p;
		proceso_mul_inicializar(&p);
		char *ruta = pm_pedir_texto("Ruta de AFILIADOS_UNIFICADO.TXT: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *id = pm_pedir_texto("ID del afiliado existente: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *tipo_relacion = pm_pedir_texto(
			"Tipo (PATROCINADOR o ENCARGADO): ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *proyecto = pm_pedir_texto("Proyecto/tabla: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *id_relacionado = pm_pedir_texto("ID relacionado: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *puntos_recibir = pm_pedir_texto("Puntos del proyecto por recibir: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *puntos_total = pm_pedir_texto("Puntos recibidos totales: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *nivel = pm_pedir_texto("Nivel: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *horizontal = pm_pedir_texto("ID horizontal: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *tipo_afiliado = pm_pedir_texto("Tipo de afiliado: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *confirmacion = pm_pedir_texto(
			"Se escribira el archivo. Escriba SI para confirmar: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		int correcto = ruta != NULL && id != NULL && tipo_relacion != NULL &&
			proyecto != NULL && id_relacionado != NULL && puntos_recibir != NULL &&
			puntos_total != NULL && nivel != NULL && horizontal != NULL &&
			tipo_afiliado != NULL && confirmacion != NULL &&
			strcmp(confirmacion, "SI") == 0;
		if(correcto)
		{
			char *resultado = registro_unificado_cod3_r(
				&p, ruta, id, tipo_relacion, proyecto, id_relacionado,
				puntos_recibir, puntos_total, nivel, horizontal, tipo_afiliado,
				error_o_datos_a_retornar, nivel_de_profundidad);
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Resultado: %s\n", resultado != NULL ? resultado : "(sin resultado)");
			correcto = resultado != NULL &&
				!resultadoTieneError(error_o_datos_a_retornar,
					nivel_de_profundidad, resultado);
			if(correcto)
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
					"Registro de relacion guardado correctamente.\n");
				char *lectura = leerArchivo(
					ruta, error_o_datos_a_retornar, nivel_de_profundidad);
				if(resultadoTieneError(error_o_datos_a_retornar,
					nivel_de_profundidad, lectura)) correcto = 0;
				sistema_memoria_liberar(
					lectura, error_o_datos_a_retornar, 0);
			}
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
		}
		else
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Cancelado o faltan parametros; no se modifico el archivo.\n");
		}
		const char *nombres[] = { ruta, id, tipo_relacion, proyecto,
			id_relacionado, puntos_recibir, puntos_total, nivel,
			horizontal, tipo_afiliado, confirmacion };
		for(size_t i = 0; i < sizeof(nombres) / sizeof(nombres[0]); ++i)
			sistema_memoria_liberar((void *)nombres[i], error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}

	if(opcion == 3)
	{
		char *cantidad = pm_pedir_texto("Monto de la venta: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *comision = pm_pedir_texto("Comision (porcentaje o monto): ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *tipo = pm_pedir_texto("Tipo (PORCENTAJE o DINERO): ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *resultado = dinero_de_venta(cantidad, comision, tipo);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Pago calculado: %s\n", resultado != NULL ? resultado : "(entrada invalida)");
		int correcto = resultado != NULL;
		sistema_memoria_liberar(cantidad, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(comision, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(tipo, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}

	if(opcion == 4)
	{
		char *cantidad = pm_pedir_texto("Monto de la venta: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *comisiones = pm_pedir_texto(
			"Comisiones separadas por | (ejemplo: 10|5|2): ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *tipo = pm_pedir_texto("Tipo (PORCENTAJE o DINERO): ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *resultado = entrada_dinero_simple(
			cantidad, comisiones, tipo, GG_caracter_separacion[0]);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Pagos: %s\n", resultado != NULL ? resultado : "(entrada invalida)");
		int correcto = resultado != NULL;
		sistema_memoria_liberar(cantidad, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(comisiones, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(tipo, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}

	if(opcion == 5)
	{
		char *ruta = pm_pedir_texto("Ruta de AFILIADOS_UNIFICADO.TXT: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *id = pm_pedir_texto("ID inicial: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *proyecto = pm_pedir_texto("Proyecto/tabla: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		int profundidad = 0;
		int correcto = pm_pedir_entero("Cantidad maxima de niveles: ",
			&profundidad, error_o_datos_a_retornar, nivel_de_profundidad);
		char *cadena = correcto ? extraer_patrosinadores_funcion_recursiva(
			ruta, id, proyecto, profundidad, GG_caracter_separacion[0],
			error_o_datos_a_retornar, nivel_de_profundidad) : NULL;
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Patrocinadores ascendentes: %s\n",
			cadena != NULL ? cadena : "(no se pudo consultar)");
		correcto = cadena != NULL;
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(id, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(proyecto, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(cadena, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}

	if(opcion == 6)
	{
		char *dinero = pm_pedir_texto("Monto de la venta: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *simples = pm_pedir_texto("Porcentajes simples separados por |: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *complejos = pm_pedir_texto("Porcentajes complejos separados por |: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *comision_venta = pm_pedir_texto(
			"Porcentaje base para la venta compleja: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *resultado = calc_din_por_enc_y_total(
			dinero, simples, complejos, comision_venta, GG_caracter_separacion[0]);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Pagos simples | pagos complejos | total: %s\n",
			resultado != NULL ? resultado : "(entrada invalida)");
		int correcto = resultado != NULL;
		sistema_memoria_liberar(dinero, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(simples, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(complejos, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(comision_venta, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}

	char *texto = pm_pedir_texto("Texto separado por |: ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	int veces = 0;
	int correcto = pm_pedir_entero("Repeticiones por elemento: ",
		&veces, error_o_datos_a_retornar, nivel_de_profundidad) && veces >= 0;
	char *separador_salida = pm_pedir_texto("Separador entre repeticiones: ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	char **resultado = correcto && texto != NULL
		? acumulador_de_strings(texto, veces, GG_caracter_separacion[0],
			separador_salida) : NULL;
	if(resultado != NULL)
	{
		for(size_t i = 0; resultado[i] != NULL; ++i)
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"[%d] %s\n", (int)i, resultado[i]);
	}
	else
	{
		correcto = 0;
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"No se pudo crear el arreglo de resultados.\n");
	}
	if(resultado != NULL)
	{
		for(size_t i = 0; resultado[i] != NULL; ++i)
			sistema_memoria_liberar(resultado[i], error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
	}
	sistema_memoria_liberar(texto, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(separador_salida, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

static int pruebas_leer_campos(
	const char **prompts,
	int cantidad,
	char ***campos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	*campos = sistema_memoria_reservar(
		(size_t)cantidad * sizeof(**campos),
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(*campos == NULL) return 0;
	for(int i = 0; i < cantidad; ++i) (*campos)[i] = NULL;
	for(int i = 0; i < cantidad; ++i)
	{
		(*campos)[i] = pm_pedir_texto(
			prompts[i], error_o_datos_a_retornar, nivel_de_profundidad);
		if((*campos)[i] == NULL) return 0;
	}
	return 1;
}

static void pruebas_liberar_campos(
	char **campos,
	int cantidad,
	char *error_o_datos_a_retornar)
{
	if(campos == NULL) return;
	for(int i = 0; i < cantidad; ++i)
		sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
}

static int pruebas_campo_serializable(const char *texto)
{
	return texto != NULL &&
		strstr(texto, GG_caracter_separacion[2]) == NULL &&
		strstr(texto, GG_caracter_separacion_funciones_espesificas[0]) == NULL &&
		strstr(texto, GG_caracter_para_transferencia_entre_archivos[1]) == NULL &&
		strchr(texto, '\r') == NULL && strchr(texto, '\n') == NULL;
}

static char *pruebas_serializar(
	int opcion,
	char **campos,
	int cantidad,
	char *error_o_datos_a_retornar)
{
	char numero[32];
	char *serializado = NULL;
	if(enteroATexto(opcion, numero, sizeof(numero)) == NULL) return NULL;
	serializado = pm_copiar_texto(numero);
	if(serializado == NULL) return NULL;
	for(int i = 0; i < cantidad; ++i)
	{
		if(!pruebas_campo_serializable(campos[i]))
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"El valor contiene un separador reservado del comando; vuelve al menú y reintenta sin ese carácter.\n");
			sistema_memoria_liberar(serializado, error_o_datos_a_retornar, 0);
			return NULL;
		}
		char *siguiente = concatenar(
			"%s%s%s", serializado, GG_caracter_separacion[2], campos[i]);
		sistema_memoria_liberar(serializado, error_o_datos_a_retornar, 0);
		serializado = siguiente;
		if(serializado == NULL) return NULL;
	}
	return serializado;
}

static char *pruebas_recopilar_espacios(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PRUEBAS DE ESPACIOS Y PERMISOS ===\n"
		"1. Crear un espacio real y sus archivos base\n"
		"2. Verificar permisos de un usuario\n"
		"3. Crear archivos base de negocio\n"
		"4. Crear estructura base de inventario\n"
		"5. Volver\n");
	if(!pm_pedir_entero("Seleccione una opción: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad)) return NULL;
	if(opcion == 5) return pm_copiar_texto("0");
	if(opcion < 1 || opcion > 4) return NULL;

	const char *crear[] = {
		"Nombre del nuevo espacio: ",
		"Usuario administrador: ",
		"Contraseña (se escribe visible): ",
		"Escriba SI para confirmar la creación: "
	};
	const char *permisos[] = {
		"Ruta del archivo de permisos: ",
		"ID/nombre del espacio: ",
		"Usuario: ",
		"Contraseña (se escribe visible): ",
		"Nivel mínimo requerido (entero): "
	};
	const char *archivos[] = {
		"Directorio del espacio o ID/nombre (no el archivo .txt): ",
		"Escriba SI para confirmar la creación: "
	};
	const char *estructura[] = {
		"Nombre descriptivo de la estructura: ",
		"Ruta del archivo del espacio: ",
		"Escriba SI para confirmar la creación: "
	};
	const char **prompts = opcion == 1 ? crear :
		(opcion == 2 ? permisos : (opcion == 3 ? archivos : estructura));
	int cantidad = opcion == 1 ? 4 : (opcion == 2 ? 5 : (opcion == 3 ? 2 : 3));
	char **campos = NULL;
	if(!pruebas_leer_campos(prompts, cantidad, &campos,
		error_o_datos_a_retornar, nivel_de_profundidad))
	{
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return NULL;
	}
	if((opcion == 1 || opcion == 3 || opcion == 4) &&
		strcmp(campos[cantidad - 1], "SI") != 0 &&
		strcmp(campos[cantidad - 1], "si") != 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Operación cancelada.\n");
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return pm_copiar_texto("0");
	}
	char *resultado = pruebas_serializar(
		opcion, campos, cantidad, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
	return resultado;
}

static char *pruebas_recopilar_proceso_mul(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PROCESO DE AFILIADOS ===\n"
		"1. Agregar usuario nuevo a AFILIADOS_UNIFICADO\n"
		"2. Registrar/actualizar relación de afiliado\n"
		"3. Calcular comisión de una venta\n"
		"4. Calcular comisiones simples\n"
		"5. Buscar cadena de patrocinadores\n"
		"6. Calcular pagos simples, complejos y total\n"
		"7. Repetir elementos de una lista de texto\n"
		"8. Agregar usuario nuevo con encargado\n"
		"9. Volver\n");
	if(!pm_pedir_entero("Seleccione una opción: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad)) return NULL;
	if(opcion == 9) return pm_copiar_texto("0");
	if(opcion < 1 || opcion > 8) return NULL;

	char **campos = NULL;
	int cantidad = 0;
	if(opcion == 1 || opcion == 8)
	{
		int columnas = 0;
		char **nombres = split(GG_secciones_negocio[4].columnas,
			GG_caracter_separacion[1], &columnas,
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(nombres == NULL || columnas != 44)
		{
			liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
			return NULL;
		}
		int inicio_datos = opcion == 8 ? 3 : 1;
		int ultima_columna = opcion == 8 ? 32 : 44;
		cantidad = opcion == 8 ? 35 : 45;
		campos = sistema_memoria_reservar(
			(size_t)cantidad * sizeof(*campos),
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(campos != NULL)
		{
			for(int i = 0; i < cantidad; ++i) campos[i] = NULL;
			campos[0] = pm_pedir_texto(
				"Ruta de AFILIADOS_UNIFICADO.TXT (Enter usa la ruta configurada): ",
				error_o_datos_a_retornar, nivel_de_profundidad);
			if(campos[0] != NULL && campos[0][0] == '\0')
			{
				sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
				campos[0] = pm_copiar_texto(
					"CONFIG/AFILIADOS/AFILIADOS_UNIFICADO.TXT");
			}
			if(opcion == 8)
			{
				campos[1] = pm_pedir_texto(
					"ID del encargado existente: ",
					error_o_datos_a_retornar, nivel_de_profundidad);
				campos[2] = pm_pedir_texto(
					"Proyecto/tabla: ",
					error_o_datos_a_retornar, nivel_de_profundidad);
			}
			for(int i = 1; campos[0] != NULL && i < ultima_columna; ++i)
			{
				char *solicitud = variable_string(
					"Valor para %s (Enter lo deja vacío): ", nombres[i]);
				campos[inicio_datos + i - 1] = solicitud != NULL
					? pm_pedir_texto(solicitud, error_o_datos_a_retornar,
						nivel_de_profundidad)
					: NULL;
				sistema_memoria_liberar(solicitud, error_o_datos_a_retornar, 0);
			}
			campos[cantidad - 1] =
				campos[inicio_datos + ultima_columna - 2] != NULL
				? pm_pedir_texto(
					opcion == 8
						? "Se agregará el usuario con la relación de encargado. Escriba SI para confirmar: "
						: "Se agregará un usuario. Escriba SI para confirmar: ",
					error_o_datos_a_retornar, nivel_de_profundidad)
				: NULL;
		}
		liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
	}
	else
	{
		static const char *prompts2[] = {
			"Ruta de AFILIADOS_UNIFICADO.TXT: ", "ID del afiliado existente: ",
			"Tipo (PATROCINADOR o ENCARGADO): ", "Proyecto/tabla: ",
			"ID relacionado: ", "Puntos del proyecto por recibir: ",
			"Puntos recibidos totales: ", "Nivel: ", "ID horizontal: ",
			"Tipo de afiliado: ", "Escriba SI para confirmar la escritura: "
		};
		static const char *prompts3[] = {
			"Monto de la venta: ", "Comisión (porcentaje o monto): ",
			"Tipo (PORCENTAJE o DINERO): "
		};
		static const char *prompts4[] = {
			"Monto de la venta: ", "Comisiones separadas por | (ejemplo: 10|5|2): ",
			"Tipo (PORCENTAJE o DINERO): "
		};
		static const char *prompts5[] = {
			"Ruta de AFILIADOS_UNIFICADO.TXT: ", "ID inicial: ",
			"Proyecto/tabla: ", "Cantidad máxima de niveles: "
		};
		static const char *prompts6[] = {
			"Monto de la venta: ", "Porcentajes simples separados por |: ",
			"Porcentajes complejos separados por |: ",
			"Porcentaje base para la venta compleja: "
		};
		static const char *prompts7[] = {
			"Texto separado por |: ", "Repeticiones por elemento: ",
			"Separador entre repeticiones: "
		};
		const char **prompts = opcion == 2 ? prompts2 :
			(opcion == 3 ? prompts3 : (opcion == 4 ? prompts4 :
			(opcion == 5 ? prompts5 : (opcion == 6 ? prompts6 : prompts7))));
		cantidad = opcion == 2 ? 11 : (opcion == 3 || opcion == 4 ? 3 :
			(opcion == 5 ? 4 : (opcion == 6 ? 4 : 3)));
		if(!pruebas_leer_campos(prompts, cantidad, &campos,
			error_o_datos_a_retornar, nivel_de_profundidad))
		{
			pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
			return NULL;
		}
	}
	if(campos == NULL) return NULL;
	for(int i = 0; i < cantidad; ++i)
	{
		if(campos[i] == NULL || !pruebas_campo_serializable(campos[i]) ||
			(opcion == 1 && i > 0 && i < 44 &&
				strstr(campos[i], GG_caracter_separacion[1]) != NULL) ||
			(opcion == 8 && i >= 3 && i < 34 &&
				strstr(campos[i], GG_caracter_separacion[1]) != NULL))
		{
			pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
			return NULL;
		}
	}
	if(opcion == 8 &&
		(campos[1][0] == '\0' || campos[2][0] == '\0' ||
			campos[3][0] == '\0'))
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"El ID del encargado, el proyecto/tabla y el nombre del usuario son obligatorios.\n");
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return NULL;
	}
	if((opcion == 1 && strcmp(campos[44], "SI") != 0 &&
		strcmp(campos[44], "si") != 0) ||
		(opcion == 8 && (strcmp(campos[34], "SI") != 0 &&
			strcmp(campos[34], "si") != 0)) ||
		(opcion == 2 && strcmp(campos[10], "SI") != 0 &&
			strcmp(campos[10], "si") != 0))
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Operación cancelada.\n");
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return pm_copiar_texto("0");
	}
	char *resultado = pruebas_serializar(
		opcion, campos, cantidad, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
	return resultado;
}

static char *pruebas_recopilar_ventas(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *prompts[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		"Códigos de barras separados por °: ",
		"Cantidades en el mismo orden, separadas por °: ",
		"Se descontarán existencias y se guardarán registros y rankings. Escriba SI para confirmar: "
	};
	char **campos = NULL;
	if(!pruebas_leer_campos(prompts, 4, &campos,
		error_o_datos_a_retornar, nivel_de_profundidad))
	{
		pruebas_liberar_campos(campos, 4, error_o_datos_a_retornar);
		return NULL;
	}
	if(campos[0][0] == '\0')
	{
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = pm_copiar_texto(
			"CONFIG/INF/INVENTARIO/INVENTARIO.TXT");
	}
	if(campos[0] == NULL)
	{
		pruebas_liberar_campos(campos, 4, error_o_datos_a_retornar);
		return NULL;
	}
	if(strcmp(campos[3], "SI") != 0 && strcmp(campos[3], "si") != 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Venta cancelada; no se modificó el inventario.\n");
		pruebas_liberar_campos(campos, 4, error_o_datos_a_retornar);
		return pm_copiar_texto("0");
	}
	char *resultado = pruebas_serializar(
		1, campos, 4, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, 4, error_o_datos_a_retornar);
	return resultado;
}

static int ventas_parsear_numero(const char *texto, double *valor)
{
	if(texto == NULL || valor == NULL || texto[0] == '\0') return 0;
	char *fin = NULL;
	errno = 0;
	double convertido = strtod(texto, &fin);
	if(errno == ERANGE || fin == texto || *fin != '\0' ||
		convertido != convertido || convertido > DBL_MAX ||
		convertido < -DBL_MAX)
		return 0;
	*valor = convertido;
	return 1;
}

static int ventas_respuesta_exitosa(const char *respuesta)
{
	int cantidad_partes = 0;
	char **partes = respuesta != NULL
		? split(respuesta, GG_caracter_para_confirmacion_o_error[0],
			&cantidad_partes, NULL, 0)
		: NULL;
	int codigo = 0;
	int correcta = partes != NULL && cantidad_partes >= 2 &&
		texto_a_int_seguro(partes[0], &codigo) == 0 && codigo > 0;
	liberarSplit(partes, cantidad_partes, NULL, 0);
	return correcta;
}

static int gg_registros_base_desde_inventario(
	const char *direccion_archivo,
	char **base_out)
{
	if(base_out == NULL) return 0;
	*base_out = NULL;
	if(direccion_archivo == NULL) return 0;
	const char *marcadores[] = {
		"/CONFIG/INF/INVENTARIO/INVENTARIO.TXT",
		"\\CONFIG\\INF\\INVENTARIO\\INVENTARIO.TXT",
		"CONFIG/INF/INVENTARIO/INVENTARIO.TXT",
		"CONFIG\\INF\\INVENTARIO\\INVENTARIO.TXT"
	};
	const char *encontrado = NULL;
	for(size_t i = 0; i < sizeof(marcadores) / sizeof(marcadores[0]); ++i)
	{
		encontrado = strstr(direccion_archivo, marcadores[i]);
		if(encontrado != NULL)
			break;
	}
	if(encontrado == NULL) return 0;
	size_t largo_base = (size_t)(encontrado - direccion_archivo);
	*base_out = variable_string("%.*s", (int)largo_base, direccion_archivo);
	return *base_out != NULL;
}

static char *gg_registros_unir_ruta(const char *base, const char *relativa)
{
	if(base == NULL || relativa == NULL) return NULL;
	if(base[0] == '\0') return variable_string("%s", relativa);
	size_t largo = strlen(base);
	const char *separador = base[largo - 1] == '/' ||
		base[largo - 1] == '\\' ? "" : "/";
	return variable_string("%s%s%s", base, separador, relativa);
}

static char *gg_registros_ruta_fecha(
	const char *base,
	int seccion,
	const char *fecha)
{
	if(base == NULL || fecha == NULL ||
		(seccion != 9 && seccion != 13 && strlen(fecha) < 8))
		return NULL;
	char *relativa = NULL;
	switch(seccion)
	{
		case 6:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s/%.6s/%.8s_REGISTRO.TXT",
				fecha, fecha, fecha);
			break;
		case 7:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s/%.6s_REGISTRO.TXT",
				fecha, fecha);
			break;
		case 8:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s_REGISTRO.TXT", fecha);
			break;
		case 9:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/ACUMULADO_REGISTRO.TXT");
			break;
		case 10:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s/%.6s/%.8s_PRODUC_REGISTRO.TXT",
				fecha, fecha, fecha);
			break;
		case 11:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s/%.6s_PRODUC_REGISTRO.TXT",
				fecha, fecha);
			break;
		case 12:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/FECHAS/%.4s_PRODUC_REGISTRO.TXT",
				fecha);
			break;
		case 13:
			relativa = variable_string(
				"CONFIG/INF/REGISTROS/ACUMULADO_PRODUC_REGISTRO.TXT");
			break;
		default:
			return NULL;
	}
	if(relativa == NULL) return NULL;
	char *ruta = gg_registros_unir_ruta(base, relativa);
	sistema_memoria_liberar(relativa, NULL, 0);
	return ruta;
}

static int gg_registros_guardar_archivo(
	const char *ruta,
	char **lineas,
	int cantidad_lineas)
{
	guardar_archivo(ruta, lineas, cantidad_lineas);
	int cantidad_verificacion = 0;
	char **verificacion = leer_archivo(ruta, &cantidad_verificacion);
	int correcto = verificacion != NULL &&
		cantidad_verificacion == cantidad_lineas;
	for(int i = 0; correcto && i < cantidad_lineas; ++i)
		if(strcmp(verificacion[i], lineas[i]) != 0) correcto = 0;
	free_lineas(verificacion, cantidad_verificacion);
	return correcto;
}

static int gg_registros_numero_opcional(const char *texto, double *valor)
{
	if(texto == NULL || texto[0] == '\0')
	{
		*valor = 0.0;
		return 1;
	}
	return ventas_parsear_numero(texto, valor);
}

static int gg_registros_calcular_impuestos(
	const char *direccion_archivo,
	const char *ids_impuestos,
	double base_imponible,
	double *total_impuestos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(total_impuestos == NULL || base_imponible < 0.0) return 0;
	*total_impuestos = 0.0;
	if(ids_impuestos == NULL || ids_impuestos[0] == '\0') return 1;
	char *base = NULL;
	if(!gg_registros_base_desde_inventario(direccion_archivo, &base))
		return 0;
	char *ruta_impuestos = base[0] != '\0'
		? gg_registros_unir_ruta(base, GG_secciones_negocio[14].ruta)
		: variable_string("%s", GG_secciones_negocio[14].ruta);
	sistema_memoria_liberar(base, error_o_datos_a_retornar, 0);
	if(ruta_impuestos == NULL) return 0;
	int cantidad_lineas = 0;
	char **lineas = leer_archivo(ruta_impuestos, &cantidad_lineas);
	sistema_memoria_liberar(ruta_impuestos, error_o_datos_a_retornar, 0);
	if(lineas == NULL) return 0;
	int cantidad_ids = 0;
	char **ids = split(ids_impuestos, GG_caracter_separacion[1],
		&cantidad_ids, error_o_datos_a_retornar, nivel_de_profundidad);
	int correcto = ids != NULL;
	for(int i = 0; correcto && i < cantidad_ids; ++i)
	{
		if(ids[i][0] == '\0') continue;
		int repetido = 0;
		for(int previo = 0; previo < i; ++previo)
			if(strcmp(ids[previo], ids[i]) == 0) repetido = 1;
		if(repetido) continue;
		int encontrado = 0;
		for(int j = 1; j < cantidad_lineas; ++j)
		{
			int cantidad_campos = 0;
			char **campos = split(lineas[j], GG_caracter_separacion[1],
				&cantidad_campos, error_o_datos_a_retornar,
				nivel_de_profundidad);
			if(campos != NULL && cantidad_campos >= 3 &&
				strcmp(campos[0], ids[i]) == 0)
			{
				double porcentaje = 0.0;
				if(ventas_parsear_numero(campos[2], &porcentaje) &&
					porcentaje >= 0.0)
				{
					double fraccion = porcentaje / 100.0;
					if(fraccion == 0.0 ||
						base_imponible <= DBL_MAX / fraccion)
					{
						double monto = base_imponible * fraccion;
						if(monto <= DBL_MAX - *total_impuestos)
						{
							*total_impuestos += monto;
							encontrado = 1;
						}
					}
				}
			}
			liberarSplit(campos, cantidad_campos,
				error_o_datos_a_retornar, 0);
			if(encontrado) break;
		}
		if(!encontrado) correcto = 0;
	}
	liberarSplit(ids, cantidad_ids, error_o_datos_a_retornar, 0);
	free_lineas(lineas, cantidad_lineas);
	return correcto;
}

static int gg_registros_campo_serializable(const char *texto)
{
	return texto != NULL &&
		strstr(texto, GG_caracter_separacion[1]) == NULL &&
		strstr(texto, GG_caracter_separacion[2]) == NULL &&
		strchr(texto, '\n') == NULL && strchr(texto, '\r') == NULL;
}

static int gg_registros_agregar_detalle(
	char **lista,
	const char *codigo,
	const char *importe,
	const char *nombre,
	const char *cantidad,
	const char *proveedores,
	const char *tipo,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(lista == NULL || !gg_registros_campo_serializable(codigo) ||
		!gg_registros_campo_serializable(importe) ||
		!gg_registros_campo_serializable(nombre) ||
		!gg_registros_campo_serializable(cantidad) ||
		!gg_registros_campo_serializable(proveedores) ||
		!gg_registros_campo_serializable(tipo))
		return 0;
	char *campos[] = {
		(char *)codigo, (char *)importe, (char *)nombre,
		(char *)cantidad, (char *)proveedores, (char *)tipo
	};
	char *detalle = join(campos, 6, GG_caracter_separacion[2],
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(detalle == NULL) return 0;
	char *nuevo = (*lista == NULL || (*lista)[0] == '\0')
		? variable_string("%s", detalle)
		: variable_string("%s%s%s", *lista,
			GG_caracter_separacion[1], detalle);
	sistema_memoria_liberar(detalle, error_o_datos_a_retornar, 0);
	if(nuevo == NULL) return 0;
	sistema_memoria_liberar(*lista, error_o_datos_a_retornar, 0);
	*lista = nuevo;
	return 1;
}

static char *gg_registros_crear_fila(
	const char *ruta,
	const char *cabecera,
	const char *campos)
{
	if(crearArchivo(ruta, cabecera) < 0) return NULL;
	int cantidad_lineas = 0;
	char **lineas = leer_archivo(ruta, &cantidad_lineas);
	if(lineas == NULL) return NULL;
	char *fila = variable_string("%d%s%s",
		cantidad_lineas, GG_caracter_separacion[1], campos);
	free_lineas(lineas, cantidad_lineas);
	if(fila == NULL) return NULL;
	char *respuesta = escribirLinea(ruta, fila, NULL, 0);
	int partes_respuesta = 0;
	char **respuesta_separada = respuesta != NULL
		? split(respuesta, GG_caracter_separacion[1],
			&partes_respuesta, NULL, 0)
		: NULL;
	int codigo_respuesta = 0;
	int agregado = respuesta_separada != NULL &&
		partes_respuesta >= 2 &&
		texto_a_int_seguro(respuesta_separada[1], &codigo_respuesta) == 0 &&
		codigo_respuesta > 0;
	liberarSplit(respuesta_separada, partes_respuesta, NULL, 0);
	sistema_memoria_liberar(respuesta, NULL, 0);
	sistema_memoria_liberar(fila, NULL, 0);
	return agregado
		? variable_string("1%sregistro_guardado",
			GG_caracter_para_confirmacion_o_error[0])
		: NULL;
}

static int gg_registros_acumular_resumen(
	const char *base,
	int seccion,
	const char *fecha,
	const char *periodo,
	const char *operacion,
	double venta,
	double costo,
	double impuestos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	char *ruta = gg_registros_ruta_fecha(base, seccion, fecha);
	if(ruta == NULL) return 0;
	if(crearArchivo(ruta, GG_secciones_negocio[seccion].columnas) < 0)
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		return 0;
	}
	int cantidad_lineas = 0;
	char **lineas = leer_archivo(ruta, &cantidad_lineas);
	if(lineas == NULL)
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		return 0;
	}
	int indice_fila = -1;
	for(int i = 1; i < cantidad_lineas; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(lineas[i], GG_caracter_separacion[1],
			&cantidad_campos, error_o_datos_a_retornar,
			nivel_de_profundidad);
		int coincide = campos != NULL && cantidad_campos >= 9 &&
			strcmp(campos[1], periodo) == 0 &&
			strcmp(campos[2], operacion) == 0;
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
		if(coincide)
		{
			indice_fila = i;
			break;
		}
	}

	char *resultado_fila = NULL;
	int fila_actualizada = 0;
	if(indice_fila >= 0)
	{
		int cantidad_campos = 0;
		char **campos = split(lineas[indice_fila],
			GG_caracter_separacion[1], &cantidad_campos,
			error_o_datos_a_retornar, nivel_de_profundidad);
		double venta_actual = 0.0, costo_actual = 0.0;
		double impuesto_actual = 0.0, impuestos_actuales = 0.0;
		int valido = campos != NULL && cantidad_campos >= 9 &&
			gg_registros_numero_opcional(campos[3], &impuesto_actual) &&
			gg_registros_numero_opcional(campos[5], &venta_actual) &&
			gg_registros_numero_opcional(campos[6], &costo_actual) &&
			gg_registros_numero_opcional(campos[7], &impuestos_actuales) &&
			impuestos <= DBL_MAX - impuesto_actual &&
			venta <= DBL_MAX - venta_actual &&
			costo <= DBL_MAX - costo_actual &&
			impuestos <= DBL_MAX - impuestos_actuales;
		if(valido)
		{
			char *texto_impuesto = variable_string("%.17g",
				impuesto_actual + impuestos);
			char *texto_venta = variable_string("%.17g", venta_actual + venta);
			char *texto_costo = variable_string("%.17g", costo_actual + costo);
			char *texto_impuestos = variable_string("%.17g",
				impuestos_actuales + impuestos);
			char *texto_ganancia = variable_string("%.17g",
				venta_actual + venta - costo_actual - costo -
				impuestos_actuales - impuestos);
			if(texto_impuesto != NULL && texto_venta != NULL && texto_costo != NULL &&
				texto_impuestos != NULL && texto_ganancia != NULL)
			{
				sistema_memoria_liberar(campos[3], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[5], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[6], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[7], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[8], error_o_datos_a_retornar, 0);
				campos[3] = texto_impuesto;
				campos[5] = texto_venta;
				campos[6] = texto_costo;
				campos[7] = texto_impuestos;
				campos[8] = texto_ganancia;
				resultado_fila = join(campos, cantidad_campos,
					GG_caracter_separacion[1], error_o_datos_a_retornar,
					nivel_de_profundidad);
			}
			else
			{
				sistema_memoria_liberar(texto_impuesto, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(texto_venta, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(texto_costo, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(texto_impuestos, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(texto_ganancia, error_o_datos_a_retornar, 0);
			}
		}
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
		if(resultado_fila != NULL)
		{
			sistema_memoria_liberar(lineas[indice_fila],
				error_o_datos_a_retornar, 0);
			lineas[indice_fila] = resultado_fila;
			resultado_fila = NULL;
			fila_actualizada = 1;
		}
	}
	else
	{
		char *texto_impuestos = variable_string("%.17g", impuestos);
		char *texto_venta = variable_string("%.17g", venta);
		char *texto_costo = variable_string("%.17g", costo);
		char *texto_ganancia = variable_string("%.17g",
			venta - costo - impuestos);
		char *campos_fila[8] = {
			(char *)periodo, (char *)operacion, texto_impuestos,
			"REGISTRO_AUTOMATICO", texto_venta, texto_costo,
			texto_impuestos, texto_ganancia
		};
		char *campos = texto_impuestos != NULL && texto_venta != NULL &&
			texto_costo != NULL && texto_ganancia != NULL
			? join(campos_fila, 8, GG_caracter_separacion[1],
				error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		if(campos != NULL)
		{
			char *respuesta = gg_registros_crear_fila(ruta,
				GG_secciones_negocio[seccion].columnas, campos);
			if(ventas_respuesta_exitosa(respuesta))
				fila_actualizada = 1;
			sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
		}
		sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(texto_impuestos, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(texto_venta, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(texto_costo, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(texto_ganancia, error_o_datos_a_retornar, 0);
	}

	int guardado = 0;
	if(indice_fila >= 0)
		guardado = fila_actualizada &&
			gg_registros_guardar_archivo(ruta, lineas, cantidad_lineas);
	else
		guardado = fila_actualizada;
	sistema_memoria_liberar(resultado_fila, error_o_datos_a_retornar, 0);
	free_lineas(lineas, cantidad_lineas);
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	return guardado;
}

static int gg_registros_actualizar_producto(
	const char *base,
	int seccion,
	const char *fecha,
	const char *codigo,
	const char *nombre,
	const char *cantidad,
	const char *proveedores,
	const char *tipo,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	char *ruta = gg_registros_ruta_fecha(base, seccion, fecha);
	if(ruta == NULL) return 0;
	if(crearArchivo(ruta, GG_secciones_negocio[seccion].columnas) < 0)
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		return 0;
	}
	int n_lineas = 0;
	char **lineas = leer_archivo(ruta, &n_lineas);
	if(lineas == NULL)
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		return 0;
	}
	int indice = -1;
	for(int i = 1; i < n_lineas; ++i)
	{
		int n_campos = 0;
		char **campos = split(lineas[i], GG_caracter_separacion[1],
			&n_campos, error_o_datos_a_retornar, nivel_de_profundidad);
		int coincide = campos != NULL && n_campos >= 4 &&
			strcmp(campos[3], codigo) == 0;
		liberarSplit(campos, n_campos, error_o_datos_a_retornar, 0);
		if(coincide)
		{
			indice = i;
			break;
		}
	}
	int guardado = 0;
	if(indice >= 0)
	{
		int n_campos = 0;
		char **campos = split(lineas[indice], GG_caracter_separacion[1],
			&n_campos, error_o_datos_a_retornar, nivel_de_profundidad);
		double anterior = 0.0, incremento = 0.0;
		if(campos != NULL && n_campos >= 12 &&
			gg_registros_numero_opcional(campos[2], &anterior) &&
			ventas_parsear_numero(cantidad, &incremento))
		{
			char *texto_cantidad = variable_string("%.17g", anterior + incremento);
			if(texto_cantidad != NULL)
			{
				char *texto_historial = variable_string("%.17g",
					anterior + incremento);
				char *texto_ranking = variable_string("%.17g",
					anterior + incremento);
				char *texto_promedio = variable_string("%.17g",
					anterior + incremento);
				if(texto_historial == NULL || texto_ranking == NULL ||
					texto_promedio == NULL)
				{
					sistema_memoria_liberar(texto_cantidad,
						error_o_datos_a_retornar, 0);
					sistema_memoria_liberar(texto_historial,
						error_o_datos_a_retornar, 0);
					sistema_memoria_liberar(texto_ranking,
						error_o_datos_a_retornar, 0);
					sistema_memoria_liberar(texto_promedio,
						error_o_datos_a_retornar, 0);
					liberarSplit(campos, n_campos,
						error_o_datos_a_retornar, 0);
					free_lineas(lineas, n_lineas);
					sistema_memoria_liberar(ruta,
						error_o_datos_a_retornar, 0);
					return 0;
				}
				sistema_memoria_liberar(campos[2], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[5], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[6], error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(campos[7], error_o_datos_a_retornar, 0);
				campos[2] = texto_cantidad;
				campos[5] = texto_historial;
				campos[6] = texto_ranking;
				campos[7] = texto_promedio;
				char *fila = join(campos, n_campos,
					GG_caracter_separacion[1], error_o_datos_a_retornar,
					nivel_de_profundidad);
				if(fila != NULL)
				{
					sistema_memoria_liberar(lineas[indice],
						error_o_datos_a_retornar, 0);
					lineas[indice] = fila;
					guardado = gg_registros_guardar_archivo(
						ruta, lineas, n_lineas);
				}
			}
		}
		liberarSplit(campos, n_campos, error_o_datos_a_retornar, 0);
	}
	else
	{
		char *inicial = variable_string("%s", cantidad);
		char *campos[11] = {
			(char *)nombre, (char *)cantidad, (char *)codigo,
			(char *)(proveedores != NULL ? proveedores : ""),
			inicial, inicial, inicial, "4", "0",
			(char *)(tipo != NULL ? tipo : ""), "0"
		};
		char *datos = inicial != NULL
			? join(campos, 11, GG_caracter_separacion[1],
				error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		char *respuesta = datos != NULL
			? gg_registros_crear_fila(ruta,
				GG_secciones_negocio[seccion].columnas, datos)
			: NULL;
		guardado = ventas_respuesta_exitosa(respuesta);
		sistema_memoria_liberar(inicial, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(datos, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	}
	free_lineas(lineas, n_lineas);
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	return guardado;
}

static int gg_registros_registrar_operacion(
	const char *direccion_archivo,
	const char *fecha_hora,
	const char *operacion,
	double venta,
	double costo,
	double impuestos,
	const char *productos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(fecha_hora == NULL || strlen(fecha_hora) < 10 ||
		(operacion == NULL) || (productos == NULL))
		return 0;
	char *base = NULL;
	if(!gg_registros_base_desde_inventario(direccion_archivo, &base))
		return 0;
	char *ruta_diaria = gg_registros_ruta_fecha(base, 6, fecha_hora);
	char *texto_venta = variable_string("%.17g", venta);
	char *texto_costo = variable_string("%.17g", costo);
	char *texto_impuestos = variable_string("%.17g", impuestos);
	char *campos_diarios_lista[10] = {
		(char *)(fecha_hora + 8), (char *)operacion, texto_impuestos,
		(char *)productos, "REGISTRO_AUTOMATICO", texto_venta,
		texto_costo, texto_impuestos, "0", ""
	};
	char *campos_diarios = texto_venta != NULL && texto_costo != NULL &&
		texto_impuestos != NULL
		? join(campos_diarios_lista, 10, GG_caracter_separacion[1],
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	char *respuesta = ruta_diaria != NULL && campos_diarios != NULL
		? gg_registros_crear_fila(ruta_diaria,
			GG_secciones_negocio[6].columnas, campos_diarios)
		: NULL;
	int correcto = ventas_respuesta_exitosa(respuesta);
	sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(campos_diarios, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(ruta_diaria, error_o_datos_a_retornar, 0);
	if(correcto)
	{
		char dia[3] = {fecha_hora[6], fecha_hora[7], '\0'};
		char mes[3] = {fecha_hora[4], fecha_hora[5], '\0'};
		char anio[5] = {fecha_hora[0], fecha_hora[1],
			fecha_hora[2], fecha_hora[3], '\0'};
		correcto = gg_registros_acumular_resumen(base, 7, fecha_hora, dia,
			operacion, venta, costo, impuestos, error_o_datos_a_retornar,
			nivel_de_profundidad) &&
			gg_registros_acumular_resumen(base, 8, fecha_hora, mes,
				operacion, venta, costo, impuestos,
				error_o_datos_a_retornar, nivel_de_profundidad) &&
			gg_registros_acumular_resumen(base, 9, anio, anio,
				operacion, venta, costo, impuestos,
				error_o_datos_a_retornar, nivel_de_profundidad);
	}
	if(correcto && strcmp(operacion, "VENTA") == 0 && productos[0] != '\0')
	{
		int n_productos = 0;
		char **lista = split(productos, GG_caracter_separacion[1],
			&n_productos, error_o_datos_a_retornar, nivel_de_profundidad);
		if(lista == NULL) correcto = 0;
		for(int i = 0; correcto && i < n_productos; ++i)
		{
			int n_campos = 0;
			char **item = split(lista[i], GG_caracter_separacion[2],
				&n_campos, error_o_datos_a_retornar, nivel_de_profundidad);
			double cantidad = 0.0;
			char *cantidad_texto = NULL;
			if(item == NULL || n_campos < 6 ||
				!ventas_parsear_numero(item[3], &cantidad))
			{
				correcto = 0;
			}
			else
			{
				cantidad_texto = variable_string("%.17g", cantidad);
				for(int seccion = 10; correcto && seccion <= 13; ++seccion)
					correcto = cantidad_texto != NULL &&
						gg_registros_actualizar_producto(base, seccion,
							fecha_hora, item[0], item[2], cantidad_texto,
							item[4], item[5], error_o_datos_a_retornar,
							nivel_de_profundidad);
			}
			sistema_memoria_liberar(cantidad_texto,
				error_o_datos_a_retornar, 0);
			liberarSplit(item, n_campos, error_o_datos_a_retornar, 0);
		}
		liberarSplit(lista, n_productos, error_o_datos_a_retornar, 0);
	}
	sistema_memoria_liberar(texto_venta, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(texto_costo, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(texto_impuestos, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(base, error_o_datos_a_retornar, 0);
	return correcto;
}

char *ventas(
	const char *direccion_archivo,
	const char *codigos,
	const char *cantidades,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(direccion_archivo == NULL || direccion_archivo[0] == '\0' ||
		codigos == NULL || cantidades == NULL)
		return crearResultado(-1, "parametros_de_venta_invalidos", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);

	int cantidad_codigos = 0;
	int cantidad_cantidades = 0;
	char **lista_codigos = split(codigos, GG_caracter_separacion[1],
		&cantidad_codigos, error_o_datos_a_retornar, nivel_de_profundidad);
	char **lista_cantidades = split(cantidades, GG_caracter_separacion[1],
		&cantidad_cantidades, error_o_datos_a_retornar, nivel_de_profundidad);
	if(lista_codigos == NULL || lista_cantidades == NULL ||
		cantidad_codigos <= 0 || cantidad_codigos != cantidad_cantidades)
	{
		liberarSplit(lista_codigos, cantidad_codigos,
			error_o_datos_a_retornar, 0);
		liberarSplit(lista_cantidades, cantidad_cantidades,
			error_o_datos_a_retornar, 0);
		return crearResultado(-1, "codigos_y_cantidades_no_coinciden", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	int actualizados = 0;
	int omitidos = 0;
	int errores_registro_inventario = 0;
	int error_detalle_registro = 0;
	double total_costo_venta = 0.0;
	double total_impuestos_venta = 0.0;
	double total_venta = 0.0;
	char *productos_registro = variable_string("%s", "");
	char fecha_hora[20];
	fechaActual(fecha_hora, "%Y%m%d%H");
	if(fecha_hora[0] == '\0' || productos_registro == NULL)
	{
		sistema_memoria_liberar(productos_registro,
			error_o_datos_a_retornar, 0);
		liberarSplit(lista_codigos, cantidad_codigos,
			error_o_datos_a_retornar, 0);
		liberarSplit(lista_cantidades, cantidad_cantidades,
			error_o_datos_a_retornar, 0);
		return crearResultado(-1, "no_se_pudo_obtener_fecha_de_venta", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	for(int i = 0; i < cantidad_codigos; ++i)
	{
		double cantidad_solicitada = 0.0;
		if(lista_codigos[i][0] == '\0' ||
			!ventas_parsear_numero(lista_cantidades[i], &cantidad_solicitada) ||
			cantidad_solicitada <= 0.0)
		{
			++omitidos;
			continue;
		}

		char *respuesta_busqueda = buscar_info_dividida(
			direccion_archivo, lista_codigos[i], 5, -1);
		int cantidad_partes = 0;
		char **partes = respuesta_busqueda != NULL
			? split(respuesta_busqueda,
				GG_caracter_para_confirmacion_o_error[0],
				&cantidad_partes, error_o_datos_a_retornar,
				nivel_de_profundidad)
			: NULL;
		if(partes == NULL || cantidad_partes < 3 ||
			strcmp(partes[0], "1") != 0)
		{
			++omitidos;
			liberarSplit(partes, cantidad_partes,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta_busqueda,
				error_o_datos_a_retornar, 0);
			continue;
		}

		int columnas_producto = 0;
		char **producto = split(partes[1], GG_caracter_separacion[0],
			&columnas_producto, error_o_datos_a_retornar,
			nivel_de_profundidad);
		if(producto == NULL || columnas_producto <= 18 ||
			(strcmp(producto[9], "PRODUCTO_PIEZA") != 0 &&
				strcmp(producto[9], "PRODUCTO_CANTIDAD") != 0 &&
				strcmp(producto[9], "PRODUCTO_ELABORADO") != 0))
		{
			++omitidos;
			liberarSplit(producto, columnas_producto,
				error_o_datos_a_retornar, 0);
			liberarSplit(partes, cantidad_partes,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta_busqueda,
				error_o_datos_a_retornar, 0);
			continue;
		}

		double stock = 0.0;
		double precio = 0.0;
		double costo_unitario = 0.0;
		if(!ventas_parsear_numero(producto[6], &stock) ||
			!ventas_parsear_numero(producto[4], &precio) ||
			stock < cantidad_solicitada || stock < 0.0 || precio < 0.0)
		{
			++omitidos;
			liberarSplit(producto, columnas_producto,
				error_o_datos_a_retornar, 0);
			liberarSplit(partes, cantidad_partes,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta_busqueda,
				error_o_datos_a_retornar, 0);
			continue;
		}

		char *columnas_actualizar = concatenar(
			"%s%s%s", "6",
			GG_caracter_separacion_funciones_espesificas[0], "18");
		char *valores_actualizar = variable_string(
			"%.17g%s%s", stock - cantidad_solicitada,
			GG_caracter_separacion_funciones_espesificas[0], fecha_hora);
		char *respuesta_edicion = columnas_actualizar != NULL &&
			valores_actualizar != NULL
			? editar_celda_id_info_dividida(direccion_archivo, partes[2],
				columnas_actualizar, valores_actualizar)
			: NULL;
		int editado = ventas_respuesta_exitosa(respuesta_edicion);
		if(editado)
		{
			++actualizados;
			double importe_producto = cantidad_solicitada * precio;
			if(precio > 0.0 && cantidad_solicitada > DBL_MAX / precio)
				error_detalle_registro = 1;
			else
			{
				if(importe_producto > DBL_MAX - total_venta)
					error_detalle_registro = 1;
				else
					total_venta += importe_producto;
				double impuesto_producto = 0.0;
				if(!gg_registros_calcular_impuestos(
					direccion_archivo, producto[15], importe_producto,
					&impuesto_producto, error_o_datos_a_retornar,
					nivel_de_profundidad) ||
					impuesto_producto > DBL_MAX - total_impuestos_venta)
					error_detalle_registro = 1;
				else
					total_impuestos_venta += impuesto_producto;
				if(ventas_parsear_numero(producto[7], &costo_unitario) &&
					costo_unitario >= 0.0 &&
					(costo_unitario == 0.0 ||
						cantidad_solicitada <= DBL_MAX / costo_unitario))
				{
					double importe_costo = cantidad_solicitada * costo_unitario;
					if(importe_costo <= DBL_MAX - total_costo_venta)
						total_costo_venta += importe_costo;
					else
						error_detalle_registro = 1;
				}
				else
				{
					error_detalle_registro = 1;
				}
				char *nombre = variable_string("%s %s %s",
					producto[1], producto[2], producto[3]);
				char *cantidad_texto = variable_string("%.17g",
					cantidad_solicitada);
				char *importe_texto = variable_string("%.17g",
					importe_producto);
				if(nombre == NULL || cantidad_texto == NULL ||
					importe_texto == NULL ||
					!gg_registros_agregar_detalle(&productos_registro,
						lista_codigos[i], importe_texto, nombre,
						cantidad_texto, producto[8], producto[9],
						error_o_datos_a_retornar, nivel_de_profundidad))
					error_detalle_registro = 1;
				sistema_memoria_liberar(nombre, error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(cantidad_texto,
					error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(importe_texto,
					error_o_datos_a_retornar, 0);
			}
			if(GG_ruta_sesion_inventario_activa != NULL)
			{
				char *cantidad_texto = variable_string(
					"%.17g", cantidad_solicitada);
				char *registro = cantidad_texto != NULL
					? inventario_registrar_movimiento_conteo(
						GG_ruta_sesion_inventario_activa, lista_codigos[i],
						producto[1], cantidad_texto, fecha_hora,
						error_o_datos_a_retornar, nivel_de_profundidad)
					: NULL;
				if(!ventas_respuesta_exitosa(registro))
					++errores_registro_inventario;
				sistema_memoria_liberar(cantidad_texto,
					error_o_datos_a_retornar, 0);
				sistema_memoria_liberar(registro,
					error_o_datos_a_retornar, 0);
			}
		}
		else
		{
			++omitidos;
		}
		sistema_memoria_liberar(columnas_actualizar,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(valores_actualizar,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta_edicion,
			error_o_datos_a_retornar, 0);
		liberarSplit(producto, columnas_producto,
			error_o_datos_a_retornar, 0);
		liberarSplit(partes, cantidad_partes,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta_busqueda,
			error_o_datos_a_retornar, 0);
	}

	char *mensaje = variable_string(
		"Productos actualizados: %d; omitidos: %d; errores al registrar movimientos del inventario: %d; total calculado: %.17g",
		actualizados, omitidos, errores_registro_inventario, total_venta);
	liberarSplit(lista_codigos, cantidad_codigos,
		error_o_datos_a_retornar, 0);
	liberarSplit(lista_cantidades, cantidad_cantidades,
		error_o_datos_a_retornar, 0);
	int error_reporte = actualizados > 0 &&
		(!gg_registros_registrar_operacion(direccion_archivo, fecha_hora,
			"VENTA", total_venta, total_costo_venta,
			total_impuestos_venta,
			productos_registro, error_o_datos_a_retornar,
			nivel_de_profundidad) || error_detalle_registro);
	sistema_memoria_liberar(productos_registro,
		error_o_datos_a_retornar, 0);
	if(mensaje == NULL)
		return crearResultado(-1, "no_se_pudo_generar_resumen_de_venta", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Venta: %s%s\n", mensaje,
		error_reporte ? "; ERROR: no se pudo completar el registro automático del movimiento" : "");
	char *mensaje_final = error_reporte
		? variable_string("%s; ERROR: no se pudo completar el registro automático del movimiento",
			mensaje)
		: variable_string("%s", mensaje);
	char *resultado = mensaje_final != NULL
		? crearResultado(actualizados > 0 ? 1 : -1,
			mensaje_final, "", __func__, error_o_datos_a_retornar,
			nivel_de_profundidad)
		: crearResultado(-1, "no_se_pudo_generar_resultado_de_venta", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_memoria_liberar(mensaje_final, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(mensaje, error_o_datos_a_retornar, 0);
	return resultado;
}

static int compras_respuesta_codigo(
	const char *respuesta,
	int *codigo,
	char ***partes_salida,
	int *cantidad_salida,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int cantidad = 0;
	char **partes = respuesta != NULL
		? split(respuesta, GG_caracter_para_confirmacion_o_error[0],
			&cantidad, error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	int correcto = partes != NULL && cantidad >= 2 &&
		texto_a_int_seguro(partes[0], codigo) == 0;
	if(!correcto)
	{
		liberarSplit(partes, cantidad, error_o_datos_a_retornar, 0);
		return 0;
	}
	if(partes_salida != NULL && cantidad_salida != NULL)
	{
		*partes_salida = partes;
		*cantidad_salida = cantidad;
	}
	else
	{
		liberarSplit(partes, cantidad, error_o_datos_a_retornar, 0);
	}
	return 1;
}

char *compras(
	const char *direccion_archivo,
	char **productos,
	int cantidad_productos,
	const char *proveedores,
	const char *sucursales,
	double porcentaje_ganancia,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(direccion_archivo == NULL || direccion_archivo[0] == '\0' ||
		productos == NULL || cantidad_productos <= 0 ||
		proveedores == NULL || sucursales == NULL ||
		porcentaje_ganancia < 0.0 ||
		porcentaje_ganancia > DBL_MAX)
		return crearResultado(-1, "parametros_de_compra_invalidos", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);

	char fecha_hora[20];
	fechaActual(fecha_hora, "%Y%m%d%H");
	if(fecha_hora[0] == '\0')
		return crearResultado(-1, "no_se_pudo_obtener_fecha_de_compra", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);

	int agregados = 0;
	int actualizados = 0;
	int fallidos = 0;
	int error_detalle_registro = 0;
	double total_compra = 0.0;
	double total_impuestos_compra = 0.0;
	char *productos_registro = variable_string("%s", "");
	if(productos_registro == NULL)
		return crearResultado(-1, "error_de_memoria_en_registro_de_compra", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	for(int i = 0; i < cantidad_productos; ++i)
	{
		char **item = productos + (size_t)i * 5;
		double cantidad = 0.0;
		double costo = 0.0;
		if(item[0] == NULL || item[0][0] == '\0' ||
			!ventas_parsear_numero(item[1], &cantidad) || cantidad <= 0.0 ||
			!ventas_parsear_numero(item[2], &costo) || costo < 0.0 ||
			item[3] == NULL || item[3][0] == '\0' ||
			item[4] == NULL || item[4][0] == '\0')
		{
			++fallidos;
			continue;
		}

		char *respuesta_busqueda = buscar_info_dividida(
			direccion_archivo, item[0], 5, -1);
		int codigo_busqueda = 0;
		char **partes_busqueda = NULL;
		int cantidad_partes = 0;
		if(!compras_respuesta_codigo(respuesta_busqueda, &codigo_busqueda,
			&partes_busqueda, &cantidad_partes, error_o_datos_a_retornar,
			nivel_de_profundidad))
		{
			++fallidos;
			sistema_memoria_liberar(respuesta_busqueda,
				error_o_datos_a_retornar, 0);
			continue;
		}
		if(codigo_busqueda == 1 && cantidad_partes >= 3)
		{
			char **fila = NULL;
			int cantidad_campos = 0;
			fila = split(partes_busqueda[1], GG_caracter_separacion[0],
				&cantidad_campos, error_o_datos_a_retornar,
				nivel_de_profundidad);
			double stock_actual = 0.0;
			double precio_actual = 0.0;
			double stock_nuevo = 0.0;
			double precio_nuevo = 0.0;
			int valida = fila != NULL && cantidad_campos >= 20 &&
				ventas_parsear_numero(fila[6], &stock_actual) &&
				ventas_parsear_numero(fila[4], &precio_actual) &&
				stock_actual >= 0.0 && precio_actual >= 0.0 &&
				stock_actual <= DBL_MAX - cantidad;
			stock_nuevo = valida ? stock_actual + cantidad : 0.0;
			precio_nuevo = precio_actual;
			if(valida && precio_actual <= costo)
			{
				precio_nuevo = costo * (1.0 + porcentaje_ganancia / 100.0);
				if(precio_nuevo > DBL_MAX || precio_nuevo < 0.0)
					valida = 0;
			}

			char *columnas = NULL;
			char *valores = NULL;
			char *stock_formateado = valida
				? variable_string("%.17g", stock_nuevo) : NULL;
			char *costo_formateado = valida
				? variable_string("%.17g", costo) : NULL;
			char *precio_formateado = valida && precio_nuevo != precio_actual
				? variable_string("%.17g", precio_nuevo) : NULL;
			double importe_producto = cantidad * costo;
			if(valida && costo > 0.0 && cantidad > DBL_MAX / costo)
				valida = 0;
			if(valida && (importe_producto > DBL_MAX - total_compra))
				valida = 0;
			double impuestos_producto = 0.0;
			if(valida && !gg_registros_calcular_impuestos(
				direccion_archivo, fila[15], importe_producto,
				&impuestos_producto, error_o_datos_a_retornar,
				nivel_de_profundidad))
			{
				error_detalle_registro = 1;
				impuestos_producto = 0.0;
			}
			if(valida &&
				impuestos_producto > DBL_MAX - total_impuestos_compra)
			{
				error_detalle_registro = 1;
				impuestos_producto = 0.0;
			}
			if(valida && stock_formateado != NULL && costo_formateado != NULL &&
				(precio_nuevo == precio_actual || precio_formateado != NULL))
			{
				if(precio_formateado != NULL)
				{
					columnas = variable_string("6%s7%s8%s18%s19%s4",
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0]);
					valores = variable_string("%s%s%s%s%s%s%s%s%s%s%s",
						stock_formateado, GG_caracter_separacion_funciones_espesificas[0],
						costo_formateado, GG_caracter_separacion_funciones_espesificas[0],
						proveedores, GG_caracter_separacion_funciones_espesificas[0],
						fecha_hora, GG_caracter_separacion_funciones_espesificas[0],
						sucursales, GG_caracter_separacion_funciones_espesificas[0],
						precio_formateado);
				}
				else
				{
					columnas = variable_string("6%s7%s8%s18%s19",
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0],
						GG_caracter_separacion_funciones_espesificas[0]);
					valores = variable_string("%s%s%s%s%s%s%s%s%s",
						stock_formateado, GG_caracter_separacion_funciones_espesificas[0],
						costo_formateado, GG_caracter_separacion_funciones_espesificas[0],
						proveedores, GG_caracter_separacion_funciones_espesificas[0],
						fecha_hora, GG_caracter_separacion_funciones_espesificas[0],
						sucursales);
				}
			}
			char *respuesta_edicion = columnas != NULL && valores != NULL
				? editar_celda_id_info_dividida(direccion_archivo,
					partes_busqueda[2], columnas, valores)
				: NULL;
			int codigo_edicion = 0;
			int editado = compras_respuesta_codigo(respuesta_edicion,
				&codigo_edicion, NULL, NULL, error_o_datos_a_retornar,
				nivel_de_profundidad) && codigo_edicion == 1;
			if(editado)
			{
				++actualizados;
				double importe_producto = cantidad * costo;
				if(costo > 0.0 && cantidad > DBL_MAX / costo)
				{
					error_detalle_registro = 1;
				}
				else
				{
					total_compra += importe_producto;
					total_impuestos_compra += impuestos_producto;
					char *cantidad_texto = variable_string("%.17g", cantidad);
					char *importe_texto = variable_string("%.17g", importe_producto);
					if(cantidad_texto == NULL || importe_texto == NULL ||
						!gg_registros_agregar_detalle(&productos_registro,
							item[0], importe_texto, item[3], cantidad_texto,
							proveedores, item[4], error_o_datos_a_retornar,
							nivel_de_profundidad))
						error_detalle_registro = 1;
					sistema_memoria_liberar(cantidad_texto,
						error_o_datos_a_retornar, 0);
					sistema_memoria_liberar(importe_texto,
						error_o_datos_a_retornar, 0);
				}
			}
			else
			{
				++fallidos;
			}
			sistema_memoria_liberar(stock_formateado,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(costo_formateado,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(precio_formateado,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(columnas, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(valores, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta_edicion,
				error_o_datos_a_retornar, 0);
			liberarSplit(fila, cantidad_campos,
				error_o_datos_a_retornar, 0);
		}
		else if(codigo_busqueda == 0 && cantidad_partes >= 2 &&
			strcmp(partes_busqueda[1], "no_encontro_el_producto") == 0)
		{
			char *campos[32];
			for(int columna = 0; columna < 32; ++columna)
				campos[columna] = "";
			double precio = costo * (1.0 + porcentaje_ganancia / 100.0);
			int numericos_validos = precio >= 0.0 && precio <= DBL_MAX;
			double importe_producto = cantidad * costo;
			if(costo > 0.0 && cantidad > DBL_MAX / costo)
				numericos_validos = 0;
			if(importe_producto > DBL_MAX - total_compra)
				numericos_validos = 0;
			char *cantidad_formateada = numericos_validos
				? variable_string("%.17g", cantidad) : NULL;
			char *costo_formateado = numericos_validos
				? variable_string("%.17g", costo) : NULL;
			char *precio_formateado = numericos_validos
				? variable_string("%.17g", precio) : NULL;
			campos[0] = item[3];
			campos[3] = precio_formateado;
			campos[4] = item[0];
			campos[5] = cantidad_formateada;
			campos[6] = costo_formateado;
			campos[7] = (char *)proveedores;
			campos[8] = item[4];
			campos[17] = fecha_hora;
			campos[18] = (char *)sucursales;
			char *fila_nueva = numericos_validos && cantidad_formateada != NULL &&
				costo_formateado != NULL && precio_formateado != NULL
				? join(campos, 32, GG_caracter_separacion[0],
					error_o_datos_a_retornar, nivel_de_profundidad)
				: NULL;
			char *respuesta_agregado = fila_nueva != NULL
				? agregar_info_dividida(direccion_archivo, fila_nueva,
					GG_secciones_negocio[1].columnas)
				: NULL;
			int codigo_agregado = 0;
			if(compras_respuesta_codigo(respuesta_agregado,
				&codigo_agregado, NULL, NULL, error_o_datos_a_retornar,
				nivel_de_profundidad) && codigo_agregado == 1)
			{
				++agregados;
				double importe_producto = cantidad * costo;
				if(costo > 0.0 && cantidad > DBL_MAX / costo)
				{
					error_detalle_registro = 1;
				}
				else
				{
					total_compra += importe_producto;
					char *importe_texto = variable_string("%.17g", importe_producto);
					if(!gg_registros_agregar_detalle(&productos_registro,
						item[0], importe_texto, item[3],
						cantidad_formateada, proveedores, item[4],
						error_o_datos_a_retornar, nivel_de_profundidad))
						error_detalle_registro = 1;
					sistema_memoria_liberar(importe_texto,
						error_o_datos_a_retornar, 0);
				}
			}
			else
			{
				++fallidos;
			}
			sistema_memoria_liberar(fila_nueva,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta_agregado,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(cantidad_formateada,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(costo_formateado,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(precio_formateado,
				error_o_datos_a_retornar, 0);
		}
		else
		{
			++fallidos;
		}
		liberarSplit(partes_busqueda, cantidad_partes,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta_busqueda,
			error_o_datos_a_retornar, 0);
	}

	char *resumen = variable_string(
		"Productos agregados: %d; actualizados: %d; fallidos: %d; total de compra procesado: %.17g",
		agregados, actualizados, fallidos, total_compra);
	int error_reporte = (agregados + actualizados) > 0 &&
		(!gg_registros_registrar_operacion(direccion_archivo, fecha_hora,
			"COMPRA", 0.0, total_compra, total_impuestos_compra,
			productos_registro,
			error_o_datos_a_retornar, nivel_de_profundidad) ||
			error_detalle_registro);
	sistema_memoria_liberar(productos_registro,
		error_o_datos_a_retornar, 0);
	if(resumen == NULL)
		return crearResultado(-1, "no_se_pudo_generar_resumen_de_compra", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Compra: %s%s\n", resumen,
		error_reporte ? "; ERROR: no se pudo completar el registro automático del movimiento" : "");
	char *mensaje_final = error_reporte
		? variable_string("%s; ERROR: no se pudo completar el registro automático del movimiento",
			resumen)
		: variable_string("%s", resumen);
	char *resultado = mensaje_final != NULL ? crearResultado(
		(agregados + actualizados) > 0 && fallidos == 0 ? 1 :
			((agregados + actualizados) > 0 ? 1 : -1),
		mensaje_final, "", __func__, error_o_datos_a_retornar,
		nivel_de_profundidad) : crearResultado(-1,
			"no_se_pudo_generar_resultado_de_compra", "",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_memoria_liberar(mensaje_final, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(resumen, error_o_datos_a_retornar, 0);
	return resultado;
}

static char *pruebas_recopilar_compras(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int cantidad_productos = 0;
	char *ruta = pm_pedir_texto(
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	char *proveedor = pm_pedir_texto("Proveedor: ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	char *sucursal = pm_pedir_texto("ID/nombre de sucursal: ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	char *porcentaje = pm_pedir_texto(
		"Porcentaje de ganancia para el precio de venta: ",
		error_o_datos_a_retornar, nivel_de_profundidad);
	char **campos = NULL;
	int capacidad = 0;
	int correcto = 0;
	double porcentaje_numero = 0.0;
	correcto = ruta != NULL && proveedor != NULL && sucursal != NULL &&
		porcentaje != NULL &&
		ventas_parsear_numero(porcentaje, &porcentaje_numero) &&
		porcentaje_numero >= 0.0;
	if(!correcto ||
		!pm_pedir_entero("Cantidad de productos en la compra: ",
			&cantidad_productos, error_o_datos_a_retornar,
			nivel_de_profundidad) ||
		cantidad_productos < 1 || cantidad_productos > 1000)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Datos de compra inválidos; indica entre 1 y 1000 productos y un porcentaje válido.\n");
		goto error_recopilar_compras;
	}
	if(ruta[0] == '\0')
	{
		sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
		ruta = pm_copiar_texto("CONFIG/INF/INVENTARIO/INVENTARIO.TXT");
		if(ruta == NULL) goto error_recopilar_compras;
	}
	if(strchr(ruta, GG_caracter_separacion[1][0]) != NULL ||
		strchr(proveedor, GG_caracter_separacion[1][0]) != NULL ||
		strchr(sucursal, GG_caracter_separacion[1][0]) != NULL)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Ruta, proveedor y sucursal no pueden contener el separador °.\n");
		goto error_recopilar_compras;
	}
	if((size_t)cantidad_productos > ((size_t)-1 / sizeof(*campos) - 6) / 5)
		goto error_recopilar_compras;
	capacidad = 6 + cantidad_productos * 5;
	campos = sistema_memoria_reservar((size_t)capacidad * sizeof(*campos),
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(campos == NULL) goto error_recopilar_compras;
	for(int i = 0; i < capacidad; ++i) campos[i] = NULL;
	campos[0] = ruta; ruta = NULL;
	campos[1] = proveedor; proveedor = NULL;
	campos[2] = sucursal; sucursal = NULL;
	campos[3] = porcentaje; porcentaje = NULL;
	campos[4] = variable_string("%d", cantidad_productos);
	for(int i = 0; i < cantidad_productos && campos[4] != NULL; ++i)
	{
		int base = 5 + i * 5;
		const char *prompts[] = {
			"Código de barras del producto: ",
			"Cantidad comprada: ",
			"Costo unitario de compra: ",
			"Nombre del producto (se usa si no existe): ",
			"Tipo (PRODUCTO_PIEZA, PRODUCTO_CANTIDAD o PRODUCTO_ELABORADO): "
		};
		for(int j = 0; j < 5; ++j)
		{
			campos[base + j] = pm_pedir_texto(
				prompts[j], error_o_datos_a_retornar, nivel_de_profundidad);
			if(campos[base + j] == NULL) goto error_recopilar_compras;
		}
		for(int j = 0; j < 5; ++j)
		{
			if(campos[base + j][0] == '\0' ||
				strchr(campos[base + j], GG_caracter_separacion[0][0]) != NULL ||
				strchr(campos[base + j], GG_caracter_separacion[1][0]) != NULL)
			{
				sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
					"Los datos del producto no pueden estar vacíos ni contener | o °.\n");
				goto error_recopilar_compras;
			}
		}
		if(strcmp(campos[base + 4], "PRODUCTO_PIEZA") != 0 &&
			strcmp(campos[base + 4], "PRODUCTO_CANTIDAD") != 0 &&
			strcmp(campos[base + 4], "PRODUCTO_ELABORADO") != 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Tipo no válido; usa PRODUCTO_PIEZA, PRODUCTO_CANTIDAD o PRODUCTO_ELABORADO.\n");
			goto error_recopilar_compras;
		}
	}
	if(campos[4] == NULL) goto error_recopilar_compras;
	{
		char *confirmacion = pm_pedir_texto(
			"Se actualizará el inventario y se guardarán registros de compra. Escriba SI para confirmar: ",
			error_o_datos_a_retornar, nivel_de_profundidad);
		if(confirmacion == NULL) goto error_recopilar_compras;
		if(strcmp(confirmacion, "SI") != 0 && strcmp(confirmacion, "si") != 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Compra cancelada; no se modificó el inventario.\n");
			sistema_memoria_liberar(confirmacion, error_o_datos_a_retornar, 0);
			for(int i = 0; i < capacidad; ++i)
				sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
			return pm_copiar_texto("0");
		}
		campos[capacidad - 1] = confirmacion;
	}
	{
		char *serializado = pruebas_serializar(1, campos, capacidad,
			error_o_datos_a_retornar);
		for(int i = 0; i < capacidad; ++i)
			sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
		return serializado;
	}

error_recopilar_compras:
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(proveedor, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(sucursal, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(porcentaje, error_o_datos_a_retornar, 0);
	if(campos != NULL)
	{
		for(int i = 0; i < capacidad; ++i)
			sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
	}
	return NULL;
}

static int ejecutar_prueba_compras(
	char **argumentos,
	int cantidad,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(argumentos == NULL || cantidad < 6 ||
		(strcmp(argumentos[cantidad - 1], "SI") != 0 &&
			strcmp(argumentos[cantidad - 1], "si") != 0))
		return -1;
	int cantidad_productos = 0;
	double porcentaje = 0.0;
	if(texto_a_int_seguro(argumentos[4], &cantidad_productos) != 0 ||
		cantidad_productos < 1 || cantidad_productos > 1000 ||
		cantidad != 6 + cantidad_productos * 5 ||
		!ventas_parsear_numero(argumentos[3], &porcentaje) ||
		porcentaje < 0.0)
		return -1;
	char **productos = argumentos + 5;
	char *resultado = compras(argumentos[0], productos,
		cantidad_productos, argumentos[1], argumentos[2], porcentaje,
		error_o_datos_a_retornar, nivel_de_profundidad);
	int codigo = -1;
	int correcto = resultado != NULL &&
		leerCodigoResultado(error_o_datos_a_retornar, 0, resultado, &codigo) &&
		codigo > 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado de compra: %s\n",
		resultado != NULL ? resultado : "(no se pudo procesar la compra)");
	sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

static int ejecutar_prueba_ventas(
	char **argumentos,
	int cantidad,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(argumentos == NULL || cantidad != 4 ||
		(strcmp(argumentos[3], "SI") != 0 &&
			strcmp(argumentos[3], "si") != 0))
		return cantidad == 4 ? 0 : -1;
	char *resultado = ventas(argumentos[0], argumentos[1], argumentos[2],
		error_o_datos_a_retornar, nivel_de_profundidad);
	int codigo = -1;
	int correcto = resultado != NULL &&
		leerCodigoResultado(error_o_datos_a_retornar, 0, resultado, &codigo) &&
		codigo > 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado de venta: %s\n",
		resultado != NULL ? resultado : "(no se pudo procesar la venta)");
	sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

static int ejecutar_prueba_espacios(
	int opcion,
	char **a,
	int n,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(opcion == 1 && n == 4)
	{
		char *creado = NULL;
		if(strcmp(a[3], "SI") != 0 && strcmp(a[3], "si") != 0) return 0;
		int estado = crear_espacios(a[0], a[1], a[2], &creado);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			estado == 0 ? "Espacio creado: %s\n" : "No se pudo crear el espacio.\n",
			creado != NULL ? creado : "(nombre no devuelto)");
		sistema_memoria_liberar(creado, error_o_datos_a_retornar, 0);
		return estado == 0 ? 0 : -1;
	}
	if(opcion == 2 && n == 5)
	{
		int minimo = 0, nivel = -1, nivel_negocio = -1;
		char *direccion = NULL;
		if(texto_a_int_seguro(a[4], &minimo) != 0) return -1;
		int permiso = checar_permiso(minimo, a[0], a[1], a[2], a[3],
			&direccion, &nivel);
		int permiso_negocio = checar_permiso_negocios(
			minimo, a[0], a[1], a[2], a[3], &nivel_negocio);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"checar_permiso: %s; nivel: %d; dirección: %s\n"
			"checar_permiso_negocios: %s; nivel: %d\n",
			permiso == 0 ? "PERMITIDO" : (permiso == 1 ? "DENEGADO" : "ERROR"),
			nivel, direccion != NULL ? direccion : "(sin dirección)",
			permiso_negocio == 0 ? "PERMITIDO" :
				(permiso_negocio == 1 ? "DENEGADO" : "ERROR"), nivel_negocio);
		sistema_memoria_liberar(direccion, error_o_datos_a_retornar, 0);
		return permiso >= 0 && permiso_negocio >= 0 ? 0 : -1;
	}
	if(opcion == 3 && n == 2)
	{
		int estado = strcmp(a[1], "SI") == 0 || strcmp(a[1], "si") == 0
			? crear_archivos_base_negocio(a[0]) : 0;
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			estado == 0 ? "Archivos base de negocio creados o ya existentes.\n"
				: "No se pudieron crear todos los archivos base.\n");
		return estado == 0 ? 0 : -1;
	}
	if(opcion == 4 && n == 3)
	{
		int estado = strcmp(a[2], "SI") == 0 || strcmp(a[2], "si") == 0
			? creacion_de_estructura_base(a[0], a[1]) : 0;
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			estado == 0 ? "Estructura base de inventario creada.\n"
				: "No se pudo crear la estructura base de inventario.\n");
		return estado == 0 ? 0 : -1;
	}
	(void)nivel_de_profundidad;
	return -1;
}

static int pm_agregar_usuario_afiliado_parametros(
	const char *ruta,
	char **datos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int columnas = 0;
	char **nombres = split(GG_secciones_negocio[4].columnas,
		GG_caracter_separacion[1], &columnas,
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(nombres == NULL || columnas != 44)
	{
		liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
		return -1;
	}
	if(datos[0][0] == '\0')
	{
		liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"El campo NOMBRE es obligatorio.\n");
		return -1;
	}
	char **campos = sistema_memoria_reservar(
		(size_t)columnas * sizeof(*campos),
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(campos == NULL)
	{
		liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
		return -1;
	}
	for(int i = 0; i < columnas; ++i) campos[i] = NULL;
	int correcto = 1;
	for(int i = 1; i < columnas; ++i)
	{
		campos[i] = pm_copiar_texto(datos[i - 1]);
		if(campos[i] == NULL) correcto = 0;
	}
	long id = 0;
	if(correcto)
		correcto = pm_siguiente_id_afiliado(ruta,
			GG_secciones_negocio[4].columnas, &id,
			error_o_datos_a_retornar, nivel_de_profundidad);
	if(correcto)
	{
		char id_texto[sizeof(long) * CHAR_BIT + 2];
		if(!pm_long_a_texto(id, id_texto, sizeof(id_texto))) correcto = 0;
		else campos[0] = pm_copiar_texto(id_texto);
		if(campos[0] == NULL) correcto = 0;
	}
	char *fila = correcto ? join(campos, columnas,
		GG_caracter_separacion[1], error_o_datos_a_retornar,
		nivel_de_profundidad) : NULL;
	char *escritura = fila != NULL ? escribirLinea(
		ruta, fila, error_o_datos_a_retornar, nivel_de_profundidad) : NULL;
	correcto = escritura != NULL &&
		!resultadoTieneError(error_o_datos_a_retornar,
			nivel_de_profundidad, escritura);
	if(correcto)
	{
		char id_asignado[sizeof(long) * CHAR_BIT + 2];
		if(!pm_long_a_texto(id, id_asignado, sizeof(id_asignado)))
			correcto = 0;
		else
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Usuario agregado. ID asignado: %s\n", id_asignado);
	}
	sistema_memoria_liberar(escritura, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	for(int i = 0; i < columnas; ++i)
		sistema_memoria_liberar(campos[i], error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(campos, error_o_datos_a_retornar, 0);
	liberarSplit(nombres, columnas, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

static int pm_agregar_usuario_afiliado_con_encargado(
	const char *ruta,
	const char *id_encargado,
	const char *proyecto_tabla,
	char **datos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta_final = ruta != NULL && ruta[0] != '\0'
		? ruta : GG_secciones_negocio[4].ruta;
	if(id_encargado == NULL || id_encargado[0] == '\0' ||
		proyecto_tabla == NULL || proyecto_tabla[0] == '\0' ||
		datos == NULL ||
		strstr(id_encargado, GG_caracter_separacion[1]) != NULL ||
		strstr(proyecto_tabla, GG_caracter_separacion[1]) != NULL ||
		strstr(id_encargado, GG_caracter_separacion[2]) != NULL ||
		strstr(proyecto_tabla, GG_caracter_separacion[2]) != NULL)
		return -1;

	for(int i = 0; i < 31; ++i)
	{
		if(datos[i] == NULL ||
			strstr(datos[i], GG_caracter_separacion[1]) != NULL ||
			strchr(datos[i], '\r') != NULL || strchr(datos[i], '\n') != NULL)
			return -1;
	}

	proceso_mul p;
	proceso_mul_inicializar(&p);
	char *fila_patrocinador = NULL;
	int numero_linea = 0;
	int encontrado = pm_obtener_fila_afiliado(
		&p, ruta_final, id_encargado, NULL, &fila_patrocinador,
		&numero_linea, error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_memoria_liberar(fila_patrocinador,
		error_o_datos_a_retornar, 0);
	if(encontrado <= 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"No se encontró un encargado válido en AFILIADOS_UNIFICADO.\n");
		return -1;
	}

	char *relacion = variable_string(
		"ENCARGADO%s%s%s%s",
		GG_caracter_separacion[2], proyecto_tabla,
		GG_caracter_separacion[2], id_encargado);
	char *puntos_proyecto = variable_string(
		"%s%s0", proyecto_tabla, GG_caracter_separacion[2]);
	char *cero = pm_copiar_texto("0");
	char *vacio_nivel = pm_copiar_texto("");
	char *vacio_horizontal = pm_copiar_texto("");
	char *vacio_tipo = pm_copiar_texto("");
	if(relacion == NULL || puntos_proyecto == NULL || cero == NULL ||
		vacio_nivel == NULL || vacio_horizontal == NULL || vacio_tipo == NULL)
	{
		sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(puntos_proyecto, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(cero, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(vacio_nivel, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(vacio_horizontal, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(vacio_tipo, error_o_datos_a_retornar, 0);
		return -1;
	}

	char *datos_nuevos[43];
	for(int i = 0; i < 43; ++i) datos_nuevos[i] = "";
	memcpy(datos_nuevos, datos, 31 * sizeof(*datos_nuevos));
	datos_nuevos[31] = relacion;
	for(int i = 32; i <= 36; ++i) datos_nuevos[i] = cero;
	datos_nuevos[37] = vacio_nivel;
	datos_nuevos[40] = puntos_proyecto;
	datos_nuevos[41] = vacio_horizontal;
	datos_nuevos[42] = vacio_tipo;
	int resultado = pm_agregar_usuario_afiliado_parametros(
		ruta_final, datos_nuevos, error_o_datos_a_retornar,
		nivel_de_profundidad);

	sistema_memoria_liberar(relacion, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(puntos_proyecto, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(cero, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(vacio_nivel, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(vacio_horizontal, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(vacio_tipo, error_o_datos_a_retornar, 0);
	return resultado;
}

char *registro_unificado_cod3_r_(
	const char *dir_arch_afiliados,
	const char *dir_arch_niveles,
	const char *id_enc,
	const char *tabla_enc,
	const char *datos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	/* El esquema unificado actual guarda nivel y relación en una sola tabla. */
	(void)dir_arch_niveles;
	int cantidad_datos = 0;
	char **datos_personales = datos != NULL
		? split(datos, GG_caracter_separacion[1], &cantidad_datos,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(datos_personales == NULL || cantidad_datos != 31)
	{
		liberarSplit(datos_personales, cantidad_datos,
			error_o_datos_a_retornar, 0);
		return crearResultado(-1,
			"se_requieren_31_campos_personales_de_afiliado",
			"Los campos corresponden a las columnas 1 a 31 del esquema unificado.",
			__func__, error_o_datos_a_retornar, nivel_de_profundidad);
	}

	int correcto = pm_agregar_usuario_afiliado_con_encargado(
		dir_arch_afiliados, id_enc, tabla_enc, datos_personales,
		error_o_datos_a_retornar, nivel_de_profundidad);
	liberarSplit(datos_personales, cantidad_datos,
		error_o_datos_a_retornar, 0);
	return crearResultado(correcto == 0 ? 1 : -1,
		correcto == 0 ? "afiliado_registrado_con_encargado"
			: "no_se_pudo_registrar_afiliado",
		"", __func__, error_o_datos_a_retornar, nivel_de_profundidad);
}

static int ejecutar_prueba_proceso_mul(
	int opcion,
	char **a,
	int n,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(opcion == 1 && n == 45)
		return pm_agregar_usuario_afiliado_parametros(
			a[0], a + 1, error_o_datos_a_retornar, nivel_de_profundidad);
	if(opcion == 8 && n == 35 &&
		(strcmp(a[34], "SI") == 0 || strcmp(a[34], "si") == 0))
	{
		char *datos = join(a + 3, 31, GG_caracter_separacion[1],
			error_o_datos_a_retornar, nivel_de_profundidad);
		char *respuesta = datos != NULL
			? registro_unificado_cod3_r_(
				a[0], "", a[1], a[2], datos,
				error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		int correcto = respuesta != NULL &&
			!resultadoTieneError(error_o_datos_a_retornar,
				nivel_de_profundidad, respuesta);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Resultado: %s\n",
			respuesta != NULL ? respuesta : "(sin resultado)");
		sistema_memoria_liberar(datos, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}
	if(opcion == 2 && n == 11)
	{
		proceso_mul p;
		proceso_mul_inicializar(&p);
		char *r = registro_unificado_cod3_r(&p, a[0], a[1], a[2],
			a[3], a[4], a[5], a[6], a[7], a[8], a[9],
			error_o_datos_a_retornar, nivel_de_profundidad);
		int correcto = r != NULL &&
			!resultadoTieneError(error_o_datos_a_retornar,
				nivel_de_profundidad, r);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Resultado: %s\n", r != NULL ? r : "(sin resultado)");
		sistema_memoria_liberar(r, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}
	if((opcion == 3 || opcion == 4) && n == 3)
	{
		char *r = opcion == 3 ? dinero_de_venta(a[0], a[1], a[2]) :
			entrada_dinero_simple(a[0], a[1], a[2],
				GG_caracter_separacion[0]);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			opcion == 3 ? "Pago calculado: %s\n" : "Pagos: %s\n",
			r != NULL ? r : "(entrada inválida)");
		int correcto = r != NULL;
		sistema_memoria_liberar(r, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}
	if(opcion == 5 && n == 4)
	{
		int profundidad = 0;
		if(texto_a_int_seguro(a[3], &profundidad) != 0) return -1;
		char *cadena = extraer_patrosinadores_funcion_recursiva(
			a[0], a[1], a[2], profundidad, GG_caracter_separacion[0],
			error_o_datos_a_retornar, nivel_de_profundidad);
		int correcto = cadena != NULL;
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Patrocinadores ascendentes: %s\n",
			cadena != NULL ? cadena : "(no se pudo consultar)");
		sistema_memoria_liberar(cadena, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}
	if(opcion == 6 && n == 4)
	{
		char *r = calc_din_por_enc_y_total(a[0], a[1], a[2], a[3],
			GG_caracter_separacion[0]);
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Pagos simples | pagos complejos | total: %s\n",
			r != NULL ? r : "(entrada inválida)");
		int correcto = r != NULL;
		sistema_memoria_liberar(r, error_o_datos_a_retornar, 0);
		return correcto ? 0 : -1;
	}
	if(opcion == 7 && n == 3)
	{
		int veces = 0;
		if(texto_a_int_seguro(a[1], &veces) != 0 || veces < 0) return -1;
		char **r = acumulador_de_strings(a[0], veces,
			GG_caracter_separacion[0], a[2]);
		if(r == NULL) return -1;
		for(size_t i = 0; r[i] != NULL; ++i)
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"[%d] %s\n", (int)i, r[i]);
		for(size_t i = 0; r[i] != NULL; ++i)
			sistema_memoria_liberar(r[i], error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(r, error_o_datos_a_retornar, 0);
		return 0;
	}
	return -1;
}

static char *pruebas_recopilar_inventario(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	char **campos = NULL;
	const char *prompts_producto[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		"Producto: 21 campos heredados o 32 campos actuales separados por |: ",
		"Escriba SI para confirmar: "
	};
	const char *prompts_busqueda[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		"Código de barras: ",
		"ID sugerido (opcional; Enter busca todos): "
	};
	const char *prompts_ruta[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): "
	};
	const char *prompts_inicio[] = {
		"Ruta del registro de conteo (Enter usa la ruta predeterminada): ",
		"Escriba SI para iniciar el conteo: "
	};
	const char *prompts_conciliar[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		"Ruta del registro de conteo: ",
		"Operación (MODIFICAR_INVENTARIO o MOSTRAR_FALTANTES): ",
		"Fecha del conteo en formato AAAAMMDD: ",
		"Conteos separados por ╬; cada fila: código|nombre|cantidad|fecha: ",
		"Esta operación actualizará archivos. Escriba SI para confirmar: "
	};
	const char **prompts = NULL;
	int cantidad = 0;
	const char *columnas_producto = GG_secciones_negocio[1].columnas;
	const char *separador_id = strstr(
		columnas_producto, GG_caracter_separacion[1]);
	if(separador_id != NULL)
		columnas_producto = separador_id +
			strlen(GG_caracter_separacion[1]);

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PRODUCTOS E INVENTARIO ===\n"
		"1. Agregar producto\n"
		"2. Agregar producto solo si no existe el código\n"
		"3. Buscar producto por código\n"
		"4. Buscar producto unitario del paquete por código\n"
		"5. Obtener el inventario como texto\n"
		"6. Inicializar un registro de conteo\n"
		"7. Conciliar conteo (sumará ventas registradas durante el conteo)\n"
		"8. Volver\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Para agregar productos, la fila actual lleva estos campos (sin ID):\n%s\n",
		columnas_producto);
	if(!pm_pedir_entero("Seleccione una opción: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad))
		return NULL;
	if(opcion == 8) return pm_copiar_texto("0");
	switch(opcion)
	{
		case 1:
		case 2:
			prompts = prompts_producto;
			cantidad = 3;
			break;
		case 3:
		case 4:
			prompts = prompts_busqueda;
			cantidad = 3;
			break;
		case 5:
			prompts = prompts_ruta;
			cantidad = 1;
			break;
		case 6:
			prompts = prompts_inicio;
			cantidad = 2;
			break;
		case 7:
			prompts = prompts_conciliar;
			cantidad = 6;
			break;
		default:
			return NULL;
	}
	if(!pruebas_leer_campos(prompts, cantidad, &campos,
		error_o_datos_a_retornar, nivel_de_profundidad))
	{
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return NULL;
	}
	if((opcion == 1 || opcion == 2 || opcion == 3 || opcion == 4 ||
		opcion == 5) && campos[0][0] == '\0')
	{
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = pm_copiar_texto(
			"CONFIG/INF/INVENTARIO/INVENTARIO.TXT");
	}
	if(opcion == 6 && campos[0][0] == '\0')
	{
		char fecha_hora[24];
		fechaActual(fecha_hora, "%Y%m%d%H%M%S");
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = fecha_hora[0] != '\0'
			? variable_string(
				"CONFIG/INF/INVENTARIO/INVENTARIO_EN_CURSO_%s.TXT",
				fecha_hora)
			: NULL;
	}
	if(opcion == 7 && campos[0][0] == '\0')
	{
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = pm_copiar_texto(
			"CONFIG/INF/INVENTARIO/INVENTARIO.TXT");
	}
	int indice_confirmacion = opcion == 1 || opcion == 2 ? 2 :
		(opcion == 6 ? 1 : (opcion == 7 ? 5 : -1));
	if(campos[0] == NULL ||
		(opcion == 7 && (strcmp(campos[2], "MODIFICAR_INVENTARIO") != 0 &&
			strcmp(campos[2], "MOSTRAR_FALTANTES") != 0)))
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Ruta u operación de inventario no válida.\n");
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return NULL;
	}
	if(indice_confirmacion >= 0 &&
		strcmp(campos[indice_confirmacion], "SI") != 0 &&
		strcmp(campos[indice_confirmacion], "si") != 0)
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Operación cancelada; no se modificaron archivos.\n");
		pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
		return pm_copiar_texto("0");
	}
	for(int i = 0; i < cantidad; ++i)
	{
		if(campos[i] == NULL || !pruebas_campo_serializable(campos[i]))
		{
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return NULL;
		}
	}
	char *serializado = pruebas_serializar(
		opcion, campos, cantidad, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
	return serializado;
}

static int ejecutar_prueba_inventario(
	int opcion,
	char **a,
	int n,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta_predeterminada =
		"CONFIG/INF/INVENTARIO/INVENTARIO.TXT";
	char *ruta = NULL;
	char *resultado = NULL;
	int codigo = -1;

	if((opcion == 1 || opcion == 2) && n == 3)
	{
		ruta = a[0][0] != '\0' ? pm_copiar_texto(a[0])
			: pm_copiar_texto(ruta_predeterminada);
		resultado = ruta != NULL
			? (opcion == 1
				? inventario_agregar_producto(ruta, a[1],
					error_o_datos_a_retornar, nivel_de_profundidad)
				: inventario_agrega_si_no_existe(ruta, a[1],
					error_o_datos_a_retornar, nivel_de_profundidad))
			: NULL;
		if(opcion == 2 && resultado != NULL &&
			compras_respuesta_codigo(resultado, &codigo, NULL, NULL,
				error_o_datos_a_retornar, nivel_de_profundidad) &&
			codigo == 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"El código ya existía; no se agregó otra fila.\n");
			sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return 0;
		}
	}
	else if((opcion == 3 || opcion == 4) && n == 3)
	{
		ruta = a[0][0] != '\0' ? pm_copiar_texto(a[0])
			: pm_copiar_texto(ruta_predeterminada);
		resultado = ruta != NULL
			? (opcion == 3
				? inventario_buscar(ruta, a[1], a[2],
					error_o_datos_a_retornar, nivel_de_profundidad)
				: inventario_sacar_info_productos_unitario_del_paquete(
					ruta, a[1], a[2], error_o_datos_a_retornar,
					nivel_de_profundidad))
			: NULL;
	}
	else if(opcion == 5 && n == 1)
	{
		ruta = a[0][0] != '\0' ? pm_copiar_texto(a[0])
			: pm_copiar_texto(ruta_predeterminada);
		resultado = ruta != NULL
			? inventario_dar_el_inventario_string_caracter_sep(
				ruta, GG_caracter_separacion_funciones_espesificas[0],
				error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		if(resultado != NULL)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"%s\n", resultado);
			sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
			return 0;
		}
	}
	else if(opcion == 6 && n == 2 &&
		(strcmp(a[1], "SI") == 0 || strcmp(a[1], "si") == 0))
	{
		resultado = inventario_archivos_inicio_hacer_inventario(
			a[0], error_o_datos_a_retornar, nivel_de_profundidad);
	}
	else if(opcion == 7 && n == 6 &&
		(strcmp(a[5], "SI") == 0 || strcmp(a[5], "si") == 0))
	{
		char *ruta_inventario = a[0][0] != '\0'
			? pm_copiar_texto(a[0]) : pm_copiar_texto(ruta_predeterminada);
		resultado = ruta_inventario != NULL
			? inventario_hacer_inventario(
				ruta_inventario, a[1], a[2], a[3],
				error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		sistema_memoria_liberar(ruta_inventario,
			error_o_datos_a_retornar, 0);
	}
	else
	{
		return -1;
	}

	int correcto = resultado != NULL &&
		compras_respuesta_codigo(resultado, &codigo, NULL, NULL,
			error_o_datos_a_retornar, nivel_de_profundidad) &&
		(opcion == 2 ? codigo >= 0 : codigo > 0);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado de inventario: %s\n",
		resultado != NULL ? resultado : "(no se pudo ejecutar)");
	sistema_memoria_liberar(ruta, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

typedef struct
{
	char *codigo;
	char *nombre;
	char *fecha;
	double cantidad;
} InventarioConteoItem;

static void inventario_liberar_items(
	InventarioConteoItem *items,
	int cantidad)
{
	if(items == NULL) return;
	for(int i = 0; i < cantidad; ++i)
	{
		sistema_memoria_liberar(items[i].codigo, NULL, 0);
		sistema_memoria_liberar(items[i].nombre, NULL, 0);
		sistema_memoria_liberar(items[i].fecha, NULL, 0);
	}
	sistema_memoria_liberar(items, NULL, 0);
}

static int inventario_agregar_item(
	InventarioConteoItem *items,
	int *cantidad_items,
	int capacidad,
	const char *codigo,
	const char *nombre,
	double cantidad,
	const char *fecha)
{
	if(items == NULL || cantidad_items == NULL || codigo == NULL ||
		codigo[0] == '\0' || nombre == NULL || cantidad < 0.0 ||
		cantidad > DBL_MAX)
		return 0;
	for(int i = 0; i < *cantidad_items; ++i)
	{
		if(strcmp(items[i].codigo, codigo) == 0)
		{
			if(cantidad > DBL_MAX - items[i].cantidad) return 0;
			items[i].cantidad += cantidad;
			if(items[i].nombre[0] == '\0' && nombre[0] != '\0')
			{
				char *copia = pm_copiar_texto(nombre);
				if(copia == NULL) return 0;
				sistema_memoria_liberar(items[i].nombre, NULL, 0);
				items[i].nombre = copia;
			}
			if(fecha != NULL && fecha[0] != '\0')
			{
				char *copia = pm_copiar_texto(fecha);
				if(copia == NULL) return 0;
				sistema_memoria_liberar(items[i].fecha, NULL, 0);
				items[i].fecha = copia;
			}
			return 1;
		}
	}
	if(*cantidad_items >= capacidad) return 0;
	InventarioConteoItem *item = &items[*cantidad_items];
	item->codigo = pm_copiar_texto(codigo);
	item->nombre = pm_copiar_texto(nombre);
	item->fecha = pm_copiar_texto(fecha != NULL ? fecha : "");
	item->cantidad = cantidad;
	if(item->codigo == NULL || item->nombre == NULL || item->fecha == NULL)
	{
		sistema_memoria_liberar(item->codigo, NULL, 0);
		sistema_memoria_liberar(item->nombre, NULL, 0);
		sistema_memoria_liberar(item->fecha, NULL, 0);
		item->codigo = NULL;
		item->nombre = NULL;
		item->fecha = NULL;
		return 0;
	}
	++*cantidad_items;
	return 1;
}

static int inventario_codigo_respuesta(const char *respuesta, int *codigo)
{
	return compras_respuesta_codigo(respuesta, codigo, NULL, NULL, NULL, 0);
}

static char *inventario_respuesta(int codigo, const char *mensaje)
{
	return variable_string("%d%s%s", codigo,
		GG_caracter_para_confirmacion_o_error[0],
		mensaje != NULL ? mensaje : "");
}

static int inventario_validar_numericos(
	char **campos,
	int cantidad,
	int heredado)
{
	static const int indices_actuales[] = {1, 3, 5, 6, 9, 17};
	static const int indices_heredados[] = {1, 3, 5, 6, 9, 15};
	const int *indices = heredado ? indices_heredados : indices_actuales;
	int cantidad_indices = 6;
	for(int i = 0; i < cantidad_indices; ++i)
	{
		int indice = indices[i];
		if(indice >= cantidad) return 0;
		if(campos[indice][0] == '\0')
		{
			if(i < 4) return 0;
			continue;
		}
		double valor = 0.0;
		if(!ventas_parsear_numero(campos[indice], &valor) || valor < 0.0)
			return 0;
	}
	return 1;
}

static int inventario_preparar_producto(
	const char *producto,
	char **fila_actual,
	char **codigo_barras,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int cantidad = 0;
	char **campos = producto != NULL
		? split(producto, GG_caracter_separacion[0], &cantidad,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(campos == NULL || (cantidad != 21 && cantidad != 32))
	{
		liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
		return 0;
	}
	int heredado = cantidad == 21;
	char **actuales = campos;
	if(heredado)
	{
		actuales = sistema_memoria_reservar(
			32 * sizeof(*actuales), error_o_datos_a_retornar,
			nivel_de_profundidad);
		if(actuales == NULL)
		{
			liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
			return 0;
		}
		for(int i = 0; i < 32; ++i) actuales[i] = "";
		actuales[0] = campos[0];
		actuales[1] = campos[1];
		actuales[2] = campos[2];
		actuales[3] = campos[3];
		actuales[4] = campos[4];
		actuales[5] = campos[5];
		actuales[6] = campos[6];
		actuales[7] = campos[7];
		actuales[8] = campos[8];
		actuales[9] = campos[9];
		actuales[11] = campos[10];
		actuales[13] = campos[11];
		actuales[14] = campos[12];
		actuales[15] = campos[13];
		actuales[16] = campos[14];
		actuales[17] = campos[15];
		actuales[18] = campos[16];
		actuales[19] = campos[17];
		actuales[20] = campos[18];
		actuales[21] = campos[19];
		actuales[22] = campos[20];
	}
	int validos = inventario_validar_numericos(
		heredado ? campos : actuales, cantidad, heredado) &&
		actuales[4] != NULL && actuales[4][0] != '\0';
	char *fila = validos ? join(actuales, 32,
		GG_caracter_separacion[0], error_o_datos_a_retornar,
		nivel_de_profundidad) : NULL;
	char *codigo = validos
		? pm_copiar_texto(actuales[4]) : NULL;
	if(heredado)
		sistema_memoria_liberar(actuales, error_o_datos_a_retornar, 0);
	liberarSplit(campos, cantidad, error_o_datos_a_retornar, 0);
	if(fila == NULL || codigo == NULL)
	{
		sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(codigo, error_o_datos_a_retornar, 0);
		return 0;
	}
	*fila_actual = fila;
	*codigo_barras = codigo;
	return 1;
}

char *inventario_agregar_producto(
	const char *direccion_archivo,
	const char *producto,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	char *fila = NULL;
	char *codigo = NULL;
	if(!inventario_preparar_producto(producto, &fila, &codigo,
		error_o_datos_a_retornar, nivel_de_profundidad))
		return inventario_respuesta(
			0, "producto_o_valores_numericos_invalidos");
	char *respuesta = agregar_info_dividida(
		ruta, fila, GG_secciones_negocio[1].columnas);
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(codigo, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *inventario_agrega_si_no_existe(
	const char *direccion_archivo,
	const char *producto,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	char *fila = NULL;
	char *codigo = NULL;
	if(!inventario_preparar_producto(producto, &fila, &codigo,
		error_o_datos_a_retornar, nivel_de_profundidad))
		return inventario_respuesta(
			0, "producto_o_valores_numericos_invalidos");
	char *respuesta = agregar_sino_existe_info_dividida(
		ruta, 5, codigo, fila, GG_secciones_negocio[1].columnas);
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(codigo, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *inventario_buscar(
	const char *direccion_archivo,
	const char *codigo_barras,
	const char *id_producto,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	int id = -1;
	if(codigo_barras == NULL || codigo_barras[0] == '\0')
		return inventario_respuesta(0, "codigo_de_barras_vacio");
	if(id_producto != NULL && id_producto[0] != '\0' &&
		(texto_a_int_seguro(id_producto, &id) != 0 || id <= 0))
		return inventario_respuesta(0, "id_sugerido_invalido");
	(void)error_o_datos_a_retornar;
	(void)nivel_de_profundidad;
	return buscar_info_dividida(ruta, codigo_barras, 5, id);
}

char *inventario_sacar_info_productos_unitario_del_paquete(
	const char *direccion_archivo,
	const char *codigo_barras,
	const char *id_producto,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	return inventario_buscar(direccion_archivo, codigo_barras,
		id_producto, error_o_datos_a_retornar, nivel_de_profundidad);
}

char *inventario_archivos_inicio_hacer_inventario(
	const char *direccion_registro,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_registro != NULL &&
		direccion_registro[0] != '\0'
		? direccion_registro
		: "CONFIG/INF/INVENTARIO/INVENTARIO_EN_CURSO.TXT";
	static const char columnas[] =
		"COD_BARRAS°NOMBRE°CANTIDAD°FECHA";
	if(GG_ruta_sesion_inventario_activa == NULL ||
		strcmp(GG_ruta_sesion_inventario_activa, ruta) != 0)
	{
		if(existe_archivo(ruta))
			return inventario_respuesta(
				0, "el_registro_ya_existe; use_una_ruta_nueva");
		char *metadata = variable_string(
			"tipo_info%sinfo\nID_TOT%s0\nCOLUMNAS%s%s\nCANT_POR_ARCH%s100",
			GG_caracter_separacion[0], GG_caracter_separacion[0],
			GG_caracter_separacion[0], columnas,
			GG_caracter_separacion[0]);
		int creado = metadata != NULL
			? crearArchivo(ruta, metadata) : -1;
		sistema_memoria_liberar(metadata,
			error_o_datos_a_retornar, 0);
		if(creado < 0)
			return inventario_respuesta(
				0, "no_se_pudo_inicializar_el_conteo");
	}
	char *ruta_activa = pm_copiar_texto(ruta);
	if(ruta_activa == NULL)
		return inventario_respuesta(0, "error_de_memoria");
	sistema_memoria_liberar(GG_ruta_sesion_inventario_activa,
		error_o_datos_a_retornar, 0);
	GG_ruta_sesion_inventario_activa = ruta_activa;
	(void)nivel_de_profundidad;
	return inventario_respuesta(1, "registro_de_conteo_inicializado");
}

char *inventario_registrar_movimiento_conteo(
	const char *direccion_registro,
	const char *codigo_barras,
	const char *nombre,
	const char *cantidad,
	const char *fecha,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_registro != NULL &&
		direccion_registro[0] != '\0'
		? direccion_registro : GG_ruta_sesion_inventario_activa;
	double numero = 0.0;
	if(ruta == NULL || codigo_barras == NULL || codigo_barras[0] == '\0' ||
		nombre == NULL || !ventas_parsear_numero(cantidad, &numero) ||
		numero <= 0.0 || fecha == NULL || fecha[0] == '\0')
		return inventario_respuesta(0, "movimiento_de_conteo_invalido");
	if(GG_ruta_sesion_inventario_activa == NULL ||
		strcmp(GG_ruta_sesion_inventario_activa, ruta) != 0)
	{
		char *respuesta_inicio =
			inventario_archivos_inicio_hacer_inventario(
				ruta, error_o_datos_a_retornar, nivel_de_profundidad);
		int codigo_inicio = -1;
		int iniciado = inventario_codigo_respuesta(
			respuesta_inicio, &codigo_inicio) && codigo_inicio > 0;
		sistema_memoria_liberar(respuesta_inicio,
			error_o_datos_a_retornar, 0);
		if(!iniciado)
			return inventario_respuesta(
				0, "no_se_pudo_abrir_el_registro_de_conteo");
	}
	char *fila = variable_string("%s%s%s%s%s%s%s",
		codigo_barras, GG_caracter_separacion[0], nombre,
		GG_caracter_separacion[0], cantidad,
		GG_caracter_separacion[0], fecha);
	if(fila == NULL) return inventario_respuesta(0, "error_de_memoria");
	char *respuesta = agregar_info_dividida(
		ruta, fila, "COD_BARRAS°NOMBRE°CANTIDAD°FECHA");
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *inventario_dar_el_inventario_string_caracter_sep(
	const char *direccion_archivo,
	const char *separador,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	const char *union_filas = separador != NULL && separador[0] != '\0'
		? separador : GG_caracter_separacion_funciones_espesificas[0];
	char *filas = leer_todo_info_dividida(ruta);
	if(filas == NULL)
		return inventario_respuesta(0, "no_se_pudo_leer_el_inventario");
	char *filas_formateadas = ReemplazarCaracteres_de_texto_string(
		filas, GG_caracter_separacion_funciones_espesificas[3],
		union_filas);
	char *columnas = ReemplazarCaracteres_de_texto_string(
		GG_secciones_negocio[1].columnas, GG_caracter_separacion[1],
		GG_caracter_separacion[0]);
	char *resultado = columnas != NULL && filas_formateadas != NULL
		? (filas_formateadas[0] != '\0'
			? variable_string("%s%s%s", columnas, union_filas,
				filas_formateadas)
			: pm_copiar_texto(columnas))
		: NULL;
	sistema_memoria_liberar(filas, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(filas_formateadas,
		error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(columnas, error_o_datos_a_retornar, 0);
	(void)nivel_de_profundidad;
	return resultado;
}

static int inventario_cargar_conteos(
	const char *texto_filas,
	int incluye_id,
	InventarioConteoItem *items,
	int *cantidad_items,
	int capacidad,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(texto_filas == NULL || texto_filas[0] == '\0') return 1;
	int cantidad_filas = 0;
	char **filas = split(texto_filas,
		GG_caracter_separacion_funciones_espesificas[3],
		&cantidad_filas, error_o_datos_a_retornar,
		nivel_de_profundidad);
	if(filas == NULL) return 0;
	int correcto = 1;
	for(int i = 0; correcto && i < cantidad_filas; ++i)
	{
		if(filas[i][0] == '\0') continue;
		int cantidad_campos = 0;
		char **campos = split(filas[i], GG_caracter_separacion[0],
			&cantidad_campos, error_o_datos_a_retornar,
			nivel_de_profundidad);
		int offset = incluye_id ? 1 : 0;
		double cantidad = 0.0;
		correcto = campos != NULL && cantidad_campos >= offset + 3 &&
			campos[offset][0] != '\0' &&
			ventas_parsear_numero(campos[offset + 2], &cantidad) &&
			cantidad >= 0.0 &&
			inventario_agregar_item(items, cantidad_items, capacidad,
				campos[offset], campos[offset + 1], cantidad,
				cantidad_campos > offset + 3 ? campos[offset + 3] : "");
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
	}
	liberarSplit(filas, cantidad_filas, error_o_datos_a_retornar, 0);
	return correcto;
}

static char *inventario_reporte_path(
	const char *direccion_registro,
	const char *tipo)
{
	return variable_string("%s.%s", direccion_registro, tipo);
}

static int inventario_iniciar_reporte(const char *ruta)
{
	if(ruta == NULL) return 0;
	if(existe_archivo(ruta)) return 1;
	char *metadata = variable_string(
		"tipo_info%sinfo\nID_TOT%s0\nCOLUMNAS%s%s\nCANT_POR_ARCH%s100",
		GG_caracter_separacion[0], GG_caracter_separacion[0],
		GG_caracter_separacion[0],
		"COD_BARRAS°NOMBRE°CANTIDAD_CONTEO°CANTIDAD_SISTEMA°ULTIMO_MOVIMIENTO",
		GG_caracter_separacion[0]);
	int creado = metadata != NULL ? crearArchivo(ruta, metadata) : -1;
	sistema_memoria_liberar(metadata, NULL, 0);
	return creado >= 0;
}

static int inventario_agregar_reporte(
	const char *ruta,
	const char *codigo,
	const char *nombre,
	double cantidad_conteo,
	double cantidad_sistema,
	const char *ultimo_movimiento)
{
	char *conteo = variable_string("%.17g", cantidad_conteo);
	char *sistema = variable_string("%.17g", cantidad_sistema);
	if(conteo == NULL || sistema == NULL)
	{
		sistema_memoria_liberar(conteo, NULL, 0);
		sistema_memoria_liberar(sistema, NULL, 0);
		return 0;
	}
	const char *campos[] = {
		codigo, nombre, conteo, sistema,
		ultimo_movimiento != NULL ? ultimo_movimiento : ""
	};
	char *fila = join((char **)campos, 5, GG_caracter_separacion[0],
		NULL, 0);
	char *respuesta = fila != NULL
		? agregar_info_dividida(ruta, fila,
			"COD_BARRAS°NOMBRE°CANTIDAD_CONTEO°CANTIDAD_SISTEMA°ULTIMO_MOVIMIENTO")
		: NULL;
	int codigo_respuesta = -1;
	int correcto = inventario_codigo_respuesta(
		respuesta, &codigo_respuesta) && codigo_respuesta > 0;
	sistema_memoria_liberar(conteo, NULL, 0);
	sistema_memoria_liberar(sistema, NULL, 0);
	sistema_memoria_liberar(fila, NULL, 0);
	sistema_memoria_liberar(respuesta, NULL, 0);
	return correcto;
}

static int inventario_parsear_fecha(const char *fecha, long *valor)
{
	if(fecha == NULL || strlen(fecha) != 8 || valor == NULL) return 0;
	for(int i = 0; i < 8; ++i)
		if(fecha[i] < '0' || fecha[i] > '9') return 0;
	char *fin = NULL;
	errno = 0;
	long convertido = strtol(fecha, &fin, 10);
	if(errno == ERANGE || fin == fecha || *fin != '\0') return 0;
	*valor = convertido;
	return 1;
}

static int inventario_agregar_producto_desde_conteo(
	const char *ruta,
	const InventarioConteoItem *item,
	const char *fecha,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	char *cantidad = variable_string("%.17g", item->cantidad);
	if(cantidad == NULL) return 0;
	char *campos[32];
	for(int i = 0; i < 32; ++i) campos[i] = "";
	campos[0] = item->nombre;
	campos[1] = "0";
	campos[3] = "0";
	campos[4] = item->codigo;
	campos[5] = cantidad;
	campos[6] = "0";
	campos[8] = "PRODUCTO_PIEZA";
	campos[9] = "1";
	campos[10] = "INDIVIDUAL";
	campos[17] = (char *)fecha;
	char *fila = join(campos, 32, GG_caracter_separacion[0],
		error_o_datos_a_retornar, nivel_de_profundidad);
	sistema_memoria_liberar(cantidad, error_o_datos_a_retornar, 0);
	char *respuesta = fila != NULL
		? inventario_agrega_si_no_existe(ruta, fila,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	int codigo = -1;
	int correcto = inventario_codigo_respuesta(respuesta, &codigo) &&
		codigo > 0;
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	return correcto;
}

char *inventario_hacer_inventario(
	const char *direccion_archivo,
	const char *direccion_registro,
	const char *informacion,
	const char *fecha,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta_inventario = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	if(direccion_registro == NULL || direccion_registro[0] == '\0' ||
		informacion == NULL || fecha == NULL)
		return inventario_respuesta(0, "parametros_de_conteo_invalidos");
	long fecha_numero = 0;
	if(!inventario_parsear_fecha(fecha, &fecha_numero))
		return inventario_respuesta(0, "fecha_de_conteo_invalida");
	int partes_info_n = 0;
	char **partes_info = split(informacion,
		GG_caracter_separacion_funciones_espesificas[2],
		&partes_info_n, error_o_datos_a_retornar,
		nivel_de_profundidad);
	if(partes_info == NULL || partes_info_n != 2 ||
		(strcmp(partes_info[0], "MODIFICAR_INVENTARIO") != 0 &&
			strcmp(partes_info[0], "MOSTRAR_FALTANTES") != 0))
	{
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "formato_de_conteo_invalido");
	}
	int cantidad_filas_conteo = 0;
	char **filas_conteo = partes_info[1][0] != '\0'
		? split(partes_info[1],
			GG_caracter_separacion_funciones_espesificas[3],
			&cantidad_filas_conteo, error_o_datos_a_retornar,
			nivel_de_profundidad)
		: NULL;
	if(partes_info[1][0] != '\0' && filas_conteo == NULL)
	{
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "no_se_pudieron_leer_los_conteos");
	}
	char *movimientos = leer_todo_info_dividida(direccion_registro);
	if(movimientos == NULL)
	{
		liberarSplit(filas_conteo, cantidad_filas_conteo,
			error_o_datos_a_retornar, 0);
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(
			0, "no_se_pudo_leer_el_registro_de_conteo");
	}
	int cantidad_movimientos = 0;
	char **filas_movimiento = movimientos[0] != '\0'
		? split(movimientos,
			GG_caracter_separacion_funciones_espesificas[3],
			&cantidad_movimientos, error_o_datos_a_retornar,
			nivel_de_profundidad)
		: NULL;
	if(movimientos[0] != '\0' && filas_movimiento == NULL)
	{
		sistema_memoria_liberar(movimientos, error_o_datos_a_retornar, 0);
		liberarSplit(filas_conteo, cantidad_filas_conteo,
			error_o_datos_a_retornar, 0);
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(
			0, "no_se_pudieron_leer_los_movimientos");
	}
	int capacidad = cantidad_filas_conteo + cantidad_movimientos;
	InventarioConteoItem *items = capacidad > 0
		? sistema_memoria_reservar((size_t)capacidad * sizeof(*items),
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(capacidad > 0 && items == NULL)
	{
		liberarSplit(filas_movimiento, cantidad_movimientos,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(movimientos, error_o_datos_a_retornar, 0);
		liberarSplit(filas_conteo, cantidad_filas_conteo,
			error_o_datos_a_retornar, 0);
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "error_de_memoria");
	}
	for(int i = 0; i < capacidad; ++i)
	{
		items[i].codigo = NULL;
		items[i].nombre = NULL;
		items[i].fecha = NULL;
		items[i].cantidad = 0.0;
	}
	int cantidad_items = 0;
	int correcto = inventario_cargar_conteos(
		partes_info[1], 0, items, &cantidad_items, capacidad,
		error_o_datos_a_retornar, nivel_de_profundidad) &&
		inventario_cargar_conteos(
			movimientos, 1, items, &cantidad_items, capacidad,
			error_o_datos_a_retornar, nivel_de_profundidad);
	liberarSplit(filas_movimiento, cantidad_movimientos,
		error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(movimientos, error_o_datos_a_retornar, 0);
	liberarSplit(filas_conteo, cantidad_filas_conteo,
		error_o_datos_a_retornar, 0);
	if(!correcto || cantidad_items == 0)
	{
		inventario_liberar_items(items, cantidad_items);
		liberarSplit(partes_info, partes_info_n,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, cantidad_items == 0
			? "no_hay_productos_para_procesar"
			: "conteo_o_movimiento_invalido");
	}

	int procesados = 0;
	int fallidos = 0;
	int productos_agregados = 0;
	if(strcmp(partes_info[0], "MODIFICAR_INVENTARIO") == 0)
	{
		for(int i = 0; i < cantidad_items; ++i)
		{
			char *cantidad = variable_string("%.17g",
				items[i].cantidad);
			if(cantidad == NULL)
			{
				++fallidos;
				continue;
			}
			char *respuesta = incrementa_celda_busqueda_info_dividida(
				ruta_inventario, items[i].codigo, 5, "6", cantidad, NULL);
			sistema_memoria_liberar(cantidad,
				error_o_datos_a_retornar, 0);
			int codigo = -1;
			if(inventario_codigo_respuesta(respuesta, &codigo) &&
				codigo > 0)
				++procesados;
			else
				++fallidos;
			sistema_memoria_liberar(respuesta,
				error_o_datos_a_retornar, 0);
		}
	}
	else
	{
		char *reporte_sobrantes = inventario_reporte_path(
			direccion_registro, "SOBRANTES");
		char *reporte_faltantes = inventario_reporte_path(
			direccion_registro, "FALTANTES");
		char *reporte_no_fisico = inventario_reporte_path(
			direccion_registro, "NO_ENCONTRADOS");
		char *reporte_posibles = inventario_reporte_path(
			direccion_registro, "POSIBLES_FALTANTES");
		char *filas_inventario = leer_todo_info_dividida(ruta_inventario);
		int cantidad_filas_inventario = 0;
		char **inventario = filas_inventario != NULL &&
			filas_inventario[0] != '\0'
			? split(filas_inventario,
				GG_caracter_separacion_funciones_espesificas[3],
				&cantidad_filas_inventario, error_o_datos_a_retornar,
				nivel_de_profundidad)
			: NULL;
		unsigned char *encontrados = sistema_memoria_reservar(
			(size_t)cantidad_items, error_o_datos_a_retornar,
			nivel_de_profundidad);
		if(encontrados == NULL || reporte_sobrantes == NULL ||
			reporte_faltantes == NULL || reporte_no_fisico == NULL ||
			reporte_posibles == NULL || filas_inventario == NULL ||
			!inventario_iniciar_reporte(reporte_sobrantes) ||
			!inventario_iniciar_reporte(reporte_faltantes) ||
			!inventario_iniciar_reporte(reporte_no_fisico) ||
			!inventario_iniciar_reporte(reporte_posibles))
		{
			++fallidos;
		}
		else
		{
			for(int i = 0; i < cantidad_items; ++i) encontrados[i] = 0;
			for(int i = 0; i < cantidad_filas_inventario; ++i)
			{
				int n_campos = 0;
				char **producto = split(inventario[i],
					GG_caracter_separacion[0], &n_campos,
					error_o_datos_a_retornar, nivel_de_profundidad);
				double stock = 0.0;
				if(producto == NULL || n_campos <= 18 ||
					!ventas_parsear_numero(producto[6], &stock))
				{
					liberarSplit(producto, n_campos,
						error_o_datos_a_retornar, 0);
					++fallidos;
					continue;
				}
				int encontrado = 0;
				for(int j = 0; j < cantidad_items; ++j)
				{
					if(strcmp(items[j].codigo, producto[5]) == 0)
					{
						encontrado = 1;
						encontrados[j] = 1;
						int guardado = 1;
						if(items[j].cantidad > stock)
							guardado = inventario_agregar_reporte(
								reporte_sobrantes, producto[5], producto[1],
								items[j].cantidad, stock, producto[18]);
						else if(items[j].cantidad < stock)
							guardado = inventario_agregar_reporte(
								reporte_faltantes, producto[5], producto[1],
								items[j].cantidad, stock, producto[18]);
						if(guardado) ++procesados;
						else ++fallidos;
						break;
					}
				}
				if(!encontrado)
				{
					long ultimo_movimiento = 0;
					char *fin_movimiento = NULL;
					if(producto[18][0] != '\0')
						ultimo_movimiento = strtol(
							producto[18], &fin_movimiento, 10);
					const char *ruta_reporte =
						ultimo_movimiento < fecha_numero - 10000
						? reporte_no_fisico : reporte_posibles;
					if(inventario_agregar_reporte(
						ruta_reporte, producto[5], producto[1], 0.0,
						stock, producto[18]))
						++procesados;
					else
						++fallidos;
				}
				liberarSplit(producto, n_campos,
					error_o_datos_a_retornar, 0);
			}
			for(int i = 0; i < cantidad_items; ++i)
			{
				if(encontrados[i] || items[i].cantidad <= 0.0) continue;
				if(inventario_agregar_producto_desde_conteo(
					ruta_inventario, &items[i], fecha,
					error_o_datos_a_retornar, nivel_de_profundidad))
				{
					++productos_agregados;
					++procesados;
				}
				else
				{
					++fallidos;
				}
			}
		}
		sistema_memoria_liberar(encontrados,
			error_o_datos_a_retornar, 0);
		liberarSplit(inventario, cantidad_filas_inventario,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(filas_inventario,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(reporte_sobrantes,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(reporte_faltantes,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(reporte_no_fisico,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(reporte_posibles,
			error_o_datos_a_retornar, 0);
	}

	char *resumen = variable_string(
		"Procesados: %d; productos nuevos agregados: %d; fallidos: %d",
		procesados, productos_agregados, fallidos);
	if(GG_ruta_sesion_inventario_activa != NULL &&
		strcmp(GG_ruta_sesion_inventario_activa, direccion_registro) == 0 &&
		fallidos == 0)
	{
		sistema_memoria_liberar(GG_ruta_sesion_inventario_activa,
			error_o_datos_a_retornar, 0);
		GG_ruta_sesion_inventario_activa = NULL;
	}
	inventario_liberar_items(items, cantidad_items);
	liberarSplit(partes_info, partes_info_n,
		error_o_datos_a_retornar, 0);
	if(resumen == NULL)
		return inventario_respuesta(0, "error_de_memoria");
	char *respuesta = variable_string("%d%s%s",
		fallidos == 0 ? 1 : 0,
		GG_caracter_para_confirmacion_o_error[0], resumen);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Conciliación de inventario: %s\n", resumen);
	sistema_memoria_liberar(resumen, error_o_datos_a_retornar, 0);
	(void)nivel_de_profundidad;
	return respuesta;
}

static char *pruebas_recopilar_proveedores(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	char **campos = NULL;
	const char *prompts_alta[] = {
		"Ruta del archivo de proveedores (Enter usa la ruta configurada): ",
		"Nombre de la empresa: ",
		"Monto/dinero de compra asignado: ",
		"Nombre del encargado: ",
		"Dirección: ",
		"Ciudad: ",
		"Estado: ",
		"Código postal: ",
		"País: ",
		"Correo electrónico: ",
		"Teléfono del encargado: ",
		"Teléfono de la empresa: ",
		"Tipo de proveedor: ",
		"Productos o servicios suministrados: ",
		"Cuenta bancaria: ",
		"Ubicación GPS: ",
		"Notas: ",
		"Recordatorio: ",
		"Estado (ACTIVO/INACTIVO): ",
		"Escriba SI para confirmar el alta: "
	};
	const char *prompts_ruta[] = {
		"Ruta del archivo de proveedores (Enter usa la ruta configurada): "
	};
	const char **prompts = NULL;
	int cantidad = 0;

	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== PRUEBAS DE PROVEEDORES ===\n"
		"1. Registrar proveedor\n"
		"2. Extraer nombres de proveedores\n"
		"3. Extraer nombres y dinero asignado para compras\n"
		"4. Volver\n");
	if(!pm_pedir_entero("Seleccione una opción: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad))
		return NULL;
	if(opcion == 4) return pm_copiar_texto("0");
	if(opcion == 1)
	{
		prompts = prompts_alta;
		cantidad = 20;
	}
	else if(opcion == 2 || opcion == 3)
	{
		prompts = prompts_ruta;
		cantidad = 1;
	}
	else
	{
		return NULL;
	}
	if(!pruebas_leer_campos(prompts, cantidad, &campos,
		error_o_datos_a_retornar, nivel_de_profundidad))
	{
		pruebas_liberar_campos(campos, cantidad,
			error_o_datos_a_retornar);
		return NULL;
	}
	if(campos[0][0] == '\0')
	{
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = pm_copiar_texto(GG_secciones_negocio[3].ruta);
	}
	if(campos[0] == NULL)
	{
		pruebas_liberar_campos(campos, cantidad,
			error_o_datos_a_retornar);
		return NULL;
	}
	if(opcion == 1)
	{
		double monto = 0.0;
		if(campos[1][0] == '\0' ||
			!ventas_parsear_numero(campos[2], &monto) || monto < 0.0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"El nombre del proveedor y el monto numérico no pueden ser inválidos.\n");
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return NULL;
		}
		if(strcmp(campos[18], "ACTIVO") != 0 &&
			strcmp(campos[18], "INACTIVO") != 0 &&
			strcmp(campos[18], "activo") != 0 &&
			strcmp(campos[18], "inactivo") != 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"El estado debe ser ACTIVO o INACTIVO.\n");
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return NULL;
		}
		if(strcmp(campos[19], "SI") != 0 &&
			strcmp(campos[19], "si") != 0)
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Alta cancelada; no se modificó el archivo.\n");
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return pm_copiar_texto("0");
		}
	}
	for(int i = 0; i < cantidad; ++i)
	{
		if(campos[i] == NULL || !pruebas_campo_serializable(campos[i]) ||
			(i > 0 && i < 19 &&
				strchr(campos[i], GG_caracter_separacion[0][0]) != NULL))
		{
			sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
				"Los campos no pueden contener separadores del protocolo ni saltos de línea.\n");
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return NULL;
		}
	}
	char *serializado = pruebas_serializar(
		opcion, campos, cantidad, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
	return serializado;
}

static int ejecutar_prueba_proveedores(
	int opcion,
	char **argumentos,
	int cantidad,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = GG_secciones_negocio[3].ruta;
	char *respuesta = NULL;
	int codigo = -1;
	if(opcion == 1 && cantidad == 20 &&
		(strcmp(argumentos[19], "SI") == 0 ||
			strcmp(argumentos[19], "si") == 0))
	{
		char *datos = join(argumentos + 1, 18,
			GG_caracter_separacion[0],
			error_o_datos_a_retornar, nivel_de_profundidad);
		respuesta = datos != NULL
			? registro_provedores_cod3_r_(
				argumentos[0][0] != '\0' ? argumentos[0] : ruta,
				datos, error_o_datos_a_retornar, nivel_de_profundidad)
			: NULL;
		sistema_memoria_liberar(datos, error_o_datos_a_retornar, 0);
	}
	else if((opcion == 2 || opcion == 3) && cantidad == 1)
	{
		const char *ruta_consulta = argumentos[0][0] != '\0'
			? argumentos[0] : ruta;
		respuesta = opcion == 2
			? extraer_nombre_provedores(
				ruta_consulta, error_o_datos_a_retornar,
				nivel_de_profundidad)
			: extraer_nombre_provedores_y_dinero(
				ruta_consulta, error_o_datos_a_retornar,
				nivel_de_profundidad);
	}
	else
	{
		return -1;
	}
	int correcto = respuesta != NULL &&
		compras_respuesta_codigo(respuesta, &codigo, NULL, NULL,
			error_o_datos_a_retornar, nivel_de_profundidad) &&
		codigo > 0;
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado de proveedores: %s\n",
		respuesta != NULL ? respuesta : "(no se pudo ejecutar)");
	sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}

char *registro_provedores_cod3_r_(
	const char *direccion_archivo,
	const char *datos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[3].ruta;
	int cantidad_datos = 0;
	char **datos_campos = datos != NULL
		? split(datos, GG_caracter_separacion[0], &cantidad_datos,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(datos_campos == NULL || cantidad_datos != 18)
	{
		liberarSplit(datos_campos, cantidad_datos,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "se_requieren_18_campos_de_proveedor");
	}
	double monto = 0.0;
	if(datos_campos[0][0] == '\0' ||
		!ventas_parsear_numero(datos_campos[1], &monto) || monto < 0.0)
	{
		liberarSplit(datos_campos, cantidad_datos,
			error_o_datos_a_retornar, 0);
		return inventario_respuesta(
			0, "nombre_o_monto_de_proveedor_invalido");
	}
	char *campos[23];
	for(int i = 0; i < 23; ++i) campos[i] = "";
	campos[0] = datos_campos[0];
	campos[1] = datos_campos[2];
	for(int i = 2; i <= 16; ++i)
		campos[i] = datos_campos[i + 1];
	campos[20] = datos_campos[1];
	char *fila = join(campos, 23, GG_caracter_separacion[0],
		error_o_datos_a_retornar, nivel_de_profundidad);
	char *respuesta = fila != NULL
		? agregar_info_dividida(
			ruta, fila, GG_secciones_negocio[3].columnas)
		: NULL;
	sistema_memoria_liberar(fila, error_o_datos_a_retornar, 0);
	liberarSplit(datos_campos, cantidad_datos,
		error_o_datos_a_retornar, 0);
	if(respuesta == NULL)
		return inventario_respuesta(0, "no_se_pudo_guardar_el_proveedor");
	return respuesta;
}

static char *proveedores_extraer_columna(
	const char *direccion_archivo,
	int columna,
	int columna_dinero,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[3].ruta;
	char *filas = leer_todo_info_dividida(ruta);
	if(filas == NULL)
		return inventario_respuesta(0, "no_se_pudo_leer_el_archivo_de_proveedores");
	int cantidad_filas = 0;
	char **lista = filas[0] != '\0'
		? split(filas, GG_caracter_separacion_funciones_espesificas[3],
			&cantidad_filas, error_o_datos_a_retornar,
			nivel_de_profundidad)
		: NULL;
	if(filas[0] != '\0' && lista == NULL)
	{
		sistema_memoria_liberar(filas, error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "no_se_pudieron_leer_los_proveedores");
	}
	char *resultado = NULL;
	int correcto = 1;
	for(int i = 0; i < cantidad_filas && correcto; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(lista[i], GG_caracter_separacion[0],
			&cantidad_campos, error_o_datos_a_retornar,
			nivel_de_profundidad);
		if(campos == NULL || columna >= cantidad_campos ||
			(columna_dinero >= 0 && columna_dinero >= cantidad_campos))
		{
			correcto = 0;
		}
		else
		{
			char *valor = columna_dinero >= 0
				? variable_string("%s%s%s", campos[columna],
					GG_caracter_separacion[2], campos[columna_dinero])
				: pm_copiar_texto(campos[columna]);
			if(valor == NULL ||
				concatenar_formato_separado_por_variable(
					&resultado,
					resultado != NULL
						? GG_caracter_separacion[1] : NULL,
					"%s", valor) < 0)
				correcto = 0;
			sistema_memoria_liberar(valor,
				error_o_datos_a_retornar, 0);
		}
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
	}
	liberarSplit(lista, cantidad_filas,
		error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(filas, error_o_datos_a_retornar, 0);
	if(!correcto)
	{
		sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "fila_de_proveedor_invalida");
	}
	if(resultado == NULL) resultado = pm_copiar_texto("");
	if(resultado == NULL)
		return inventario_respuesta(0, "error_de_memoria");
	char *respuesta = variable_string("1%s%s",
		GG_caracter_para_confirmacion_o_error[0], resultado);
	sistema_memoria_liberar(resultado, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *extraer_nombre_provedores(
	const char *direccion_archivo,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	return proveedores_extraer_columna(direccion_archivo, 1, -1,
		error_o_datos_a_retornar, nivel_de_profundidad);
}

char *extraer_nombre_provedores_y_dinero(
	const char *direccion_archivo,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	return proveedores_extraer_columna(direccion_archivo, 1, 21,
		error_o_datos_a_retornar, nivel_de_profundidad);
}

static int analisis_reemplazar_celda(
	char **campos,
	int cantidad_campos,
	int columna,
	const char *valor)
{
	if(campos == NULL || valor == NULL || columna < 0 ||
		columna >= cantidad_campos)
		return 0;
	char *copia = pm_copiar_texto(valor);
	if(copia == NULL) return 0;
	sistema_memoria_liberar(campos[columna], NULL, 0);
	campos[columna] = copia;
	return 1;
}

static int analisis_numero_celda(
	const char *texto,
	double valor_vacio,
	double *valor)
{
	if(texto == NULL || valor == NULL) return 0;
	if(texto[0] == '\0')
	{
		*valor = valor_vacio;
		return 1;
	}
	return ventas_parsear_numero(texto, valor);
}

static int analisis_enriquecer_fila(
	char **campos,
	int cantidad_campos,
	int columna_historial,
	int columna_promedio,
	int columna_veces,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(campos == NULL || cantidad_campos < 11 ||
		columna_historial < 0 || columna_historial >= cantidad_campos ||
		columna_promedio < 0 || columna_promedio >= cantidad_campos ||
		columna_veces < 0 || columna_veces >= cantidad_campos)
		return 0;

	int cantidad_semanas = 0;
	char **historial = split(campos[columna_historial],
		GG_caracter_separacion[1], &cantidad_semanas,
		error_o_datos_a_retornar, nivel_de_profundidad);
	if(historial == NULL || cantidad_semanas == 0)
	{
		liberarSplit(historial, cantidad_semanas,
			error_o_datos_a_retornar, 0);
		return 0;
	}

	double semanas[4] = {0.0, 0.0, 0.0, 0.0};
	int semanas_validas = 0;
	for(int i = 0; i < cantidad_semanas && semanas_validas < 4; ++i)
	{
		if(historial[i][0] == '\0') break;
		if(!ventas_parsear_numero(historial[i], &semanas[semanas_validas]) ||
			semanas[semanas_validas] < 0.0)
		{
			liberarSplit(historial, cantidad_semanas,
				error_o_datos_a_retornar, 0);
			return 0;
		}
		++semanas_validas;
	}
	liberarSplit(historial, cantidad_semanas,
		error_o_datos_a_retornar, 0);

	double promedio = 0.0;
	double veces_numero = 0.0;
	if(!analisis_numero_celda(campos[columna_promedio], 0.0, &promedio) ||
		!analisis_numero_celda(campos[columna_veces], 0.0, &veces_numero) ||
		promedio < 0.0 || veces_numero < 0.0)
		return 0;
	int veces = veces_numero >= 4.0 ? 4 : (int)veces_numero;
	if(promedio == 0.0 && semanas_validas > 0)
		promedio = semanas[0];
	else if(semanas_validas > 0 && semanas[0] > promedio)
	{
		if(veces < 4) ++veces;
		if(veces >= 4 && semanas_validas == 4)
			promedio = (semanas[0] + semanas[1] +
				semanas[2] + semanas[3]) / 4.0;
	}
	else
	{
		veces = 0;
	}

	char *promedio_texto = variable_string("%.17g", promedio);
	char *veces_texto = variable_string("%d", veces);
	int correcto = promedio_texto != NULL && veces_texto != NULL &&
		analisis_reemplazar_celda(
			campos, cantidad_campos, columna_promedio, promedio_texto) &&
		analisis_reemplazar_celda(
			campos, cantidad_campos, columna_veces, veces_texto);
	sistema_memoria_liberar(promedio_texto, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(veces_texto, error_o_datos_a_retornar, 0);
	return correcto;
}

static int analisis_agregar_datos_inventario(
	char **campos,
	int cantidad_campos,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(campos == NULL || cantidad_campos < 11) return 0;
	if(campos[0][0] != '\0')
	{
		char *respuesta = inventario_buscar(
			GG_secciones_negocio[1].ruta, campos[0], "",
			error_o_datos_a_retornar, nivel_de_profundidad);
		int codigo = 0;
		char **partes = NULL;
		int cantidad_partes = 0;
		if(respuesta == NULL ||
			!compras_respuesta_codigo(respuesta, &codigo, &partes,
				&cantidad_partes, error_o_datos_a_retornar,
				nivel_de_profundidad))
		{
			sistema_memoria_liberar(respuesta,
				error_o_datos_a_retornar, 0);
			return 0;
		}
		if(codigo > 0 && cantidad_partes >= 2)
		{
			int cantidad_inventario = 0;
			char **producto = split(partes[1], GG_caracter_separacion[0],
				&cantidad_inventario, error_o_datos_a_retornar,
				nivel_de_profundidad);
			int correcto = producto != NULL && cantidad_inventario > 9 &&
				analisis_reemplazar_celda(
					campos, cantidad_campos, 8, producto[6]) &&
				analisis_reemplazar_celda(
					campos, cantidad_campos, 9, producto[9]) &&
				analisis_reemplazar_celda(
					campos, cantidad_campos, 10, producto[7]);
			liberarSplit(producto, cantidad_inventario,
				error_o_datos_a_retornar, 0);
			liberarSplit(partes, cantidad_partes,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta,
				error_o_datos_a_retornar, 0);
			return correcto;
		}
		if(codigo == 0 && cantidad_partes >= 2 &&
			strstr(partes[1], "no_encontro") == NULL)
		{
			liberarSplit(partes, cantidad_partes,
				error_o_datos_a_retornar, 0);
			sistema_memoria_liberar(respuesta,
				error_o_datos_a_retornar, 0);
			return 0;
		}
		liberarSplit(partes, cantidad_partes,
			error_o_datos_a_retornar, 0);
		sistema_memoria_liberar(respuesta,
			error_o_datos_a_retornar, 0);
	}

	for(int i = 8; i <= 10; ++i)
	{
		if(campos[i][0] == '\0' &&
			!analisis_reemplazar_celda(
				campos, cantidad_campos, i, i == 9 ? "" : "0"))
			return 0;
	}
	return 1;
}

char *existe_informacion(
	const char *direccion_archivo,
	const char *informacion,
	int columna,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	const char *ruta = direccion_archivo != NULL &&
		direccion_archivo[0] != '\0'
		? direccion_archivo : GG_secciones_negocio[1].ruta;
	if(informacion == NULL || informacion[0] == '\0' || columna < 0)
		return inventario_respuesta(0, "parametros_de_busqueda_invalidos");
	char *respuesta = buscar_info_dividida(ruta, informacion, columna, -1);
	int codigo = 0;
	char **partes = NULL;
	int cantidad = 0;
	if(respuesta == NULL ||
		!compras_respuesta_codigo(respuesta, &codigo, &partes, &cantidad,
			error_o_datos_a_retornar, nivel_de_profundidad))
	{
		sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "error_al_buscar_informacion");
	}
	char *resultado = NULL;
	if(codigo > 0)
		resultado = inventario_respuesta(1, "encontrado");
	else if(cantidad >= 2 && strstr(partes[1], "no_encontro") != NULL)
		resultado = inventario_respuesta(-1, "no_encontrado");
	else
		resultado = inventario_respuesta(0,
			cantidad >= 2 ? partes[1] : "error_al_buscar_informacion");
	liberarSplit(partes, cantidad, error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	return resultado;
}

typedef struct
{
	char *fila;
	double ranking;
	double nivel_necesidad;
} AnalisisFilaCompra;

static void analisis_liberar_filas(
	AnalisisFilaCompra *filas,
	int cantidad)
{
	if(filas == NULL) return;
	for(int i = 0; i < cantidad; ++i)
		sistema_memoria_liberar(filas[i].fila, NULL, 0);
	sistema_memoria_liberar(filas, NULL, 0);
}

char *calculo_pred_compra(
	char **ranking,
	int cantidad_filas,
	int columna_ranking,
	int columna_promedio_compra,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(ranking == NULL || cantidad_filas < 0 ||
		columna_ranking < 0 || columna_promedio_compra < 0)
		return inventario_respuesta(0, "parametros_de_ranking_invalidos");
	if((size_t)cantidad_filas >
		(size_t)-1 / sizeof(AnalisisFilaCompra))
		return inventario_respuesta(0, "ranking_demasiado_grande");
	AnalisisFilaCompra *filas = cantidad_filas > 0
		? (AnalisisFilaCompra *)sistema_memoria_reservar(
			(size_t)cantidad_filas * sizeof(*filas),
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(cantidad_filas > 0 && filas == NULL)
		return inventario_respuesta(0, "error_de_memoria");
	if(filas != NULL)
		memset(filas, 0, (size_t)cantidad_filas * sizeof(*filas));

	for(int i = 0; i < cantidad_filas; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(ranking[i], GG_caracter_separacion[0],
			&cantidad_campos, error_o_datos_a_retornar,
			nivel_de_profundidad);
		double stock = 0.0;
		double promedio = 0.0;
		double valor_ranking = 0.0;
		int valido = campos != NULL && cantidad_campos >= 11 &&
			columna_ranking < cantidad_campos &&
			columna_promedio_compra < cantidad_campos &&
			analisis_numero_celda(campos[8], 0.0, &stock) &&
			analisis_numero_celda(campos[columna_promedio_compra],
				0.0, &promedio) &&
			analisis_numero_celda(campos[columna_ranking],
				0.0, &valor_ranking) &&
			stock >= 0.0 && promedio >= 0.0;
		if(valido)
		{
			filas[i].fila = pm_copiar_texto(ranking[i]);
			filas[i].ranking = valor_ranking;
			filas[i].nivel_necesidad =
				(double)posicion_rango_nesecidad(promedio, stock);
			valido = filas[i].fila != NULL;
		}
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
		if(!valido)
		{
			analisis_liberar_filas(filas, cantidad_filas);
			return inventario_respuesta(0,
				"fila_de_ranking_o_valor_numerico_invalido");
		}
	}

	for(int i = 0; i < cantidad_filas; ++i)
	{
		for(int j = i + 1; j < cantidad_filas; ++j)
		{
			if(filas[j].nivel_necesidad < filas[i].nivel_necesidad ||
				(filas[j].nivel_necesidad == filas[i].nivel_necesidad &&
					filas[j].ranking < filas[i].ranking))
			{
				AnalisisFilaCompra temporal = filas[i];
				filas[i] = filas[j];
				filas[j] = temporal;
			}
		}
	}

	char *lista = NULL;
	int correcto = 1;
	for(int i = 0; i < cantidad_filas && correcto; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(filas[i].fila,
			GG_caracter_separacion[0], &cantidad_campos,
			error_o_datos_a_retornar, nivel_de_profundidad);
		double stock = 0.0;
		double promedio = 0.0;
		double costo = 0.0;
		correcto = campos != NULL && cantidad_campos >= 11 &&
			analisis_numero_celda(campos[8], 0.0, &stock) &&
			analisis_numero_celda(campos[columna_promedio_compra],
				0.0, &promedio) &&
			analisis_numero_celda(campos[10], 0.0, &costo);
		if(correcto)
		{
			double por_comprar = promedio > stock ? promedio - stock : 0.0;
			char *fila_salida = variable_string(
				"%.17g%s%s%s%s%s%s%s%.17g%s%.17g",
				por_comprar, GG_caracter_separacion[0],
				campos[1], GG_caracter_separacion[0],
				campos[0], GG_caracter_separacion[0],
				campos[3], GG_caracter_separacion[0],
				stock, GG_caracter_separacion[0], costo);
			if(fila_salida == NULL)
				correcto = 0;
			else
			{
				char *nueva_lista = lista == NULL
					? pm_copiar_texto(fila_salida)
					: variable_string("%s%s%s", lista,
						GG_caracter_separacion_funciones_espesificas[0],
						fila_salida);
				if(nueva_lista == NULL)
					correcto = 0;
				else
				{
					sistema_memoria_liberar(lista,
						error_o_datos_a_retornar, 0);
					lista = nueva_lista;
				}
				sistema_memoria_liberar(fila_salida,
					error_o_datos_a_retornar, 0);
			}
		}
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
	}
	analisis_liberar_filas(filas, cantidad_filas);
	if(!correcto)
	{
		sistema_memoria_liberar(lista, error_o_datos_a_retornar, 0);
		return inventario_respuesta(0, "no_se_pudo_calcular_la_prediccion");
	}
	char *respuesta = inventario_respuesta(1, lista != NULL ? lista : "");
	sistema_memoria_liberar(lista, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *prediccion_arreglo_compra(
	char **todo_el_ranking,
	int cantidad_filas,
	int columna_historial,
	int columna_ranking,
	int columna_promedio_compra,
	int columna_veces_supera_promedio,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(todo_el_ranking == NULL || cantidad_filas < 0 ||
		columna_historial < 0 || columna_ranking < 0 ||
		columna_promedio_compra < 0 ||
		columna_veces_supera_promedio < 0)
		return inventario_respuesta(0,
			"parametros_de_prediccion_invalidos");
	if((size_t)cantidad_filas > (size_t)-1 / sizeof(*todo_el_ranking))
		return inventario_respuesta(0, "ranking_demasiado_grande");
	char **enriquecido = cantidad_filas > 0
		? (char **)sistema_memoria_reservar(
			(size_t)cantidad_filas * sizeof(*enriquecido),
			error_o_datos_a_retornar, nivel_de_profundidad)
		: NULL;
	if(cantidad_filas > 0 && enriquecido == NULL)
		return inventario_respuesta(0, "error_de_memoria");
	if(enriquecido != NULL)
		memset(enriquecido, 0, (size_t)cantidad_filas * sizeof(*enriquecido));

	int correcto = 1;
	for(int i = 0; i < cantidad_filas && correcto; ++i)
	{
		int cantidad_campos = 0;
		char **campos = split(todo_el_ranking[i],
			GG_caracter_separacion[0], &cantidad_campos,
			error_o_datos_a_retornar, nivel_de_profundidad);
		correcto = campos != NULL &&
			analisis_enriquecer_fila(campos, cantidad_campos,
				columna_historial, columna_promedio_compra,
				columna_veces_supera_promedio,
				error_o_datos_a_retornar, nivel_de_profundidad) &&
			analisis_agregar_datos_inventario(
				campos, cantidad_campos, error_o_datos_a_retornar,
				nivel_de_profundidad);
		if(correcto)
			enriquecido[i] = join(campos, cantidad_campos,
				GG_caracter_separacion[0],
				error_o_datos_a_retornar, nivel_de_profundidad);
		if(enriquecido[i] == NULL) correcto = 0;
		liberarSplit(campos, cantidad_campos,
			error_o_datos_a_retornar, 0);
	}
	char *respuesta = correcto
		? calculo_pred_compra(enriquecido, cantidad_filas,
			columna_ranking, columna_promedio_compra,
			error_o_datos_a_retornar, nivel_de_profundidad)
		: inventario_respuesta(0,
			"no_se_pudo_preparar_una_fila_del_ranking");
	for(int i = 0; i < cantidad_filas; ++i)
		sistema_memoria_liberar(enriquecido[i],
			error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(enriquecido, error_o_datos_a_retornar, 0);
	return respuesta;
}

char *prediccion_archivo_compra(
	const char *direccion_ranking,
	int columna_historial,
	int columna_ranking,
	int columna_promedio_compra,
	int columna_veces_supera_promedio,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	if(direccion_ranking == NULL || direccion_ranking[0] == '\0')
		return inventario_respuesta(0, "ruta_de_ranking_vacia");
	int total_lineas = 0;
	char **lineas = leer_archivo(direccion_ranking, &total_lineas);
	if(lineas == NULL)
		return inventario_respuesta(0, "no_se_pudo_leer_el_archivo_de_ranking");
	int inicio = GG_indice_donde_comensar;
	if(inicio < 0) inicio = 0;
	if(inicio > total_lineas) inicio = total_lineas;
	char *respuesta = prediccion_arreglo_compra(
		lineas + inicio, total_lineas - inicio, columna_historial,
		columna_ranking, columna_promedio_compra,
		columna_veces_supera_promedio, error_o_datos_a_retornar,
		nivel_de_profundidad);
	free_lineas(lineas, total_lineas);
	return respuesta;
}

int posicion_rango_nesecidad(
	double promedio_compra,
	double cantidad_inventario)
{
	if(promedio_compra <= 0.0 || cantidad_inventario < 0.0)
		return 4;
	if(cantidad_inventario < promedio_compra * 0.3) return 1;
	if(cantidad_inventario < promedio_compra * 0.6) return 2;
	if(cantidad_inventario < promedio_compra * 0.9) return 3;
	return 4;
}

static char *pruebas_recopilar_analisis_compras(
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	int opcion = 0;
	char **campos = NULL;
	const char *prompts_busqueda[] = {
		"Ruta del inventario dividido (Enter usa la ruta configurada): ",
		"Valor que desea buscar: ",
		"Columna de búsqueda (índice desde 0): "
	};
	const char *prompts_prediccion[] = {
		"Ruta del archivo de ranking: ",
		"Columna del historial semanal (índice desde 0): ",
		"Columna del ranking numérico (índice desde 0): ",
		"Columna del promedio de compra (índice desde 0): ",
		"Columna de semanas sobre el promedio (índice desde 0): "
	};
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"\n=== ANÁLISIS DE DATOS Y COMPRAS ===\n"
		"1. Buscar si existe un valor en el inventario\n"
		"2. Predecir compras desde un archivo de ranking\n"
		"3. Volver\n");
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Ranking esperado: 0 código | 1 nombre | 2 ventas | 3 proveedor | "
		"4 historial | 5 ranking | 6 promedio | 7 semanas sobre promedio | "
		"8 inventario | 9 tipo | 10 costo\n");
	if(!pm_pedir_entero("Seleccione una opción: ", &opcion,
		error_o_datos_a_retornar, nivel_de_profundidad))
		return NULL;
	if(opcion == 3) return pm_copiar_texto("0");
	int cantidad = opcion == 1 ? 3 : (opcion == 2 ? 5 : 0);
	const char **prompts = opcion == 1
		? prompts_busqueda : prompts_prediccion;
	if(cantidad == 0 ||
		!pruebas_leer_campos(prompts, cantidad, &campos,
			error_o_datos_a_retornar, nivel_de_profundidad))
	{
		pruebas_liberar_campos(campos, cantidad,
			error_o_datos_a_retornar);
		return NULL;
	}
	if(opcion == 1 && campos[0][0] == '\0')
	{
		sistema_memoria_liberar(campos[0], error_o_datos_a_retornar, 0);
		campos[0] = pm_copiar_texto(GG_secciones_negocio[1].ruta);
	}
	int indices_validos = campos[0] != NULL;
	if(opcion == 1 && indices_validos)
	{
		int indice = -1;
		indices_validos = texto_a_int_seguro(campos[2], &indice) == 0 &&
			indice >= 0;
	}
	else
	{
		for(int i = 1; i < cantidad && indices_validos; ++i)
		{
			int indice = -1;
			indices_validos = texto_a_int_seguro(campos[i], &indice) == 0 &&
				indice >= 0;
		}
	}
	if(!indices_validos ||
		(opcion == 1 && campos[1][0] == '\0') ||
		(opcion == 2 && campos[0][0] == '\0'))
	{
		sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
			"Ruta, valor o índice de columna inválido.\n");
		pruebas_liberar_campos(campos, cantidad,
			error_o_datos_a_retornar);
		return NULL;
	}
	for(int i = 0; i < cantidad; ++i)
	{
		if(campos[i] == NULL || !pruebas_campo_serializable(campos[i]))
		{
			pruebas_liberar_campos(campos, cantidad,
				error_o_datos_a_retornar);
			return NULL;
		}
	}
	char *serializado = pruebas_serializar(
		opcion, campos, cantidad, error_o_datos_a_retornar);
	pruebas_liberar_campos(campos, cantidad, error_o_datos_a_retornar);
	return serializado;
}

static int ejecutar_prueba_analisis_compras(
	int opcion,
	char **argumentos,
	int cantidad,
	char *error_o_datos_a_retornar,
	int nivel_de_profundidad)
{
	char *respuesta = NULL;
	if(opcion == 1 && cantidad == 3)
	{
		int columna = -1;
		if(texto_a_int_seguro(argumentos[2], &columna) != 0 ||
			columna < 0)
			return -1;
		respuesta = existe_informacion(
			argumentos[0], argumentos[1], columna,
			error_o_datos_a_retornar, nivel_de_profundidad);
	}
	else if(opcion == 2 && cantidad == 5)
	{
		int columnas[4];
		for(int i = 0; i < 4; ++i)
		{
			if(texto_a_int_seguro(argumentos[i + 1], &columnas[i]) != 0 ||
				columnas[i] < 0)
				return -1;
		}
		respuesta = prediccion_archivo_compra(
			argumentos[0], columnas[0], columnas[1], columnas[2],
			columnas[3], error_o_datos_a_retornar,
			nivel_de_profundidad);
	}
	else
	{
		return -1;
	}
	int codigo = 0;
	char **partes = NULL;
	int cantidad_partes = 0;
	int correcto = respuesta != NULL &&
		compras_respuesta_codigo(respuesta, &codigo, &partes,
			&cantidad_partes, error_o_datos_a_retornar,
			nivel_de_profundidad) &&
		(opcion == 1 ? codigo != 0 : codigo > 0);
	sistema_consola_escribir_formato(error_o_datos_a_retornar, 0,
		"Resultado del análisis: %s\n",
		respuesta != NULL ? respuesta : "(no se pudo ejecutar)");
	liberarSplit(partes, cantidad_partes,
		error_o_datos_a_retornar, 0);
	sistema_memoria_liberar(respuesta, error_o_datos_a_retornar, 0);
	return correcto ? 0 : -1;
}