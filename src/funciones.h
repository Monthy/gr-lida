/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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

#include <QListWidget>
#include <QTreeWidget>
#include <QFileInfoList>
#include <QPushButton>
#include <QComboBox>
#include <QString>
#include <QSettings>
#include <QScriptEngine>

#include "g_structs.h"
#include "qcheckcombobox.h"
#include "qgroupcombobox.h"

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
	CbxListJs   = 3
};

enum TipoListTw {
	TwListSvm        = 0,
	TwListEmu        = 1,
	TwListTraduccion = 2,
	TwListSmile      = 3,
	TwList2col       = 4,
	TwList3col       = 5,
	TwListDbx        = 6
};

enum InfoVersion {
	esMenor = 0,
	esMayor = 1,
	esIgual = 2
};

struct stVersion {
	bool isNew;
	int  info;
};


// INICIO ComboBoxTwItem ----------------------------------------------------------------------------------------
class ComboBoxTwItem : public QComboBox
{
	Q_OBJECT

private:
	QTreeWidgetItem *item;
	int column;

signals:
	void itemSelect(QTreeWidgetItem *item);

public:
	ComboBoxTwItem(QTreeWidgetItem *twItem, int twCol)
	{
		item = twItem;
		column = twCol;
		connect(this, SIGNAL(activated(int)), SLOT(changeItem(int)));
	}

public slots:
	void changeItem(int index)
	{
		if (index > -1)
		{
			item->setText(column, itemData(index).toString());
			emit itemSelect(item);
		}
	}
};
// FIN ComboBoxTwItem -------------------------------------------------------------------------------------------


class Funciones : public QObject
{
	Q_OBJECT
public:
	Funciones();
	~Funciones();

// Versión del GR-lida.
	QString versionGrl() {return "0.12.0";}
// Versión del GR-dap
	QString versionGrd() {return "0.2.0";}

// Versiones de los emuladores que soporta el GR-lida
// DOSBox
	QString versionDbx() {return "0.74";}
// ScummVM
	QString versionSvm() {return "1.9.0";}
// VDMSound
	QString versionVdms() {return "2.0.4";}

	QString getOS() { return stOS; }
// Compara distintas versiones y devuelve True si es nueva o mayor
	stVersion version_compare(QString ver_old, QString ver_new);
// Crea una copia de seguridad de la base de datos
	void crearCopiaSeguridad_DB(QString dir, QString version_grl);
// Convierte de número a texto
	QString	intToStr(int num);
	QString intToStr(qint64 num);
	QString intToStr(quint64 num);
	QString intToStr(uint num);
// Convierte de texto a número
	int strToInt(QString str);
// Convierte texto "true", "yes", "1" a tipo bool
	bool strToBool(QString str);
// Convierte bool a texto "true", "yes" indicando el tipo
	QString boolToStr(bool estado, bool type_yes = false);
// Devuelve las cordenadas de posición
	int getPosition(int posText, bool wordWrap = false);
// Devuelve un QImagen en estado desactivado
	QImage imagenToDisabled(QString filename);
// Devuelve un QColor desde un QStringList o QString
	QColor getColor(QStringList color);
	QColor getColor(QString color);
// Convierte un QColor a un QStringList o QString
	QStringList setColor(QColor color);
	QString setColorStr(QColor color);
// Devuelve valor Script
	QString getValueScript(QScriptEngine &engine, QString key, QString str_default);
// Convierte la lista de un QListWidget a QStringList
	QStringList lwToStrList(QListWidget *lwlist);
// Devuelve el tiempo actual, formato unix
	QString getTime();
// Devuelve la hora y la fecha actual
	QString horaFechaActual(QString fecha, QString formato = "dd/MM/yyyy HH:mm:ss");
// Crea y devuelve un listado de fechas desde 1970 a año actual
	QStringList listaFechas();
// Devuelve la respuesta
	bool questionMsg(QString titulo, QString mensaje, QString texto_btn_si = "", QString texto_btn_no = "");
// Devuelve una forma correcta de parametros
	QString getParametrosEXE(QString parametros);
// Devuelve una forma correcta
	QString str_end(QString str, QString str_end);
// Elimina caracteres no permitidos por windows por ejemplo
	QString eliminar_caracteres(QString str, QString cambiar = "", QString sep = ",");
// Obtiene una la lista de archivos
	QFileInfoList getListFiles(QString dir, QStringList filter, bool isSubDir = false);
// Obtiene información del archivo
	stFileInfo getInfoFile(QString filename, TipoHash hash = hashNo);
// Obtiene el Hash del archivo
	QString getHashFile(QString filename, TipoHash hash = hashSha1);
// Devuelve un icono de montaje
	QString getIconMount(QString tipoDrive, QString select_mount = "");
// Crear Thumbs
	bool crearThumbs(QString origen, QString destino, int width = 145, int height = 186, int quality = -1, bool ignoreAspectRatio = false, QString format = "JPG");
// Leer archivo de texto
	QString leerArchivo(QString filename, const QByteArray code = "");
// Guarda archivo de texto
	bool guardarArchivo(QString filename, QString txt_out, const QByteArray code = QByteArray());


// Copia un archivo de un directorio a otro
	bool copiarArchivo(QString origen, QString destino, bool permisos = false, bool sobrescribir = false, bool preguntar = false);
// Copia varios archivos de un directorio a otro
	void copiarArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep = ";", bool permisos = false, bool sobrescribir = false, bool preguntar = false);
// Mueve un archivo de un directorio a otro
	bool moverArchivo(QString origen, QString destino, bool permisos = false, bool sobrescribir = false, bool preguntar = false);
// Mueve varios archivos de un directorio a otro
	void moverArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep = ";", bool permisos = false, bool sobrescribir = false, bool preguntar = false);


// Elimina archivos de un directorio
	bool eliminarArchivo(QString nameFile);
	void eliminarArchivos(QFileInfoList list_files);
// Elimina un directorio si existe
	bool eliminarDirectorio(QString dir);
// Renombra un directorio si existe
	bool renombrarDirectorio(QString dir_old, QString dir_new);
// Comprueba si un directorio existe y sino lo crea
	bool comprobarDirectorio(QString dir, bool info = false);
// Comprueba si un directorio si termina o empieza con barra o si ella
	bool comprobarBarraPath(QString dir, bool inicio = false, bool addChkSlashIni = true);
// Compara dos estructuras stGrlDatos si son iguales.
	bool compararStGrlDatos(stGrlDatos a, stGrlDatos b);

// Devuelve el directorio que usa el GR-lida
	QString homePath();
	QString dirApp(){ return stDirApp; }
// Rutas relativas
	QString getRelative(QString dir, QString dir_relative = "");
	QString setDirRelative(QString dir, QString dir_relative = "");
	QString getDirRelative(QString dir, QString dir_relative = "");
// Devuelve el directorio del Theme a usar
	void setTheme(QString theme);
	QString theme(){ return stTheme; }
// Devuelve el directorio del Idioma a usar
	void setIdioma(QString lng = "es_ES");
	QString idioma(){ return stLngSelect; }
// Carga la hoja de estilo y reemplaza el comodin <theme> por la ruta del theme que se use
	QString myStyleSheet(QString file_style);
	QString tplInfoJuego(QString file_tpl);
// Lista los gupos para la configuración de los iconos
	QStringList listItemIconConf(QString iniFileName);
// Obtiene la configuración de los distintos elementos
	stLwItemCfg getDefectLwItemConf(QSettings &settings);
// Carga la configuración de los iconos de la lista en modo Icono
	stLwIconCfg cargarListWidgetIconConf(QString tabla = "dbgrl", QString theme = "");
// Crea la configuración de los iconos de la lista en modo Icono
	void guardarListWidgetIconConf(stLwIconCfg lwConf, QString tabla = "dbgrl", QString theme = "");

// Carga la configuración del GR-lida
	stGrlCfg cargarGRLConfig(QString iniFileName);
// Guarda la configuración del GR-lida
	void guardarGRLConfig(QString iniFileName, stGrlCfg config);
// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
	QString ventanaAbrirArchivos(QWidget *parent, QString caption, QString dir, QString dir_relative, QString filter, QString *selectedFilter = 0, bool isSave = false);
// Obtiene la dirección de una carpeta atraves de QFileDialog
	QString ventanaDirectorios(QWidget *parent, QString caption, QString dir, QString dir_relative = "");
// Abre un archivo con el programa predeterminado
	bool abrirArchivo(QString filePath);
// Abre un directorio
	bool abrirDirectorio(QString dir);

// Elimina un item de un QTreeWidget
	void deleteItemTree(QTreeWidgetItem *item);
// Mueve hacia arriba una posición de un QTreeWidget
	void moveUpItemTw(QTreeWidget *twList);
// Mueve hacia abajo una posición de un QTreeWidget
	void moveDownItemTw(QTreeWidget *twList);
// Marca o desmarca todos los item de un QTreeWidget
	void checkStateAllItemTw(QTreeWidget *twList, int col, bool checked);
// Comprueba si existe la fila o subitem de la fila de un QTreeWidget
	bool comprobarItemTwList(QTreeWidget *twList, int row, int sub_row);
// Activa o desactiva un boton asociado si existe un cambio en el item seleccionado del QTreeWidget
	void enabledButtonUpdateTwList(QTreeWidget *twList, QPushButton *btn, QString texto, int col);


// Carga la lista de archivos en un QComboBox
	void cargarListaArchivosComboBox(TipoListCbx tipo, QComboBox *cbx, QString dir, QString dirImg, QString subDir = "", QString select_data = "", QString filter = "*.*", QString filter_sep = ";", bool isClear = true, QString contiene = "");
// Carga la lista de archivos especifico Idiomas, Scriptsm, Profiles, Iconos
	void cargarIdiomasComboBox(QComboBox *cbx, QString dirLng, QString dirImg, QString select_data = "", bool isClear = true);
	void cargarScriptsComboBox(QComboBox *cbx, QString dirScripts, QString dirImg, QString select_data = "");
	void cargarProfilesComboBox(QComboBox *cbx, QString dirProfiles, QString dirImg, QString select_data = "");
	void cargarIconosComboBox(QComboBox *cbx, QString dirIcon, QString dirImg, QString subDir = "", QString select_data = "", QString filter = "*.*", bool isClear = true, QString contiene = "");
	void cargarIconosGroupComboBox(QGroupComboBox *cbx, QString dirIcon, QString dirImg, QString subDir = "", QString filter = "*.*", QString contiene = "");
// Carga los distintos archivos de la ruta indicada en un QListWidget
	void cargarArchivosLwLista(QListWidget *lwList, QString dir, QString filter, QString filter_sep, QString img, bool isClear = true, bool isThumbs = false, bool isSmall = false);
// Carga la lista de datos de un archivo a un QTreeWidget
	void cargarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep = "|", bool isLng = false);
// Guarda la lista de datos de un QTreeWidget a un archivo
	void guardarDatosTwLista(QTreeWidget *twList, QString archivo, TipoListTw tipo, QString sep = "|");

// Carga la lista de datos
	QList<stGrlDatos> cargaListaDatos(QString archivo, int num_col, QString sep);
// Carga la lista de datos en un QHash
	QHash<QString, stGrlDatos> cargaDatosQHash(QString archivo, int num_col, QString sep);
// Carga la lista de datos de un archivo a un QComboBox o QCheckComboBox
	void cargarDatosComboBox(QComboBox *cbx, QString archivo, QString dirImg, QString select_data = "", int num_col = 2, QString sep = "|", bool isClear = true, bool keyVisible = false);
	void cargarDatosCheckComboBox(QCheckComboBox *chk_cbx, QString archivo, QString dirImg, QString select_data = "", int num_col = 2, QString sep = "|", bool isClear = true);

// Remplaza texto por las imágenes de los emoticones
	QString reemplazaTextoSmiles(QString str, QHash<QString, stGrlDatos> lista);
// Devuelve imágenes para la info del juego
	QString getImgDatos(QHash<QString, stGrlDatos> datos, QStringList lista, bool isLng);
	void insertaIconos(QHash<QString, QPixmap> &ico, QString dir, QString dir_img, QString prefix = "", QString filter = "*.*", QString contiene = "");

// Montajes de unidades
	QString getNameTo8Caracter(QString name, int index = 1);
	QString getShortPathName(QString longPath);
	QList<stConfigDOSBoxMount> getListCfgMount(QTreeWidget *twDbxMount);
// Crea la configuración de los Montajes para el DOSBox
	QStringList creaConfigMontajes(QList<stConfigDOSBoxMount> listMount, stConfigDOSBox cfgDbx, bool isSetup = false);

// Crea el archivo de configuración del DOSBox
	QString crearArchivoConfigDbx(stDatosJuego datos, stConfigDOSBox cfgDbx, QList<stConfigDOSBoxMount> listMount, QString dir_app, QString tabla, QString pathSaveConfg, bool isSetup = false);
// Crea la configuración del ScummVM
	QString creaIniScummVM(stConfigScummVM cfgSvm, QString pathSaveConfg);
// Crea el archivo de configuración del VDMSound
	QString crearArchivoConfigVdmS(stConfigVDMSound cfgVdms, QString pathSaveConfg);

// Exportar la configuración del GR-lida, Datos, DOSBox, ScummVM y VDMSound
	QString exportarProfileGRlida(stDatosJuego datos, QList<QString> url_list, stConfigDOSBox cfgDbx, QList<stConfigDOSBoxMount> listMount, stConfigScummVM cfgSvm, stConfigVDMSound cfgVdms, QString dir_app, QString tabla, QString pathSaveConfg, TipoCfg tipo_cfg = ExportGrlida, bool isSetup = false);
// Importar la configuración del DOSBox del DFend
	QHash<QString, QString> importarProfileDFend(QString dir_app, QString filename, bool isDbx = false);
// Importar la configuración del ScummVM
	QHash<QString, QString> importarIniScummVM(QString filename);

private:
	QString stDirApp, stLngSelect, stTheme, stOS;

// Copia o mueve un archivo de un directorio a otro
	bool opArchivo(QString origen, QString destino, bool mover, bool permisos, bool sobrescribir, bool preguntar);
// Copia o mueve varios archivo de un directorio a otro
	void opArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep, bool mover, bool permisos, bool sobrescribir, bool preguntar);

};

#endif // FUNCIONES_H
