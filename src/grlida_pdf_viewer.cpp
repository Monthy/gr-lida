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

#include "grlida_pdf_viewer.h"
#include "ui_pdf_viewer.h"

frmPdfViewer::frmPdfViewer(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::frmPdfViewer)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlDir.Home = fGrl->GRlidaHomePath();

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmPdfViewer::~frmPdfViewer()
{
	delete doc_pdf_widget;
	delete ui;
}

void frmPdfViewer::closeEvent(QCloseEvent *event)
{
	QSettings settings(grlDir.Home +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("PdfViewerState");
		settings.setValue("maximized"  , isMaximized() );
		settings.setValue("showTextSelect", ui->btn_TextSelect->isChecked() );
	settings.endGroup();

	event->accept();
}

void frmPdfViewer::cargarConfig()
{
	ui->cbxOrden->addItem(tr("Hacia adelante"), "adelante");
	ui->cbxOrden->addItem(tr("Hacia atras")   , "atras"   );
	ui->cbxOrden->setCurrentIndex(0);

	scaleFactors << 0.25 << 0.5 << 0.75 << 1. << 1.25 << 1.5 << 1.75 << 2. << 2.5 << 3. << 3.5 << 4.;
	ui->cbxZoom->addItem( "25%");
	ui->cbxZoom->addItem( "50%");
	ui->cbxZoom->addItem( "75%");
	ui->cbxZoom->addItem("100%");
	ui->cbxZoom->addItem("125%");
	ui->cbxZoom->addItem("150%");
	ui->cbxZoom->addItem("175%");
	ui->cbxZoom->addItem("200%");
	ui->cbxZoom->addItem("250%");
	ui->cbxZoom->addItem("300%");
	ui->cbxZoom->addItem("350%");
	ui->cbxZoom->addItem("400%");
	id_scale = 3;
	ui->cbxZoom->setCurrentIndex(3);

	doc_pdf_widget = new DocPdfWidget();
	ui->scrollArea->setWidget(doc_pdf_widget);
	connect(doc_pdf_widget, SIGNAL(pageChanged(int)), ui->spinPagina, SLOT(setValue(int)));
	connect(doc_pdf_widget, SIGNAL(textSelected(const QString &)), this, SLOT(showSelectedText(const QString &)));

	ui->btn_TextSelect->setChecked(false);
	emit on_btn_TextSelect_toggled(false);
}

void frmPdfViewer::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/pdf.png") );

	ui->btn_Salir->setIcon( QIcon(fGrl->Theme() +"img16/cerrar.png") );
	ui->btn_Abrir->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btn_TextSelect->setIcon( QIcon(fGrl->Theme() +"img16/edit_select_all.png") );
	ui->btn_PaginaBack->setIcon( QIcon(fGrl->Theme() +"img16/go-up.png") );
	ui->btn_PaginaNext->setIcon( QIcon(fGrl->Theme() +"img16/go-down.png") );
	ui->btn_Buscar->setIcon( QIcon(fGrl->Theme() +"img16/edit_buscar.png") );
	ui->btn_Limpiar->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btn_ZoomIn->setIcon( QIcon(fGrl->Theme() +"img16/zoom_in.png") );
	ui->btn_ZoomOut->setIcon( QIcon(fGrl->Theme() +"img16/zoom_out.png") );
	ui->btn_ZoomNormal->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );

	QSettings settings(grlDir.Home +"GR-lida.conf", QSettings::IniFormat);
	settings.beginGroup("PdfViewerState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
		ui->btn_TextSelect->setChecked( settings.value("showTextSelect", false).toBool() );
	settings.endGroup();
}

void frmPdfViewer::openPdf(QString fileName)
{
	if( fileName.isEmpty() )
		return;

	if( doc_pdf_widget->setDocument(fileName) )
	{
		lastPath = fileName;
		ui->btn_PaginaBack->setEnabled(false);
		ui->btn_PaginaNext->setEnabled(true);
		ui->btn_Buscar->setEnabled(true);
		ui->btn_Limpiar->setEnabled(true);
		ui->btn_ZoomIn->setEnabled(true);
		ui->btn_ZoomOut->setEnabled(true);
		ui->btn_ZoomNormal->setEnabled(true);
		ui->spinPagina->setEnabled(true);
		ui->spinPagina->setMinimum(1);
		ui->spinPagina->setMaximum(doc_pdf_widget->document()->numPages());
		ui->spinPagina->setValue(1);
		ui->txtBuscar->setEnabled(true);
		ui->cbxOrden->setEnabled(true);
		ui->cbxZoom->setEnabled(true);
	} else
		QMessageBox::warning(this, windowTitle(), tr("El archivo especificado no se ha podido abrir."));
}

void frmPdfViewer::showSelectedText(const QString &text)
{
	ui->txtTextSelect->setPlainText(text);
	ui->btn_TextSelect->setChecked(true);
}

void frmPdfViewer::on_btn_Salir_clicked()
{
	close();
}

void frmPdfViewer::on_btn_Abrir_clicked()
{
	QString fileName = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), lastPath, stHomeDir, tr("Abrir PDF") +" (*.pdf);;"+ tr("Todos los archivo") +" (*)");

	if( !fileName.isEmpty() )
		openPdf( fileName );
}

void frmPdfViewer::on_btn_TextSelect_toggled(bool checked)
{
	ui->txtTextSelect->setVisible(checked);
}

void frmPdfViewer::on_btn_PaginaBack_clicked()
{
	if( ui->spinPagina->value() > 0 )
	{
		ui->btn_PaginaBack->setEnabled(true);
		ui->btn_PaginaNext->setEnabled(true);
		ui->spinPagina->setValue(ui->spinPagina->value()-1);
	}

	if( ui->spinPagina->value() <= 1 )
		ui->btn_PaginaBack->setEnabled(false);
}

void frmPdfViewer::on_btn_PaginaNext_clicked()
{
	if( ui->spinPagina->value() < doc_pdf_widget->document()->numPages() )
	{
		ui->btn_PaginaBack->setEnabled(true);
		ui->btn_PaginaNext->setEnabled(true);
		ui->spinPagina->setValue(ui->spinPagina->value()+1);
	}

	if( ui->spinPagina->value() >= doc_pdf_widget->document()->numPages() )
		ui->btn_PaginaNext->setEnabled(false);
}

void frmPdfViewer::on_spinPagina_valueChanged(int arg1)
{
	doc_pdf_widget->setPage(arg1);
	ui->lb_pagina_count->setText("("+ fGrl->IntToStr(arg1) +" "+ tr("de") +" "+ fGrl->IntToStr(doc_pdf_widget->document()->numPages()) +")");
}

void frmPdfViewer::on_txtBuscar_editingFinished()
{
//	if( text.isEmpty() )
		doc_pdf_widget->setPage();
}

void frmPdfViewer::on_txtBuscar_textChanged(const QString &arg1)
{
	if( arg1.isEmpty() )
		doc_pdf_widget->setPage();
}

void frmPdfViewer::on_btn_Buscar_clicked()
{
	if( !ui->txtBuscar->text().isEmpty() )
	{
		QRectF location;
		if( ui->cbxOrden->currentIndex() == 0 )
			location = doc_pdf_widget->searchForwards(ui->txtBuscar->text());
		else
			location = doc_pdf_widget->searchBackwards(ui->txtBuscar->text());

		QPoint target = doc_pdf_widget->matrix().mapRect(location).center().toPoint();
		ui->scrollArea->ensureVisible(target.x(), target.y());
	}
}

void frmPdfViewer::on_btn_Limpiar_clicked()
{
	ui->txtBuscar->clear();
}

void frmPdfViewer::on_btn_ZoomIn_clicked()
{
	if( id_scale < 11)
	{
		id_scale++;
		ui->cbxZoom->setCurrentIndex(id_scale);
		emit on_cbxZoom_activated(id_scale);
	}
}

void frmPdfViewer::on_btn_ZoomOut_clicked()
{
	if( id_scale > 0 )
	{
		id_scale--;
		ui->cbxZoom->setCurrentIndex(id_scale);
		emit on_cbxZoom_activated(id_scale);
	}
}

void frmPdfViewer::on_cbxZoom_activated(int index)
{
	id_scale = index;
	doc_pdf_widget->setScale(scaleFactors[id_scale]);
}

void frmPdfViewer::on_btn_ZoomNormal_clicked()
{
	ui->cbxZoom->setCurrentIndex(3);
	emit on_cbxZoom_activated(3);
}
