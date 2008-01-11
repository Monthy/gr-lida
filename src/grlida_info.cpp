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

#include "grlida_info.h"

frmInfo::frmInfo(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	
	stHomeDir     = QDir::homePath()+"/.gr-lida/";	// directorio de trabajo del GR-lida
//	stHomeDir     = QDir::currentPath()+"/";		// directorio de trabajo del GR-lida
//	stHomeDir     = "./";							// directorio de trabajo del GR-lida	

	QSettings settings(stHomeDir + "GR-lida.conf", QSettings::IniFormat); 
	settings.beginGroup("SqlDatabase");
	  stdb_host	   = settings.value("db_host"	 , stHomeDir + "db_grl.grl" ).toString();
	settings.endGroup();
	
	QFileInfo fidb( stdb_host );
	
	ui.twInfo->header()->setStretchLastSection(true); 
	ui.twInfo->header()->setMovable(false);
	ui.twInfo->header()->setResizeMode(QHeaderView::Interactive);
	ui.twInfo->header()->resizeSection(0, 200 );
	
	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Versión")+ " GR-lida"	); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/grlida.png")	); //
		ItemInfo->setText( 1 , fGrl.stVersionGrl() 			); //	
	
	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Versión")+ " QT"			); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/qt.png")		); //
		ItemInfo->setText( 1 , qVersion() 					); //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , "DOSBox "+ tr("Soportado")	); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/dosbox.png")	); //
		ItemInfo->setText( 1 , fGrl.stVersionDbx()			); //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , "ScummVM "+ tr("Soportado")	); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/scummvm.png") ); //
		ItemInfo->setText( 1 , fGrl.stVersionSvm()			); //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , "VDMSound "+ tr("Soportado")	); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/vdmsound.png")); //
		ItemInfo->setText( 1 , fGrl.stVersionVdms() 		); //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , "" ); //
		ItemInfo->setIcon( 0 , QIcon(":/img16/sinimg.png")	); //
		ItemInfo->setText( 1 , "" ); //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Base de Datos")			); //
		ItemInfo->setIcon( 0 ,QIcon(":/img16/basedatos.png")); //
		ItemInfo->setText( 1 , fidb.fileName() 				); //
	
	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Total de Juegos")		) ; //
		ItemInfo->setIcon( 0 , QIcon(":/img16/grlida.png")	); //
		ItemInfo->setText( 1 , fGrl.IntToStr(sql->getCount("dbgrl")) + tr(" juego/s")	) ; //
		
	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Total de Juegos Datos")) ; //
		ItemInfo->setIcon( 0 , QIcon(":/img16/datos_1.png") ); //
		ItemInfo->setText( 1 , fGrl.IntToStr(sql->getCount("dbgrl","WHERE tipo_emu=\"datos\"")) + tr(" juego/s") ) ; //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Total de Juegos") + " DOSBox") ; //
		ItemInfo->setIcon( 0 , QIcon(":/img16/dosbox.png") ); //
		ItemInfo->setText( 1 , fGrl.IntToStr(sql->getCount("dbgrl","WHERE tipo_emu=\"dosbox\"")) + tr(" juego/s") ) ; //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Total de Juegos") + " ScummVM") ; //
		ItemInfo->setIcon( 0 , QIcon(":/img16/scummvm.png") ); //
		ItemInfo->setText( 1 , fGrl.IntToStr(sql->getCount("dbgrl","WHERE tipo_emu=\"scummvm\"")) + tr(" juego/s") ) ; //

	ItemInfo = new QTreeWidgetItem( ui.twInfo );
		ItemInfo->setText( 0 , tr("Total de Juegos") + " VDMSound") ; //
		ItemInfo->setIcon( 0 , QIcon(":/img16/vdmsound.png") ); //
		ItemInfo->setText( 1 , fGrl.IntToStr(sql->getCount("dbgrl","WHERE tipo_emu=\"vdmsound\"")) + tr(" juego/s") ) ; //

// centra la aplicacion en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmInfo::~frmInfo()
{

}

