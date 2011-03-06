/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_INSTALAR_JUEGO_H
#define GRLIDA_INSTALAR_JUEGO_H

#include <QtCore>
#include <QtGui>

#include "funciones.h"
#include "ui_instalar_juego.h"

class frmInstalarJuego : public QDialog
{
    Q_OBJECT

public:
	frmInstalarJuego(QDialog *parent = 0, Qt::WFlags flags = 0);
	~frmInstalarJuego();

	Ui::InstalarJuegoClass ui;
private:
	Funciones fGrl;
	QString stHomeDir, stTheme;
	QHash<QString, QVariant> GRLConfig;

	void setTheme();
	void Ejecutar(const QString &bin, const QString &parametros = "", const QString &dirWorking = "");

private slots:
	void on_changeTypeDrive(int row);
	void on_btnOk();
	void on_btnOrigen();
	void on_btnDestino();
	void on_btnSubirIso();
	void on_btnBajarIso();
	void on_btnDeleteIso();
	void on_lwOrigenMultiPath_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
};

#endif // GRLIDA_INSTALAR_JUEGO_H
