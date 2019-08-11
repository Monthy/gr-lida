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

#ifndef GRLIDA_ADDEDIT_SCUMMVM_H
#define GRLIDA_ADDEDIT_SCUMMVM_H

#include <QTabWidget>

#include "funciones.h"
#include "dbsql.h"

namespace Ui {
	class frmAddEditScummVM;
}

class frmAddEditScummVM : public QTabWidget
{
	Q_OBJECT

public:
	explicit frmAddEditScummVM(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, bool m_editando, QWidget *parent = 0);
	~frmAddEditScummVM();

	bool isCorrectNext();
	void cargarConfig();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stConfigScummVM getDatosScummVM(){return DatosScummVM;}

	void setDatosScummVM();
	void cargarDatosScummVM(stConfigScummVM cfgSvm);

private:
	Ui::frmAddEditScummVM *ui;

	Funciones *fGrl;
	dbSql *sql;

	QString titulo_ventana(){ return windowTitle(); }

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;
	stConfigScummVM DatosScummVM;

	QStandardItemModel *svm_model;
	QList<stGrlDatos> list_svm;

	QString IdGame, IdSvm;
	bool Editando;

	void setTheme();

private slots:
	void on_cbxScummVM_activated(int index);

	void on_btnSvm_Path_clicked();
	void on_btnSvm_Path_clear_clicked();
	void on_btnSvm_PathSave_clicked();
	void on_btnSvm_PathSave_clear_clicked();
	void on_btnSvm_PathExtra_clicked();
	void on_btnSvm_PathExtra_clear_clicked();
	void on_btnSvm_SoundFont_clicked();
	void on_btnSvm_SoundFont_clear_clicked();

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

#endif // GRLIDA_ADDEDIT_SCUMMVM_H
