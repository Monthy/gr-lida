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

#ifndef GRLIDA_CONFIG_INICIAL_H
#define GRLIDA_CONFIG_INICIAL_H

#include <QtCore>
#include <QtGui>
#include <QTranslator>
#include <QLocale>
#include "funciones.h"
#include "ui_configinicial.h"

class frmConfigInicial : public QDialog {
    Q_OBJECT
public:
	frmConfigInicial( QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmConfigInicial();
	
	Ui::ConfigInicialClass ui;
	
	QString IdiomaSelect ;

private:
	Funciones fGrl;
	QString stHomeDir ;
	QTranslator translator;
	bool IdiomaExterno;

private slots:
	void on_btnOk();
	void on_btnDirDbx();
	void on_btnDirSvm();
	void on_setLanguage(const QString txt_locale);

protected:
	void closeEvent( QCloseEvent *e );
};

#endif // GRLIDA_CONFIG_INICIAL_H
