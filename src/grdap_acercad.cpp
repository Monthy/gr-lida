/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014-2016 Pedro A. Garcia Rosado Aka Monthy
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
#include <QStyle>

#include "grdap_acercad.h"
#include "ui_grdap_acercad.h"

frmGrDapAcercaD::frmGrDapAcercaD(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmGrDapAcercaD)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmGrDapAcercaD::~frmGrDapAcercaD()
{
	delete fGrl;
	delete ui;
}

void frmGrDapAcercaD::setTheme()
{
	ui->btnAcercaQT->setIcon(QIcon(fGrl->theme() +"img16/qt.png"));
	ui->btnClose->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
}

void frmGrDapAcercaD::cargarConfig()
{
	QString str_info_ad = "";
	str_info_ad.append("<p style=\"font-size:18pt; font-weight:600;\">GR-dap v"+ fGrl->versionGrd() +"</p>");
	str_info_ad.append("<p style=\"font-size:10pt;\">"+ tr("Diseñado y programado por") +": <b>Pedro A. Garcia Rosado</b> "+ tr("a.k.a.") +" <b>Monthy</b></p>");
	str_info_ad.append("<p style=\"font-size:10pt;\">"+ tr("Visor de protecciones de juegos") +", "+ tr("Ruletas del tipo Monkey Island") +"<br />"+ tr("Cartas del tipo Alone in the Dark 2") +"<br />"+ tr("Texto oculto en el manual del tipo Indiana Jones y la Última Cruzada") +"</p>");
	str_info_ad.append("<p style=\"font-size:10pt;\"><a href=\"http://www.gr-lida.org/\" style=\"text-decoration:none;\">http://www.gr-lida.org/</a></p>");

	ui->lbad_info->setText(str_info_ad);
}

void frmGrDapAcercaD::on_btnClose_clicked()
{
	QDialog::accept();
}

void frmGrDapAcercaD::on_btnAcercaQT_clicked()
{
	qApp->aboutQt();
}
