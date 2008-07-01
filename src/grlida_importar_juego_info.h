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

#ifndef GRLIDA_IMPORTARJUEGOINFO_H
#define GRLIDA_IMPORTARJUEGOINFO_H

#include <QtCore>
#include <QtGui>

#include <QHttp>
#include <QBuffer>
#include <QXmlStreamReader>

#include "funciones.h"
#include "httpdownload.h"
#include "ui_importar_juego_info.h"

class frmImportarJuegoInfo : public QDialog {
    Q_OBJECT
public:
	frmImportarJuegoInfo( QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmImportarJuegoInfo();

	Ui::ImportarJuegoInfoClass ui;

	QHash<QString, QString> DatosJuego;
private:
	Funciones fGrl;
	HttpDownload *httpdown;

	void setTheme();
	void parseXml();

	QString stHomeDir, stTheme, url_filed;
	QString img_thumbs, img_cover_front, img_cover_back;
	QString url_xmldb, temp_url_xmldb, texto_html, str_html_old;
	QXmlStreamReader xml;
	QString currentTag, str_id, str_id_emu, str_Icon, str_titulo;

	QHttp http;
	int connectionId;

private slots:
	void on_btnOk();
	void on_btnVerInfo();
	void fetch();
	void finished(int id, bool error);
	void readData(const QHttpResponseHeader &);
	void on_treeWidget_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_treeWidget_Dblclicked(QTreeWidgetItem *item);
	void itemActivated(QTreeWidgetItem * item);	
	void on_changeURL_XML(const QString &url);
	void isRequestFinished();

};

#endif // GRLIDA_IMPORTARJUEGOINFO_H
