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

#ifndef GRLIDA_IMPORTAR_JUEGO_H
#define GRLIDA_IMPORTAR_JUEGO_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"
#include "httpdownload.h"
#include "grlida_importpath.h"
#include "ui_importar_juego.h"

Q_DECLARE_METATYPE(QTreeWidgetItem*)
Q_DECLARE_METATYPE(QTreeWidget*)

// INICIO ImportarTwPrototype -----------------------------------------------------------------------------------
class ImportarTwPrototype : public QObject, public QScriptable
{
	Q_OBJECT

public:
	ImportarTwPrototype(QString dir_theme, QObject *parent = 0);

private:
	Funciones fGrl;
	QString stDirApp, stTheme;

public slots:
	void addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, QString icono = "tag");
	void addItemDatosFiles(const QString &nombre, const QString &crc, const QString &descripcion, const QString &size, const QString &path, QString icono = "archivos");
	void addItemMounts(const QString &path, const QString &label, const QString &tipo_as, const QString &letter, const QString &indx_cd, const QString &opt_mount, const QString &io_ctrl, const QString &select_mount, const QString &opt_size, const QString &opt_freesize, const QString &freesize);

};
// FIN ImportarTwPrototype --------------------------------------------------------------------------------------

namespace Ui {
	class frmImportarJuego;
}

class frmImportarJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmImportarJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, stConfigDOSBox datos_dbx, stConfigScummVM datos_svm, stConfigVDMSound datos_vdms, bool m_editando = false, QWidget *parent = 0);
	~frmImportarJuego();

	stDatosJuego getDatosJuegos(bool getTipoEmu = false);
	stConfigDOSBox getDatosDosBox(){return DatosDosBox;}
	stConfigScummVM getDatosScummVM(){return DatosScummVM;}
	stConfigVDMSound getDatosVDMSound(){return DatosVDMSound;}
	stGrlCfg getGrlCfg(){return grlCfg;}
	QString TipoEmu;

	Ui::frmImportarJuego *ui;

private:
	enum e_fin_descarga {
		NohacerNada            = 0,
		MostrarFicha           = 1,
		AnalizarPaginaBusqueda = 2,
		AnalizarPaginaFicha    = 3,
		CargarThumb            = 4,
		CargarCoverFront       = 5,
		CargarCoverBack        = 6
	};

	enum tab_datos {
		tabDatos    = 0,
		tabDOSBox   = 1,
		tabScummVM  = 2,
		tabVDMSound = 3
	};

	Funciones *fGrl;
	dbSql *sql;
	HttpDownload *httpdown;
	frmImportPath *ImportPathNew;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;
	stConfigDOSBox DatosDosBox;
	stConfigScummVM DatosScummVM;
	stConfigVDMSound DatosVDMSound;

	QHash<QString, QString> datosImportar;
	QHash<QString, stGrlDatos> emu_list;
	QHash<QString, stGrlDatos> smiles_list;
	QHash<QString, stGrlDatos> idiomas_list;
	QHash<QString, stGrlDatos> edades_list;

	QString str_url_web, str_html_old;

	QByteArray codecFileHtml;
	int index_fin_descarga;
	bool Editando;

// Ini Script ------
	QScriptEngine *engine;
	QScriptValue ctor;
	QScriptValueList args;
	ImportarTwPrototype *twProto;
// Fin Script ------

	void createConnections();
	void cargarConfig();
	void setTheme();

	void cargarScript(QString fileScript);
	void mostrarFichaHtml(QHash<QString, QString> datos);
	QString leerArchivoHTML(QString file_html);
//
	void downloadFile(QString dat_key, QString dat_url_key, QString tipo);
	void addTitles(QString filename);
	void analyzePage(QString filename, bool local = false, bool tipoDFend = false);
	void insertarDatosJuego(QString etiqueta, QString icono, QString old_dat, QString new_dat, QString key_dat);
	void setDatosJuego(QHash<QString, QString> datos);

private slots:
	void on_btnOk_clicked();
	void on_btnCancelar_clicked();

	void on_btnAbrir_clicked();
	void on_btnAbrirUrl_clicked();
	void on_btnBuscar_clicked();
	void on_btnAbortar_clicked();
	void on_btnVerInfo_clicked();

	void on_cbxScriptURL_activated(int index);
	void on_twListaBusqueda_itemClicked(QTreeWidgetItem *item, int column);
	void on_twListaBusqueda_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_txtTituloBuscar_returnPressed();

	void statusBtnEnabled(bool estado);
	void statusFinished();

};

#endif // GRLIDA_IMPORTAR_JUEGO_H
