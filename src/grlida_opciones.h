/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_OPCIONES_H
#define GRLIDA_OPCIONES_H

#include <QtCore>
#include <QtGui>
#include <QHash>
#include <QVariant>

#include "funciones.h"
#include "ui_opciones.h"

class frmOpciones : public QDialog
{
    Q_OBJECT

public:
	frmOpciones(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmOpciones();

	Ui::OpcionesClass ui;

	QHash<QString, QVariant> DatosConfiguracion;

protected:
	void closeEvent( QCloseEvent *e );

private:
	Funciones fGrl;
	QHash<QString, QVariant> GRLConfig;

	QString stHomeDir, url_xmldb;
	QString stIdiomaSelect, stTheme, stIconoFav, stPicFlowReflection, stNameDirTheme, stStyleSelect;
	QString stdb_type;
	QPalette originalPalette;

	void createConnections();
	void CargarConfig();
	void GuardarConfig();
	void setTheme();
	void CargarListaThemes();
	void changePalette();

private slots:
	void on_btnOk();
	void on_btnDirDbx();
	void on_btnDirSvm();
	void on_btnDirDB();
	void on_btnDirBaseGames();
	void on_setLanguage(int idx_locale);
	void on_changeStyle(const QString &styleName);
	void on_changeTypeDB(int idx_typedb);
	void on_twThemes_clicked( QTreeWidgetItem *item );
	void on_twThemes_currentItemChanged( QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_InfoFormatoFecha();
	void on_btnOpenUrl();
};

#endif // GRLIDA_OPCIONES_H
