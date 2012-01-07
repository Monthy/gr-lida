/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_H
#define GRLIDA_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QHash>
#include <QVariant>

#include "funciones.h"
#include "dbsql.h"
#include "ui_grlida.h"

enum col_TwJuegos {
	col_IdGrl    = 0,
	col_Titulo   = 1,
	col_Compania = 2,
	col_Anno     = 3,
	col_Rating   = 4,
	col_TipoEmu  = 5,
	col_Favorito = 6,
	col_PicFlow  = 7
};

class GrLida : public QMainWindow
{
    Q_OBJECT

public:
	GrLida(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GrLida();

	Ui::GrLidaClass ui;

// Hace la conexion con la Base de Datos.
	void ConectarBaseDatos();
// Abre y carga la base de datos.
	void CargarListaJuegosDB(QString strBuscar = "", QString sqlQuery = "");
// Comprueba si existen los archivos de datos.
	void ComprobarArchivosDatos(QString Version_GRL, QString lng = "");
// Ocualta o muesta la ventana Principal.
	void setVisible(bool visible);

protected:
	void closeEvent( QCloseEvent *e );
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QString stTituloGrl(){ return tr("GR-lida Lanzador para el DOSBox, ScummVM y VdmSound"); }

	Funciones fGrl;
	dbSql *sql;
	QTreeWidgetItem *twCategoria;
	QHash<QString, QVariant> GRLConfig;
	QHash<QString, QVariant> LwConf;

	QProcess *dBoxSvm;
	QUrl my_url;

	int new_rating, old_rating;
	int id_ImgPicFlow, id_ImgPicFlow_old, numSkip_PicFlow;
	bool chkVersionDesdeMenu, item_changed;

	QString stHomeDir, stDatosDir, stIconDir, stTempDir, stConfgDbxDir, stConfgVdmSDir;
	QString stIdiomaSelect, stTheme, stIdioma, stIconoFav, stPicFlowReflection;
	QString str_html_new, str_html_old, str_html_comentario;
	QString stdb_type, stdb_server, stdb_host, stdb_name, stdb_username, stdb_password, stdb_port;
	QString stdb_Orden_ColTabla, stdb_Orden_By, stdb_Orden;
	QString stBinExeDbx, stBinExeSvm;
	QString stItemIndex, stConfgJuego, stJuegoParametrosExe, stDirWorkingJuego, stTipoEmu;
	QString stDirCapturas, stCaratula_Delantera, stCaratula_Trasera;
	QString stCoversDir, stThumbsDir, stListThumbsDir, stCapturesDir;
	QString stThumbs, stCoverFront, stCoverBack;

	QStringList stl_param;
	QStringList smiles_Lista, smiles_ListaTemp;

	QHash<QString, QString> listSmailes;
	QHash<QString, QString>::const_iterator i_Hash;

	QHash<QString, QListWidgetItem *> lwlista_pos;
	QHash<QString, QTreeWidgetItem *> twlista_pos;

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

	void ItemNext();
	void ItemBack();

	void createConnections();
	void createToolBars();
	void createDockBars();
// Menu de Navegacion de las distintas categorias --
	void CrearMenuNav();
	void MenuNav_AddCat(QString etiqueta_cat, QString icono_cat, QString tabla = "all", bool m_expanded = true);
	void MenuNav_AddSubCat(QString etiqueta_subcat, QString icono_subcat, QString tabla);
	void MenuNav_AddSubCat(QString archivo, QString tabla, int num_col = 1, bool idioma_svm = false);
// -------------------------------------------------
	void createStatusBar();
	bool createTrayIcon();

	void CargarConfig();	// Carga la Configuración del GR-lida.
	void GuardarConfig();	// Guarda la Configuración del GR-lida.
	void setTheme();		// Carga el Tema de los Iconos.

	void CrearArchivoDato(QString origen, QString destino);		// Crea los archivos de datos.

	void MostrarDatosDelJuego(QString IDitem = "");				// Muestra los Datos del Juego.
	void MostrarArchivosURLDelJuego(QString IDitem = "");		// Muestra o carga los datos de los Archivos y Url del Juego.
	void CargarCapturasImagenes(const QString directorio);		// Carga las Imágenes en una lista.
	void CargarCapturasVideoSonidos(const QString directorio);	// Carga los Videos y Sonidos en una lista.

	void NuevoItemTreeWidget(const QHash<QString, QString> datos, QString IDitem, bool nuevo_juego = false);
	void NuevoItemCopiarImagenes(QHash<QString, QString> datos, QString IDitem, bool nuevo_juego = true);

	void Config_Dbx(QString IDitem);	// Carga la configuración del DOSBox.
	void Config_Svm(QString IDitem);	// Carga la configuración del ScummVM.
	void Config_Vdms(QString IDitem);	// Carga la configuración del VDMSound.
	void Config_Clear();				// Limpia la configuración para evitar errores.

	void Ejecutar(QString bin, QString parametros = "", QString dirWorking = "");

	bool version_compare(QString ver_old, QString ver_new);	// Compara las versiones
	void on_CheckUpdateGrl();	// Comprueba si existen actualizaciones.

private slots:
	void on_ver_nav(bool mChecked);				//
	void on_ver_archivos_url(bool mChecked);	// Muestra la pestaña de los Archivos/Url.
	void on_ver_datos(bool mChecked);			// Muestra la pestaña de la información de los Datos del Juego.
	void on_ver_capturas(bool mChecked);		// Muestra la lista de Captura del Juego si la posee.

	void showPopup(const QPoint& aPosition);
	void on_CoverMode(bool icon_mode);
	void on_setFavorito();				// Marcamos como Favorito True/False dependiendo del estado.

	void on_twJuegos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_twJuegos_clicked(QTreeWidgetItem *twItem);
	void on_twJuegos_Dblclicked(QTreeWidgetItem *twItem);

	void on_lwJuegos_clicked(QListWidgetItem *lwItem);
	void on_lwJuegos_Dblclicked(QListWidgetItem *lwItem);

	void on_twListNav_clicked(QTreeWidgetItem *twItem);
	void on_twListNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void on_twCapturas_Dblclicked(QTreeWidgetItem *twItem);
	void on_btnVer_CoverFront();		// Muestra la carátula Frontal.
	void on_btnVer_CoverBack();			// Muestra la carátula Trasera.
	void on_btnVer_Archivos();			// Muestra la barra de archivos.

	void on_twCapturaVideo_Dblclicked(QTreeWidgetItem *twItem);
	void on_twCapturaSonido_Dblclicked(QTreeWidgetItem *twItem);
	void on_twUrls_Dblclicked(QTreeWidgetItem *twItem);
	void on_twFiles_Dblclicked(QTreeWidgetItem *twItem);

	void on_txtBuscar_textChanged(const QString &text);
	void on_Ordenar_Lista();			// Ordena la Lista de Juegos.

	void on_Informacion();				// Muestra distinta informacion.

	void on_AddNewDbx();				// Aade un juego para el DOSBox.
	void on_AddNewSvm();				// Aade un juego para el scummvm.
	void on_AddNewVdms();				// Aade un juego para el vdmsound.

	void on_NuevoJuego();				// Aade un juego, solo datos.
	void on_EditarJuego();				// Edita juego no importa de que tipo sea.
	void on_EliminarJuego();			// Elimina un juego de la BD y sus Configuración.

	void on_EjecutarDosbox();			// Ejecuta el emulador DOSBox.
	void on_EjecutarScummVM();			// Ejecuta el emulador ScummVM.
	void on_EjecutarJuego();			// Ejecuta el juego.
	void on_EjecutarSetup();			// Ejecuta el Setup o install del juego.
	void fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus);
	void fin_ProcesoError(QProcess::ProcessError error);

	void on_InstalarJuego();
	void on_ImportarJuego();			// Obtiene los datos de juego de forma externa.
	void on_ExportarJuego();			// Exporta los datos de juego para ser usados en otro equipo o lanzador.
	void on_ReconstruirTodasCaratulas(bool db_cargada = true);
	void on_ReconstruirCaratula();

	void on_Opciones();					// Accede a las Opciones del GR-lida.
	void on_BuscarUpdates();			// Menú para comprobar si existen actualizaciones.
	void on_AcercaD();					// Acerca del GR-lida.
	void on_VerRating(bool visible);	// Muestra/Oculta el Rating de la Lista.
	void on_VerCompania(bool visible);	// Muestra/Oculta la Compañia de la Lista.
	void on_VerAnno(bool visible);		// Muestra/Oculta el Año de la Lista.

	void on_VerCarpeta_confdbx();		// Muestra la carpeta confdbx.
	void on_VerCarpeta_confvdms();		// Muestra la carpeta confvdms.
	void on_VerCarpeta_thumbs();		// Muestra la carpeta thumbs.
	void on_VerCarpeta_covers();		// Muestra la carpeta covers.
	void on_VerCarpeta_iconos();		// Muestra la carpeta iconos.
	void on_VerCarpeta_smiles();		// Muestra la carpeta smiles.
	void on_VerCarpeta_themes();		// Muestra la carpeta themes.
	void on_VerCarpeta_datos();			// Muestra la carpeta datos.
	void on_VerCarpeta_scripts();		// Muestra la carpeta scripts.
	void on_VerCarpeta_templates();		// Muestra la carpeta templates.
	void on_VerCarpeta_idiomas();		// Muestra la carpeta idiomas.
	void on_VerCarpeta_temp();			// Muestra la carpeta temp.
	void on_VerCarpeta_home();			// Muestra la carpeta principal.

	void isCheckUpdateFinished();		// Comprueba la versión.
};

#endif // GRLIDA_H
