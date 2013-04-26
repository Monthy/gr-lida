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

#ifndef GRLIDA_UPDATE_H
#define GRLIDA_UPDATE_H

#include <QDialog>
#include <QDesktopServices>

#include "funciones.h"
#include "httpdownload.h"

namespace Ui {
class frmUpdate;
}

class frmUpdate : public QDialog
{
	Q_OBJECT
	
public:
	explicit frmUpdate(stGrlCfg m_cfg, QWidget *parent = 0);
	~frmUpdate();

	stGrlCfg getGrlCfg(){return grlCfg;}
	void checkUpdateGrl();

private:
	Ui::frmUpdate *ui;

	enum e_fin_descarga {
		CompruebaUpdates  = 0,
		CopiarArchivo     = 1,
		DescargaSiguiente = 2
	};

	struct stUpdates {
		QString tipo;
		QString archivo;
		QString url;
	};

	Funciones *fGrl;
	HttpDownload *httpdown;

	stGrlDir grlDir;
	stGrlCfg grlCfg;

	QString url_web, str_html_new, str_html_old, ver_list_svm;

	bool isDownload, isUpdates;
	int total_update, id_descarga, index_fin_descarga;
	QHash<int, stUpdates> grlUpdate;

	QTreeWidgetItem *twListUpdates;

	void cargarConfig();
	void comprobarActualizaciones();
	void listUpdateAddCat(QString etiqueta, QString icono);
	void listUpdateAddSubCat(QString etiqueta, QString icono, QString version_old, QString version_new, QString descipcion, QString tipo, QString filename, QString url, bool checked, bool select, bool oculto = false);
	void downloadFile(int id_down);

private slots:
	void on_btnOk_clicked();
	void on_btnUpdate_clicked();
	void on_btnCheckUpdate_clicked();

	void on_twUpdates_itemClicked(QTreeWidgetItem *item, int column);

	void statusBtnEnabled(bool estado);
	void statusFinished();

	void on_chkSoloUpdates_clicked(bool checked);
};

#endif // GRLIDA_UPDATE_H
