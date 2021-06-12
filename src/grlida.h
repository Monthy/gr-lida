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

#ifndef GRLIDA_H
#define GRLIDA_H

#include <QMainWindow>
#include <QLabel>
#include <QCloseEvent>
#include <QTreeWidget>
#include <QStandardItem>
#include <QSystemTrayIcon>
#include <QActionGroup>
#include <QHash>
#include <QXmlStreamReader>
//#include <QScrollArea>

#include "funciones.h"
#include "dbsql.h"
#include "grlida_delegate.h"
#include "grlida_picflow.h"
#include "grlida_multimedia.h"
#include "grlida_pdf_viewer.h"
#include "grdap.h"
#include "animatedtextbrowser.h"
#include "httpdownload.h"

namespace Ui {
	class GrLida;
}

class GrLida : public QMainWindow
{
	Q_OBJECT

public:
	explicit GrLida(QWidget *parent = 0);
	~GrLida();

protected:
	void closeEvent(QCloseEvent *event);
	bool eventFilter(QObject *object, QEvent *event);
	void setVisible(bool visible);

private:
	Ui::GrLida *ui;

	QString tituloGrl() { return tr("GR-lida lanzador para el DOSBox, ScummVM y VdmSound"); }

	Funciones *fGrl;
	dbSql *sql;
	QProcess *grlProcess, *grlProcessMount;
	GrlMultiMedia *mediaSound, *mediaVideo;
	GrDap *grdap;
	frmPdfViewer *pdfViewer;
	HttpDownload *httpdown;
	QXmlStreamReader xml;

	GrlTreeViewModel *data_model;
	GrlTreeViewDelegate *grl_tv_delegate;
	GrlListViewDelegate *grl_lv_delegate;
	GrlPicFlow *grl_picflow;
	QActionGroup *grpActions;
	QAction *action;
//	QScrollArea *scrollArea;
//	AnimatedTextBrowser *txtInfo;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stCfgExec cfgExec;
	stLwIconCfg lwIconCfg;

	QModelIndex select_row;
	QString IdGame, TipoEmu, tpl_info_game_old, tpl_info_game_empty_old;
	QFileSystemWatcher fs_watcher;
	QString CoverFront, CoverBack, pathCapturas, pathImagenes, pathVideos, pathSonidos;
	QString sql_where_select, pathImageSelect, mount_image_select, unmount_image_select;
	QString iconExtraImg;
	QImage img_pf_favorite, img_pf_favorite_gray, img_pf_original, img_pf_original_gray;
	QImage icono_fav, icono_fav_gray;

	unsigned int skip_picflow;
	int id_cat, total_juegos;
	QHash<int, stGrlCats> categoria;
	QList<GrlPicFlow::stListPicFlow> thumbs_list;
	QTreeWidgetItem *twListMnuNav;

	QHash<QString, stGrlDatos> edades_list;
	QHash<QString, stGrlDatos> idiomas_list;
	QHash<QString, stGrlDatos> smiles_list;
	QHash<QString, stGrlDatos> dbx_list;
	QMap<QString, stGrlDatos> emu_list;
	QList<stGrlDatos> list_emu;

	stVirtualDrive virtualDrive;
	QHash<QString, stVirtualDrive> virtual_drive_list;
	QHash<QString, QString> conf_vd;

// StatusBar
	QLabel *lb_panel_1, *lb_panel_2, *lb_panel_3, *lb_panel_4, *lb_panel_5, *lb_panel_6, *lb_panel_7;

// Tray icon
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu, *ljMenuPopUp;
	bool isTrayIcon, isPicFlowActive, isSelectRow, isItemChanged, isNavItemChanged, isMountImage, isAutoMountImageExe;
	bool isImportar, isOpenGrDap, isOpenPdf, isUpdateMenu;

	void conectarBaseDatos();


	void actualizaArchivoDatos(QList<stGrlDatos> datos, QString archivo, int num_col, QString sep);
	void comprobarArchivosDatos(QString version_grl, QString lng = "es_ES");

	void createWidgets();
	void createToolBars();
	void createStatusBar();
	void createTrayIcon();
	void createTbShortCut();

	void cargarConfig();
	void guardarConfig();
	void setTheme();
	void setSlideIconExtra();
	void cargarTextResource();
	void setToolbarBigIcon(bool bigIcon);
	void showPopup(QWidget *w_parent, const QPoint &pos);

	void checkUpdateGrl(bool is_menu = false);
	bool getDatosUpdates(QList<stUpdates> &update, QString tipo);

	void cargarListaShortCut();
	void cargarListaCategorias();
	void setConfigHeaderListaJuegos();
	void cargarListaJuegosDB(QString sql_where = "");

	void setChangeCategorias(int cat_id);
	void menuNavAddCat(QString etiqueta, QString dir_icono, QString icono, QString sql_query = "", bool m_expanded = true, bool m_show_total = true);
	void menuNavAddSubCat(QString etiqueta, QString dir_icono, QString icono, QString sql_query = "", QString sql_col = "");
	void crearMenuNav();

	void nuevoEditarDatosDelJuego(stDatosJuego datos, bool isNew, bool isMenu = false);
	QStandardItem* nuevoStdrItem(QString nombre, QString icono = "");

	void listItemSelect(const QModelIndex &index, QString info);
	void listItemSelectDoubleClicked(const QModelIndex &index, QString info);

	void listNavItemSelect(QTreeWidgetItem *item, int column);

	void mostrarDatosDelJuego(QString IDitem, bool soloInfo = false);
	void itemNext();
	void itemBack();
	void setCurrentItem(int index);
	void cargarConfigEmu(QString tipo_emu = "");
	void comprobarEmuloresDisp();
	void ejecutar_desmontaje();

	void ejecutar(QString bin, QString parametros = "", QString working_dir = "");
	void montaje(QString bin, QString parametros = "", QString working_dir = "");

	void error_msg_Proceso(QProcess::ProcessError error);

private slots:
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void actionShortCutClicked(QAction *action);
	void statusFinished();
	void changeMediaConfig();

	void on_txtInfo_anchorClicked(const QUrl &arg1);
// Menú archivo
	void on_mnu_file_informacion_triggered();
	void on_mnu_file_cerrar_triggered();
// Menú editar
	void on_mnu_edit_nuevo_dosbox_triggered();
	void on_mnu_edit_nuevo_scummvm_triggered();
	void on_mnu_edit_nuevo_vdmsound_triggered();
	void on_mnu_edit_nuevo_triggered();
	void on_mnu_edit_editar_triggered();
	void on_mnu_edit_eliminar_triggered();
	void on_mnu_edit_favorito_triggered(bool checked);
	void on_mnu_edit_original_triggered(bool checked);
	void updateCol(const QModelIndex &index, int col);
// Menú ejecutar
	void on_mnu_ejecutar_dosbox_triggered();
	void on_mnu_ejecutar_scummvm_triggered();
	void on_mnu_ejecutar_juego_triggered();
	void on_mnu_ejecutar_setup_triggered();
	void on_mnu_ejecutar_montaje_triggered();
// Menú herramientas
	void on_mnu_tool_instalar_triggered();
	void on_mnu_tool_virtual_drive_triggered();
	void on_mnu_tool_importar_triggered();
	void on_mnu_tool_exportar_triggered();
	void on_mnu_tool_cambiar_categoria_triggered();
	void on_mnu_tool_edit_themes_triggered();
	void on_mnu_tool_opciones_triggered();
// Menú ver
	void on_mnu_ver_nav_triggered(bool checked);
	void on_mnu_ver_cover_mode_triggered(bool checked);
	void on_mnu_ver_pictureflow_triggered(bool checked);
	void on_mnu_ver_pictureflow_to_center_triggered(bool checked);
	void on_mnu_ver_datos_triggered(bool checked);
	void on_mnu_ver_archivos_triggered(bool checked);
	void on_mnu_ver_enlaces_triggered(bool checked);
	void on_mnu_ver_videos_triggered(bool checked);
	void on_mnu_ver_sonidos_triggered(bool checked);
	void on_mnu_ver_capturas_triggered(bool checked);
	void on_mnu_ver_imagenes_triggered(bool checked);
	void on_mnu_ver_menubar_triggered(bool checked);
	void on_mnu_ver_toolbar_triggered(bool checked);
	void on_mnu_ver_toolbar_datos_triggered(bool checked);
	void on_mnu_ver_ordenar_triggered(bool checked);
	void on_mnu_ver_shortcut_triggered(bool checked);
	void on_mnu_ver_statusbar_triggered(bool checked);
	void on_mnu_ver_fullscreen_triggered(bool checked);
	void on_mnu_ver_ayuda_triggered();
	void on_mnu_ver_check_updates_triggered();
	void on_mnu_ver_acercad_triggered();
// Menús Ver Paneles
	void on_mnu_ver_ignore_auto_hide_nav_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_datos_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_files_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_urls_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_videos_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_sonidos_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_capturas_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_imagenes_triggered(bool checked);
	void on_mnu_ver_ignore_auto_hide_pictureflow_triggered(bool checked);
	void on_mnu_ver_titlebars_nav_triggered(bool checked);
	void on_mnu_ver_titlebars_datos_triggered(bool checked);
	void on_mnu_ver_titlebars_files_triggered(bool checked);
	void on_mnu_ver_titlebars_urls_triggered(bool checked);
	void on_mnu_ver_titlebars_videos_triggered(bool checked);
	void on_mnu_ver_titlebars_sonidos_triggered(bool checked);
	void on_mnu_ver_titlebars_capturas_triggered(bool checked);
	void on_mnu_ver_titlebars_imagenes_triggered(bool checked);
	void on_mnu_ver_titlebars_pictureflow_triggered(bool checked);
// Menú ver columnas
	void on_mnu_ver_col_icono_triggered(bool checked);
	void on_mnu_ver_col_subtitulo_triggered(bool checked);
	void on_mnu_ver_col_genero_triggered(bool checked);
	void on_mnu_ver_col_compania_triggered(bool checked);
	void on_mnu_ver_col_desarrollador_triggered(bool checked);
	void on_mnu_ver_col_tema_triggered(bool checked);
	void on_mnu_ver_col_grupo_triggered(bool checked);
	void on_mnu_ver_col_perspectiva_triggered(bool checked);
	void on_mnu_ver_col_idioma_triggered(bool checked);
	void on_mnu_ver_col_idioma_voces_triggered(bool checked);
	void on_mnu_ver_col_formato_triggered(bool checked);
	void on_mnu_ver_col_anno_triggered(bool checked);
	void on_mnu_ver_col_numdisc_triggered(bool checked);
	void on_mnu_ver_col_sistemaop_triggered(bool checked);
	void on_mnu_ver_col_tamano_triggered(bool checked);
	void on_mnu_ver_col_graficos_triggered(bool checked);
	void on_mnu_ver_col_sonido_triggered(bool checked);
	void on_mnu_ver_col_jugabilidad_triggered(bool checked);
	void on_mnu_ver_col_original_triggered(bool checked);
	void on_mnu_ver_col_estado_triggered(bool checked);
	void on_mnu_ver_col_fecha_triggered(bool checked);
	void on_mnu_ver_col_tipoemu_triggered(bool checked);
	void on_mnu_ver_col_favorito_triggered(bool checked);
	void on_mnu_ver_col_rating_triggered(bool checked);
	void on_mnu_ver_col_edad_triggered(bool checked);
	void on_mnu_ver_col_usuario_triggered(bool checked);
// Menú ver carpetas
	void on_mnu_ver_carpeta_datos_juego_triggered();
	void on_mnu_ver_carpeta_iconos_triggered();
	void on_mnu_ver_carpeta_smiles_triggered();
	void on_mnu_ver_carpeta_themes_triggered();
	void on_mnu_ver_carpeta_datos_triggered();
	void on_mnu_ver_carpeta_scripts_triggered();
	void on_mnu_ver_carpeta_templates_triggered();
	void on_mnu_ver_carpeta_idiomas_triggered();
	void on_mnu_ver_carpeta_temp_triggered();
	void on_mnu_ver_carpeta_home_triggered();
// Barra de heramientas
	void on_mnu_file_cerrar_big_triggered();
	void on_mnu_edit_nuevo_dosbox_big_triggered();
	void on_mnu_edit_nuevo_scummvm_big_triggered();
	void on_mnu_edit_nuevo_vdmsound_big_triggered();
	void on_mnu_ejecutar_juego_big_triggered();
	void on_mnu_ejecutar_setup_big_triggered();
	void on_mnu_edit_nuevo_big_triggered();
	void on_mnu_edit_editar_big_triggered();
	void on_mnu_edit_eliminar_big_triggered();
	void on_mnu_tool_importar_big_triggered();
	void on_mnu_ver_datos_big_triggered(bool checked);
	void on_mnu_ver_archivos_big_triggered(bool checked);
	void on_mnu_ver_capturas_big_triggered(bool checked);
	void on_mnu_tool_opciones_big_triggered();
	void on_mnu_ver_acercad_big_triggered();
// Barra de heramientas busqueda
	void on_txtBuscar_textChanged(const QString &arg1);
	void on_cbxOrdenBy_activated(int index);
	void on_cbxOrden_activated(int index);
	void on_btnOrdenar_clicked();
// Menú desplegable de categorias
	void on_cbxCategoriasTb_activated(int index);
	void on_cbxCategorias_activated(int index);
// Lista navegación
	void on_twListNav_itemClicked(QTreeWidgetItem *item, int column);
	void on_twListNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Lista de juegos QListView
	void lvJuegos_changed(const QModelIndex &current, const QModelIndex &previous);
	void on_lvJuegos_clicked(const QModelIndex &index);
	void on_lvJuegos_doubleClicked(const QModelIndex &index);
	void on_lvJuegos_customContextMenuRequested(const QPoint &pos);
// Lista principal QTreeView
	void tvJuegos_changed(const QModelIndex &current, const QModelIndex &previous);
	void on_tvJuegos_clicked(const QModelIndex &index);
	void on_tvJuegos_doubleClicked(const QModelIndex &index);
	void on_tvJuegos_customContextMenuRequested(const QPoint &pos);
// PictureFlow
	void grl_picflow_skip(int current);
	void grl_picflow_finishedAnimation();
	void grl_picflow_isActive(bool active);
	void grl_picflow_selected(int current);
	void grl_picflow_customContextMenuRequested(const QPoint &pos);
// Opciones de Datos
	void on_btnVer_Archivos_clicked(bool checked);
	void on_btnVer_CoverFront_clicked();
	void on_btnVer_CoverBack_clicked();
// Lista de files, urls, videos, sonidos y capturas
	void on_twFiles_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twUrls_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_lwVideos_itemDoubleClicked(QListWidgetItem *item);
	void on_lwCapturas_itemDoubleClicked(QListWidgetItem *item);
	void on_lwImagenes_itemDoubleClicked(QListWidgetItem *item);
	void directoryChanged(QString dir);
// Paneles
	void on_mnu_ver_auto_hide_title_bars_triggered(bool checked);
	void dockw_Nav_toggled(bool visible);
	void dockw_Datos_toggled(bool visible);
	void dockw_Files_toggled(bool visible);
	void dockw_Urls_toggled(bool visible);
	void dockw_Videos_toggled(bool visible);
	void dockw_Sonidos_toggled(bool visible);
	void dockw_Capturas_toggled(bool visible);
	void dockw_Imagenes_toggled(bool visible);
	void dockw_PictureFlow_toggled(bool visible);
// Procesos
	void fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus);
	void fin_ProcesoMount(int exitCode, QProcess::ExitStatus exitStatus);
	//void fin_ProcesoMountEjecutar();
	void error_Proceso(QProcess::ProcessError error);
	void error_ProcesoMount(QProcess::ProcessError error);

//	void sliderRange(int min, int max);
//	void on_btnLeftSc_triggered();
//	void on_btnRightSc_triggered();

};

#endif // GRLIDA_H
