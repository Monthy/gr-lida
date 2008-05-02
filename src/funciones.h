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
	QString stVersionGrl(){ return "0.4.8"; }
// Versiones que soporta
	QString stVersionDbx(){ return "0.72";  }
	QString stVersionSvm(){ return "0.11.1";}
	QString stVersionVdms(){return "2.0.4"; }
// Devuelve el directorio que usa el GR-lida
	QString GRlidaHomePath();
// Muestra el Nombre de la plataforma donde esta compilada
	QString get_Plataforma();
// Convierte de Número a Texto
	QString	IntToStr(int num);
// Convierte de Texto a Número
	int StrToInt(QString text);
// Convierte Texto "true", "yes" a tipo Bool
	bool StrToBool(QString text);
	QString BoolToStr(bool estado, bool type_yes = false);
// Devuelve la hora y la fecha
	QString HoraFechaActual();
// Devuelve una URL correcta
	QString url_correcta(QString url);
// Elimina caracteres no permitidos por windows por ejemplo
	QString eliminar_caracteres(QString str);
// Elimina un item de un TreeWidget
	void DeleteItemTree( QTreeWidgetItem * item );
//	
	void CargarIdiomasCombo(const QString dirLng, QComboBox *myCombobox);
	void CargarDatosComboBox(QString Archivo, QComboBox *myCombobox,int num_col, bool idioma_svm);
	void CargarIconosComboBox(QString IconDir, QComboBox *myCombobox, QStringList filters);
	void CargarDatosListaSvm(QString Archivo, QTreeWidget *myTreeWidget);
//Carga los Smiles en un TreeWidget
	QHash<QString, QString> Cargar_Smiles(QString Archivo, QTreeWidget *myTreeWidget);
	QHash<QString, QString> Cargar_Smiles(QString Archivo);
	QString ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista);
// Carga los Distintos datos para Ordenar
	QStringList CargaDatosListas(QString Archivo, QString delimitador);
// Obtiene la direccion y el nombre del archivo atraves de QFileDialog
	QString VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save);
// Obtiene la direccion de una carpeta atraves de QFileDialog
	QString VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir);
//
	void CreaIniScummVM(QString dirIni, QHash<QString, QString> conf_Svm);
//
	void CrearArchivoConfigDbx(const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg, bool ExportToDFend = false);
	QStringList CreaConfigMontajes(QTreeWidget *treeWidget, const QHash<QString, QString> datos);
//
	void CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString PathSaveConfg);
// Carga la lista de los perfiles preconfigurados en un ComboBox
	void Cargar_Profile_DFend_ComboBox(QString dirProfiles, QComboBox *myCombobox);
// Exportar la configuracion del DOSBox para el DFend y GR-lida
	void Exportar_Profile_DFend(const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg);
	void Exportar_Profile_GRLida(const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg);
// Importar la configuracion del DOSBox para el DFend
	QHash<QString, QString> Importar_Profile_DFend(QString fileName);
};

#endif /*FUNCIONES_H*/
