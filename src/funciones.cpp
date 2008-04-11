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

#include "funciones.h"

Funciones::Funciones(){}

Funciones::~Funciones(){}

QString Funciones::get_Plataforma()
{
QString plataforma;
	#ifdef Q_OS_WIN32
		plataforma = "Windows";
	#endif
	#ifdef Q_OS_MACX
		plataforma = "MacX" ;
	#endif
	#ifdef Q_OS_MAC9
		plataforma = "Mac9" ;
	#endif
	#ifdef Q_OS_BSD4 
		plataforma = "BSD 4.4" ;
	#endif
	#ifdef Q_OS_BSDI 
		plataforma = "BSD/OS" ;
	#endif
	#ifdef Q_OS_FREEBSD 
		plataforma = "FreeBSD" ;
	#endif
	#ifdef Q_OS_LINUX 
		plataforma = "Linux" ;
	#endif
	#ifdef Q_OS_LYNX 
		plataforma = "LynxOS" ;
	#endif
	#ifdef Q_OS_MSDOS 
		plataforma = "MS-DOS and Windows" ;
	#endif
	#ifdef Q_OS_NETBSD 
		plataforma = "NetBSD" ;
	#endif
	#ifdef Q_OS_OS2 
		plataforma = "OS/2" ;
	#endif
	#ifdef Q_OS_OPENBSD 
		plataforma = "OpenBSD" ;
	#endif
	#ifdef Q_OS_SOLARIS 
		plataforma = "Sun Solaris" ;
	#endif
//	#ifdef Q_OS_UNIX 
//		plataforma = "UNIX BSD/SYSV" ;
//	#endif

	return plataforma ;
}

// Convierte de Nmero a Texto
QString	Funciones::IntToStr(int num)
{
	QString temp;
	return temp.setNum(num);
}

// Convierte de Texto a Nmero
int Funciones::StrToInt(QString text)
{
	return text.toInt();
}

// Devuelve la hora y la fecha
QString Funciones::HoraFechaActual()
{
	QDateTime dt;
	dt = QDateTime::currentDateTime();
	return dt.toString("dd/MM/yyyy HH:mm:ss");
}

QString Funciones::url_correcta(QString url)
{
	bool url_ok;
    url_ok = url.startsWith("http://");
   	if(url_ok==false) url.prepend("http://");

   	url_ok = url.endsWith("/");
	if(url_ok==false) url.append("/");

	return url;
}

QString Funciones::eliminar_caracteres(QString str)
{
	str.replace(" ", "_");
	str.replace("\\", "");
	str.replace("/", "");
	str.replace(":", "");
	str.replace("*", "");
	str.replace("?", "");
	str.replace("\"", "");
	str.replace("<", "");
	str.replace(">", "");
	str.replace("|", "");

	return str;
}

void Funciones::DeleteItemTree( QTreeWidgetItem * item )
{
   if (!item) return;
   for(int i=item->childCount()-1; i>=0; i--)
   {
     DeleteItemTree(item->child(i));
   }
   delete item;
}

void Funciones::CargarIdiomasCombo(const QString dirLng, QComboBox *myCombobox)
{
	QString tmp_locale;	
	QString filter = "*.qm";
//	QFile temp_lng_internal;
	QDir dir( dirLng );
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort = QDir::Name;
	QFileInfoList entries = dir.entryInfoList(QStringList() << filter, filters, sort);
	foreach (QFileInfo file, entries)
	{
		// pick country and language out of the file name
		QStringList parts = file.baseName().split("_");
		tmp_locale = parts.value(1) + "_" + parts.value(2);

		QLocale locale(tmp_locale);
		QString language = QLocale::languageToString(locale.language());
		QString country  = QLocale::countryToString(locale.country());
		QString namelang = language + " (" + country + ") - "+ tmp_locale;
		
		myCombobox->addItem( namelang );	
	}
}

void Funciones::CargarDatosComboBox(QString Archivo, QComboBox *myCombobox,int num_col, bool idioma_svm)
{
	QStringList cbx_Lista, cbx_ListaTemp;
	QPixmap pixmap;
	QFile file( Archivo );
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			cbx_ListaTemp << in.readLine();

		myCombobox->clear();
		for ( int i = 0; i < cbx_ListaTemp.count(); i++ )
		{
			cbx_Lista = cbx_ListaTemp[i].split( "|" );
			pixmap.load(":/img16/sinimg.png");
			if(idioma_svm==true)
				pixmap.load(":/img_lng/"+cbx_Lista.value(1)+".png");
			else
				pixmap.load(":/img16/"+cbx_Lista.value(1)+".png");
			if( pixmap.isNull() ) pixmap.load(":/img16/sinimg.png");
			
			switch ( num_col )
			{
				case 1: // 1 columna
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0).toLatin1() );
				break;
				case 2: // 2 columna
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0).toLatin1() + " - " + cbx_Lista.value(1) );
				break;
				case 3: // 3 columna
					pixmap.load(":/img16/"+cbx_Lista.value(2)+".png");
					if( pixmap.isNull() ) pixmap.load(":/img16/sinimg.png");
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0).toLatin1() + " - " + cbx_Lista.value(1) );
				break;
		    }	
		}
	}
	file.close();
}

void Funciones::CargarIconosComboBox(QString IconDir, QComboBox *myCombobox, QStringList filters)
{
	QDir dir( IconDir );
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);
	dir.setNameFilters(filters);
	QFileInfoList list = dir.entryInfoList();
	myCombobox->clear();
	myCombobox->addItem( QIcon( ":/img24/emu_datos.png" ), "datos" );
	myCombobox->addItem( QIcon( ":/img24/emu_dbx.png"   ), "dosbox" );
	myCombobox->addItem( QIcon( ":/img24/emu_svm.png"   ), "scummvm" );
	myCombobox->addItem( QIcon( ":/img24/emu_vdms.png"  ), "vdmsound" );
	for (int i = 0; i < list.size(); ++i)
	{
		QFileInfo fileInfo = list.at(i);
		QPixmap pixmap(IconDir + fileInfo.fileName() );
		myCombobox->addItem( QIcon( pixmap ), fileInfo.fileName() );
	}
}

void Funciones::CargarDatosListaSvm(QString Archivo, QTreeWidget *myTreeWidget)
{
// Abrimos la lista de compatibilidad del ScummVM y rellenamos el twScummVM
	QFile file( Archivo );
	QStringList svm_Lista, svm_ListaTemp;
	
	myTreeWidget->clear();
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		while ( !in.atEnd() )
			svm_ListaTemp << in.readLine();

		for ( int i = 0; i < svm_ListaTemp.size(); i++ )
		{
			svm_Lista = svm_ListaTemp[i].split( "|" );
			
			QTreeWidgetItem *item = new QTreeWidgetItem( myTreeWidget );
			item->setText( 0, svm_Lista.value(0) );
			if( svm_Lista.value(1)=="" )
			{
				item->setTextColor(0,QColor(0,0,0));
				item->setFont( 0, QFont("Times", 10, QFont::Bold));
				if( svm_Lista.value(2)=="-1" || svm_Lista.value(2)=="")
					item->setIcon( 0, QIcon(":/imgsvm/svmlist_space.png") );
				else
					item->setIcon( 0, QIcon(":/imgsvm/"+svm_Lista.value(2)+".png") );	
			} else
				item->setIcon( 0, QIcon(":/imgsvm/"+svm_Lista.value(2)+".png") );
			item->setText( 1, svm_Lista.value(1) );
		}
	}
	file.close();
}

//Carga los Smiles en un TreeWidget
QHash<QString, QString> Funciones::Cargar_Smiles(QString Archivo, QTreeWidget *myTreeWidget)
{
	QFile file( Archivo );
	QStringList smiles_Lista, smiles_ListaTemp;
	QHash<QString, QString> listSmailes;

	myTreeWidget->clear();
	listSmailes.clear();
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		while ( !in.atEnd() )
			smiles_ListaTemp << in.readLine();
		for ( int i = 0; i < smiles_ListaTemp.size(); i++ )
		{
			smiles_Lista = smiles_ListaTemp[i].split( "\",\"" );
			listSmailes.insert( smiles_Lista.value(0), smiles_Lista.value(1) );

			QTreeWidgetItem *item_smiles = new QTreeWidgetItem( myTreeWidget );
			item_smiles->setText( 0 , smiles_Lista.value(0)       ) ; // codigo
			item_smiles->setIcon( 0 , QIcon(smiles_Lista.value(1))) ; // imagen
		}
	}
	return listSmailes;
}

//Carga los Smiles en un QHash<QString, QString>
QHash<QString, QString> Funciones::Cargar_Smiles(QString Archivo)
{
	QFile file( Archivo );
	QStringList smiles_Lista, smiles_ListaTemp;
	QHash<QString, QString> listSmailes;

	listSmailes.clear();
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		while ( !in.atEnd() )
			smiles_ListaTemp << in.readLine();
		for ( int i = 0; i < smiles_ListaTemp.size(); i++ )
		{
			smiles_Lista = smiles_ListaTemp[i].split( "\",\"" );
			listSmailes.insert( smiles_Lista.value(0), smiles_Lista.value(1) );
		}
	}
	return listSmailes;
}

//
QStringList Funciones::CargaDatosListas(QString Archivo, QString delimitador)
{
	QFile file( Archivo );
	QStringList str_Lista, str_ListaTemp, listaDatos;
	//QHash<QString, QString> listaDatos;

	listaDatos.clear();
	if (file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		while ( !in.atEnd() )
			str_ListaTemp << in.readLine();
		for ( int i = 0; i < str_ListaTemp.size(); i++ )
		{
			str_Lista = str_ListaTemp[i].split( delimitador );
			//listaDatos.insert( str_Lista.value(0), str_Lista.value(1) );
			listaDatos << str_Lista.value(0);
		}
	}
	return listaDatos;
}

// Remplaza texto por las imagenes de los emoticones
QString Funciones::ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista)
{
	QHash<QString, QString>::const_iterator i_Hash;
//	QString strTempHtml = ui.txtDatos_4->toPlainText();
	for (i_Hash = lista.constBegin(); i_Hash != lista.constEnd(); ++i_Hash)
		str.replace( i_Hash.key(), "<img src=\""+i_Hash.value()+"\" />");
//	{
//		QString strSmaile = i_Hash.value();
//		if( strSmaile.contains(":/smiles", Qt::CaseInsensitive) )
//			strTempHtml.replace( i_Hash.key(), "<img src=\""+i_Hash.value()+"\" />");
//		else
//			strTempHtml.replace( i_Hash.key(), "<img src=\"" + stHomeDir + i_Hash.value()+"\" />");
//	}
	return str;
}

QString Funciones::VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save)
{
	QString archivo;

	if(Open_Save== true)
		archivo = QFileDialog::getSaveFileName(0, caption, dir, filter, selectedFilter);
	else
		archivo = QFileDialog::getOpenFileName(0, caption, dir, filter, selectedFilter);

	if(!archivo.isEmpty())
		return archivo;
	else
		return tmp_dir;
}

QString Funciones::VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir)
{
	QString directorio;
	directorio = QFileDialog::getExistingDirectory(0, caption, dir, QFileDialog::ShowDirsOnly);
	
	if (!directorio.isEmpty())
		return directorio;
	else
		return tmp_dir;
}

void Funciones::CreaIniScummVM(QString dirIni, QHash<QString, QString> conf_Svm)
{
	QSettings settings( dirIni, QSettings::IniFormat );
	settings.clear();
	settings.beginGroup( conf_Svm["game"] );
		settings.setValue("description"		, conf_Svm["description"]	); //
		settings.setValue("gameid"			, conf_Svm["game"]			); // monkey
		settings.setValue("language"		, conf_Svm["language"]		); // es
		settings.setValue("subtitles"		, conf_Svm["subtitles"]		); // true	
		settings.setValue("platform"		, conf_Svm["platform"]		); // pc	
		settings.setValue("gfx_mode"		, conf_Svm["gfx_mode"]		); // 2x
		settings.setValue("render_mode"		, conf_Svm["render_mode"]	); // hercGreen
		settings.setValue("fullscreen"		, conf_Svm["fullscreen"]	); // true
		settings.setValue("aspect_ratio"	, conf_Svm["aspect_ratio"]	); // true
		settings.setValue("path"			, conf_Svm["path"]			);
		settings.setValue("extrapath"		, conf_Svm["path_extra"]	);
		settings.setValue("savepath"		, conf_Svm["path_save"]		);
		settings.setValue("music_driver"	, conf_Svm["music_driver"]	); // adlib
		settings.setValue("enable_gs"		, conf_Svm["enable_gs"]		); // true
		settings.setValue("multi_midi"		, conf_Svm["multi_midi"]	); // true
		settings.setValue("native_mt32"		, conf_Svm["native_mt32"]	); // true
		settings.setValue("master_volume"	, conf_Svm["master_volume"]	); // 192
		settings.setValue("music_volume"	, conf_Svm["music_volume"]	); // 192
		settings.setValue("sfx_volume"		, conf_Svm["sfx_volume"]	); // 192
		settings.setValue("speech_volume"	, conf_Svm["speech_volume"]	); // 192
		settings.setValue("tempo"			, conf_Svm["tempo"]			); // 0
		settings.setValue("talkspeed"		, conf_Svm["talkspeed"]		); // 107
		settings.setValue("cdrom"			, conf_Svm["cdrom"]			); // 0
		settings.setValue("joystick_num"	, conf_Svm["joystick_num"]	); // -1
		settings.setValue("output_rate"		, conf_Svm["output_rate"]	); // 44100
		settings.setValue("midi_gain"		, conf_Svm["midi_gain"]		); // 257
		settings.setValue("copy_protection"	, conf_Svm["copy_protection"]);// false
		settings.setValue("soundfont"		, conf_Svm["sound_font"]	);
	settings.endGroup();
	settings.beginGroup("scummvm");
		settings.setValue("gui_theme","modern");
		settings.setValue("gfx_mode","2x");
		settings.setValue("fullscreen","false");
	settings.endGroup();
}

QStringList Funciones::CreaConfigMontajes(QTreeWidget *myTreeWidget, const QHash<QString, QString> datos)
{
// Creando la configuracion de los distintos Montajes
	QString NombreEXEDbx, DirEXEDbx, chkDbx_loadfix, chkDbx_cerrardbx;
	QString mount_letra_primario, mount_dir_primario, mount_dir, montaje_boot;
	QString mount_type, mount_drive, mount_letter, mount_label, mount_Options, mount_IOCtrl;
	int num_mount = 0;
	bool montaje_IMG = false;
	bool mount_Boot = false;
	QStringList listmontajes;
	QFileInfo fi( datos["path_exe"] );
	
	NombreEXEDbx = fi.fileName(); 		// Nombre del ejecutable
	DirEXEDbx    = QDir::toNativeSeparators( datos["path_exe"] ) ;	// Directorio donde esta
	DirEXEDbx.remove( fi.fileName(), Qt::CaseInsensitive );

// loadfix
	if( datos["opt_loadfix"] == "true" && datos["opt_loadfix_mem"] != "" )
		chkDbx_loadfix = "loadfix -" + datos["opt_loadfix_mem"] + " " + NombreEXEDbx + " " + datos["parametros_exe"];
	else
		chkDbx_loadfix = NombreEXEDbx + " " + datos["parametros_exe"];
// Cerrar DOSBox
	if( datos["opt_cerrar_dbox"]=="true" )
		chkDbx_cerrardbx = "exit";
	else
		chkDbx_cerrardbx = "";

// Montajes
	listmontajes.clear();
	for ( num_mount = 0; num_mount < myTreeWidget->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *item = myTreeWidget->topLevelItem( num_mount );
	// Indicamos el directorio y la letra a montar
		mount_drive  = item->text(0); // Real Drive or Directory or Image ISO, IMA
		mount_letter = item->text(3); // Emulated Drive letter
	// Situa el montaje primario independiente de donde este colocado
		if( item->text(7) == "v")
		{
			mount_letra_primario = mount_letter;
			mount_dir_primario   = QDir::toNativeSeparators( mount_drive );
		} else {
			mount_letra_primario = myTreeWidget->topLevelItem(0)->text(3);
			mount_dir_primario   = QDir::toNativeSeparators( myTreeWidget->topLevelItem(0)->text(0) );
		}
	//Montando las unidades
		if ( item->text(2) != "boot")
		{
			mount_Boot = false;
			if ( item->text(2) == "floppy")
			{
				montaje_IMG = false;
				mount_type = " -t floppy";
			}		
			if ( item->text(2) == "drive" )
			{
				montaje_IMG = false;
				mount_type = " ";
			}
			if ( item->text(2) == "cdrom" )
			{
				montaje_IMG  = false;
				mount_type  = " -t cdrom";
				mount_IOCtrl = " " + item->text(6);
				if ( item->text(5) != "" )
					mount_Options = " " + item->text(5);
				else
					mount_Options = " ";
			} else
				mount_IOCtrl= " ";
		// Montando imagenes de Discos, disquetes y CDs
			if ( item->text(2) == "IMG_floppy")
			{
				montaje_IMG = true;
				mount_type = " -t floppy";
			}
			if ( item->text(2) == "IMG_hdd")
			{
				montaje_IMG = true;
				mount_type = " -t hdd";
			}
			if ( item->text(2) == "IMG_iso")
			{
				montaje_IMG = true;
				mount_type = " -t iso";
			}
		// Etiqueta de las unidades.
			if ( item->text(1) != "" )
				mount_label = " -label " + item->text(1);
			else
				mount_label = "";
					
			if( montaje_IMG == true )
				listmontajes << "imgmount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_label ;
			else
				listmontajes << "mount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_Options + mount_IOCtrl + mount_label ;
		} else {
			mount_Boot = true;
			montaje_boot = "boot \"" + mount_drive + "\"" ;
		}
	}
	mount_dir = DirEXEDbx ;
	mount_dir.remove(mount_dir_primario, Qt::CaseInsensitive);

	if(mount_Boot == false)
	{
		listmontajes << mount_letra_primario + ":";
		listmontajes << "cd " + mount_dir;	
		listmontajes << chkDbx_loadfix;
		listmontajes << chkDbx_cerrardbx;
	} else {
		listmontajes << mount_letra_primario + ":";
		listmontajes << montaje_boot;
	}
	return listmontajes;
}

void Funciones::Exportar_Profile_DFend(QString fileName)
{
	/* Para futura exportacion de este frontend :) */
}
