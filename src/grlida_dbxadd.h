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

#ifndef GRLIDA_DBXADD_H
#define GRLIDA_DBXADD_H

#include <QtCore>
#include <QtGui>

#include "funciones.h"
#include "ui_dbxadd.h"

class frmDbxAdd : public QDialog
{
    Q_OBJECT

public:
	frmDbxAdd(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmDbxAdd();

	Ui::DbxAddClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosDosBox;

private:
	QString stTituloDbx(){ return tr("Nuevo juego para el DOSBox"); }

	Funciones fGrl;
	int intStepwizard;
	bool isImportDbx, isCreateMounts;

	QString stHomeDir, stTheme;

	QHash<QString, QVariant> GRLConfig;
	QHash<QString, QString> tempDatosJuego;
	QHash<QString, QString> tempProfileDosBox;

	void createConnections();
	void setTheme();
	void CargarConfig();
	void PrevierMontajes();
	void CargarDatosDosBox(QHash<QString, QString> datosDbx, bool isProfileGame = false);
	void CargarDatosDBxMontaje(QTreeWidget *twMontajesDbx);

private slots:
	void on_btnOk();
	void on_btnNext();
	void on_btnPrevious();
	void on_txtDatos_Titulo_textChanged(const QString &text);
	void on_txtDbx_path_exe_textChanged(const QString &text);
	void on_setProfileGame(int row);
	void on_btnDbx_FileConfg();
	void on_btnDbx_ExeJuego();
	void on_btnDbx_ExeSetup();
	void on_btnMount_Add();
	void on_btnMount_Edit();
	void on_btnMount_Delete();
	void on_btnMount_Clear();
	void on_btnMount_Subir();
	void on_btnMount_Bajar();
	void on_btnMount_AutoCrear();
	void on_btnMount_Primario();
	void on_btnDescargarInfo();
	void on_btnInstalarJuego();

};

#endif // GRLIDA_DBXADD_H
