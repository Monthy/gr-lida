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

#ifndef GRLIDA_OPCIONES_H
#define GRLIDA_OPCIONES_H

#include <QDialog>
#include <QTranslator>

#include "funciones.h"
#include "dbsql.h"
#include "qcheckcombobox.h"
#include "grlida_picflow.h"

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
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::frmOpciones *ui;

	Funciones *fGrl;
	dbSql *sql;
	GrlPicFlow *grl_picflow;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QCheckComboBox *cbxCatEmuShow;
	QHash<QString, stGrlDatos> emu_list;

	QRegExp regexp;
	QTranslator translator;
	QString idioma, IdiomaSelect, ThemeSelect, tpl_info_formato_fecha_old;
	QStringList list_key_sequence;
//	QStringList list_pos;

	void createWidgets();
	void cargarConfig();
	void guardarConfig();
	void setTheme();
	void cargarListaThemes();

	void setColorBtn(QPushButton *btnColor, QColor color);
	void enabledButton(QTreeWidget *twList, QPushButton *btn, QString texto, int col);

	void addEditTwDOSBox(bool isAdd);
	void addEditTwEmus(bool isAdd);
	void addEditTwCategorias(bool isAdd);
	void textEditedTxtDbxDato();
	void addEditTwMnuNav(bool isAdd);
	void addEditTwMnuShortCut(bool isAdd);
	void addEditDatosTwLista(bool isAdd);
	void guardarDatosTwLista();


private slots:
	void on_btnOk_clicked();
	void on_btnCancel_clicked();
	void on_lwOpciones_currentRowChanged(int currentRow);
// General
	void on_cbxIdioma_activated(int index);
	void on_btnSowEmus_clicked();
	void on_btnDirBaseGames_clicked();
	void on_btnDirBaseGames_clear_clicked();
	void on_btnOpenUrl_clicked();
	void on_txtFormatoFecha_textEdited(const QString &arg1);
	void on_btnInfoFormatoFecha_clicked();
// Emuladores
//	void on_chkScummVMDisp_toggled(bool checked);
	void on_btnSvmPath_clicked();
	void on_btnSvmPath_find_clicked();
	void on_btnSvmPath_clear_clicked();
//	void on_chkDOSBoxDisp_toggled(bool checked);
	void on_txtDbxTitulo_textEdited(const QString &arg1);
	void on_chkDbxSVN_toggled(bool checked);
	void on_cbxDbxVersion_activated(int index);
//	void on_cbxDbxImg_activated(int index);
	void on_btnDbxPath_clicked();
	void on_btnDbxPath_find_clicked();
	void on_btnDbxPath_clear_clicked();
	void on_btnDbxDefault_clicked();
	void on_btnDbxNew_clicked();
	void on_btnDbxAdd_clicked();
	void on_btnDbxEdit_clicked();
	void on_btnDbxUpdate_clicked();
	void on_btnDbxSubir_clicked();
	void on_btnDbxBajar_clicked();
	void on_btnDbxDelete_clicked();
	void on_btnDbxCancel_clicked();
// Otros emuladores
	void on_txtEmuTitulo_textEdited(const QString &arg1);
	void on_txtEmuDato_textEdited(const QString &arg1);
	void on_txtEmuPath_textEdited(const QString &arg1);
	void on_cbxEmuImg_activated(int index);
	void on_btnEmuPath_clicked();
	void on_btnEmuPath_clear_clicked();
	void on_btnEmuAdd_clicked();
	void on_btnEmuUpdate_clicked();
	void on_btnEmuSubir_clicked();
	void on_btnEmuBajar_clicked();
	void on_btnEmuDelete_clicked();
	void on_twEmus_itemClicked(QTreeWidgetItem *item, int column);
	void on_twEmus_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Categoría
	void on_txtCatNombre_editingFinished();
	void on_txtCatNombre_textEdited(const QString &arg1);
	void on_txtCatTabla_textEdited(const QString &arg1);
	void on_cbxCatImg_activated(int index);
	void cbxCatEmuShow_activated(int index);
	void on_btnCatAdd_clicked();
	void on_btnCatUpdate_clicked();
	void on_btnCatSubir_clicked();
	void on_btnCatBajar_clicked();
	void on_btnCatEditTheme_clicked();
	void on_btnCatDelete_clicked();
	void on_twCategorias_itemClicked(QTreeWidgetItem *item, int column);
	void on_twCategorias_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// MenuNav
	void on_txtMnuNavNombre_textEdited(const QString &arg1);
	void on_chkMnuNavExpanded_clicked(bool checked);
	void on_chkMnuNavShow_clicked(bool checked);
	void on_cbxMnuNavColName_activated(int index);
	void on_cbxMnuNavImg_activated(int index);
	void on_cbxMnuNavArchivo_activated(int index);
	void on_cbxMnuNavColValue_editTextChanged(const QString &arg1);
	void on_cbxMnuNavSqlQuery_editTextChanged(const QString &arg1);
	void on_btnMnuNavAdd_clicked();
	void on_btnMnuNavUpdate_clicked();
	void on_btnMnuNavSubir_clicked();
	void on_btnMnuNavBajar_clicked();
	void on_btnMnuNavDefecto_clicked();
	void on_btnMnuNavDelete_clicked();
	void on_twMnuNav_itemClicked(QTreeWidgetItem *item, int column);
	void on_twMnuNav_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// MnuShortCut
	void on_txtMnuShortCutTitulo_textEdited(const QString &arg1);
	void on_btnShortcutKeySequenceReset_clicked();
	void on_chkMnuShortCutShow_toggled(bool checked);
//	void on_chkMnuShortCutSeparator_toggled(bool checked);
	void on_cbxMnuShortCutImg_activated(int index);
	void on_cbxMnuShortCutSqlQuery_editTextChanged(const QString &arg1);
	void on_txtShortcutKeySequence_textChanged(const QString &arg1);
	void on_btnMnuShortCutAdd_clicked();
	void on_btnMnuShortCutAddSeparator_clicked();
	void on_btnMnuShortCutUpdate_clicked();
	void on_btnMnuShortCutSubir_clicked();
	void on_btnMnuShortCutBajar_clicked();
	void on_btnMnuShortCutDefecto_clicked();
	void on_btnMnuShortCutDelete_clicked();
	void on_twMnuShortCut_itemClicked(QTreeWidgetItem *item, int column);
	void on_twMnuShortCut_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Datos
	void on_cbxDatArchivo_activated(int index);
	void on_txtDatNombre_textEdited(const QString &arg1);
	void on_cbxDatImgCmpt_activated(int index);
	void on_txtDatDato_textEdited(const QString &arg1);
	void on_cbxDatImg_activated(int index);
	void on_txtDatExtra_textEdited(const QString &arg1);
	void on_btnDatAdd_clicked();
	void on_btnDatUpdate_clicked();
	void on_btnDatSubir_clicked();
	void on_btnDatBajar_clicked();
	void on_btnDatAddSmile_clicked();
	void on_btnDatDelete_clicked();
	void on_twDatos_itemClicked(QTreeWidgetItem *item, int column);
	void on_twDatos_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// Base de datos
	void on_btnDirDB_clicked();
	void on_btnDirDB_clear_clicked();
// Style themes
	void on_cbxStyle_activated(int index);
	void on_cbxIconFav_activated(int index);
	void on_chkStylePalette_clicked(bool checked);
	void on_chkUsarTipoFuente_clicked(bool checked);
	void on_cbxFuenteTipo_activated(const QString &arg1);
	void on_cbxFontSize_valueChanged(const QString &arg1);
	void on_btnEditTheme_clicked();
	void on_twThemes_itemClicked(QTreeWidgetItem *item, int column);
	void on_twThemes_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
// PictureFlow
	void on_cbxPicFlowType_activated(int index);
	void on_btnPicFlowBgColor_clicked();
	void on_cbxPicFlowFontFamily_activated(const QString &arg1);
	void on_spinBox_PicFlowFontSize_valueChanged(int arg1);
	void on_chkPicFlowFontBold_toggled(bool checked);
	void on_chkPicFlowFontItalic_toggled(bool checked);
	void on_btnPicFlowFontColor_clicked();

	void on_chkPicFlowShowTitle_toggled(bool checked);
	void on_cbxPicFlowTitlePos_activated(int index);
	void on_spinBox_PicFlowTitleMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowTitleMarginY_valueChanged(int arg1);

	void on_chkPicFlowShowTitleIcon_toggled(bool checked);
	void on_cbxPicFlowTitleIconPos_activated(int index);
	void on_spinBox_PicFlowTitleIconMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowTitleIconMarginY_valueChanged(int arg1);

	void on_chkPicFlowShowCaption_toggled(bool checked);
	void on_cbxPicFlowCaptionPos_activated(int index);
	void on_spinBox_PicFlowCaptionMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowCaptionMarginY_valueChanged(int arg1);

	void on_chkPicFlowShowNumber_toggled(bool checked);
	void on_cbxPicFlowNumberPos_activated(int index);
	void on_spinBox_PicFlowNumberMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowNumberMarginY_valueChanged(int arg1);
	void on_txtPicFlowNumberFormat_textChanged(const QString &arg1);

	void on_chkPicFlowShowIconExtra_toggled(bool checked);
	void on_cbxPicFlowIconExtraPos_activated(int index);
	void on_spinBox_PicFlowIconExtraMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowIconExtraMarginY_valueChanged(int arg1);
	void on_cbxPicFlowIconExtraUse_activated(int index);
	void on_cbxPicFlowIconExtraImage_activated(int index);

	void on_spinBox_PicFlowMarginX_valueChanged(int arg1);
	void on_spinBox_PicFlowMarginY_valueChanged(int arg1);

	void on_chkPicFlowShowTriangle_toggled(bool checked);
// --
	void on_btn_regenerar_thumbs_clicked();
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
