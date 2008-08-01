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

Funciones::Funciones()
{
	stTheme = ThemeGrl();
}

Funciones::~Funciones(){}

QString Funciones::GRlidaHomePath()
{

	QFile appConfg;
	QString stDirApp, stConfgCurrentPath;
	stDirApp.clear();
	stConfgCurrentPath.clear();
	stConfgCurrentPath = QDir::currentPath()+"/GR-lida.conf";

	if( appConfg.exists( stConfgCurrentPath ) )
	{
		QSettings settings( stConfgCurrentPath, QSettings::IniFormat );
		settings.beginGroup("OpcGeneral");
			stDirApp = settings.value("DirApp", "HomePath").toString().toLower();
		settings.endGroup();
	} else
		stDirApp = "homepath";

	if( stDirApp == "currentpath" )
		return QDir::currentPath()+"/";
	else if( stDirApp == "homepath" )
		return QDir::homePath()+"/.gr-lida/";
	else
		return QDir::homePath()+"/.gr-lida/";

//	return QDir::homePath()+"/.gr-lida/";
}

QString Funciones::ThemeGrl()
{
	QString stDirApp, stNameDirTheme, theme;
	QDir ThemeDir;

	stDirApp = GRlidaHomePath();

	QSettings settings( stDirApp + "/GR-lida.conf", QSettings::IniFormat );
	settings.beginGroup("OpcGeneral");
		stNameDirTheme = settings.value("NameDirTheme", "defecto").toString();
	settings.endGroup();

	if( stNameDirTheme == "defecto" || stNameDirTheme.isEmpty() )
		theme = ":/";
	else {
		if( ThemeDir.exists(stDirApp+"themes/"+ stNameDirTheme +"/") )
			theme = stDirApp+"themes/"+ stNameDirTheme +"/";
		else
			theme = ":/";
	}
	return theme;
}

QString Funciones::get_Plataforma()
{
QString plataforma;
	#ifdef Q_OS_WIN32
		plataforma = "Windows";
	#endif
	#ifdef Q_OS_MACX
		plataforma = "MacX";
	#endif
	#ifdef Q_OS_MAC9
		plataforma = "Mac9";
	#endif
	#ifdef Q_OS_BSD4
		plataforma = "BSD 4.4";
	#endif
	#ifdef Q_OS_BSDI
		plataforma = "BSD/OS";
	#endif
	#ifdef Q_OS_FREEBSD
		plataforma = "FreeBSD";
	#endif
	#ifdef Q_OS_LINUX
		plataforma = "Linux";
	#endif
	#ifdef Q_OS_LYNX
		plataforma = "LynxOS";
	#endif
	#ifdef Q_OS_MSDOS
		plataforma = "MS-DOS and Windows";
	#endif
	#ifdef Q_OS_NETBSD
		plataforma = "NetBSD";
	#endif
	#ifdef Q_OS_OS2
		plataforma = "OS/2";
	#endif
	#ifdef Q_OS_OPENBSD
		plataforma = "OpenBSD";
	#endif
	#ifdef Q_OS_SOLARIS
		plataforma = "Sun Solaris";
	#endif
//	#ifdef Q_OS_UNIX
//		plataforma = "UNIX BSD/SYSV";
//	#endif

	return plataforma;
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

// Convierte Texto "true", "yes" a tipo Bool
bool Funciones::StrToBool(QString text)
{
	text.toLower();
	if(text=="true" || text=="yes" )
		return true;
	else
		return false;
}

QString Funciones::BoolToStr(bool estado, bool type_yes)
{
	if(estado)
	{
		if(type_yes)
			return "yes"; else return "true";
	} else {
		if(type_yes)
			return "no"; else return "false";
	}
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
	bool url_ok,  s_url_ok;
	s_url_ok = url.startsWith("https://");
	if( s_url_ok )
	{
		url_ok = url.endsWith("/");
		if(url_ok==false) url.append("/");
	} else {
		url_ok = url.startsWith("http://");
		if(url_ok==false) url.prepend("http://");

		url_ok = url.endsWith("/");
		if(url_ok==false) url.append("/");
	}

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
	if(!item) return;
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
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			cbx_ListaTemp << in.readLine();

		myCombobox->clear();
		for ( int i = 0; i < cbx_ListaTemp.count(); i++ )
		{
			cbx_Lista = cbx_ListaTemp[i].split( "|" );
			pixmap.load(stTheme+"img16/sinimg.png");
			if(idioma_svm==true)
				pixmap.load(stTheme+"img_lng/"+cbx_Lista.value(1)+".png");
			else
				pixmap.load(stTheme+"img16/"+cbx_Lista.value(1)+".png");
			if( pixmap.isNull() ) pixmap.load(stTheme+"img16/sinimg.png");

			switch ( num_col )
			{
				case 1: // 1 columna
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0) );
				break;
				case 2: // 2 columna
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0) + " - " + cbx_Lista.value(1) );
				break;
				case 3: // 3 columna
					pixmap.load(stTheme+"img16/"+cbx_Lista.value(2)+".png");
					if( pixmap.isNull() ) pixmap.load(stTheme+"img16/sinimg.png");
					myCombobox->addItem( QIcon( pixmap ), cbx_Lista.value(0) + " - " + cbx_Lista.value(1) );
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
	myCombobox->addItem( QIcon( stTheme+"img24/emu_datos.png" ), "datos" );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_dbx.png"   ), "dosbox" );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_svm.png"   ), "scummvm" );
	myCombobox->addItem( QIcon( stTheme+"img24/emu_vdms.png"  ), "vdmsound" );
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
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

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
					item->setIcon( 0, QIcon(stTheme+"imgsvm/svmlist_space.png") );
				else
					item->setIcon( 0, QIcon(stTheme+"imgsvm/"+svm_Lista.value(2)+".png") );
			} else
				item->setIcon( 0, QIcon(stTheme+"imgsvm/"+svm_Lista.value(2)+".png") );
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
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			smiles_ListaTemp << in.readLine();

		for ( int i = 0; i < smiles_ListaTemp.size(); i++ )
		{
			smiles_Lista = smiles_ListaTemp[i].split( "\",\"" );
			listSmailes.insert( smiles_Lista.value(0), smiles_Lista.value(1) );

			QTreeWidgetItem *item_smiles = new QTreeWidgetItem( myTreeWidget );
			item_smiles->setText( 0 , smiles_Lista.value(0)       ); // codigo
			item_smiles->setIcon( 0 , QIcon(smiles_Lista.value(1))); // imagen
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
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

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
	if( file.open(QIODevice::ReadOnly)!=0 )
	{
		QTextStream in(&file);
		in.setCodec("UTF-8");

		while ( !in.atEnd() )
			str_ListaTemp << in.readLine();

		for ( int i = 0; i < str_ListaTemp.size(); i++ )
		{
			str_Lista = str_ListaTemp[i].split( delimitador );
			listaDatos << str_Lista.value(0);
			//listaDatos.insert( str_Lista.value(0), str_Lista.value(1) );
		}
	}
	return listaDatos;
}

// Remplaza texto por las imagenes de los emoticones
QString Funciones::ReemplazaTextoSmiles(QString str, QHash<QString, QString> lista)
{
	QHash<QString, QString>::const_iterator i_Hash;

	for (i_Hash = lista.constBegin(); i_Hash != lista.constEnd(); ++i_Hash)
		str.replace( i_Hash.key(), "<img src=\""+i_Hash.value()+"\" />");

	return str;
}

QString Funciones::VentanaAbrirArchivos(const QString caption, const QString dir, const QString tmp_dir, const QString filter, QString *selectedFilter, bool Open_Save)
{
	QString archivo, base;
	QFileInfo fi;

	fi.setFile( dir );
	if( fi.exists() )
		base = dir;
	else
		base = GRlidaHomePath();

	if(Open_Save== true)
		archivo = QFileDialog::getSaveFileName(0, caption, base, filter, selectedFilter);
	else
		archivo = QFileDialog::getOpenFileName(0, caption, base, filter, selectedFilter);

	if(!archivo.isEmpty())
		return archivo;
	else
		return tmp_dir;
}

QString Funciones::VentanaDirectorios(const QString caption, const QString dir, const QString tmp_dir)
{
	QString directorio;
	directorio = QFileDialog::getExistingDirectory(0, caption, dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

	if(!directorio.isEmpty())
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

void Funciones::Exportar_Profile_GRLida(const QHash<QString, QString> datos, const QHash<QString, QString> datos_emu, QTreeWidget *treeWidget, const QString PathSaveConfg)
{
	QFile file_out( PathSaveConfg );
	if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl;
		out << "<grlida_db version=\"0.1.0\">" << endl;
		out << "  <lista_db>" << endl;
		out << "    <title>Export GR-lida</title>" << endl;
		out << "    <link></link>" << endl;
		out << "    <description></description>" << endl;
		out << "    <lastBuildDate>" << HoraFechaActual() << "</lastBuildDate>" << endl;
		out << "    <language>es-ES</language>" << endl;
		out << "    <copyright>Copyright 2003-2008</copyright>" << endl;

		out << "    <datos>" << endl;
//		out << "      <usuario>" <<  datos["user_name"] << "</usuario>" << endl;
		out << "      <icono>" << datos["icono"] << "</icono>" << endl;
		out << "      <titulo>" <<  datos["titulo"] << "</titulo>" << endl;
		out << "      <subtitulo>" <<  datos["subtitulo"] << "</subtitulo>" << endl;
		out << "      <genero>" << datos["genero"] << "</genero>" << endl;
		out << "      <compania>" << datos["compania"] << "</compania>" << endl;
		out << "      <desarrollador>" << datos["desarrollador"] << "</desarrollador>" << endl;
		out << "      <tema>" << datos["tema"] << "</tema>" << endl;
		out << "      <idioma>" << datos["idioma"] << "</idioma>" << endl;
		out << "      <formato>" << datos["formato"] << "</formato>" << endl;
		out << "      <anno>" <<  datos["anno"] << "</anno>" << endl;
		out << "      <numdisc>" <<  datos["numdisc"] << "</numdisc>" << endl;
		out << "      <sistemaop>" << datos["sistemaop"] << "</sistemaop>" << endl;
		out << "      <tamano>" << datos["tamano"] << "</tamano>" << endl;
		out << "      <graficos>" << datos["graficos"] << "</graficos>" << endl;
		out << "      <sonido>" << datos["sonido"] << "</sonido>" << endl;
		out << "      <jugabilidad>" << datos["jugabilidad"] << "</jugabilidad>" << endl;
		out << "      <original>" << datos["original"] << "</original>" << endl;
		out << "      <estado>" << datos["estado"] << "</estado>" << endl;
		out << "      <thumbs>" << datos["thumbs"] << "</thumbs>" << endl;
		out << "      <cover_front>" << datos["cover_front"] << "</cover_front>" << endl;
		out << "      <cover_back>" << datos["cover_back"] << "</cover_back>" << endl;
		out << "      <fecha>" << datos["fecha"] << "</fecha>" << endl;
		out << "      <tipo_emu>" << datos["tipo_emu"] << "</tipo_emu>" << endl;
		out << "      <comentario><![CDATA[" << datos["comentario"] << "]]></comentario>" << endl;
		out << "      <favorito>" << datos["favorito"] << "</favorito>" << endl;
		out << "    </datos>" << endl;

		if( datos["tipo_emu"] == "dosbox" )
		{
			out << "    <dosbox>" << endl;
			//out << "      <usuario>" <<  datos_emu["user_name"] << "</usuario>" << endl;
			out << "      <sdl_fullscreen>" << datos_emu["sdl_fullscreen"] << "</sdl_fullscreen>" << endl;
			out << "      <sdl_fulldouble>" << datos_emu["sdl_fulldouble"] << "</sdl_fulldouble>" << endl;
			out << "      <sdl_fullfixed>" << datos_emu["sdl_fullfixed"] << "</sdl_fullfixed>" << endl;
			out << "      <sdl_fullresolution>" << datos_emu["sdl_fullresolution"] << "</sdl_fullresolution>" << endl;
			out << "      <sdl_windowresolution>" << datos_emu["sdl_windowresolution"] << "</sdl_windowresolution>" << endl;
			out << "      <sdl_output>" << datos_emu["sdl_output"] << "</sdl_output>" << endl;
			out << "      <sdl_hwscale>" << datos_emu["sdl_hwscale"] << "</sdl_hwscale>" << endl;
			out << "      <sdl_autolock>" << datos_emu["sdl_autolock"] << "</sdl_autolock>" << endl;
			out << "      <sdl_sensitivity>" << datos_emu["sdl_sensitivity"] << "</sdl_sensitivity>" << endl;
			out << "      <sdl_waitonerror>" << datos_emu["sdl_waitonerror"] << "</sdl_waitonerror>" << endl;
			out << "      <sdl_priority>" << datos_emu["sdl_priority"] << "</sdl_priority>" << endl;
			out << "      <sdl_mapperfile>" << datos_emu["sdl_mapperfile"] << "</sdl_mapperfile>" << endl;
			out << "      <sdl_usescancodes>" << datos_emu["sdl_usescancodes"] << "</sdl_usescancodes>" << endl;
			out << "      <dosbox_language>" << datos_emu["dosbox_language"] << "</dosbox_language>" << endl;
			out << "      <dosbox_machine>" << datos_emu["dosbox_machine"] << "</dosbox_machine>" << endl;
			out << "      <dosbox_captures>" << datos_emu["dosbox_captures"] << "</dosbox_captures>" << endl;
			out << "      <dosbox_memsize>" << datos_emu["dosbox_memsize"] << "</dosbox_memsize>" << endl;
			out << "      <render_frameskip>" << datos_emu["render_frameskip"] << "</render_frameskip>" << endl;
			out << "      <render_aspect>" << datos_emu["render_aspect"] << "</render_aspect>" << endl;
			out << "      <render_scaler>" << datos_emu["render_scaler"] << "</render_scaler>" << endl;
			out << "      <cpu_core>" << datos_emu["cpu_core"] << "</cpu_core>" << endl;
			out << "      <cpu_cycles>" << datos_emu["cpu_cycles"] << "</cpu_cycles>" << endl;
			out << "      <cpu_cycleup>" << datos_emu["cpu_cycleup"] << "</cpu_cycleup>" << endl;
			out << "      <cpu_cycledown>" << datos_emu["cpu_cycledown"] << "</cpu_cycledown>" << endl;
			out << "      <mixer_nosound>" << datos_emu["mixer_nosound"] << "</mixer_nosound>" << endl;
			out << "      <mixer_rate>" << datos_emu["mixer_rate"] << "</mixer_rate>" << endl;
			out << "      <mixer_blocksize>" << datos_emu["mixer_blocksize"] << "</mixer_blocksize>" << endl;
			out << "      <mixer_prebuffer>" << datos_emu["mixer_prebuffer"] << "</mixer_prebuffer>" << endl;
			out << "      <midi_mpu401>" << datos_emu["midi_mpu401"] << "</midi_mpu401>" << endl;
			out << "      <midi_intelligent>" << datos_emu["midi_intelligent"] << "</midi_intelligent>" << endl;
			out << "      <midi_device>" << datos_emu["midi_device"] << "</midi_device>" << endl;
			out << "      <midi_config>" << datos_emu["midi_config"] << "</midi_config>" << endl;
			out << "      <midi_mt32rate>" << datos_emu["midi_mt32rate"] << "</midi_mt32rate>" << endl;
			out << "      <sblaster_sbtype>" << datos_emu["sblaster_sbtype"] << "</sblaster_sbtype>" << endl;
			out << "      <sblaster_sbbase>" << datos_emu["sblaster_sbbase"] << "</sblaster_sbbase>" << endl;
			out << "      <sblaster_irq>" << datos_emu["sblaster_irq"] << "</sblaster_irq>" << endl;
			out << "      <sblaster_dma>" << datos_emu["sblaster_dma"] << "</sblaster_dma>" << endl;
			out << "      <sblaster_hdma>" << datos_emu["sblaster_hdma"] << "</sblaster_hdma>" << endl;
			out << "      <sblaster_mixer>" << datos_emu["sblaster_mixer"] << "</sblaster_mixer>" << endl;
			out << "      <sblaster_oplmode>" << datos_emu["sblaster_oplmode"] << "</sblaster_oplmode>" << endl;
			out << "      <sblaster_oplrate>" << datos_emu["sblaster_oplrate"] << "</sblaster_oplrate>" << endl;
			out << "      <gus_gus>" << datos_emu["gus_gus"] << "</gus_gus>" << endl;
			out << "      <gus_gusrate>" << datos_emu["gus_gusrate"] << "</gus_gusrate>" << endl;
			out << "      <gus_gusbase>" << datos_emu["gus_gusbase"] << "</gus_gusbase>" << endl;
			out << "      <gus_irq1>" << datos_emu["gus_irq1"] << "</gus_irq1>" << endl;
			out << "      <gus_irq2>" << datos_emu["gus_irq2"] << "</gus_irq2>" << endl;
			out << "      <gus_dma1>" << datos_emu["gus_dma1"] << "</gus_dma1>" << endl;
			out << "      <gus_dma2>" << datos_emu["gus_dma2"] << "</gus_dma2>" << endl;
			out << "      <gus_ultradir>" << datos_emu["gus_ultradir"] << "</gus_ultradir>" << endl;
			out << "      <speaker_pcspeaker>" << datos_emu["speaker_pcspeaker"] << "</speaker_pcspeaker>" << endl;
			out << "      <speaker_pcrate>" << datos_emu["speaker_pcrate"] << "</speaker_pcrate>" << endl;
			out << "      <speaker_tandy>" << datos_emu["speaker_tandy"] << "</speaker_tandy>" << endl;
			out << "      <speaker_tandyrate>" << datos_emu["speaker_tandyrate"] << "</speaker_tandyrate>" << endl;
			out << "      <speaker_disney>" << datos_emu["speaker_disney"] << "</speaker_disney>" << endl;
			out << "      <joystick_type>" << datos_emu["joystick_type"] << "</joystick_type>" << endl;
			out << "      <joystick_timed>" << datos_emu["joystick_timed"] << "</joystick_timed>" << endl;
			out << "      <joystick_autofire>" << datos_emu["joystick_autofire"] << "</joystick_autofire>" << endl;
			out << "      <joystick_swap34>" << datos_emu["joystick_swap34"] << "</joystick_swap34>" << endl;
			out << "      <joystick_buttonwrap>" << datos_emu["joystick_buttonwrap"] << "</joystick_buttonwrap>" << endl;
			out << "      <modem_modem>" << datos_emu["modem_modem"] << "</modem_modem>" << endl;
			out << "      <modem_comport>" << datos_emu["modem_comport"] << "</modem_comport>" << endl;
			out << "      <modem_listenport>" << datos_emu["modem_listenport"] << "</modem_listenport>" << endl;
			out << "      <dserial_directserial>" << datos_emu["dserial_directserial"] << "</dserial_directserial>" << endl;
			out << "      <dserial_comport>" << datos_emu["dserial_comport"] << "</dserial_comport>" << endl;
			out << "      <dserial_realport>" << datos_emu["dserial_realport"] << "</dserial_realport>" << endl;
			out << "      <dserial_defaultbps>" << datos_emu["dserial_defaultbps"] << "</dserial_defaultbps>" << endl;
			out << "      <dserial_parity>" << datos_emu["dserial_parity"] << "</dserial_parity>" << endl;
			out << "      <dserial_bytesize>" << datos_emu["dserial_bytesize"] << "</dserial_bytesize>" << endl;
			out << "      <dserial_stopbit>" << datos_emu["dserial_stopbit"] << "</dserial_stopbit>" << endl;
			out << "      <serial_1>" << datos_emu["serial_1"] << "</serial_1>" << endl;
			out << "      <serial_2>" << datos_emu["serial_2"] << "</serial_2>" << endl;
			out << "      <serial_3>" << datos_emu["serial_3"] << "</serial_3>" << endl;
			out << "      <serial_4>" << datos_emu["serial_4"] << "</serial_4>" << endl;
			out << "      <dos_xms>" << datos_emu["dos_xms"] << "</dos_xms>" << endl;
			out << "      <dos_ems>" << datos_emu["dos_ems"] << "</dos_ems>" << endl;
			out << "      <dos_umb>" << datos_emu["dos_umb"] << "</dos_umb>" << endl;
			out << "      <dos_keyboardlayout>" << datos_emu["dos_keyboardlayout"] << "</dos_keyboardlayout>" << endl;
			out << "      <ipx_ipx>" << datos_emu["ipx_ipx"] << "</ipx_ipx>" << endl;
			out << "      <autoexec>" << datos_emu["autoexec"] << "</autoexec>" << endl;
			out << "      <opt_autoexec>" << datos_emu["opt_autoexec"] << "</opt_autoexec>" << endl;
			out << "      <opt_loadfix>" << datos_emu["opt_loadfix"] << "</opt_loadfix>" << endl;
			out << "      <opt_loadfix_mem>" << datos_emu["opt_loadfix_mem"] << "</opt_loadfix_mem>" << endl;
			out << "      <opt_consola_dbox>" << datos_emu["opt_consola_dbox"] << "</opt_consola_dbox>" << endl;
			out << "      <opt_cerrar_dbox>" << datos_emu["opt_cerrar_dbox"] << "</opt_cerrar_dbox>" << endl;
			out << "      <opt_cycle_sincronizar>" << datos_emu["opt_cycle_sincronizar"] << "</opt_cycle_sincronizar>" << endl;
			out << "      <path_conf>" << datos_emu["path_conf"] << "</path_conf>" << endl;
			out << "      <path_sonido>" << datos_emu["path_sonido"] << "</path_sonido>" << endl;
			out << "      <path_exe>" << datos_emu["path_exe"] << "</path_exe>" << endl;
			out << "      <path_setup>" << datos_emu["path_setup"] << "</path_setup>" << endl;
			out << "      <parametros_exe>" << datos_emu["parametros_exe"] << "</parametros_exe>" << endl;
			out << "      <parametros_setup>" << datos_emu["parametros_setup"] << "</parametros_setup>" << endl;
			out << "    </dosbox>" << endl;

			for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
				out << "    <montajes>" << endl;
				out << "      <id_lista>" << item->text(9) << "</id_lista>" << endl;
				out << "      <path>" << item->text(0) << "</path>" << endl;
				out << "      <label>" << item->text(1) << "</label>" << endl;
				out << "      <tipo_as>" << item->text(2) << "</tipo_as>" << endl;
				out << "      <letter>" << item->text(3) << "</letter>" << endl;
				out << "      <indx_cd>" << item->text(4) << "</indx_cd>" << endl;
				out << "      <opt_mount>" << item->text(5) << "</opt_mount>" << endl;
				out << "      <io_ctrl>" << item->text(6) << "</io_ctrl>" << endl;
				out << "      <select_mount>" << item->text(7) << "</select_mount>" << endl;
				out << "    </montajes>" << endl;
			}
		}

		if( datos["tipo_emu"] == "scummvm" )
		{
			out << "    <scummvm>" << endl;
			out << "      <game>" << datos_emu["game"] << "</game>" << endl;
			out << "      <language>" << datos_emu["language"] << "</language>" << endl;
			out << "      <subtitles>" << datos_emu["subtitles"] << "</subtitles>" << endl;
			out << "      <platform>" << datos_emu["platform"] << "</platform>" << endl;
			out << "      <gfx_mode>" << datos_emu["gfx_mode"] << "</gfx_mode>" << endl;
			out << "      <render_mode>" << datos_emu["render_mode"] << "</render_mode>" << endl;
			out << "      <fullscreen>" << datos_emu["fullscreen"] << "</fullscreen>" << endl;
			out << "      <aspect_ratio>" << datos_emu["aspect_ratio"] << "</aspect_ratio>" << endl;
			out << "      <path>" << datos_emu["path"] << "</path>" << endl;
			out << "      <path_setup>" << datos_emu["path_setup"] << "</path_setup>" << endl;
			out << "      <path_extra>" << datos_emu["path_extra"] << "</path_extra>" << endl;
			out << "      <path_save>" << datos_emu["path_save"] << "</path_save>" << endl;
			out << "      <path_capturas>" << datos_emu["path_capturas"] << "</path_capturas>" << endl;
			out << "      <path_sonido>" << datos_emu["path_sonido"] << "</path_sonido>" << endl;
			out << "      <music_driver>" << datos_emu["music_driver"] << "</music_driver>" << endl;
			out << "      <enable_gs>" << datos_emu["enable_gs"] << "</enable_gs>" << endl;
			out << "      <multi_midi>" << datos_emu["multi_midi"] << "</multi_midi>" << endl;
			out << "      <native_mt32>" << datos_emu["native_mt32"] << "</native_mt32>" << endl;
			out << "      <master_volume>" << datos_emu["master_volume"] << "</master_volume>" << endl;
			out << "      <music_volume>" << datos_emu["music_volume"] << "</music_volume>" << endl;
			out << "      <sfx_volume>" << datos_emu["sfx_volume"] << "</sfx_volume>" << endl;
			out << "      <speech_volume>" << datos_emu["speech_volume"] << "</speech_volume>" << endl;
			out << "      <tempo>" << datos_emu["tempo"] << "</tempo>" << endl;
			out << "      <talkspeed>" << datos_emu["talkspeed"] << "</talkspeed>" << endl;
			out << "      <debuglevel>" << datos_emu["debuglevel"] << "</debuglevel>" << endl;
			out << "      <cdrom>" << datos_emu["cdrom"] << "</cdrom>" << endl;
			out << "      <joystick_num>" << datos_emu["joystick_num"] << "</joystick_num>" << endl;
			out << "      <output_rate>" << datos_emu["output_rate"] << "</output_rate>" << endl;
			out << "      <midi_gain>" << datos_emu["midi_gain"] << "</midi_gain>" << endl;
			out << "      <copy_protection>" << datos_emu["copy_protection"] << "</copy_protection>" << endl;
			out << "      <sound_font>" << datos_emu["sound_font"] << "</sound_font>" << endl;
			out << "    </scummvm>" << endl;
		}

		out << "  </lista_db>" << endl;
		out << "</grlida_db>" << endl;
	}
}

void Funciones::CrearArchivoConfigDbx(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg, bool ExportToDFend)
{
	QFile file_out( PathSaveConfg );
	if ( file_out.open(QIODevice::WriteOnly | QIODevice::Text) )
	{
		QTextStream out(&file_out);
		out.setCodec("UTF-8");

		if( ExportToDFend )
		{
			out << "[Extra]" << endl;
			out << "Exe=" << datosDbx["path_exe"] << endl;
			//out << "Environment=" << datosDbx[""] << endl;
			//out << "Tab=" << datosDbx[""] << endl;
			//out << "Tab2=" << datosDbx[""] << endl;
			out << "Loadhigh=" << datosDbx["opt_loadfix"] << endl;
			out << "CloseOnExit=" << datosDbx["opt_cerrar_dbox"] << endl;
			//out << "AutoMountCDs=" << datosDbx[""] << endl;
			//out << "Overridegamestart=" << datosDbx[""] << endl;
			//out << "OverrideMount=" << datosDbx[""] << endl;
			//out << "LoadFixVal=" << datosDbx["opt_loadfix_mem"] << endl;
			//out << "ExeMD5=" << datosDbx[""] << endl;
			out << "Setup=" << datosDbx["path_setup"] << endl;
			//out << "SetupMD5=" << datosDbx[""] << endl;
			out << "GameParameters=" << datosDbx["parametros_exe"] << endl;
			out << "SetupParameters=" << datosDbx["parametros_setup"] << endl;
			//out << "ExtraDirs=" << datosDbx[""] << endl;
			//out << "LastModification=" << datosDbx[""] << endl;
			//out << "DataDir=" << datosDbx[""] << endl;
			out << "NrOfMounts=" << treeWidget->topLevelItemCount() << endl;
			for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
			{
				QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
				out << num_mount << "=" << item->text(0) << ";" << item->text(2) << ";" << item->text(3) << ";" << item->text(6) << endl;
			}
			out << "autoexec=" << datosDbx["autoexec"] << endl << endl;
			//out << "BootImage=" << datosDbx[""] << endl;
			//out << "Finalization=" << datosDbx[""] << endl;
			//out << "CustomSettings=" << datosDbx[""] << endl;

			QString stNotes;
			stNotes.clear();
			stNotes = datos["comentario"];
			stNotes.replace("<br/>\n","[13][10]").replace("<br/>","[13]").replace("\n","[13]");

			out << "[ExtraInfo]" << endl;
			out << "Genre=" << datos["genero"] << endl;
			out << "Developer=" << datos["desarrollador"] << endl;
			out << "Publisher=" << datos["compania"] << endl;
			out << "Year=" << datos["anno"] << endl;
			out << "Language=" << datos["idioma"] << endl;
			//out << "UserInfo=" << datos[""] << endl;
			out << "Favorite=" << datos["favorito"] << endl;
			out << "Name=" << datos["titulo"] << endl;
			out << "Icon=" << datos["icono"] << endl;
			out << "Notes=" << stNotes << endl << endl;
			//out << "WWW=" << datosDbx[""] << endl;
		}

		out << "# This is the configurationfile for DOSBox 0.72." << endl;
		out << "# Lines starting with a # are commentlines." << endl;
		out << "# They are used to (briefly) document the effect of each option." << endl << endl;

		out << "[sdl]" << endl;
		out << "# fullscreen -- Start dosbox directly in fullscreen." << endl;
		out << "# fulldouble -- Use double buffering in fullscreen." << endl;
		out << "# fullresolution -- What resolution to use for fullscreen: original or fixed size (e.g. 1024x768)." << endl;
		out << "# windowresolution -- Scale the window to this size IF the output device supports hardware scaling." << endl;
		out << "# output -- What to use for output: surface,overlay,opengl,openglnb,ddraw." << endl;
		out << "# autolock -- Mouse will automatically lock, if you click on the screen." << endl;
		out << "# sensitiviy -- Mouse sensitivity." << endl;
		out << "# waitonerror -- Wait before closing the console if dosbox has an error." << endl;
		out << "# priority -- Priority levels for dosbox: lowest,lower,normal,higher,highest,pause (when not focussed)." << endl;
		out << "#             Second entry behind the comma is for when dosbox is not focused/minimized." << endl;
		out << "# mapperfile -- File used to load/save the key/event mappings from." << endl;
		out << "# usescancodes -- Avoid usage of symkeys, might not work on all operating systems." << endl << endl;

		out << "fullscreen=" << datosDbx["sdl_fullscreen"] << endl;
		out << "fulldouble=" << datosDbx["sdl_fulldouble"] << endl;
		out << "fullfixed=" << datosDbx["sdl_fullfixed"] << endl;					// DOSBox 0.63
		out << "fullresolution=" << datosDbx["sdl_fullresolution"] << endl;
		out << "windowresolution=" << datosDbx["sdl_windowresolution"] << endl;
		out << "output=" << datosDbx["sdl_output"] << endl;
		out << "hwscale=" << datosDbx["sdl_hwscale"] << endl;						// DOSBox 0.63
		out << "autolock=" << datosDbx["sdl_autolock"] << endl;
		out << "sensitivity=" << datosDbx["sdl_sensitivity"] << endl;
		out << "waitonerror=" << datosDbx["sdl_waitonerror"] << endl;
		out << "priority=" << datosDbx["sdl_priority"] << endl;
		out << "mapperfile=" << datosDbx["sdl_mapperfile"] << endl;
		out << "usescancodes=" << datosDbx["sdl_usescancodes"] << endl << endl;

		out << "[dosbox]" << endl;
		out << "# language -- Select another language file." << endl;
		out << "# memsize -- Amount of memory DOSBox has in megabytes." << endl;
		out << "# machine -- The type of machine tries to emulate:hercules,cga,tandy,pcjr,vga." << endl;
		out << "# captures -- Directory where things like wave,midi,screenshot get captured." << endl << endl;

		if( !datosDbx["dosbox_language"].isEmpty() || datosDbx["dosbox_language"]!=" ")
		out << "language=" << datosDbx["dosbox_language"] << endl;
		out << "machine=" << datosDbx["dosbox_machine"] << endl;
		out << "captures=" << datosDbx["dosbox_captures"] << endl;
		out << "memsize=" << datosDbx["dosbox_memsize"] << endl << endl;

		out << "[render]" << endl;
		out << "# frameskip -- How many frames DOSBox skips before drawing one." << endl;
		out << "# aspect -- Do aspect correction, if your output method doesn't support scaling this can slow things down!." << endl;
		out << "# scaler -- Scaler used to enlarge/enhance low resolution modes." << endl;
		out << "#           Supported are none,normal2x,normal3x,advmame2x,advmame3x,hq2x,hq3x," << endl;
		out << "#                         2xsai,super2xsai,supereagle,advinterp2x,advinterp3x," << endl;
		out << "#                         tv2x,tv3x,rgb2x,rgb3x,scan2x,scan3x." << endl;
		out << "#           If forced is appended (like scaler=hq2x forced), the scaler will be used" << endl;
		out << "#           even if the result might not be desired." << endl << endl;

		out << "frameskip=" << datosDbx["render_frameskip"] << endl;
		out << "aspect=" << datosDbx["render_aspect"] << endl;
		out << "scaler=" << datosDbx["render_scaler"] << endl << endl;

		out << "[cpu]" << endl;
		out << "# core -- CPU Core used in emulation: normal,simple,dynamic,auto." << endl;
		out << "#         auto switches from normal to dynamic if appropriate." << endl;
		out << "# cycles -- Amount of instructions DOSBox tries to emulate each millisecond." << endl;
		out << "#           Setting this value too high results in sound dropouts and lags." << endl;
		out << "#           You can also let DOSBox guess the correct value by setting it to max." << endl;
		out << "#           The default setting (auto) switches to max if appropriate." << endl;
		out << "# cycleup   -- Amount of cycles to increase/decrease with keycombo." << endl;
		out << "# cycledown    Setting it lower than 100 will be a percentage." << endl << endl;

		out << "core=" << datosDbx["cpu_core"] << endl;
		out << "cycles=" << datosDbx["cpu_cycles"] << endl;
		out << "cycleup=" << datosDbx["cpu_cycleup"] << endl;
		out << "cycledown=" << datosDbx["cpu_cycledown"] << endl << endl;

		out << "[mixer]" << endl;
		out << "# nosound -- Enable silent mode, sound is still emulated though." << endl;
		out << "# rate -- Mixer sample rate, setting any devices higher than this will" << endl;
		out << "#         probably lower their sound quality." << endl;
		out << "# blocksize -- Mixer block size, larger blocks might help sound stuttering" << endl;
		out << "#              but sound will also be more lagged." << endl;
		out << "# prebuffer -- How many milliseconds of data to keep on top of the blocksize." << endl << endl;

		out << "nosound=" << datosDbx["mixer_nosound"] << endl;
		out << "rate=" << datosDbx["mixer_rate"] << endl;
		out << "blocksize=" << datosDbx["mixer_blocksize"] << endl;
		out << "prebuffer=" << datosDbx["mixer_prebuffer"] << endl << endl;

		out << "[midi]" << endl;
		out << "# mpu401      -- Type of MPU-401 to emulate: none, uart or intelligent." << endl;
		out << "# device      -- Device that will receive the MIDI data from MPU-401." << endl;
		out << "#                This can be default,alsa,oss,win32,coreaudio,none." << endl;
		out << "# config      -- Special configuration options for the device. In Windows put" << endl;
		out << "#                the id of the device you want to use. See README for details." << endl << endl;

		out << "mpu401=" << datosDbx["midi_mpu401"] << endl;
		out << "intelligent=" << datosDbx["midi_intelligent"] << endl;	// DOSBox 0.63
		out << "device=" << datosDbx["midi_device"] << endl;
		out << "config=" << datosDbx["midi_config"] << endl;
		out << "mt32rate=" << datosDbx["midi_mt32rate"] << endl << endl;		// DOSBox 0.63

		out << "[sblaster]" << endl;
		out << "# sbtype -- Type of sblaster to emulate:none,sb1,sb2,sbpro1,sbpro2,sb16." << endl;
		out << "# sbbase,irq,dma,hdma -- The IO/IRQ/DMA/High DMA address of the soundblaster." << endl;
		out << "# mixer -- Allow the soundblaster mixer to modify the DOSBox mixer." << endl;
		out << "# oplmode -- Type of OPL emulation: auto,cms,opl2,dualopl2,opl3." << endl;
		out << "#            On auto the mode is determined by sblaster type." << endl;
		out << "#            All OPL modes are 'Adlib', except for CMS." << endl;
		out << "# oplrate -- Sample rate of OPL music emulation." << endl << endl;

		out << "sbtype=" << datosDbx["sblaster_sbtype"] << endl;
		out << "sbbase=" << datosDbx["sblaster_sbbase"] << endl;
		out << "irq=" << datosDbx["sblaster_irq"] << endl;
		out << "dma=" << datosDbx["sblaster_dma"] << endl;
		out << "hdma=" << datosDbx["sblaster_hdma"] << endl;
		out << "mixer=" << datosDbx["sblaster_mixer"] << endl;
		out << "oplmode=" << datosDbx["sblaster_oplmode"] << endl;
		out << "oplrate=" << datosDbx["sblaster_oplrate"] << endl << endl;

		out << "[gus]" << endl;
		out << "# gus -- Enable the Gravis Ultrasound emulation." << endl;
		out << "# gusbase,irq1,irq2,dma1,dma2 -- The IO/IRQ/DMA addresses of the" << endl;
		out << "#            Gravis Ultrasound. (Same IRQ's and DMA's are OK.)" << endl;
		out << "# gusrate -- Sample rate of Ultrasound emulation." << endl;
		out << "# ultradir -- Path to Ultrasound directory.  In this directory" << endl;
		out << "#             there should be a MIDI directory that contains" << endl;
		out << "#             the patch files for GUS playback.  Patch sets used" << endl;
		out << "#             with Timidity should work fine." << endl << endl;

		out << "gus=" << datosDbx["gus_gus"] << endl;
		out << "gusrate=" << datosDbx["gus_gusrate"] << endl;
		out << "gusbase=" << datosDbx["gus_gusbase"] << endl;
		out << "irq1=" << datosDbx["gus_irq1"] << endl;
		out << "irq2=" << datosDbx["gus_irq2"] << endl;
		out << "dma1=" << datosDbx["gus_dma1"] << endl;
		out << "dma2=" << datosDbx["gus_dma2"] << endl;
		out << "ultradir=" << datosDbx["gus_ultradir"] << endl << endl;

		out << "[speaker]" << endl;
		out << "# pcspeaker -- Enable PC-Speaker emulation." << endl;
		out << "# pcrate -- Sample rate of the PC-Speaker sound generation." << endl;
		out << "# tandy -- Enable Tandy Sound System emulation (off,on,auto)." << endl;
		out << "#          For auto Tandysound emulation is present only if machine is set to tandy." << endl;
		out << "# tandyrate -- Sample rate of the Tandy 3-Voice generation." << endl;
		out << "# disney -- Enable Disney Sound Source emulation. Covox Voice Master and Speech Thing compatible." << endl << endl;

		out << "pcspeaker=" << datosDbx["speaker_pcspeaker"] << endl;
		out << "pcrate=" << datosDbx["speaker_pcrate"] << endl;
		out << "tandy=" << datosDbx["speaker_tandy"] << endl;
		out << "tandyrate=" << datosDbx["speaker_tandyrate"] << endl;
		out << "disney=" << datosDbx["speaker_disney"] << endl << endl;

		out << "[joystick]" << endl;
		out << "# joysticktype -- Type of joystick to emulate: auto (default), none," << endl;
		out << "#                 2axis (supports two joysticks," << endl;
		out << "#                 4axis (supports one joystick, first joystick used)," << endl;
		out << "#                 4axis_2 (supports one joystick, second joystick used)," << endl;
		out << "#                 fcs (Thrustmaster), ch (CH Flightstick)." << endl;
		out << "#                 none disables joystick emulation." << endl;
		out << "#                 auto chooses emulation depending on real joystick(s)." << endl;
		out << "# timed -- enable timed intervals for axis. (false is old style behaviour)." << endl;
		out << "# autofire -- continuously fires as long as you keep the button pressed." << endl;
		out << "# swap34 -- swap the 3rd and the 4th axis. can be useful for certain joysticks." << endl;
		out << "# buttonwrap -- enable button wrapping at the number of emulated buttons." << endl << endl;

		out << "joysticktype=" << datosDbx["joystick_type"] << endl;
		out << "timed=" << datosDbx["joystick_timed"] << endl;
		out << "autofire=" << datosDbx["joystick_autofire"] << endl;
		out << "swap34=" << datosDbx["joystick_swap34"] << endl;
		out << "buttonwrap=" << datosDbx["joystick_buttonwrap"] << endl << endl;

		out << "[modem]" << endl;											// DOSBox 0.63
		out << "modem=" << datosDbx["modem_modem"] << endl;
		out << "comport=" << datosDbx["modem_comport"] << endl;
		out << "listenport=" << datosDbx["modem_listenport"] << endl << endl;

		out << "[directserial]" << endl;									// DOSBox 0.63
		out << "directserial=" << datosDbx["dserial_directserial"] << endl;
		out << "comport=" << datosDbx["dserial_comport"] << endl;
		out << "realport=" << datosDbx["dserial_realport"] << endl;
		out << "defaultbps=" << datosDbx["dserial_defaultbps"] << endl;
		out << "parity=" << datosDbx["dserial_parity"] << endl;
		out << "bytesize=" << datosDbx["dserial_bytesize"] << endl;
		out << "stopbit=" << datosDbx["dserial_stopbit"] << endl << endl;

		out << "[serial]" << endl;
		out << "# serial1-4 -- set type of device connected to com port." << endl;
		out << "#              Can be disabled, dummy, modem, nullmodem, directserial." << endl;
		out << "#              Additional parameters must be in the same line in the form of" << endl;
		out << "#              parameter:value. Parameter for all types is irq." << endl;
		out << "#              for directserial: realport (required), rxdelay (optional)." << endl;
		out << "#              for modem: listenport (optional)." << endl;
		out << "#              for nullmodem: server, rxdelay, txdelay, telnet, usedtr," << endl;
		out << "#                             transparent, port, inhsocket (all optional)." << endl;
		out << "#              Example: serial1=modem listenport:5000" << endl << endl;

		out << "serial1=" << datosDbx["serial_1"] << endl;
		out << "serial2=" << datosDbx["serial_2"] << endl;
		out << "serial3=" << datosDbx["serial_3"] << endl;
		out << "serial4=" << datosDbx["serial_4"] << endl << endl;

		out << "[dos]" << endl;
		out << "# xms -- Enable XMS support." << endl;
		out << "# ems -- Enable EMS support." << endl;
		out << "# umb -- Enable UMB support." << endl;
		out << "# keyboardlayout -- Language code of the keyboard layout (or none)." << endl << endl;

		out << "xms=" << datosDbx["dos_xms"] << endl;
		out << "ems=" << datosDbx["dos_ems"] << endl;
		out << "umb=" << datosDbx["dos_umb"] << endl;
		out << "keyboardlayout=" << datosDbx["dos_keyboardlayout"] << endl << endl;

		out << "[ipx]" << endl;
		out << "# ipx -- Enable ipx over UDP/IP emulation." << endl << endl;

		out << "ipx=" << datosDbx["ipx_ipx"] << endl << endl;

		if( !ExportToDFend )
		{
			out << "[autoexec]" << endl;
			out << "# Lines in this section will be run at startup." << endl << endl;

			if( datosDbx["opt_autoexec"] == "true" )
				out << datosDbx["autoexec"] << endl;
			else {
				// Creando el Autoexec
				QStringList listamontaje;
				QHash<QString, QString> datos_montaje;
				int i = 0;

				datos_montaje.clear();
				datos_montaje["path_exe"]        = datosDbx["path_exe"];
				datos_montaje["parametros_exe"]  = datosDbx["parametros_exe"];
				datos_montaje["opt_loadfix_mem"] = datosDbx["opt_loadfix_mem"];
				datos_montaje["opt_loadfix"]     = datosDbx["opt_loadfix"];
				datos_montaje["opt_cerrar_dbox"] = datosDbx["opt_cerrar_dbox"];

				listamontaje.clear();
				listamontaje = CreaConfigMontajes( treeWidget, datos_montaje );

				for (i = 0; i < listamontaje.size(); ++i)
					out << listamontaje.value( i ) << endl;
			}
		}

		out.flush();
		file_out.close();
	}
}

// Funcion para Poner nombres cortos en DOS.
// Estado: Beta
QString Funciones::getShortPathName(QString longPath)
{
	QString str, shortPath;
	QStringList list, listshortPath;

	list = longPath.split("\\");

	for ( int i = 0; i < list.size(); i++ )
	{
		str = list.value(i);

		if( str.length() > 8 )
			listshortPath << str.replace(" ","").left(6).append("~1");
		else
			listshortPath << str;
	}

	shortPath = listshortPath.join("\\");

	return shortPath;
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

	NombreEXEDbx = fi.fileName();		// Nombre del ejecutable
	DirEXEDbx    = QDir::toNativeSeparators( datos["path_exe"] );	// Directorio donde esta
	DirEXEDbx.replace("/","\\");
	DirEXEDbx.remove( NombreEXEDbx, Qt::CaseInsensitive );

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
		mount_drive  = QDir::toNativeSeparators( item->text(0) ); // Real Drive or Directory or Image ISO, IMA
		mount_letter = item->text(3); // Emulated Drive letter
	// Situa el montaje primario independiente de donde este colocado
		if( item->text(7) == "v")
		{
			mount_letra_primario = mount_letter;
			mount_dir_primario   = mount_drive;
		} else {
			mount_letra_primario = myTreeWidget->topLevelItem(0)->text(3);
			mount_dir_primario   = QDir::toNativeSeparators( myTreeWidget->topLevelItem(0)->text(0) );
		}
		mount_dir_primario.replace("/","\\");

	//Montando las unidades
		if( item->text(2) != "boot")
		{
			mount_Boot = false;
			if( item->text(2) == "floppy")
			{
				montaje_IMG = false;
				mount_type = " -t floppy";
			}
			if( item->text(2) == "drive" )
			{
				montaje_IMG = false;
				mount_type = " ";
			}
			if( item->text(2) == "cdrom" )
			{
				montaje_IMG  = false;
				mount_type  = " -t cdrom";
				mount_IOCtrl = " " + item->text(6);
				if( item->text(5) != "" )
					mount_Options = " " + item->text(5);
				else
					mount_Options = " ";
			} else
				mount_IOCtrl= " ";
		// Montando imagenes de Discos, disquetes y CDs
			if( item->text(2) == "IMG_floppy")
			{
				montaje_IMG = true;
				mount_type = " -t floppy";
			}
			if( item->text(2) == "IMG_hdd")
			{
				montaje_IMG = true;
				mount_type = " -t hdd";
			}
			if( item->text(2) == "IMG_iso")
			{
				montaje_IMG = true;
				mount_type = " -t iso";
			}
		// Etiqueta de las unidades.
			if( item->text(1) != "" )
				mount_label = " -label " + item->text(1);
			else
				mount_label = "";

			if( montaje_IMG == true )
				listmontajes << "imgmount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_label;
			else
				listmontajes << "mount " + mount_letter + " \"" + mount_drive + "\"" + mount_type + mount_Options + mount_IOCtrl + mount_label;
		} else {
			mount_Boot = true;
			montaje_boot = "boot \"" + mount_drive + "\"";
		}
	}
	mount_dir = DirEXEDbx;
	mount_dir.remove(mount_dir_primario, Qt::CaseInsensitive);

	if(mount_Boot == false)
	{
		listmontajes << mount_letra_primario + ":";
		listmontajes << "cd " + getShortPathName( mount_dir );
		listmontajes << chkDbx_loadfix;
		listmontajes << chkDbx_cerrardbx;
	} else {
		listmontajes << mount_letra_primario + ":";
		listmontajes << montaje_boot;
		listmontajes << chkDbx_cerrardbx;
	}
	return listmontajes;
}

void Funciones::CrearArchivoConfigVdmS(const QHash<QString, QString> datosVdms, const QString PathSaveConfg)
{
	QSettings * settings = new QSettings( PathSaveConfg, QSettings::IniFormat );
	QFileInfo workdir( datosVdms["path_exe"] );

	settings->beginGroup("program");
		settings->setValue("executable"	, datosVdms["path_exe"]  );
		settings->setValue("workdir"	, workdir.absolutePath() );
		settings->setValue("params"		, datosVdms["program_1"] );
		settings->setValue("icon"		, datosVdms["program_2"] );
	settings->endGroup();

	settings->beginGroup("vdms.debug");
		settings->setValue("useCustomCfg"	, datosVdms["vdms_debug_1"] );
		settings->setValue("customCfg"		, datosVdms["vdms_debug_2"] );
	settings->endGroup();

	settings->beginGroup("winnt.dos");
		settings->setValue("useAutoexec"	, datosVdms["winnt_dos_1"] );
		settings->setValue("autoexec"		, datosVdms["winnt_dos_2"] );
	settings->endGroup();

	settings->beginGroup("winnt.dosbox");
		settings->setValue("exitclose"	, datosVdms["winnt_dosbox_1"] );
		settings->setValue("exitWarn"	, datosVdms["winnt_dosbox_2"] );
		settings->setValue("fastPaste"	, datosVdms["winnt_dosbox_3"] );
	settings->endGroup();

	settings->beginGroup("winnt.storage");
		settings->setValue("useCDROM"	, datosVdms["winnt_storage_1"] );
		settings->setValue("useNetware"	, datosVdms["winnt_storage_2"] );
	settings->endGroup();

	delete settings;
}

void Funciones::Cargar_Profile_DFend_ComboBox(QString dirProfiles, QComboBox *myCombobox)
{
	QDir dir( dirProfiles );
	QString filter = "*.prof";
	QDir::Filters filters = QDir::Files | QDir::Readable;
	QDir::SortFlags sort  = QDir::Name;
	QFileInfoList entries = dir.entryInfoList( QStringList() << filter, filters, sort);

	myCombobox->clear();
	myCombobox->addItem( "<defecto>" );
	foreach (QFileInfo file, entries)
	{
		myCombobox->addItem( file.completeBaseName() );
	}
}

void Funciones::Exportar_Profile_DFend(const QHash<QString, QString> datos, const QHash<QString, QString> datosDbx, QTreeWidget *treeWidget, const QString PathSaveConfg)
{
	CrearArchivoConfigDbx(datos, datosDbx, treeWidget, PathSaveConfg, true);
}

QHash<QString, QString> Funciones::Importar_Profile_DFend(QString fileName)
{
	QString stline, fileTemp, strTemp, info_name_conf;
	QHash<QString, QString> ProfileDFend;
	QStringList list_priority;
	int n=0, num_mounts=0;
	bool str_ok;

	fileTemp = GRlidaHomePath()+"temp_config.prof";

// Leer archivo ------------------------------------------------------
	QFile file_in( fileName );
	file_in.open( QIODevice::ReadOnly | QIODevice::Text );

	QTextStream in(&file_in);
	in.setCodec("UTF-8");

	stline.clear();
	stline = in.readAll();
	stline.replace("\\", "\\\\");
	stline.replace(";", "|");
	stline.replace(",", "--");

	file_in.close();

// Escribir archivo --------------------------------------------------
	QFile file_out( fileTemp );
	file_out.open( QIODevice::WriteOnly | QIODevice::Text );

	QTextStream out(&file_out);
		out.setCodec("UTF-8");
		out << stline;
		out.flush();

	file_out.close();
	stline.clear();

	ProfileDFend.clear();
	QSettings settings( fileTemp , QSettings::IniFormat );

// Datos del juego -----
	settings.beginGroup("ExtraInfo");
		ProfileDFend["genero"]        = settings.value("Genre"    , "").toString().replace("--", ",");
		ProfileDFend["desarrollador"] = settings.value("Developer", "").toString().replace("--", ",");
		ProfileDFend["anno"]          = settings.value("Year"     , "").toString().replace("--", ",");
		ProfileDFend["comentario"]    = settings.value("Notes"    , "").toString().replace("--", ",").replace("[13][10]","<br/>\n").replace("[13]","\n");

		if(settings.value("Favorite","false").toBool())
			ProfileDFend["favorito"] = "true";
		else
			ProfileDFend["favorito"] = "false";

		if( settings.value("Icon", "dosbox").toString().replace("--", ",") !="")
			ProfileDFend["icono"] = settings.value("Icon", "dosbox").toString().replace("--", ",");
		else
			ProfileDFend["icono"] = "dosbox";

		ProfileDFend["compania"] = settings.value("Publisher", "").toString().replace("--", ",");
		ProfileDFend["WWW"]       = settings.value("WWW"      , "").toString().replace("--", ",");
		ProfileDFend["idioma"]  = settings.value("Language" , "").toString().replace("--", ",");
		ProfileDFend["UserInfo"]  = settings.value("UserInfo" , "").toString().replace("--", ",");
		ProfileDFend["titulo"]      = settings.value("Name"     , "").toString().replace("--", ",");
	settings.endGroup();

	ProfileDFend["opt_autoexec"]          = "false";
	ProfileDFend["opt_consola_dbox"]      = "false";
	ProfileDFend["opt_cycle_sincronizar"] = "false";
	ProfileDFend["path_sonido"]           = "";

	info_name_conf = eliminar_caracteres( ProfileDFend["Name"] );
	strTemp = info_name_conf;

	str_ok = strTemp.endsWith(".conf");
	if(str_ok == false)
		strTemp.append(".conf");

	QFile appConfg( QDir::homePath()+"/.gr-lida/confdbx/"+ strTemp );
	if( appConfg.exists() )
	{
		qsrand( QTime(0,0,0).secsTo( QTime::currentTime() ) );
		ProfileDFend["path_conf"] = info_name_conf+"_"+IntToStr( qrand() );
	} else
		ProfileDFend["path_conf"] = strTemp;

	str_ok = ProfileDFend["path_conf"].endsWith(".conf");
	if(str_ok == false)
		ProfileDFend["path_conf"].append(".conf");

	settings.beginGroup("Extra");
		ProfileDFend["path_exe"]         = settings.value("Exe"             ,""  ).toString().replace("--", ",");
		ProfileDFend["path_setup"]       = settings.value("Setup"           ,""  ).toString().replace("--", ",");
		ProfileDFend["parametros_exe"]   = settings.value("GameParameters"  ,""  ).toString().replace("--", ",");
		ProfileDFend["parametros_setup"] = settings.value("SetupParameters" ,""  ).toString().replace("--", ",");
		ProfileDFend["autoexec"]         = settings.value("autoexec"        ,""  ).toString().replace("--", ",");
		ProfileDFend["opt_loadfix_mem"]  = settings.value("LoadFixVal"      ,"64").toString();
		ProfileDFend["NrOfMounts"]       = settings.value("NrOfMounts"      ,"0" ).toString();

		if(settings.value("CloseOnExit","false").toBool())
			ProfileDFend["opt_cerrar_dbox"] = "true"; else ProfileDFend["opt_cerrar_dbox"] = "false";

		if(settings.value("Loadhigh","false").toBool())
			ProfileDFend["opt_loadfix"] = "true"; else ProfileDFend["opt_loadfix"] = "false";

		num_mounts = StrToInt( ProfileDFend["NrOfMounts"] );
		if( num_mounts > 0 )
		{
			for(n=0; n < num_mounts; n++)
				ProfileDFend.insert(IntToStr(n), settings.value(IntToStr(n),"").toString().replace("--",",") );
		}

		//ProfileDFend["ExeMD5"]            = settings.value("ExeMD5"           ,"").toString();
		//ProfileDFend["LastModification"]  = settings.value("LastModification" ,"").toString();
		//ProfileDFend["Environment"]       = settings.value("Environment"      ,"").toString();
		//ProfileDFend["Tab"]               = settings.value("Tab"              ,"").toString();
		//ProfileDFend["Tab2"]              = settings.value("Tab2"             ,"").toString();
		//ProfileDFend["SetupMD5"]          = settings.value("SetupMD5"         ,"").toString();
		//ProfileDFend["DataDir"]           = settings.value("DataDir"          ,"").toString();
		//ProfileDFend["Overridegamestart"] = settings.value("Overridegamestart","").toString();
		//ProfileDFend["OverrideMount"]     = settings.value("OverrideMount"    ,"").toString();
		//ProfileDFend["AutoMountCDs"]      = settings.value("AutoMountCDs"     ,"").toString();
		//ProfileDFend["ExtraDirs"]         = settings.value("ExtraDirs"        ,"").toString();
		//ProfileDFend["0"]                 = settings.value("0"                ,"").toString();
		//ProfileDFend["1"]                 = settings.value("1"                ,"").toString();
		//ProfileDFend["2"]                 = settings.value("2"                ,"").toString();
		//ProfileDFend["3"]                 = settings.value("3"                ,"").toString();
		//ProfileDFend["4"]                 = settings.value("4"                ,"").toString();
		//ProfileDFend["5"]                 = settings.value("5"                ,"").toString();
		//ProfileDFend["6"]                 = settings.value("6"                ,"").toString();
		//ProfileDFend["7"]                 = settings.value("7"                ,"").toString();
		//ProfileDFend["8"]                 = settings.value("8"                ,"").toString();
		//ProfileDFend["9"]                 = settings.value("9"                ,"").toString();
		//ProfileDFend["BootImage"]         = settings.value("BootImage"        ,"").toString();
		//ProfileDFend["CustomSettings"]    = settings.value("CustomSettings"   ,"").toString();
	settings.endGroup();

// Datos del emulador --
	settings.beginGroup("sdl");
		if(settings.value("fullscreen","false").toBool())
			ProfileDFend["sdl_fullscreen"] = "true";
		else
			ProfileDFend["sdl_fullscreen"] = "false";

		if(settings.value("fulldouble","false").toBool())
			ProfileDFend["sdl_fulldouble"] = "true";
		else
			ProfileDFend["sdl_fulldouble"] = "false";

		if(settings.value("fullfixed","false").toBool())
			ProfileDFend["sdl_fullfixed"] = "true";
		else
			ProfileDFend["sdl_fullfixed"] = "false";

		ProfileDFend["sdl_fullresolution"]    = settings.value("fullresolution"  ,"original").toString();
		ProfileDFend["sdl_windowresolution"]  = settings.value("windowresolution","original").toString();
		ProfileDFend["sdl_output"]            = settings.value("output"          ,"surface" ).toString();
		ProfileDFend["sdl_hwscale"]           = settings.value("hwscale"         ,"1.00"    ).toString();

		if(settings.value("autolock","true").toBool())
			ProfileDFend["sdl_autolock"] = "true";
		else
			ProfileDFend["sdl_autolock"] = "false";

		ProfileDFend["sdl_sensitivity"] = settings.value("sensitivity", "100").toString();

		if(settings.value("waitonerror","true").toBool())
			ProfileDFend["sdl_waitonerror"] = "true";
		else
			ProfileDFend["sdl_waitonerror"] = "false";

		//list_priority.clear();
		//list_priority                = settings.value("priority", "higher,normal").toStringList();
		//ProfileDFend["sdl_priority"] = list_priority.value(0)+","+ list_priority.value(1);

		ProfileDFend["sdl_priority"]   = settings.value("priority"  , "higher,normal").toString().replace("--", ",");
		ProfileDFend["sdl_mapperfile"] = settings.value("mapperfile", "mapper.txt"   ).toString().replace("--", ",");

		if(settings.value("usescancodes","true").toBool())
			ProfileDFend["sdl_usescancodes"] = "true";
		else
			ProfileDFend["sdl_usescancodes"] = "false";

	settings.endGroup();

	settings.beginGroup("dosbox");
		ProfileDFend["dosbox_language"] = settings.value("language","").toString().replace("--", ",");
		ProfileDFend["dosbox_machine"]  = settings.value("machine" ,"vga").toString();
		ProfileDFend["dosbox_captures"] = settings.value("captures","capture").toString().replace("--", ",");
		ProfileDFend["dosbox_memsize"]  = settings.value("memsize" ,"16").toString();
	settings.endGroup();
	settings.beginGroup("render");
		ProfileDFend["render_frameskip"] = settings.value("frameskip","0").toString();

		if(settings.value("aspect","false").toBool())
			ProfileDFend["render_aspect"] = "true"; else ProfileDFend["render_aspect"] = "false";

		ProfileDFend["render_scaler"] = settings.value("scaler"   ,"normal2x").toString();
	settings.endGroup();

	settings.beginGroup("cpu");
		ProfileDFend["cpu_core"]      = settings.value("core"     ,"normal").toString();
		ProfileDFend["cpu_cycles"]    = settings.value("cycles"   ,"auto").toString();
		ProfileDFend["cpu_cycleup"]   = settings.value("cycleup"  ,"500").toString();
		ProfileDFend["cpu_cycledown"] = settings.value("cycledown","20").toString();
	settings.endGroup();

	settings.beginGroup("mixer");
		if(settings.value("nosound","false").toBool())
			ProfileDFend["mixer_nosound"] = "true"; else ProfileDFend["mixer_nosound"] = "false";

		ProfileDFend["mixer_rate"]      = settings.value("rate"     ,"22050").toString();
		ProfileDFend["mixer_blocksize"] = settings.value("blocksize","2048").toString();
		ProfileDFend["mixer_prebuffer"] = settings.value("prebuffer","10").toString();
	settings.endGroup();

	settings.beginGroup("midi");
		ProfileDFend["midi_mpu401"]      = settings.value("mpu401"       ,"intelligent").toString();

		if(settings.value("midi_intelligent","true").toBool())
			ProfileDFend["midi_intelligent"] ="true"; else ProfileDFend["midi_intelligent"] ="false";

		ProfileDFend["midi_device"]      = settings.value("device"       ,"default").toString();
		ProfileDFend["midi_config"]      = settings.value("config"       ,"").toString();
		ProfileDFend["midi_mt32rate"]    = settings.value("mt32rate"     ,"").toString();
	settings.endGroup();

	settings.beginGroup("sblaster");
		ProfileDFend["sblaster_sbtype"] = settings.value("sbtype" ,"sb16").toString();
		ProfileDFend["sblaster_sbbase"] = settings.value("sbbase" ,"220").toString();
		ProfileDFend["sblaster_irq"]    = settings.value("irq"    ,"7").toString();
		ProfileDFend["sblaster_dma"]    = settings.value("dma"    ,"1").toString();
		ProfileDFend["sblaster_hdma"]   = settings.value("hdma"   ,"5").toString();

		if(settings.value("mixer","true").toBool())
			ProfileDFend["sblaster_mixer"] = "true"; else ProfileDFend["sblaster_mixer"] = "false";

		ProfileDFend["sblaster_oplmode"] = settings.value("oplmode","auto").toString();
		ProfileDFend["sblaster_oplrate"] = settings.value("oplrate","22050").toString();
	settings.endGroup();

	settings.beginGroup("gus");
		if(settings.value("gus","true").toBool())
			ProfileDFend["gus_gus"] = "true"; else ProfileDFend["gus_gus"] = "false";

		ProfileDFend["gus_gusrate"]   = settings.value("gusrate" ,"22050").toString();
		ProfileDFend["gus_gusbase"]   = settings.value("gusbase" ,"240").toString();
		ProfileDFend["gus_irq1"]      = settings.value("irq1"    ,"5").toString();
		ProfileDFend["gus_irq2"]      = settings.value("irq2"    ,"5").toString();
		ProfileDFend["gus_dma1"]      = settings.value("dma1"    ,"3").toString();
		ProfileDFend["gus_dma2"]      = settings.value("dma2"    ,"3").toString();
		ProfileDFend["gus_ultradir"]  = settings.value("ultradir","C:\\ULTRASND").toString().replace("--",",");
	settings.endGroup();

	settings.beginGroup("speaker");
		if(settings.value("pcspeaker","true").toBool())
			ProfileDFend["speaker_pcspeaker"] = "true"; else ProfileDFend["speaker_pcspeaker"] = "false";

		ProfileDFend["speaker_pcrate"]    = settings.value("pcrate"   ,"22050").toString();
		ProfileDFend["speaker_tandy"]     = settings.value("tandy"    ,"auto").toString();
		ProfileDFend["speaker_tandyrate"] = settings.value("tandyrate","22050").toString();

		if(settings.value("disney","true").toBool())
			ProfileDFend["speaker_disney"] = "true"; else ProfileDFend["speaker_disney"] = "false";
	settings.endGroup();

	settings.beginGroup("joystick");
		ProfileDFend["joystick_type"] = settings.value("joysticktype","auto").toString();

		if(settings.value("timed","true").toBool())
			ProfileDFend["joystick_timed"] = "true"; else ProfileDFend["joystick_timed"] = "false";

		if(settings.value("autofire","false").toBool())
			ProfileDFend["joystick_autofire"] = "true"; else ProfileDFend["joystick_autofire"] = "false";

		if(settings.value("swap34","false").toBool())
			ProfileDFend["joystick_swap34"] = "true"; else ProfileDFend["joystick_swap34"] = "false";

		if(settings.value("buttonwrap","true").toBool())
			ProfileDFend["joystick_buttonwrap"] = "true"; else ProfileDFend["joystick_buttonwrap"] = "false";
	settings.endGroup();

	ProfileDFend["modem_modem"]          = settings.value("modem"       ,"true").toString();
	ProfileDFend["modem_comport"]        = settings.value("comport"     ,"1").toString();
	ProfileDFend["modem_listenport"]     = settings.value("listenport"  ,"23").toString();
	ProfileDFend["dserial_directserial"] = settings.value("directserial","true").toString();
	ProfileDFend["dserial_comport"]      = settings.value("comport"     ,"1").toString();
	ProfileDFend["dserial_realport"]     = settings.value("realport"    ,"COM1").toString();
	ProfileDFend["dserial_defaultbps"]   = settings.value("defaultbps"  ,"1200").toString();
	ProfileDFend["dserial_parity"]       = settings.value("parity"      ,"N").toString();
	ProfileDFend["dserial_bytesize"]     = settings.value("bytesize"    ,"8").toString();
	ProfileDFend["dserial_stopbit"]      = settings.value("stopbit"     ,"1").toString();

	settings.beginGroup("serial");
		ProfileDFend["serial_1"] = settings.value("serial1", "dummy").toString().replace("--",",");
		ProfileDFend["serial_2"] = settings.value("serial2", "dummy").toString().replace("--",",");
		ProfileDFend["serial_3"] = settings.value("serial3", "disabled").toString().replace("--",",");
		ProfileDFend["serial_4"] = settings.value("serial4", "disabled").toString().replace("--",",");
	settings.endGroup();

	settings.beginGroup("dos");
		if(settings.value("xms","true").toBool())
			ProfileDFend["dos_xms"] = "true"; else ProfileDFend["dos_xms"] = "false";

		if(settings.value("ems","true").toBool())
			ProfileDFend["dos_ems"] = "true"; else ProfileDFend["dos_ems"] = "false";

		ProfileDFend["dos_umb"] = settings.value("umb", "true").toString();

		if(settings.value("keyboardlayout","none").toString()=="default")
			ProfileDFend["dos_keyboardlayout"] = "none";
		else
			ProfileDFend["dos_keyboardlayout"] = settings.value("keyboardlayout", "none").toString();

	settings.endGroup();
	settings.beginGroup("ipx");
		if(settings.value("ipx","false").toBool())
			ProfileDFend["ipx_ipx"] = "true"; else ProfileDFend["ipx_ipx"] = "false";
	settings.endGroup();

	if( file_out.exists() )
		file_out.remove();

	return ProfileDFend;
}
