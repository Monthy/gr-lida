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

#ifndef GRLIDA_OPCIONES_H
#define GRLIDA_OPCIONES_H

#include <QtCore>
#include <QtGui>
#include <QTranslator>
#include <QLocale>
#include "funciones.h"
#include "ui_opciones.h"

class frmOpciones : public QDialog {
    Q_OBJECT
public:
	frmOpciones( QDialog *parent = 0, Qt::WFlags flags = 0 );
	~frmOpciones();

	Ui::OpcionesClass ui;

	QHash<QString, QString> DatosConfiguracion;

private:
	Funciones fGrl;
	QString stHomeDir, IdiomaSelect, url_xmldb, stStyleSelect, stdb_type, stIconoFav;
	QPalette originalPalette;

private slots:
	void on_btnOk();
	void on_btnDirDbx();
	void on_btnDirSvm();
	void on_btnDirDB();
	void on_btnDirBaseGames();
	void on_setLanguage(const QString txt_locale);
	void on_changeStyle(const QString &styleName);
	void on_changeTypeDB(const QString &typedb);
	void changePalette();
};

#endif // GRLIDA_OPCIONES_H
