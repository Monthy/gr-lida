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

#include <QDesktopWidget>
#include <QStyleFactory>
#include <QTextStream>

#include "grlida_config_inicial.h"
#include "grlida_buscar.h"

#include "ui_config_inicial.h"

frmConfigInicial::frmConfigInicial(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmConfigInicial)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home    = fGrl->dirApp();
	grlDir.Datos   = grlDir.Home +"datos/";
	grlDir.Idiomas = grlDir.Home +"idiomas/";

	cargarConfig();
	setTheme();

// Centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmConfigInicial::~frmConfigInicial()
{
	delete fGrl;
	delete ui;
}

void frmConfigInicial::cargarConfig()
{
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, ":/idiomas/"  , "img16/lng/");
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, grlDir.Idiomas, "img16/lng/", "", false);
	int idx = ui->cbxIdioma->findData(grlCfg.IdiomaSelect +".qm", Qt::UserRole, Qt::MatchContains);
	ui->cbxIdioma->setCurrentIndex(idx);
	emit on_cbxIdioma_activated(idx);

	id_key_dbx = -1;
	new_dbx_default = false;

	dbx_list.clear();
	dbx_list = fGrl->cargaListaDatos(grlDir.Datos +"dbx_list.txt", 6, "|");
	const int listSize = dbx_list.size();
	if (listSize > 0)
	{
		id_key_dbx = 0;
		for (int i = 0; i < listSize; ++i)
		{
			if (grlCfg.DOSBoxDefault == dbx_list.at(i).key)
				id_key_dbx = i;
		}
		ui->txtDbxPath->setText(dbx_list.at(id_key_dbx).extra);
	} else
		new_dbx_default = true;

	ui->chkDOSBoxDisp->setChecked(grlCfg.DOSBoxDisp);
	ui->chkScummVMDisp->setChecked(grlCfg.ScummVMDisp);
	ui->txtSvmPath->setText(grlCfg.DirScummVM);

	ui->chkVDMSoundDisp->setChecked(grlCfg.VDMSoundDisp);
	ui->chkShowNext->setChecked(grlCfg.Primeravez);

	#ifdef Q_OS_WIN32
		ui->chkVDMSoundDisp->setEnabled(true);
	#else
		ui->chkVDMSoundDisp->setEnabled(false);
	#endif
}

void frmConfigInicial::setTheme()
{
	setWindowIcon(QIcon(":/img32/gr-lida.png"));

	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
	ui->btnDbxPath->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnDbxPath_find->setIcon(QIcon(fGrl->theme() +"img16/zoom.png"));
	ui->btnDbxPath_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));
	ui->btnSvmPath->setIcon(QIcon(fGrl->theme() +"img16/carpeta_1.png"));
	ui->btnSvmPath_find->setIcon(QIcon(fGrl->theme() +"img16/zoom.png"));
	ui->btnSvmPath_clear->setIcon(QIcon(fGrl->theme() +"img16/limpiar.png"));

	QString font_usar = grlCfg.font_usar ? "*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}" : "";
	this->setStyleSheet(fGrl->myStyleSheet("StyleSheet.qss") + font_usar);
	this->setStyle(QStyleFactory::create(grlCfg.Style));

	if (grlCfg.StylePalette)
		this->setPalette(QApplication::style()->standardPalette());
	else
		this->setPalette(QApplication::palette());
}

void frmConfigInicial::on_cbxIdioma_activated(int index)
{
	if (index > -1)
	{
		QString idioma = ui->cbxIdioma->itemData(index).toString();
		translator.load(idioma);

		idioma.remove(grlDir.Idiomas +"gr-lida_");
		idioma.remove(":/idiomas/gr-lida_");
		idioma.remove(".qm");

		grlCfg.IdiomaSelect = idioma;
	} else {
		grlCfg.IdiomaSelect = "es_ES";
		translator.load(":/idiomas/gr-lida_"+ grlCfg.IdiomaSelect +".qm");
	}

	fGrl->setIdioma(grlCfg.IdiomaSelect);

	qApp->installTranslator(&translator);
	ui->retranslateUi(this);
}

void frmConfigInicial::on_btnDbxPath_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona el ejecutable del DOSBox"), grlCfg.Dbx_path, "", "DOSBox (dosbox.exe dosbox);;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		grlCfg.Dbx_path = fGrl->setDirRelative(f_info.Path);
		ui->txtDbxPath->setText(fGrl->setDirRelative(f_info.FilePath));
	}
}

void frmConfigInicial::on_btnDbxPath_find_clicked()
{
	frmBuscar *buscar = new frmBuscar("DOSBox", ".exe", ui->txtDbxPath->text(), fGrl->theme(), this);
	if (buscar->exec() == QDialog::Accepted)
		ui->txtDbxPath->setText(fGrl->setDirRelative(buscar->getSelect()));
	delete buscar;
}

void frmConfigInicial::on_btnDbxPath_clear_clicked()
{
	ui->txtDbxPath->clear();
}

void frmConfigInicial::on_btnSvmPath_clicked()
{
	stFileInfo f_info = fGrl->getInfoFile(fGrl->ventanaAbrirArchivos(this, tr("Selecciona el ejecutable del ScummVM"), grlCfg.Svm_path, "", "ScummVM (scummvm.exe scummvm);;"+ tr("Todos los archivos") +" (*)"));

	if (f_info.Exists)
	{
		grlCfg.Svm_path = fGrl->setDirRelative(f_info.Path);
		ui->txtSvmPath->setText(fGrl->setDirRelative(f_info.FilePath));
	}
}

void frmConfigInicial::on_btnSvmPath_find_clicked()
{
	frmBuscar *buscar = new frmBuscar("ScummVM", ".exe", ui->txtSvmPath->text(), fGrl->theme(), this);
	if (buscar->exec() == QDialog::Accepted)
		ui->txtSvmPath->setText(fGrl->setDirRelative(buscar->getSelect()));
	delete buscar;
}

void frmConfigInicial::on_btnSvmPath_clear_clicked()
{
	ui->txtSvmPath->clear();
}

void frmConfigInicial::on_btnOk_clicked()
{
	if (new_dbx_default)
	{
		stGrlDatos dbx_datos;
		dbx_datos.titulo  = "DOSBox";
		dbx_datos.icono   = "cat/dosbox.png";
		dbx_datos.extra   = ui->txtDbxPath->text();
		dbx_datos.issvn   = "no";
		dbx_datos.version = "0.74-2";
		dbx_datos.key     = "dosbox";

		dbx_list << dbx_datos;
	} else
		dbx_list[id_key_dbx].extra = ui->txtDbxPath->text();

	QFile file_out;
	file_out.setFileName(grlDir.Datos +"dbx_list.txt");
	fGrl->comprobarDirectorio(grlDir.Datos);

	if (file_out.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");
		const int listSize = dbx_list.size();
		if (listSize > 0)
		{
			id_key_dbx = 0;
			for (int i = 0; i < listSize; ++i)
				out << dbx_list.at(i).titulo << "|" << dbx_list.at(i).key << "|" << dbx_list.at(i).extra << "|" << dbx_list.at(i).icono << "|" << dbx_list.at(i).version << "|" << dbx_list.at(i).issvn << endl;
		}
		out.flush();
	}
	file_out.close();

	grlCfg.DOSBoxDisp   = ui->chkDOSBoxDisp->isChecked();
	grlCfg.ScummVMDisp  = ui->chkScummVMDisp->isChecked();
	grlCfg.DirScummVM   = ui->txtSvmPath->text();
	grlCfg.VDMSoundDisp = ui->chkVDMSoundDisp->isChecked();
	grlCfg.Primeravez   = ui->chkShowNext->isChecked();

	QDialog::accept();
}
