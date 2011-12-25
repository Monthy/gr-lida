/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef PDF_VIEWER_H
#define PDF_VIEWER_H

#include <QMainWindow>

#include "funciones.h"
#include "doc_pdf_widget.h"
#include "ui_pdf_viewer.h"

class PdfViewer : public QMainWindow
{
    Q_OBJECT

public:
	PdfViewer(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PdfViewer();

	Ui::PdfViewerClass ui;

	void openPdf(QString fileName);

protected:
	void closeEvent( QCloseEvent *e );

private:
	Funciones fGrl;

	QString lastPath, stHomeDir, stTheme;
	int id_scale;
	DocPdfWidget *doc_pdf_widget;
    QVector<qreal> scaleFactors;

	void CargarConfig();
	void createConnections();
	void setTheme();

private slots:
	void open();
	void buscarTexto();
	void cambiarPagina(int id_pagina);
	void scaleDocument(int index);
	void checkSearchText(const QString &text);
	void showSelectedText(const QString &text);
	void on_btn_TextSelect(bool estado);
	void on_btn_PaginaBack();
	void on_btn_PaginaNext();
	void on_btn_ZoomIn();
	void on_btn_ZoomOut();
	void on_btn_ZoomNormal();

};

#endif // PDF_VIEWER_H
