/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2018 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#ifndef GRLIDA_BUSCAR_H
#define GRLIDA_BUSCAR_H

#include <QDialog>
#include <QMovie>

#include <QCloseEvent>

#include "findfiles.h"

namespace Ui {
	class frmBuscar;
}

class frmBuscar : public QDialog
{
	Q_OBJECT

public:
	explicit frmBuscar(QString f_text, QString f_ext, QString f_sel, QString m_theme, QWidget *parent = 0);
	~frmBuscar();

	QString getSelect(){return f_select;}

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmBuscar *ui;

	findFiles *buscar;
	bool isPpause;
	QString f_texto, f_select, f_theme, styleSheet;
	QMovie *load_movie;

	void setTheme();
	void setStatusBotons(bool estado);

private slots:
	void on_btn_buscar_clicked();
	void on_btn_pausa_clicked();
	void on_btn_parar_clicked();
	void on_btn_aceptar_clicked();
	void on_btn_cancelar_clicked();
//--
	void finished();
	void fileFind(QString filename);

};

#endif // GRLIDA_BUSCAR_H
