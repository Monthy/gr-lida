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

// Hace la conexion con la Base de Datos
	void ConectarBaseDatos();
// Abre y carga la base de datos
	void CargarListaJuegosDB(QString strBuscar = "");
// Comprueba si existen los archivos de datos
	void ComprobarArchivosDatos(QString Version_GRL);

	void setVisible(bool visible);

protected:
	void closeEvent( QCloseEvent *e );
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Funciones fGrl;
	dbSql *sql;

	QProcess *dBoxSvm;
	QUrl my_url;
	int id_ImgPicFlow, id_ImgPicFlow_old, numSkip_PicFlow;

	QString stHomeDir, stDatosDir, stIconDir, stConfgDbxDir, stConfgVdmSDir;
	QString stIdiomaSelect, stTheme, stIconoFav, stPicFlowReflection, str_html_comentario;
	QString stdb_type, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port;
	QString stdb_Orden_ColTabla, stdb_Orden_By, stdb_Orden;
	QString stBinExeDbx, stBinExeSvm;
	QString stItemIndex, stConfgJuego, stTipoEmu;
	QString stDirCapturas, stCaratula_Delantera, stCaratula_Trasera;
	//QString stCapturasDbx, stCapturasSvm, stCapturasVdms, stCaratula_Delantera, stCaratula_Trasera;

	QStringList stl_param;
	QStringList smiles_Lista, smiles_ListaTemp;

	QHash<QString, QString> listSmailes;
	QHash<QString, QString>::const_iterator i_Hash;
	QHash<QString, QString> conf_scummvm;

// Barra de Heramientas Ordenar
	QHBoxLayout *HLayoutOrdenar;
	QFrame *frameOrdenar;
	QLineEdit *txtBuscar;
	QLabel *lb_ordenar_1;
	QLabel *lb_ordenar_2;
	QComboBox *cbxColTabla;
	QComboBox *cbxOrdenBy;
	QComboBox *cbxOrden;
	QToolButton *btn_Ordenar;

// StatusBar
	QLabel *lbpanel_1;
	QLabel *lbpanel_2;
	QLabel *lbpanel_3;
	QLabel *lbpanel_4;
	QLabel *lbpanel_5;

// Tray Icon
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu, *ljMenuPopUp;
	bool isTrayIcon;

	QString stTituloGrl(){ return tr("GR-lida Lanzador para el DOSBox, ScummVM y VdmSound"); }
	void createConnections();
	void createToolBars();
	void createStatusBar();
	bool createTrayIcon();

	void CargarConfig();
	void GuardarConfig();
	void setTheme();

	void CrearArchivoDato(QString archivo);  // Crea los archivos de datos

	void MostrarDatosDelJuego(QString IDitem = "");
	void CargarThumbsTreeWidget(const QString directorio);

	void NuevoItemTreeWidget(const QHash<QString, QString> datos, QString imgEmu, QString IDitem);

	void Config_Dbx(QString IDitem);
	void Config_Svm(QString IDitem);
	void Config_Vdms(QString IDitem);
	void Config_Clear();

	void Ejecutar(const QString &bin, const QString &parametros = "");

private slots:
	void showPopup(const QPoint& aPosition);

//	void on_twNav_clicked( QTreeWidgetItem *item);
//	void on_twNav_currentItemChanged( QTreeWidgetItem *item1, QTreeWidgetItem *item2);

	void on_setFavorito();
	void on_twJuegos_clicked(QTreeWidgetItem *item);
	void on_twJuegos_Dblclicked(QTreeWidgetItem *item);
	void on_twJuegos_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2);

	void on_twCapturas_Dblclicked(QTreeWidgetItem *item);
	void on_btnVer_CoverFront();
	void on_btnVer_CoverBack();

	void on_twUrls_Dblclicked(QTreeWidgetItem *item);

	void on_txtBuscar_textChanged(const QString &text);
	void on_Ordenar_Lista();	// Ordena la Lista de Juegos

	void on_Informacion();		// Muestra distinta informacion

	void on_AddNewDbx();		// Aade un juego para el DOSBox.
	void on_AddNewSvm();		// Aade un juego para el scummvm.
	void on_AddNewVdms();		// Aade un juego para el vdmsound.

	void on_NuevoJuego();		// Aade un juego, solo datos.
	void on_EditarJuego();		// Edita juego no importa de que tipo sea.
	void on_EliminarJuego();	// Elimina un juego de la BD y sus Configuracin.

	void on_EjecutarDosbox();	// Ejecuta el emulador DOSBox.
	void on_EjecutarScummVM();	// Ejecuta el emulador ScummVM.
	void on_EjecutarJuego();	// Ejecuta el juego.
	void on_EjecutarSetup();	// Ejecuta el Setup o install del juego.
	void fin_Proceso(int, QProcess::ExitStatus);
	void fin_ProcesoError(QProcess::ProcessError);

	void on_ImportarJuego();	// Obtiene los datos de juego de forma externa.
	void on_ExportarJuego();	// Exporta los datos de juego para ser usados en otro equipo o lanzador.

	void on_Opciones();			// Accede a las Opciones del GR-lida
	void on_AcercaD();			// Acerca del GR-lida.

};

#endif // GRLIDA_H
