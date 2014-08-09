/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef PDF_VIEWER_H
#define PDF_VIEWER_H

#include <QMainWindow>

#include "funciones.h"
#include "doc_pdf_widget.h"

namespace Ui {
	class frmPdfViewer;
}

class frmPdfViewer : public QMainWindow
{
	Q_OBJECT

public:
	explicit frmPdfViewer(QWidget *parent = 0);
	~frmPdfViewer();

	void openPdf(QString fileName);

protected:
	void closeEvent( QCloseEvent *e );

private:
	Ui::frmPdfViewer *ui;

	Funciones *fGrl;

	stGrlDir grlDir;

	QString lastPath, stHomeDir, stTheme;
	int id_scale;
	DocPdfWidget *doc_pdf_widget;
	QVector<qreal> scaleFactors;

	void cargarConfig();
	void setTheme();

private slots:
	void showSelectedText(const QString &text);
	void on_btn_Salir_clicked();
	void on_btn_Abrir_clicked();
	void on_btn_TextSelect_toggled(bool checked);
	void on_btn_PaginaBack_clicked();
	void on_btn_PaginaNext_clicked();
	void on_spinPagina_valueChanged(int arg1);
	void on_txtBuscar_editingFinished();
	void on_txtBuscar_textChanged(const QString &arg1);
	void on_btn_Buscar_clicked();
	void on_btn_Limpiar_clicked();
	void on_btn_ZoomIn_clicked();
	void on_btn_ZoomOut_clicked();
	void on_cbxZoom_activated(int index);
	void on_btn_ZoomNormal_clicked();

};

#endif // PDF_VIEWER_H
