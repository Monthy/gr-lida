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

#ifndef GRLIDA_ADDEDIT_VDMSOUND_H
#define GRLIDA_ADDEDIT_VDMSOUND_H

#include <QtGui>
#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"
#include "ui_addedit_vdmsound.h"

class frmAddEditVDMSound : public QTabWidget
{
    Q_OBJECT

public:
	frmAddEditVDMSound(QWidget *parent = 0);
	~frmAddEditVDMSound();

	Ui::AddEditVDMSoundClass ui;

	bool isCorrectNext();
	void setEditandoJuego(bool editando = false);
	void setConfigDefecto();
	void CargarDatosVDMSound(QString IDGrl);
	void CargarDatosVDMSound(QHash<QString, QString> datosVdms);
	QHash<QString, QString> setDatosVDMSound();

private:
	QHash<QString, QVariant> GRLConfig;

	Funciones fGrl;
	dbSql *sql;

	bool EditandoJuego;

	QString stTituloAddEdit(){ return windowTitle(); }
	QString stHomeDir, stTheme, stItemIDVdms;

	void createConnections();
	void setTheme();

private slots:
	void on_btnVdms_FileConfg();
	void on_btnVdms_ExeJuego();
	void on_btnVdms_Icono();

};

#endif // GRLIDA_ADDEDIT_VDMSOUND_H
