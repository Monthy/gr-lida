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

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QHttp>
#include <QBuffer>
#include <QXmlStreamReader>

#include "funciones.h"
#include "httpdownload.h"
#include "grlida_importpath.h"
#include "ui_importar_juego_info.h"

enum e_fin_descarga {
	NohacerNada,
	MostarFichaCompleta,
	AnalizarPagina,
	AnalizarPaginaBusqueda,
	AnalizarPaginaFichaLIDA,
	AnalizarPaginaFichaMobyGames,
	CargarThumb,
	CargarCoverFront,
	CargarCoverBack
};

class frmImportarJuegoInfo : public QDialog
{
    Q_OBJECT
public:
	frmImportarJuegoInfo(QString titulo_busqueda, QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmImportarJuegoInfo();

	Ui::ImportarJuegoInfoClass ui;

	QHash<QString, QString> DatosJuego;

private:
	Funciones fGrl;
	frmImportPath *ImportPathNew;
	HttpDownload *httpdown;
	int indx_fin_descarga;
	QString stHomeDir, stTheme;
	QString stUrlSelect, stFileBusqueda, stFileFichaJuego;
	QString str_html_old, str_html_new;
	QString img_filename, img_thumbs, img_cover_front, img_cover_back;
	QFile file_thumbs, file_cover_front, file_cover_back;

	QXmlStreamReader xml;

	void createConnections();
	void setTheme();
	void adjustarHeader();
	void MostrarFichaHtml(QHash<QString, QString> datos);

	QString LeerArchivoHTML(QString str_file_html);
	void AnalyzeFindPage(QString Page);
// MobyGames -----------------------------------------------
	void AnalyzeGamePageMobyGames(QString Page);
	QString AnalyzeCategoriasMobyGames(QString Page, QString stRegExp, QString stRegExpDos, int indxExp=2);
	void AddGamesTitlesMobyGames(QString ResultsPage);

// La Isla del Abandoware ----------------------------------
	void AnalyzeGamePageLIDA(QString Page);
	void AddGamesTitlesLIDA(QString ResultsPage);

private slots:
	void on_twListaBusqueda_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_twListaBusqueda_clicked(QTreeWidgetItem *item);
	void on_twListaBusqueda_Dblclicked(QTreeWidgetItem *item);

	void on_Buscar();
	void on_Abortar();
	void on_btnVerInfo();
	void on_btnOk();

	void on_changeURL_XML(const QString &url);
	void isRequestFinished();

};

#endif // GRLIDA_IMPORTARJUEGOINFO_H
