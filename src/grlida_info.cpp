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

#include "grlida_info.h"
#include "ui_informacion.h"

frmInfo::frmInfo(dbSql *m_sql, stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmInfo)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	sql    = m_sql;
	grlCfg = m_cfg;

	grlDir.Home  = fGrl->dirApp();
	grlDir.Datos = grlDir.Home +"datos/";

	cargarConfig();
	setTheme();
	cargarListaCategorias();

// Centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmInfo::~frmInfo()
{
	delete fGrl;
	delete ui;
}

void frmInfo::cargarConfig()
{
// Configuración del twInfo
	ui->twInfo->headerItem()->setIcon(0, QIcon(fGrl->theme() +"img16/tag.png"));
	ui->twInfo->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
	ui->twInfo->header()->setSectionsMovable(false);
	ui->twInfo->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	ui->twInfo->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
#else
	ui->twInfo->header()->setMovable(false);
	ui->twInfo->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twInfo->header()->setResizeMode(1, QHeaderView::ResizeToContents);
#endif
	ui->twInfo->setColumnWidth(1, 60);

	ui->lb_info_grlida->setText(tr("Versión") +" GR-lida");
	ui->lb_info_grlida_version->setText(fGrl->versionGrl());

	ui->lb_info_qt->setText(tr("Versión") +" Qt");
	ui->lb_info_qt_version->setText(qVersion());

	ui->lb_info_dosbox->setText("DOSBox "+ tr("Soportado"));
	ui->lb_info_dosbox_version->setText(fGrl->versionDbx());

	ui->lb_info_scummvm->setText("ScummVM "+ tr("Soportado"));
	ui->lb_info_scummvm_version->setText(fGrl->versionSvm());

	ui->lb_info_vdmsound->setText("VDMSound "+ tr("Soportado"));
	ui->lb_info_vdmsound_version->setText(fGrl->versionVdms());
}

void frmInfo::setTheme()
{
	setWindowIcon(QIcon(fGrl->theme() +"img16/informacion.png"));

	ui->lb_info_ico->setPixmap(QPixmap(fGrl->theme() +"img16/informacion.png"));
	ui->lb_info_grlida_ico->setPixmap(QPixmap(fGrl->theme() +"img16/gr-lida.png"));
	ui->lb_info_qt_ico->setPixmap(QPixmap(fGrl->theme() +"img16/qt.png"));
	ui->lb_info_dosbox_ico->setPixmap(QPixmap(fGrl->theme() +"img16/cat/dosbox.png"));
	ui->lb_info_scummvm_ico->setPixmap(QPixmap(fGrl->theme() +"img16/cat/scummvm.png"));
	ui->lb_info_vdmsound_ico->setPixmap(QPixmap(fGrl->theme() +"img16/cat/vdmsound.png"));
	ui->btnOk->setIcon(QIcon(fGrl->theme() +"img16/aplicar.png"));
}

// Carga las categorías de la base de datos.
void frmInfo::cargarListaCategorias()
{
	QList<stGrlCats> list_cat = sql->getCategorias();

	categoria.clear();
	ui->cbxCategorias->clear();
	const int listCatSize = list_cat.size();
	for (int i = 0; i < listCatSize; ++i)
	{
		categoria.insert(list_cat.at(i).id.toInt(), list_cat.at(i));

		QString titulo_cat = QString("%1 (%2)").arg(list_cat.at(i).titulo).arg(list_cat.at(i).total);

		if (QFile::exists(fGrl->theme() +"img16/"+ list_cat.at(i).img))
			ui->cbxCategorias->addItem(QIcon(fGrl->theme() +"img16/"+ list_cat.at(i).img), titulo_cat, list_cat.at(i).id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img16/sinimg.png"), titulo_cat, list_cat.at(i).id);
	}

	int id_index = ui->cbxCategorias->findData(id_cat);
	if (id_index < 0)
		id_index = 0;

	ui->cbxCategorias->setCurrentIndex(id_index);
	emit on_cbxCategorias_activated(id_index);
}

void frmInfo::menuNavAddCat(QString etiqueta, QString icono, QString sql_query, bool m_expanded, bool m_show_total)
{
	QFont m_font;
	int total = 0;
	if (m_show_total)
		total = sql->getCount(categoria[id_cat].tabla, sql_query);

	m_font.setBold((total > 0) ? true : false);

	twListInfo = new QTreeWidgetItem;//(ui->twInfo)
	twListInfo->setIcon(0, QIcon(icono));
	twListInfo->setFont(1, m_font);
	twListInfo->setTextAlignment(1, Qt::AlignCenter);
	twListInfo->setText(0, etiqueta);
	twListInfo->setText(1, m_show_total ? fGrl->intToStr(total) : "");
	twListInfo->setText(2, sql_query);
	twListInfo->setExpanded(m_expanded);
	ui->twInfo->addTopLevelItem(twListInfo);
}

void frmInfo::menuNavAddSubCat(QString etiqueta, QString icono, QString sql_query, QString sql_col)
{
	QFont m_font;
	int total = 0;
	total = sql->getCount(categoria[id_cat].tabla, sql_query, sql_col, etiqueta);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem;//(twListInfo)

	if (QFile::exists(icono))
		sub_cat->setIcon(0, QIcon(icono));
	else
		sub_cat->setIcon(0, QIcon(fGrl->theme() +"img16/sinimg.png"));

	m_font.setBold((total > 0) ? true : false);

	sub_cat->setFont(1, m_font);
	sub_cat->setTextAlignment(1, Qt::AlignCenter);
	sub_cat->setText(0, etiqueta);
	sub_cat->setText(1, fGrl->intToStr(total));
	sub_cat->setText(2, sql_query);
	twListInfo->addChild(sub_cat);
}

void frmInfo::crearMenuNav()
{
	ui->twInfo->clear();

	menuNavAddCat(tr("Todos")     , fGrl->theme() +"img16/basedatos.png"    , ""                     , false, true);
	menuNavAddCat(tr("Favoritos") , fGrl->theme() +"img16/"+ grlCfg.IconoFav, "WHERE favorito='true'", false);
	menuNavAddCat(tr("Originales"), fGrl->theme() +"img16/original.png"     , "WHERE original='true'", false);

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);

	menuNavAddCat(tr("Tipo emulador"), fGrl->theme() +"img16/tag.png", "", true, false);
	if (lista.isEmpty() || lista.contains("all") || lista.contains("dosbox"))
		menuNavAddSubCat("DOSBox", fGrl->theme() +"img16/cat/dosbox.png", "WHERE tipo_emu='dosbox'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("scummvm"))
		menuNavAddSubCat("ScummVM", fGrl->theme() +"img16/cat/scummvm.png", "WHERE tipo_emu='scummvm'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound"))
		menuNavAddSubCat("VDMSound", fGrl->theme() +"img16/cat/vdmsound.png", "WHERE tipo_emu='vdmsound'");
	if (lista.isEmpty() || lista.contains("all") || lista.contains("datos"))
		menuNavAddSubCat("Datos", fGrl->theme() +"img16/datos_3.png", "WHERE tipo_emu='datos'");

	QList<stGrlDatos> list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	const int listSize = list_emu.size();
	for (int i = 0; i < listSize; ++i)
	{
		if (lista.isEmpty() || lista.contains("all") || lista.contains(list_emu.at(i).key))
		{
			menuNavAddSubCat(list_emu.at(i).titulo, fGrl->theme() +"img16/"+ list_emu.at(i).icono, "WHERE tipo_emu='"+ list_emu.at(i).key +"'");
		}
	}

	sql->cargarMenuNav(ui->twInfo, categoria[id_cat].tabla);
}

void frmInfo::on_cbxCategorias_activated(int index)
{
	if (index > -1)
		id_cat = ui->cbxCategorias->itemData(index).toInt();
	else
		id_cat = 0;

	crearMenuNav();
}

void frmInfo::on_btnOk_clicked()
{
	QDialog::accept();
}
