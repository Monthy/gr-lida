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

#include "grlida_exportar_juego.h"
#include "ui_exportar_juego.h"

frmExportarJuego::frmExportarJuego(dbSql *m_sql, stGrlCfg m_cfg, int m_id_cat, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmExportarJuego)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;
	sql    = m_sql;
	id_cat = m_id_cat;

	grlDir.Home   = fGrl->GRlidaHomePath();
	grlDir.Datos  = grlDir.Home +"datos/";
	grlDir.Iconos = grlDir.Home +"iconos/";

	cargarConfig();
	setTheme();
	cargarListaCategorias();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmExportarJuego::~frmExportarJuego()
{
	delete fGrl;
	delete twMontajes;
	delete ui;
}

void frmExportarJuego::cargarConfig()
{
// Configuración del twListaJuegos
	ui->twListaJuegos->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png"));
	ui->twListaJuegos->header()->setMovable(false);
	ui->twListaJuegos->header()->setStretchLastSection(true);

	ui->cbxExpotarComo->clear();
	ui->cbxExpotarComo->addItem(QIcon(fGrl->Theme() +"img16/gr-lida.png"), "GR-lida (*.xml)"         , "grl"  );
	ui->cbxExpotarComo->addItem(QIcon(fGrl->Theme() +"img16/DFend.png")  , "D-Fend Reloaded (*.prof)", "dfend");
	ui->cbxExpotarComo->setCurrentIndex(0);
	emit on_cbxExpotarComo_activated(0);

	twMontajes = new QTreeWidget(this);
	twMontajes->setVisible(false);

	emu_list = fGrl->cargaDatosQHash(grlDir.Datos +"emu_list.txt", 4, "|");
}

void frmExportarJuego::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/exportar.png") );

	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancelar->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );
	ui->btnDirExportPath->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
}

// Carga las categorías de la base de datos.
void frmExportarJuego::cargarListaCategorias()
{
//setUpdatesEnabled( false );
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
				if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
					ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
				else
					ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
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
			if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
				ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
			else
				ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
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
		if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ cat.img) )
			ui->cbxCategorias->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ cat.img), cat.titulo +" ("+ total +")", cat.id);
		else
			ui->cbxCategorias->addItem(QIcon(":/img16_cat/sinimg.png"), cat.titulo +" ("+ total +")", cat.id);
	}
	query.clear();
//setUpdatesEnabled( true );

	int id_index = ui->cbxCategorias->findData(id_cat);
	if( id_index < 0 )
		id_index = 0;

	ui->cbxCategorias->setCurrentIndex(id_index);
	emit on_cbxCategorias_activated(id_index);
}

void frmExportarJuego::cargarListaJuegos(QString sql_where)
{
	QSqlQuery query(sql->getSqlDB());
	query.exec("SELECT idgrl, icono, titulo FROM "+ categoria[id_cat].tabla +" "+ sql_where +" ORDER BY titulo ASC");
	ui->twListaJuegos->clear();
	if( query.first() )
	{
		do {
			QTreeWidgetItem *item = new QTreeWidgetItem( ui->twListaJuegos );
			item->setIcon( 0, QIcon(grlDir.Iconos + query.record().value("icono").toString()) );
			item->setCheckState(0, Qt::Unchecked);
			item->setText( 0, query.record().value("titulo").toString()   );
			item->setText( 1, query.record().value("idgrl").toString()    );
		} while (query.next());
		ui->twListaJuegos->setCurrentItem( ui->twListaJuegos->itemAt(0,0) );
	}

	if( ui->twListaJuegos->topLevelItemCount() > 0 )
		ui->btnOk->setEnabled( true );
	else
		ui->btnOk->setEnabled( false );
}

void frmExportarJuego::cargarDatosExportar(QString IdGame)
{
	if( !IdGame.isEmpty() )
	{
		DatosJuego = sql->show_Datos(categoria[id_cat].tabla, IdGame);

		if( DatosJuego.tipo_emu == "scummvm" )
			DatosScummVM = sql->showConfg_ScummVM(IdGame, fGrl->IntToStr(id_cat));
		else if( DatosJuego.tipo_emu == "vdmsound" )
			DatosVDMSound = sql->showConfg_VDMSound(IdGame, fGrl->IntToStr(id_cat));
		else if( DatosJuego.tipo_emu == "dosbox" )
		{
			DatosDosBox = sql->showConfg_DOSBox(IdGame, fGrl->IntToStr(id_cat));
			twMontajes->clear();

			QSqlQuery query(sql->getSqlDB());
			query.exec("SELECT id, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ DatosDosBox.id +" ORDER BY id_lista");
			if( query.first() )
			{
				do {
					QTreeWidgetItem *item = new QTreeWidgetItem( twMontajes );
					item->setText( 0, query.record().value("path").toString()         );	// Directorio o iso
					item->setText( 1, query.record().value("letter").toString()       );	// Letra de montaje
					item->setText( 2, query.record().value("tipo_as").toString()      );	// Tipo de montaje
					item->setText( 3, query.record().value("label").toString()        );	// Etiqueta
					item->setText( 4, query.record().value("indx_cd").toString()      );	// Index de la unidad de cd-rom
					item->setText( 5, query.record().value("opt_mount").toString()    );	// Opciones del cd-rom
					item->setText( 6, query.record().value("io_ctrl").toString()      );	// Cd/dvd
					item->setText( 7, query.record().value("select_mount").toString() );	// Primer montaje
					item->setText( 8, query.record().value("id").toString()           );	// Id
					item->setText( 9, query.record().value("id_lista").toString()     );	// Id_lista
					item->setText(10, query.record().value("opt_size").toString()     );
					item->setText(11, query.record().value("opt_freesize").toString() );
					item->setText(12, query.record().value("freesize").toString()     );
				} while (query.next());
			}
		}
	}
}

void frmExportarJuego::on_cbxCategorias_activated(int index)
{
	if( index > -1 )
		id_cat = ui->cbxCategorias->itemData(index, Qt::UserRole).toInt();
	else
		id_cat = 0;

	ui->cbxTipoEmu->clear();
	ui->cbxTipoEmu->addItem(QIcon(fGrl->Theme() +"img16/tag.png"), tr("Todos los emuladores"), "");

	QStringList lista = categoria[id_cat].emu_show.split(";", QString::SkipEmptyParts);
	if( lista.isEmpty() || lista.contains("all") || lista.contains("datos") )
		ui->cbxTipoEmu->addItem(QIcon(fGrl->Theme() +"img16/datos.png")   , tr("Datos")   , "WHERE tipo_emu='datos'"   );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("dosbox") )
		ui->cbxTipoEmu->addItem(QIcon(fGrl->Theme() +"img16/dosbox.png")  , tr("DOSBox")  , "WHERE tipo_emu='dosbox'"  );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("scummvm") )
		ui->cbxTipoEmu->addItem(QIcon(fGrl->Theme() +"img16/scummvm.png") , tr("ScummVM") , "WHERE tipo_emu='scummvm'" );
	if( lista.isEmpty() || lista.contains("all") || lista.contains("vdmsound") )
		ui->cbxTipoEmu->addItem(QIcon(fGrl->Theme() +"img16/vdmsound.png"), tr("VDMSound"), "WHERE tipo_emu='vdmsound'");

	foreach (const stGrlDatos &dat, emu_list)
	{
		if( lista.isEmpty() || lista.contains("all") || lista.contains(dat.key) )
		{
			if( QFile::exists(fGrl->ThemeApp() +"img16_cat/"+ dat.icono) )
				ui->cbxTipoEmu->addItem(QIcon(fGrl->ThemeApp() +"img16_cat/"+ dat.icono), dat.titulo, "WHERE tipo_emu='"+ dat.key +"'");
			else
				ui->cbxTipoEmu->addItem(QIcon(":/img16_cat/sinimg.png"), dat.titulo, "WHERE tipo_emu='"+ dat.key +"'");
		}
	}

	ui->cbxTipoEmu->setCurrentIndex(0);
	emit on_cbxTipoEmu_activated(0);
}

void frmExportarJuego::on_cbxTipoEmu_activated(int index)
{
	if( index > -1 )
		cargarListaJuegos(ui->cbxTipoEmu->itemData(index, Qt::UserRole).toString());
	else
		cargarListaJuegos();
}

void frmExportarJuego::on_btnCheckedAll_clicked()
{
	int pos = ui->twListaJuegos->indexOfTopLevelItem( ui->twListaJuegos->currentItem() );
	const int listSize = ui->twListaJuegos->topLevelItemCount();
	if( listSize > 0 && pos != -1 )
	{
		for(int i = 0; i < listSize; ++i)
		{
			QTreeWidgetItem *item = ui->twListaJuegos->topLevelItem(i);
			item->setCheckState(0, Qt::Checked);
		}
	}
}

void frmExportarJuego::on_btnUnCheckedAll_clicked()
{
	int pos = ui->twListaJuegos->indexOfTopLevelItem( ui->twListaJuegos->currentItem() );
	const int listSize = ui->twListaJuegos->topLevelItemCount();
	if( listSize > 0 && pos != -1 )
	{
		for(int i = 0; i < listSize; ++i)
		{
			QTreeWidgetItem *item = ui->twListaJuegos->topLevelItem(i);
			item->setCheckState(0, Qt::Unchecked);
		}
	}
}

void frmExportarJuego::on_cbxExpotarComo_activated(int index)
{
	if( index > -1 )
		tipo_export = ui->cbxExpotarComo->itemData(index).toString();
	else
		tipo_export = "grl";
}

void frmExportarJuego::on_btnDirExportPath_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.DirExportPath);

	if( !directorio.isEmpty() && fGrl->comprobarDirectorio(directorio, true) )
	{
		ui->txtDirExportPath->setText( fGrl->setDirRelative(directorio) );
		grlCfg.DirExportPath = ui->txtDirExportPath->text();

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "DirExportPath", grlCfg.DirExportPath);
	}
}

void frmExportarJuego::on_btnDirExportPath_clear_clicked()
{
	ui->txtDirExportPath->clear();
}

void frmExportarJuego::on_btnOk_clicked()
{
	QDir dir_export( ui->txtDirExportPath->text() );

	if( !ui->txtDirExportPath->text().isEmpty() && dir_export.exists() )
	{
		int pos = ui->twListaJuegos->indexOfTopLevelItem( ui->twListaJuegos->currentItem() );
		const int listSize = ui->twListaJuegos->topLevelItemCount();
		if( listSize > 0 && pos != -1 )
		{
			QStringList lista_id;
			bool multiple = false;
			int isChecked = 0;

			ui->gBox_OptExportar->setEnabled(false);
			lista_id.clear();
			for(int i = 0; i < listSize; ++i)
			{
				if( ui->twListaJuegos->topLevelItem(i)->checkState(0) == Qt::Checked )
				{
					lista_id << ui->twListaJuegos->topLevelItem(i)->text(1);
					++isChecked;
				}
				if( isChecked > 1 )
					multiple = true;
			}

			QString dir_destino, archivo;

			dir_destino = dir_export.absolutePath();
			if( !dir_destino.endsWith("/") )
				dir_destino.append("/");

			if( multiple )
			{
				dir_destino.append("GR-lida_Conf_Export/");
				fGrl->comprobarDirectorio(dir_destino);
			}

			for(int i = 0; i < lista_id.size(); ++i)
			{
				cargarDatosExportar( lista_id.at(i) );
				archivo = "id-"+ DatosJuego.idgrl +"_"+ fGrl->eliminar_caracteres(DatosJuego.titulo) +"_"+ DatosJuego.tipo_emu;
				QList<QString> url_list = sql->getDatosUrls(DatosJuego.idgrl, categoria[id_cat].id);

				if( tipo_export == "dfend" )
				{
					if( DatosJuego.tipo_emu == "dosbox" )
					{
						if( !archivo.endsWith(".prof") )
							archivo.append(".prof");

						fGrl->crearArchivoConfigDbx(DatosJuego, url_list, DatosDosBox, twMontajes, grlDir.Home, categoria[id_cat].tabla, dir_destino + archivo, ExportDfend);
					}
				}

				if( tipo_export == "grl" )
				{
					if( !archivo.endsWith(".xml") )
						archivo.append(".xml");

					fGrl->exportarProfileGRlida(DatosJuego, url_list, DatosDosBox, twMontajes, DatosScummVM, DatosVDMSound, grlDir.Home, categoria[id_cat].tabla, dir_destino + archivo, ExportGrlida);
				}
			}
			ui->gBox_OptExportar->setEnabled(true);
		}
	}
}

void frmExportarJuego::on_btnCancelar_clicked()
{
	QDialog::reject();
}
