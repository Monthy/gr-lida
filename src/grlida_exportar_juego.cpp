/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006  Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *
**/

#include "grlida_exportar_juego.h"

frmExportarJuego::frmExportarJuego(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

// Conecta los distintos botones
	connect( ui.btnOk           , SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.btnCheckedAll   , SIGNAL( clicked() ), this, SLOT( on_btnCheckedAll() ) );
	connect( ui.btnUnCheckedAll , SIGNAL( clicked() ), this, SLOT( on_btnUnCheckedAll() ) );
	connect( ui.btnDirExportPath, SIGNAL( clicked() ), this, SLOT( on_btnDirExportPath() ) );

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stIconDir = stHomeDir + "iconos/";	// directorio de iconos para el GR-lida
	stTheme   = fGrl.ThemeGrl();

	setStyleSheet( fGrl.StyleSheet() );

	QSettings lastdir( stHomeDir + "GR-lida.conf", QSettings::IniFormat);
	UltimoPath.clear();
	lastdir.beginGroup("UltimoDirectorio");
		UltimoPath["DirExportPath"] = lastdir.value("DirExportPath", "").toString();
	lastdir.endGroup();

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancelar->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnDirExportPath->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );

	CargarListaJuegos();

	ui.cbxExpotarComo->clear();
	ui.cbxExpotarComo->addItem("D-Fend Reloaded (*.prof)"); // Index 0
	ui.cbxExpotarComo->addItem("GR-lida (*.xml)");          // Index 1
	ui.cbxExpotarComo->setCurrentIndex( 1 );

	twMontajes = new QTreeWidget();

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmExportarJuego::~frmExportarJuego()
{
	delete twMontajes;
}

void frmExportarJuego::on_btnOk()
{
	QDir dir_export( ui.txtDirExportPath->text() );

	if( !ui.txtDirExportPath->text().isEmpty() && dir_export.exists() )
	{
		bool str_ok;
		QString archivo;

		int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );

		if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
		{
			ui.gBox_OptExportar->setEnabled( false );
			for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
			{
				QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
				if( item->checkState(0) == Qt::Checked )
				{
					CargarDatosExportar( item->text(1) );

					QFileInfo fi( item->text(0) );
					archivo = fi.completeBaseName();
					archivo = fGrl.eliminar_caracteres( archivo );

					switch ( ui.cbxExpotarComo->currentIndex() )
					{
						case 0: // D-Fend Reloaded
							if( TempDatosJuego["tipo_emu"] == "dosbox" )
							{
								str_ok = archivo.endsWith(".prof");
								if(str_ok == false)
									archivo.append(".prof");

								fGrl.Exportar_Profile_DFend( TempDatosJuego, TempDatosDosBox, twMontajes, ui.txtDirExportPath->text() + "/" + archivo);
							}
						break;
						case 1: // GR-lida
							str_ok = archivo.endsWith(".xml");
							if(str_ok == false)
								archivo.append(".xml");

							if( TempDatosJuego["tipo_emu"] == "scummvm" )
								fGrl.Exportar_Profile_GRLida( TempDatosJuego, TempDatosScummvm, twMontajes, ui.txtDirExportPath->text() + "/" + archivo);
							else
								fGrl.Exportar_Profile_GRLida( TempDatosJuego, TempDatosDosBox, twMontajes, ui.txtDirExportPath->text() + "/" + archivo);
						break;
					}
				}
			}
			ui.gBox_OptExportar->setEnabled( true );
		}
	}
}

void frmExportarJuego::on_btnCheckedAll()
{
	int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );
	if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
	{
		for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
		{
			QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
			item->setCheckState(0, Qt::Checked);
		}
	}
}

void frmExportarJuego::on_btnUnCheckedAll()
{
	int pos = ui.twListaJuegos->indexOfTopLevelItem( ui.twListaJuegos->currentItem() );
	if( ui.twListaJuegos->topLevelItemCount()>0 && pos!=-1 )
	{
		for(int num = 0; num < ui.twListaJuegos->topLevelItemCount(); num++ )
		{
			QTreeWidgetItem *item = ui.twListaJuegos->topLevelItem( num );
			item->setCheckState(0, Qt::Unchecked);
		}
	}
}

void frmExportarJuego::on_btnDirExportPath()
{
	ui.txtDirExportPath->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), UltimoPath["DirExportPath"], ui.txtDirExportPath->text() ));

	QSettings lastdir( stHomeDir+"GR-lida.conf", QSettings::IniFormat );
	lastdir.beginGroup("UltimoDirectorio");
		lastdir.setValue("DirExportPath", ui.txtDirExportPath->text()+"/" );
	lastdir.endGroup();
	UltimoPath["DirExportPath"] = ui.txtDirExportPath->text()+"/";
}

void frmExportarJuego::CargarListaJuegos(QString TipoEmu, QString stdb_Orden_By, QString stdb_Orden)
{
	QString stIcono, stSqlWhere;
	QSqlQuery query;

	if( !TipoEmu.isEmpty() )
		stSqlWhere = " WHERE tipo_emu LIKE '%"+TipoEmu+"%'";
	else
		stSqlWhere= " WHERE tipo_emu!='datos'";

	query.exec("SELECT * FROM dbgrl "+stSqlWhere+" ORDER BY "+ stdb_Orden_By +" "+stdb_Orden);

	if( query.first() )
	{
		do {
			QTreeWidgetItem *item = new QTreeWidgetItem( ui.twListaJuegos );

			item->setText( 0 , query.record().value("titulo").toString() );
			item->setCheckState(0, Qt::Unchecked);
			item->setText( 1 , query.record().value("idgrl").toString()    );	// titulo
			item->setText( 2 , query.record().value("tipo_emu").toString() );	// tipo_emu

			stIcono = fGrl.getIconListaJuegos(query.record().value("icono").toString(), stIconDir);
			item->setIcon( 0, QIcon(stIcono) );

		} while (query.next());
		ui.twListaJuegos->setCurrentItem( ui.twListaJuegos->itemAt(0,0) );
	}
}

void frmExportarJuego::CargarDatosExportar( QString stIDx )
{
	TempDatosJuego.clear();
	TempDatosScummvm.clear();
	TempDatosDosBox.clear();
	if( !stIDx.isEmpty() )
	{
		TempDatosJuego  = sql->show_Datos( stIDx );

		if( TempDatosJuego["tipo_emu"] == "scummvm" )
			TempDatosScummvm = sql->showConfg_ScummVM( stIDx );
		else if(TempDatosJuego["tipo_emu"]=="dosbox")
		{
			TempDatosDosBox = sql->showConfg_DOSBox( stIDx );

			twMontajes->clear();

			QSqlQuery query;
			query.exec("SELECT * FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+stIDx+" ORDER BY id_lista");
			if( query.first() )
			{
				do {
					QTreeWidgetItem *item_mount = new QTreeWidgetItem( twMontajes );

					item_mount->setText( 0 , query.record().value("path").toString() );			// path			- directorio o iso
					item_mount->setText( 1 , query.record().value("label").toString() );		// label		- etiqueta
					item_mount->setText( 2 , query.record().value("tipo_as").toString() );		// tipo_as		- tipo de montaje
					item_mount->setText( 3 , query.record().value("letter").toString() );		// letter		- letra de montaje
					item_mount->setText( 4 , query.record().value("indx_cd").toString() );		// indx_cd		- index de la unidad de cd-rom
					item_mount->setText( 5 , query.record().value("opt_mount").toString() );	// opt_mount	- opciones del cd-rom
					item_mount->setText( 6 , query.record().value("io_ctrl").toString() );		// io_ctrl		- cd/dvd
					item_mount->setText( 7 , query.record().value("select_mount").toString());	// select_mount	- primer montaje
					item_mount->setText( 8 , query.record().value("id").toString() );			// id
					item_mount->setText( 9 , query.record().value("id_lista").toString() );		// id_lista		- id_lista

				} while (query.next());
			}
		}
	}
}
