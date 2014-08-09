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

#include "grlida_update.h"
#include "qtzip.h"
#include "ui_update.h"

frmUpdate::frmUpdate(stGrlCfg m_cfg, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmUpdate)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	url_web = "http://www.gr-lida.org/";
	grlCfg  = m_cfg;

	grlDir.Home    = fGrl->GRlidaHomePath();
	grlDir.Datos   = grlDir.Home +"datos/";
	grlDir.Scripts = grlDir.Home +"scripts/";
	grlDir.Temp    = grlDir.Home +"temp/";
	grlDir.Themes  = grlDir.Home +"themes/";

	cargarConfig();

// centra la aplicacion en el escritorio
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->availableGeometry()));
}

frmUpdate::~frmUpdate()
{
	delete httpdown;
	delete fGrl;
	delete ui;
}

void frmUpdate::cargarConfig()
{
	setWindowIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );

// Configuración del twInfo
	ui->twUpdates->headerItem()->setIcon(0, QIcon(fGrl->Theme() +"img16/tag.png"));
	ui->twUpdates->header()->setMovable(false);
	ui->twUpdates->header()->setStretchLastSection(false);
	ui->twUpdates->header()->setResizeMode(0, QHeaderView::Stretch);
	ui->twUpdates->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->twUpdates->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->twUpdates->setColumnWidth(1, 60);
	ui->twUpdates->setColumnWidth(2, 60);

	ui->btnUpdate->setText(tr("Descargar") +" / "+ tr("Actualizar"));
	ui->btnCheckUpdate->setText(tr("Comprobar actualizaciones"));

	ui->btnClose->setIcon( QIcon(fGrl->Theme() +"img16/aplicar.png") );
	ui->btnUpdate->setIcon( QIcon(fGrl->Theme() +"img16/descarga.png") );
	ui->btnCheckUpdate->setIcon( QIcon(fGrl->Theme() +"img16/actualizar.png") );
	ui->lb_update_ico->setPixmap( QPixmap(fGrl->Theme() +"img16/actualizar.png") );

	ui->chkSoloUpdates->setChecked(grlCfg.SoloUpdates);

	str_html_old = fGrl->leerArchivo(fGrl->ThemeApp() +"tpl_info.html", "UTF-8");

	isUpdates  = false;
	isDownload = false;
	httpdown = new HttpDownload(this);
	httpdown->setHidden(true);
	httpdown->setHttpWindowTitle(tr("Actualizaciones"));
	httpdown->setWindowIcon(QIcon(fGrl->Theme() +"img16/descarga.png"));

	if( grlCfg.ProxyEnable )
		httpdown->setHttpProxy(grlCfg.ProxyType, grlCfg.ProxyHost, grlCfg.ProxyPort.toInt(), grlCfg.ProxyUserName, grlCfg.ProxyPassword);

	connect(httpdown, SIGNAL( statusFinished()       ), this, SLOT( statusFinished()       ) );
	connect(httpdown, SIGNAL( statusBtnEnabled(bool) ), this, SLOT( statusBtnEnabled(bool) ) );
}

void frmUpdate::checkUpdateGrl()
{
	index_fin_descarga = CompruebaUpdates;
	httpdown->downloadFile("http://www.gr-lida.org/lastver.ini", grlDir.Temp +"lastver.ini");
}

void frmUpdate::comprobarActualizaciones()
{
	QString version_grl, info_grl, version, file;
	bool isWin, isMac, isLinux, isWinSelect, isMacSelect, isLinuxSelect, isChecked, isOculto, scriptSelect;
	bool isNuevaVersionGRlida, isNuevaVersionListSvm, isNuevosScript, isNuevosThemes;
	int num_js, num_st;

#ifdef Q_OS_WIN32
	isWin   = true;
	isMac   = false;
	isLinux = false;
	isWinSelect   = true;
	isMacSelect   = false;
	isLinuxSelect = false;
#else
	#ifdef Q_OS_MAC
		isWin   = false;
		isMac   = true;
		isLinux = false;
		isWinSelect   = false;
		isMacSelect   = true;
		isLinuxSelect = false;
	#else
		isWin   = false;
		isMac   = false;
		isLinux = true;
		isWinSelect   = false;
		isMacSelect   = false;
		isLinuxSelect = true;
	#endif
#endif

// Abrimos el fichero lastver.ini para comparar las versiones.
	QSettings settings(grlDir.Temp +"lastver.ini", QSettings::IniFormat);
	settings.beginGroup("update");
		ver_list_svm = settings.value("ver_list_svm", fGrl->versionSvm() ).toString();
		version_grl  = settings.value("version"     , fGrl->versionGrl() ).toString();
		info_grl     = settings.value("info", "").toString();
	settings.endGroup();

// Obtenemos la version de cada script para comparar si tiene actualizacion
	QHash<QString, QString> js_version;
	QList<stGrlDatos> list_js = fGrl->cargarListaArchivos(grlDir.Scripts, CbxListJs, "*.js", ";");
	const int listJsSize = list_js.size();
	if( listJsSize > 0 )
	{
		for (int i = 0; i < listJsSize; ++i)
			js_version.insert(list_js.at(i).key, list_js.at(i).extra);
	}

	settings.beginGroup("update_scripts");
		num_js = settings.value("num_js", 0).toInt();
		isNuevosScript = false;
		if( num_js > 0 )
		{
			for(int i = 0; i < num_js; ++i)
			{
				QString js_i = fGrl->IntToStr(i+1);
				file = settings.value("js_file_"+ js_i).toString();
				if( !js_version.isEmpty() )
				{
					if( js_version.contains(file) )
					{
						if( fGrl->version_compare(js_version[file], settings.value("js_ver_"+ js_i).toString()) )
							isNuevosScript = true;
					}
				}
			}
		}
	settings.endGroup();

// Obtenemos la version de cada themes para comparar si tiene actualizacion
	QHash<QString, QString> st_version;
	QDir dir_st( grlDir.Themes );
	QStringList list_st = dir_st.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
	const int listStSize = list_st.size();
	if( listStSize > 0 )
	{
		for (int i = 0; i < listStSize; ++i)
		{
			if( QFile::exists(grlDir.Themes + list_st.at(i) +"/info.ini") )
			{
				QSettings settings_theme(grlDir.Themes + list_st.at(i) +"/info.ini", QSettings::IniFormat);
				settings_theme.beginGroup("info");
					st_version.insert(list_st.at(i), settings_theme.value("version", "").toString());
				settings_theme.endGroup();
			}
		}
	}

	settings.beginGroup("update_themes");
		num_st = settings.value("num_st", 0).toInt();
		isNuevosThemes = false;
		if( num_st > 0 )
		{
			for(int i = 0; i < num_st; ++i)
			{
				QString st_i = fGrl->IntToStr(i+1);
				file = settings.value("st_titulo_"+ st_i).toString();
				if( !st_version.isEmpty() )
				{
					if( st_version.contains(file) )
					{
						if( fGrl->version_compare(st_version[file], settings.value("st_ver_"+ st_i).toString()) )
							isNuevosThemes = true;
					}
				}
			}
		}
	settings.endGroup();

	isNuevaVersionGRlida  = fGrl->version_compare(fGrl->versionGrl(), version_grl );
	isNuevaVersionListSvm = fGrl->version_compare(grlCfg.VerListSvm , ver_list_svm);

	scriptSelect = true;
	ui->twUpdates->clear();

	if( isNuevaVersionGRlida )
	{
		listUpdateAddCat("GR-lida", fGrl->Theme() +"img16/gr-lida.png");
		listUpdateAddSubCat("GR-lida Windows 32bit" , fGrl->Theme() +"img16/gr-lida.png", fGrl->versionGrl(), version_grl, info_grl, "grl", "GR-lida-"+ version_grl +".exe"        , "gr-lida_win"         , isWin  , isWinSelect  );
		listUpdateAddSubCat("GR-lida Windows 64bit" , fGrl->Theme() +"img16/gr-lida.png", fGrl->versionGrl(), version_grl, info_grl, "grl", "GR-lida-"+ version_grl +"-x64.exe"    , "gr-lida_win64"       , isWin  , false        );
		listUpdateAddSubCat("GR-lida Ubuntu 32bit"  , fGrl->Theme() +"img16/gr-lida.png", fGrl->versionGrl(), version_grl, info_grl, "grl", "gr-lida-"+ version_grl +"-1_i386.deb" , "gr-lida_deb_ubuntu"  , isLinux, isLinuxSelect);
		listUpdateAddSubCat("GR-lida Ubuntu 64bit"  , fGrl->Theme() +"img16/gr-lida.png", fGrl->versionGrl(), version_grl, info_grl, "grl", "gr-lida-"+ version_grl +"-1_amd64.deb", "gr-lida_deb_ubuntu64", isLinux, false        );
		listUpdateAddSubCat("GR-lida Mac OS X intel", fGrl->Theme() +"img16/gr-lida.png", fGrl->versionGrl(), version_grl, info_grl, "grl", "GR-lida-"+ version_grl +"-macosx.dmg" , "gr-lida_mac"         , isMac  , isMacSelect  );
		scriptSelect = false;
	}

	listUpdateAddCat(tr("Lista de compatibilidad del ScummVM"), fGrl->Theme() +"img16/scummvm.png");
	listUpdateAddSubCat(tr("Lista de compatibilidad"), fGrl->Theme() +"img16/scummvm.png", grlCfg.VerListSvm, ver_list_svm, tr("Lista de compatibilidad del ScummVM"), "svm_list", "svm_lista.txt", "", isNuevaVersionListSvm, scriptSelect);
	if( isNuevaVersionListSvm )
		scriptSelect = false;

	if( num_js > 0 )
	{
		settings.beginGroup("update_scripts");
			listUpdateAddCat(tr("Script para importar datos del juego"), fGrl->Theme() +"img16/script.png");

			for(int i = 0; i < num_js; ++i)
			{
				QString js_i = fGrl->IntToStr(i+1);
				file = settings.value("js_file_"+ js_i).toString();
				if( !js_version.isEmpty() )
				{
					if( js_version.contains(file) )
					{
						if( fGrl->version_compare(js_version[file], settings.value("js_ver_"+ js_i).toString()) )
						{
							isChecked = true;
							isOculto  = false;
							version   = js_version[file];
						} else {
							isChecked = false;
							isOculto  = true;
							version   = js_version[file];
						}
					} else {
						isChecked = false;
						isOculto  = true;
						version   = "";
					}
				} else {
					isChecked = false;
					isOculto  = true;
					version   = "";
				}
				listUpdateAddSubCat(settings.value("js_titulo_"+ js_i).toString(), fGrl->Theme() +"img16/script.png",
												version, settings.value("js_ver_"+ js_i).toString(),
												settings.value("js_info_"+ js_i).toString(), "script",
												settings.value("js_file_"+ js_i).toString().remove(".js") +".zip",
												"gr-lida_scripts", isChecked, scriptSelect, isOculto);
				scriptSelect = false;
			}
		settings.endGroup();
	}

	if( num_st > 0 )
	{
		settings.beginGroup("update_themes");
			listUpdateAddCat("Themes GR-lida", fGrl->Theme() +"img16/style.png");

			for(int i = 0; i < num_st; ++i)
			{
				QString st_i = fGrl->IntToStr(i+1);
				file = settings.value("st_titulo_"+ st_i).toString();
				if( !st_version.isEmpty() )
				{
					if( st_version.contains(file) )
					{
						if( fGrl->version_compare(st_version[file], settings.value("st_ver_"+ st_i).toString()) )
						{
							isChecked = true;
							isOculto  = false;
							version   = st_version[file];
						} else {
							isChecked = false;
							isOculto  = true;
							version   = st_version[file];
						}
					} else {
						isChecked = false;
						isOculto  = true;
						version   = "";
					}
				} else {
					isChecked = false;
					isOculto  = true;
					version   = "";
				}
				listUpdateAddSubCat(settings.value("st_titulo_"+ st_i).toString(), fGrl->Theme() +"img16/style.png",
												version, settings.value("st_ver_"+ st_i).toString(),
												settings.value("st_info_"+ st_i).toString(), "themes",
												settings.value("st_file_"+ st_i).toString(),
												"gr-lida_themes", isChecked, scriptSelect, isOculto);
				scriptSelect = false;
			}
		settings.endGroup();
	}

	if( isNuevaVersionGRlida || isNuevaVersionListSvm || isNuevosScript || isNuevosThemes )
		isUpdates = true;
	else {
		isUpdates = false;
		QMessageBox::information(this, "", tr("No ha encontrado ninguna actualización"));
	}
}

void frmUpdate::listUpdateAddCat(QString etiqueta, QString icono)
{
	QFont m_font;
	m_font.setBold(true);

	twListUpdates = new QTreeWidgetItem( ui->twUpdates );
	twListUpdates->setIcon( 0, QIcon(icono) );
	twListUpdates->setFont( 0, m_font   );
	twListUpdates->setText( 0, etiqueta );
	twListUpdates->setText( 1, "" );
	twListUpdates->setText( 2, "" );
	twListUpdates->setText( 3, "" );
	twListUpdates->setText( 4, "" );
	twListUpdates->setText( 5, "" );
	twListUpdates->setText( 6, "" );
	twListUpdates->setText( 7, "" );
	twListUpdates->setExpanded( true );
}

void frmUpdate::listUpdateAddSubCat(QString etiqueta, QString icono, QString version_old, QString version_new, QString descipcion, QString tipo, QString filename, QString url, bool checked, bool select, bool oculto)
{
	QFont m_font;
	if( version_old == version_new || version_old.isEmpty() || !fGrl->version_compare(version_old, version_new) )
	{
		m_font.setBold(false);
		oculto = true;
	} else
		m_font.setBold(true);

	QTreeWidgetItem *sub_cat = new QTreeWidgetItem( twListUpdates );
	sub_cat->setTextAlignment(1, Qt::AlignCenter);
	sub_cat->setTextAlignment(2, Qt::AlignCenter);
	sub_cat->setFont( 2, m_font       );
	sub_cat->setIcon( 0, QIcon(icono) );
	sub_cat->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
	sub_cat->setHidden( grlCfg.SoloUpdates ? oculto : false );
	sub_cat->setText( 0, etiqueta     );
	sub_cat->setText( 1, version_old  );
	sub_cat->setText( 2, version_new  );
	sub_cat->setText( 3, descipcion   );
	sub_cat->setText( 4, tipo         );
	sub_cat->setText( 5, filename     );
	sub_cat->setText( 6, url          );
	sub_cat->setText( 7, oculto ? "oculto" : "visible");

	if( select )
	{
		emit on_twUpdates_itemClicked(sub_cat, 0);

		ui->twUpdates->clearSelection();
		ui->twUpdates->setCurrentItem(sub_cat);
		ui->twUpdates->setFocus();
	}
}

void frmUpdate::downloadFile(int id_down)
{
	isDownload = true;
	index_fin_descarga = CopiarArchivo;
	httpdown->downloadFile(grlUpdate[id_down].url, grlDir.Temp + grlUpdate[id_down].archivo, "GET");
}

void frmUpdate::statusBtnEnabled(bool estado)
{
	ui->btnUpdate->setEnabled(estado);
}

void frmUpdate::statusFinished()
{
	switch( index_fin_descarga )
	{
		case CompruebaUpdates:
			comprobarActualizaciones();
		break;
		case CopiarArchivo:
		{
			QtZip zip;
			if( grlUpdate[id_descarga].tipo == "script" )
				zip.extractZip(grlDir.Temp + grlUpdate[id_descarga].archivo, grlDir.Scripts);
			if( grlUpdate[id_descarga].tipo == "themes" )
				zip.extractZip(grlDir.Temp + grlUpdate[id_descarga].archivo, grlDir.Themes);
			if( grlUpdate[id_descarga].tipo == "svm_list" )
			{
				fGrl->copiarArchivo(grlDir.Temp + grlUpdate[id_descarga].archivo, grlDir.Datos + grlUpdate[id_descarga].archivo, true);
				fGrl->guardarKeyGRLConfig(grlDir.Home +"GR-lida.conf", "Updates", "VerListSvm", ver_list_svm);
				grlCfg.VerListSvm = ver_list_svm;
			}
			index_fin_descarga = DescargaSiguiente;
			emit statusFinished();
		}
		break;
		case DescargaSiguiente:
		{
			++id_descarga;

			if( id_descarga > total_update )
			{
				isDownload = false;
				checkUpdateGrl();
			} else
				downloadFile(id_descarga);
		}
		break;
	}
}

void frmUpdate::on_btnClose_clicked()
{
	if( isDownload )
		emit httpdown->cancelDownload();
	QDialog::accept();
}

void frmUpdate::on_btnUpdate_clicked()
{
	stUpdates dat;
	int id_update = -1;
	const int listSize = ui->twUpdates->topLevelItemCount();
	grlUpdate.clear();
	for(int i = 0; i < listSize; ++i)
	{
		QTreeWidgetItem *item = ui->twUpdates->topLevelItem(i);
		for(int n = 0; n < item->childCount(); ++n)
		{
			if( item->child(n)->checkState(0) == Qt::Checked && !item->child(n)->isHidden() )
			{
				++id_update;
				dat.tipo    = item->child(n)->text(4);
				dat.archivo = item->child(n)->text(5);
				dat.url     = url_web +"downloads/"+ item->child(n)->text(6) +"/"+ dat.archivo;
				grlUpdate.insert(id_update, dat);
			}
		}
	}
	total_update = id_update;
	id_descarga  = -1;

	if( id_update > -1 )
	{
		index_fin_descarga = DescargaSiguiente;
		emit statusFinished();
	}
}

void frmUpdate::on_btnCheckUpdate_clicked()
{
	checkUpdateGrl();
}

void frmUpdate::on_twUpdates_itemClicked(QTreeWidgetItem *item, int column)
{
	if( item && column > -1 )
	{
		str_html_new = str_html_old;
		str_html_new.replace("<theme>", fGrl->Theme());
		str_html_new.replace("{contenido_info}", item->text(3).replace("\n","<br/>"));
		ui->txtInfoUpdate->setHtml( str_html_new );
	}
}

void frmUpdate::on_chkSoloUpdates_clicked(bool checked)
{
	grlCfg.SoloUpdates = checked;

	const int listSize = ui->twUpdates->topLevelItemCount();
	for(int i = 0; i < listSize; ++i)
	{
		QTreeWidgetItem *item = ui->twUpdates->topLevelItem(i);
		for(int n = 0; n < item->childCount(); ++n)
		{
			if( item->child(n)->text(7) == "oculto" )
				item->child(n)->setHidden( checked );
		}
	}
}
