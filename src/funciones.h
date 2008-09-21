/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006  Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <QtCore>
#include <QtGui>

class Funciones
{
public:
	Funciones();
	~Funciones();

// Versión del GR-lida
	QString stVersionGrl(){ return "0.7.0"; }
// Versiones de los emuladores que soporta el GR-lida
// DOSBox
	QString stVersionDbx(){ return "0.72";  }
// ScummVM
	QString stVersionSvm(){ return "0.12.0";}
// VDMSound
	QString stVersionVdms(){return "2.0.4"; }
// Muestra el Nombre de la plataforma donde esta compilada
	QString get_Plataforma();

// Convierte de Número a Texto
	QString	IntToStr(int num);
// Convierte de Texto a Número
	int StrToInt(QString text);
// Convierte Texto "true", "yes" a tipo Bool
	bool StrToBool(QString text);
// Convierte Bool a Texto "true", "yes" indicando el tipo
	QString BoolToStr(bool estado, bool type_yes = false);
// Devuelve la hora y la fecha actual
	QString HoraFechaActual(QString hora = "");
// Devuelve una URL correcta
	QString url_correcta(QString url);
// Elimina caracteres no permitidos por windows por ejemplo
	QString eliminar_caracteres(QString str);
// Devuelve un Icono
	QString getIconListaJuegos(QString strIcon, QString stIconDir);
	QString getIconMount(QString tipoDrive, QString select_mount);
// Devuelve el directorio que usa el GR-lida
	QString GRlidaHomePath();
// Devuelve el directorio del Theme a usar
	QString ThemeGrl();
// Carga la hoja de estilo y reemplazando el comodin <theme> por el que se use
	QString StyleSheet();

// Obtiene la dirección y el nombre del archivo atraves de QFileDialog
	QString VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save);
// Obtiene la dirección de una carpeta atraves de QFileDialog
	QString VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir);

// Elimina un item de un TreeWidget
	void DeleteItemTree(QTreeWidgetItem * item);
// Carga la lista de Idiomas en un QComboBox
	void CargarIdiomasCombo(const QString dirLng, QComboBox *myCombobox);
// Carga una lista de datos en un QComboBox
	void CargarDatosComboBox(QString Archivo, QComboBox *myCombobox,int num_col, bool idioma_svm);
// Carga Iconos e imagenes soportadas en un QComboBox de un directorio
	void CargarIconosComboBox(QString IconDir, QComboBox *myCombobox, QStringList filters);
// Carga la lista del ScummVM en un QTreeWidget
	void CargarDatosListaSvm(QString Archivo, QTreeWidget *myTreeWidget);
// Carga la lista de los Smiles y los devuelve en un QHash<QString, QString>
	QHash<QString, QString> Cargar_Smiles(QString Archivo);
// Carga la lista de los Smiles en un TreeWidget y los devuelve en un QHash<QString, QString>
	QHash<QString, QString> Cargar_Smiles(QString Archivo, QTreeWidget *myTreeWidget);
// Remplaza texto por las imagenes de los emoticones
	QString ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista);
// Carga los Distintos datos para Ordenar
	QStringList CargaDatosListas(QString Archivo, QString delimitador);

// Crea la configuración del ScummVM
	void CreaIniScummVM(QString dirIni, QHash<QString, QString> conf_Svm);
// Crea la configuración de los Montajes para el DOSBox
	QStringList CreaConfigMontajes(QTreeWidget *treeWidget, const QHash<QString, QString> datos);
// Crea el archivo de configuración del DOSBox
	void CrearArchivoConfigDbx(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg, bool ExportToDFend = false);
// Crea el archivo de configuración del VDMSound
	void CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString PathSaveConfg);

// Exportar la configuracion del DOSBox para el GR-lida
	void Exportar_Profile_GRLida(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg);
// Carga la lista de los perfiles preconfigurados en un QComboBox
	void Cargar_Profile_DFend_ComboBox(QString dirProfiles, QComboBox *myCombobox);
// Exportar la configuracion del DOSBox para el DFend
	void Exportar_Profile_DFend(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg);
// Importar la configuracion del DOSBox para el DFend
	QHash<QString, QString> Importar_Profile_DFend(QString fileName);

private:
	QString stTheme;
	// Funcion para Poner nombres cortos en DOS.
	QString getShortPathName(QString longPath);

};

#endif /*FUNCIONES_H*/
