/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_CONFIG_INICIAL_H
#define GRLIDA_CONFIG_INICIAL_H

#include <QDialog>
#include <QTranslator>

#include "funciones.h"

namespace Ui {
	class frmConfigInicial;
}

class frmConfigInicial : public QDialog
{
	Q_OBJECT

public:
	frmConfigInicial(stGrlCfg m_cfg, QWidget *parent = 0);
	~frmConfigInicial();

	stGrlCfg getGrlCfg(){return grlCfg;}

private:
	Ui::frmConfigInicial *ui;

	Funciones *fGrl;
	QTranslator translator;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	void cargarConfig();
	void guardarConfig();
	void setTheme();

private slots:
	void on_cbxIdioma_activated(int index);

	void on_btnDirDbx_clicked();
	void on_btnDirDbx_find_clicked();
	void on_btnDirDbx_clear_clicked();

	void on_btnDirSvm_clicked();
	void on_btnDirSvm_find_clicked();
	void on_btnDirSvm_clear_clicked();

	void on_btnOk_clicked();

};

#endif // GRLIDA_CONFIG_INICIAL_H
