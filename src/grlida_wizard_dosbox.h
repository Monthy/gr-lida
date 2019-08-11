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

#ifndef GRLIDA_WIZARD_DOSBOX_H
#define GRLIDA_WIZARD_DOSBOX_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"
#include "grlida_importar_juego.h"

namespace Ui {
	class frmWizardDosBox;
}

class frmWizardDosBox : public QDialog
{
	Q_OBJECT

public:
	explicit frmWizardDosBox(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent = 0);
	~frmWizardDosBox();

	stGrlCfg getGrlCfg(){return grlCfg;}
	stDatosJuego getDatosJuegos(){return DatosJuego;}

private:
	Ui::frmWizardDosBox *ui;

	QString titulo_ventana(){ return tr("Nuevo juego para el DOSBox"); }

	Funciones *fGrl;
	dbSql *sql;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;
	stConfigDOSBox DatosDosBox;
	QList<stDatosImagenes> listImagenesImportadas;

	QString Thumbs, CoverFront, CoverBack, CoverLeft, CoverRight, CoverTop, CoverBottom;
	QString file_thumbs, file_cover_front, file_cover_back, file_cover_left, file_cover_right, file_cover_top, file_cover_bottom;
	int index_wizard;

	QHash<QString, stGrlDatos> dbx_list;
	bool isDbxSVN;

	void cargarConfig();
	void setTheme();

	void cargarDatosJuego(stDatosJuego datos);
	bool setDatosJuegos();

	void previerMontajes();
	void setDatosDosBox();
	void cargarDatosDosBox(stConfigDOSBox cfgDbx);
	void cargarDatosDBxMontaje(QList<stConfigDOSBoxMount> listMount);
	QList<stConfigDOSBoxMount> getListMount();

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnNext_clicked();
	void on_btnPrevious_clicked();

	void on_btnDbx_ExeJuego_clicked();
	void on_btnDbx_ExeJuego_clear_clicked();
	void on_btnDbx_ExeSetup_clicked();
	void on_btnDbx_ExeSetup_clear_clicked();
	void on_btnDescargarInfo_clicked();
	void on_btnInstalarJuego_clicked();
	void on_btnMount_Add_clicked();
	void on_btnMount_Edit_clicked();
	void on_btnMount_Delete_clicked();
	void on_btnMount_Clear_clicked();
	void on_btnMount_Subir_clicked();
	void on_btnMount_Bajar_clicked();
	void on_btnMount_AutoCrear_clicked();
	void on_btnMount_Primario_clicked();

	void on_cbxDbx_EmuKey_activated(int index);
	void on_cbxDbx_Profiles_activated(int index);

};

#endif // GRLIDA_WIZARD_DOSBOX_H
