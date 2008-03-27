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

#include <QDomDocument>
#include <QIcon>
#include <QTreeWidget>
#include <QtNetwork>

class QDialogButtonBox;
class QFile;
class QHttp;
class QHttpResponseHeader;
class QProgressDialog;
class QAuthenticator;
#include "funciones.h"
#include "ui_importar_juego.h"

class frmImportarJuego : public QDialog {
    Q_OBJECT
public:
	frmImportarJuego( QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmImportarJuego();

	Ui::ImportarJuegoClass ui;

	QHash<QString, QString>  DatosJuego;
	QHash<QString, QString>  DatosScummvm;
	QHash<QString, QString>  DatosDosBox;
	QHash<QString, QString>  DatosVDMSound;

private:
	Funciones fGrl;
	QString stHomeDir, texto_html, img_thumbs, img_cover_front, img_cover_back ;
	QString url_xmldb, xml_ListaJuegos, xml_InfoJuegos, url_filed;
	int indx_fin_descarga;
	bool xml_read(QString fileName);
    QHttp *http;
    QFile *file;
    QDomDocument domDocument;
    QProgressDialog *progressDialog;
	int  httpGetId;
    bool httpRequestAborted;
    void parseListaJuegos(const QDomElement &element);
    void LoadProfile_DFend(QString fileName);

private slots:
	void on_btnPrevious();
	void on_btnNext();
	void on_btnOk();
	void on_btnDirFileXML();
	void on_xml_open();
	void on_download();
	void on_treeWidget_Dblclicked( QTreeWidgetItem *item);
	void on_treeWidget_clicked( QTreeWidgetItem *item);

	void downloadFile(QString urlfile, QString fileName);
	void cancelDownload();
	void httpRequestFinished(int requestId, bool error);
	void readResponseHeader(const QHttpResponseHeader &responseHeader);
	void updateDataReadProgress(int bytesRead, int totalBytes);
	void enableDownloadButton();
	void slotAuthenticationRequired(const QString &, quint16, QAuthenticator *); // Qt 4.3.1
	void on_changeURL_XML(const QString &url);
};

#endif // GRLIDA_IMPORTARJUEGO_H
