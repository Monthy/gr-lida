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

#ifndef GRLIDA_SVMADD_H
#define GRLIDA_SVMADD_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmSvmAdd;
}

class frmSvmAdd : public QDialog
{
	Q_OBJECT

public:
	explicit frmSvmAdd(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent = 0);
	~frmSvmAdd();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stDatosJuego getDatosJuegos(){return DatosJuego;}

private:
	Ui::frmSvmAdd *ui;

	QString titulo_ventana(){ return tr("Nuevo juego para el ScummVM"); }

	Funciones *fGrl;
	dbSql *sql;

	stDatosJuego DatosJuego;
	stConfigScummVM DatosScummVM;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;
	int index_wizard;

	void cargarConfig();
	void setTheme();

	void setDatosScummVM();
	void cargarDatosScummVM(stConfigScummVM cfgSvm);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnNext_clicked();
	void on_btnPrevious_clicked();

	void on_twScummVM_itemClicked(QTreeWidgetItem *item, int column);
	void on_twScummVM_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_twScummVM_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void on_btnSvm_PathGame_clicked();
	void on_btnSvm_PathGame_clear_clicked();
	void on_btnSvm_PathSave_clicked();
	void on_btnSvm_PathSave_clear_clicked();
	void on_btnDescargarInfo_clicked();

	void on_h_SliderSvm_music_volume_valueChanged(int value);
	void on_h_SliderSvm_sfx_volume_valueChanged(int value);
	void on_h_SliderSvm_speech_volume_valueChanged(int value);
	void on_h_SliderSvm_tempo_valueChanged(int value);
	void on_h_SliderSvm_talkspeed_valueChanged(int value);
	void on_h_SliderSvm_debuglevel_valueChanged(int value);
	void on_h_SliderSvm_midi_gain_valueChanged(int value);
	void on_h_SliderSvm_walkspeed_valueChanged(int value);
	void on_btnDefectoSvm_clicked();

};

#endif // GRLIDA_SVMADD_H
