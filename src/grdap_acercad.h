/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection
 * Copyright (C) 2014-2016 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRDAP_ACERCAD_H
#define GRDAP_ACERCAD_H

#include <QDialog>

#include "funciones.h"

namespace Ui {
	class frmGrDapAcercaD;
}

class frmGrDapAcercaD : public QDialog
{
	Q_OBJECT

public:
	explicit frmGrDapAcercaD(QWidget *parent = 0);
	~frmGrDapAcercaD();

private:
	Ui::frmGrDapAcercaD *ui;

	Funciones *fGrl;

	void cargarConfig();
	void setTheme();

private slots:
	void on_btnClose_clicked();
	void on_btnAcercaQT_clicked();

};

#endif // GRDAP_ACERCAD_H
