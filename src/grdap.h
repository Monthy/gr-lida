/*
 *
 * GR-dap by Monthy
 *
 * This file is part of GR-dap is Dial-A-Protection and
 * GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
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

#ifndef GRDAP_H
#define GRDAP_H

#include <QMainWindow>
#include <QColorDialog>
#include <QDropEvent>
#include <QDragEnterEvent>

#include "grdapview.h"
#include "funciones.h"
#include "qtzip.h"

namespace Ui {
	class GrDap;
}

class GrDap : public QMainWindow
{
	Q_OBJECT

public:
	explicit GrDap(QWidget *parent = 0);
	~GrDap();

	void cargarArchivo(QString filename);
	void cargarImagen(QString filename);
	void cargarImagenes(QStringList images, int index, bool directo = true);

protected:
	void closeEvent(QCloseEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);

private:
	struct stGrdCfg {
	// MainState
		bool       main_fullscreen;
		bool       main_maximized;
		QByteArray main_geometry;
	// OpcVer
		bool show_info;
	};

	Ui::GrDap *ui;
	Funciones *fGrl;
	GrDapView *grdapView;
	QtZip zip;

	stGrdCfg grdCfg;
	stGrlDir grlDir;
	QHash<QString, QString> config;
	QStringList img_ext, list_zip, list_titulos;
	QString stDir, texto_pagina;
	QColor visor_color;
	bool isInicio, isZip, visor_show;
	int num_img_pagina;

	void cargarConfig();
	void guardarConfig();
	QPixmap loadPixmap(QString filename);
	QBitmap loadBitmap(QString filename);
	void setCbxNextIndex(QComboBox *cbx);
	void setCbxBackIndex(QComboBox *cbx);
	void setColorBtn(QColor color);
	void setDefecto();
	void getInfoImg();

private slots:
//	void openfileDragDrop(QString filename);
	void on_btn_cerrar_info_clicked();
	void on_btn_salir_clicked();
	void on_btn_abrir_clicked();
	void on_slider_zoom_valueChanged(int value);
	void on_btn_zoom_in_clicked();
	void on_btn_zoom_out_clicked();
	void on_btn_zoom_normal_clicked();
	void on_btn_fullscreen_toggled(bool checked);
	void on_btn_info_toggled(bool checked);
	void on_btn_acercade_triggered();
// Ruletas de la A a la D
	void on_slider_rotar_a_valueChanged(int value);
	void on_slider_rotar_b_valueChanged(int value);
	void on_slider_rotar_c_valueChanged(int value);
	void on_slider_rotar_d_valueChanged(int value);
	void on_btn_rotar_a_clicked();
	void on_btn_rotar_b_clicked();
	void on_btn_rotar_c_clicked();
	void on_btn_rotar_d_clicked();
// --
// Visor de cartas: Alone in the Dark 2
	void on_cbx_carta_top_activated(int index);
	void on_cbx_carta_bot_activated(int index);
// --
// Visor de imagenes
	void on_cbx_img_pagina_activated(int index);
	void on_btn_img_back_clicked();
	void on_btn_img_next_clicked();
	void on_slider_rotar_img_valueChanged(int value);
	void on_btn_rotar_img_clicked();
// --
// Visor de texto oculto: Indiana Jones y la Última Cruzada
	void on_btn_visor_clicked(bool checked);
	void on_btn_visor_color_clicked();
	void on_cbx_visor_mode_activated(int index);
	void visorColorChanged(QColor color);
// --

};

#endif // GRDAP_H
