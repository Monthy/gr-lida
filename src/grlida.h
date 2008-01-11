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

#ifndef GRLIDA_H
#define GRLIDA_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFile>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QTextStream>
#include "funciones.h"
#include "dbsql.h"
#include "ui_grlida.h"

class GrLida : public QMainWindow
{
    Q_OBJECT
public:
	GrLida(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GrLida();

	Ui::GrLidaClass ui;
	
	void CargarBaseDatos();		   // Abre y carga la base de datos
	void CargarConfigInicial();    // Cargar la Configuracin Inicial
	void ComprobarArchivosDatos(QString Version_GRL); // Comprueba si existen los archivos de datos

private:
	Funciones fGrl;
	dbSql *sql;
	QProcess *dBoxSvm;
	
	QString stTituloGrl() { return tr("GR-lida Lanzador para el DOSBox, ScummVM y VdmSound"); }

	QString stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port;
	QString stHomeDir, stIconDir, stDatosDir, stConfgDbxDir, stConfgVdmSDir;
	QString stBinExeDbx, stBinExeSvm;
	QString stItemIndex, stConfgJuego, stTipoEmu, stIdiomaSelect, stCapturasDbx, stCapturasSvm, stCapturasVdms;

	QStringList stl_param, smiles_Lista, smiles_ListaTemp;

	QHash<QString, QString> listSmailes;
	QHash<QString, QString>::const_iterator i_Hash;

	QLabel lbpanel_1, lbpanel_2, lbpanel_3, lbpanel_4, lbpanel_5;

	void MostrarDatosDelJuego(QString IDitem);
	void CargarThumbsTreeWidget(const QString directorio);
	void NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem);
	void CrearArchivoDato(QString archivo);  // Crea los archivos de datos
	void Ejecutar( const QString& bin, const QString &parametros);
	void Confg_Svm_Dbx(QString IDitem);	
	void CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString PathSaveConfg);
	void CrearArchivoConfigDbx(const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg);

protected:
	void closeEvent( QCloseEvent *e );

private slots:
	void on_twJuegos_clicked( QTreeWidgetItem *item );
	void on_twJuegos_Dblclicked( QTreeWidgetItem *item);
	void on_twJuegos_currentItemChanged( QTreeWidgetItem *item1, QTreeWidgetItem *item2);

	void on_AddNewDbx();		// Aade un juego para el DOSBox.
	void on_Informacion();		// Muestra distinta informacion
	void on_AddNewSvm();		// Aade un juego para el scummvm.
	void on_AddNewVdms();		// Aade un juego para el vdmsound.
	void on_EjecutarJuego();	// Ejecuta el juego.
	void on_EjecutarSetup();	// Ejecuta el Setup o install del juego.
	void on_NuevoJuego();		// Aade un juego, solo datos.
	void on_EditarJuego();		// Edita juego no importa de que tipo sea.
	void on_EliminarJuego();	// Elimina un juego de la BD y sus Configuracin.
	void on_Opciones();			// Accede a las Opciones del GR-lida
    void on_AcercaD();			// Acerca del GR-lida.
	void on_ImportarJuego();	// Obtiene lo datos de jugo de forma externa.
	void on_EjecutarDosbox();
	void on_EjecutarScummVM();
	void on_btn_fileurl_1();
	void on_btn_fileurl_2();
	void on_btn_imgtumb_1();
	void on_btn_imgtumb_2();

//public slots:

};

#endif // GRLIDA_H
