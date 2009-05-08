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

#ifndef GRLIDA_IMPORTARJUEGO_H
#define GRLIDA_IMPORTARJUEGO_H

#include <QtCore>
#include <QtGui>
#include <QtScript>
#include <QRegExp>


#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QHash>
#include <QScriptValue>
#include <QScriptEngine>
#include <QScriptable>

#include "httpdownload.h"
#include "funciones.h"
#include "ui_importar_juego.h"

enum e_fin_descarga {
	NohacerNada,
	MostrarFicha,
	AnalizarPaginaBusqueda,
	AnalizarPaginaFicha,
	CargarThumb,
	CargarCoverFront,
	CargarCoverBack
};

//! [1]
class ImportarTwPrototype : public QObject, public QScriptable
{
    Q_OBJECT

private:
	Funciones fGrl;
	QString stHomeDir, stTheme;

public:
	ImportarTwPrototype(QObject *parent = 0);

public slots:
	void addItemFind(const QString &titulo, const QString &plataforma, const QString &anno, const QString &url, QString icono = "tag");
	void addItemDatosFiles(const QString &nombre, const QString &crc, const QString &descripcion, const QString &size, const QString &path, QString icono = "archivos");
	void addItemMounts(const QString &path, const QString &label, const QString &tipo_as, const QString &letter, const QString &indx_cd, const QString &opt_mount, const QString &io_ctrl, const QString &select_mount);
};
//! [1]


class frmImportarJuego : public QDialog
{
    Q_OBJECT

public:
	frmImportarJuego(QString titulo_busqueda, QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmImportarJuego();

	Ui::ImportarJuegoClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosScummVM;
	QHash<QString, QString> DatosDosBox;
	QHash<QString, QString> DatosVDMSound;

private:
	Funciones fGrl;
	HttpDownload *httpdown;
	int indx_fin_descarga;

	QString stHomeDir, stTheme, stTempDir, stCoversDir, stThumbsDir;
	QString stFileBusqueda, stFileFicha, stUrlWeb;
	QString str_html_new, str_html_old;
	QString img_thumbs, img_cover_front, img_cover_back;
	QString img_url_cover_thumbs, img_url_cover_front, img_url_cover_back;
	QFile file_thumbs, file_cover_front, file_cover_back;

	QHash<QString, QString> DatosJuegoTemp;
	QHash<QString, QVariant> GRLConfig;

// Ini Script ------
	QString stScripts;
	QScriptEngine engine;
	QScriptValue ctor;
	QScriptValueList args;

	ImportarTwPrototype twProto;

	void CargarScript(QString fileScript);
// Fin Script ------

	void setTheme();
	void MostrarFichaHtml(QHash<QString, QString> datos);
	QString LeerArchivoHTML(QString file_html);
//
	void AddTitles(QString ResultsPage);
	void AnalyzePage(QString Page, bool local = false, bool tipoDFend = false);

private slots:
	void on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_twListaBusqueda_clicked(QTreeWidgetItem *item);
	void on_twListaBusqueda_Dblclicked(QTreeWidgetItem *item);

	void on_btnOk();
	void on_btnAbrir();
	void on_btnBuscar();
	void on_btnVerInfo();
	void on_btnAbrirUrl();
	void on_changeScriptURL(int row);
	void on_changeEnabled(bool estado);
	void isRequestFinished();

};

#endif // GRLIDA_IMPORTARJUEGO_H
