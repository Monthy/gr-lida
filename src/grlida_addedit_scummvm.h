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

#ifndef GRLIDA_ADDEDIT_SCUMMVM_H
#define GRLIDA_ADDEDIT_SCUMMVM_H

#include <QtGui>
#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"
#include "ui_addedit_scummvm.h"

class frmAddEditScummVM : public QTabWidget
{
    Q_OBJECT

public:
	frmAddEditScummVM(QWidget *parent = 0);
	~frmAddEditScummVM();

	Ui::AddEditScummVMClass ui;

	bool isCorrectNext();
	void setEditandoJuego(bool editando = false);
	void CargarConfigScummVM();
	void setConfigDefecto();
	void CargarDatosScummVM(QString IDGrl);
	void CargarDatosScummVM(QHash<QString, QString> datosSvm);
	QHash<QString, QString> setDatosScummVM();

private:
	QHash<QString, QVariant> GRLConfig;

	Funciones fGrl;
	dbSql *sql;

	bool EditandoJuego;

	QString stTituloAddEdit(){ return windowTitle(); }
	QString stHomeDir, stIdioma, stTheme, stItemIDSvm;

	void createConnections();
	void setTheme();

private slots:
	void on_twScummVM_clicked( QTreeWidgetItem *item );
	void on_btnDefecto();
	void on_btnSvm_Path();
	void on_btnSvm_PathSave();
	void on_btnSvm_PathSetup();
	void on_btnSvm_PathExtra();
	void on_btnSvm_PathCapturas();
	void on_btnSvm_PathSonido();
	void on_btnSvm_SoundFont();
};

#endif // GRLIDA_ADDEDIT_SCUMMVM_H
