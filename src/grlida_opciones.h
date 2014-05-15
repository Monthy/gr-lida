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

#ifndef GRLIDA_OPCIONES_H
#define GRLIDA_OPCIONES_H

#include <QDialog>
#include <QTranslator>

#include "funciones.h"
#include "dbsql.h"
#include "qcheckcombobox.h"

namespace Ui {
	class frmOpciones;
}

class frmOpciones : public QDialog
{
	Q_OBJECT

public:
	explicit frmOpciones(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent = 0);
	~frmOpciones();

	stGrlCfg getConfig(){return grlCfg;}
	QString getIdioma(){return idioma;}

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmOpciones *ui;

	Funciones *fGrl;
	dbSql *sql;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QCheckComboBox *cbxCat_EmuShow;
	QHash<QString, stGrlDatos> emu_list;

	QTranslator translator;
	QString idioma, IdiomaSelect;

	void createWidgets();
	void cargarConfig();
	void guardarConfig();
	void setTheme();
	void cargarListaThemes();

	void enabledCatUpdate(QString texto, int col);
	void addEditTwMnuNav(bool isNew);
	void enabledMnuNavUpdate(QString texto, int col);
	void addEditDatosTwLista(bool isNew);
	void enabledDatosUpdate(QString texto, int col);
	void guardarDatosTwLista();
	void setColorBtn(QColor color);

private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_lwOpciones_currentRowChanged(int currentRow);
// General
	void on_cbxIdioma_activated(int index);
	void on_chkDOSBoxDisp_toggled(bool checked);
	void on_btnDbxPath_clicked();
	void on_btnDbxPath_find_clicked();
	void on_btnDbxPath_clear_clicked();
	void on_chkScummVMDisp_toggled(bool checked);
	void on_btnSvmPath_clicked();
	void on_btnSvmPath_find_clicked();
	void on_btnSvmPath_clear_clicked();
	void on_btnDirBaseGames_clicked();
	void on_btnDirBaseGames_clear_clicked();
	void on_btnOpenUrl_clicked();
	void on_btnInfoFormatoFecha_clicked();
// Categoría
	void on_txtCat_Nombre_editingFinished();
	void on_txtCat_Nombre_textEdited(const QString &arg1);
	void on_txtCat_Tabla_textEdited(const QString &arg1);
	void on_cbxCat_Img_activated(int index);
	void cbxCat_EmuShow_activated(int index);
	void on_btnCatAdd_clicked();
	void on_btnCatUpdate_clicked();
	void on_btnCatSubir_clicked();
	void on_btnCatBajar_clicked();
	void on_btnCatEditTheme_clicked();
	void on_btnCatDelete_clicked();
	void on_twCategorias_itemClicked(QTreeWidgetItem *item, int column);
	void on_twCategorias_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// MenuNav
	void on_txtMnuNav_Nombre_textEdited(const QString &arg1);
	void on_chkMnuNav_Expanded_clicked(bool checked);
	void on_chkMnuNav_Show_clicked(bool checked);
	void on_cbxMnuNav_ColName_activated(int index);
	void on_cbxMnuNav_Img_activated(int index);
	void on_cbxMnuNav_Archivo_activated(int index);
	void on_cbxMnuNav_ColValue_editTextChanged(const QString &arg1);
	void on_cbxMnuNav_SqlQuery_editTextChanged(const QString &arg1);
	void on_btnMnuNavAdd_clicked();
	void on_btnMnuNavUpdate_clicked();
	void on_btnMnuNavSubir_clicked();
	void on_btnMnuNavBajar_clicked();
	void on_btnMnuNavDefecto_clicked();
	void on_btnMnuNavDelete_clicked();
	void on_twMnuNav_itemClicked(QTreeWidgetItem *item, int column);
	void on_twMnuNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Datos
	void on_cbxDat_Archivo_activated(int index);
	void on_txtDat_Nombre_textEdited(const QString &arg1);
	void on_cbxDat_ImgCmpt_activated(int index);
	void on_txtDat_Dato_textEdited(const QString &arg1);
	void on_cbxDat_Img_activated(int index);
	void on_txtDat_Extra_textEdited(const QString &arg1);
	void on_btnDatPath_clicked();
	void on_btnDatPath_clear_clicked();
	void on_btnDatAdd_clicked();
	void on_btnDatUpdate_clicked();
	void on_btnDatSubir_clicked();
	void on_btnDatBajar_clicked();
	void on_btnDatAddSmile_clicked();
	void on_btnDatDelete_clicked();
	void on_twDatos_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Base de Datos
	void on_btnDirDB_clicked();
	void on_btnDirDB_clear_clicked();
// Style themes
	void on_cbxStyle_activated(int index);
	void on_chkStylePalette_clicked(bool checked);
	void on_chkUsarTipoFuente_clicked(bool checked);
	void on_cbxFuenteTipo_activated(const QString &arg1);
	void on_sbxFontSize_valueChanged(const QString &arg1);
	void on_cbxPicFlowType_activated(int index);
	void on_btnPicFlowBgColor_clicked();
	void on_btnEditTheme_clicked();
	void on_twThemes_itemClicked(QTreeWidgetItem *item, int column);
	void on_twThemes_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// --
	void on_btnExtVideoAdd_clicked();
	void on_btnExtVideoDelete_clicked();
	void on_btnExtVideoDefecto_clicked();
	void on_btnExtMusicAdd_clicked();
	void on_btnExtMusicDelete_clicked();
	void on_btnExtMusicDefecto_clicked();
	void on_btnExtImageAdd_clicked();
	void on_btnExtImageDelete_clicked();
	void on_btnExtImageDefecto_clicked();

};

#endif // GRLIDA_OPCIONES_H
