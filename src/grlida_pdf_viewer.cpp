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

#include "grlida_pdf_viewer.h"

PdfViewer::PdfViewer(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	stHomeDir = fGrl.GRlidaHomePath();
	stTheme   = fGrl.ThemeGrl();

	CargarConfig();
	createConnections();
	setTheme();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

PdfViewer::~PdfViewer()
{
	delete doc_pdf_widget;
}

void PdfViewer::closeEvent( QCloseEvent *e )
{
	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("PdfViewerState");
		settings.setValue("maximized"  , isMaximized() );
		settings.setValue("showTextSelect", ui.btn_TextSelect->isChecked() );
	settings.endGroup();

	e->accept();
}

void PdfViewer::CargarConfig()
{
	ui.cbxOrden->addItem(tr("Hacia adelante"), "adelante");
	ui.cbxOrden->addItem(tr("Hacia atras")   , "atras"   );
	ui.cbxOrden->setCurrentIndex(0);

	scaleFactors << 0.25 << 0.5 << 0.75 << 1. << 1.25 << 1.5 << 1.75 << 2. << 2.5 << 3. << 3.5 << 4.;
	ui.cbxZoom->addItem( "25%");
	ui.cbxZoom->addItem( "50%");
	ui.cbxZoom->addItem( "75%");
	ui.cbxZoom->addItem("100%");
	ui.cbxZoom->addItem("125%");
	ui.cbxZoom->addItem("150%");
	ui.cbxZoom->addItem("175%");
	ui.cbxZoom->addItem("200%");
	ui.cbxZoom->addItem("250%");
	ui.cbxZoom->addItem("300%");
	ui.cbxZoom->addItem("350%");
	ui.cbxZoom->addItem("400%");
	id_scale = 3;
	ui.cbxZoom->setCurrentIndex(3);

	doc_pdf_widget = new DocPdfWidget();
	ui.scrollArea->setWidget(doc_pdf_widget);
}

void PdfViewer::createConnections()
{
	connect(ui.btn_Salir     , SIGNAL( clicked() ), this, SLOT( close()             ));
	connect(ui.btn_Abrir     , SIGNAL( clicked() ), this, SLOT( open()              ));
	connect(ui.btn_TextSelect, SIGNAL( toggled(bool) ), this, SLOT( on_btn_TextSelect(bool) ));
	connect(ui.btn_PaginaBack, SIGNAL( clicked() ), this, SLOT( on_btn_PaginaBack() ));
	connect(ui.btn_PaginaNext, SIGNAL( clicked() ), this, SLOT( on_btn_PaginaNext() ));
	connect(ui.btn_Buscar    , SIGNAL( clicked() ), this, SLOT( buscarTexto()    ));
	connect(ui.btn_Limpiar   , SIGNAL( clicked() ), doc_pdf_widget, SLOT(setPage()  ));
	connect(ui.btn_ZoomIn    , SIGNAL( clicked() ), this, SLOT( on_btn_ZoomIn()     ));
	connect(ui.btn_ZoomOut   , SIGNAL( clicked() ), this, SLOT( on_btn_ZoomOut()    ));
	connect(ui.btn_ZoomNormal, SIGNAL( clicked() ), this, SLOT( on_btn_ZoomNormal() ));

	connect(ui.spinPagina, SIGNAL(valueChanged(int)), this, SLOT( cambiarPagina(int) ));
	connect(ui.txtBuscar, SIGNAL(returnPressed()), this, SLOT(buscarTexto()));
	connect(ui.txtBuscar, SIGNAL(textChanged(const QString &)), this, SLOT(checkSearchText(const QString &)));
	connect(ui.cbxZoom, SIGNAL(currentIndexChanged(int)), this, SLOT(scaleDocument(int)));
	connect(doc_pdf_widget, SIGNAL(pageChanged(int)), ui.spinPagina, SLOT(setValue(int)));
	connect(doc_pdf_widget, SIGNAL(textSelected(const QString &)), this, SLOT(showSelectedText(const QString &)));

	ui.btn_TextSelect->setChecked(false);
	emit on_btn_TextSelect(false);
}

void PdfViewer::setTheme()
{
	setWindowTitle(tr("Ver documentos en PDF"));
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/pdf.png") );

	ui.btn_Salir->setIcon( QIcon(stTheme+"img16/cerrar.png") );
	ui.btn_Abrir->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btn_TextSelect->setIcon( QIcon(stTheme+"img16/edit_select_all.png") );
	ui.btn_PaginaBack->setIcon( QIcon(stTheme+"img16/go-up.png") );
	ui.btn_PaginaNext->setIcon( QIcon(stTheme+"img16/go-down.png") );
	ui.btn_Buscar->setIcon( QIcon(stTheme+"img16/edit_buscar.png") );
	ui.btn_Limpiar->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btn_ZoomIn->setIcon( QIcon(stTheme+"img16/zoom_in.png") );
	ui.btn_ZoomOut->setIcon( QIcon(stTheme+"img16/zoom_out.png") );
	ui.btn_ZoomNormal->setIcon( QIcon(stTheme+"img16/zoom.png") );

	QSettings settings( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("PdfViewerState");
		if( settings.value("maximized", isMaximized() ).toBool() ) // maximizado.
			setWindowState( windowState() | Qt::WindowMaximized );
		ui.btn_TextSelect->setChecked( settings.value("showTextSelect", false).toBool() );
	settings.endGroup();

	settings.beginGroup("OpcFuente");
		if( settings.value("font_usar", false).toBool() )
			setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+settings.value("font_family","Tahoma").toString()+"\";font-size:"+settings.value("font_size","8").toString()+"pt;}");
	settings.endGroup();
}

void PdfViewer::open()
{
	QString fileName = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), lastPath, stHomeDir,
												tr("Abrir PDF")+" (*.pdf);;"+
												tr("Todos los archivo") + " (*)", 0, false);
	if( !fileName.isEmpty() )
		openPdf( fileName );
}

void PdfViewer::openPdf(QString fileName)
{
	if( fileName.isEmpty())
		return;

	if( doc_pdf_widget->setDocument(fileName) )
	{
		lastPath = fileName;
		ui.btn_PaginaBack->setEnabled(false);
		ui.btn_PaginaNext->setEnabled(true);
		ui.btn_Buscar->setEnabled(true);
		ui.btn_Limpiar->setEnabled(true);
		ui.btn_ZoomIn->setEnabled(true);
		ui.btn_ZoomOut->setEnabled(true);
		ui.btn_ZoomNormal->setEnabled(true);
		ui.spinPagina->setEnabled(true);
		ui.spinPagina->setMinimum(1);
		ui.spinPagina->setMaximum(doc_pdf_widget->document()->numPages());
		ui.spinPagina->setValue(1);
		ui.txtBuscar->setEnabled(true);
		ui.cbxOrden->setEnabled(true);
		ui.cbxZoom->setEnabled(true);
	} else
		QMessageBox::warning(this, windowTitle(), tr("El archivo especificado no se ha podido abrir."));
}

void PdfViewer::buscarTexto()
{
	if( ui.txtBuscar->text() != "" )
	{
		QRectF location;
		if( ui.cbxOrden->currentIndex() == 0 )
			location = doc_pdf_widget->searchForwards(ui.txtBuscar->text());
		else
			location = doc_pdf_widget->searchBackwards(ui.txtBuscar->text());

		QPoint target = doc_pdf_widget->matrix().mapRect(location).center().toPoint();
		ui.scrollArea->ensureVisible(target.x(), target.y());
	}
}

void PdfViewer::cambiarPagina(int id_pagina)
{
	doc_pdf_widget->setPage(id_pagina);
	ui.lb_pagina_count->setText("("+ fGrl.IntToStr(id_pagina)+" "+ tr("de") +" "+ fGrl.IntToStr(doc_pdf_widget->document()->numPages()) +")");
}

void PdfViewer::scaleDocument(int index)
{
	id_scale = index;
	doc_pdf_widget->setScale(scaleFactors[id_scale]);
}

void PdfViewer::checkSearchText(const QString &text)
{
	if( text.isEmpty() )
		doc_pdf_widget->setPage();
}

void PdfViewer::showSelectedText(const QString &text)
{
	ui.txtTextSelect->setPlainText(text);
	ui.btn_TextSelect->setChecked(true);
}

void PdfViewer::on_btn_TextSelect(bool estado)
{
	ui.txtTextSelect->setVisible(estado);
}

void PdfViewer::on_btn_PaginaBack()
{
	if( ui.spinPagina->value() > 0 )
	{
		ui.btn_PaginaBack->setEnabled(true);
		ui.btn_PaginaNext->setEnabled(true);
		ui.spinPagina->setValue(ui.spinPagina->value()-1);
	}

	if( ui.spinPagina->value() <= 1 )
		ui.btn_PaginaBack->setEnabled(false);
}

void PdfViewer::on_btn_PaginaNext()
{
	if( ui.spinPagina->value() < doc_pdf_widget->document()->numPages() )
	{
		ui.btn_PaginaBack->setEnabled(true);
		ui.btn_PaginaNext->setEnabled(true);
		ui.spinPagina->setValue(ui.spinPagina->value()+1);
	}

	if( ui.spinPagina->value() >= doc_pdf_widget->document()->numPages() )
		ui.btn_PaginaNext->setEnabled(false);
}

void PdfViewer::on_btn_ZoomIn()
{
	if( id_scale < 11)
	{
		id_scale++;
		ui.cbxZoom->setCurrentIndex(id_scale);
		emit scaleDocument(id_scale);
	}
}

void PdfViewer::on_btn_ZoomOut()
{
	if( id_scale > 0 )
	{
		id_scale--;
		ui.cbxZoom->setCurrentIndex(id_scale);
		emit scaleDocument(id_scale);
	}
}

void PdfViewer::on_btn_ZoomNormal()
{
	ui.cbxZoom->setCurrentIndex(3);
	emit scaleDocument(3);
}
