/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
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
#include <QInputDialog>

#include "grlida_buscar.h"
#include "ui_buscar.h"

frmBuscar::frmBuscar(QString f_text, QString f_ext, QString f_sel, QString m_theme, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmBuscar)
{
	ui->setupUi(this);
	this->setWindowTitle(tr("Buscando") +": "+ f_text);

	f_texto  = f_text;
	f_select = f_sel;
	f_theme  = m_theme;

	setTheme();

	ui->cbx_drivers->clear();
#ifdef Q_OS_WIN32
	f_texto.append(f_ext);
	QFileInfoList list_drives = QDir::drives();
	for (int i = 0; i < list_drives.size(); ++i)
		ui->cbx_drivers->addItem(QIcon(f_theme +"img16/drive_hd.png"), list_drives.at(i).absoluteFilePath());
	ui->cbx_drivers->setCurrentIndex(0);
#else
	f_texto = f_text.toLower();
	QDir dir("/");
	QStringList list_dir = dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);

	ui->cbx_drivers->addItem(QIcon(f_theme +"img16/carpeta_0.png"), "/");
	ui->cbx_drivers->addItem(QIcon(f_theme +"img16/carpeta_0.png"), "/usr/bin/");

	const int list_dirSize = list_dir.size();
	for (int i = 0; i < list_dirSize; ++i)
		ui->cbx_drivers->addItem(QIcon(f_theme +"img16/carpeta_0.png"), list_dir.at(i));
	ui->cbx_drivers->setCurrentIndex(1);
#endif

	buscar = new findFiles(this);
	connect(buscar, SIGNAL(finished()), this, SLOT(finished()));
	connect(buscar, SIGNAL(fileFind(QString)), this, SLOT(fileFind(QString)));

	load_movie = new QMovie(ui->lb_icon);
	ui->lb_icon->setMovie(load_movie);
	ui->lb_num->setText("");

// Centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmBuscar::~frmBuscar()
{
	delete ui;
}

void frmBuscar::closeEvent(QCloseEvent *event)
{
	emit on_btn_parar_clicked();
	event->accept();
}

void frmBuscar::setTheme()
{
	setWindowIcon(QIcon(f_theme +"img16/buscar.png"));

	ui->btn_buscar->setIcon(QIcon(f_theme +"img16/buscar.png"));
	ui->btn_pausa->setIcon(QIcon(f_theme +"img16/mp_pausa.png"));
	ui->btn_parar->setIcon(QIcon(f_theme +"img16/mp_stop.png"));
	ui->btn_aceptar->setIcon(QIcon(f_theme +"img16/aplicar.png"));
	ui->btn_cancelar->setIcon(QIcon(f_theme +"img16/cancelar.png"));
}

void frmBuscar::setStatusBotons(bool estado)
{
	isPpause = false;
	ui->btn_buscar->setEnabled(!estado);
	ui->btn_pausa->setEnabled(estado);
	ui->btn_parar->setEnabled(estado);
	ui->cbx_drivers->setEnabled(!estado);

	if (estado)
	{
		load_movie->setFileName(f_theme +"img16/carga.gif");
		load_movie->jumpToFrame(0);
		load_movie->start();
		ui->btn_aceptar->setEnabled(false);
		ui->cbx_lista->setEnabled(false);
		ui->cbx_lista->clear();
		buscar->find(ui->cbx_drivers->currentText(), f_texto);
	} else {
		load_movie->setFileName(f_theme +"img16/sinimg.png");
		load_movie->stop();
		load_movie->jumpToFrame(0);
		buscar->stop();
	}
}

void frmBuscar::on_btn_buscar_clicked()
{
	ui->lb_num->setText(tr("Encontrados") + ": 0");
	setStatusBotons(true);
}

void frmBuscar::on_btn_pausa_clicked()
{
	isPpause = isPpause ? false : true;
	load_movie->setPaused(isPpause);
	buscar->pause(isPpause);
}

void frmBuscar::on_btn_parar_clicked()
{
	if (ui->btn_parar->isEnabled())
		setStatusBotons(false);
}

void frmBuscar::on_btn_aceptar_clicked()
{
	emit on_btn_parar_clicked();

	if (!ui->cbx_lista->currentText().isEmpty())
		f_select = ui->cbx_lista->currentText();

	QDialog::accept();
}

void frmBuscar::on_btn_cancelar_clicked()
{
	emit on_btn_parar_clicked();

	QDialog::reject();
}

void frmBuscar::fileFind(QString filename)
{
	ui->cbx_lista->addItem(filename);
	ui->lb_num->setText(tr("Encontrados") +": "+ QVariant(ui->cbx_lista->count()).toString());

	if (ui->cbx_lista->count() > 0 && !ui->btn_aceptar->isEnabled())
	{
		ui->cbx_lista->setEnabled(true);
		ui->btn_aceptar->setEnabled(true);
	}
}

void frmBuscar::finished()
{
	if (ui->cbx_lista->count() > 0)
		ui->btn_aceptar->setEnabled(true);
	if (!isPpause)
		emit on_btn_parar_clicked();
}
