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

#ifndef GRLIDA_VDMSADD_H
#define GRLIDA_VDMSADD_H

#include <QtCore>
#include <QtGui>

#include "funciones.h"
#include "ui_vdmsound.h"

class frmVdmsAdd : public QDialog
{
    Q_OBJECT

public:
	frmVdmsAdd(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmVdmsAdd();

	Ui::VdmsAddClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosVDMSound;

private:
	QString stTituloVdms(){ return tr("Nuevo juego para el VDMSound"); }

	Funciones fGrl;

	QString stHomeDir, stConfgVdmSDir, stTheme;

	QHash<QString, QVariant> GRLConfig;
	QHash<QString, QString> tempDatosJuego;


	void createConnections();
	void setTheme();
	void CargarConfig();

private slots:
	void on_btnOk();
	void on_btnVdms_FileConfg();
	void on_btnVdms_ExeJuego();
	void on_btnVdms_Icono();
	void on_txtDatos_Titulo_textChanged(const QString &);
	void on_btnDescargarInfo();

};

#endif // GRLIDA_VDMSADD_H
