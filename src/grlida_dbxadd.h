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

#ifndef GRLIDA_DBXADD_H
#define GRLIDA_DBXADD_H

#include <QtCore>
#include <QtGui>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include "funciones.h"
#include "ui_dbxadd.h"

class frmDbxAdd : public QDialog {
    Q_OBJECT
public:
	frmDbxAdd( QDialog *parent = 0, Qt::WFlags flags = 0 );
    ~frmDbxAdd();
    
	Ui::DbxAddClass ui;
    
	QHash<QString, QString>  DatosJuego;
	QHash<QString, QString>  DatosDosBox;

private:
	Funciones fGrl;
	QString stTituloDbx(){ return tr("Nuevo juego para el DOSBox"); }
	int intStepwizard;
	QString stHomeDir;
	
private slots:
	void on_btnOk();
	void on_btnNext();
	void on_btnPrevious();
	void on_txtDbx_1_textChanged(const QString &);
	void on_btnFileConfg();
	void on_btnExeJuego();
	void on_btnExeSetup();
	void on_btnMount_Add();
	void on_btnMount_Edit();
	void on_btnMount_Delete();
	void on_btnMount_Clear();
	void on_btnMount_Subir();
	void on_btnMount_Bajar();
	void on_btnMount_AutoCrear();
	void on_btnMount_Primario();
};

#endif // GRLIDA_DBXADD_H
