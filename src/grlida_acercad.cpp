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

#include <QDesktopWidget>

#include "grlida_acercad.h"
#include "ui_acercad.h"

frmAcercaD::frmAcercaD(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmAcercaD)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmAcercaD::~frmAcercaD()
{
	delete fGrl;
	delete ui;
}

void frmAcercaD::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/acercad.png") );

	ui->lb_Dbximg->setPixmap( QPixmap(fGrl->Theme() +"img24/dosbox.png") );
	ui->lb_Svmimg->setPixmap( QPixmap(fGrl->Theme() +"img24/scummvm.png") );
	ui->lb_Vdmsimg->setPixmap( QPixmap(fGrl->Theme() +"img24/vdmsound.png") );
	ui->tabAcercaD->setTabIcon(0, QIcon(fGrl->Theme() +"img16/datos.png") );
	ui->tabAcercaD->setTabIcon(1, QIcon(fGrl->Theme() +"img16/idiomas.png") );
	ui->tabAcercaD->setTabIcon(2, QIcon(fGrl->Theme() +"img16/nuevo.png") );
	ui->btnAcercaQT->setIcon( QIcon(fGrl->Theme() +"img16/qt.png") );
	ui->btnClose->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
}

void frmAcercaD::cargarConfig()
{
	ui->txtAcercaD->setHtml("<center><strong>GR-lida</strong> "+ tr("es un lanzador común para los emuladores:") +
		"<br><br><strong>DOSBox</strong>, <strong><font color=\"#009900\">ScummVM</font></strong>, <strong>VDMSound</strong><br><br>"
		"<strong>GR-lida</strong> "+ tr("es GPL. Para mejorar el programa puedes dejar tu opinión en") +"<br><a href=\"http://www.gr-lida.org/\">http://www.gr-lida.org/</a></center><br>"+
		tr("Web Oficiales del lanzador y los distintos emuladores:") +"<br>"+
		tr("Lanzador") +" <strong>GR-lida</strong>: <a href=\"http://www.gr-lida.org/\">http://www.gr-lida.org/</a><br>"+
		tr("Emulador") +" <strong>DOSBox</strong>: <a href=\"http://www.dosbox.com/\">http://www.dosbox.com/</a><br>"+
		tr("Emulador") +" <strong><font color=\"#009900\">ScummVM</font></strong>: <a href=\"http://www.scummvm.org/\">http://www.scummvm.org/</a><br>"+
		tr("Emulador") +" <strong>VDMSound</strong>: <a href=\"http://sourceforge.net/projects/vdmsound/\">http://sourceforge.net/projects/vdmsound/</a><br>"
	);

	ui->lb_byOS->setText("Windows, Linux, Mac");
	ui->lb_version->setText( "v"+ fGrl->versionGrl() );
	ui->lb_Dbx->setText( "Dosbox v"+ fGrl->versionDbx() );
	ui->lb_Svm->setText( "ScummVM v"+ fGrl->versionSvm() );
	ui->lb_Vdms->setText("VDMSound v"+ fGrl->versionVdms() );

// Abrimos la lista de Traductores
	fGrl->cargarDatosTwLista(ui->treeWidget, ":/list_translators.txt", TwListTraduccion);
	ui->treeWidget->setColumnWidth(0, 250);

// licencia GPL
	ui->txtLicense->setPlainText( fGrl->leerArchivo(":/license.gpl", "UTF-8") );
	ui->txtLicense->moveCursor( QTextCursor::Start );
}

void frmAcercaD::on_btnClose_clicked()
{
	QDialog::accept();
}

void frmAcercaD::on_btnAcercaQT_clicked()
{
	qApp->aboutQt();
}
