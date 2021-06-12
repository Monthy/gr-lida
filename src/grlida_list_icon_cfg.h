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

#ifndef GRLIDA_LIST_ICON_CFG_H
#define GRLIDA_LIST_ICON_CFG_H

#include <QDialog>
#include <QTextEdit>

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
	explicit frmListIconCfg(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat = 0, QWidget *parent = 0);
	~frmListIconCfg();

protected:
	void closeEvent(QCloseEvent *event);

private:
	Ui::frmListIconCfg *ui;

	Funciones *fGrl;
	dbSql *sql;
	CodeEditor *editor;

	GrlTreeViewModel *data_model;
	GrlTreeViewDelegate *grl_tv_delegate;
	GrlListViewDelegate *grl_lv_delegate;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stLwIconCfg lwIconCfg, lwIconCfgtmp;

	QModelIndex select_row;
	int id_cat, id_theme;
	QString textEditDef, stTheme, stItem;
	QHash<int, stGrlCats> categoria;
	QHash<int, stGrlDatos> themes;
	QList<stGrlDatos> list_emu;
	QHash<QString, int> list_items;

	void cargarConfig();
	void setTheme();
	void setColorBtn(QPushButton *btn, QColor color);

	void cargarListaCategorias();
	void cargarListaThemes();
	void cargarListaItems();

	void cargarIconConfig();
	void guardarIconConfig();
	void previewIconConfig();

private slots:
// Categorias y Themes
	void on_cbxThemes_activated(int index);
	void on_cbxCategorias_activated(int index);
	void on_cbxTipoEmu_activated(int index);

// Iconos del treewidget
	void on_tw_icon_width_valueChanged(int arg1);
	void on_btn_tw_icon_width_def_clicked();
	void on_tw_icon_height_valueChanged(int arg1);
	void on_btn_tw_icon_height_def_clicked();
	void on_tw_alternating_row_colors_toggled(bool checked);
	void on_btn_tw_alternating_row_colors_def_clicked();

// Imágenes del picturefow
	void on_pf_img_width_valueChanged(int arg1);
	void on_btn_pf_img_width_def_clicked();
	void on_pf_img_height_valueChanged(int arg1);
	void on_btn_pf_img_height_def_clicked();
	void on_pf_img_fixsize_toggled(bool checked);
	void on_btn_pf_img_fixsize_def_clicked();

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
	void on_btn_img_border_color_clicked();
	void on_btn_img_border_color_def_clicked();
	void on_btn_img_border_color_select_clicked();
	void on_btn_img_border_color_select_def_clicked();
	void on_slider_img_border_color_alpha_valueChanged(int value);
	void on_img_cover_top_bg_clicked(bool checked);
	void on_img_cover_top_pos_x_valueChanged(int arg1);
	void on_btn_img_cover_top_pos_x_def_clicked();
	void on_img_cover_top_pos_y_valueChanged(int arg1);
	void on_btn_img_cover_top_pos_y_def_clicked();
	void on_img_cover_top_zindex_clicked(bool checked);
	void on_btn_img_cover_top_zindex_def_clicked();
//--
	void on_img_scaled_clicked(bool checked);
	void on_btn_img_scaled_def_clicked();
	void on_img_scale_w_valueChanged(int arg1);
	void on_btn_img_scale_w_def_clicked();
	void on_img_scale_h_valueChanged(int arg1);
	void on_btn_img_scale_h_def_clicked();
	void on_img_pos_x_valueChanged(int arg1);
	void on_btn_img_pos_x_def_clicked();
	void on_img_pos_y_valueChanged(int arg1);
	void on_btn_img_pos_y_def_clicked();
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

	void on_title_font_pos_activated(int index);
	void on_btn_title_font_pos_def_clicked();

	void on_btn_title_font_color_clicked();
	void on_btn_title_font_color_def_clicked();
	void on_btn_title_font_color_select_clicked();
	void on_btn_title_font_color_select_def_clicked();
	void on_title_font_wordwrap_clicked(bool checked);
	void on_btn_title_font_wordwrap_def_clicked();
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
	void on_btnTextDef_clicked();
	void on_btnSaveText_clicked();
//--
	void on_btnOk_clicked();
	void on_tvJuegos_clicked(const QModelIndex &index);
	void on_lvJuegos_clicked(const QModelIndex &index);

};

#endif // GRLIDA_LIST_ICON_CFG_H
