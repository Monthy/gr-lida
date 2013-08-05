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

#ifndef GRLIDA_H
#define GRLIDA_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QProcess>
#include <QSystemTrayIcon>

#include "funciones.h"
#include "dbsql.h"
#include "grlida_delegate.h"
#include "grlida_picflow.h"

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
	bool eventFilter(QObject *obj, QEvent *event);
	void setVisible(bool visible);

private:
	Ui::GrLida *ui;

	QString tituloGrl(){ return tr("GR-lida lanzador para el DOSBox, ScummVM y VdmSound"); }

	Funciones *fGrl;
	dbSql *sql;
	GrlTreeViewDelegate *grl_tv_delegate;
	GrlListViewDelegate *grl_lv_delegate;
	GrlPicFlow *grl_picflow;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stCfgExec cfgExec;
	stLwIconCfg lwIconCfg;

	QString IdGame, TipoEmu, str_html_old;
	QString Thumbs, CoverFront, CoverBack;
	QString sql_where_select, sql_where_select_old;
	bool isInicio, isUpdateMenu, isPicFlowActive;

	unsigned int id_cat, skip_picflow, total_juegos, row_select, col_select;
	QHash<int, int> col;
	QHash<int, stGrlCats> categoria;
	QTreeWidgetItem *twListMnuNav;

	QHash<QString, stGrlDatos> emu_list;
	QHash<QString, stGrlDatos> smiles_list;
	QHash<QString, stGrlDatos> idiomas_list;
	QHash<QString, stGrlDatos> edades_list;

	GrlTreeViewModel *tv_model;
	QStandardItemModel *lv_model;

// StatusBar
	QLabel *lb_panel_1, *lb_panel_2, *lb_panel_3,*lb_panel_4, *lb_panel_5;

// Tray icon
	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu, *ljMenuPopUp;
	bool isTrayIcon;

	QProcess *grlProcess;

	void conectarBaseDatos();
	void comprobarArchivosDatos(QString version_grl, QString lng = "es_ES");
	void copiarArchivos(QString dir_old, QString dir_new, QString filter, QString filter_sep = ";");

	void createToolBars();
	void createStatusBar();
	void createTrayIcon();
	void createConnections();

	void cargarConfig();
	void guardarConfig();
	void setTheme();
	void showPopup(QWidget *w_parent, const QPoint &pos);
	void checkUpdateGrl(bool is_menu = false);

	void cargarListaCategorias(bool isEmit = true);
	void setConfigHeaderListaJuegos();
	void cargarListaJuegosDB(QString sql_where = "");
	void cargarListaPicFlow(QString sql_where = "");

	void setChangeCategorias(int cat_id);
	void menuNavAddCat(QString etiqueta, QString icono, QString sql_query = "", bool m_expanded = true, bool m_show_total = true);
	void menuNavAddSubCat(QString etiqueta, QString icono, QString sql_query = "", QString sql_col = "");
	void crearMenuNav();

	void nuevoEditarDatosDelJuego(stDatosJuego datos, bool isNew);
	stDatosJuego nuevoItemCopiarImagenes(stDatosJuego datos, QString tabla, bool isNew);

	void mostrarDatosDelJuego(QString IDitem = "");
	void itemNext();
	void itemBack();

	void cargarConfigEmu(QString tipo_emu = "datos");
	void comprobarEmuloresDisp();
	void ejecutar(QString bin, QString parametros = "", QString working_dir = "");

private slots:
	void centerIndexPicFlow(const QModelIndex &index);
	void skipPicFlow(int current);
	void finishedAnimationPicFlow();
	void isActivePicFlow(bool checked);
	void fin_Proceso(int exitCode, QProcess::ExitStatus exitStatus);
	void error_Proceso(QProcess::ProcessError error);

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
// Menú ejecutar
	void on_mnu_ejecutar_dosbox_triggered();
	void on_mnu_ejecutar_scummvm_triggered();
	void on_mnu_ejecutar_juego_triggered();
	void on_mnu_ejecutar_setup_triggered();
// Menú herramientas
	void on_mnu_tool_instalar_triggered();
	void on_mnu_tool_importar_triggered();
	void on_mnu_tool_exportar_triggered();
	void on_mnu_tool_opciones_triggered();
// Menú ver
	void on_mnu_ver_nav_triggered(bool checked);
	void on_mnu_ver_cover_mode_triggered(bool checked);
	void on_mnu_ver_archivos_url_triggered(bool checked);
	void on_mnu_ver_datos_triggered(bool checked);
	void on_mnu_ver_pictureflow_triggered(bool checked);
	void on_mnu_ver_pictureflow_to_center_triggered(bool checked);
	void on_mnu_ver_capturas_triggered(bool checked);
	void on_mnu_ver_ordenar_triggered(bool checked);
	void on_mnu_ver_menubar_triggered(bool checked);
	void on_mnu_ver_toolbar_triggered(bool checked);
	void on_mnu_ver_statusbar_triggered(bool checked);
	void on_mnu_ver_fullscreen_triggered(bool checked);
	void on_mnu_ver_ayuda_triggered();
	void on_mnu_ver_check_updates_triggered();
	void on_mnu_ver_acercad_triggered();
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
	void on_mnu_ver_carpeta_confdbx_triggered();
	void on_mnu_ver_carpeta_confvdms_triggered();
	void on_mnu_ver_carpeta_thumbs_triggered();
	void on_mnu_ver_carpeta_covers_triggered();
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
	void on_mnu_ver_archivos_url_big_triggered(bool checked);
	void on_mnu_ver_datos_big_triggered(bool checked);
	void on_mnu_ver_capturas_big_triggered(bool checked);
	void on_mnu_tool_opciones_big_triggered();
	void on_mnu_ver_acercad_big_triggered();
//--
	void on_txtBuscar_textChanged(const QString &arg1);
	void on_cbxOrdenBy_activated(int index);
	void on_cbxOrden_activated(int index);
	void on_btnOrdenar_clicked();

	void on_cbxCategoriasTb_activated(int index);
	void on_cbxCategorias_activated(int index);

	void on_twListNav_itemClicked(QTreeWidgetItem *item, int column);
	void on_twListNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

	void grl_picflow_customContextMenuRequested(const QPoint &pos);

	void on_lvJuegos_clicked(const QModelIndex &index);
	void on_lvJuegos_doubleClicked(const QModelIndex &index);
	void on_lvJuegos_customContextMenuRequested(const QPoint &pos);
	void on_tvJuegos_clicked(const QModelIndex &index);
	void on_tvJuegos_doubleClicked(const QModelIndex &index);
	void on_tvJuegos_customContextMenuRequested(const QPoint &pos);

	void on_twCapturas_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twCapturaVideo_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twCapturaSonido_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twUrls_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twFiles_itemDoubleClicked(QTreeWidgetItem *item, int column);

	void on_btnVer_Archivos_clicked(bool checked);
	void on_btnVer_CoverFront_clicked();
	void on_btnVer_CoverBack_clicked();

	void on_dockw_Nav_visibilityChanged(bool visible);
	void on_dockw_Datos_visibilityChanged(bool visible);
	void on_dockw_PictureFlow_visibilityChanged(bool visible);
	void on_dockw_FilesUrl_visibilityChanged(bool visible);

};

#endif // GRLIDA_H
