/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2011 Pedro A. Garcia Rosado Aka Monthy
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#include "grlida_instalar_juego.h"

frmInstalarJuego::frmInstalarJuego(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);

	connect( ui.cbxMontaje_type_drive, SIGNAL( activated(int) ), this, SLOT( on_changeTypeDrive(int) ) );
	connect( ui.btnOk       , SIGNAL( clicked() ), this, SLOT( on_btnOk()        ) );
	connect( ui.btnOrigen   , SIGNAL( clicked() ), this, SLOT( on_btnOrigen()    ) );
	connect( ui.btnDestino  , SIGNAL( clicked() ), this, SLOT( on_btnDestino()   ) );
	connect( ui.btnSubirIso , SIGNAL( clicked() ), this, SLOT( on_btnSubirIso()  ) );
	connect( ui.btnBajarIso , SIGNAL( clicked() ), this, SLOT( on_btnBajarIso()  ) );
	connect( ui.btnDeleteIso, SIGNAL( clicked() ), this, SLOT( on_btnDeleteIso() ) );
	connect( ui.lwOrigenMultiPath, SIGNAL( itemActivated(QListWidgetItem*) ), ui.btnDeleteIso, SLOT(setEnabled(bool)));

	stHomeDir = fGrl.GRlidaHomePath();		// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	setTheme();

	ui.cbxMontaje_type_drive->clear();
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_hd.png")    , tr("Carpeta como disco duro"), "drive"        );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_cdrom.png") , tr("Unidad de CD-ROM")       , "cdrom"        );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_floppy.png"), tr("Carpeta como disquete")  , "floppy"       );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/floppy_1.png")    , tr("Imagen de disquete")     , "IMG_floppy"   );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/cd_iso.png")      , tr("Imagen ISO, CUE/BIN")    , "IMG_iso"      );
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/cd_multi_iso.png"), tr("Imagen ISO multiples")   , "IMG_multi_iso");
	ui.cbxMontaje_type_drive->addItem( QIcon(stTheme+"img16/drive_hd.png")    , tr("Imagen de disco duro")   , "IMG_hdd"      );
	ui.cbxMontaje_type_drive->setCurrentIndex(1);
	on_changeTypeDrive(0);

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmInstalarJuego::~frmInstalarJuego()
{
	//
}

void frmInstalarJuego::setTheme()
{
	setStyleSheet( fGrl.StyleSheet() );
	setWindowIcon( QIcon(stTheme+"img16/applications.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );
	ui.btnOrigen->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnDestino->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnOrigenClear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
	ui.btnDestinoClear->setIcon( QIcon(stTheme+"img16/limpiar.png") );
}

void frmInstalarJuego::on_changeTypeDrive(int row)
{
	if( row >= 0)
	{
		if( ui.cbxMontaje_type_drive->itemData( row ).toString() == "IMG_multi_iso" )
		{
			ui.txtOrigenPath->setEnabled( false );
			ui.lwOrigenMultiPath->setEnabled( true );
			ui.btnSubirIso->setEnabled( true );
			ui.btnBajarIso->setEnabled( true );
			ui.btnDeleteIso->setEnabled( true );
		} else {
			ui.txtOrigenPath->setEnabled( true );
			ui.lwOrigenMultiPath->setEnabled( false );
			ui.btnSubirIso->setEnabled( false );
			ui.btnBajarIso->setEnabled( false );
			ui.btnDeleteIso->setEnabled( false );
		}
	}
}

void frmInstalarJuego::on_btnOk()
{
	QDir dir;
	QString install_info = tr("El ejecutable suele ser: INSTALL, si no esta seguro teclee DIR y pulsa INTRO");
	if( QFile::exists(GRLConfig["DirDOSBox"].toString()) && dir.exists(ui.txtDestinoPath->text()) )
	{
		QStringList lista_multiple_iso;
		QString tipo_origen = ui.cbxMontaje_type_drive->itemData( ui.cbxMontaje_type_drive->currentIndex() ).toString();
		QFile file_out(stHomeDir+"dosbox.conf");
		if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
		{
			QTextStream out(&file_out);
			out.setCodec("UTF-8");
			out << "[autoexec]" << endl;
			out << "@echo off" << endl;

			if( tipo_origen == "drive" )
				out << "mount D \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\"" << endl;
			else if( tipo_origen == "cdrom" )
				out << "mount D \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\" -t cdrom" << endl;
			else if( tipo_origen == "floppy" )
				out << "mount A \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\" -t floppy" << endl;
			else if( tipo_origen == "IMG_floppy" )
				out << "imgmount A \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\" -t floppy" << endl;
			else if( tipo_origen == "IMG_iso" )
				out << "imgmount D \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\" -t iso" << endl;
			else if( tipo_origen == "IMG_multi_iso" )
			{
				lista_multiple_iso.clear();
				for( int i = 0; i < ui.lwOrigenMultiPath->count(); ++i )
				{
					if(i == 0)
						lista_multiple_iso << "\""+fGrl.getShortPathName( ui.lwOrigenMultiPath->item(i)->text() )+"\"";
					else
						lista_multiple_iso << "\""+ ui.lwOrigenMultiPath->item(i)->text() +"\"";
				}
				out << "imgmount D " << lista_multiple_iso.join(" ") << " -t iso" << endl;
			}
			else if( tipo_origen == "IMG_hdd" )
				out << "imgmount D \"" << QDir::toNativeSeparators(ui.txtOrigenPath->text()) << "\" -t hdd" << endl;

			out << "mount C \"" << QDir::toNativeSeparators(ui.txtDestinoPath->text()) << "\"" << endl;
			out << "echo " << install_info << endl;

			if(tipo_origen == "floppy" || tipo_origen == "IMG_floppy")
				out << "A:" << endl;
			else
				out << "D:" << endl;

			out.flush();
			file_out.close();
		}
		Ejecutar( GRLConfig["DirDOSBox"].toString(), "-conf|"+stHomeDir+"dosbox.conf", stHomeDir);
	} else
		QMessageBox::information( this, "",tr("Puede que no este disponible el ejecutable del emulador.\nO el directorio de destino."));
}

void frmInstalarJuego::on_btnOrigen()
{
	QString tipo_montaje = ui.cbxMontaje_type_drive->itemData( ui.cbxMontaje_type_drive->currentIndex() ).toString();

	if( tipo_montaje == "drive" || tipo_montaje == "cdrom" || tipo_montaje == "floppy" )
	{
		QString directorio = fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Montaje_path"].toString(), ui.txtOrigenPath->text() );
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui.txtOrigenPath->setText( directorio );
			GRLConfig["Montaje_path"] = ui.txtOrigenPath->text();
		} else {
			ui.txtOrigenPath->setText( "" );
			GRLConfig["Montaje_path"] = "";
		}
	} else {
		QString tipo_archivo;
		if( tipo_montaje == "IMG_floppy" || tipo_montaje == "boot" || tipo_montaje == "IMG_hdd" )
			tipo_archivo = tr("Imagen") +" (*.ima *.img);;";
		else if( tipo_montaje == "IMG_iso" || tipo_montaje == "IMG_multi_iso" )
			tipo_archivo = tr("Imagen CD") +" (*.iso *.cue);;";
		else
			tipo_archivo = "";

		QString archivo = fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Montaje_path"].toString(), ui.txtOrigenPath->text(), tipo_archivo + tr("Todos los archivo") +" (*)", 0, false);
		QFileInfo fi( archivo );
		if( fi.exists() )
		{
			if( tipo_montaje == "IMG_multi_iso" )
			{
				QListWidgetItem *itemIso = new QListWidgetItem( ui.lwOrigenMultiPath );
				itemIso->setIcon( QIcon(stTheme+"img16/cd_iso.png") );
				itemIso->setText( archivo );
			} else
				ui.txtOrigenPath->setText( archivo );

			GRLConfig["Montaje_path"] = fi.absolutePath();
		} else {
			ui.txtOrigenPath->setText("");
			GRLConfig["Montaje_path"] = "";
		}
	}

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Montaje_path", GRLConfig["Montaje_path"].toString() );
}

void frmInstalarJuego::on_btnDestino()
{
	QString directorio = fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Montaje_path"].toString(), GRLConfig["DirBaseGames"].toString() );
	QDir dir( directorio );
	if( dir.exists() )
	{
		ui.btnOk->setEnabled(true);
		ui.txtDestinoPath->setText( directorio );
	} else {
		ui.btnOk->setEnabled(false);
		ui.txtDestinoPath->text() = "";
	}
}

void frmInstalarJuego::on_btnSubirIso()
{
	int listIndex = ui.lwOrigenMultiPath->currentRow();
	if(listIndex > 0)
	{
		QListWidgetItem *item = new QListWidgetItem(*ui.lwOrigenMultiPath->currentItem());
		delete ui.lwOrigenMultiPath->currentItem();
		ui.lwOrigenMultiPath->insertItem(listIndex - 1, item);
		ui.lwOrigenMultiPath->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btnBajarIso()
{
	int listIndex = ui.lwOrigenMultiPath->currentRow();
	if(listIndex < (ui.lwOrigenMultiPath->count() - 1))
	{
		QListWidgetItem *item = new QListWidgetItem(*ui.lwOrigenMultiPath->currentItem());
		delete ui.lwOrigenMultiPath->currentItem();
		ui.lwOrigenMultiPath->insertItem(listIndex + 1, item);
		ui.lwOrigenMultiPath->setCurrentItem(item);
	}
}

void frmInstalarJuego::on_btnDeleteIso()
{
	QListWidgetItem *item = ui.lwOrigenMultiPath->currentItem();
	if(item != NULL)
	{
		int respuesta = QMessageBox::question(this, tr("¿Eliminar...?"), tr("¿Quieres eliminar la ISO de la lista?"), tr("Si"), tr("Cancelar"), 0 , 1);
		if(respuesta == 0)
		{
			delete item;
		}
	}
}

void frmInstalarJuego::on_lwOrigenMultiPath_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if(previous != NULL)
	{
		//
	}

	if(current != NULL)
	{
		int listIndex = ui.lwOrigenMultiPath->currentRow();
		if(listIndex == 0)
			ui.btnSubirIso->setEnabled(false);
		else
			ui.btnSubirIso->setEnabled(true);

		if(listIndex == (ui.lwOrigenMultiPath->count() - 1))
			ui.btnBajarIso->setEnabled(false);
		else
			ui.btnBajarIso->setEnabled(true);

		ui.btnDeleteIso->setEnabled(true);
	}
	else
	{
		ui.btnDeleteIso->setEnabled(false);
		ui.btnSubirIso->setEnabled(false);
		ui.btnBajarIso->setEnabled(false);
	}
}

void frmInstalarJuego::Ejecutar(const QString &bin, const QString &parametros, const QString &dirWorking)
{
	QStringList stl_param;
	QProcess *ejecutar = new QProcess( this );
	QFile appBin( bin );
	if( appBin.exists() )
	{
		if( dirWorking != "" )
			ejecutar->setWorkingDirectory( dirWorking );

		if( parametros !="")
		{
			stl_param << parametros.split("|", QString::SkipEmptyParts);
			ejecutar->start( bin , stl_param );
			stl_param.clear();
		} else
			ejecutar->start( bin );
	} else
		QMessageBox::information( this, "",tr("No esta disponible el ejecutable del emulador.\nCompruebe si lo tiene instalado."));
}
