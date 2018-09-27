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

	grlDir.Home  = fGrl->GRlidaHomePath();
	grlDir.Datos = grlDir.Home +"datos/";

	cargarConfig();
	setTheme();
	cargarListaCategorias();

// centra la aplicacion en el escritorio
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
	ui->twInfo->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png"));
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

	ui->lb_info_grlida->setText( tr("Versión") +" GR-lida" );
	ui->lb_info_grlida_version->setText( fGrl->versionGrl() );

	ui->lb_info_qt->setText( tr("Versión") +" Qt" );
	ui->lb_info_qt_version->setText( qVersion() );

	ui->lb_info_dosbox->setText( "DOSBox "+ tr("Soportado") );
	ui->lb_info_dosbox_version->setText( fGrl->versionDbx() );

	ui->lb_info_scummvm->setText( "ScummVM "+ tr("Soportado") );
	ui->lb_info_scummvm_version->setText( fGrl->versionSvm() );

	ui->lb_info_vdmsound->setText( "VDMSound "+ tr("Soportado") );
	ui->lb_info_vdmsound_version->setText( fGrl->versionVdms() );
}

void frmInfo::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/informacion.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->lb_info_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/informacion.png") );
	ui->lb_info_grlida_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/gr-lida.png") );
	ui->lb_info_qt_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/qt.png") );
	ui->lb_info_dosbox_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/dosbox.png") );
	ui->lb_info_scummvm_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/scummvm.png") );
	ui->lb_info_vdmsound_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/vdmsound.png") );
}

// Carga las categorías de la base de datos.
void frmInfo::cargarListaCategorias()
{
	QSqlQuery query(sql->getSqlDB());
	stGrlCats cat;

	categoria.clear();
	ui->cbxCategorias->clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias ORDER BY orden ASC;");
	if( sql->chequearQuery(query) )
	{
		if( query.first() )
		{
			do {
				cat.id       = query.record().value("id").toString();
				cat.tabla    = query.record().value("tabla").toString();
				cat.titulo   = query.record().value("titulo").toString();
				cat.img      = query.record().value("img").toString();
				cat.orden    = query.record().value("orden").toString();
				cat.emu_show = query.record().value("emu_show").toString();
				categoria.insert(cat.id.toInt(), cat);

				QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
				if( QFile::exists(fGrl->ThemeApp() +"img16/cat/"+ cat.img) )
					ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16/cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
				else
					ui->cbxCategorias->addItem(QIcon(":/img16/cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
			} while ( query.next() );
		} else {
			cat.tabla    = "dbgrl";
			cat.titulo   = "Db GR-lida";
			cat.img      = "pc.png";
			cat.orden    = "0";
			cat.emu_show = "all";
			cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
			categoria.insert(cat.id.toInt(), cat);

			QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
			if( QFile::exists(fGrl->ThemeApp() +"img16/cat/"+ cat.img) )
				ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16/cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
			else
				ui->cbxCategorias->addItem(QIcon(":/img16/cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
		}
	} else {
		cat.tabla    = "dbgrl";
		cat.titulo   = "Db GR-lida";
		cat.img      = "pc.png";
		cat.orden    = "0";
		cat.emu_show = "all";
		cat.id       = sql->insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
		categoria.insert(cat.id.toInt(), cat);

		QString total = fGrl->IntToStr( sql->getCount(cat.tabla) );
		if( QFile::exists(fGrl->ThemeApp() +"img16/cat/"+ cat.img) )
			ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16/cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img16/cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
	}
	query.clear();

	int id_index = ui->cbxCategorias->findData(id_cat);
	if( id_index < 0 )
		id_index = 0;

	ui->cbxCategorias->setCurrentIndex(id_index);
	emit on_cbxCategorias_activated(id_index);
}

void frmInfo::menuNavAddCat(QString etiqueta, QString icono, QString sql_query, bool m_expanded, bool m_show_total)
{
	QFont m_font;
	int total = 0;
	if( m_show_total )
		total = sql->getCount(categoria[id_cat].tabla, sql_query);

	m_font.setBold( (total > 0) ? true : false);

	twListInfo = new QTreeWidgetItem( ui->twInfo );
	twListInfo->setIcon( 0, QIcon(icono) );
	twListInfo->setTextAlignment(1, Qt::AlignCenter);
	twListInfo->setFont( 1, m_font   );
	twListInfo->setText( 0, etiqueta );
	twListInfo->setText( 1, m_show_total ? fGrl->IntToStr(total) : "");
	twListInfo->setText( 2, sql_query );
	twListInfo->setExpanded( m_expanded );
}

void frmInfo::menuNavAddSubCat(QString etiqueta, QString icono, QString sql_query, QString sql_col)
{
	QFont m_font;
	int total = 0;
	total = sql->getCount(categoria[id_cat].tabla, sql_query, sql_col, etiqueta);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem( twListInfo );

	if( QFile::exists(icono) )
		sub_cat->setIcon( 0, QIcon( icono ) );
	else
		sub_cat->setIcon( 0, QIcon(fGrl->Theme() +"img16/sinimg.png") );

	m_font.setBold( (total > 0) ? true : false);

	sub_cat->setTextAlignment(1, Qt::AlignCenter);
	sub_cat->setFont( 1, m_font               );
	sub_cat->setText( 0, etiqueta             );
	sub_cat->setText( 1, fGrl->IntToStr(total) );
	sub_cat->setText( 2, sql_query            );
}

void frmInfo::crearMenuNav()
{
	ui->twInfo->clear();

	menuNavAddCat( tr("Todos")     , fGrl->Theme() +"img16/basedatos.png"    , "", false, true);
	menuNavAddCat( tr("Favoritos") , fGrl->Theme() +"img16/"+ grlCfg.IconoFav, "WHERE favorito='true'", false);
	menuNavAddCat( tr("Originales"), fGrl->Theme() +"img16/original.png"     , "WHERE original='true'", false);

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);

	menuNavAddCat( tr("Tipo emulador"), fGrl->Theme() +"img16/tag.png", "", true, false);
	if( lista.isEmpty() || lista.contains("all") || lista.contains("dosbox") )
		menuNavAddSubCat("DOSBox"  , fGrl->Theme() +"img16/dosbox.png"  , "WHERE tipo_emu='dosbox'"  );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("scummvm") )
		menuNavAddSubCat("ScummVM" , fGrl->Theme() +"img16/scummvm.png" , "WHERE tipo_emu='scummvm'" );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound") )
		menuNavAddSubCat("VDMSound", fGrl->Theme() +"img16/vdmsound.png", "WHERE tipo_emu='vdmsound'");
	if( lista.isEmpty() || lista.contains("all") || lista.contains("datos") )
		menuNavAddSubCat("Datos"   , fGrl->Theme() +"img16/datos_3.png" , "WHERE tipo_emu='datos'"   );

	QList<stGrlDatos> list_emu = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	const int listSize = list_emu.size();
	for (int i = 0; i < listSize; ++i)
	{
		if( lista.isEmpty() || lista.contains("all") || lista.contains(list_emu.at(i).key) )
		{
			menuNavAddSubCat(list_emu.at(i).titulo, fGrl->ThemeApp() +"img16/cat/"+ list_emu.at(i).icono, "WHERE tipo_emu='"+ list_emu.at(i).key +"'");
		}
	}

	sql->cargarMenuNav(ui->twInfo, categoria[id_cat].tabla);
}

void frmInfo::on_cbxCategorias_activated(int index)
{
	if( index > -1 )
		id_cat = ui->cbxCategorias->itemData(index, Qt::UserRole).toInt();
	else
		id_cat = 0;

	crearMenuNav();
}

void frmInfo::on_btnOk_clicked()
{
	QDialog::accept();
}
