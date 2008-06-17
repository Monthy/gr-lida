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
#include <QSystemTrayIcon>
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

	void CargarBaseDatos(QString str="");				// Abre y carga la base de datos
	void CargarConfigInicial();							// Cargar la Configuracin Inicial
	void ComprobarArchivosDatos(QString Version_GRL);	// Comprueba si existen los archivos de datos

	void setVisible(bool visible);

private:
	Funciones fGrl;
	dbSql *sql;

	QProcess *dBoxSvm;
	
	QString stTituloGrl() { return tr("GR-lida Lanzador para el DOSBox, ScummVM y VdmSound"); }

	QString stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port;
	QString stdb_Orden_ColTabla, stdb_Orden_By, stdb_Orden;
	QString stHomeDir, stIconDir, stDatosDir, stConfgDbxDir, stConfgVdmSDir, stTheme;
	QString stBinExeDbx, stBinExeSvm;
	QString stItemIndex, stConfgJuego, stTipoEmu, stIdiomaSelect;
	QString stCapturasDbx, stCapturasSvm, stCapturasVdms, stCaratula_Delantera, stCaratula_Trasera, stIconoFav;
	QUrl my_url;
	QStringList stl_param, smiles_Lista, smiles_ListaTemp;

	QHash<QString, QString> listSmailes;
	QHash<QString, QString>::const_iterator i_Hash;
	QHash<QString, QString> conf_scummvm;

	QLabel lbpanel_1, lbpanel_2, lbpanel_3, lbpanel_4, lbpanel_5;
	
	QAction *minimizeAction;
	QAction *maximizeAction;
	QAction *restoreAction;
	QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu, *ljMenuPopUp;
    bool isTrayIcon;
    int id_ImgPicFlow;

	void MostrarDatosDelJuego(QString IDitem);
	void CargarThumbsTreeWidget(const QString directorio);
	void NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem);
	void CrearArchivoDato(QString archivo);  // Crea los archivos de datos
	void Ejecutar( const QString& bin, const QString &parametros);
	void Confg_Svm_Dbx(QString IDitem);
	void setTheme();
	bool createTrayIcon();

protected:
	void closeEvent( QCloseEvent *e );
	bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void showPopup(const QPoint& aPosition);
	void on_twJuegos_clicked( QTreeWidgetItem *item );
	void on_twJuegos_Dblclicked( QTreeWidgetItem *item);
	void on_twJuegos_currentItemChanged( QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_twCapturas_Dblclicked( QTreeWidgetItem *item);
	void on_twUrls_Dblclicked( QTreeWidgetItem *item);
	void on_txtBuscar_textChanged(const QString &);

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
    void on_Ordenar_Lista();	// Ordena la Lista de Juegos
    void on_ImportarJuego();	// Obtiene lo datos de jugo de forma externa.
    void on_ExportarJuego();
	void on_EjecutarDosbox();
	void on_EjecutarScummVM();
	void on_btnVer_CoverFront();
	void on_btnVer_CoverBack();
	void on_setFavorito();

	void fin_Proceso( int, QProcess::ExitStatus );
	void fin_ProcesoError( QProcess::ProcessError );

//public slots:

};

#endif // GRLIDA_H
