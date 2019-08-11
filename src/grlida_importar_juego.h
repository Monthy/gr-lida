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

#ifndef GRLIDA_IMPORTAR_JUEGO_H
#define GRLIDA_IMPORTAR_JUEGO_H

#include <QDialog>
#include <QScriptable>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QScriptEngine>
//#include <QTreeWidget>
#include <QTreeWidgetItem>
//#include <QComboBox>
#include <QXmlStreamReader>

#include "funciones.h"
#include "dbsql.h"
#include "httpdownload.h"
#include "grlida_importar_datos.h"
#include "grlida_importar_dosbox.h"
#include "grlida_importar_scummvm.h"
#include "animatedtextbrowser.h"
#include "Qt7zip/qt7zip.h"

Q_DECLARE_METATYPE(QTreeWidgetItem*)
Q_DECLARE_METATYPE(QTreeWidget*)
Q_DECLARE_METATYPE(QComboBox*)

enum col_TwListaJuegos {
	icol_titulo     = 0,
	icol_anno       = 1,
	icol_plataforma = 2,
	icol_cbx_dosbox = 3,
	icol_url        = 4,
	icol_tipo_emu   = 5
};

enum col_TwDatosImages {
	col_img_thumb     = 0,
	col_img_nombre    = 1,
	col_img_tipo      = 2,
	col_img_url_full  = 3,
	col_img_url_small = 4,
	col_img_id        = 5
};

struct stZListItem {
	QString z_in;
	QString z_out;
};

struct stImport {
// Datos
	stDatosJuego datos;
	stDatosJuego datos_def;
	stCheckedDatos datos_chk;
	QList<stDatosUrls> urls;
	QList<stDatosFiles> files;
// DOSBox
	stConfigDOSBox dosbox;
	QList<stConfigDOSBoxMount> dbxMount;
// ScummVM
	stConfigScummVM scummvm;
// VDMSound
	stConfigVDMSound vdmsound;
// DBGL
	stConfigDBGL dbgl;
// Imágenes
	QList<stDatosImagenes> imagenes;
	QList<stDatosImagenes> imagenes_tmp;
// Zip
	QList<szEntryExtract> z_list;
// --
	bool    solo_datos;
	bool    solo_cfg_dosbox;
	bool    import_capturas;
	bool    is_importado;
	QString url_all_covers;
};

// INICIO ImportarTwPrototype -----------------------------------------------------------------------------------
class ImportarTwPrototype : public QObject, public QScriptable
{
	Q_OBJECT

public:
	ImportarTwPrototype(dbSql *m_sql, QString dir_theme, bool m_importMain = false, QObject *parent = 0);
	void setIdGame(QString id_game) { IdGame = id_game; }

	void clearListImport();
	QHash<QTreeWidgetItem *, stImport> getListImport() { return list_import; }

private:
	Funciones fGrl;
	dbSql *sql;

	stDatosJuego datos;
	stConfigDOSBox dosbox;
	stConfigScummVM scummvm;
	stConfigVDMSound vdmsound;

	QString stDirApp, stTheme, IdGame;
	bool importMain;
	QHash<QTreeWidgetItem *, stImport> list_import;

public slots:
	void addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, const QString &tipo_emu);
	void addItemPages(const QString &titulo, const QString &url);
//	void addItemDatosFiles(const QString &nombre, const QString &crc, const QString &descripcion, const QString &size, const QString &path, QString icono = "archivos");
//	void addItemMounts(const QString &path, const QString &label, const QString &tipo_as, const QString &letter, const QString &indx_cd, const QString &opt_mount, const QString &io_ctrl, const QString &select_mount, const QString &opt_size, const QString &opt_freesize, const QString &freesize);

};
// FIN ImportarTwPrototype --------------------------------------------------------------------------------------


namespace Ui {
class frmImportarJuego;
}

class frmImportarJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmImportarJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, stConfigDOSBox datos_dbx, stConfigScummVM datos_svm, stConfigVDMSound datos_vdms, bool m_import_main = false, bool m_editando = false, QWidget *parent = 0);
	~frmImportarJuego();

	stGrlCfg getGrlCfg() { return grlCfg; }
	QList<stImport> getDatosImport() { return list_import; }

protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	Ui::frmImportarJuego *ui;

	enum e_type_script {
		t_File   = 0,
		t_DBGL   = 1,
		t_Script = 2
	//	t_GRlida = 1,
	//	t_DFend  = 2,
	};

	enum e_fin_descarga {
		NohacerNada              = 0,
		MostrarFicha             = 1,
		AnalizarPaginaBusqueda   = 2,
		AnalizarPaginaFicha      = 3,
		AnalizarPaginaCovers     = 4,
		CargarThumb              = 5,
		CargarCovers             = 6,
		ImportarJuegos           = 7,
		AnalizarPaginaMoreCovers = 8,
		CargarTwTempImages       = 9
	};

	enum tab_datos {
		tabInfo     = 0,
		tabDatos    = 1,
		tabDOSBox   = 2,
		tabScummVM  = 3,
		tabOpciones = 4
//		tabVDMSound
	};

	Funciones *fGrl;
	dbSql *sql;
	HttpDownload *httpdown;
	Qt7zip *z_file;
	QXmlStreamReader xml;

	frmImportarDatos *wDat;
	frmImportarDosBox *wDbx;
	frmImportarScummVM *wSvm;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;
	stConfigDOSBox DatosDosBox;
	stConfigScummVM DatosScummVM;
	stConfigVDMSound DatosVDMSound;

	QList<QTreeWidgetItem *> items_dbgl;
	QHash<QTreeWidgetItem *, stImport> list_import_temp;
	QList<stImport> list_import;

	QHash<QString, QString> datosImportar;
	QHash<QString, QString> info_dbgl_export;
	QHash<QString, stGrlDatos> edades_list;
	QHash<QString, stGrlDatos> idiomas_list;
	QHash<QString, stGrlDatos> smiles_list;
	QHash<QString, stGrlDatos> dbx_list;
	QHash<QString, stGrlDatos> emu_list;
	QHash<QString, stGrlDatos> str_replace_list;

	QFileInfoList icon_list;
	QStringList icon_title_list;
	QStringList z_list;

	QString TipoEmu, str_url_web, z_dir_out;
	QString tpl_info_old, tpl_info_game_old, tpl_info_game_empty_old;
	QString total_num_pag, num_pag;

	QByteArray codecFileHtml;
	int index_fin_descarga, index_page, type_import;
	bool Editando, importMain, isItemChanged, isItemDoubleClicked, isScriptURLInit;
	bool isZip, z_is_open, is_load_7zlib;
	QFrame *msgInfo;
	QLabel *lb_msgInfo;

// Ini Script ------
	QScriptEngine *engine;
	QScriptValue ctor;
	QScriptValueList args;
	ImportarTwPrototype *twProto;
// Fin Script ------

	void createWidgets();
	void cargarConfig();
	void setTheme();
	void cargarTextResource();
	void cargarScript(QString fileScript);
	QString leerArchivoHTML(QString file_html);
	void mostrarFichaHtml(QTreeWidgetItem *item);
	void mostrarURLs(QList<stDatosUrls> urls);
	void mostrarFiles(QList<stDatosFiles> files);
	void setTabImportEnabled(QString tipo_emu);
	QString correctSlash(QString str, bool remplace = true);
	void defectoLimpiar();
	bool isSelectedItem(QTreeWidgetItem *item);

// HttpDownload
	void addTitles(QString filename);
	void analyzePage(QString filename, bool local = false, bool tipoDFend = false);
	void analyzePageCovers(QString filename);
	void analyzePageMoreCovers(QString filename);
	void analyzeImportImagenes(QString all_covers, QList<stDatosImagenes> &imagenes);
	void setScriptDatos();
	void setScriptImagenes(QTreeWidget *twImagen, QList<stDatosImagenes> imagenes, bool isCheck = false);

// Importar DBGL
	bool leerDbglZip(QString filename);
	bool leerDbglXml(QString filename);
	QString leerDbglSubItem(QString key, bool isDir = false);
	QHash<QString, QString> leerDbglSubItemDouble(QString key1, QString key2);
	void leerDbglExport();
	void leerDbglProfile();
	void leerDbglMetaInfo(QTreeWidgetItem *item, stImport &import);
	void leerDbglCfgDOSBox(stImport &import, QString cfg);
	void leerDbglFilesURL(stImport &import, QString raw, QString title);
	stConfigDOSBoxMount getDbglMount(QString linea, QString type);

// Importar
	void setDefaultDatosIsChecked(stImport &import);
	void createListImagesDownload(QTreeWidgetItem *item, QList<stDownItem> &listDown);
	void createListZipFiles(QTreeWidgetItem *item, const int z_listSize);
	void importarJuegos();

private slots:
//	void onValueDatosChanged(PropertyItem *prop, const QVariant &value);
	void onCfgDatChanged(stDatosJuego cfgDat);
	void onCfgDbxChanged(stConfigDOSBox cfgDbx);
	void onCfgSvmChanged(stConfigScummVM cfgSvm);
	void onCbxDbx_activated(QTreeWidgetItem *item);

	void on_cbxScriptURL_activated(int index);
	void on_txtTituloBuscar_returnPressed();
	void on_cbxNumPag_activated(int index);

	void on_txtInfo_anchorClicked(const QUrl &arg1);
	void on_txtInfoJuego_anchorClicked(const QUrl &arg1);

	void on_btnAbrir_clicked();
	void on_btnAbrirUrl_clicked();
	void on_btnBuscar_clicked();
	void on_btnAbortar_clicked();

	void on_twListaJuegos_itemClicked(QTreeWidgetItem *item, int column);
	void on_twListaJuegos_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twListaJuegos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column);

	void on_btnEliminarImagen_clicked();
	void on_btnAddImagen_clicked();
	void on_chkSelectDatosTempImages_toggled(bool checked);
	void on_btnDownAllCovers_clicked();

	void on_chkSelectListaJuegos_toggled(bool checked);
	void on_btnDownInfo_clicked();
	void on_btnToggleInfo_clicked();

	void on_chkSoloDatos_toggled(bool checked);
	void on_chkSoloCfgDOSBox_toggled(bool checked);
	void on_chkImportarCapturas_toggled(bool checked);
	void on_chkSoloDatos_all_toggled(bool checked);
	void on_chkSoloCfgDOSBox_all_toggled(bool checked);
	void on_chkImportarCapturas_all_toggled(bool checked);

	void on_btnOk_clicked();
	void on_btnCancelar_clicked();

// HttpDownload
	void statusBtnEnabled(bool estado);
	void statusWidgetEnabled(bool estado);
	void statusFinished();

};

#endif // GRLIDA_IMPORTAR_JUEGO_H
