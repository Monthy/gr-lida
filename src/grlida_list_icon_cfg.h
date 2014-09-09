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

#ifndef GRLIDA_LIST_ICON_CFG_H
#define GRLIDA_LIST_ICON_CFG_H

#include <QDialog>

#include "dbsql.h"
#include "funciones.h"
#include "grlida_delegate.h"
#include "editorwidget/codeeditor.h"

namespace Ui {
	class frmListIconCfg;
}

class frmListIconCfg : public QDialog
{
	Q_OBJECT

public:
	explicit frmListIconCfg(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat = 0, QString name_theme = "", QWidget *parent = 0);
	~frmListIconCfg();

private:
	Ui::frmListIconCfg *ui;

	Funciones *fGrl;
	dbSql *sql;
	GrlListViewDelegate *grl_lv_delegate;
	CodeEditor *editor;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stLwIconCfg lwConf, lwConftmp;
	QStandardItemModel *lv_model;

	unsigned int id_cat;
	QString textEditDef;
	QHash<int, stGrlCats> categoria;
	QHash<int, stGrlDatos> themes;
	QColor title_font_color, title_font_color_select;

	void cargarConfig();
	void setTheme();
	void cargarListaThemes();
	void cargarListaCategorias();
	void setColorBtn(QPushButton *btn, QColor color);

	void cargarIconConfig();
	void guardarIconConfig();
	void previewIconConfig();

private slots:
// Categorias y Themes
	void on_cbxCategorias_activated(int index);
	void on_cbxThemes_activated(int index);

// Iconos del treewidget
	void on_tw_icon_width_valueChanged(int arg1);
	void on_btn_tw_icon_width_def_clicked();
	void on_tw_icon_height_valueChanged(int arg1);
	void on_btn_tw_icon_height_def_clicked();

// Imagenes del picturefow
	void on_pf_img_width_valueChanged(int arg1);
	void on_btn_pf_img_width_def_clicked();
	void on_pf_img_height_valueChanged(int arg1);
	void on_btn_pf_img_height_def_clicked();

// Iconos de la lista en modo caratula
	void on_icon_width_valueChanged(int arg1);
	void on_btn_icon_width_def_clicked();
	void on_icon_height_valueChanged(int arg1);
	void on_btn_icon_height_def_clicked();
//--
	void on_btn_img_cover_top_clicked();
	void on_btn_img_cover_top_def_clicked();
	void on_img_cover_top_editingFinished();
	void on_btn_img_cover_top_select_clicked();
	void on_btn_img_cover_top_select_def_clicked();
	void on_img_cover_top_select_editingFinished();
	void on_img_cover_top_pos_x_valueChanged(int arg1);
	void on_btn_img_cover_top_pos_x_def_clicked();
	void on_img_cover_top_pos_y_valueChanged(int arg1);
	void on_btn_img_cover_top_pos_y_def_clicked();
//--
	void on_img_scaled_clicked(bool checked);
	void on_btn_img_scaled_def_clicked();
	void on_img_scale_w_valueChanged(int arg1);
	void on_btn_img_scale_w_def_clicked();
	void on_img_scale_h_valueChanged(int arg1);
	void on_btn_img_scale_h_def_clicked();
	void on_img_scale_pos_x_valueChanged(int arg1);
	void on_btn_img_scale_pos_x_def_clicked();
	void on_img_scale_pos_y_valueChanged(int arg1);
	void on_btn_img_scale_pos_y_def_clicked();
//--
	void on_tipo_emu_show_clicked(bool checked);
	void on_btn_tipo_emu_show_def_clicked();
	void on_tipo_emu_pos_x_valueChanged(int arg1);
	void on_btn_tipo_emu_pos_x_def_clicked();
	void on_tipo_emu_pos_y_valueChanged(int arg1);
	void on_btn_tipo_emu_pos_y_def_clicked();
//--
	void on_rating_show_clicked(bool checked);
	void on_btn_rating_show_def_clicked();
	void on_rating_vertical_clicked(bool checked);
	void on_btn_rating_vertical_def_clicked();
	void on_rating_pos_x_valueChanged(int arg1);
	void on_btn_rating_pos_x_def_clicked();
	void on_rating_pos_y_valueChanged(int arg1);
	void on_btn_rating_pos_y_def_clicked();
//--
	void on_title_bg_show_clicked(bool checked);
	void on_btn_title_bg_show_def_clicked();
	void on_btn_title_bg_clicked();
	void on_btn_title_bg_def_clicked();
	void on_title_bg_editingFinished();
	void on_btn_title_bg_select_clicked();
	void on_btn_title_bg_select_def_clicked();
	void on_title_bg_select_editingFinished();
	void on_title_bg_pos_x_valueChanged(int arg1);
	void on_btn_title_bg_pos_x_def_clicked();
	void on_title_bg_pos_y_valueChanged(int arg1);
	void on_btn_title_bg_pos_y_def_clicked();
//--
	void on_title_show_clicked(bool checked);
	void on_btn_title_show_def_clicked();
	void on_title_pos_x_valueChanged(int arg1);
	void on_btn_title_pos_x_def_clicked();
	void on_title_pos_y_valueChanged(int arg1);
	void on_btn_title_pos_y_def_clicked();
	void on_title_width_valueChanged(int arg1);
	void on_btn_title_width_def_clicked();
	void on_title_height_valueChanged(int arg1);
	void on_btn_title_height_def_clicked();
	void on_title_max_caracteres_valueChanged(int arg1);
	void on_btn_title_max_caracteres_def_clicked();
//--
	void on_title_font_activated(const QString &arg1);
	void on_btn_title_font_def_clicked();
	void on_title_font_size_valueChanged(int arg1);
	void on_btn_title_font_size_def_clicked();
	void on_btn_title_font_color_clicked();
	void on_btn_title_font_color_def_clicked();
	void on_btn_title_font_color_select_clicked();
	void on_btn_title_font_color_select_def_clicked();
	void on_title_font_bold_clicked(bool checked);
	void on_btn_title_font_bold_def_clicked();
	void on_title_font_italic_clicked(bool checked);
	void on_btn_title_font_italic_def_clicked();
	void on_title_font_underline_clicked(bool checked);
	void on_btn_title_font_underline_def_clicked();
//--
	void on_btnIconMode_clicked(bool checked);
	void on_btnSave_clicked();
//--
	void on_cbxListFiles_activated(int index);
	void on_btnSaveText_clicked();
//--
	void on_btnOk_clicked();

	void on_btnTextDef_clicked();
};

#endif // GRLIDA_LIST_ICON_CFG_H
