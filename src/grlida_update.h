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

#ifndef GRLIDA_UPDATE_H
#define GRLIDA_UPDATE_H

#include <QDialog>
#include <QDesktopServices>

#include "funciones.h"
#include "httpdownload.h"
#include "Qt7zip/qt7zip.h"


namespace Ui {
class frmUpdate;
}

class frmUpdate : public QDialog
{
	Q_OBJECT

public:
	explicit frmUpdate(QList<stUpdates> m_up_grl, QList<stUpdates> m_up_js, QList<stUpdates> m_up_st, stUpdates m_up_svm, stGrlCfg m_cfg, QWidget *parent = 0);
	~frmUpdate();

	stGrlCfg getGrlCfg() { return grlCfg;}

private:
	Ui::frmUpdate *ui;

	Funciones *fGrl;
	HttpDownload *httpdown;
	Qt7zip *z_file;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QFont m_font;
	QTreeWidgetItem *twItem;
	QString tpl_info_old;
	bool is_load_7zlib, z_is_open;

	enum col_TwUpdates {
		col_title      = 0,
		col_ver_old    = 1,
		col_version    = 2,
		col_info       = 3,
		col_info_item  = 4,
		col_tipo       = 5,
		col_file       = 6,
		col_url_method = 7,
		col_url        = 8,
	};

	struct stListFiles {
		QString archivo;
		QString tipo;
	};
	QList<stListFiles> listFiles;

	void cargarConfig();
	void setTheme();

	void twAddCat(QString etiqueta, QString info, QString icono);
	void twAddSubCat(stUpdates up, QString icono, QString url_method);
	bool extractFile(const QString file_in, const QString dir_out);

private slots:
	void statusBtnEnabled(bool estado);
	void statusFinished();

	void on_twUpdates_itemClicked(QTreeWidgetItem *item, int column);
	void on_btnUpdate_clicked();
	void on_btnClose_clicked();

};

#endif // GRLIDA_UPDATE_H
