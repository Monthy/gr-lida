/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QLocale>

#include "g_structs.h"
#include "crc32qt.h"
#include "qcheckcombobox.h"

enum TipoCfg {
	ExportDbx    = 0,
	ExportGrlida = 1,
	ExportDfend  = 2
};

enum TipoHash {
	hashNo    = 0,
	hashAll   = 1,
	hashCrc32 = 2,
	hashSha1  = 3,
	hashMd5   = 4
};

enum TipoListCbx {
	CbxListIcon = 0,
	CbxListLng  = 1,
	CbxListProf = 2,
	CbxListJs   = 3,
	CbxListJsi  = 4
};

enum TipoListTw {
	TwListSvm        = 0,
	TwListEmu        = 1,
	TwListTraduccion = 2,
	TwListSmile      = 3,
	TwList2col       = 4,
	TwList3col       = 5
};

class Funciones : public QObject
{
	Q_OBJECT
public:
	Funciones();
	~Funciones();

// Versión del GR-lida
	QString versionGrl(){return "0.11.0";}

// Versiones de los emuladores que soporta el GR-lida
// DOSBox
	QString versionDbx(){return "0.74";}
// ScummVM
	QString versionSvm(){return "1.5.0";}
// VDMSound
	QString versionVdms(){return "2.0.4";}

// Compara distintas version y devuelve True si es nueva o mayor
	bool version_compare(QString ver_old, QString ver_new);
// Crea una copia de seguridad de la base de datos
	void crearCopiaSeguridad_DB(QString dir, QString version_grl);
// Convierte de número a texto
	QString	IntToStr(int num);
// Convierte de texto a número
	int StrToInt(QString str);
// Convierte texto "true", "yes" a tipo bool
	bool StrToBool(QString str);
// Convierte bool a texto "true", "yes" indicando el tipo
	QString BoolToStr(bool estado, bool type_yes = false);
// Convierte bytes
	QString covertir_bytes(float size);
// Convierte la lista de un QListWidget a QStringList
	QStringList lwToStrList(QListWidget *lwlist);
// Devuelve el tiempo actual, formato unix
	QString getTime();
// Convierte fecha 'dd/MM/yyyy HH:mm:ss' a formato unix
	QString setTime(QString fecha, QString formato = "dd/MM/yyyy HH:mm:ss");
// Devuelve la hora y la fecha actual
	QString HoraFechaActual(QString fecha, QString formato = "dd/MM/yyyy HH:mm:ss");
// Devuelve una la forma correcta
	QString str_correcto(QString str, QString str_ini = "", QString str_end = "");
	QString str_start(QString str, QString str_ini);
	QString str_end(QString str, QString str_end);
// Elimina caracteres no permitidos por windows por ejemplo
	QString eliminar_caracteres(QString str, QString cambiar = "", QString sep = ",");
// Devuelve si el archivos admiten traducción
	QString getArchivoIsLng(QString archivo);
// Devuelve si el archivos es de 3 columnas
	bool getArchivoIs3Col(QString archivo);
// Busca y obtiene la ruta de un archivo
	QString getFindFile(QString dir, QString filename);
// Obtiene una la lista de archivos
	QFileInfoList getListFiles(QString dir, QStringList filter);
// Obtiene información del archivo
	stFileInfo getInfoFile(QString filename, TipoHash hash = hashNo);
// Obtiene el Hash del archivo
	QString getHashFile(QString filename, TipoHash hash = hashSha1);
// Devuelve un icono montaje
	QString getIconMount(QString theme, QString tipoDrive, QString select_mount = "");
// Leer archivo de texto
	QString leerArchivo(QString filename, const QByteArray code = "");
// Copia un archivo de un directorio a otro.
	bool copiarArchivo(QString origen, QString destino, bool permisos = false);
// Elimina un directorio si existe
	bool eliminarDirectorio(QString nameDir);
// Renombra un directorio existe
	void renombrarDirectorio(QString oldName, QString newName);
// Comprueba si un directorio existe y sino lo crea
	bool comprobarDirectorio(QString nameDir, bool info = false);

// Devuelve el directorio que usa el GR-lida
	QString GRlidaHomePath();
	QString DirApp(){ return stDirApp; }
// Rutas relativas
	QString setDirRelative(QString dir, QString carpeta = "");
	QString getDirRelative(QString dir, QString carpeta = "");
// Devuelve el directorio del Theme a usar
	void setTheme(QString theme);
	QString Theme(){ return stTheme; }
	QString ThemeApp(){ return stThemeApp; }
// Devuelve el directorio del Idioma a usar
	void setIdioma(QString lng = "es_ES");
	QString Idioma(){ return stLngSelect; }
// Carga la hoja de estilo y reemplazando el comodin <theme> por el que se use
	QString myStyleSheet(bool isList = false);

// Carga la configuración de los iconos de la lista en modo Icono
	stLwIconCfg cargarListWidgetIconConf(QString tabla = "dbgrl");
// Carga la configuración del GR-lida
	stGrlCfg cargarGRLConfig(QString iniFileName);
// Guarda la configuración del GR-lida
	void guardarGRLConfig(QString iniFileName, stGrlCfg config);
	void guardarKeyGRLConfig(QString iniFileName, QString grupo, QString key, QString value);

// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
	QString ventanaAbrirArchivos(QString caption, QString dir, QString dir_relative, QString filter, QString *selectedFilter = 0, bool isSave = false);
// Obtiene la dirección de una carpeta atraves de QFileDialog
	QString ventanaDirectorios(QString caption, QString dir, QString dir_relative = "");
// Abre un archivo con el programa predeterminado
	bool abrirArchivo(QString filePath);
// Abre un directorio
	bool abrirDirectorio(QString dirPath);

// Crea una ventana para mostrar Información
	void ventanaInfo(QString titulo, QString icono, int w, int h, QString contenido, bool isTextoPlano = false);

// Elimina un item de un TreeWidget
	void deleteItemTree(QTreeWidgetItem *item);
// Mueve hacia arriba una posición de un QTreeWidget
	void moveUpItemTw(QTreeWidget *twList);
// Mueve hacia abajo una posición de un QTreeWidget
	void moveDownItemTw(QTreeWidget *twList);

// Carga la lista de archivos en QList
	QList<stGrlDatos> cargarListaArchivos(QString m_dir, TipoListCbx tipo, QString filter = "*.*", QString filter_sep = ";");
// Carga la lista de archivos en QComboBox
	void cargarListaArchivosComboBox(QComboBox *cbx, QString m_dir, QString select_data, TipoListCbx tipo, QString filter = "*.*", QString filter_sep = ";", bool isClear = true);
// Carga la lista de archivos en QCheckComboBox
	void cargarListaArchivosCheckComboBox(QCheckComboBox *cbx, QString m_dir, QString select_data, TipoListCbx tipo, QString filter = "*.*", QString filter_sep = ";");
// Carga la lista de archivos especifico Idiomas, Scriptsm, Iconos
	void cargarIdiomasComboBox(QComboBox *cbx, QString dirLng, bool isClear = true, QString select_data = "", QString filter = "*.qm");
	void cargarScriptsComboBox(QComboBox *cbx, QString dirScripts, QString select_data = "", QString filter = "*.js");
	void cargarIconosComboBox(QComboBox *cbx, QString iconDir, QString select_data = "", QString filter = "*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.pbm;*.pgm;*.ppm;*.tiff;*.xbm;*.xpm;*.svg");
// Carga las distintos archivos la ruta indicada en un QTreeWidget.
	void cargarArchivosTwLista(QTreeWidget *twList, QString m_dir, QString filter = "*.*", QString filter_sep = ";", QString img = "");

// Carga la lista de datos de un archivo a un QTreeWidget
	void cargarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep = "|", bool isLng = false);
// Guarda la lista de datos de un archivo a un QTreeWidget
	void guardarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep = "|");

// Carga la lista de datos.
	QList<stGrlDatos> cargaListaDatos(QString archivo, int num_col, QString sep);
// Carga la lista de datos en un QHash.
	QHash<QString, stGrlDatos> cargaDatosQHash(QString archivo, int num_col, QString sep);
// Carga la lista de datos de un archivo a un QComboBox o QCheckComboBox
	void cargarDatosComboBox(QComboBox *cbx, QString archivo, QString theme, QString select_data = "", int num_col = 2, QString sep = "|", bool isClear = true, bool keyVisible = false);
	void cargarDatosCheckComboBox(QCheckComboBox *chk_cbx, QString archivo, QString theme, QString select_data = "", int num_col = 2, QString sep = "|");

// Remplaza texto por las imagenes de los emoticones
	QString reemplazaTextoSmiles(QString str, QHash<QString, stGrlDatos> lista);

// Devuelve imagenes para la info del juego
	QString getImgDatos(QHash<QString, stGrlDatos> datos, QStringList lista, bool isLng);
	void insertaIconos(QHash<QString, QPixmap> &ico, QString m_dir, QString contiene = "", QString filter = "*.*", bool isExt = true);

	stDatosJuego getDefectDatos(QString icono = "datos", QHash<QString, QString> dato = QHash<QString, QString>());
	stConfigDOSBox getDefectDOSBox(QHash<QString, QString> dato = QHash<QString, QString>());
	stConfigScummVM getDefectScummVM(QHash<QString, QString> dato = QHash<QString, QString>());
	stConfigVDMSound getDefectVDMSound(QHash<QString, QString> dato = QHash<QString, QString>());

	QString getNameTo8Caracter(QString name, int index = 1);
	QString getShortPathName(QString longPath);
	QStringList creaConfigMontajes(QTreeWidget *twDbxMount, stConfigDOSBox cfgDbx);
// Crea el archivo de configuración del DOSBox
	void crearArchivoConfigDbx(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QTreeWidget *twDbxMount, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg = ExportDbx);
// Crea la configuración del ScummVM
	void creaIniScummVM(stConfigScummVM cfgSvm, QString archivo);
// Crea el archivo de configuración del VDMSound
	void crearArchivoConfigVdmS(stConfigVDMSound cfgVdms, QString pathSaveConfg);

// Exportar la configuración del GR-lida, Datos, DOSBox, ScummVM y VDMSound
	void exportarProfileGRlida(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QTreeWidget *twDbxMount, stConfigScummVM cfgSvm, stConfigVDMSound cfgVdms, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg = ExportGrlida);
// Importar la configuración del DOSBox del DFend
	QHash<QString, QString> importarProfileDFend(QString dir_app, QString tabla, QString filename);

private:
	QString stDirApp, stLngSelect, stTheme, stThemeApp;

};

#endif // FUNCIONES_H
