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

#include "grlida_importpath.h"

frmImportPath::frmImportPath(stGrlCfg m_cfg, stGrlCats m_categoria, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmImportPath)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg    = m_cfg;
	categoria = m_categoria;

	grlDir.Home      = fGrl->GRlidaHomePath();
	grlDir.Capturas  = grlDir.Home +"capturas/"+ categoria.tabla +"/";
	grlDir.Confdbx   = grlDir.Home +"confdbx/"+ categoria.tabla +"/";
	grlDir.Confvdms  = grlDir.Home +"confvdms/"+ categoria.tabla +"/";
	grlDir.Covers    = grlDir.Home +"covers/"+ categoria.tabla +"/";
	grlDir.Datos     = grlDir.Home +"datos/";
	grlDir.Iconos    = grlDir.Home +"iconos/";
	grlDir.Idiomas   = grlDir.Home +"idiomas/";
	grlDir.Scripts   = grlDir.Home +"scripts/";
	grlDir.Smiles    = grlDir.Home +"smiles/";
	grlDir.Temp      = grlDir.Home +"temp/";
	grlDir.Templates = grlDir.Home +"templates/";
	grlDir.Themes    = grlDir.Home +"themes/";
	grlDir.Thumbs    = grlDir.Home +"thumbs/"+ categoria.tabla +"/";

	setTheme();

// centra la aplicación en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmImportPath::~frmImportPath()
{
	delete fGrl;
	delete ui;
}

void frmImportPath::setTheme()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/importar.png") );

	ui->lb_icon->setPixmap( QPixmap(fGrl->Theme() +"img32/descarga.png") );
	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnCancel->setIcon( QIcon(fGrl->Theme() +"img16/cancelar.png") );

	ui->tabDatConf->setTabIcon(0, QIcon(fGrl->Theme() +"img16/datos_3.png") );
	ui->tabDatConf->setTabIcon(1, QIcon(fGrl->Theme() +"img16/dosbox.png") );
	ui->tabDatConf->setTabIcon(2, QIcon(fGrl->Theme() +"img16/scummvm.png") );
	ui->tabDatConf->setTabIcon(3, QIcon(fGrl->Theme() +"img16/vdmsound.png") );

//-- DOSBox
	ui->btnDbx_FileConfg->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_FileConfg_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_ExeJuego->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_ExeJuego_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_ExeSetup->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_ExeSetup_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_language->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_language_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_mapperfile->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDbx_mapperfile_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_gus_ultradir_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_capturas->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnDbx_capturas_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDbx_musica->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnDbx_musica_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
//-- ScummVM
	ui->btnSvm_Path->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_Path_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSave->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathSave_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSetup->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnSvm_PathSetup_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathExtra->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathExtra_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathCapturas->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathCapturas_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnSvm_PathSonido->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_0.png") );
	ui->btnSvm_PathSonido_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
//-- VDMSound
	ui->btnVdms_PathConf->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_PathConf_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnVdms_PathExe->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnVdms_PathExe_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
}

void frmImportPath::statusBtnEnabled(bool estado)
{
	ui->btnOk->setEnabled(estado);
	ui->frame_info->setVisible(!estado);
}

void frmImportPath::on_btnOk_clicked()
{
	QDialog::accept();
}

void frmImportPath::on_btnCancel_clicked()
{
	QDialog::reject();
}

//-- Datos
void frmImportPath::on_twDatosJuego_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > 0 )
	{
		if( item->checkState(0) == Qt::Checked )
			item->setCheckState(0, Qt::Unchecked);
		else
			item->setCheckState(0, Qt::Checked);
	}
}

void frmImportPath::on_chk_selectDatos_clicked(bool checked)
{
	const int count_num = ui->twDatosJuego->topLevelItemCount();
	for (int num = 0; num < count_num; ++num)
	{
		QTreeWidgetItem *item = ui->twDatosJuego->topLevelItem( num );
		if( checked )
			item->setCheckState(0, Qt::Checked);
		else
			item->setCheckState(0, Qt::Unchecked);
	}
}

//-- DOSBox
void frmImportPath::on_btnDbx_FileConfg_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Guardar archivo como..."), grlDir.Confdbx, ui->txtDbx_path_conf->text(), tr("Todos los archivo") +" (*)", 0, true);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
		ui->txtDbx_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".conf");
	else
		ui->txtDbx_path_conf->setText(f_info.Name +".conf");
}

void frmImportPath::on_btnDbx_FileConfg_clear_clicked()
{
	ui->txtDbx_path_conf->clear();
}

void frmImportPath::on_btnDbx_ExeJuego_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_path_exe, fGrl->getDirRelative(ui->txtDbx_path_exe->text(), "DosGames"), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDbx_path_exe->setText( fGrl->setDirRelative(archivo, "DosGames") );
		grlCfg.Dbx_path_exe = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path_exe", grlCfg.Dbx_path_exe);
	}
}

void frmImportPath::on_btnDbx_ExeJuego_clear_clicked()
{
	ui->txtDbx_path_exe->clear();
}

void frmImportPath::on_btnDbx_ExeSetup_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_path_setup, fGrl->getDirRelative(ui->txtDbx_path_setup->text(), "DosGames"), tr("Ejecutables") +" (*.exe *.bat *.com);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDbx_path_setup->setText( fGrl->setDirRelative(archivo, "DosGames") );
		grlCfg.Dbx_path_setup = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path_setup", grlCfg.Dbx_path_setup);
	}
}

void frmImportPath::on_btnDbx_ExeSetup_clear_clicked()
{
	ui->txtDbx_path_setup->clear();
}

void frmImportPath::on_btnDbx_language_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_dosbox_language, fGrl->getDirRelative(ui->txtDbx_dosbox_language->text()), tr("Idioma") +" (*.lng *.txt);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDbx_dosbox_language->setText( fGrl->setDirRelative(archivo) );
		grlCfg.Dbx_dosbox_language = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_dosbox_language", grlCfg.Dbx_dosbox_language);
	}
}

void frmImportPath::on_btnDbx_language_clear_clicked()
{
	ui->txtDbx_dosbox_language->clear();
}

void frmImportPath::on_btnDbx_mapperfile_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Dbx_sdl_mapperfile, fGrl->getDirRelative(ui->txtDbx_sdl_mapperfile->text()), "KeyMapper (*.map *.txt);;"+ tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtDbx_sdl_mapperfile->setText( fGrl->setDirRelative(archivo) );
		grlCfg.Dbx_sdl_mapperfile = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_sdl_mapperfile", grlCfg.Dbx_sdl_mapperfile);
	}
}

void frmImportPath::on_btnDbx_mapperfile_clear_clicked()
{
	ui->txtDbx_sdl_mapperfile->clear();
}

void frmImportPath::on_btnDbx_gus_ultradir_clear_clicked()
{
	ui->txtDbx_gus_ultradir->clear();
}

void frmImportPath::on_btnDbx_capturas_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Dbx_dosbox_captures, fGrl->getDirRelative(ui->txtDbx_dosbox_captures->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtDbx_dosbox_captures->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Dbx_dosbox_captures = ui->txtDbx_dosbox_captures->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_dosbox_captures", grlCfg.Dbx_dosbox_captures);
		}
	}
}

void frmImportPath::on_btnDbx_capturas_clear_clicked()
{
	ui->txtDbx_dosbox_captures->clear();
}

void frmImportPath::on_btnDbx_musica_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Dbx_path_sonido, fGrl->getDirRelative(ui->txtDbx_path_sonido->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtDbx_path_sonido->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Dbx_path_sonido = ui->txtDbx_path_sonido->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Dbx_path_sonido", grlCfg.Dbx_path_sonido);
		}
	}
}

void frmImportPath::on_btnDbx_musica_clear_clicked()
{
	ui->txtDbx_path_sonido->clear();
}

//-- ScummVM
void frmImportPath::on_btnSvm_Path_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path, fGrl->getDirRelative(ui->txtSvm_path->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_path = ui->txtSvm_path->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path", grlCfg.Svm_path);
		}
	}
}

void frmImportPath::on_btnSvm_Path_clear_clicked()
{
	ui->txtSvm_path->clear();
}

void frmImportPath::on_btnSvm_PathSave_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_savepath, fGrl->getDirRelative(ui->txtSvm_path_save->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_save->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_savepath = ui->txtSvm_path_save->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_savepath", grlCfg.Svm_savepath);
		}
	}
}

void frmImportPath::on_btnSvm_PathSave_clear_clicked()
{
	ui->txtSvm_path_save->clear();
}

void frmImportPath::on_btnSvm_PathSetup_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Svm_path_setup, fGrl->getDirRelative(ui->txtSvm_path_setup->text(), "DosGames"), tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtSvm_path_setup->setText( fGrl->setDirRelative(archivo, "DosGames") );
		grlCfg.Svm_path_setup = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_setup", grlCfg.Svm_path_setup);
	}
}

void frmImportPath::on_btnSvm_PathSetup_clear_clicked()
{
	ui->txtSvm_path_setup->clear();
}

void frmImportPath::on_btnSvm_PathExtra_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_extrapath, fGrl->getDirRelative(ui->txtSvm_path_extra->text(), "DosGames") );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_extra->setText( fGrl->setDirRelative(directorio, "DosGames") );
			grlCfg.Svm_extrapath = ui->txtSvm_path_extra->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_extrapath", grlCfg.Svm_extrapath);
		}
	}
}

void frmImportPath::on_btnSvm_PathExtra_clear_clicked()
{
	ui->txtSvm_path_extra->clear();
}

void frmImportPath::on_btnSvm_PathCapturas_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_capturas, fGrl->getDirRelative(ui->txtSvm_path_capturas->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_capturas->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Svm_path_capturas = ui->txtSvm_path_capturas->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_capturas", grlCfg.Svm_path_capturas);
		}
	}
}

void frmImportPath::on_btnSvm_PathCapturas_clear_clicked()
{
	ui->txtSvm_path_capturas->clear();
}

void frmImportPath::on_btnSvm_PathSonido_clicked()
{
	QString directorio = fGrl->ventanaDirectorios( tr("Seleccionar un directorio"), grlCfg.Svm_path_sonido, fGrl->getDirRelative(ui->txtSvm_path_sonido->text()) );

	if( !directorio.isEmpty() )
	{
		QDir dir( directorio );
		if( dir.exists() )
		{
			ui->txtSvm_path_sonido->setText( fGrl->setDirRelative(directorio) );
			grlCfg.Svm_path_sonido = ui->txtSvm_path_sonido->text();
	
			fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Svm_path_sonido", grlCfg.Svm_path_sonido);
		}
	}
}

void frmImportPath::on_btnSvm_PathSonido_clear_clicked()
{
	ui->txtSvm_path_sonido->clear();
}

//-- VDMSound
void frmImportPath::on_btnVdms_PathConf_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Guardar archivo como..."), grlDir.Confvdms, ui->txtVdms_path_conf->text(), tr("Todos los archivo") +" (*)", 0, true);

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
		ui->txtVdms_path_conf->setText(f_info.Name +"_"+ fGrl->HoraFechaActual(fGrl->getTime(), "ddMMyyyy_HHmmss") +".vlp");
	else
		ui->txtVdms_path_conf->setText(f_info.Name +".vlp");
}

void frmImportPath::on_btnVdms_PathConf_clear_clicked()
{
	ui->txtVdms_path_conf->clear();
}

void frmImportPath::on_btnVdms_PathExe_clicked()
{
	QString archivo = fGrl->ventanaAbrirArchivos( tr("Selecciona un archivo"), grlCfg.Vdms_path_exe, fGrl->getDirRelative(ui->txtVdms_path_exe->text(), "DosGames"), tr("Todos los archivo") +" (*)");

	stFileInfo f_info = fGrl->getInfoFile( archivo );
	if( f_info.Exists )
	{
		ui->txtVdms_path_exe->setText( fGrl->setDirRelative(archivo, "DosGames") );
		grlCfg.Vdms_path_exe = f_info.Path;

		fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "UltimoDirectorio", "Vdms_path_exe", grlCfg.Vdms_path_exe);
	}
}

void frmImportPath::on_btnVdms_PathExe_clear_clicked()
{
	ui->txtVdms_path_exe->clear();
}
