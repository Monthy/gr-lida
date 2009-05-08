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

#ifndef GRLIDA_SVMADD_H
#define GRLIDA_SVMADD_H

#include <QtCore>
#include <QtGui>

#include "funciones.h"
#include "ui_svmadd.h"

class frmSvmAdd : public QDialog
{
    Q_OBJECT

public:
	frmSvmAdd(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmSvmAdd();

	Ui::SvmAddClass ui;

	QHash<QString, QString> DatosJuego;
	QHash<QString, QString> DatosScummVM;

private:
	QString stTituloSvm(){ return tr("Nuevo juego para el ScummVM"); }

	Funciones fGrl;
	int intStepwizard;

	QString stHomeDir, stDatosDir, stTheme;

	QHash<QString, QVariant> GRLConfig;
	QHash<QString, QString> tempDatosJuego;

	void createConnections();
	void setTheme();
	void CargarConfig();

private slots:
	void on_btnOk();
	void on_btnNext();
	void on_btnPrevious();
	void on_btnDirGame();
	void on_btnDirSave();
	void on_btnDefecto();
	void on_twScummVM_clicked(QTreeWidgetItem *item);
	void on_twScummVM_Dblclicked(QTreeWidgetItem *item);
	void on_twScummVM_currentItemChanged(QTreeWidgetItem *item1, QTreeWidgetItem *item2);
	void on_btnDescargarInfo();

};

#endif // GRLIDA_SVMADD_H
