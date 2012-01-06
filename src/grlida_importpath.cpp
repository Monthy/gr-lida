/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2012 Pedro A. Garcia Rosado Aka Monthy
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

#include "grlida_importpath.h"

frmImportPath::frmImportPath(QDialog *parent, Qt::WFlags flags)
    : QDialog( parent, flags )
{
	ui.setupUi(this);
//	emit on_changeEnabled(false);

	stHomeDir = fGrl.GRlidaHomePath();	// directorio de trabajo del GR-lida
	stTheme   = fGrl.ThemeGrl();
	GRLConfig = fGrl.CargarGRLConfig( stHomeDir + "GR-lida.conf" );

	ui.twDatosJuego->header()->setStretchLastSection(true);
	ui.twDatosJuego->header()->setMovable(false);
	ui.twDatosJuego->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.twDatosJuego->setColumnWidth(0, 100);

	createConnections();
	setTheme();

// centra la ventana en el escritorio
	QDesktopWidget *desktop = qApp->desktop();
	const QRect rect = desktop->availableGeometry( desktop->primaryScreen() );
	int left = ( rect.width() - width() ) / 2;
	int top = ( rect.height() - height() ) / 2;
	setGeometry( left, top, width(), height() );
}

frmImportPath::~frmImportPath()
{
	//
}

void frmImportPath::on_changeEnabled(bool estado)
{
	ui.btnOk->setEnabled(estado);
	ui.frame_info->setVisible(!estado);
}

void frmImportPath::createConnections()
{
	connect( ui.btnOk, SIGNAL( clicked() ), this, SLOT( on_btnOk() ) );
	connect( ui.chk_selectDatos, SIGNAL(toggled(bool)), this, SLOT( on_estado_cheket_items(bool)) );

	connect( ui.btnDbx_FileConfg , SIGNAL( clicked() ), this, SLOT( on_btnDbx_FileConfg()  ) );
	connect( ui.btnDbx_ExeJuego  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeJuego()   ) );
	connect( ui.btnDbx_ExeSetup  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_ExeSetup()   ) );
	connect( ui.btnDbx_mapperfile, SIGNAL( clicked() ), this, SLOT( on_btnDbx_mapperfile() ) );
	connect( ui.btnDbx_language  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_language()   ) );
	connect( ui.btnDbx_capturas  , SIGNAL( clicked() ), this, SLOT( on_btnDbx_capturas()   ) );
	connect( ui.btnDbx_musica    , SIGNAL( clicked() ), this, SLOT( on_btnDbx_musica()     ) );

	connect( ui.btnSvm_Path        , SIGNAL( clicked() ), this, SLOT( on_btnSvm_Path()         ) );
	connect( ui.btnSvm_PathSave    , SIGNAL( clicked() ), this, SLOT( on_btnSvm_PathSave()     ) );
	connect( ui.btnSvm_PathSetup   , SIGNAL( clicked() ), this, SLOT( on_btnSvm_PathSetup()    ) );
	connect( ui.btnSvm_PathExtra   , SIGNAL( clicked() ), this, SLOT( on_btnSvm_PathExtra()    ) );
	connect( ui.btnSvm_PathCapturas, SIGNAL( clicked() ), this, SLOT( on_btnSvm_PathCapturas() ) );
	connect( ui.btnSvm_PathSonido  , SIGNAL( clicked() ), this, SLOT( on_btnSvm_PathSonido()   ) );

	connect( ui.btnVdms_PathConf, SIGNAL( clicked() ), this, SLOT( on_btnVdms_PathConf() ) );
	connect( ui.btnVdms_PathExe , SIGNAL( clicked() ), this, SLOT( on_btnVdms_PathExe()  ) );
}

void frmImportPath::setTheme()
{
	setWindowIcon( QIcon(stTheme+"img16/archivos.png") );

	ui.btnOk->setIcon( QIcon(stTheme+"img16/aplicar.png") );
	ui.btnCancel->setIcon( QIcon(stTheme+"img16/cancelar.png") );

	ui.btnDbx_FileConfg->setIcon( QIcon(stTheme +"img16/carpeta_1.png") );
	ui.btnDbx_ExeJuego->setIcon( QIcon(stTheme +"img16/carpeta_1.png") );
	ui.btnDbx_ExeSetup->setIcon( QIcon(stTheme +"img16/carpeta_1.png") );
	ui.btnDbx_language->setIcon( QIcon(stTheme +"img16/carpeta_1.png") );
	ui.btnDbx_mapperfile->setIcon( QIcon(stTheme +"img16/carpeta_1.png") );
	ui.btnDbx_capturas->setIcon( QIcon(stTheme +"img16/carpeta_0.png") );
	ui.btnDbx_musica->setIcon( QIcon(stTheme +"img16/carpeta_0.png") );

	ui.btnSvm_Path->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnSvm_PathSave->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnSvm_PathSetup->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnSvm_PathExtra->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnSvm_PathCapturas->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );
	ui.btnSvm_PathSonido->setIcon( QIcon(stTheme+"img16/carpeta_0.png") );

	ui.btnVdms_PathConf->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );
	ui.btnVdms_PathExe->setIcon( QIcon(stTheme+"img16/carpeta_1.png") );

	if( GRLConfig["font_usar"].toBool() )
		setStyleSheet(fGrl.StyleSheet()+"*{font-family:\""+GRLConfig["font_family"].toString()+"\";font-size:"+GRLConfig["font_size"].toString()+"pt;}");
	else
		setStyleSheet(fGrl.StyleSheet());
}

void frmImportPath::on_btnOk()
{
	QFile appConfg;
	QString stExeJuego;
	bool siguiente, isEmuEnabled;
	siguiente = true;

	if( ui.tabDatConf->isTabEnabled(1) || ui.tabDatConf->isTabEnabled(2) || ui.tabDatConf->isTabEnabled(3) )
		isEmuEnabled = true;
	else
		isEmuEnabled = false;

	if( isEmuEnabled )
	{
	// DOSBox
		if( ui.tabDatConf->isTabEnabled(1) )
			appConfg.setFileName( stHomeDir + "confdbx/"+ ui.txtDbx_path_conf->text() );
	// VDMSound
		if( ui.tabDatConf->isTabEnabled(3) )
			appConfg.setFileName( stHomeDir + "confvdms/"+ ui.txtVdms_path_conf->text() );

		if( appConfg.exists() )
		{
			siguiente = false;
			QMessageBox::information( this, "ImportPath", tr("El archivo de Configuración ya esixte")+":\n\n"+appConfg.fileName() );
		} else {
			siguiente = true;
			stExeJuego.clear();
		// DOSBox
			if( ui.tabDatConf->isTabEnabled(1) )
				stExeJuego = ui.txtDbx_path_exe->text();
		// ScummVM
			if( ui.tabDatConf->isTabEnabled(2) )
				stExeJuego = ui.txtSvm_path->text();
		// VDMSound
			if( ui.tabDatConf->isTabEnabled(3) )
				stExeJuego = ui.txtVdms_path_exe->text();

			if( stExeJuego.isEmpty() )
			{
				siguiente = false;
				if( ui.tabDatConf->isTabEnabled(2) )
					QMessageBox::information(this, "ImportPath", tr("Debes indicar el Directorio del juego") );
				else
					QMessageBox::information(this, "ImportPath", tr("Debes indicar el Ejecutable del juego") );
			} else
				siguiente = true;
		}
	}
	if( siguiente == true )
		QDialog::accept();
	else
		QDialog::rejected();
}

void frmImportPath::on_estado_cheket_items(bool estado)
{
	for(int num = 0; num < ui.twDatosJuego->topLevelItemCount(); num++ )
	{
		QTreeWidgetItem *item = ui.twDatosJuego->topLevelItem( num );
		if( estado )
			item->setCheckState(0,Qt::Checked );
		else
			item->setCheckState(0,Qt::Unchecked );
	}
}

void frmImportPath::on_btnDbx_FileConfg()
{
	QString archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir+"/confdbx/", ui.txtDbx_path_conf->text(), "Config DOSBox (*.conf);;"+ tr("Todos los archivo") +" (*)", 0, true);
	QFile appConfg( archivo );
	if( !appConfg.exists() )
	{
		QFileInfo fi( archivo );
		ui.txtDbx_path_conf->setText( fi.fileName() );
	} else {
		QMessageBox::information( this, "GR-lida", tr("El archivo de Configuración para el DOSBox ya esixte"));
		ui.txtDbx_path_conf->setText("");
	}
}

void frmImportPath::on_btnDbx_ExeJuego()
{
	ui.txtDbx_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_exe"].toString(), ui.txtDbx_path_exe->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_exe", GRLConfig["Dbx_path_exe"].toString() );
}

void frmImportPath::on_btnDbx_ExeSetup()
{
	ui.txtDbx_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_path_setup"].toString(), ui.txtDbx_path_setup->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_path_setup->text() );
	if( fi.exists() )
		GRLConfig["Dbx_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Dbx_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_setup", GRLConfig["Dbx_path_setup"].toString() );
}

void frmImportPath::on_btnDbx_language()
{
	ui.txtDbx_dosbox_language->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_dosbox_language"].toString(), ui.txtDbx_dosbox_language->text(), tr("Idioma") +" (*.lng *.txt);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_dosbox_language->text() );
	if( fi.exists() )
		GRLConfig["Dbx_dosbox_language"] = fi.absolutePath();
	else
		GRLConfig["Dbx_dosbox_language"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_language", GRLConfig["Dbx_dosbox_language"].toString() );
}

void frmImportPath::on_btnDbx_mapperfile()
{
	ui.txtDbx_sdl_mapperfile->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Dbx_sdl_mapperfile"].toString(), ui.txtDbx_sdl_mapperfile->text(), tr("KeyMapper") +" (*.map *.txt);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtDbx_sdl_mapperfile->text() );
	if( fi.exists() )
		GRLConfig["Dbx_sdl_mapperfile"] = fi.absolutePath();
	else
		GRLConfig["Dbx_sdl_mapperfile"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_sdl_mapperfile", GRLConfig["Dbx_sdl_mapperfile"].toString() );
}
/*
void frmImportPath::on_btnOpenDbxDir_6()
{
	ui.txtDbx_gus_ultradir->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_gus_ultradir"].toString(), ui.txtDbx_gus_ultradir->text() ));

	QDir dir( ui.txtDbx_gus_ultradir->text() );
	if( dir.exists() )
		GRLConfig["Dbx_gus_ultradir"] = ui.txtDbx_gus_ultradir->text();
	else
		GRLConfig["Dbx_gus_ultradir"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_gus_ultradir", GRLConfig["Dbx_gus_ultradir"].toString() );
}
*/
void frmImportPath::on_btnDbx_capturas()
{
	ui.txtDbx_dosbox_captures->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_dosbox_captures"].toString(), ui.txtDbx_dosbox_captures->text() ));

	QDir dir( ui.txtDbx_dosbox_captures->text() );
	if( dir.exists() )
		GRLConfig["Dbx_dosbox_captures"] = ui.txtDbx_dosbox_captures->text();
	else
		GRLConfig["Dbx_dosbox_captures"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_dosbox_captures", GRLConfig["Dbx_dosbox_captures"].toString() );
}

void frmImportPath::on_btnDbx_musica()
{
	ui.txtDbx_path_sonido->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Dbx_path_sonido"].toString(), ui.txtDbx_path_sonido->text() ));

	QDir dir( ui.txtDbx_path_sonido->text() );
	if( dir.exists() )
		GRLConfig["Dbx_path_sonido"] = ui.txtDbx_path_sonido->text();
	else
		GRLConfig["Dbx_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Dbx_path_sonido", GRLConfig["Dbx_path_sonido"].toString() );
}

void frmImportPath::on_btnSvm_Path()
{
	ui.txtSvm_path->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path"].toString(), ui.txtSvm_path->text() ));

	QDir dir( ui.txtSvm_path->text() );
	if( dir.exists() )
		GRLConfig["Svm_path"] = ui.txtSvm_path->text();
	else
		GRLConfig["Svm_path"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path", GRLConfig["Svm_path"].toString() );
}

void frmImportPath::on_btnSvm_PathSave()
{
	ui.txtSvm_path_save->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_savepath"].toString(), ui.txtSvm_path_save->text() ));

	QDir dir( ui.txtSvm_path_save->text() );
	if( dir.exists() )
		GRLConfig["Svm_savepath"] = ui.txtSvm_path_save->text();
	else
		GRLConfig["Svm_savepath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_savepath", GRLConfig["Svm_savepath"].toString() );
}

void frmImportPath::on_btnSvm_PathSetup()
{
	ui.txtSvm_path_setup->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Svm_path_setup"].toString(), ui.txtSvm_path_setup->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtSvm_path_setup->text() );
	if( fi.exists() )
		GRLConfig["Svm_path_setup"] = fi.absolutePath();
	else
		GRLConfig["Svm_path_setup"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_setup", GRLConfig["Svm_path_setup"].toString() );
}

void frmImportPath::on_btnSvm_PathExtra()
{
	ui.txtSvm_path_extra->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_extrapath"].toString(), ui.txtSvm_path_extra->text() ));

	QDir dir( ui.txtSvm_path_extra->text() );
	if( dir.exists() )
		GRLConfig["Svm_extrapath"] = ui.txtSvm_path_extra->text();
	else
		GRLConfig["Svm_extrapath"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_extrapath", GRLConfig["Svm_extrapath"].toString() );
}

void frmImportPath::on_btnSvm_PathCapturas()
{
	ui.txtSvm_path_capturas->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path_capturas"].toString(), ui.txtSvm_path_capturas->text() ));

	QDir dir( ui.txtSvm_path_capturas->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_capturas"] = ui.txtSvm_path_capturas->text();
	else
		GRLConfig["Svm_path_capturas"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_capturas", GRLConfig["Svm_path_capturas"].toString() );
}

void frmImportPath::on_btnSvm_PathSonido()
{
	ui.txtSvm_path_sonido->setText( fGrl.VentanaDirectorios( tr("Seleccionar un directorio."), GRLConfig["Svm_path_sonido"].toString(), ui.txtSvm_path_sonido->text() ));

	QDir dir( ui.txtSvm_path_sonido->text() );
	if( dir.exists() )
		GRLConfig["Svm_path_sonido"] = ui.txtSvm_path_sonido->text();
	else
		GRLConfig["Svm_path_sonido"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Svm_path_sonido", GRLConfig["Svm_path_sonido"].toString() );
}

void frmImportPath::on_btnVdms_PathConf()
{
	QString archivo = fGrl.VentanaAbrirArchivos( tr("Guardar archivo como..."), stHomeDir+"/confvdms/", ui.txtVdms_path_conf->text(), "Config VDMSound (*.vlp);;"+ tr("Todos los archivo") +" (*)", 0, true);
	QFile appConfg( archivo );
	if( !appConfg.exists() )
	{
		QFileInfo fi( archivo );
		ui.txtVdms_path_conf->setText( fi.fileName() );
	} else {
		QMessageBox::information( this, "GR-lida", tr("El archivo de Configuración para el VDMSound ya esixte"));
		ui.txtVdms_path_conf->setText("");
	}
}

void frmImportPath::on_btnVdms_PathExe()
{
	ui.txtVdms_path_exe->setText( fGrl.VentanaAbrirArchivos( tr("Selecciona un archivo"), GRLConfig["Vdms_path_exe"].toString(), ui.txtVdms_path_exe->text(), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)", 0, false) );

	QFileInfo fi( ui.txtVdms_path_exe->text() );
	if( fi.exists() )
		GRLConfig["Vdms_path_exe"] = fi.absolutePath();
	else
		GRLConfig["Vdms_path_exe"] = "";

	fGrl.GuardarKeyGRLConfig(stHomeDir+"GR-lida.conf","UltimoDirectorio","Vdms_path_exe", GRLConfig["Vdms_path_exe"].toString() );
}
