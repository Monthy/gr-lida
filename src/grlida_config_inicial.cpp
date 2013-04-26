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

#include "grlida_config_inicial.h"
#include "ui_config_inicial.h"

frmConfigInicial::frmConfigInicial(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmConfigInicial)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg = m_cfg;

	grlDir.Home    = fGrl->GRlidaHomePath();
	grlDir.Idiomas = grlDir.Home +"idiomas/";

	fGrl->setTheme(grlCfg.NameDirTheme);
	fGrl->setIdioma(grlCfg.IdiomaSelect);

	cargarConfig();
	setTheme();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmConfigInicial::~frmConfigInicial()
{
	delete fGrl;
	delete ui;
}

void frmConfigInicial::cargarConfig()
{
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, ":/idiomas/");
	fGrl->cargarIdiomasComboBox(ui->cbxIdioma, grlDir.Idiomas, false);

	int idx = ui->cbxIdioma->findData( grlCfg.IdiomaSelect +".qm", Qt::UserRole, Qt::MatchEndsWith);
	ui->cbxIdioma->setCurrentIndex( idx );
	emit on_cbxIdioma_activated( idx );

	ui->chkDOSBoxDisp->setChecked( grlCfg.DOSBoxDisp );
	ui->txtDirDbx->setText( grlCfg.DirDOSBox );
	ui->chkScummVMDisp->setChecked( grlCfg.ScummVMDisp );
	ui->txtDirSvm->setText( grlCfg.DirScummVM );

	ui->chkVDMSoundDisp->setChecked( grlCfg.VDMSoundDisp );
	ui->chkShowNext->setChecked( grlCfg.Primeravez );

	#ifdef Q_OS_WIN32
		ui->chkVDMSoundDisp->setEnabled(true);
	#else
		ui->chkVDMSoundDisp->setEnabled(false);
	#endif
}

void frmConfigInicial::setTheme()
{
	ui->btnOk->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnDirDbx->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDirDbx_find->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->btnDirDbx_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );
	ui->btnDirSvm->setIcon( QIcon(fGrl->Theme() +"img16/carpeta_1.png") );
	ui->btnDirSvm_find->setIcon( QIcon(fGrl->Theme() +"img16/zoom.png") );
	ui->btnDirSvm_clear->setIcon( QIcon(fGrl->Theme() +"img16/limpiar.png") );

	if( grlCfg.font_usar )
		this->setStyleSheet(fGrl->myStyleSheet() +"*{font-family:\""+ grlCfg.font_family +"\";font-size:"+ grlCfg.font_size +"pt;}");
	else
		this->setStyleSheet(fGrl->myStyleSheet());

	this->setStyle(QStyleFactory::create(grlCfg.Style));

	if( grlCfg.StylePalette )
		this->setPalette(QApplication::style()->standardPalette());
	else
		this->setPalette(QApplication::palette());
}

void frmConfigInicial::on_cbxIdioma_activated(int index)
{
	if( index > -1 )
	{
		QString idioma = ui->cbxIdioma->itemData(index).toString();
		translator.load( idioma );

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

void frmConfigInicial::on_btnDirDbx_clicked()
{
	ui->txtDirDbx->setText( fGrl->ventanaAbrirArchivos( tr("Selecciona el ejecutable del DOSBox"), grlCfg.DirDbx, ui->txtDirDbx->text(), "DOSBox (dosbox.exe dosbox);;"+ tr("Todos los archivo") +" (*)") );

	if( QFile::exists( ui->txtDirDbx->text() ) )
		grlCfg.DirDbx = fGrl->getInfoFile( ui->txtDirDbx->text() ).Path;
}

void frmConfigInicial::on_btnDirDbx_find_clicked()
{
#ifdef Q_OS_WIN32
	QStringList lista_drivers;
	QFileInfoList list_drives = QDir::drives();
	lista_drivers.clear();
	for (int i = 0; i < list_drives.size(); ++i)
		lista_drivers.insert(i, QDir::toNativeSeparators(list_drives.at(i).absoluteFilePath()));

	bool ok = false;
	QString letra_drive = QInputDialog::getItem(this, tr("Buscar") +" DOSBox", tr("Selecciona la letra de la unidad:"), lista_drivers, 0, false, &ok);
	if( ok && !letra_drive.isEmpty() )
		ui->txtDirDbx->setText( fGrl->getFindFile(letra_drive, "dosbox.exe") );
#else
	#ifdef Q_OS_MAC
		ui->txtDirDbx->setText( fGrl->getFindFile("/", "dosbox") );
	#else
		ui->txtDirDbx->setText( fGrl->getFindFile("/usr/bin/", "dosbox") );
	#endif
#endif
}

void frmConfigInicial::on_btnDirDbx_clear_clicked()
{
	ui->txtDirDbx->clear();
}

void frmConfigInicial::on_btnDirSvm_clicked()
{
	ui->txtDirSvm->setText( fGrl->ventanaAbrirArchivos( tr("Selecciona el ejecutable del ScummVM"), grlCfg.DirSvm, ui->txtDirSvm->text(), "ScummVM (scummvm.exe scummvm);;"+ tr("Todos los archivo") +" (*)") );

	if( QFile::exists( ui->txtDirSvm->text() ) )
		grlCfg.DirSvm = fGrl->getInfoFile( ui->txtDirSvm->text() ).Path;
}

void frmConfigInicial::on_btnDirSvm_find_clicked()
{
#ifdef Q_OS_WIN32
	QStringList lista_drivers;
	QFileInfoList list_drives = QDir::drives();
	lista_drivers.clear();
	for (int i = 0; i < list_drives.size(); ++i)
		lista_drivers.insert(i, QDir::toNativeSeparators(list_drives.at(i).absoluteFilePath()));

	bool ok = false;
	QString letra_drive = QInputDialog::getItem(this, tr("Buscar") +" ScummVM", tr("Selecciona la letra de la unidad:"), lista_drivers, 0, false, &ok);
	if( ok && !letra_drive.isEmpty() )
		ui->txtDirSvm->setText( fGrl->getFindFile(letra_drive, "scummvm.exe") );
#else
	#ifdef Q_OS_MAC
		ui->txtDirSvm->setText( fGrl->getFindFile("/", "scummvm") );
	#else
		ui->txtDirSvm->setText( fGrl->getFindFile("/usr/games/", "scummvm") );
	#endif
#endif
}

void frmConfigInicial::on_btnDirSvm_clear_clicked()
{
	ui->txtDirSvm->clear();
}

void frmConfigInicial::on_btnOk_clicked()
{
	grlCfg.DOSBoxDisp   = ui->chkDOSBoxDisp->isChecked();
	grlCfg.DirDOSBox    = ui->txtDirDbx->text();
	grlCfg.ScummVMDisp  = ui->chkScummVMDisp->isChecked();
	grlCfg.DirScummVM   = ui->txtDirSvm->text();
	grlCfg.VDMSoundDisp = ui->chkVDMSoundDisp->isChecked();
	grlCfg.Primeravez   = ui->chkShowNext->isChecked();

	QDialog::accept();
}
