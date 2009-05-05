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

#ifndef GRLIDA_ADDEDIT_DOSBOX_H
#define GRLIDA_ADDEDIT_DOSBOX_H

#include <QtGui>
#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"
#include "ui_addedit_dosbox.h"

class frmAddEditDosBox : public QTabWidget
{
    Q_OBJECT

public:
	frmAddEditDosBox(QWidget *parent = 0);
	~frmAddEditDosBox();

	Ui::AddEditDosBoxClass ui;

	void setEditandoJuego(bool editando = false);
	void CargarConfigDosBox();
	void setConfigDefecto();
	void CargarDatosDosBox(QString IDGrl, QString ProfileGame = "", bool isProfileGame = false);
	QHash<QString, QString> setDatosDosBox();

private:
	QHash<QString, QVariant> GRLConfig;

	Funciones fGrl;
	dbSql *sql;

	bool EditandoJuego;

	QString stTituloAddEdit(){ return windowTitle(); }
	QString stHomeDir, stTheme, stItemIDDbx;

	void createConnections();
	void setTheme();

	void PrevierMontajes();
	void CargarDatosDBxMontaje( QString IDdbx);
	QString setOpcionesSerial();

private slots:
	void on_setProfileGame(const QString ProfileGame);
	void on_btnDbx_FileConfg();
	void on_btnDbx_ExeJuego();
	void on_btnDbx_ExeSetup();
	void on_btnDirGravisUltraSound();
	void on_btnDbx_mapperfile();
	void on_btnDbx_language();
	void on_btnDbx_capturas();
	void on_btnDbx_musica();
	void on_btnDbx_AddSerial();
	void on_btnMount_Add();
	void on_btnMount_Edit();
	void on_btnMount_Delete();
	void on_btnMount_Clear();
	void on_btnMount_Subir();
	void on_btnMount_Bajar();
	void on_btnMount_AutoCrear();
	void on_btnMount_Primario();

};

#endif // GRLIDA_ADDEDIT_DOSBOX_H
