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

#include "dbsql.h"

dbSql::dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, QString db_port, QString lng, QString theme)
{
	isOpenDb = false;

	fGrl = new Funciones;

	dbType = db_type; // QSQLITE, QMYSQL, QPSQL
	if( dbType == "QMYSQL" )
		dbTypeId = "INT(11) UNSIGNED NOT NULL AUTO_INCREMENT";
	else if( dbType == "QPSQL" )
		dbTypeId = "SERIAL";
	else if( dbType == "QSQLITE" )
		dbTypeId = "INTEGER NOT NULL";
	else
		dbTypeId = "INTEGER NOT NULL";

	if( dbType == "QMYSQL" || dbType == "QPSQL" )
	{
		sqldb = QSqlDatabase::addDatabase(dbType, "qt_sql_dbgrl");
		sqldb.setHostName( db_server   );
		sqldb.setDatabaseName( db_name );
		sqldb.setUserName( db_username );
		sqldb.setPassword( db_password );
		sqldb.setPort( db_port.toInt() );
		isOpenDb = sqldb.open();
	} else {
		sqldb = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_dbgrl");
		sqldb.setDatabaseName( db_host );
		isOpenDb = sqldb.open();
	}

	setTheme(theme);
	setIdioma(lng);
}

dbSql::~dbSql()
{
	if( isOpenDb )
		close();

	delete fGrl;
}

void dbSql::setTheme(QString theme)
{
	fGrl->setTheme(theme);
}

void dbSql::setIdioma(QString lng)
{
	fGrl->setIdioma(lng);
}

bool dbSql::close()
{
	sqldb.close();
	sqldb = QSqlDatabase();
	sqldb.close();

	return isOpenDb = false;
}

// Retorna si esta abierta o no la base de datos.
bool dbSql::dbisOpen()
{
	return isOpenDb;
}

// Lista las Tablas de la Base de Datos
QStringList dbSql::listarTablas()
{
	return sqldb.tables(QSql::Tables);
}

// Comprueba si el Query se ha ejecutado bien.
bool dbSql::chequearQuery(QSqlQuery &query)
{
	if( query.lastError().type() != QSqlError::NoError )
	{
		fGrl->ventanaInfo("SQL info", ":/img16/basedatos.png", 400, 300, "SQL:\n"+ query.lastQuery() +"\n\nError:\n"+ query.lastError().text(), true);
		return false;
	}
	return true;
}

QString dbSql::lastInsertId(QSqlQuery &query, QString sql_col)
{
	if( dbType == "QPSQL" )
	{
		query.next();
		return query.record().value(sql_col).toString();
	} else
		return query.lastInsertId().toString();
}

// Cuenta el número de registros que tiene.
int dbSql::getCount(QString tabla, QString sql_where, QString sql_col, QString col_value)
{
	Q_UNUSED(col_value);
	QSqlQuery query(sqldb);
	int numRows = 0;

	if( sql_col.isEmpty() )
		sql_col = "COUNT(*)";

	query.exec("SELECT "+ sql_col +" FROM "+ tabla +" "+ sql_where);
	if( chequearQuery(query) )
	{
		if( sql_col == "COUNT(*)" )
		{
			query.next();
			numRows = query.value(0).toInt();
		} else {
			if( sqldb.driver()->hasFeature(QSqlDriver::QuerySize) )
				numRows = query.size();
			else {
			// this can be very slow
				query.last();
				numRows = query.at() + 1;
			}
/*			do {
				QStringList list = query.record().value(sql_col).toString().split(";", QString::SkipEmptyParts);
				if( list.contains(col_value, Qt::CaseInsensitive) )
					++numRows;
			} while ( query.next() );*/
		}
	} else
		numRows = 0;

	if( numRows < 0 )
		numRows = 0;

	return numRows;
}

// Devuelve los datos correspondientes a la tabla, id y columnas indicadas.
QHash<QString, QString> dbSql::getDatos(QString tabla, QString sql_where, QString sql_col, QString sep)
{
	QSqlQuery query(sqldb);
	QHash<QString, QString> datos;

	query.exec("SELECT "+ sql_col +" FROM "+ tabla +" "+ sql_where);
	if( query.first() )
	{
		QStringList list_col = sql_col.split(sep, QString::SkipEmptyParts);

		datos.clear();
		const int listSize = list_col.size();
		for (int i = 0; i < listSize; ++i)
			datos.insert(list_col.at(i), query.record().value(list_col.value(i)).toString());
	}

	return datos;
}

// Renombrar una Tabla
bool dbSql::renombrarTabla(QString old_name, QString new_name)
{
// Obtenemos las tablas de la Base de Datos.
	QStringList lista_tablas = listarTablas();

	if( lista_tablas.contains(new_name) )
		return false;
	else {
		if( !lista_tablas.contains(old_name, Qt::CaseInsensitive) )
			return false;
		else {
			QSqlQuery query(sqldb);

			query.exec("ALTER TABLE "+ old_name +" RENAME TO "+ new_name +";");

			return chequearQuery(query);
		}
	}
}

bool dbSql::crearTabla(QString tabla)
{
	QString qpsql_oids, qpsql_pkey, qpsql_text;

	if( dbType == "QPSQL" )
	{
		qpsql_pkey = "CONSTRAINT "+ tabla +"_pkey ";
		qpsql_oids = "WITH (OIDS=FALSE)";
		qpsql_text = "TEXT";
	} else {
		qpsql_pkey = "";
		qpsql_oids = "";
		qpsql_text = "LONGTEXT";
	}

	if( tabla.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("CREATE TABLE IF NOT EXISTS "+ tabla +" ("
		"	idgrl					"+ dbTypeId +","
		"	icono					VARCHAR(255) NOT NULL DEFAULT '',"
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	subtitulo				VARCHAR(255) NOT NULL DEFAULT '',"
		"	genero					VARCHAR(255) NOT NULL DEFAULT '',"
		"	compania				VARCHAR(255) NOT NULL DEFAULT '',"
		"	desarrollador			VARCHAR(255) NOT NULL DEFAULT '',"
		"	tema					VARCHAR(255) NOT NULL DEFAULT '',"
		"	grupo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	perspectiva				VARCHAR(255) NOT NULL DEFAULT '',"
		"	idioma					VARCHAR(50) NOT NULL DEFAULT '',"
		"	idioma_voces			VARCHAR(50) NOT NULL DEFAULT '',"
		"	formato					VARCHAR(50) NOT NULL DEFAULT '',"
		"	anno					VARCHAR(4) NOT NULL DEFAULT '',"
		"	numdisc					VARCHAR(50) NOT NULL DEFAULT '',"
		"	sistemaop				VARCHAR(200) NOT NULL DEFAULT '',"
		"	tamano					VARCHAR(50) NOT NULL DEFAULT '',"
		"	graficos				INTEGER NOT NULL DEFAULT 0,"
		"	sonido					INTEGER NOT NULL DEFAULT 0,"
		"	jugabilidad				INTEGER NOT NULL DEFAULT 0,"
		"	original				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	estado					VARCHAR(255) NOT NULL DEFAULT '',"
		"	thumbs					VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_front				VARCHAR(255) NOT NULL DEFAULT '',"
		"	cover_back				VARCHAR(255) NOT NULL DEFAULT '',"
		"	fecha					VARCHAR(50) NOT NULL DEFAULT '',"
		"	tipo_emu				VARCHAR(255) NOT NULL DEFAULT 'datos',"
		"	comentario				"+ qpsql_text +","
		"	favorito				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	rating					INTEGER NOT NULL DEFAULT 0,"
		"	edad_recomendada		VARCHAR(2) NOT NULL DEFAULT 'nd',"
		"	usuario					VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_exe				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_exe			VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_capturas			VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_setup				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_setup		VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey +" PRIMARY KEY(idgrl)"
		")"+ qpsql_oids +";");

		return chequearQuery(query);
	}
}

// Crea las Tablas de la Base de Datos si es Necesario
void dbSql::crearTablas()
{
	QSqlQuery query(sqldb);
	QString qpsql_oids, qpsql_pkey, qpsql_text;
	QStringList lista_tablas = listarTablas();

// Renombramos tabla
	renombrarTabla("dbgrl_file", "dbgrl_files");
	renombrarTabla("dbgrl_url" , "dbgrl_urls" );

// Añade las distintas tablas si no están disponibles
// Tabla principal - dbgrl
	if( !lista_tablas.contains("dbgrl") )
		crearTabla("dbgrl");

	if( dbType == "QPSQL" )
	{
		qpsql_pkey = "";
		qpsql_oids = " WITH (OIDS=FALSE)";
		qpsql_text = "TEXT";
	} else {
		qpsql_pkey = "";
		qpsql_oids = "";
		qpsql_text = "LONGTEXT";
	}

// Tabla - dbgrl_emu_dosbox
	if( !lista_tablas.contains("dbgrl_emu_dosbox") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_emu_dosbox_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_dosbox ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	sdl_fullscreen			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sdl_fulldouble			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sdl_fullfixed			VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	sdl_fullresolution		VARCHAR(15) NOT NULL DEFAULT 'original',"
		"	sdl_windowresolution	VARCHAR(15) NOT NULL DEFAULT 'original',"
		"	sdl_output				VARCHAR(15) NOT NULL DEFAULT 'surface',"
		"	sdl_hwscale				VARCHAR(15) NOT NULL DEFAULT '1.00',"		// DOSBox 0.63
		"	sdl_autolock			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sdl_sensitivity			VARCHAR(3) NOT NULL DEFAULT '100',"
		"	sdl_waitonerror			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sdl_priority			VARCHAR(20) NOT NULL DEFAULT 'higher,normal',"
		"	sdl_mapperfile			VARCHAR(255) NOT NULL DEFAULT 'mapper.txt',"
		"	sdl_usescancodes		VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dosbox_language			VARCHAR(255) NOT NULL DEFAULT '',"
		"	dosbox_machine			VARCHAR(20) NOT NULL DEFAULT 'svga_s3',"
		"	dosbox_captures			VARCHAR(255) NOT NULL DEFAULT 'capture',"
		"	dosbox_memsize			VARCHAR(4) NOT NULL DEFAULT '16',"
		"	dosbox_emu_key			VARCHAR(255) NOT NULL DEFAULT 'dosbox',"
		"	render_frameskip		VARCHAR(2) NOT NULL DEFAULT '0',"
		"	render_aspect			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	render_scaler			VARCHAR(15) NOT NULL DEFAULT 'normal2x',"
		"	cpu_core				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	cpu_cputype				VARCHAR(20) NOT NULL DEFAULT 'auto',"
		"	cpu_cycles				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	cpu_cycles_realmode		VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycles_protmode		VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycles_limitmode	VARCHAR(10) NOT NULL DEFAULT '',"
		"	cpu_cycleup				VARCHAR(10) NOT NULL DEFAULT '10',"
		"	cpu_cycledown			VARCHAR(10) NOT NULL DEFAULT '20',"
		"	mixer_nosound			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	mixer_rate				VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	mixer_blocksize			VARCHAR(10) NOT NULL DEFAULT '1024',"
		"	mixer_prebuffer			VARCHAR(10) NOT NULL DEFAULT '20',"
		"	midi_mpu401				VARCHAR(15) NOT NULL DEFAULT 'intelligent',"
		"	midi_intelligent		VARCHAR(5) NOT NULL DEFAULT 'true',"		// DOSBox 0.63
		"	midi_device				VARCHAR(10) NOT NULL DEFAULT 'default',"
		"	midi_config				VARCHAR(255) NOT NULL DEFAULT '',"
		"	midi_mt32rate			VARCHAR(10) NOT NULL DEFAULT '44100',"		// DOSBox 0.65
		"	sblaster_sbtype			VARCHAR(10) NOT NULL DEFAULT 'sb16',"
		"	sblaster_sbbase			VARCHAR(4) NOT NULL DEFAULT '220',"
		"	sblaster_irq			VARCHAR(4) NOT NULL DEFAULT '7',"
		"	sblaster_dma			VARCHAR(4) NOT NULL DEFAULT '1',"
		"	sblaster_hdma			VARCHAR(4) NOT NULL DEFAULT '5',"
		"	sblaster_mixer			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	sblaster_oplmode		VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	sblaster_oplemu			VARCHAR(10) NOT NULL DEFAULT 'default',"
		"	sblaster_oplrate		VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	gus_gus					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	gus_gusrate				VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	gus_gusbase				VARCHAR(4) NOT NULL DEFAULT '240',"
		"	gus_irq1				VARCHAR(4) NOT NULL DEFAULT '5',"
		"	gus_irq2				VARCHAR(4) NOT NULL DEFAULT '5',"
		"	gus_dma1				VARCHAR(4) NOT NULL DEFAULT '3',"
		"	gus_dma2				VARCHAR(4) NOT NULL DEFAULT '3',"
		"	gus_ultradir			VARCHAR(255) NOT NULL DEFAULT 'C:\\ULTRASND',"
		"	speaker_pcspeaker		VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	speaker_pcrate			VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	speaker_tandy			VARCHAR(5) NOT NULL DEFAULT 'auto',"		// DOSBox 0.65
		"	speaker_tandyrate		VARCHAR(10) NOT NULL DEFAULT '44100',"
		"	speaker_disney			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	joystick_type			VARCHAR(50) NOT NULL DEFAULT 'auto',"		// DOSBox 0.70
		"	joystick_timed			VARCHAR(5) NOT NULL DEFAULT 'true',"		// DOSBox 0.70
		"	joystick_autofire		VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	joystick_swap34			VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	joystick_buttonwrap		VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.70
		"	modem_modem				VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	modem_comport			VARCHAR(4) NOT NULL DEFAULT '2',"			// DOSBox 0.63
		"	modem_listenport		VARCHAR(4) NOT NULL DEFAULT '23',"			// DOSBox 0.63
		"	dserial_directserial	VARCHAR(5) NOT NULL DEFAULT 'false',"		// DOSBox 0.63
		"	dserial_comport			VARCHAR(2) NOT NULL DEFAULT '1',"			// DOSBox 0.63
		"	dserial_realport		VARCHAR(5) NOT NULL DEFAULT 'COM1',"		// DOSBox 0.63
		"	dserial_defaultbps		VARCHAR(5) NOT NULL DEFAULT '1200',"		// DOSBox 0.63
		"	dserial_parity			VARCHAR(2) NOT NULL DEFAULT 'N',"			// DOSBox 0.63
		"	dserial_bytesize		VARCHAR(2) NOT NULL DEFAULT '8',"			// DOSBox 0.63
		"	dserial_stopbit			VARCHAR(2) NOT NULL DEFAULT '1',"			// DOSBox 0.63
		"	serial_1				TEXT,"
		"	serial_2				TEXT,"
		"	serial_3				TEXT,"
		"	serial_4				TEXT,"
		"	dos_xms					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_ems					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_umb					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	dos_keyboardlayout		VARCHAR(5) NOT NULL DEFAULT 'auto',"
		"	dos_version				VARCHAR(5) NOT NULL DEFAULT '',"
		"	ipx_ipx					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	ipx_type				VARCHAR(10) NOT NULL DEFAULT 'none',"
		"	ipx_port				VARCHAR(5) NOT NULL DEFAULT '213',"
		"	ipx_ip					VARCHAR(255) NOT NULL DEFAULT '',"
		"	autoexec				"+ qpsql_text +","
		"	opt_autoexec			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_loadfix				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_loadfix_mem			VARCHAR(5) NOT NULL DEFAULT '64',"
		"	opt_consola_dbox		VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	opt_cerrar_dbox			VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	opt_cycle_sincronizar	VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	path_conf				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_sonido				VARCHAR(255) NOT NULL DEFAULT 'waves',"
		"	path_exe				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_setup				VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_exe			VARCHAR(255) NOT NULL DEFAULT '',"
		"	parametros_setup		VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_dosbox_mount
	if( !lista_tablas.contains("dbgrl_emu_dosbox_mount") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_emu_dosbox_mount_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_dosbox_mount ("
		"	id						"+ dbTypeId +","
		"	id_dosbox				INTEGER NOT NULL DEFAULT 1,"
		"	id_lista				VARCHAR(255) NOT NULL DEFAULT '0',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	label					VARCHAR(255) NOT NULL DEFAULT '',"
		"	tipo_as					VARCHAR(50) NOT NULL DEFAULT 'drive',"
		"	letter					VARCHAR(255) NOT NULL DEFAULT 'C',"
		"	indx_cd					VARCHAR(255) NOT NULL DEFAULT '',"
		"	opt_mount				VARCHAR(255) NOT NULL DEFAULT '',"
		"	io_ctrl					VARCHAR(50) NOT NULL DEFAULT '',"
		"	select_mount			VARCHAR(10) NOT NULL DEFAULT 'x',"
		"	opt_size				VARCHAR(255) NOT NULL DEFAULT '',"
		"	opt_freesize			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	freesize				INTEGER NOT NULL DEFAULT 250,"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_scummvm
	if( !lista_tablas.contains("dbgrl_emu_scummvm") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_emu_scummvm_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_scummvm ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	game					VARCHAR(255) NOT NULL DEFAULT '',"
		"	game_label				VARCHAR(255) NOT NULL DEFAULT '',"
		"	language				VARCHAR(4) NOT NULL DEFAULT 'es',"
		"	subtitles				VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	platform				VARCHAR(50) NOT NULL DEFAULT 'pc',"
		"	gfx_mode				VARCHAR(50) NOT NULL DEFAULT '2x',"
		"	render_mode				VARCHAR(50) NOT NULL DEFAULT '',"
		"	fullscreen				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	aspect_ratio			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_extra				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_save				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_capturas			VARCHAR(255) NOT NULL DEFAULT '',"
		"	path_sonido				VARCHAR(255) NOT NULL DEFAULT '',"
		"	music_driver			VARCHAR(50) NOT NULL DEFAULT 'adlib',"
		"	enable_gs				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	multi_midi				VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	native_mt32				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	mute					VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	master_volume			INTEGER NOT NULL DEFAULT 192,"
		"	music_volume			INTEGER NOT NULL DEFAULT 192,"
		"	sfx_volume				INTEGER NOT NULL DEFAULT 192,"
		"	speech_volume			INTEGER NOT NULL DEFAULT 192,"
		"	speech_mute				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	tempo					INTEGER NOT NULL DEFAULT 100,"
		"	talkspeed				INTEGER NOT NULL DEFAULT 60,"
		"	debuglevel				INTEGER NOT NULL DEFAULT 0,"
		"	cdrom					INTEGER NOT NULL DEFAULT 0,"
		"	joystick_num			INTEGER NOT NULL DEFAULT 0,"
		"	output_rate				VARCHAR(10) NOT NULL DEFAULT '',"
		"	midi_gain				INTEGER NOT NULL DEFAULT 100,"
		"	copy_protection			VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	sound_font				VARCHAR(255) NOT NULL DEFAULT '',"
		"	walkspeed				INTEGER NOT NULL DEFAULT 0,"
		"	opl_driver				VARCHAR(10) NOT NULL DEFAULT 'auto',"
		"	disable_dithering		VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	alt_intro				VARCHAR(5) NOT NULL DEFAULT 'false',"
		"	boot_param				VARCHAR(15) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_emu_vdmsound
	if( !lista_tablas.contains("dbgrl_emu_vdmsound") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_emu_vdmsound_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_emu_vdmsound ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	path_conf				TEXT,"
		"	path_exe				TEXT,"
		"	program					TEXT,"
		"	vdms_debug				TEXT,"
		"	winnt_dos				TEXT,"
		"	winnt_dosbox			TEXT,"
		"	winnt_storage			TEXT,"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_files
	if( !lista_tablas.contains("dbgrl_files") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_files_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_files ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	nombre					VARCHAR(255) NOT NULL DEFAULT '',"
		"	crc						VARCHAR(8) NOT NULL DEFAULT '00000000',"
		"	descripcion				VARCHAR(255) NOT NULL DEFAULT '',"
		"	path					VARCHAR(255) NOT NULL DEFAULT '',"
		"	size					VARCHAR(50) NOT NULL DEFAULT '',"
		"	tipo					VARCHAR(50) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}

// Tabla - dbgrl_urls
	if( !lista_tablas.contains("dbgrl_urls") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_urls_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_urls ("
		"	id						"+ dbTypeId +","
		"	idgrl					INTEGER NOT NULL DEFAULT 1,"
		"	idcat					INTEGER NOT NULL DEFAULT 1,"
		"	url						VARCHAR(255) NOT NULL DEFAULT '',"
		"	descripcion				VARCHAR(255) NOT NULL DEFAULT '',"
		"	"+ qpsql_pkey +" PRIMARY KEY(id)"
		")"+ qpsql_oids +";");

		chequearQuery(query);
	}
// -------------------------------------------------------------------------------------
// Tabla - dbgrl_categorias
	if( !lista_tablas.contains("dbgrl_categorias") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_categorias_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_categorias ("
		"	id						"+ dbTypeId +","
		"	tabla					VARCHAR(255) NOT NULL DEFAULT 'dbgrl',"
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	img						VARCHAR(255) NOT NULL DEFAULT 'sinimg.png',"
		"	orden					INTEGER NOT NULL DEFAULT 0,"
		"	emu_show				TEXT,"
		"	"+ qpsql_pkey +" PRIMARY KEY (id)"
		")"+ qpsql_oids +";");

		if( chequearQuery(query) )
			insertaCategoria("dbgrl", "Db GR-lida", "pc.png", 0, "all");
	}
// Tabla - dbgrl_mnu_nav
	if( !lista_tablas.contains("dbgrl_mnu_nav") )
	{
		if( dbType == "QPSQL" )
			qpsql_pkey = "CONSTRAINT dbgrl_mnu_nav_pkey ";

		query.exec("CREATE TABLE IF NOT EXISTS dbgrl_mnu_nav ("
		"	id						"+ dbTypeId +","
		"	titulo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	col_value				VARCHAR(255) NOT NULL DEFAULT '',"
		"	col_name				VARCHAR(255) NOT NULL DEFAULT '',"
		"	sql_query				TEXT,"
		"	archivo					VARCHAR(255) NOT NULL DEFAULT '',"
		"	img						VARCHAR(255) NOT NULL DEFAULT 'sinimg.png',"
		"	orden					INTEGER NOT NULL DEFAULT 0,"
		"	show					VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	expanded				VARCHAR(5) NOT NULL DEFAULT 'true',"
		"	"+ qpsql_pkey +" PRIMARY KEY (id)"
		")"+ qpsql_oids +";");

		if( chequearQuery(query) )
		{
			insertaMenuNav( tr("Generos")        , "{value}", "genero"       , "WHERE {col_name} LIKE ('%{col_value}%')", "generos.txt"  , "datos_3.png", 0, true, true);
			insertaMenuNav( tr("Temas")          , "{value}", "tema"         , "WHERE {col_name} LIKE ('%{col_value}%')", "temas.txt"    , "datos_3.png", 1, true, true);
			insertaMenuNav( tr("Desarrolladores"), "{value}", "desarrollador", "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 2, true, true);
			insertaMenuNav( tr("Compañias")      , "{value}", "compania"     , "WHERE {col_name} LIKE ('%{col_value}%')", "companias.txt", "datos_3.png", 3, true, true);
			insertaMenuNav( tr("Años")           , "{value}", "anno"         , "WHERE {col_name} LIKE ('%{col_value}%')", "fechas.txt"   , "fecha.png"  , 4, true, true);
			insertaMenuNav( tr("Idiomas")        , "{value}", "idioma"       , "WHERE {col_name} LIKE ('%{col_value}%')", "idiomas.txt"  , "idiomas.png", 5, true, true);
		}
	}
// -------------------------------------------------------------------------------------
// Añade las columnas si no están disponibles
// Tabla - dbgrl
	QSqlQuery query_cat(sqldb);
	query_cat.exec("SELECT tabla FROM dbgrl_categorias;");
	if( chequearQuery(query_cat) )
	{
		if( query_cat.first() )
		{
			do {
				QString tabla = query_cat.record().value("tabla").toString();

				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN grupo VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN perspectiva VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN idioma_voces VARCHAR(50) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN favorito VARCHAR(5) NOT NULL DEFAULT 'false';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN rating INTEGER NOT NULL DEFAULT 0;");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN edad_recomendada VARCHAR(2) NOT NULL DEFAULT 'nd';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN usuario VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN path_exe VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN parametros_exe VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN path_capturas VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN path_setup VARCHAR(255) NOT NULL DEFAULT '';");
				query.exec("ALTER TABLE "+ tabla +" ADD COLUMN parametros_setup VARCHAR(255) NOT NULL DEFAULT '';");
			} while ( query_cat.next() );
		}
	} else {
		query.exec("ALTER TABLE dbgrl ADD COLUMN grupo VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN perspectiva VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN idioma_voces VARCHAR(50) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN favorito VARCHAR(5) NOT NULL DEFAULT 'false';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN rating INTEGER NOT NULL DEFAULT 0;");
		query.exec("ALTER TABLE dbgrl ADD COLUMN edad_recomendada VARCHAR(2) NOT NULL DEFAULT 'nd';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN usuario VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN path_exe VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN parametros_exe VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN path_capturas VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN path_setup VARCHAR(255) NOT NULL DEFAULT '';");
		query.exec("ALTER TABLE dbgrl ADD COLUMN parametros_setup VARCHAR(255) NOT NULL DEFAULT '';");
	}
	query_cat.clear();

// Tabla - dbgrl_emu_dosbox
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cputype VARCHAR(20) NOT NULL DEFAULT 'auto';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN sblaster_oplemu VARCHAR(10) NOT NULL DEFAULT 'default';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_realmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_protmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN cpu_cycles_limitmode VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN dos_version VARCHAR(5) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_type VARCHAR(5) NOT NULL DEFAULT 'none';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_port VARCHAR(5) NOT NULL DEFAULT '213';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN ipx_ip VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox ADD COLUMN dosbox_emu_key VARCHAR(255) NOT NULL DEFAULT 'dosbox';");
// Tabla - dbgrl_emu_dosbox_mount
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN opt_size VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN opt_freesize VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_dosbox_mount ADD COLUMN freesize INTEGER NOT NULL DEFAULT 250;");
// Tabla - dbgrl_emu_scummvm
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN output_rate VARCHAR(10) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN midi_gain INTEGER NOT NULL DEFAULT 100;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN copy_protection VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN sound_font VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN game_label VARCHAR(255) NOT NULL DEFAULT '';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN walkspeed INTEGER NOT NULL DEFAULT 0;");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN opl_driver VARCHAR(10) NOT NULL DEFAULT 'auto';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN disable_dithering VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN mute VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN speech_mute VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN alt_intro VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE dbgrl_emu_scummvm ADD COLUMN boot_param VARCHAR(5) NOT NULL DEFAULT '';");
// Tabla - dbgrl_emu_vdmsound
	query.exec("ALTER TABLE dbgrl_emu_vdmsound ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
// Tabla - dbgrl_files
	query.exec("ALTER TABLE dbgrl_files ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
	query.exec("ALTER TABLE dbgrl_files ADD COLUMN tipo VARCHAR(50) NOT NULL DEFAULT '';");
// Tabla - dbgrl_urls
	query.exec("ALTER TABLE dbgrl_urls ADD COLUMN idcat INTEGER NOT NULL DEFAULT 1;");
}

// INICIO Datos Juego -------------------------------------------------------------------------------------------
stDatosJuego dbSql::show_Datos(QString tabla, QString IDgrl)
{
	QSqlQuery query(sqldb);
	stDatosJuego datos;

	query.exec("SELECT * FROM "+ tabla +" WHERE idgrl="+ IDgrl +" LIMIT 0,1;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			datos.idgrl            = query.record().value("idgrl").toString();
			datos.icono            = query.record().value("icono").toString();
			datos.titulo           = query.record().value("titulo").toString();
			datos.subtitulo        = query.record().value("subtitulo").toString();
			datos.genero           = query.record().value("genero").toString();
			datos.compania         = query.record().value("compania").toString();
			datos.desarrollador    = query.record().value("desarrollador").toString();
			datos.tema             = query.record().value("tema").toString();
			datos.grupo            = query.record().value("grupo").toString();
			datos.perspectiva      = query.record().value("perspectiva").toString();
			datos.idioma           = query.record().value("idioma").toString();
			datos.idioma_voces     = query.record().value("idioma_voces").toString();
			datos.formato          = query.record().value("formato").toString();
			datos.anno             = query.record().value("anno").toString();
			datos.numdisc          = query.record().value("numdisc").toString();
			datos.sistemaop        = query.record().value("sistemaop").toString();
			datos.tamano           = query.record().value("tamano").toString();
			datos.graficos         = query.record().value("graficos").toString();
			datos.sonido           = query.record().value("sonido").toString();
			datos.jugabilidad      = query.record().value("jugabilidad").toString();
			datos.original         = query.record().value("original").toString().toLower();
			datos.estado           = query.record().value("estado").toString();
			datos.thumbs           = query.record().value("thumbs").toString();
			datos.thumbs_new       = false;
			datos.cover_front      = query.record().value("cover_front").toString();
			datos.cover_front_new  = false;
			datos.cover_back       = query.record().value("cover_back").toString();
			datos.cover_back_new   = false;
			datos.fecha            = query.record().value("fecha").toString();
			datos.tipo_emu         = query.record().value("tipo_emu").toString();
			datos.comentario       = query.record().value("comentario").toString();
			datos.favorito         = query.record().value("favorito").toString().toLower();
			datos.rating           = query.record().value("rating").toString();
			datos.edad_recomendada = query.record().value("edad_recomendada").toString();
			datos.usuario          = query.record().value("usuario").toString();
			datos.path_exe         = query.record().value("path_exe").toString();
			datos.parametros_exe   = query.record().value("parametros_exe").toString();
			datos.path_capturas    = query.record().value("path_capturas").toString();
			datos.path_setup       = query.record().value("path_setup").toString();
			datos.parametros_setup = query.record().value("parametros_setup").toString();
		} else
			datos = fGrl->getDefectDatos();
	} else
		datos = fGrl->getDefectDatos();

	return datos;
}

QString dbSql::insertaDatos(QString tabla, stDatosJuego datos)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING idgrl" : "";
	strSQL = "INSERT INTO "+ tabla +" ("
			"icono, titulo, subtitulo, genero, compania, desarrollador, tema, grupo, perspectiva, idioma, idioma_voces, formato, anno, numdisc, "
			"sistemaop, tamano, graficos, sonido, jugabilidad, original, estado, thumbs, cover_front, cover_back, "
			"fecha, tipo_emu, comentario, favorito, rating, edad_recomendada, usuario, path_exe, parametros_exe, path_capturas, path_setup, parametros_setup"
		") VALUES ("
			":icono, :titulo, :subtitulo, :genero, :compania, :desarrollador, :tema, :grupo, :perspectiva, :idioma, :idioma_voces, :formato, :anno, :numdisc, "
			":sistemaop, :tamano, :graficos, :sonido, :jugabilidad, :original, :estado, :thumbs, :cover_front, :cover_back, "
			":fecha, :tipo_emu, :comentario, :favorito, :rating, :edad_recomendada, :usuario, :path_exe, :parametros_exe, :path_capturas, :path_setup, :parametros_setup"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":icono"           , datos.icono             );
	query.bindValue(":titulo"          , datos.titulo            );
	query.bindValue(":subtitulo"       , datos.subtitulo         );
	query.bindValue(":genero"          , datos.genero            );
	query.bindValue(":compania"        , datos.compania          );
	query.bindValue(":desarrollador"   , datos.desarrollador     );
	query.bindValue(":tema"            , datos.tema              );
	query.bindValue(":grupo"           , datos.grupo             );
	query.bindValue(":perspectiva"     , datos.perspectiva       );
	query.bindValue(":idioma"          , datos.idioma            );
	query.bindValue(":idioma_voces"    , datos.idioma_voces      );
	query.bindValue(":formato"         , datos.formato           );
	query.bindValue(":anno"            , datos.anno              );
	query.bindValue(":numdisc"         , datos.numdisc           );
	query.bindValue(":sistemaop"       , datos.sistemaop         );
	query.bindValue(":tamano"          , datos.tamano            );
	query.bindValue(":graficos"        , datos.graficos          );
	query.bindValue(":sonido"          , datos.sonido            );
	query.bindValue(":jugabilidad"     , datos.jugabilidad       );
	query.bindValue(":original"        , datos.original.toLower());
	query.bindValue(":estado"          , datos.estado            );
	query.bindValue(":thumbs"          , datos.thumbs            );
	query.bindValue(":cover_front"     , datos.cover_front       );
	query.bindValue(":cover_back"      , datos.cover_back        );
	query.bindValue(":fecha"           , datos.fecha             );
	query.bindValue(":tipo_emu"        , datos.tipo_emu          );
	query.bindValue(":comentario"      , datos.comentario        );
	query.bindValue(":favorito"        , datos.favorito.toLower());
	query.bindValue(":rating"          , datos.rating            );
	query.bindValue(":edad_recomendada", datos.edad_recomendada  );
	query.bindValue(":usuario"         , datos.usuario           );
	query.bindValue(":path_exe"        , datos.path_exe          );
	query.bindValue(":parametros_exe"  , datos.parametros_exe    );
	query.bindValue(":path_capturas"   , datos.path_capturas     );
	query.bindValue(":path_setup"      , datos.path_setup        );
	query.bindValue(":parametros_setup", datos.parametros_setup  );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "idgrl");
	else
		return "";
}

bool dbSql::actualizaDatos(QString tabla, stDatosJuego datos)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE "+ tabla +" SET "
			"icono = :icono, titulo = :titulo, subtitulo = :subtitulo, genero = :genero, compania = :compania, "
			"desarrollador = :desarrollador, tema = :tema, grupo = :grupo, perspectiva = :perspectiva, idioma = :idioma, "
			"idioma_voces = :idioma_voces, formato = :formato, anno = :anno, numdisc = :numdisc, sistemaop = :sistemaop, "
			"tamano = :tamano, graficos = :graficos, sonido = :sonido, jugabilidad = :jugabilidad, original = :original, "
			"estado = :estado, thumbs = :thumbs, cover_front = :cover_front, cover_back = :cover_back, fecha = :fecha, "
			"tipo_emu = :tipo_emu, comentario = :comentario, favorito = :favorito, rating = :rating, "
			"edad_recomendada = :edad_recomendada, usuario = :usuario, path_exe = :path_exe, parametros_exe = :parametros_exe, "
			"path_capturas = :path_capturas, path_setup = :path_setup, parametros_setup = :parametros_setup "
			" WHERE idgrl = :idgrl;";

	query.prepare( strSQL );
	query.bindValue(":icono"           , datos.icono           );
	query.bindValue(":titulo"          , datos.titulo          );
	query.bindValue(":subtitulo"       , datos.subtitulo       );
	query.bindValue(":genero"          , datos.genero          );
	query.bindValue(":compania"        , datos.compania        );
	query.bindValue(":desarrollador"   , datos.desarrollador   );
	query.bindValue(":tema"            , datos.tema            );
	query.bindValue(":grupo"           , datos.grupo           );
	query.bindValue(":perspectiva"     , datos.perspectiva     );
	query.bindValue(":idioma"          , datos.idioma          );
	query.bindValue(":idioma_voces"    , datos.idioma_voces    );
	query.bindValue(":formato"         , datos.formato         );
	query.bindValue(":anno"            , datos.anno            );
	query.bindValue(":numdisc"         , datos.numdisc         );
	query.bindValue(":sistemaop"       , datos.sistemaop       );
	query.bindValue(":tamano"          , datos.tamano          );
	query.bindValue(":graficos"        , datos.graficos        );
	query.bindValue(":sonido"          , datos.sonido          );
	query.bindValue(":jugabilidad"     , datos.jugabilidad     );
	query.bindValue(":original"        , datos.original        );
	query.bindValue(":estado"          , datos.estado          );
	query.bindValue(":thumbs"          , datos.thumbs          );
	query.bindValue(":cover_front"     , datos.cover_front     );
	query.bindValue(":cover_back"      , datos.cover_back      );
	query.bindValue(":fecha"           , datos.fecha           );
	query.bindValue(":tipo_emu"        , datos.tipo_emu        );
	query.bindValue(":comentario"      , datos.comentario      );
	query.bindValue(":favorito"        , datos.favorito        );
	query.bindValue(":rating"          , datos.rating          );
	query.bindValue(":edad_recomendada", datos.edad_recomendada);
	query.bindValue(":usuario"         , datos.usuario         );
	query.bindValue(":path_exe"        , datos.path_exe        );
	query.bindValue(":parametros_exe"  , datos.parametros_exe  );
	query.bindValue(":path_capturas"   , datos.path_capturas   );
	query.bindValue(":path_setup"      , datos.path_setup      );
	query.bindValue(":parametros_setup", datos.parametros_setup);
	query.bindValue(":idgrl"           , datos.idgrl           );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::actualizaDatosItem(QString tabla, QString IDgrl, QString m_key, QString m_value)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE "+ tabla +" SET "+ m_key +" = :"+ m_key +" WHERE idgrl = :idgrl;");
	query.bindValue(":"+ m_key, m_value);
	query.bindValue(":idgrl"  , IDgrl  );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::actualizaDatosRating(QString tabla, QString IDgrl, QString new_rating)
{
	return actualizaDatosItem(tabla, IDgrl, "rating", new_rating);
}

bool dbSql::actualizaDatosFavorito(QString tabla, QString IDgrl, QString estadoFav)
{
	return actualizaDatosItem(tabla, IDgrl, "favorito", estadoFav);
}

bool dbSql::eliminarDatos(QString IDgrl, QString IDcat)
{
	if( !IDgrl.isEmpty() && !IDcat.isEmpty() )
	{
		QSqlQuery query(sqldb);
		QString IDdbx, IDvdms;
		QHash<int, stGrlCats> cat_list = getCategoriasLista("WHERE id = "+ IDcat);
		int id_cat = IDcat.toInt();
	// Datos url y archivos
		query.exec("DELETE FROM dbgrl_urls WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
		query.exec("DELETE FROM dbgrl_files WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
	// ScummVM
		query.exec("DELETE FROM dbgrl_emu_scummvm WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
	// DOSBox
		query.exec("SELECT id, path_conf FROM dbgrl_emu_dosbox WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
		if( query.first())
		{
			do {
				IDdbx = query.record().value("id").toString();
				QFile::remove(fGrl->DirApp() +"confdbx/"+ cat_list[id_cat].tabla +"/"+ query.record().value("path_conf").toString());
				query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdbx);
			} while ( query.next() );
		}
		query.exec("DELETE FROM dbgrl_emu_dosbox WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
	// VDMSound
		query.exec("SELECT id, path_conf FROM dbgrl_emu_vdmsound WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
		if( query.first())
		{
			do {
				IDvdms = query.record().value("id").toString();
				QFile::remove(fGrl->DirApp() +"confvdms/"+ cat_list[id_cat].tabla +"/"+ query.record().value("path_conf").toString());
			} while ( query.next() );
		}
		query.exec("DELETE FROM dbgrl_emu_vdmsound WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat);
	// Datos
		query.exec("DELETE FROM "+ cat_list[id_cat].tabla +" WHERE idgrl="+ IDgrl);

		return chequearQuery(query);
	} else
		return false;
}
// FIN Datos Juego ----------------------------------------------------------------------------------------------

// INICIO Config DOSBox -----------------------------------------------------------------------------------------
stConfigDOSBox dbSql::showConfg_DOSBox(QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	stConfigDOSBox cfgDbx;

	query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			cfgDbx.id                    = query.record().value("id").toString();
			cfgDbx.idgrl                 = query.record().value("idgrl").toString();
			cfgDbx.idcat                 = query.record().value("idcat").toString();
		// [sdl]
			cfgDbx.sdl_fullscreen        = query.record().value("sdl_fullscreen").toString();
			cfgDbx.sdl_fulldouble        = query.record().value("sdl_fulldouble").toString();
			cfgDbx.sdl_fullfixed         = query.record().value("sdl_fullfixed").toString();
			cfgDbx.sdl_fullresolution    = query.record().value("sdl_fullresolution").toString();
			cfgDbx.sdl_windowresolution  = query.record().value("sdl_windowresolution").toString();
			cfgDbx.sdl_output            = query.record().value("sdl_output").toString();
			cfgDbx.sdl_hwscale           = query.record().value("sdl_hwscale").toString();
			cfgDbx.sdl_autolock          = query.record().value("sdl_autolock").toString();
			cfgDbx.sdl_sensitivity       = query.record().value("sdl_sensitivity").toString();
			cfgDbx.sdl_waitonerror       = query.record().value("sdl_waitonerror").toString();
			cfgDbx.sdl_priority          = query.record().value("sdl_priority").toString();
			cfgDbx.sdl_mapperfile        = query.record().value("sdl_mapperfile").toString();
			cfgDbx.sdl_usescancodes      = query.record().value("sdl_usescancodes").toString();
		// [dosbox]
			cfgDbx.dosbox_language       = query.record().value("dosbox_language").toString();
			cfgDbx.dosbox_machine        = query.record().value("dosbox_machine").toString();
			cfgDbx.dosbox_captures       = query.record().value("dosbox_captures").toString();
			cfgDbx.dosbox_memsize        = query.record().value("dosbox_memsize").toString();
			cfgDbx.dosbox_emu_key        = query.record().value("dosbox_emu_key").toString();
		// [render]
			cfgDbx.render_frameskip      = query.record().value("render_frameskip").toString();
			cfgDbx.render_aspect         = query.record().value("render_aspect").toString();
			cfgDbx.render_scaler         = query.record().value("render_scaler").toString();
		// [cpu]
			cfgDbx.cpu_core              = query.record().value("cpu_core").toString();
			cfgDbx.cpu_cputype           = query.record().value("cpu_cputype").toString();
			cfgDbx.cpu_cycles            = query.record().value("cpu_cycles").toString();
			cfgDbx.cpu_cycles_realmode   = query.record().value("cpu_cycles_realmode").toString();
			cfgDbx.cpu_cycles_protmode   = query.record().value("cpu_cycles_protmode").toString();
			cfgDbx.cpu_cycles_limitmode  = query.record().value("cpu_cycles_limitmode").toString();
			cfgDbx.cpu_cycleup           = query.record().value("cpu_cycleup").toString();
			cfgDbx.cpu_cycledown         = query.record().value("cpu_cycledown").toString();
		// [mixer]
			cfgDbx.mixer_nosound         = query.record().value("mixer_nosound").toString();
			cfgDbx.mixer_rate            = query.record().value("mixer_rate").toString();
			cfgDbx.mixer_blocksize       = query.record().value("mixer_blocksize").toString();
			cfgDbx.mixer_prebuffer       = query.record().value("mixer_prebuffer").toString();
		// [midi]
			cfgDbx.midi_mpu401           = query.record().value("midi_mpu401").toString();
			cfgDbx.midi_intelligent      = query.record().value("midi_intelligent").toString();
			cfgDbx.midi_device           = query.record().value("midi_device").toString();
			cfgDbx.midi_config           = query.record().value("midi_config").toString();
			cfgDbx.midi_mt32rate         = query.record().value("midi_mt32rate").toString();
		// [sblaster]
			cfgDbx.sblaster_sbtype       = query.record().value("sblaster_sbtype").toString();
			cfgDbx.sblaster_sbbase       = query.record().value("sblaster_sbbase").toString();
			cfgDbx.sblaster_irq          = query.record().value("sblaster_irq").toString();
			cfgDbx.sblaster_dma          = query.record().value("sblaster_dma").toString();
			cfgDbx.sblaster_hdma         = query.record().value("sblaster_hdma").toString();
			cfgDbx.sblaster_mixer        = query.record().value("sblaster_mixer").toString();
			cfgDbx.sblaster_oplmode      = query.record().value("sblaster_oplmode").toString();
			cfgDbx.sblaster_oplemu       = query.record().value("sblaster_oplemu").toString();
			cfgDbx.sblaster_oplrate      = query.record().value("sblaster_oplrate").toString();
		// [gus]
			cfgDbx.gus_gus               = query.record().value("gus_gus").toString();
			cfgDbx.gus_gusrate           = query.record().value("gus_gusrate").toString();
			cfgDbx.gus_gusbase           = query.record().value("gus_gusbase").toString();
			cfgDbx.gus_irq1              = query.record().value("gus_irq1").toString();
			cfgDbx.gus_irq2              = query.record().value("gus_irq2").toString();
			cfgDbx.gus_dma1              = query.record().value("gus_dma1").toString();
			cfgDbx.gus_dma2              = query.record().value("gus_dma2").toString();
			cfgDbx.gus_ultradir          = query.record().value("gus_ultradir").toString();
		// [speaker]
			cfgDbx.speaker_pcspeaker     = query.record().value("speaker_pcspeaker").toString();
			cfgDbx.speaker_pcrate        = query.record().value("speaker_pcrate").toString();
			cfgDbx.speaker_tandy         = query.record().value("speaker_tandy").toString();
			cfgDbx.speaker_tandyrate     = query.record().value("speaker_tandyrate").toString();
			cfgDbx.speaker_disney        = query.record().value("speaker_disney").toString();
		// [joystick]
			cfgDbx.joystick_type         = query.record().value("joystick_type").toString();
			cfgDbx.joystick_timed        = query.record().value("joystick_timed").toString();
			cfgDbx.joystick_autofire     = query.record().value("joystick_autofire").toString();
			cfgDbx.joystick_swap34       = query.record().value("joystick_swap34").toString();
			cfgDbx.joystick_buttonwrap   = query.record().value("joystick_buttonwrap").toString();
		// [modem]
			cfgDbx.modem_modem           = query.record().value("modem_modem").toString();
			cfgDbx.modem_comport         = query.record().value("modem_comport").toString();
			cfgDbx.modem_listenport      = query.record().value("modem_listenport").toString();
		// [dserial]
			cfgDbx.dserial_directserial  = query.record().value("dserial_directserial").toString();
			cfgDbx.dserial_comport       = query.record().value("dserial_comport").toString();
			cfgDbx.dserial_realport      = query.record().value("dserial_realport").toString();
			cfgDbx.dserial_defaultbps    = query.record().value("dserial_defaultbps").toString();
			cfgDbx.dserial_parity        = query.record().value("dserial_parity").toString();
			cfgDbx.dserial_bytesize      = query.record().value("dserial_bytesize").toString();
			cfgDbx.dserial_stopbit       = query.record().value("dserial_stopbit").toString();
			cfgDbx.serial_1              = query.record().value("serial_1").toString();
			cfgDbx.serial_2              = query.record().value("serial_2").toString();
			cfgDbx.serial_3              = query.record().value("serial_3").toString();
			cfgDbx.serial_4              = query.record().value("serial_4").toString();
		// [dos]
			cfgDbx.dos_xms               = query.record().value("dos_xms").toString();
			cfgDbx.dos_ems               = query.record().value("dos_ems").toString();
			cfgDbx.dos_umb               = query.record().value("dos_umb").toString();
			cfgDbx.dos_keyboardlayout    = query.record().value("dos_keyboardlayout").toString();
		// [ipx]
			cfgDbx.ipx_ipx               = query.record().value("ipx_ipx").toString();
			cfgDbx.ipx_type              = query.record().value("ipx_type").toString();
			cfgDbx.ipx_port              = query.record().value("ipx_port").toString();
			cfgDbx.ipx_ip                = query.record().value("ipx_ip").toString();
		// [autoexec]
			cfgDbx.autoexec              = query.record().value("autoexec").toString();
		// Opciones
			cfgDbx.opt_autoexec          = query.record().value("opt_autoexec").toString();
			cfgDbx.opt_loadfix           = query.record().value("opt_loadfix").toString();
			cfgDbx.opt_loadfix_mem       = query.record().value("opt_loadfix_mem").toString();
			cfgDbx.opt_consola_dbox      = query.record().value("opt_consola_dbox").toString();
			cfgDbx.opt_cerrar_dbox       = query.record().value("opt_cerrar_dbox").toString();
			cfgDbx.opt_cycle_sincronizar = query.record().value("opt_cycle_sincronizar").toString();
		// Otras opciones
			cfgDbx.path_conf             = query.record().value("path_conf").toString();
			cfgDbx.path_sonido           = query.record().value("path_sonido").toString();
			cfgDbx.path_exe              = query.record().value("path_exe").toString();
			cfgDbx.path_setup            = query.record().value("path_setup").toString();
			cfgDbx.parametros_exe        = query.record().value("parametros_exe").toString();
			cfgDbx.parametros_setup      = query.record().value("parametros_setup").toString();
		} else
			cfgDbx = fGrl->getDefectDOSBox();
	} else
		cfgDbx = fGrl->getDefectDOSBox();

	return cfgDbx;
}

QString dbSql::insertaDbx(stConfigDOSBox cfgDbx)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_emu_dosbox ("
			"idgrl, idcat, sdl_fullscreen, sdl_fulldouble, sdl_fullfixed, sdl_fullresolution, sdl_windowresolution, sdl_output, "
			"sdl_hwscale, sdl_autolock, sdl_sensitivity, sdl_waitonerror, sdl_priority, sdl_mapperfile, sdl_usescancodes, "
			"dosbox_language, dosbox_machine, dosbox_captures, dosbox_memsize, dosbox_emu_key, render_frameskip, render_aspect, render_scaler, "
			"cpu_core, cpu_cputype, cpu_cycles, cpu_cycles_realmode, cpu_cycles_protmode, cpu_cycles_limitmode, cpu_cycleup, cpu_cycledown, mixer_nosound, mixer_rate, mixer_blocksize, mixer_prebuffer, "
			"midi_mpu401, midi_intelligent, midi_device, midi_config, midi_mt32rate, sblaster_sbtype, sblaster_sbbase, sblaster_irq, "
			"sblaster_dma, sblaster_hdma, sblaster_mixer, sblaster_oplmode, sblaster_oplemu, sblaster_oplrate, gus_gus, gus_gusrate, gus_gusbase, "
			"gus_irq1, gus_irq2, gus_dma1, gus_dma2, gus_ultradir, speaker_pcspeaker, speaker_pcrate, speaker_tandy, speaker_tandyrate, "
			"speaker_disney, joystick_type, joystick_timed, joystick_autofire, joystick_swap34, joystick_buttonwrap, "
			"modem_modem, modem_comport, modem_listenport, dserial_directserial, dserial_comport, "
			"dserial_realport, dserial_defaultbps, dserial_parity, dserial_bytesize, dserial_stopbit, serial_1, serial_2, serial_3, "
			"serial_4, dos_xms, dos_ems, dos_umb, dos_keyboardlayout, ipx_ipx, ipx_type, ipx_port, ipx_ip, autoexec, opt_autoexec, opt_loadfix, opt_loadfix_mem, opt_consola_dbox, "
			"opt_cerrar_dbox, opt_cycle_sincronizar, path_conf, path_sonido, path_exe, path_setup, parametros_exe, parametros_setup"
		") VALUES ("
			":idgrl, :idcat, :sdl_fullscreen, :sdl_fulldouble, :sdl_fullfixed, :sdl_fullresolution, :sdl_windowresolution, :sdl_output, "
			":sdl_hwscale, :sdl_autolock, :sdl_sensitivity, :sdl_waitonerror, :sdl_priority, :sdl_mapperfile, :sdl_usescancodes, "
			":dosbox_language, :dosbox_machine, :dosbox_captures, :dosbox_memsize, :dosbox_emu_key, :render_frameskip, :render_aspect, :render_scaler, "
			":cpu_core, :cpu_cputype, :cpu_cycles, :cpu_cycles_realmode, :cpu_cycles_protmode, :cpu_cycles_limitmode, :cpu_cycleup, :cpu_cycledown, :mixer_nosound, :mixer_rate, :mixer_blocksize, :mixer_prebuffer, "
			":midi_mpu401, :midi_intelligent, :midi_device, :midi_config, :midi_mt32rate, :sblaster_sbtype, :sblaster_sbbase, :sblaster_irq, "
			":sblaster_dma, :sblaster_hdma, :sblaster_mixer, :sblaster_oplmode, :sblaster_oplemu, :sblaster_oplrate, :gus_gus, :gus_gusrate, :gus_gusbase, "
			":gus_irq1, :gus_irq2, :gus_dma1, :gus_dma2, :gus_ultradir, :speaker_pcspeaker, :speaker_pcrate, :speaker_tandy, :speaker_tandyrate, "
			":speaker_disney, :joystick_type, :joystick_timed, :joystick_autofire, :joystick_swap34, :joystick_buttonwrap, "
			":modem_modem, :modem_comport, :modem_listenport, :dserial_directserial, :dserial_comport, "
			":dserial_realport, :dserial_defaultbps, :dserial_parity, :dserial_bytesize, :dserial_stopbit, :serial_1, :serial_2, :serial_3, "
			":serial_4, :dos_xms, :dos_ems, :dos_umb, :dos_keyboardlayout, :ipx_ipx, :ipx_type, :ipx_port, :ipx_ip, :autoexec, :opt_autoexec, :opt_loadfix, :opt_loadfix_mem, :opt_consola_dbox, "
			":opt_cerrar_dbox, :opt_cycle_sincronizar, :path_conf, :path_sonido, :path_exe, :path_setup, :parametros_exe, :parametros_setup"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":idgrl"                , cfgDbx.idgrl                 );
	query.bindValue(":idcat"                , cfgDbx.idcat                 );
	query.bindValue(":sdl_fullscreen"       , cfgDbx.sdl_fullscreen        );
	query.bindValue(":sdl_fulldouble"       , cfgDbx.sdl_fulldouble        );
	query.bindValue(":sdl_fullfixed"        , cfgDbx.sdl_fullfixed         );
	query.bindValue(":sdl_fullresolution"   , cfgDbx.sdl_fullresolution    );
	query.bindValue(":sdl_windowresolution" , cfgDbx.sdl_windowresolution  );
	query.bindValue(":sdl_output"           , cfgDbx.sdl_output            );
	query.bindValue(":sdl_hwscale"          , cfgDbx.sdl_hwscale           );
	query.bindValue(":sdl_autolock"         , cfgDbx.sdl_autolock          );
	query.bindValue(":sdl_sensitivity"      , cfgDbx.sdl_sensitivity       );
	query.bindValue(":sdl_waitonerror"      , cfgDbx.sdl_waitonerror       );
	query.bindValue(":sdl_priority"         , cfgDbx.sdl_priority          );
	query.bindValue(":sdl_mapperfile"       , cfgDbx.sdl_mapperfile        );
	query.bindValue(":sdl_usescancodes"     , cfgDbx.sdl_usescancodes      );
	query.bindValue(":dosbox_language"      , cfgDbx.dosbox_language       );
	query.bindValue(":dosbox_machine"       , cfgDbx.dosbox_machine        );
	query.bindValue(":dosbox_captures"      , cfgDbx.dosbox_captures       );
	query.bindValue(":dosbox_memsize"       , cfgDbx.dosbox_memsize        );
	query.bindValue(":dosbox_emu_key"       , cfgDbx.dosbox_emu_key        );
	query.bindValue(":render_frameskip"     , cfgDbx.render_frameskip      );
	query.bindValue(":render_aspect"        , cfgDbx.render_aspect         );
	query.bindValue(":render_scaler"        , cfgDbx.render_scaler         );
	query.bindValue(":cpu_core"             , cfgDbx.cpu_core              );
	query.bindValue(":cpu_cputype"          , cfgDbx.cpu_cputype           );
	query.bindValue(":cpu_cycles"           , cfgDbx.cpu_cycles            );
	query.bindValue(":cpu_cycles_realmode"  , cfgDbx.cpu_cycles_realmode   );
	query.bindValue(":cpu_cycles_protmode"  , cfgDbx.cpu_cycles_protmode   );
	query.bindValue(":cpu_cycles_limitmode" , cfgDbx.cpu_cycles_limitmode  );
	query.bindValue(":cpu_cycleup"          , cfgDbx.cpu_cycleup           );
	query.bindValue(":cpu_cycledown"        , cfgDbx.cpu_cycledown         );
	query.bindValue(":mixer_nosound"        , cfgDbx.mixer_nosound         );
	query.bindValue(":mixer_rate"           , cfgDbx.mixer_rate            );
	query.bindValue(":mixer_blocksize"      , cfgDbx.mixer_blocksize       );
	query.bindValue(":mixer_prebuffer"      , cfgDbx.mixer_prebuffer       );
	query.bindValue(":midi_mpu401"          , cfgDbx.midi_mpu401           );
	query.bindValue(":midi_intelligent"     , cfgDbx.midi_intelligent      );
	query.bindValue(":midi_device"          , cfgDbx.midi_device           );
	query.bindValue(":midi_config"          , cfgDbx.midi_config           );
	query.bindValue(":midi_mt32rate"        , cfgDbx.midi_mt32rate         );
	query.bindValue(":sblaster_sbtype"      , cfgDbx.sblaster_sbtype       );
	query.bindValue(":sblaster_sbbase"      , cfgDbx.sblaster_sbbase       );
	query.bindValue(":sblaster_irq"         , cfgDbx.sblaster_irq          );
	query.bindValue(":sblaster_dma"         , cfgDbx.sblaster_dma          );
	query.bindValue(":sblaster_hdma"        , cfgDbx.sblaster_hdma         );
	query.bindValue(":sblaster_mixer"       , cfgDbx.sblaster_mixer        );
	query.bindValue(":sblaster_oplmode"     , cfgDbx.sblaster_oplmode      );
	query.bindValue(":sblaster_oplemu"      , cfgDbx.sblaster_oplemu       );
	query.bindValue(":sblaster_oplrate"     , cfgDbx.sblaster_oplrate      );
	query.bindValue(":gus_gus"              , cfgDbx.gus_gus               );
	query.bindValue(":gus_gusrate"          , cfgDbx.gus_gusrate           );
	query.bindValue(":gus_gusbase"          , cfgDbx.gus_gusbase           );
	query.bindValue(":gus_irq1"             , cfgDbx.gus_irq1              );
	query.bindValue(":gus_irq2"             , cfgDbx.gus_irq2              );
	query.bindValue(":gus_dma1"             , cfgDbx.gus_dma1              );
	query.bindValue(":gus_dma2"             , cfgDbx.gus_dma2              );
	query.bindValue(":gus_ultradir"         , cfgDbx.gus_ultradir          );
	query.bindValue(":speaker_pcspeaker"    , cfgDbx.speaker_pcspeaker     );
	query.bindValue(":speaker_pcrate"       , cfgDbx.speaker_pcrate        );
	query.bindValue(":speaker_tandy"        , cfgDbx.speaker_tandy         );
	query.bindValue(":speaker_tandyrate"    , cfgDbx.speaker_tandyrate     );
	query.bindValue(":speaker_disney"       , cfgDbx.speaker_disney        );
	query.bindValue(":joystick_type"        , cfgDbx.joystick_type         );
	query.bindValue(":joystick_timed"       , cfgDbx.joystick_timed        );
	query.bindValue(":joystick_autofire"    , cfgDbx.joystick_autofire     );
	query.bindValue(":joystick_swap34"      , cfgDbx.joystick_swap34       );
	query.bindValue(":joystick_buttonwrap"  , cfgDbx.joystick_buttonwrap   );
	query.bindValue(":modem_modem"          , cfgDbx.modem_modem           );
	query.bindValue(":modem_comport"        , cfgDbx.modem_comport         );
	query.bindValue(":modem_listenport"     , cfgDbx.modem_listenport      );
	query.bindValue(":dserial_directserial" , cfgDbx.dserial_directserial  );
	query.bindValue(":dserial_comport"      , cfgDbx.dserial_comport       );
	query.bindValue(":dserial_realport"     , cfgDbx.dserial_realport      );
	query.bindValue(":dserial_defaultbps"   , cfgDbx.dserial_defaultbps    );
	query.bindValue(":dserial_parity"       , cfgDbx.dserial_parity        );
	query.bindValue(":dserial_bytesize"     , cfgDbx.dserial_bytesize      );
	query.bindValue(":dserial_stopbit"      , cfgDbx.dserial_stopbit       );
	query.bindValue(":serial_1"             , cfgDbx.serial_1              );
	query.bindValue(":serial_2"             , cfgDbx.serial_2              );
	query.bindValue(":serial_3"             , cfgDbx.serial_3              );
	query.bindValue(":serial_4"             , cfgDbx.serial_4              );
	query.bindValue(":dos_xms"              , cfgDbx.dos_xms               );
	query.bindValue(":dos_ems"              , cfgDbx.dos_ems               );
	query.bindValue(":dos_umb"              , cfgDbx.dos_umb               );
	query.bindValue(":dos_keyboardlayout"   , cfgDbx.dos_keyboardlayout    );
	query.bindValue(":ipx_ipx"              , cfgDbx.ipx_ipx               );
	query.bindValue(":ipx_type"             , cfgDbx.ipx_type              );
	query.bindValue(":ipx_port"             , cfgDbx.ipx_port              );
	query.bindValue(":ipx_ip"               , cfgDbx.ipx_ip                );
	query.bindValue(":autoexec"             , cfgDbx.autoexec              );
	query.bindValue(":opt_autoexec"         , cfgDbx.opt_autoexec          );
	query.bindValue(":opt_loadfix"          , cfgDbx.opt_loadfix           );
	query.bindValue(":opt_loadfix_mem"      , cfgDbx.opt_loadfix_mem       );
	query.bindValue(":opt_consola_dbox"     , cfgDbx.opt_consola_dbox      );
	query.bindValue(":opt_cerrar_dbox"      , cfgDbx.opt_cerrar_dbox       );
	query.bindValue(":opt_cycle_sincronizar", cfgDbx.opt_cycle_sincronizar );
	query.bindValue(":path_conf"            , cfgDbx.path_conf             );
	query.bindValue(":path_sonido"          , cfgDbx.path_sonido           );
	query.bindValue(":path_exe"             , cfgDbx.path_exe              );
	query.bindValue(":path_setup"           , cfgDbx.path_setup            );
	query.bindValue(":parametros_exe"       , cfgDbx.parametros_exe        );
	query.bindValue(":parametros_setup"     , cfgDbx.parametros_setup      );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaDbx(stConfigDOSBox cfgDbx)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_dosbox SET "
		"sdl_fullscreen = :sdl_fullscreen, sdl_fulldouble = :sdl_fulldouble, sdl_fullfixed = :sdl_fullfixed, sdl_fullresolution = :sdl_fullresolution, "
		"sdl_windowresolution = :sdl_windowresolution, sdl_output = :sdl_output, sdl_hwscale = :sdl_hwscale, sdl_autolock = :sdl_autolock, "
		"sdl_sensitivity = :sdl_sensitivity, sdl_waitonerror = :sdl_waitonerror, sdl_priority = :sdl_priority, sdl_mapperfile = :sdl_mapperfile, "
		"sdl_usescancodes = :sdl_usescancodes, dosbox_language = :dosbox_language, dosbox_machine = :dosbox_machine, dosbox_captures = :dosbox_captures, "
		"dosbox_memsize = :dosbox_memsize, dosbox_emu_key = :dosbox_emu_key, render_frameskip = :render_frameskip, render_aspect = :render_aspect, render_scaler = :render_scaler, "
		"cpu_core = :cpu_core, cpu_cputype = :cpu_cputype, cpu_cycles = :cpu_cycles, cpu_cycles_realmode = :cpu_cycles_realmode, "
		"cpu_cycles_protmode = :cpu_cycles_protmode, cpu_cycles_limitmode = :cpu_cycles_limitmode, cpu_cycleup = :cpu_cycleup, cpu_cycledown = :cpu_cycledown, mixer_nosound = :mixer_nosound, "
		"mixer_rate = :mixer_rate, mixer_blocksize = :mixer_blocksize, mixer_prebuffer = :mixer_prebuffer, midi_mpu401 = :midi_mpu401, midi_intelligent = :midi_intelligent, "
		"midi_device = :midi_device, midi_config = :midi_config, midi_mt32rate = :midi_mt32rate, sblaster_sbtype = :sblaster_sbtype, sblaster_sbbase = :sblaster_sbbase, "
		"sblaster_irq = :sblaster_irq, sblaster_dma = :sblaster_dma, sblaster_hdma = :sblaster_hdma, sblaster_mixer = :sblaster_mixer, sblaster_oplmode = :sblaster_oplmode, "
		"sblaster_oplemu = :sblaster_oplemu, sblaster_oplrate = :sblaster_oplrate, gus_gus = :gus_gus, gus_gusrate = :gus_gusrate, gus_gusbase = :gus_gusbase, gus_irq1 = :gus_irq1, gus_irq2 = :gus_irq2, "
		"gus_dma1 = :gus_dma1, gus_dma2 = :gus_dma2, gus_ultradir = :gus_ultradir, speaker_pcspeaker = :speaker_pcspeaker, speaker_pcrate = :speaker_pcrate, "
		"speaker_tandy = :speaker_tandy, speaker_tandyrate = :speaker_tandyrate, speaker_disney = :speaker_disney, joystick_type = :joystick_type, "
		"joystick_timed = :joystick_timed, joystick_autofire = :joystick_autofire, joystick_swap34 = :joystick_swap34, joystick_buttonwrap = :joystick_buttonwrap, "
		"modem_modem = :modem_modem, modem_comport = :modem_comport, modem_listenport = :modem_listenport, dserial_directserial = :dserial_directserial, "
		"dserial_comport = :dserial_comport, dserial_realport = :dserial_realport, dserial_defaultbps = :dserial_defaultbps, dserial_parity = :dserial_parity, "
		"dserial_bytesize = :dserial_bytesize, dserial_stopbit = :dserial_stopbit, serial_1 = :serial_1, serial_2 = :serial_2, serial_3 = :serial_3, serial_4 = :serial_4, dos_xms = :dos_xms, "
		"dos_ems = :dos_ems, dos_umb = :dos_umb, dos_keyboardlayout = :dos_keyboardlayout, ipx_ipx = :ipx_ipx, ipx_type = :ipx_type, ipx_port = :ipx_port, ipx_ip = :ipx_ip, autoexec = :autoexec, "
		"opt_autoexec = :opt_autoexec, opt_loadfix = :opt_loadfix, opt_loadfix_mem = :opt_loadfix_mem, opt_consola_dbox = :opt_consola_dbox, opt_cerrar_dbox = :opt_cerrar_dbox, opt_cycle_sincronizar = :opt_cycle_sincronizar, "
		"path_conf = :path_conf, path_sonido = :path_sonido, path_exe = :path_exe, path_setup = :path_setup, parametros_exe = :parametros_exe, parametros_setup = :parametros_setup "
		"WHERE id = :id;";

	query.prepare( strSQL );
	query.bindValue(":sdl_fullscreen"       , cfgDbx.sdl_fullscreen        );
	query.bindValue(":sdl_fulldouble"       , cfgDbx.sdl_fulldouble        );
	query.bindValue(":sdl_fullfixed"        , cfgDbx.sdl_fullfixed         );
	query.bindValue(":sdl_fullresolution"   , cfgDbx.sdl_fullresolution    );
	query.bindValue(":sdl_windowresolution" , cfgDbx.sdl_windowresolution  );
	query.bindValue(":sdl_output"           , cfgDbx.sdl_output            );
	query.bindValue(":sdl_hwscale"          , cfgDbx.sdl_hwscale           );
	query.bindValue(":sdl_autolock"         , cfgDbx.sdl_autolock          );
	query.bindValue(":sdl_sensitivity"      , cfgDbx.sdl_sensitivity       );
	query.bindValue(":sdl_waitonerror"      , cfgDbx.sdl_waitonerror       );
	query.bindValue(":sdl_priority"         , cfgDbx.sdl_priority          );
	query.bindValue(":sdl_mapperfile"       , cfgDbx.sdl_mapperfile        );
	query.bindValue(":sdl_usescancodes"     , cfgDbx.sdl_usescancodes      );
	query.bindValue(":dosbox_language"      , cfgDbx.dosbox_language       );
	query.bindValue(":dosbox_machine"       , cfgDbx.dosbox_machine        );
	query.bindValue(":dosbox_captures"      , cfgDbx.dosbox_captures       );
	query.bindValue(":dosbox_memsize"       , cfgDbx.dosbox_memsize        );
	query.bindValue(":dosbox_emu_key"       , cfgDbx.dosbox_emu_key        );
	query.bindValue(":render_frameskip"     , cfgDbx.render_frameskip      );
	query.bindValue(":render_aspect"        , cfgDbx.render_aspect         );
	query.bindValue(":render_scaler"        , cfgDbx.render_scaler         );
	query.bindValue(":cpu_core"             , cfgDbx.cpu_core              );
	query.bindValue(":cpu_cputype"          , cfgDbx.cpu_cputype           );
	query.bindValue(":cpu_cycles"           , cfgDbx.cpu_cycles            );
	query.bindValue(":cpu_cycles_realmode"  , cfgDbx.cpu_cycles_realmode   );
	query.bindValue(":cpu_cycles_protmode"  , cfgDbx.cpu_cycles_protmode   );
	query.bindValue(":cpu_cycles_limitmode" , cfgDbx.cpu_cycles_limitmode  );
	query.bindValue(":cpu_cycleup"          , cfgDbx.cpu_cycleup           );
	query.bindValue(":cpu_cycledown"        , cfgDbx.cpu_cycledown         );
	query.bindValue(":mixer_nosound"        , cfgDbx.mixer_nosound         );
	query.bindValue(":mixer_rate"           , cfgDbx.mixer_rate            );
	query.bindValue(":mixer_blocksize"      , cfgDbx.mixer_blocksize       );
	query.bindValue(":mixer_prebuffer"      , cfgDbx.mixer_prebuffer       );
	query.bindValue(":midi_mpu401"          , cfgDbx.midi_mpu401           );
	query.bindValue(":midi_intelligent"     , cfgDbx.midi_intelligent      );
	query.bindValue(":midi_device"          , cfgDbx.midi_device           );
	query.bindValue(":midi_config"          , cfgDbx.midi_config           );
	query.bindValue(":midi_mt32rate"        , cfgDbx.midi_mt32rate         );
	query.bindValue(":sblaster_sbtype"      , cfgDbx.sblaster_sbtype       );
	query.bindValue(":sblaster_sbbase"      , cfgDbx.sblaster_sbbase       );
	query.bindValue(":sblaster_irq"         , cfgDbx.sblaster_irq          );
	query.bindValue(":sblaster_dma"         , cfgDbx.sblaster_dma          );
	query.bindValue(":sblaster_hdma"        , cfgDbx.sblaster_hdma         );
	query.bindValue(":sblaster_mixer"       , cfgDbx.sblaster_mixer        );
	query.bindValue(":sblaster_oplmode"     , cfgDbx.sblaster_oplmode      );
	query.bindValue(":sblaster_oplemu"      , cfgDbx.sblaster_oplemu       );
	query.bindValue(":sblaster_oplrate"     , cfgDbx.sblaster_oplrate      );
	query.bindValue(":gus_gus"              , cfgDbx.gus_gus               );
	query.bindValue(":gus_gusrate"          , cfgDbx.gus_gusrate           );
	query.bindValue(":gus_gusbase"          , cfgDbx.gus_gusbase           );
	query.bindValue(":gus_irq1"             , cfgDbx.gus_irq1              );
	query.bindValue(":gus_irq2"             , cfgDbx.gus_irq2              );
	query.bindValue(":gus_dma1"             , cfgDbx.gus_dma1              );
	query.bindValue(":gus_dma2"             , cfgDbx.gus_dma2              );
	query.bindValue(":gus_ultradir"         , cfgDbx.gus_ultradir          );
	query.bindValue(":speaker_pcspeaker"    , cfgDbx.speaker_pcspeaker     );
	query.bindValue(":speaker_pcrate"       , cfgDbx.speaker_pcrate        );
	query.bindValue(":speaker_tandy"        , cfgDbx.speaker_tandy         );
	query.bindValue(":speaker_tandyrate"    , cfgDbx.speaker_tandyrate     );
	query.bindValue(":speaker_disney"       , cfgDbx.speaker_disney        );
	query.bindValue(":joystick_type"        , cfgDbx.joystick_type         );
	query.bindValue(":joystick_timed"       , cfgDbx.joystick_timed        );
	query.bindValue(":joystick_autofire"    , cfgDbx.joystick_autofire     );
	query.bindValue(":joystick_swap34"      , cfgDbx.joystick_swap34       );
	query.bindValue(":joystick_buttonwrap"  , cfgDbx.joystick_buttonwrap   );
	query.bindValue(":modem_modem"          , cfgDbx.modem_modem           );
	query.bindValue(":modem_comport"        , cfgDbx.modem_comport         );
	query.bindValue(":modem_listenport"     , cfgDbx.modem_listenport      );
	query.bindValue(":dserial_directserial" , cfgDbx.dserial_directserial  );
	query.bindValue(":dserial_comport"      , cfgDbx.dserial_comport       );
	query.bindValue(":dserial_realport"     , cfgDbx.dserial_realport      );
	query.bindValue(":dserial_defaultbps"   , cfgDbx.dserial_defaultbps    );
	query.bindValue(":dserial_parity"       , cfgDbx.dserial_parity        );
	query.bindValue(":dserial_bytesize"     , cfgDbx.dserial_bytesize      );
	query.bindValue(":dserial_stopbit"      , cfgDbx.dserial_stopbit       );
	query.bindValue(":serial_1"             , cfgDbx.serial_1              );
	query.bindValue(":serial_2"             , cfgDbx.serial_2              );
	query.bindValue(":serial_3"             , cfgDbx.serial_3              );
	query.bindValue(":serial_4"             , cfgDbx.serial_4              );
	query.bindValue(":dos_xms"              , cfgDbx.dos_xms               );
	query.bindValue(":dos_ems"              , cfgDbx.dos_ems               );
	query.bindValue(":dos_umb"              , cfgDbx.dos_umb               );
	query.bindValue(":dos_keyboardlayout"   , cfgDbx.dos_keyboardlayout    );
	query.bindValue(":ipx_ipx"              , cfgDbx.ipx_ipx               );
	query.bindValue(":ipx_type"             , cfgDbx.ipx_type              );
	query.bindValue(":ipx_port"             , cfgDbx.ipx_port              );
	query.bindValue(":ipx_ip"               , cfgDbx.ipx_ip                );
	query.bindValue(":autoexec"             , cfgDbx.autoexec              );
	query.bindValue(":opt_autoexec"         , cfgDbx.opt_autoexec          );
	query.bindValue(":opt_loadfix"          , cfgDbx.opt_loadfix           );
	query.bindValue(":opt_loadfix_mem"      , cfgDbx.opt_loadfix_mem       );
	query.bindValue(":opt_consola_dbox"     , cfgDbx.opt_consola_dbox      );
	query.bindValue(":opt_cerrar_dbox"      , cfgDbx.opt_cerrar_dbox       );
	query.bindValue(":opt_cycle_sincronizar", cfgDbx.opt_cycle_sincronizar );
	query.bindValue(":path_conf"            , cfgDbx.path_conf             );
	query.bindValue(":path_sonido"          , cfgDbx.path_sonido           );
	query.bindValue(":path_exe"             , cfgDbx.path_exe              );
	query.bindValue(":path_setup"           , cfgDbx.path_setup            );
	query.bindValue(":parametros_exe"       , cfgDbx.parametros_exe        );
	query.bindValue(":parametros_setup"     , cfgDbx.parametros_setup      );
	query.bindValue(":id"                   , cfgDbx.id                    );
	query.exec();

	return chequearQuery(query);
}

void dbSql::insertaMontajesDbx(QTreeWidget *twDbx, QString IDdbx)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_emu_dosbox_mount ("
			"id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize"
		") VALUES ( "
			":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount, :opt_size, :opt_freesize, :freesize"
		")"+ qpsql_return_id;

	const int count_mount = twDbx->topLevelItemCount();
	for (int num_mount = 0; num_mount < count_mount; ++num_mount)
	{
		QTreeWidgetItem *item_mount = twDbx->topLevelItem( num_mount );
		query.prepare( strSQL );
		query.bindValue(":id_dosbox"   , IDdbx               );
		query.bindValue(":id_lista"    , num_mount           );
		query.bindValue(":path"        , item_mount->text(0) );
		query.bindValue(":letter"      , item_mount->text(1) );
		query.bindValue(":tipo_as"     , item_mount->text(2) );
		query.bindValue(":label"       , item_mount->text(3) );
		query.bindValue(":indx_cd"     , item_mount->text(4) );
		query.bindValue(":opt_mount"   , item_mount->text(5) );
		query.bindValue(":io_ctrl"     , item_mount->text(6) );
		query.bindValue(":select_mount", item_mount->text(7) );
		query.bindValue(":opt_size"    , item_mount->text(10));
		query.bindValue(":opt_freesize", item_mount->text(11));
		query.bindValue(":freesize"    , item_mount->text(12));
		query.exec();

		if( chequearQuery(query) )
			item_mount->setText(8, lastInsertId(query, "id"));
	}
}

QString dbSql::insertaUnMontajeDbx(stConfigDOSBoxMount cfgDbxMount)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_emu_dosbox_mount ("
			"id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount, opt_size, opt_freesize, freesize"
		") VALUES ("
			":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount, :opt_size, :opt_freesize, :freesize"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":id_dosbox"   , cfgDbxMount.id_dosbox    );
	query.bindValue(":id_lista"    , cfgDbxMount.id_lista     );
	query.bindValue(":path"        , cfgDbxMount.path         );
	query.bindValue(":label"       , cfgDbxMount.label        );
	query.bindValue(":tipo_as"     , cfgDbxMount.tipo_as      );
	query.bindValue(":letter"      , cfgDbxMount.letter       );
	query.bindValue(":indx_cd"     , cfgDbxMount.indx_cd      );
	query.bindValue(":opt_mount"   , cfgDbxMount.opt_mount    );
	query.bindValue(":io_ctrl"     , cfgDbxMount.io_ctrl      );
	query.bindValue(":select_mount", cfgDbxMount.select_mount );
	query.bindValue(":opt_size"    , cfgDbxMount.opt_size     );
	query.bindValue(":opt_freesize", cfgDbxMount.opt_freesize );
	query.bindValue(":freesize"    , cfgDbxMount.freesize     );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

void dbSql::actualizaMontajeDbx(QTreeWidget *twDbx)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_dosbox_mount SET "
		"id_lista = :id_lista, path = :path, label = :label, "
		"tipo_as = :tipo_as, letter = :letter, indx_cd = :indx_cd, opt_mount = :opt_mount, io_ctrl = :io_ctrl, "
		"select_mount = :select_mount, opt_size = :opt_size, opt_freesize = :opt_freesize, freesize = :freesize "
		"WHERE id = :id;";

	const int count_mount = twDbx->topLevelItemCount();
	if( count_mount > 0 )
	{
		for (int num_mount = 0; num_mount < count_mount; ++num_mount)
		{
			QTreeWidgetItem *item_mount = twDbx->topLevelItem( num_mount );
			query.prepare( strSQL );
			query.bindValue(":id_lista"    , num_mount           );
			query.bindValue(":path"        , item_mount->text(0) );
			query.bindValue(":letter"      , item_mount->text(1) );
			query.bindValue(":tipo_as"     , item_mount->text(2) );
			query.bindValue(":label"       , item_mount->text(3) );
			query.bindValue(":indx_cd"     , item_mount->text(4) );
			query.bindValue(":opt_mount"   , item_mount->text(5) );
			query.bindValue(":io_ctrl"     , item_mount->text(6) );
			query.bindValue(":select_mount", item_mount->text(7) );
			query.bindValue(":id"          , item_mount->text(8) );
			query.bindValue(":opt_size"    , item_mount->text(10));
			query.bindValue(":opt_freesize", item_mount->text(11));
			query.bindValue(":freesize"    , item_mount->text(12));
			query.exec();

			chequearQuery(query);
		}
	}
}

bool dbSql::eliminarMontaje(QString IDmountdbx)
{
	if( IDmountdbx.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id="+ IDmountdbx +";");

		return chequearQuery(query);
	}
}
// FIN Config DOSBox --------------------------------------------------------------------------------------------

// INICIO Config ScummVM ----------------------------------------------------------------------------------------
stConfigScummVM dbSql::showConfg_ScummVM(QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	stConfigScummVM cfgSvm;

	query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			cfgSvm.id                = query.record().value("id" ).toString();
			cfgSvm.idgrl             = query.record().value("idgrl" ).toString();
			cfgSvm.idcat             = query.record().value("idcat" ).toString();
			cfgSvm.game              = query.record().value("game" ).toString();
			cfgSvm.game_label        = query.record().value("game_label" ).toString();
			cfgSvm.language          = query.record().value("language").toString();
			cfgSvm.subtitles         = query.record().value("subtitles").toString();
			cfgSvm.platform          = query.record().value("platform").toString();
			cfgSvm.gfx_mode          = query.record().value("gfx_mode").toString();
			cfgSvm.render_mode       = query.record().value("render_mode").toString();
			cfgSvm.fullscreen        = query.record().value("fullscreen").toString();
			cfgSvm.aspect_ratio      = query.record().value("aspect_ratio").toString();
			cfgSvm.path_game         = query.record().value("path").toString();
			cfgSvm.path_extra        = query.record().value("path_extra").toString();
			cfgSvm.path_save         = query.record().value("path_save").toString();
			cfgSvm.path_capturas     = query.record().value("path_capturas").toString();
			cfgSvm.path_sonido       = query.record().value("path_sonido").toString();
			cfgSvm.music_driver      = query.record().value("music_driver").toString();
			cfgSvm.enable_gs         = query.record().value("enable_gs").toString();
			cfgSvm.multi_midi        = query.record().value("multi_midi").toString();
			cfgSvm.native_mt32       = query.record().value("native_mt32").toString();
			cfgSvm.mute              = query.record().value("mute").toString();
			cfgSvm.master_volume     = query.record().value("master_volume").toString();
			cfgSvm.music_volume      = query.record().value("music_volume").toString();
			cfgSvm.sfx_volume        = query.record().value("sfx_volume").toString();
			cfgSvm.speech_volume     = query.record().value("speech_volume").toString();
			cfgSvm.speech_mute       = query.record().value("speech_mute").toString();
			cfgSvm.tempo             = query.record().value("tempo").toString();
			cfgSvm.talkspeed         = query.record().value("talkspeed").toString();
			cfgSvm.debuglevel        = query.record().value("debuglevel").toString();
			cfgSvm.cdrom             = query.record().value("cdrom").toString();
			cfgSvm.joystick_num      = query.record().value("joystick_num").toString();
			cfgSvm.output_rate       = query.record().value("output_rate").toString();
			cfgSvm.midi_gain         = query.record().value("midi_gain").toString();
			cfgSvm.copy_protection   = query.record().value("copy_protection").toString();
			cfgSvm.sound_font        = query.record().value("sound_font").toString();
			cfgSvm.walkspeed         = query.record().value("walkspeed").toString();
			cfgSvm.opl_driver        = query.record().value("opl_driver").toString();
			cfgSvm.disable_dithering = query.record().value("disable_dithering").toString();
			cfgSvm.alt_intro         = query.record().value("alt_intro").toString();
			cfgSvm.boot_param        = query.record().value("boot_param").toString();
		//--
			cfgSvm.description       = "";
			cfgSvm.emu_svmpath       = "";
		} else
			cfgSvm = fGrl->getDefectScummVM();
	} else
		cfgSvm = fGrl->getDefectScummVM();

	return cfgSvm;
}

QString dbSql::insertaSvm(stConfigScummVM cfgSvm)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_emu_scummvm ("
			"idgrl, idcat, game, game_label, language, subtitles, platform, gfx_mode, render_mode, fullscreen, aspect_ratio, path, "
			"path_extra, path_save, path_capturas, path_sonido, music_driver, enable_gs, multi_midi, native_mt32, mute, "
			"master_volume, music_volume, sfx_volume, speech_volume, speech_mute, tempo, talkspeed, debuglevel, cdrom, joystick_num, "
			"output_rate, midi_gain, copy_protection, sound_font, walkspeed, opl_driver, disable_dithering, alt_intro, boot_param"
		") VALUES ("
			":idgrl, :idcat, :game, :game_label, :language, :subtitles, :platform, :gfx_mode, :render_mode, :fullscreen, :aspect_ratio, :path_game, "
			":path_extra, :path_save, :path_capturas, :path_sonido, :music_driver, :enable_gs, :multi_midi, :native_mt32, :mute, "
			":master_volume, :music_volume, :sfx_volume, :speech_volume, :speech_mute, :tempo, :talkspeed, :debuglevel, :cdrom, :joystick_num, "
			":output_rate, :midi_gain, :copy_protection, :sound_font, :walkspeed, :opl_driver, :disable_dithering, :alt_intro, :boot_param"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":idgrl"            , cfgSvm.idgrl             );
	query.bindValue(":idcat"            , cfgSvm.idcat             );
	query.bindValue(":game"             , cfgSvm.game              );
	query.bindValue(":game_label"       , cfgSvm.game_label        );
	query.bindValue(":language"         , cfgSvm.language          );
	query.bindValue(":subtitles"        , cfgSvm.subtitles         );
	query.bindValue(":platform"         , cfgSvm.platform          );
	query.bindValue(":gfx_mode"         , cfgSvm.gfx_mode          );
	query.bindValue(":render_mode"      , cfgSvm.render_mode       );
	query.bindValue(":fullscreen"       , cfgSvm.fullscreen        );
	query.bindValue(":aspect_ratio"     , cfgSvm.aspect_ratio      );
	query.bindValue(":path_game"        , cfgSvm.path_game         );
	query.bindValue(":path_extra"       , cfgSvm.path_extra        );
	query.bindValue(":path_save"        , cfgSvm.path_save         );
	query.bindValue(":path_capturas"    , cfgSvm.path_capturas     );
	query.bindValue(":path_sonido"      , cfgSvm.path_sonido       );
	query.bindValue(":music_driver"     , cfgSvm.music_driver      );
	query.bindValue(":enable_gs"        , cfgSvm.enable_gs         );
	query.bindValue(":multi_midi"       , cfgSvm.multi_midi        );
	query.bindValue(":native_mt32"      , cfgSvm.native_mt32       );
	query.bindValue(":mute"             , cfgSvm.mute              );
	query.bindValue(":master_volume"    , cfgSvm.master_volume     );
	query.bindValue(":music_volume"     , cfgSvm.music_volume      );
	query.bindValue(":sfx_volume"       , cfgSvm.sfx_volume        );
	query.bindValue(":speech_volume"    , cfgSvm.speech_volume     );
	query.bindValue(":speech_mute"      , cfgSvm.speech_mute       );
	query.bindValue(":tempo"            , cfgSvm.tempo             );
	query.bindValue(":talkspeed"        , cfgSvm.talkspeed         );
	query.bindValue(":debuglevel"       , cfgSvm.debuglevel        );
	query.bindValue(":cdrom"            , cfgSvm.cdrom             );
	query.bindValue(":joystick_num"     , cfgSvm.joystick_num      );
	query.bindValue(":output_rate"      , cfgSvm.output_rate       );
	query.bindValue(":midi_gain"        , cfgSvm.midi_gain         );
	query.bindValue(":copy_protection"  , cfgSvm.copy_protection   );
	query.bindValue(":sound_font"       , cfgSvm.sound_font        );
	query.bindValue(":walkspeed"        , cfgSvm.walkspeed         );
	query.bindValue(":opl_driver"       , cfgSvm.opl_driver        );
	query.bindValue(":disable_dithering", cfgSvm.disable_dithering );
	query.bindValue(":alt_intro"        , cfgSvm.alt_intro         );
	query.bindValue(":boot_param"       , cfgSvm.boot_param        );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaSvm(stConfigScummVM cfgSvm)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_scummvm SET "
		"game = :game, game_label = :game_label, language = :language, subtitles = :subtitles, platform = :platform, gfx_mode = :gfx_mode, "
		"render_mode = :render_mode, fullscreen = :fullscreen, aspect_ratio = :aspect_ratio, path = :path_game, "
		"path_extra = :path_extra, path_save = :path_save, path_capturas = :path_capturas, path_sonido = :path_sonido, music_driver = :music_driver, "
		"enable_gs = :enable_gs, multi_midi = :multi_midi, native_mt32 = :native_mt32, mute = :mute, master_volume = :master_volume, "
		"music_volume = :music_volume, sfx_volume = :sfx_volume, speech_volume = :speech_volume, speech_mute = :speech_mute, tempo = :tempo, "
		"talkspeed = :talkspeed, debuglevel = :debuglevel, cdrom = :cdrom, joystick_num = :joystick_num, output_rate = :output_rate, "
		"midi_gain = :midi_gain, copy_protection = :copy_protection, sound_font = :sound_font, walkspeed = :walkspeed, opl_driver = :opl_driver, "
		"disable_dithering = :disable_dithering, alt_intro = :alt_intro, boot_param = :boot_param "
		"WHERE id = :id;";

	query.prepare( strSQL );
//	query.bindValue(":idgrl"            , cfgSvm.idgrl             );
//	query.bindValue(":idcat"            , cfgSvm.idcat             );
	query.bindValue(":game"             , cfgSvm.game              );
	query.bindValue(":game_label"       , cfgSvm.game_label        );
	query.bindValue(":language"         , cfgSvm.language          );
	query.bindValue(":subtitles"        , cfgSvm.subtitles         );
	query.bindValue(":platform"         , cfgSvm.platform          );
	query.bindValue(":gfx_mode"         , cfgSvm.gfx_mode          );
	query.bindValue(":render_mode"      , cfgSvm.render_mode       );
	query.bindValue(":fullscreen"       , cfgSvm.fullscreen        );
	query.bindValue(":aspect_ratio"     , cfgSvm.aspect_ratio      );
	query.bindValue(":path_game"        , cfgSvm.path_game         );
	query.bindValue(":path_extra"       , cfgSvm.path_extra        );
	query.bindValue(":path_save"        , cfgSvm.path_save         );
	query.bindValue(":path_capturas"    , cfgSvm.path_capturas     );
	query.bindValue(":path_sonido"      , cfgSvm.path_sonido       );
	query.bindValue(":music_driver"     , cfgSvm.music_driver      );
	query.bindValue(":enable_gs"        , cfgSvm.enable_gs         );
	query.bindValue(":multi_midi"       , cfgSvm.multi_midi        );
	query.bindValue(":native_mt32"      , cfgSvm.native_mt32       );
	query.bindValue(":mute"             , cfgSvm.mute              );
	query.bindValue(":master_volume"    , cfgSvm.master_volume     );
	query.bindValue(":music_volume"     , cfgSvm.music_volume      );
	query.bindValue(":sfx_volume"       , cfgSvm.sfx_volume        );
	query.bindValue(":speech_volume"    , cfgSvm.speech_volume     );
	query.bindValue(":speech_mute"      , cfgSvm.speech_mute       );
	query.bindValue(":tempo"            , cfgSvm.tempo             );
	query.bindValue(":talkspeed"        , cfgSvm.talkspeed         );
	query.bindValue(":debuglevel"       , cfgSvm.debuglevel        );
	query.bindValue(":cdrom"            , cfgSvm.cdrom             );
	query.bindValue(":joystick_num"     , cfgSvm.joystick_num      );
	query.bindValue(":output_rate"      , cfgSvm.output_rate       );
	query.bindValue(":midi_gain"        , cfgSvm.midi_gain         );
	query.bindValue(":copy_protection"  , cfgSvm.copy_protection   );
	query.bindValue(":sound_font"       , cfgSvm.sound_font        );
	query.bindValue(":walkspeed"        , cfgSvm.walkspeed         );
	query.bindValue(":opl_driver"       , cfgSvm.opl_driver        );
	query.bindValue(":disable_dithering", cfgSvm.disable_dithering );
	query.bindValue(":alt_intro"        , cfgSvm.alt_intro         );
	query.bindValue(":boot_param"       , cfgSvm.boot_param        );
	query.bindValue(":id"               , cfgSvm.id                );
	query.exec();

	return chequearQuery(query);
}
// FIN Config ScummVM --------------------------------------------------------------------------------------------

// INICIO Config VDMSound ----------------------------------------------------------------------------------------
stConfigVDMSound dbSql::showConfg_VDMSound(QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	stConfigVDMSound cfgVdms;

	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" LIMIT 0,1;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			QStringList list_program       = query.record().value("program").toString().split("|");
			QStringList list_vdms_debug    = query.record().value("vdms_debug").toString().split("|");
			QStringList list_winnt_dos     = query.record().value("winnt_dos").toString().split("|");
			QStringList list_winnt_dosbox  = query.record().value("winnt_dosbox").toString().split("|");
			QStringList list_winnt_storage = query.record().value("winnt_storage").toString().split("|");

			cfgVdms.id              = query.record().value("id").toString();
			cfgVdms.idgrl           = query.record().value("idgrl").toString();
			cfgVdms.idcat           = query.record().value("idcat").toString();

			cfgVdms.path_conf       = query.record().value("path_conf").toString();
			cfgVdms.path_exe        = query.record().value("path_exe").toString();
			cfgVdms.program_1       = ""+ list_program.value(0, "");
			cfgVdms.program_2       = ""+ list_program.value(1, "");
			cfgVdms.vdms_debug_1    = ""+ list_vdms_debug.value(0, "no");
			cfgVdms.vdms_debug_2    = ""+ list_vdms_debug.value(1, "");
			cfgVdms.winnt_dos_1     = ""+ list_winnt_dos.value(0, "no");
			cfgVdms.winnt_dos_2     = ""+ list_winnt_dos.value(1, "");
			cfgVdms.winnt_dosbox_1  = ""+ list_winnt_dosbox.value(0, "yes");
			cfgVdms.winnt_dosbox_2  = ""+ list_winnt_dosbox.value(1, "no");
			cfgVdms.winnt_dosbox_3  = ""+ list_winnt_dosbox.value(2, "no");
			cfgVdms.winnt_storage_1 = ""+ list_winnt_storage.value(0, "yes");
			cfgVdms.winnt_storage_2 = ""+ list_winnt_storage.value(1, "no");
		} else
			cfgVdms = fGrl->getDefectVDMSound();
	} else
		cfgVdms = fGrl->getDefectVDMSound();

	return cfgVdms;
}

QString dbSql::insertaVdms(stConfigVDMSound cfgVdms)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_emu_vdmsound ("
			"idgrl, idcat, path_conf, path_exe, program, vdms_debug, winnt_dos, winnt_dosbox, winnt_storage "
		") VALUES ("
			":idgrl, :idcat, :path_conf, :path_exe, :program, :vdms_debug, :winnt_dos, :winnt_dosbox, :winnt_storage "
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":idgrl"        , cfgVdms.idgrl );
	query.bindValue(":idcat"        , cfgVdms.idcat );
	query.bindValue(":path_conf"    , cfgVdms.path_conf );
	query.bindValue(":path_exe"     , cfgVdms.path_exe );
	query.bindValue(":program"      , cfgVdms.program_1 +"|"+ cfgVdms.program_2 );
	query.bindValue(":vdms_debug"   , cfgVdms.vdms_debug_1 +"|"+ cfgVdms.vdms_debug_2 );
	query.bindValue(":winnt_dos"    , cfgVdms.winnt_dos_1 +"|"+ cfgVdms.winnt_dos_2 );
	query.bindValue(":winnt_dosbox" , cfgVdms.winnt_dosbox_1 +"|"+ cfgVdms.winnt_dosbox_2 +"|"+ cfgVdms.winnt_dosbox_3 );
	query.bindValue(":winnt_storage", cfgVdms.winnt_storage_1 +"|"+ cfgVdms.winnt_storage_2 );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaVdms(stConfigVDMSound cfgVdms)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_emu_vdmsound SET "
		"path_conf = :path_conf, path_exe = :path_exe, program = :program, vdms_debug = :vdms_debug, "
		"winnt_dos = :winnt_dos, winnt_dosbox = :winnt_dosbox, winnt_storage = :winnt_storage "
		"WHERE id = :id;";

	query.prepare( strSQL );
	query.bindValue(":path_conf"    , cfgVdms.path_conf );
	query.bindValue(":path_exe"     , cfgVdms.path_exe );
	query.bindValue(":program"      , cfgVdms.program_1 +"|"+ cfgVdms.program_2 );
	query.bindValue(":vdms_debug"   , cfgVdms.vdms_debug_1 +"|"+ cfgVdms.vdms_debug_2 );
	query.bindValue(":winnt_dos"    , cfgVdms.winnt_dos_1 +"|"+ cfgVdms.winnt_dos_2 );
	query.bindValue(":winnt_dosbox" , cfgVdms.winnt_dosbox_1 +"|"+ cfgVdms.winnt_dosbox_2 +"|"+ cfgVdms.winnt_dosbox_3 );
	query.bindValue(":winnt_storage", cfgVdms.winnt_storage_1 +"|"+ cfgVdms.winnt_storage_2 );
	query.bindValue(":id"           , cfgVdms.id );
	query.exec();

	return chequearQuery(query);
}
// FIN Config VDMSound ------------------------------------------------------------------------------------------

// INICIO Archivos ----------------------------------------------------------------------------------------------
// Muestra los Archivos en un QTreeWidget
void dbSql::cargarDatosFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);

	if( sql_where.isEmpty() )
		sql_where = "ORDER BY nombre ASC";

	twFiles->clear();
	query.exec("SELECT id, idgrl, idcat, nombre, crc, descripcion, path, size, tipo FROM dbgrl_files WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if( query.first())
	{
		do {
			QTreeWidgetItem *item_files = new QTreeWidgetItem( twFiles );

			QString file_ico = query.record().value("tipo").toString();
			if( file_ico == "manual" )
				item_files->setIcon( 0, QIcon(fGrl->Theme() +"img16/datos.png") );
			else if( file_ico == "pdf" )
				item_files->setIcon( 0, QIcon(fGrl->Theme() +"img16/pdf.png") );
			else if( file_ico == "ruleta" )
				item_files->setIcon( 0, QIcon(fGrl->Theme() +"img16/ruleta.png") );
			else if( file_ico == "archivo" )
				item_files->setIcon( 0, QIcon(fGrl->Theme() +"img16/archivos.png") );
			else
				item_files->setIcon( 0, QIcon(fGrl->Theme() +"img16/archivos.png") );

			item_files->setTextAlignment(1, Qt::AlignCenter);
			item_files->setTextAlignment(3, Qt::AlignCenter);
			item_files->setText( 0, query.record().value("nombre").toString()      );
			item_files->setText( 1, query.record().value("crc").toString()         );
			item_files->setText( 2, query.record().value("descripcion").toString() );
			item_files->setText( 3, fGrl->covertir_bytes( query.record().value("size").toFloat() ) );
			item_files->setText( 4, query.record().value("path").toString()        );
			item_files->setText( 5, query.record().value("tipo").toString()        );
			item_files->setText( 6, query.record().value("id").toString()          );
			item_files->setText( 7, query.record().value("idgrl").toString()       );
			item_files->setText( 8, query.record().value("idcat").toString()       );
			item_files->setText( 9, query.record().value("size").toString()        );
		} while ( query.next() );
	}
}

void dbSql::insertaFiles(QTreeWidget *twFiles, QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	QString strSQL = "INSERT INTO dbgrl_files ("
			"idgrl, idcat, nombre, crc, descripcion, path, size, tipo"
		") VALUES ("
			":idgrl, :idcat, :nombre, :crc, :descripcion, :path, :size, :tipo"
		")";

	const int count_file = twFiles->topLevelItemCount();
	for (int num_file = 0; num_file < count_file; ++num_file)
	{
		QTreeWidgetItem *item_files = twFiles->topLevelItem( num_file );
		query.prepare( strSQL );
		query.bindValue(":idgrl"      , IDgrl               );
		query.bindValue(":idcat"      , IDcat               );
		query.bindValue(":nombre"     , item_files->text(0) );
		query.bindValue(":crc"        , item_files->text(1) );
		query.bindValue(":descripcion", item_files->text(2) );
		query.bindValue(":path"       , item_files->text(4) );
		query.bindValue(":size"       , item_files->text(9) );
		query.bindValue(":tipo"       , item_files->text(5) );
		query.exec();

		if( chequearQuery(query) )
			item_files->setText(6, lastInsertId(query, "id"));
	}
}

QString dbSql::insertaUnFiles(stDatosFiles datos)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_files ("
			"idgrl, idcat, nombre, crc, descripcion, path, size, tipo"
		") VALUES ("
			":idgrl, :idcat, :nombre, :crc, :descripcion, :path, :size, :tipo"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":idgrl"      , datos.idgrl       );
	query.bindValue(":idcat"      , datos.idcat       );
	query.bindValue(":nombre"     , datos.nombre      );
	query.bindValue(":crc"        , datos.crc         );
	query.bindValue(":descripcion", datos.descripcion );
	query.bindValue(":path"       , datos.path        );
	query.bindValue(":size"       , datos.size        );
	query.bindValue(":tipo"       , datos.tipo        );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaFiles(stDatosFiles datos)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_files SET nombre = :nombre, crc = :crc, descripcion = :descripcion, path = :path, size = :size, tipo = :tipo WHERE id = :id;");
	query.bindValue(":nombre"     , datos.nombre      );
	query.bindValue(":crc"        , datos.crc         );
	query.bindValue(":descripcion", datos.descripcion );
	query.bindValue(":path"       , datos.path        );
	query.bindValue(":size"       , datos.size        );
	query.bindValue(":tipo"       , datos.tipo        );
	query.bindValue(":id"         , datos.id          );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarFiles(QString IDFiles)
{
	if( IDFiles.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("DELETE FROM dbgrl_files WHERE id="+ IDFiles +";");

		return chequearQuery(query);
	}
}
// FIN Archivos -------------------------------------------------------------------------------------------------

// INICIO URL ---------------------------------------------------------------------------------------------------
// Muestra las URLs en un QList
QList<QString> dbSql::getDatosUrls(QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);
	QList<QString> url_list;

	if( sql_where.isEmpty() )
		sql_where = "ORDER BY url ASC";

	url_list.clear();
	query.exec("SELECT id, url FROM dbgrl_urls WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if( query.first())
	{
		do {
			url_list.insert(query.record().value("id").toInt(), query.record().value("url").toString());
		} while ( query.next() );
	}

	return url_list;
}

void dbSql::cargarDatosUrls(QTreeWidget *twUrls, QString IDgrl, QString IDcat, QString sql_where)
{
	QSqlQuery query(sqldb);

	if( sql_where.isEmpty() )
		sql_where = "ORDER BY url ASC";

	twUrls->clear();
	query.exec("SELECT id, idgrl, idcat, url, descripcion FROM dbgrl_urls WHERE idgrl="+ IDgrl +" AND idcat="+ IDcat +" "+ sql_where);
	if( query.first())
	{
		do {
			QTreeWidgetItem *item_url = new QTreeWidgetItem( twUrls );
			item_url->setText( 0, query.record().value("url").toString()         );
			item_url->setIcon( 0, QIcon(fGrl->Theme() +"img16/html.png")         );
			item_url->setText( 1, query.record().value("descripcion").toString() );
			item_url->setText( 2, query.record().value("id").toString()          );
			item_url->setText( 3, query.record().value("idgrl").toString()       );
			item_url->setText( 4, query.record().value("idcat").toString()       );
		} while ( query.next() );
	}
}

void dbSql::insertaURL(QTreeWidget *twUrls, QString IDgrl, QString IDcat)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_urls (idgrl, idcat, url, descripcion) VALUES (:idgrl, :idcat, :url, :descripcion)"+ qpsql_return_id;

	const int count_url = twUrls->topLevelItemCount();
	for (int num_url = 0; num_url < count_url; ++num_url)
	{
		QTreeWidgetItem *item_url = twUrls->topLevelItem( num_url );
		query.prepare( strSQL );
		query.bindValue(":idgrl"      , IDgrl              );
		query.bindValue(":idcat"      , IDcat              );
		query.bindValue(":url"        , item_url->text(0) );
		query.bindValue(":descripcion", item_url->text(1) );
		query.exec();

		if( chequearQuery(query) )
			item_url->setText(2, lastInsertId(query, "id"));
	}
}

QString dbSql::insertaUnURL(stDatosUrls datos)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_urls (idgrl, idcat, url, descripcion) VALUES (:idgrl, :idcat, :url, :descripcion)"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":idgrl"      , datos.idgrl       );
	query.bindValue(":idcat"      , datos.idcat       );
	query.bindValue(":url"        , datos.url         );
	query.bindValue(":descripcion", datos.descripcion );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaURL(stDatosUrls datos)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_urls SET url = :url, descripcion = :descripcion WHERE id = :id;");
	query.bindValue(":url"        , datos.url         );
	query.bindValue(":descripcion", datos.descripcion );
	query.bindValue(":id"         , datos.id          );
	query.exec();

	return chequearQuery(query);
}

bool dbSql::eliminarURL(QString IDUrl)
{
	if( IDUrl.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("DELETE FROM dbgrl_urls WHERE id="+ IDUrl +";");

		return chequearQuery(query);
	}
}
// FIN URL ------------------------------------------------------------------------------------------------------

// INICIO Categorias --------------------------------------------------------------------------------------------
QList<stGrlCats> dbSql::getCategorias(QString sql_where)
{
	QSqlQuery query(sqldb);
	stGrlCats cat;
	QList<stGrlCats> cat_list;

	cat_list.clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias "+ sql_where +" ORDER BY orden ASC;");
	if( chequearQuery(query) )
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
				cat_list.insert(cat.id.toInt(), cat);
			} while ( query.next() );
		} else {
			cat.tabla    = "dbgrl";
			cat.titulo   = "Db GR-lida";
			cat.img      = "pc.png";
			cat.orden    = "0";
			cat.emu_show = "all";
			cat.id       = insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
			cat_list.insert(cat.id.toInt(), cat);
		}
	} else {
		cat.tabla    = "dbgrl";
		cat.titulo   = "Db GR-lida";
		cat.img      = "pc.png";
		cat.orden    = "0";
		cat.emu_show = "all";
		cat.id       = insertaCategoria(cat.tabla, cat.titulo, cat.img, cat.orden.toInt(), cat.emu_show);
		cat_list.insert(cat.id.toInt(), cat);
	}

	return cat_list;
}

QHash<int, stGrlCats> dbSql::getCategoriasLista(QString sql_where)
{
	QList<stGrlCats> list = getCategorias(sql_where);
	QHash<int, stGrlCats> cat_list;
	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
		cat_list.insert(list.at(i).id.toInt(), list.at(i));

	return cat_list;
}

void dbSql::cargarCategorias(QTreeWidget *twCat)
{
	QSqlQuery query(sqldb);

	twCat->clear();
	query.exec("SELECT id, tabla, titulo, img, orden, emu_show FROM dbgrl_categorias ORDER BY orden ASC;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			do {
				QString total_tabla = fGrl->IntToStr( getCount(query.record().value("tabla").toString()) );

				QTreeWidgetItem *item_cat = new QTreeWidgetItem( twCat );
				item_cat->setTextAlignment(2, Qt::AlignCenter);

				if( QFile::exists(fGrl->ThemeApp() +"img32_cat/"+ query.record().value("img").toString()) )
					item_cat->setIcon( 0, QIcon(fGrl->ThemeApp() +"img32_cat/"+ query.record().value("img").toString()) );
				else
					item_cat->setIcon( 0, QIcon(":/img32_cat/sinimg.png") );

				item_cat->setText( 0, query.record().value("titulo").toString()   );
				item_cat->setText( 1, query.record().value("tabla").toString()    );
				item_cat->setText( 2, total_tabla                                 );
				item_cat->setText( 3, query.record().value("id").toString()       );
				item_cat->setText( 4, query.record().value("img").toString()      );
				item_cat->setText( 5, query.record().value("orden").toString()    );
				item_cat->setText( 6, query.record().value("emu_show").toString() );
			} while ( query.next() );
		} else {
			QString total_tabla = fGrl->IntToStr( getCount("dbgrl") );
			QTreeWidgetItem *item_cat = new QTreeWidgetItem( twCat );
			item_cat->setTextAlignment(2, Qt::AlignCenter);

			if( QFile::exists(fGrl->ThemeApp() +"img32_cat/pc.png") )
				item_cat->setIcon( 0, QIcon(fGrl->ThemeApp() +"img32_cat/pc.png") );
			else
				item_cat->setIcon( 0, QIcon(":/img32_cat/sinimg.png") );

			item_cat->setText( 0, "Db GR-lida" );
			item_cat->setText( 1, "dbgrl"      );
			item_cat->setText( 2, total_tabla  );
			item_cat->setText( 3, "1"          );
			item_cat->setText( 4, "pc.png"     );
			item_cat->setText( 5, "0"          );
			item_cat->setText( 6, "all"        );
		}
	} else {
		QString total_tabla = fGrl->IntToStr( getCount("dbgrl") );
		QTreeWidgetItem *item_cat = new QTreeWidgetItem( twCat );
		item_cat->setTextAlignment(2, Qt::AlignCenter);

		if( QFile::exists(fGrl->ThemeApp() +"img32_cat/pc.png") )
			item_cat->setIcon( 0, QIcon(fGrl->ThemeApp() +"img32_cat/pc.png") );
		else
			item_cat->setIcon( 0, QIcon(":/img32_cat/sinimg.png") );


		item_cat->setText( 0, "Db GR-lida" );
		item_cat->setText( 1, "dbgrl"      );
		item_cat->setText( 2, total_tabla  );
		item_cat->setText( 3, "1"          );
		item_cat->setText( 4, "pc.png"     );
		item_cat->setText( 5, "0"          );
		item_cat->setText( 6, "all"        );
	}
}

QString dbSql::insertaCategoria(QString tabla, QString titulo, QString img, int orden, QString emu_show)
{
	QSqlQuery query(sqldb);
	QString qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";

	query.prepare("INSERT INTO dbgrl_categorias (tabla, titulo, img, orden, emu_show) VALUES (:tabla, :titulo, :img, :orden, :emu_show)"+ qpsql_return_id);
	query.bindValue(":tabla"   , tabla    );
	query.bindValue(":titulo"  , titulo   );
	query.bindValue(":img"     , img      );
	query.bindValue(":orden"   , orden    );
	query.bindValue(":emu_show", emu_show );
	query.exec();

	if( chequearQuery(query) )
	{
		QString last_id = lastInsertId(query, "id");

		crearTabla(tabla);

		fGrl->comprobarDirectorio(fGrl->DirApp() +"capturas/"+ tabla +"/");
		fGrl->comprobarDirectorio(fGrl->DirApp() +"confdbx/" + tabla +"/");
		fGrl->comprobarDirectorio(fGrl->DirApp() +"confvdms/"+ tabla +"/");
		fGrl->comprobarDirectorio(fGrl->DirApp() +"covers/"  + tabla +"/");
		fGrl->comprobarDirectorio(fGrl->DirApp() +"thumbs/"  + tabla +"/");

		return last_id;
	} else
		return "";
}

bool dbSql::actualizaCategoria(QString IDcat, QString tabla, QString titulo, QString img, QString emu_show)
{
	QSqlQuery query(sqldb);

	query.prepare("UPDATE dbgrl_categorias SET tabla = :tabla, titulo = :titulo, img = :img, emu_show = :emu_show WHERE id = :id;");
	query.bindValue(":tabla"   , tabla    );
	query.bindValue(":titulo"  , titulo   );
	query.bindValue(":img"     , img      );
	query.bindValue(":emu_show", emu_show );
	query.bindValue(":id"      , IDcat    );
	query.exec();

	return chequearQuery(query);
}

void dbSql::actualizaOrdenCategoria(QTreeWidget *twCat)
{
	QSqlQuery query(sqldb);

	const int count_orden = twCat->topLevelItemCount();
	if( count_orden > 0 )
	{
		for (int orden = 0; orden < count_orden; ++orden)
		{
			QTreeWidgetItem *item_cat = twCat->topLevelItem( orden );
			item_cat->setText(5, fGrl->IntToStr(orden) );

			query.prepare("UPDATE dbgrl_categorias SET orden = :orden WHERE id = :id;");
			query.bindValue(":orden", orden );
			query.bindValue(":id"   , item_cat->text(3) );
			query.exec();

			chequearQuery(query);
		}
	}
}

bool dbSql::eliminarCategoria(QString IDcat)
{
	if( IDcat.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);
		QHash<int, stGrlCats> cat_list = getCategoriasLista("WHERE id = "+ IDcat);
		int id_cat = IDcat.toInt();

		if( cat_list[id_cat].tabla == "dbgrl" )
			return false;
		else {
			QString IDdbx, IDvdms;
		// Datos
			query.exec("DELETE FROM dbgrl_urls WHERE idcat="+ IDcat);
			query.exec("DELETE FROM dbgrl_files WHERE idcat="+ IDcat);
		// ScummVM
			query.exec("DELETE FROM dbgrl_emu_scummvm WHERE idcat="+ IDcat);
		// DOSBox
			query.exec("SELECT id, path_conf FROM dbgrl_emu_dosbox WHERE idcat="+ IDcat);
			if( query.first())
			{
				do {
					IDdbx = query.record().value("id").toString();
					QFile::remove(fGrl->DirApp() +"confdbx/"+ cat_list[id_cat].tabla +"/"+ query.record().value("path_conf").toString());
					query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+ IDdbx);
				} while ( query.next() );
			}
			query.exec("DELETE FROM dbgrl_emu_dosbox WHERE idcat="+ IDcat);
		// VDMSound
			query.exec("SELECT id, path_conf FROM dbgrl_emu_vdmsound WHERE idcat="+ IDcat);
			if( query.first())
			{
				do {
					IDvdms = query.record().value("id").toString();
					QFile::remove(fGrl->DirApp() +"confvdms/"+ cat_list[id_cat].tabla +"/"+ query.record().value("path_conf").toString());
				} while ( query.next() );
			}
			query.exec("DELETE FROM dbgrl_emu_vdmsound WHERE idcat="+ IDcat);
		// Tabla y Categoria
			query.exec("DROP TABLE "+ cat_list[id_cat].tabla +";");
			query.exec("DELETE FROM dbgrl_categorias WHERE id = "+ IDcat +";");

			return chequearQuery(query);
		}
	}
}
// FIN Categorias -----------------------------------------------------------------------------------------------

// INICIO MenuNav -----------------------------------------------------------------------------------------------
void dbSql::cargarMenuNav(QTreeWidget *twMnuNav, QString tabla, bool isOpt)
{
	QSqlQuery query(sqldb);
	QTreeWidgetItem *item;
	QStringList lista;
	QFile file_in;
	QString linea, sLng, titulo, col_value, tmp_col_value, col_name, sql_query, tmp_query, archivo, img, orden, dir_img;
	QFont m_font;
	int col_img   = 1;
	int total     = 0;
	int total_sub = 0;

	query.exec("SELECT id, titulo, col_value, col_name, sql_query, archivo, img, orden, show, expanded FROM dbgrl_mnu_nav ORDER BY orden ASC;");
	if( chequearQuery(query) )
	{
		if( query.first() )
		{
			do {
				if( fGrl->StrToBool(query.record().value("show").toString()) || isOpt )
				{
					titulo    = query.record().value("titulo").toString();
					col_value = query.record().value("col_value").toString();
					col_name  = query.record().value("col_name").toString();
					sql_query = query.record().value("sql_query").toString();
					archivo   = query.record().value("archivo").toString();
					img       = query.record().value("img").toString();
					orden     = query.record().value("orden").toString();
					sLng      = fGrl->getArchivoIsLng(archivo);

					if( isOpt )
					{
						item = new QTreeWidgetItem( twMnuNav );
						if( QFile::exists(fGrl->Theme() +"img16/"+ img) )
							item->setIcon( 0, QIcon(fGrl->Theme() +"img16/"+ img) );
						else
							item->setIcon( 0, QIcon(fGrl->Theme() +"img16/sinimg.png") );

						item->setText( 0, titulo    );
						item->setText( 1, col_value );
						item->setText( 2, col_name  );
						item->setText( 3, sql_query );
						item->setText( 4, archivo   );
						item->setText( 5, query.record().value("show").toString()     );
						item->setText( 6, query.record().value("expanded").toString() );
						item->setText( 7, img       );
						item->setText( 8, orden     );
						item->setText( 9, query.record().value("id").toString() );
					} else {
						item = new QTreeWidgetItem( twMnuNav );
						if( QFile::exists(fGrl->Theme() +"img16/"+ img) )
							item->setIcon( 0, QIcon(fGrl->Theme() +"img16/"+ img) );
						else
							item->setIcon( 0, QIcon(fGrl->Theme() +"img16/sinimg.png") );
						item->setTextAlignment(1, Qt::AlignCenter);
						item->setText( 0, titulo );

						if( archivo == "defecto" )
						{
							tmp_col_value = col_value;
							if( tmp_col_value == "{value}" )
								tmp_col_value = titulo;

							tmp_query = sql_query;
							tmp_query.replace("{col_value}", tmp_col_value.replace("'", "''")).replace("{col_name}", col_name);

							total = getCount(tabla, tmp_query, col_name, tmp_col_value);
							m_font.setBold( (total > 0) ? true : false);
							item->setFont( 1, m_font );
							item->setText( 1, fGrl->IntToStr(total) );
							item->setText( 2, tmp_query );
						} else {
							item->setText( 1, "" );
							item->setText( 2, "" );

							if( archivo == "idiomas.txt" || archivo == "svm_idioma.txt" || archivo == "dbx_keyboardlayout.txt" )
								dir_img = fGrl->Theme() +"img_lng/";
							else if( archivo == "rating.txt" )
								dir_img = fGrl->Theme() +"images/";
							else
								dir_img = fGrl->Theme() +"img16/";

							if( fGrl->getArchivoIs3Col(archivo) )
								col_img = 2;
							else
								col_img = 1;

							file_in.setFileName(fGrl->DirApp() +"datos/"+ sLng + archivo);
							if( !file_in.exists() )
								file_in.setFileName(":/datos/"+ sLng + archivo);

							if( file_in.open(QIODevice::ReadOnly | QIODevice::Text) )
							{
								QTextStream in( &file_in );
								in.setCodec("UTF-8");

							//	while ( !in.atEnd() )
								do {
									linea = in.readLine();
									if( !linea.isEmpty() )
									{
										lista = linea.split("|");

										titulo = lista.value(0);
										if( !titulo.isEmpty() )
										{
											tmp_col_value = col_value;
											if( tmp_col_value == "{value}" )
												tmp_col_value = titulo;

											tmp_query = sql_query;
											tmp_query.replace("{col_value}", tmp_col_value.replace("'", "''")).replace("{col_name}", col_name);
											img       = lista.value(col_img);

											if( img.isEmpty() )
												img = "sinimg.png";

											total_sub = getCount(tabla, tmp_query, col_name, tmp_col_value);
											m_font.setBold( (total_sub > 0) ? true : false);

											QTreeWidgetItem *sub_item = new QTreeWidgetItem( item );
											if( QFile::exists(dir_img + img) )
												sub_item->setIcon( 0, QIcon(dir_img + img) );
											else
												sub_item->setIcon( 0, QIcon(dir_img +"sinimg.png") );
											sub_item->setTextAlignment(1, Qt::AlignCenter);
											sub_item->setFont( 1, m_font    );
											sub_item->setText( 0, titulo    );
											sub_item->setText( 1, fGrl->IntToStr(total_sub) );
											sub_item->setText( 2, tmp_query );
										}
									}
								} while ( !linea.isNull() );
							}
							file_in.close();
							item->setExpanded( fGrl->StrToBool(query.record().value("expanded").toString()) );
						}
					}
				}
			} while ( query.next() );
		}
	}
}

QString dbSql::insertaMenuNav(QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool show, bool expanded)
{
	QSqlQuery query(sqldb);
	QString strSQL, qpsql_return_id;

	qpsql_return_id = (dbType=="QPSQL") ? " RETURNING id" : "";
	strSQL = "INSERT INTO dbgrl_mnu_nav ("
			"titulo, col_value, col_name, sql_query, archivo, img, orden, show, expanded"
		") VALUES ("
			":titulo, :col_value, :col_name, :sql_query, :archivo, :img, :orden, :show, :expanded"
		")"+ qpsql_return_id;

	query.prepare( strSQL );
	query.bindValue(":titulo"   , titulo    );
	query.bindValue(":col_value", col_value );
	query.bindValue(":col_name" , col_name  );
	query.bindValue(":sql_query", sql_query );
	query.bindValue(":archivo"  , archivo   );
	query.bindValue(":img"      , img       );
	query.bindValue(":orden"    , orden     );
	query.bindValue(":show"     , fGrl->BoolToStr(show)     );
	query.bindValue(":expanded" , fGrl->BoolToStr(expanded) );
	query.exec();

	if( chequearQuery(query) )
		return lastInsertId(query, "id");
	else
		return "";
}

bool dbSql::actualizaMenuNav(QString IDmnu, QString titulo, QString col_value, QString col_name, QString sql_query, QString archivo, QString img, int orden, bool show, bool expanded)
{
	QSqlQuery query(sqldb);
	QString strSQL = "UPDATE dbgrl_mnu_nav SET "
		"titulo = :titulo, col_value = :col_value, col_name = :col_name, sql_query = :sql_query, "
		"archivo = :archivo, img = :img, orden = :orden, show = :show, expanded = :expanded "
		"WHERE id = :id;";

	query.prepare( strSQL );
	query.bindValue(":titulo"   , titulo    );
	query.bindValue(":col_value", col_value );
	query.bindValue(":col_name" , col_name  );
	query.bindValue(":sql_query", sql_query );
	query.bindValue(":archivo"  , archivo   );
	query.bindValue(":img"      , img       );
	query.bindValue(":orden"    , orden     );
	query.bindValue(":show"     , fGrl->BoolToStr(show)     );
	query.bindValue(":expanded" , fGrl->BoolToStr(expanded) );
	query.bindValue(":id"       , IDmnu  );
	query.exec();

	return chequearQuery(query);
}

void dbSql::actualizaOrdenMenuNav(QTreeWidget *twMnuNav)
{
	QSqlQuery query(sqldb);

	const int count_orden = twMnuNav->topLevelItemCount();
	if( count_orden > 0 )
	{
		for (int orden = 0; orden < count_orden; ++orden)
		{
			QTreeWidgetItem *item_mnu_nav = twMnuNav->topLevelItem( orden );
			item_mnu_nav->setText(8, fGrl->IntToStr(orden) );

			query.prepare("UPDATE dbgrl_mnu_nav SET orden = :orden WHERE id = :id;");
			query.bindValue(":orden", orden );
			query.bindValue(":id"   , item_mnu_nav->text(9) );
			query.exec();

			chequearQuery(query);
		}
	}
}

bool dbSql::eliminarMenuNav(QString IDmnu)
{
	if( IDmnu.isEmpty() )
		return false;
	else {
		QSqlQuery query(sqldb);

		query.exec("DELETE FROM dbgrl_mnu_nav WHERE id="+ IDmnu +";");

		return chequearQuery(query);
	}
}
// FIN MenuNav --------------------------------------------------------------------------------------------------
