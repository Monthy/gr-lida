/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2014 Pedro A. Garcia Rosado Aka Monthy
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

#ifndef GRLIDA_ADDEDIT_JUEGO_H
#define GRLIDA_ADDEDIT_JUEGO_H

#include <QDialog>

#include "funciones.h"
#include "dbsql.h"
#include "grlida_addedit_dosbox.h"
#include "grlida_addedit_scummvm.h"
#include "grlida_addedit_vdmsound.h"
#include "qcheckcombobox.h"
#include "editorwidget/editorwidget.h"

namespace Ui {
	class frmAddEditJuego;
}

class frmAddEditJuego : public QDialog
{
	Q_OBJECT

public:
	explicit frmAddEditJuego(dbSql *m_sql, stGrlCfg m_cfg, stGrlCats m_categoria, QString id_game, QString tipo_emu, bool m_editando, QWidget *parent = 0);
	~frmAddEditJuego();

	stDatosJuego getDatosJuegos(){return DatosJuego;}
	stGrlCfg getGrlCfg(){return grlCfg;}

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmAddEditJuego *ui;

	enum tab_datos {
		tabDOSBox   = 4,
		tabScummVM  = 5,
		tabVDMSound = 6
	};

	QString titulo_ventana(){ return windowTitle(); }

	Funciones *fGrl;
	dbSql *sql;
	EditorWidget *txtDatos_Comentario;

	frmAddEditDosBox *wDbx;
	frmAddEditScummVM *wSvm;
	frmAddEditVDMSound *wVdms;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stDatosJuego DatosJuego;
	stConfigDOSBox DatosDosBox;
	stConfigScummVM DatosScummVM;
	stConfigVDMSound DatosVDMSound;

	QString IdGame, TipoEmu, oldTitulo, str_html_old;
	QString Thumbs, CoverFront, CoverBack;
	QString file_thumbs, file_cover_front, file_cover_back;

	QHash<QString, stGrlDatos> emu_list;
	bool Editando;

	QCheckComboBox *cbxDatos_Genero;
	QCheckComboBox *cbxDatos_Compania;
	QCheckComboBox *cbxDatos_Desarrollador;
	QCheckComboBox *cbxDatos_Tema;
	QCheckComboBox *cbxDatos_Grupo;
	QCheckComboBox *cbxDatos_Idioma;
	QCheckComboBox *cbxDatos_IdiomaVoces;
	QCheckComboBox *cbxDatos_Formato;
	QCheckComboBox *cbxDatos_Perspectiva;
	QCheckComboBox *cbxDatos_SistemaOp;

	void createWidgets();
	void createConnections();
	void cargarConfig();
	void setTheme();

	void cargarDatosJuego(stDatosJuego datos, bool isImport = false);
	bool setDatosJuegos(bool isSoloDatos = false);
	void comprobarDirCapturas(QString dir_capturas, QString old_titulo, QString new_titulo, QString tipo_emu);

	void enabledUrlUpdate(QString texto, int col);
	void addEditTwDatosURL(bool isNew);

	void enabledDatosUpdate(QString texto, int col);
	void addEditTwDatosFiles(bool isNew);
	void buscarTexto(QString texto, bool anterior = false);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_btnDescargarInfo_clicked();
	void on_txtDatos_Titulo_editingFinished();
	void on_cbxDatos_TipoEmu_activated(int index);
	void on_btnImgAbrir_Thumbs_clicked();
	void on_btnImgVer_Thumbs_clicked();
	void on_btnImgEliminar_Thumbs_clicked();
	void on_btnImgAbrir_CoverFront_clicked();
	void on_btnImgVer_CoverFront_clicked();
	void on_btnImgEliminar_CoverFront_clicked();
	void on_btnImgAbrir_CoverBack_clicked();
	void on_btnImgVer_CoverBack_clicked();
	void on_btnImgEliminar_CoverBack_clicked();
// Otros datos
	void on_btnDatos_ExeJuego_clicked();
	void on_btnDatos_ExeJuego_clear_clicked();
	void on_btnDatos_ParametrosExe_clear_clicked();
	void on_btnDatos_SetupJuego_clicked();
	void on_btnDatos_SetupJuego_clear_clicked();
	void on_btnDatos_ParametrosSetup_clear_clicked();
	void on_btnDatos_PathCapturas_clicked();
	void on_btnDatos_PathCapturas_clear_clicked();
// Direcciones url
	void on_cbxUrl_url_editTextChanged(const QString &arg1);
	void on_txtUrl_descripcion_textChanged();
	void on_cbxUrl_url_activated(int index);
	void on_btnAddUrl_clicked();
	void on_btnUpdateUrl_clicked();
	void on_btnAbrirUrl_clicked();
	void on_btnDeleteUrl_clicked();
	void on_twDatosURL_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatosURL_itemDoubleClicked(QTreeWidgetItem *item, int column);
// Datos usuario
	void on_btnDatos_Usuario_clear_clicked();
// Datos archivos
	void on_txtDatosFiles_PathFile_textEdited(const QString &arg1);
	void on_txtDatosFiles_FileName_textEdited(const QString &arg1);
	void on_txtDatosFiles_Comentario_textChanged();
	void on_btnDatosFiles_PathFile_clicked();
	void on_btnDatosFiles_PathFile_clear_clicked();
	void on_btnAddFile_clicked();
	void on_btnUpdateFile_clicked();
	void on_btnDeleteFile_clicked();
	void on_twDatosFiles_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatosFiles_itemDoubleClicked(QTreeWidgetItem *item, int column);

};

#endif // GRLIDA_ADDEDIT_JUEGO_H
