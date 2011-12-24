/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

class Funciones
{
public:
	Funciones();
	~Funciones();

// Versión del GR-lida
	QString stVersionGrl(){ return "0.9.2"; }
// Versiones de los emuladores que soporta el GR-lida
// DOSBox
	QString stVersionDbx(){ return "0.74";  }
// ScummVM
	QString stVersionSvm(){ return "1.4.0";}
// VDMSound
	QString stVersionVdms(){return "2.0.4"; }
// Muestra el Nombre de la plataforma donde esta compilada
	QString get_Plataforma();

// Crea una copia de seguridad de la Base de Datos
	void CrearCopiaSeguridad_DB(QString dir, QString Version_GRL);
// Convierte de Número a Texto
	QString	IntToStr(int num);
// Convierte de Texto a Número
	int StrToInt(QString text);
// Convierte Texto "true", "yes" a tipo Bool
	bool StrToBool(QString text);
// Convierte Bool a Texto "true", "yes" indicando el tipo
	QString BoolToStr(bool estado, bool type_yes = false);
// Devuelve la hora y la fecha actual
	QString HoraFechaActual(QString formato = "");
// Devuelve una URL correcta
	QString url_correcta(QString url);
// Elimina caracteres no permitidos por windows por ejemplo
	QString eliminar_caracteres(QString str);
// Obtiene la extension del archivo
	QString getExtension(QString str);
// Obtiene la ruta del archivo
	QString getFilePath(QString str);
// Devuelve un Icono
	QString getIconListaJuegos(QString icono, QString iconoDir);
	QString getIconMount(QString tipoDrive, QString select_mount = "");
// Devuelve el directorio que usa el GR-lida
	QString GRlidaHomePath();
// Devuelve el directorio del Theme a usar
	QString ThemeGrl();
	void setTheme();
// Devuelve el directorio del Theme a usar
	QString IdiomaGrl();
// Carga la hoja de estilo y reemplazando el comodin <theme> por el que se use
	QString StyleSheet(bool list = false, bool isBgFixed = false);
// Carga la configuración de los iconos de la lista en modo Icono
	QHash<QString, QVariant> CargarListWidgetIconConf();
// Carga la configuración del GR-lida
	QHash<QString, QVariant> CargarGRLConfig(QString iniFileName);
// Guarda la configuración del GR-lida
	void GuardarGRLConfig(QString iniFileName, QHash<QString, QVariant> config);
	void GuardarKeyGRLConfig(QString iniFileName, QString grupo, QString key, QString value);

// Comprueba si un directorio existe y sino lo crea
	void ComprobarDirectorio(QString nameDir);
// Comprueba si existe un archivo altenativo para cargarlo.
	QString ComprobarArchivoAlternativo(QString archivo);
// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
	QString VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save = false);
// Obtiene la dirección de una carpeta atraves de QFileDialog
	QString VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir);
// Abre un archivo con el programa predeterminado
	bool abrirArchivo(QString filePath);
// Abre un directorio
	bool abrirDirectorio(QString dirPath);
// Crea una ventana para mostrar Información
	void VentanaInfo(QString titulo, QString icono, int w, int h, QString contenido);

// Elimina un item de un TreeWidget
	void DeleteItemTree(QTreeWidgetItem *item);
// Carga la lista de Scripts en un QComboBox
	void CargarScriptsComboBox(const QString dirScripts, QComboBox *myCombobox, QString filter = "*.js");
// Carga la lista de Idiomas en un QComboBox
	void CargarIdiomasCombo(const QString dirLng, QComboBox *myCombobox, QString filter = "*.qm", bool isScript = false);
// Carga una lista de datos en un QComboBox
	void CargarDatosComboBox(QString archivo, QComboBox *myCombobox, int num_col = 1, bool idioma_svm = false);
// Carga Iconos e imagenes soportadas en un QComboBox de un directorio
	void CargarIconosComboBox(QString iconDir, QComboBox *myCombobox, QStringList filters);
// Carga la lista del ScummVM en un QTreeWidget
	void CargarDatosListaSvm(QString archivo, QTreeWidget *myTreeWidget);
// Carga la lista de los Smiles y los devuelve en un QHash<QString, QString>
	QHash<QString, QString> Cargar_Smiles(QString archivo);
// Carga la lista de los Smiles en un TreeWidget y los devuelve en un QHash<QString, QString>
	QHash<QString, QString> Cargar_Smiles(QString archivo, QTreeWidget *myTreeWidget);
// Remplaza texto por las imagenes de los emoticones
	QString ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista);
// Carga los Distintos datos para Ordenar
	QStringList CargaDatosListas(QString archivo, QString delimitador);
// Carga las distintos archivos la ruta indicada en una Lista.
	void CargarListaDeCaptura(QTreeWidget *myTreeWidget, const QString directorio, QStringList filters, QString imagen = "");

// Crea la configuración del ScummVM
	void CreaIniScummVM(QString dirIni, QHash<QString, QString> conf_Svm);
// Funcion para Poner nombres cortos en DOS.
	QString getNameTo8Caracter(QString name);
	QString getShortPathName(QString longPath);
// Crea la configuración de los Montajes para el DOSBox
	QStringList CreaConfigMontajes(QTreeWidget *treeWidget, const QHash<QString, QString> datos);
// Crea el archivo de configuración del DOSBox
	void CrearArchivoConfigDbx(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString pathSaveConfg, bool exportToDFend = false);
// Crea el archivo de configuración del VDMSound
	void CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString pathSaveConfg);

// Exportar la configuracion del DOSBox para el GR-lida
	void Exportar_Profile_GRLida(const QHash<QString, QString> datos, const QHash<QString, QString> datos_emu, QTreeWidget *treeWidget, const QString pathSaveConfg, int exportGRLVersion = 1);
// Carga la lista de los perfiles preconfigurados en un QComboBox
	void Cargar_Profile_DFend_ComboBox(QString dirProfiles, QComboBox *myCombobox);
// Exportar la configuracion del DOSBox para el DFend
	void Exportar_Profile_DFend(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString pathSaveConfg);
// Importar la configuracion del DOSBox para el DFend
	QHash<QString, QString> Importar_Profile_DFend(QString fileName);
// Crea la imagen de la lista
	void CrearCoverList(QHash<QString, QVariant> datos, QHash<QString, QVariant> config);

// Datos por defecto
	QHash<QString, QString> getDefectDatosJuego();
	QHash<QString, QString> getDefectDatosDosBox();
	QHash<QString, QString> getDefectDatosScummVM();
	QHash<QString, QString> getDefectDatosVDMSound();

private:
	QString stTheme;

};

#endif /*FUNCIONES_H*/
