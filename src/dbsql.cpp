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

#include "dbsql.h"

dbSql::dbSql(QString db_type, QString db_server, QString db_host, QString db_name, QString db_username, QString db_password, QString db_port)
{
	dbType = db_type;
	ok_OpenDB = false;
	if( dbType == "QMYSQL" || dbType == "QPSQL" )
	{
		sqldb = QSqlDatabase::addDatabase( dbType );
		sqldb.setHostName( db_server   ); // localhost, archivo
		sqldb.setDatabaseName( db_name ); // Nombre base de datos
		sqldb.setUserName( db_username ); // Nombre del Usuario
		sqldb.setPassword( db_password ); // Password
		sqldb.setPort( db_port.toInt() );
		ok_OpenDB = sqldb.open();
	} else {
		sqldb = QSqlDatabase::addDatabase("QSQLITE");
		sqldb.setDatabaseName( db_host );
		ok_OpenDB = sqldb.open();
	}
/*
db_type = "QSQLITE"		; // QSQLITE, QMYSQL, QPSQL
db_host = "database.db"	; // archivo
db_server = "localhost"	; // localhost
db_name = "gr_lida"		; // Nombre base de datos
db_username = "root"	; // Usuario
db_password = "123456"	; // Password
*/
}

bool dbSql::dbisOpen()
{
	return ok_OpenDB;
}

void dbSql::dbClose()
{
	sqldb.close();
}

dbSql::~dbSql()
{
	if( sqldb.isOpen()) sqldb.close();
}

bool dbSql::Chequear_Query(QSqlQuery q)
{
	if( q.lastError().type() != QSqlError::NoError )
	{
		QMessageBox::critical( 0, QCoreApplication::applicationName(), "SQL: " + q.lastQuery() + "\nError: " + q.lastError().text() );
		return false;
	}
	return true;
}

int dbSql::getCount(QString stTable, QString stWhere)
{
	QSqlQuery result( "SELECT COUNT(*) FROM "+ stTable + " " + stWhere );
	if( Chequear_Query(result) )
	{
		result.next();
		int count = result.record().value(0).toInt();
		result.clear();
		return count;
	}
	result.clear();
	return -1;
}

QString dbSql::ItemIDIndex(QString SQLtabla, QString SQLindex, QString column)
{
	QSqlQuery query;
	query.clear();
	query.exec("SELECT "+column+" FROM "+SQLtabla+" WHERE idgrl="+SQLindex+" LIMIT 0,1");
	query.first();

	if(column!="*")
		return query.record().value(column).toString();
	else
		return query.value(0).toString();
}

void dbSql::ItemEliminar(const QString IDgrl)
{
	QSqlQuery query;
	QString IDdbx, temp_FileConf;

	query.clear();
	query.exec("DELETE FROM dbgrl WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_url WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_file WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_scummvm WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("SELECT id, idgrl, path_conf FROM dbgrl_emu_dosbox WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();
		IDdbx.clear();
		IDdbx = query.record().value("id").toString();
		temp_FileConf.clear();
		temp_FileConf = query.record().value("path_conf").toString();
		eliminararchivo( "confdbx/" + temp_FileConf );
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+IDdbx);
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_dosbox WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("SELECT id, idgrl, path_conf FROM dbgrl_emu_vdmsound WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();
		temp_FileConf.clear();
		temp_FileConf = query.record().value("path_conf").toString();
		eliminararchivo( "confvdms/" + temp_FileConf );
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_vdmsound WHERE idgrl="+IDgrl);
	query.clear();
}

void dbSql::eliminararchivo(QString archivo)
{
// Directorio de configuracion para el DOSBox
	QString stArchivoConfg = fGrl.GRlidaHomePath() + archivo;
	QFile f( stArchivoConfg );
	if( f.exists() )
		f.remove();
}

// Crea las Tablas de la Base de Datos si es Necesario
void dbSql::CrearTablas()
{
	QSqlQuery query;
	QStringList tables = sqldb.tables();
	QString tipo_id_sql;

	tipo_id_sql.clear();
	if( dbType == "QMYSQL" )
		tipo_id_sql = "INT(11) UNSIGNED NOT NULL AUTO_INCREMENT";
	else if( dbType == "QPSQL" )
		tipo_id_sql = "SERIAL";
	else if( dbType == "QSQLITE" )
		tipo_id_sql = "INTEGER NOT NULL";
	else
		tipo_id_sql = "INTEGER NOT NULL";

// Añade las distintas tablas si no están disponibles
// Tabla principal - dbgrl
	if( !tables.contains("dbgrl") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl` ("
		"	`idgrl`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`icono`					varchar(255) NOT NULL default '',"
		"	`titulo`				varchar(255) NOT NULL default '',"
		"	`subtitulo`				varchar(255) NOT NULL default '',"
		"	`genero`				varchar(255) NOT NULL default '',"
		"	`compania`				varchar(255) NOT NULL default '',"
		"	`desarrollador`			varchar(255) NOT NULL default '',"
		"	`tema`					varchar(255) NOT NULL default '',"
		"	`perspectiva`			varchar(255) NOT NULL default '',"
		"	`idioma`				varchar(50) NOT NULL default '',"
		"	`formato`				varchar(50) NOT NULL default '',"
		"	`anno`					varchar(4) NOT NULL default '',"
		"	`numdisc`				varchar(50) NOT NULL default '',"
		"	`sistemaop`				varchar(200) NOT NULL default '',"
		"	`tamano`				varchar(50) NOT NULL default '',"
		"	`graficos`				integer NOT NULL default 1,"
		"	`sonido`				integer NOT NULL default 1,"
		"	`jugabilidad`			integer NOT NULL default 1,"
		"	`original`				varchar(5) NOT NULL default 'false',"
		"	`estado`				varchar(255) NOT NULL default '',"
		"	`thumbs`				varchar(255) NOT NULL default '',"
		"	`cover_front`			varchar(255) NOT NULL default '',"
		"	`cover_back`			varchar(255) NOT NULL default '',"
		"	`fecha`					varchar(50) NOT NULL default '',"
		"	`tipo_emu`				varchar(255) NOT NULL default 'datos',"
		"	`comentario`			longtext,"
		"	`favorito`				varchar(5) NOT NULL default 'false',"
		"	`rating`				integer NOT NULL default 0,"
		"	`edad_recomendada`		varchar(2) NOT NULL default 'nd',"
		"	`usuario`				varchar(255) NOT NULL default '',"
		"	`path_exe`				varchar(255) NOT NULL default '',"
		"	`parametros_exe`		varchar(255) NOT NULL default ''"
		");");
	}

// Tabla - dbgrl_emu_dosbox
	if( !tables.contains("dbgrl_emu_dosbox") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_emu_dosbox` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`idgrl`					integer NOT NULL default 1,"
		"	`sdl_fullscreen`		varchar(5) NOT NULL default 'false',"
		"	`sdl_fulldouble`		varchar(5) NOT NULL default 'false',"
		"	`sdl_fullfixed`			varchar(5) NOT NULL default 'false',"		// DOSBox 0.63
		"	`sdl_fullresolution`	varchar(15) NOT NULL default 'original',"
		"	`sdl_windowresolution`	varchar(15) NOT NULL default 'original',"
		"	`sdl_output`			varchar(15) NOT NULL default 'surface',"
		"	`sdl_hwscale`			varchar(15) NOT NULL default '1.00',"		// DOSBox 0.63
		"	`sdl_autolock`			varchar(5) NOT NULL default 'true',"
		"	`sdl_sensitivity`		varchar(3) NOT NULL default '100',"
		"	`sdl_waitonerror`		varchar(5) NOT NULL default 'true',"
		"	`sdl_priority`			varchar(20) NOT NULL default 'higher,normal',"
		"	`sdl_mapperfile`		varchar(255) NOT NULL default 'mapper.txt',"
		"	`sdl_usescancodes`		varchar(5) NOT NULL default 'true',"
		"	`dosbox_language`		varchar(255) NOT NULL default '',"
		"	`dosbox_machine`		varchar(20) NOT NULL default 'vga',"
		"	`dosbox_captures`		varchar(255) NOT NULL default 'capture',"
		"	`dosbox_memsize`		varchar(4) NOT NULL default '16',"
		"	`render_frameskip`		varchar(2) NOT NULL default '0',"
		"	`render_aspect`			varchar(5) NOT NULL default 'false',"
		"	`render_scaler`			varchar(15) NOT NULL default 'normal2x',"
		"	`cpu_core`				varchar(10) NOT NULL default 'auto',"
		"	`cpu_cputype`			varchar(20) NOT NULL default 'auto',"
		"	`cpu_cycles`			varchar(10) NOT NULL default 'auto',"
		"	`cpu_cycleup`			varchar(10) NOT NULL default '500',"
		"	`cpu_cycledown`			varchar(10) NOT NULL default '20',"
		"	`mixer_nosound`			varchar(5) NOT NULL default 'false',"
		"	`mixer_rate`			varchar(10) NOT NULL default '22050',"
		"	`mixer_blocksize`		varchar(10) NOT NULL default '2048',"
		"	`mixer_prebuffer`		varchar(10) NOT NULL default '10',"
		"	`midi_mpu401`			varchar(15) NOT NULL default 'intelligent',"
		"	`midi_intelligent`		varchar(5) NOT NULL default 'true',"			// DOSBox 0.63
		"	`midi_device`			varchar(10) NOT NULL default 'default',"
		"	`midi_config`			varchar(255) NOT NULL default '',"
		"	`midi_mt32rate`			varchar(10) NOT NULL default '22050',"		// DOSBox 0.65
		"	`sblaster_sbtype`		varchar(10) NOT NULL default 'sb16',"
		"	`sblaster_sbbase`		varchar(4) NOT NULL default '220',"
		"	`sblaster_irq`			varchar(4) NOT NULL default '7',"
		"	`sblaster_dma`			varchar(4) NOT NULL default '1',"
		"	`sblaster_hdma`			varchar(4) NOT NULL default '5',"
		"	`sblaster_mixer`		varchar(5) NOT NULL default 'true',"
		"	`sblaster_oplmode`		varchar(10) NOT NULL default 'auto',"
		"	`sblaster_oplemu`		varchar(10) NOT NULL default 'default',"
		"	`sblaster_oplrate`		varchar(10) NOT NULL default '22050',"
		"	`gus_gus`				varchar(5) NOT NULL default 'true',"
		"	`gus_gusrate`			varchar(10) NOT NULL default '22050',"
		"	`gus_gusbase`			varchar(4) NOT NULL default '240',"
		"	`gus_irq1`				varchar(4) NOT NULL default '5',"
		"	`gus_irq2`				varchar(4) NOT NULL default '5',"
		"	`gus_dma1`				varchar(4) NOT NULL default '3',"
		"	`gus_dma2`				varchar(4) NOT NULL default '3',"
		"	`gus_ultradir`			varchar(255) NOT NULL default 'C:\\ULTRASND',"
		"	`speaker_pcspeaker`		varchar(5) NOT NULL default 'true',"
		"	`speaker_pcrate`		varchar(10) NOT NULL default '22050',"
		"	`speaker_tandy`			varchar(5) NOT NULL default 'auto',"		// DOSBox 0.65
		"	`speaker_tandyrate`		varchar(10) NOT NULL default '22050',"
		"	`speaker_disney`		varchar(5) NOT NULL default 'true',"
		"	`joystick_type`			varchar(50) not null default 'auto',"		// DOSBox 0.70
		"	`joystick_timed`		varchar(5) not null default 'true',"		// DOSBox 0.70
		"	`joystick_autofire`		varchar(5) not null default 'false',"		// DOSBox 0.70
		"	`joystick_swap34`		varchar(5) not null default 'false',"		// DOSBox 0.70
		"	`joystick_buttonwrap`	varchar(5) not null default 'true',"		// DOSBox 0.70
		"	`modem_modem`			varchar(5) NOT NULL default 'false',"		// DOSBox 0.63
		"	`modem_comport`			varchar(4) NOT NULL default '2',"			// DOSBox 0.63
		"	`modem_listenport`		varchar(4) NOT NULL default '23',"			// DOSBox 0.63
		"	`dserial_directserial`	varchar(5) NOT NULL default 'false',"		// DOSBox 0.63
		"	`dserial_comport`		varchar(2) NOT NULL default '1',"			// DOSBox 0.63
		"	`dserial_realport`		varchar(5) NOT NULL default 'COM1',"		// DOSBox 0.63
		"	`dserial_defaultbps`	varchar(5) NOT NULL default '1200',"		// DOSBox 0.63
		"	`dserial_parity`		varchar(2) NOT NULL default 'N',"			// DOSBox 0.63
		"	`dserial_bytesize`		varchar(2) NOT NULL default '8',"			// DOSBox 0.63
		"	`dserial_stopbit`		varchar(2) NOT NULL default '1',"			// DOSBox 0.63
		"	`serial_1`				text,"
		"	`serial_2`				text,"
		"	`serial_3`				text,"
		"	`serial_4`				text,"
		"	`dos_xms`				varchar(5) NOT NULL default 'true',"
		"	`dos_ems`				varchar(5) NOT NULL default 'true',"
		"	`dos_umb`				varchar(5) NOT NULL default 'true',"
		"	`dos_keyboardlayout`	varchar(5) NOT NULL default 'auto',"
		"	`ipx_ipx`				varchar(5) NOT NULL default 'true',"
		"	`autoexec`				longtext,"
		"	`opt_autoexec`			varchar(5) NOT NULL default 'false',"
		"	`opt_loadfix`			varchar(5) NOT NULL default 'false',"
		"	`opt_loadfix_mem`		varchar(5) NOT NULL default '64',"
		"	`opt_consola_dbox`		varchar(5) NOT NULL default 'false',"
		"	`opt_cerrar_dbox`		varchar(5) NOT NULL default 'true',"
		"	`opt_cycle_sincronizar`	varchar(5) NOT NULL default 'false',"
		"	`path_conf`				varchar(255) NOT NULL default '',"
		"	`path_sonido`			varchar(255) NOT NULL default 'waves',"
		"	`path_exe`				varchar(255) NOT NULL default '',"
		"	`path_setup`			varchar(255) NOT NULL default '',"
		"	`parametros_exe`		varchar(255) NOT NULL default '',"
		"	`parametros_setup`		varchar(255) NOT NULL default ''"
		");");
	}

// Tabla - dbgrl_emu_dosbox_mount
	if( !tables.contains("dbgrl_emu_dosbox_mount") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_emu_dosbox_mount` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`id_dosbox`				integer NOT NULL default 1,"
		"	`id_lista`				varchar(255) NOT NULL default '0',"
		"	`path`					varchar(255) NOT NULL default 'C:\\',"
		"	`label`					varchar(255) NOT NULL default '',"
		"	`tipo_as`				varchar(50) NOT NULL default 'drive',"
		"	`letter`				varchar(255) NOT NULL default 'C',"
		"	`indx_cd`				varchar(255) NOT NULL default '',"
		"	`opt_mount`				varchar(255) NOT NULL default '',"
		"	`io_ctrl`				varchar(50) NOT NULL default '',"
		"	`select_mount`			varchar(10) NOT NULL default 'x'"
		");");
	}

// Tabla - dbgrl_emu_scummvm
	if( !tables.contains("dbgrl_emu_scummvm") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_emu_scummvm` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`idgrl`					integer NOT NULL default 1,"
		"	`game`					varchar(255) NOT NULL default '',"
		"	`game_label`			varchar(255) NOT NULL default '',"
		"	`language`				varchar(4) NOT NULL default 'es',"
		"	`subtitles`				varchar(5) NOT NULL default 'true',"
		"	`platform`				varchar(50) NOT NULL default 'pc',"
		"	`gfx_mode`				varchar(50) NOT NULL default '2x',"
		"	`render_mode`			varchar(50) NOT NULL default '',"
		"	`fullscreen`			varchar(5) NOT NULL default 'false',"
		"	`aspect_ratio`			varchar(5) NOT NULL default 'true',"
		"	`path`					varchar(255) NOT NULL default '',"
		"	`path_setup`			varchar(255) NOT NULL default '',"
		"	`path_extra`			varchar(255) NOT NULL default '',"
		"	`path_save`				varchar(255) NOT NULL default '',"
		"	`path_capturas`			varchar(255) NOT NULL default '',"
		"	`path_sonido`			varchar(255) NOT NULL default '',"
		"	`music_driver`			varchar(50) NOT NULL default 'adlib',"
		"	`enable_gs`				varchar(5) NOT NULL default 'true',"
		"	`multi_midi`			varchar(5) NOT NULL default 'true',"
		"	`native_mt32`			varchar(5) NOT NULL default 'true',"
		"	`master_volume`			integer NOT NULL default 192,"
		"	`music_volume`			integer NOT NULL default 192,"
		"	`sfx_volume`			integer NOT NULL default 192,"
		"	`speech_volume`			integer NOT NULL default 192,"
		"	`tempo`					integer NOT NULL default 100,"
		"	`talkspeed`				integer NOT NULL default 60,"
		"	`debuglevel`			integer NOT NULL default 0,"
		"	`cdrom`					integer NOT NULL default 0,"
		"	`joystick_num`			integer NOT NULL default 0,"
		"	`output_rate`			varchar(10) NOT NULL default '<defecto>',"
		"	`midi_gain`				integer NOT NULL default 100,"
		"	`copy_protection`		varchar(5) NOT NULL default 'false',"
		"	`sound_font`			varchar(255) NOT NULL default '',"
		"	`walkspeed`				integer NOT NULL default 0,"
		"	`opl_driver`			varchar(10) NOT NULL default 'auto'"
		");");
	}

// Tabla - dbgrl_emu_vdmsound
	if( !tables.contains("dbgrl_emu_vdmsound") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_emu_vdmsound` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`idgrl`					integer NOT NULL default 1,"
		"	`path_conf`				text,"
		"	`path_exe`				text,"
		"	`program`				text,"
		"	`vdms_debug`			text,"
		"	`winnt_dos`				text,"
		"	`winnt_dosbox`			text,"
		"	`winnt_storage`			text"
		");");
	}

// Tabla - dbgrl_file
	if( !tables.contains("dbgrl_file") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_file` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`idgrl`					integer NOT NULL default 1,"
		"	`nombre`				varchar(255) NOT NULL default '',"
		"	`crc`					varchar(8) NOT NULL default '00000000',"
		"	`descripcion`			varchar(255) NOT NULL default '',"
		"	`path`					varchar(255) NOT NULL default '',"
		"	`size`					varchar(50) NOT NULL default '',"
		"	`tipo`					varchar(50) NOT NULL default ''"
		");");
	}

// Tabla - dbgrl_url
	if( !tables.contains("dbgrl_url") )
	{
		query.clear();
		query.exec(
		"CREATE TABLE `dbgrl_url` ("
		"	`id`					" + tipo_id_sql + " PRIMARY KEY,"
		"	`idgrl`					integer NOT NULL default 1,"
		"	`url`					varchar(255) NOT NULL default '',"
		"	`descripcion`			varchar(255) NOT NULL default ''"
		");");
	}

//	Añade las columnas si no están disponibles
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'favorito' varchar(5) NOT NULL DEFAULT 'false';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'rating' integer NOT NULL DEFAULT 0;");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'usuario' varchar(255) NOT NULL default '';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'path_exe' varchar(255) NOT NULL default '';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'parametros_exe' varchar(255) NOT NULL default '';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'edad_recomendada' varchar(2) NOT NULL default 'nd';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'perspectiva' varchar(255) NOT NULL default '';");

	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_dosbox' ADD COLUMN 'cpu_cputype' varchar(20) NOT NULL DEFAULT 'auto';");
	query.exec("ALTER TABLE 'dbgrl_emu_dosbox' ADD COLUMN 'sblaster_oplemu' varchar(10) NOT NULL DEFAULT 'default';");

	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'output_rate' varchar(10) NOT NULL DEFAULT '<defecto>';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'midi_gain' integer NOT NULL default 100;");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'copy_protection' varchar(5) NOT NULL DEFAULT 'false';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'sound_font' varchar(255) NOT NULL DEFAULT '';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'game_label' varchar(255) NOT NULL DEFAULT '';");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'walkspeed' integer NOT NULL DEFAULT 0;");
	query.clear();
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'opl_driver' varchar(10) NOT NULL default 'auto';");

	query.clear();
	query.exec("ALTER TABLE 'dbgrl_file' ADD COLUMN 'tipo' varchar(50) NOT NULL DEFAULT '';");
}

QHash<QString, QString> dbSql::show_Datos(QString IDgrl)
{
	QHash<QString, QString> tmpDatosJuego;

	QSqlQuery query;
	query.clear();
	query.exec("SELECT * FROM dbgrl WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();

	tmpDatosJuego["Dat_idgrl"]         = query.record().value("idgrl").toString();
	tmpDatosJuego["Dat_icono"]         = query.record().value("icono").toString();
	tmpDatosJuego["Dat_titulo"]        = query.record().value("titulo").toString();
	tmpDatosJuego["Dat_subtitulo"]     = query.record().value("subtitulo").toString();
	tmpDatosJuego["Dat_genero"]        = query.record().value("genero").toString();
	tmpDatosJuego["Dat_compania"]      = query.record().value("compania").toString();
	tmpDatosJuego["Dat_desarrollador"] = query.record().value("desarrollador").toString();
	tmpDatosJuego["Dat_tema"]          = query.record().value("tema").toString();
	tmpDatosJuego["Dat_perspectiva"]   = query.record().value("perspectiva").toString();
	tmpDatosJuego["Dat_idioma"]        = query.record().value("idioma").toString();
	tmpDatosJuego["Dat_formato"]       = query.record().value("formato").toString();
	tmpDatosJuego["Dat_anno"]          = query.record().value("anno").toString();
	tmpDatosJuego["Dat_numdisc"]       = query.record().value("numdisc").toString();
	tmpDatosJuego["Dat_sistemaop"]     = query.record().value("sistemaop").toString();
	tmpDatosJuego["Dat_tamano"]        = query.record().value("tamano").toString();
	tmpDatosJuego["Dat_graficos"]      = query.record().value("graficos").toString();
	tmpDatosJuego["Dat_sonido"]        = query.record().value("sonido").toString();
	tmpDatosJuego["Dat_jugabilidad"]   = query.record().value("jugabilidad").toString();
	tmpDatosJuego["Dat_original"]      = query.record().value("original").toString();
	tmpDatosJuego["Dat_estado"]        = query.record().value("estado").toString();
	tmpDatosJuego["Dat_thumbs"]        = query.record().value("thumbs").toString();
	tmpDatosJuego["Dat_cover_front"]   = query.record().value("cover_front").toString();
	tmpDatosJuego["Dat_cover_back"]    = query.record().value("cover_back").toString();
	tmpDatosJuego["Dat_fecha"]         = query.record().value("fecha").toString();
	tmpDatosJuego["Dat_tipo_emu"]      = query.record().value("tipo_emu").toString();
	tmpDatosJuego["Dat_comentario"]    = query.record().value("comentario").toString();
	tmpDatosJuego["Dat_favorito"]      = query.record().value("favorito").toString();
	tmpDatosJuego["Dat_rating"]        = query.record().value("rating").toString();
	tmpDatosJuego["Dat_edad_recomendada"]= query.record().value("edad_recomendada").toString();
	tmpDatosJuego["Dat_usuario"]       = query.record().value("usuario").toString();
	tmpDatosJuego["Dat_path_exe"]      = query.record().value("path_exe").toString();
	tmpDatosJuego["Dat_parametros_exe"]= query.record().value("parametros_exe").toString();

	return tmpDatosJuego;
}

QString dbSql::ItemInsertaDatos(const QHash<QString, QString> datos)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl (");
	strSQL.append("icono, titulo, subtitulo, genero, compania, desarrollador, tema, perspectiva, idioma, formato, anno, numdisc, ");
	strSQL.append("sistemaop, tamano, graficos, sonido, jugabilidad, original, estado, thumbs, cover_front, cover_back, ");
	strSQL.append("fecha, tipo_emu, comentario, favorito, rating, edad_recomendada, usuario, path_exe, parametros_exe ");
	strSQL.append(") VALUES ( ");
	strSQL.append(":icono, :titulo, :subtitulo, :genero, :compania, :desarrollador, :tema, :perspectiva, :idioma, :formato, :anno, :numdisc, ");
	strSQL.append(":sistemaop, :tamano, :graficos, :sonido, :jugabilidad, :original, :estado, :thumbs, :cover_front, :cover_back, ");
	strSQL.append(":fecha, :tipo_emu, :comentario, :favorito, :rating, :edad_recomendada, :usuario, :path_exe, :parametros_exe )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":icono"        , datos["Dat_icono"]         );
	query.bindValue(":titulo"       , datos["Dat_titulo"]        );
	query.bindValue(":subtitulo"    , datos["Dat_subtitulo"]     );
	query.bindValue(":genero"       , datos["Dat_genero"]        );
	query.bindValue(":compania"     , datos["Dat_compania"]      );
	query.bindValue(":desarrollador", datos["Dat_desarrollador"] );
	query.bindValue(":tema"         , datos["Dat_tema"]          );
	query.bindValue(":perspectiva"  , datos["Dat_perspectiva"]   );
	query.bindValue(":idioma"       , datos["Dat_idioma"]        );
	query.bindValue(":formato"      , datos["Dat_formato"]       );
	query.bindValue(":anno"         , datos["Dat_anno"]          );
	query.bindValue(":numdisc"      , datos["Dat_numdisc"]       );
	query.bindValue(":sistemaop"    , datos["Dat_sistemaop"]     );
	query.bindValue(":tamano"       , datos["Dat_tamano"]        );
	query.bindValue(":graficos"     , datos["Dat_graficos"]      );
	query.bindValue(":sonido"       , datos["Dat_sonido"]        );
	query.bindValue(":jugabilidad"  , datos["Dat_jugabilidad"]   );
	query.bindValue(":original"     , datos["Dat_original"]      );
	query.bindValue(":estado"       , datos["Dat_estado"]        );
	query.bindValue(":thumbs"       , datos["Dat_thumbs"]        );
	query.bindValue(":cover_front"  , datos["Dat_cover_front"]   );
	query.bindValue(":cover_back"   , datos["Dat_cover_back"]    );
	query.bindValue(":fecha"        , datos["Dat_fecha"]         );
	query.bindValue(":tipo_emu"     , datos["Dat_tipo_emu"]      );
	query.bindValue(":comentario"   , datos["Dat_comentario"]    );
	query.bindValue(":favorito"     , datos["Dat_favorito"]      );
	query.bindValue(":rating"       , datos["Dat_rating"]        );
	query.bindValue(":edad_recomendada", datos["Dat_edad_recomendada"]);
	query.bindValue(":usuario"         , datos["Dat_usuario"]       );
	query.bindValue(":path_exe"        , datos["Dat_path_exe"]      );
	query.bindValue(":parametros_exe"  , datos["Dat_parametros_exe"]);
	query.exec();

	if( Chequear_Query(query) )
		return query.lastInsertId().toString();
	else
		return "";
}

void dbSql::ItemActualizaDatos(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl SET ");
	strSQL.append("icono = :icono, titulo = :titulo, subtitulo = :subtitulo, genero = :genero, ");
	strSQL.append("compania  = :compania, desarrollador = :desarrollador, tema = :tema, perspectiva = :perspectiva, idioma = :idioma, ");
	strSQL.append("formato = :formato, anno = :anno, numdisc = :numdisc, sistemaop = :sistemaop, ");
	strSQL.append("tamano = :tamano, graficos = :graficos, sonido = :sonido, jugabilidad = :jugabilidad, ");
	strSQL.append("original = :original, estado = :estado, thumbs = :thumbs, cover_front = :cover_front, ");
	strSQL.append("cover_back = :cover_back, comentario = :comentario, favorito = :favorito, rating = :rating, edad_recomendada = :edad_recomendada, ");
	strSQL.append("usuario = :usuario, path_exe = :path_exe, parametros_exe = :parametros_exe WHERE idgrl = :idgrl;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":icono"        , datos["Dat_icono"]         );
	query.bindValue(":titulo"       , datos["Dat_titulo"]        );
	query.bindValue(":subtitulo"    , datos["Dat_subtitulo"]     );
	query.bindValue(":genero"       , datos["Dat_genero"]        );
	query.bindValue(":compania"     , datos["Dat_compania"]      );
	query.bindValue(":desarrollador", datos["Dat_desarrollador"] );
	query.bindValue(":tema"         , datos["Dat_tema"]          );
	query.bindValue(":perspectiva"  , datos["Dat_perspectiva"]   );
	query.bindValue(":idioma"       , datos["Dat_idioma"]        );
	query.bindValue(":formato"      , datos["Dat_formato"]       );
	query.bindValue(":anno"         , datos["Dat_anno"]          );
	query.bindValue(":numdisc"      , datos["Dat_numdisc"]       );
	query.bindValue(":sistemaop"    , datos["Dat_sistemaop"]     );
	query.bindValue(":tamano"       , datos["Dat_tamano"]        );
	query.bindValue(":graficos"     , datos["Dat_graficos"]      );
	query.bindValue(":sonido"       , datos["Dat_sonido"]        );
	query.bindValue(":jugabilidad"  , datos["Dat_jugabilidad"]   );
	query.bindValue(":original"     , datos["Dat_original"]      );
	query.bindValue(":estado"       , datos["Dat_estado"]        );
	query.bindValue(":thumbs"       , datos["Dat_thumbs"]        );
	query.bindValue(":cover_front"  , datos["Dat_cover_front"]   );
	query.bindValue(":cover_back"   , datos["Dat_cover_back"]    );
//	query.bindValue(":fecha"        , datos["Dat_fecha"]         );
//	query.bindValue(":tipo_emu"     , datos["Dat_tipo_emu"]      );
	query.bindValue(":comentario"   , datos["Dat_comentario"]    );
	query.bindValue(":favorito"     , datos["Dat_favorito"]      );
	query.bindValue(":rating"       , datos["Dat_rating"]        );
	query.bindValue(":edad_recomendada", datos["Dat_edad_recomendada"]);
	query.bindValue(":usuario"         , datos["Dat_usuario"]       );
	query.bindValue(":path_exe"        , datos["Dat_path_exe"]      );
	query.bindValue(":parametros_exe"  , datos["Dat_parametros_exe"]);
	query.bindValue(":idgrl"           , IDgrl                      );
	query.exec();
}

void dbSql::ItemActualizaDatosItem(const QString m_key, const QString m_value, const QString IDgrl)
{
	QSqlQuery query;
	query.prepare("UPDATE dbgrl SET "+m_key+" = :"+m_key+" WHERE idgrl = :idgrl;");
	query.bindValue(":"+m_key , m_value);
	query.bindValue(":idgrl"  , IDgrl  );
	query.exec();
}

void dbSql::ItemActualizaDatosRating(const QString new_rating , const QString IDgrl )
{
	QSqlQuery query;
	query.clear();
	query.prepare("UPDATE dbgrl SET rating = :rating WHERE idgrl = :idgrl;");
	query.bindValue(":rating" , new_rating);
	query.bindValue(":idgrl"  , IDgrl     );
	query.exec();
}

void dbSql::ItemActualizaDatosFavorito(const QString EstadoFav , const QString IDgrl )
{
	QSqlQuery query;
	query.clear();
	query.prepare("UPDATE dbgrl SET favorito = :favorito WHERE idgrl = :idgrl;");
	query.bindValue(":favorito" , EstadoFav);
	query.bindValue(":idgrl"    , IDgrl    );
	query.exec();
}

QHash<QString, QString> dbSql::showConfg_DOSBox(QString IDgrl)
{
	QHash<QString, QString> tmpDosBox;

	QSqlQuery query;
	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();

	tmpDosBox["Dbx_id"]                   = query.record().value("id").toString();
	tmpDosBox["Dbx_idgrl"]                = query.record().value("idgrl").toString();
// [sdl]
	tmpDosBox["Dbx_sdl_fullscreen"]       = query.record().value("sdl_fullscreen").toString();
	tmpDosBox["Dbx_sdl_fulldouble"]       = query.record().value("sdl_fulldouble").toString();
	tmpDosBox["Dbx_sdl_fullfixed"]        = query.record().value("sdl_fullfixed").toString();
	tmpDosBox["Dbx_sdl_fullresolution"]   = query.record().value("sdl_fullresolution").toString();
	tmpDosBox["Dbx_sdl_windowresolution"] = query.record().value("sdl_windowresolution").toString();
	tmpDosBox["Dbx_sdl_output"]           = query.record().value("sdl_output").toString();
	tmpDosBox["Dbx_sdl_hwscale"]          = query.record().value("sdl_hwscale").toString();
	tmpDosBox["Dbx_sdl_autolock"]         = query.record().value("sdl_autolock").toString();
	tmpDosBox["Dbx_sdl_sensitivity"]      = query.record().value("sdl_sensitivity").toString();
	tmpDosBox["Dbx_sdl_waitonerror"]      = query.record().value("sdl_waitonerror").toString();
	tmpDosBox["Dbx_sdl_priority"]         = query.record().value("sdl_priority").toString();
	tmpDosBox["Dbx_sdl_mapperfile"]       = query.record().value("sdl_mapperfile").toString();
	tmpDosBox["Dbx_sdl_usescancodes"]     = query.record().value("sdl_usescancodes").toString();
// [dosbox]
	tmpDosBox["Dbx_dosbox_language"]      = query.record().value("dosbox_language").toString();
	tmpDosBox["Dbx_dosbox_machine"]       = query.record().value("dosbox_machine").toString();
	tmpDosBox["Dbx_dosbox_captures"]      = query.record().value("dosbox_captures").toString();
	tmpDosBox["Dbx_dosbox_memsize"]       = query.record().value("dosbox_memsize").toString();
// [render]
	tmpDosBox["Dbx_render_frameskip"]     = query.record().value("render_frameskip").toString();
	tmpDosBox["Dbx_render_aspect"]        = query.record().value("render_aspect").toString();
	tmpDosBox["Dbx_render_scaler"]        = query.record().value("render_scaler").toString();
// [cpu]
	tmpDosBox["Dbx_cpu_core"]             = query.record().value("cpu_core").toString();
	tmpDosBox["Dbx_cpu_cputype"]             = query.record().value("cpu_cputype").toString();
	tmpDosBox["Dbx_cpu_cycles"]           = query.record().value("cpu_cycles").toString();
	tmpDosBox["Dbx_cpu_cycleup"]          = query.record().value("cpu_cycleup").toString();
	tmpDosBox["Dbx_cpu_cycledown"]        = query.record().value("cpu_cycledown").toString();
// [mixer]
	tmpDosBox["Dbx_mixer_nosound"]        = query.record().value("mixer_nosound").toString();
	tmpDosBox["Dbx_mixer_rate"]           = query.record().value("mixer_rate").toString();
	tmpDosBox["Dbx_mixer_blocksize"]      = query.record().value("mixer_blocksize").toString();
	tmpDosBox["Dbx_mixer_prebuffer"]      = query.record().value("mixer_prebuffer").toString();
// [midi]
	tmpDosBox["Dbx_midi_mpu401"]          = query.record().value("midi_mpu401").toString();
	tmpDosBox["Dbx_midi_intelligent"]     = query.record().value("midi_intelligent").toString();
	tmpDosBox["Dbx_midi_device"]          = query.record().value("midi_device").toString();
	tmpDosBox["Dbx_midi_config"]          = query.record().value("midi_config").toString();
	tmpDosBox["Dbx_midi_mt32rate"]        = query.record().value("midi_mt32rate").toString();
// [sblaster]
	tmpDosBox["Dbx_sblaster_sbtype"]      = query.record().value("sblaster_sbtype").toString();
	tmpDosBox["Dbx_sblaster_sbbase"]      = query.record().value("sblaster_sbbase").toString();
	tmpDosBox["Dbx_sblaster_irq"]         = query.record().value("sblaster_irq").toString();
	tmpDosBox["Dbx_sblaster_dma"]         = query.record().value("sblaster_dma").toString();
	tmpDosBox["Dbx_sblaster_hdma"]        = query.record().value("sblaster_hdma").toString();
	tmpDosBox["Dbx_sblaster_mixer"]       = query.record().value("sblaster_mixer").toString();
	tmpDosBox["Dbx_sblaster_oplmode"]     = query.record().value("sblaster_oplmode").toString();
	tmpDosBox["Dbx_sblaster_oplemu"]      = query.record().value("sblaster_oplemu").toString();
	tmpDosBox["Dbx_sblaster_oplrate"]     = query.record().value("sblaster_oplrate").toString();
// [gus]
	tmpDosBox["Dbx_gus_gus"]              = query.record().value("gus_gus").toString();
	tmpDosBox["Dbx_gus_gusrate"]          = query.record().value("gus_gusrate").toString();
	tmpDosBox["Dbx_gus_gusbase"]          = query.record().value("gus_gusbase").toString();
	tmpDosBox["Dbx_gus_irq1"]             = query.record().value("gus_irq1").toString();
	tmpDosBox["Dbx_gus_irq2"]             = query.record().value("gus_irq2").toString();
	tmpDosBox["Dbx_gus_dma1"]             = query.record().value("gus_dma1").toString();
	tmpDosBox["Dbx_gus_dma2"]             = query.record().value("gus_dma2").toString();
	tmpDosBox["Dbx_gus_ultradir"]         = query.record().value("gus_ultradir").toString();
// [speaker]
	tmpDosBox["Dbx_speaker_pcspeaker"]    = query.record().value("speaker_pcspeaker").toString();
	tmpDosBox["Dbx_speaker_pcrate"]       = query.record().value("speaker_pcrate").toString();
	tmpDosBox["Dbx_speaker_tandy"]        = query.record().value("speaker_tandy").toString();
	tmpDosBox["Dbx_speaker_tandyrate"]    = query.record().value("speaker_tandyrate").toString();
	tmpDosBox["Dbx_speaker_disney"]       = query.record().value("speaker_disney").toString();
// [joystick]
	tmpDosBox["Dbx_joystick_type"]        = query.record().value("joystick_type").toString();
	tmpDosBox["Dbx_joystick_timed"]       = query.record().value("joystick_timed").toString();
	tmpDosBox["Dbx_joystick_autofire"]    = query.record().value("joystick_autofire").toString();
	tmpDosBox["Dbx_joystick_swap34"]      = query.record().value("joystick_swap34").toString();
	tmpDosBox["Dbx_joystick_buttonwrap"]  = query.record().value("joystick_buttonwrap").toString();
// [modem]
	tmpDosBox["Dbx_modem_modem"]          = query.record().value("modem_modem").toString();
	tmpDosBox["Dbx_modem_comport"]        = query.record().value("modem_comport").toString();
	tmpDosBox["Dbx_modem_listenport"]     = query.record().value("modem_listenport").toString();
// [dserial]
	tmpDosBox["Dbx_dserial_directserial"] = query.record().value("dserial_directserial").toString();
	tmpDosBox["Dbx_dserial_comport"]      = query.record().value("dserial_comport").toString();
	tmpDosBox["Dbx_dserial_realport"]     = query.record().value("dserial_realport").toString();
	tmpDosBox["Dbx_dserial_defaultbps"]   = query.record().value("dserial_defaultbps").toString();
	tmpDosBox["Dbx_dserial_parity"]       = query.record().value("dserial_parity").toString();
	tmpDosBox["Dbx_dserial_bytesize"]     = query.record().value("dserial_bytesize").toString();
	tmpDosBox["Dbx_dserial_stopbit"]      = query.record().value("dserial_stopbit").toString();
	tmpDosBox["Dbx_serial_1"]             = query.record().value("serial_1").toString();
	tmpDosBox["Dbx_serial_2"]             = query.record().value("serial_2").toString();
	tmpDosBox["Dbx_serial_3"]             = query.record().value("serial_3").toString();
	tmpDosBox["Dbx_serial_4"]             = query.record().value("serial_4").toString();
// [dos]
	tmpDosBox["Dbx_dos_xms"]              = query.record().value("dos_xms").toString();
	tmpDosBox["Dbx_dos_ems"]              = query.record().value("dos_ems").toString();
	tmpDosBox["Dbx_dos_umb"]              = query.record().value("dos_umb").toString();
	tmpDosBox["Dbx_dos_keyboardlayout"]   = query.record().value("dos_keyboardlayout").toString();
// [ipx]
	tmpDosBox["Dbx_ipx_ipx"]              = query.record().value("ipx_ipx").toString();
// [autoexec]
	tmpDosBox["Dbx_autoexec"]             = query.record().value("autoexec").toString();
// Opciones
	tmpDosBox["Dbx_opt_autoexec"]         = query.record().value("opt_autoexec").toString();
	tmpDosBox["Dbx_opt_loadfix"]          = query.record().value("opt_loadfix").toString();
	tmpDosBox["Dbx_opt_loadfix_mem"]      = query.record().value("opt_loadfix_mem").toString();
	tmpDosBox["Dbx_opt_consola_dbox"]     = query.record().value("opt_consola_dbox").toString();
	tmpDosBox["Dbx_opt_cerrar_dbox"]      = query.record().value("opt_cerrar_dbox").toString();
	tmpDosBox["Dbx_opt_cycle_sincronizar"]= query.record().value("opt_cycle_sincronizar").toString();
// Otras opciones
	tmpDosBox["Dbx_path_conf"]            = query.record().value("path_conf").toString();
	tmpDosBox["Dbx_path_sonido"]          = query.record().value("path_sonido").toString();
	tmpDosBox["Dbx_path_exe"]             = query.record().value("path_exe").toString();
	tmpDosBox["Dbx_path_setup"]           = query.record().value("path_setup").toString();
	tmpDosBox["Dbx_parametros_exe"]       = query.record().value("parametros_exe").toString();
	tmpDosBox["Dbx_parametros_setup"]     = query.record().value("parametros_setup").toString();

	return tmpDosBox;
}

QString dbSql::ItemInsertaDbx(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_dosbox (");
	strSQL.append("idgrl, sdl_fullscreen, sdl_fulldouble, sdl_fullfixed, sdl_fullresolution, sdl_windowresolution, sdl_output, ");
	strSQL.append("sdl_hwscale, sdl_autolock, sdl_sensitivity, sdl_waitonerror, sdl_priority, sdl_mapperfile, sdl_usescancodes, ");
	strSQL.append("dosbox_language, dosbox_machine, dosbox_captures, dosbox_memsize, render_frameskip, render_aspect, render_scaler, ");
	strSQL.append("cpu_core, cpu_cputype, cpu_cycles, cpu_cycleup, cpu_cycledown, mixer_nosound, mixer_rate, mixer_blocksize, mixer_prebuffer, ");
	strSQL.append("midi_mpu401, midi_intelligent, midi_device, midi_config, midi_mt32rate, sblaster_sbtype, sblaster_sbbase, sblaster_irq, ");
	strSQL.append("sblaster_dma, sblaster_hdma, sblaster_mixer, sblaster_oplmode, sblaster_oplemu, sblaster_oplrate, gus_gus, gus_gusrate, gus_gusbase, ");
	strSQL.append("gus_irq1, gus_irq2, gus_dma1, gus_dma2, gus_ultradir, speaker_pcspeaker, speaker_pcrate, speaker_tandy, speaker_tandyrate, ");
	strSQL.append("speaker_disney, joystick_type, joystick_timed, joystick_autofire, joystick_swap34, joystick_buttonwrap, ");
	strSQL.append("modem_modem, modem_comport, modem_listenport, dserial_directserial, dserial_comport, ");
	strSQL.append("dserial_realport, dserial_defaultbps, dserial_parity, dserial_bytesize, dserial_stopbit, serial_1, serial_2, serial_3, ");
	strSQL.append("serial_4, dos_xms, dos_ems, dos_umb, dos_keyboardlayout, ipx_ipx, autoexec, opt_autoexec, opt_loadfix, opt_loadfix_mem, opt_consola_dbox, ");
	strSQL.append("opt_cerrar_dbox, opt_cycle_sincronizar, path_conf, path_sonido, path_exe, path_setup, parametros_exe, parametros_setup ");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :sdl_fullscreen, :sdl_fulldouble, :sdl_fullfixed, :sdl_fullresolution, :sdl_windowresolution, :sdl_output, ");
	strSQL.append(":sdl_hwscale, :sdl_autolock, :sdl_sensitivity, :sdl_waitonerror, :sdl_priority, :sdl_mapperfile, :sdl_usescancodes, ");
	strSQL.append(":dosbox_language, :dosbox_machine, :dosbox_captures, :dosbox_memsize, :render_frameskip, :render_aspect, :render_scaler, ");
	strSQL.append(":cpu_core, :cpu_cputype, :cpu_cycles, :cpu_cycleup, :cpu_cycledown, :mixer_nosound, :mixer_rate, :mixer_blocksize, :mixer_prebuffer, ");
	strSQL.append(":midi_mpu401, :midi_intelligent, :midi_device, :midi_config, :midi_mt32rate, :sblaster_sbtype, :sblaster_sbbase, :sblaster_irq, ");
	strSQL.append(":sblaster_dma, :sblaster_hdma, :sblaster_mixer, :sblaster_oplmode, :sblaster_oplemu, :sblaster_oplrate, :gus_gus, :gus_gusrate, :gus_gusbase, ");
	strSQL.append(":gus_irq1, :gus_irq2, :gus_dma1, :gus_dma2, :gus_ultradir, :speaker_pcspeaker, :speaker_pcrate, :speaker_tandy, :speaker_tandyrate, ");
	strSQL.append(":speaker_disney, :joystick_type, :joystick_timed, :joystick_autofire, :joystick_swap34, :joystick_buttonwrap, ");
	strSQL.append(":modem_modem, :modem_comport, :modem_listenport, :dserial_directserial, :dserial_comport, ");
	strSQL.append(":dserial_realport, :dserial_defaultbps, :dserial_parity, :dserial_bytesize, :dserial_stopbit, :serial_1, :serial_2, :serial_3, ");
	strSQL.append(":serial_4, :dos_xms, :dos_ems, :dos_umb, :dos_keyboardlayout, :ipx_ipx, :autoexec, :opt_autoexec, :opt_loadfix, :opt_loadfix_mem, :opt_consola_dbox, ");
	strSQL.append(":opt_cerrar_dbox, :opt_cycle_sincronizar, :path_conf, :path_sonido, :path_exe, :path_setup, :parametros_exe, :parametros_setup )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":idgrl"                 , IDgrl                              );
	query.bindValue(":sdl_fullscreen"        , datos["Dbx_sdl_fullscreen"]        );
	query.bindValue(":sdl_fulldouble"        , datos["Dbx_sdl_fulldouble"]        );
	query.bindValue(":sdl_fullfixed"         , datos["Dbx_sdl_fullfixed"]         );
	query.bindValue(":sdl_fullresolution"    , datos["Dbx_sdl_fullresolution"]    );
	query.bindValue(":sdl_windowresolution"  , datos["Dbx_sdl_windowresolution"]  );
	query.bindValue(":sdl_output"            , datos["Dbx_sdl_output"]            );
	query.bindValue(":sdl_hwscale"           , datos["Dbx_sdl_hwscale"]           );
	query.bindValue(":sdl_autolock"          , datos["Dbx_sdl_autolock"]          );
	query.bindValue(":sdl_sensitivity"       , datos["Dbx_sdl_sensitivity"]       );
	query.bindValue(":sdl_waitonerror"       , datos["Dbx_sdl_waitonerror"]       );
	query.bindValue(":sdl_priority"          , datos["Dbx_sdl_priority"]          );
	query.bindValue(":sdl_mapperfile"        , datos["Dbx_sdl_mapperfile"]        );
	query.bindValue(":sdl_usescancodes"      , datos["Dbx_sdl_usescancodes"]      );
	query.bindValue(":dosbox_language"       , datos["Dbx_dosbox_language"]       );
	query.bindValue(":dosbox_machine"        , datos["Dbx_dosbox_machine"]        );
	query.bindValue(":dosbox_captures"       , datos["Dbx_dosbox_captures"]       );
	query.bindValue(":dosbox_memsize"        , datos["Dbx_dosbox_memsize"]        );
	query.bindValue(":render_frameskip"      , datos["Dbx_render_frameskip"]      );
	query.bindValue(":render_aspect"         , datos["Dbx_render_aspect"]         );
	query.bindValue(":render_scaler"         , datos["Dbx_render_scaler"]         );
	query.bindValue(":cpu_core"              , datos["Dbx_cpu_core"]              );
	query.bindValue(":cpu_cputype"           , datos["Dbx_cpu_cputype"]           );
	query.bindValue(":cpu_cycles"            , datos["Dbx_cpu_cycles"]            );
	query.bindValue(":cpu_cycleup"           , datos["Dbx_cpu_cycleup"]           );
	query.bindValue(":cpu_cycledown"         , datos["Dbx_cpu_cycledown"]         );
	query.bindValue(":mixer_nosound"         , datos["Dbx_mixer_nosound"]         );
	query.bindValue(":mixer_rate"            , datos["Dbx_mixer_rate"]            );
	query.bindValue(":mixer_blocksize"       , datos["Dbx_mixer_blocksize"]       );
	query.bindValue(":mixer_prebuffer"       , datos["Dbx_mixer_prebuffer"]       );
	query.bindValue(":midi_mpu401"           , datos["Dbx_midi_mpu401"]           );
	query.bindValue(":midi_intelligent"      , datos["Dbx_midi_intelligent"]      );
	query.bindValue(":midi_device"           , datos["Dbx_midi_device"]           );
	query.bindValue(":midi_config"           , datos["Dbx_midi_config"]           );
	query.bindValue(":midi_mt32rate"         , datos["Dbx_midi_mt32rate"]         );
	query.bindValue(":sblaster_sbtype"       , datos["Dbx_sblaster_sbtype"]       );
	query.bindValue(":sblaster_sbbase"       , datos["Dbx_sblaster_sbbase"]       );
	query.bindValue(":sblaster_irq"          , datos["Dbx_sblaster_irq"]          );
	query.bindValue(":sblaster_dma"          , datos["Dbx_sblaster_dma"]          );
	query.bindValue(":sblaster_hdma"         , datos["Dbx_sblaster_hdma"]         );
	query.bindValue(":sblaster_mixer"        , datos["Dbx_sblaster_mixer"]        );
	query.bindValue(":sblaster_oplmode"      , datos["Dbx_sblaster_oplmode"]      );
	query.bindValue(":sblaster_oplemu"       , datos["Dbx_sblaster_oplemu"]       );
	query.bindValue(":sblaster_oplrate"      , datos["Dbx_sblaster_oplrate"]      );
	query.bindValue(":gus_gus"               , datos["Dbx_gus_gus"]               );
	query.bindValue(":gus_gusrate"           , datos["Dbx_gus_gusrate"]           );
	query.bindValue(":gus_gusbase"           , datos["Dbx_gus_gusbase"]           );
	query.bindValue(":gus_irq1"              , datos["Dbx_gus_irq1"]              );
	query.bindValue(":gus_irq2"              , datos["Dbx_gus_irq2"]              );
	query.bindValue(":gus_dma1"              , datos["Dbx_gus_dma1"]              );
	query.bindValue(":gus_dma2"              , datos["Dbx_gus_dma2"]              );
	query.bindValue(":gus_ultradir"          , datos["Dbx_gus_ultradir"]          );
	query.bindValue(":speaker_pcspeaker"     , datos["Dbx_speaker_pcspeaker"]     );
	query.bindValue(":speaker_pcrate"        , datos["Dbx_speaker_pcrate"]        );
	query.bindValue(":speaker_tandy"         , datos["Dbx_speaker_tandy"]         );
	query.bindValue(":speaker_tandyrate"     , datos["Dbx_speaker_tandyrate"]     );
	query.bindValue(":speaker_disney"        , datos["Dbx_speaker_disney"]        );
	query.bindValue(":joystick_type"         , datos["Dbx_joystick_type"]         );
	query.bindValue(":joystick_timed"        , datos["Dbx_joystick_timed"]        );
	query.bindValue(":joystick_autofire"     , datos["Dbx_joystick_autofire"]     );
	query.bindValue(":joystick_swap34"       , datos["Dbx_joystick_swap34"]       );
	query.bindValue(":joystick_buttonwrap"   , datos["Dbx_joystick_buttonwrap"]   );
	query.bindValue(":modem_modem"           , datos["Dbx_modem_modem"]           );
	query.bindValue(":modem_comport"         , datos["Dbx_modem_comport"]         );
	query.bindValue(":modem_listenport"      , datos["Dbx_modem_listenport"]      );
	query.bindValue(":dserial_directserial"  , datos["Dbx_dserial_directserial"]  );
	query.bindValue(":dserial_comport"       , datos["Dbx_dserial_comport"]       );
	query.bindValue(":dserial_realport"      , datos["Dbx_dserial_realport"]      );
	query.bindValue(":dserial_defaultbps"    , datos["Dbx_dserial_defaultbps"]    );
	query.bindValue(":dserial_parity"        , datos["Dbx_dserial_parity"]        );
	query.bindValue(":dserial_bytesize"      , datos["Dbx_dserial_bytesize"]      );
	query.bindValue(":dserial_stopbit"       , datos["Dbx_dserial_stopbit"]       );
	query.bindValue(":serial_1"              , datos["Dbx_serial_1"]              );
	query.bindValue(":serial_2"              , datos["Dbx_serial_2"]              );
	query.bindValue(":serial_3"              , datos["Dbx_serial_3"]              );
	query.bindValue(":serial_4"              , datos["Dbx_serial_4"]              );
	query.bindValue(":dos_xms"               , datos["Dbx_dos_xms"]               );
	query.bindValue(":dos_ems"               , datos["Dbx_dos_ems"]               );
	query.bindValue(":dos_umb"               , datos["Dbx_dos_umb"]               );
	query.bindValue(":dos_keyboardlayout"    , datos["Dbx_dos_keyboardlayout"]    );
	query.bindValue(":ipx_ipx"               , datos["Dbx_ipx_ipx"]               );
	query.bindValue(":autoexec"              , datos["Dbx_autoexec"]              );
	query.bindValue(":opt_autoexec"          , datos["Dbx_opt_autoexec"]          );
	query.bindValue(":opt_loadfix"           , datos["Dbx_opt_loadfix"]           );
	query.bindValue(":opt_loadfix_mem"       , datos["Dbx_opt_loadfix_mem"]       );
	query.bindValue(":opt_consola_dbox"      , datos["Dbx_opt_consola_dbox"]      );
	query.bindValue(":opt_cerrar_dbox"       , datos["Dbx_opt_cerrar_dbox"]       );
	query.bindValue(":opt_cycle_sincronizar" , datos["Dbx_opt_cycle_sincronizar"] );
	query.bindValue(":path_conf"             , datos["Dbx_path_conf"]             );
	query.bindValue(":path_sonido"           , datos["Dbx_path_sonido"]           );
	query.bindValue(":path_exe"              , datos["Dbx_path_exe"]              );
	query.bindValue(":path_setup"            , datos["Dbx_path_setup"]            );
	query.bindValue(":parametros_exe"        , datos["Dbx_parametros_exe"]        );
	query.bindValue(":parametros_setup"      , datos["Dbx_parametros_setup"]      );
	query.exec();

	if( Chequear_Query(query) )
		return query.lastInsertId().toString();
	else
		return "";
}

void dbSql::ItemActualizaDbx(const QHash<QString, QString> datos, const QString IDdbx)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_dosbox SET ");
	strSQL.append("sdl_fullscreen = :sdl_fullscreen , sdl_fulldouble = :sdl_fulldouble , sdl_fullfixed = :sdl_fullfixed , sdl_fullresolution = :sdl_fullresolution ,");
	strSQL.append("sdl_windowresolution = :sdl_windowresolution , sdl_output = :sdl_output , sdl_hwscale = :sdl_hwscale , sdl_autolock = :sdl_autolock ,");
	strSQL.append("sdl_sensitivity = :sdl_sensitivity , sdl_waitonerror = :sdl_waitonerror , sdl_priority = :sdl_priority , sdl_mapperfile = :sdl_mapperfile ,");
	strSQL.append("sdl_usescancodes = :sdl_usescancodes , dosbox_language = :dosbox_language , dosbox_machine = :dosbox_machine , dosbox_captures = :dosbox_captures ,");
	strSQL.append("dosbox_memsize = :dosbox_memsize , render_frameskip = :render_frameskip , render_aspect = :render_aspect , render_scaler = :render_scaler ,");
	strSQL.append("cpu_core = :cpu_core , cpu_cputype = :cpu_cputype , cpu_cycles = :cpu_cycles , cpu_cycleup = :cpu_cycleup , cpu_cycledown = :cpu_cycledown , mixer_nosound = :mixer_nosound ,");
	strSQL.append("mixer_rate = :mixer_rate , mixer_blocksize = :mixer_blocksize , mixer_prebuffer = :mixer_prebuffer , midi_mpu401 = :midi_mpu401 , midi_intelligent = :midi_intelligent ,");
	strSQL.append("midi_device = :midi_device , midi_config = :midi_config , midi_mt32rate = :midi_mt32rate , sblaster_sbtype = :sblaster_sbtype , sblaster_sbbase = :sblaster_sbbase ,");
	strSQL.append("sblaster_irq = :sblaster_irq , sblaster_dma = :sblaster_dma , sblaster_hdma = :sblaster_hdma , sblaster_mixer = :sblaster_mixer , sblaster_oplmode = :sblaster_oplmode ,");
	strSQL.append("sblaster_oplemu = :sblaster_oplemu, sblaster_oplrate = :sblaster_oplrate , gus_gus = :gus_gus , gus_gusrate = :gus_gusrate , gus_gusbase = :gus_gusbase , gus_irq1 = :gus_irq1 , gus_irq2 = :gus_irq2 ,");
	strSQL.append("gus_dma1 = :gus_dma1 , gus_dma2 = :gus_dma2 , gus_ultradir = :gus_ultradir , speaker_pcspeaker = :speaker_pcspeaker , speaker_pcrate = :speaker_pcrate ,");
	strSQL.append("speaker_tandy = :speaker_tandy , speaker_tandyrate = :speaker_tandyrate , speaker_disney = :speaker_disney , joystick_type = :joystick_type ,");
	strSQL.append("joystick_timed = :joystick_timed , joystick_autofire = :joystick_autofire , joystick_swap34 = :joystick_swap34 , joystick_buttonwrap = :joystick_buttonwrap ,");
	strSQL.append("modem_modem = :modem_modem , modem_comport = :modem_comport , modem_listenport = :modem_listenport , dserial_directserial = :dserial_directserial ,");
	strSQL.append("dserial_comport = :dserial_comport , dserial_realport = :dserial_realport , dserial_defaultbps = :dserial_defaultbps , dserial_parity = :dserial_parity ,");
	strSQL.append("dserial_bytesize = :dserial_bytesize , dserial_stopbit = :dserial_stopbit , serial_1 = :serial_1 , serial_2 = :serial_2 , serial_3 = :serial_3 , serial_4 = :serial_4 ,");
	strSQL.append("dos_xms = :dos_xms , dos_ems = :dos_ems , dos_umb = :dos_umb , dos_keyboardlayout = :dos_keyboardlayout , ipx_ipx = :ipx_ipx , autoexec = :autoexec , opt_autoexec = :opt_autoexec , opt_loadfix = :opt_loadfix ,");
	strSQL.append("opt_loadfix_mem = :opt_loadfix_mem , opt_consola_dbox = :opt_consola_dbox , opt_cerrar_dbox = :opt_cerrar_dbox , opt_cycle_sincronizar = :opt_cycle_sincronizar ,");
	strSQL.append("path_conf = :path_conf , path_sonido = :path_sonido , path_exe = :path_exe , path_setup = :path_setup , parametros_exe = :parametros_exe , parametros_setup = :parametros_setup ");
	strSQL.append("WHERE id = :id;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
//	query.bindValue(":idgrl"                 , IDgrl                              );
	query.bindValue(":sdl_fullscreen"        , datos["Dbx_sdl_fullscreen"]        );
	query.bindValue(":sdl_fulldouble"        , datos["Dbx_sdl_fulldouble"]        );
	query.bindValue(":sdl_fullfixed"         , datos["Dbx_sdl_fullfixed"]         );
	query.bindValue(":sdl_fullresolution"    , datos["Dbx_sdl_fullresolution"]    );
	query.bindValue(":sdl_windowresolution"  , datos["Dbx_sdl_windowresolution"]  );
	query.bindValue(":sdl_output"            , datos["Dbx_sdl_output"]            );
	query.bindValue(":sdl_hwscale"           , datos["Dbx_sdl_hwscale"]           );
	query.bindValue(":sdl_autolock"          , datos["Dbx_sdl_autolock"]          );
	query.bindValue(":sdl_sensitivity"       , datos["Dbx_sdl_sensitivity"]       );
	query.bindValue(":sdl_waitonerror"       , datos["Dbx_sdl_waitonerror"]       );
	query.bindValue(":sdl_priority"          , datos["Dbx_sdl_priority"]          );
	query.bindValue(":sdl_mapperfile"        , datos["Dbx_sdl_mapperfile"]        );
	query.bindValue(":sdl_usescancodes"      , datos["Dbx_sdl_usescancodes"]      );
	query.bindValue(":dosbox_language"       , datos["Dbx_dosbox_language"]       );
	query.bindValue(":dosbox_machine"        , datos["Dbx_dosbox_machine"]        );
	query.bindValue(":dosbox_captures"       , datos["Dbx_dosbox_captures"]       );
	query.bindValue(":dosbox_memsize"        , datos["Dbx_dosbox_memsize"]        );
	query.bindValue(":render_frameskip"      , datos["Dbx_render_frameskip"]      );
	query.bindValue(":render_aspect"         , datos["Dbx_render_aspect"]         );
	query.bindValue(":render_scaler"         , datos["Dbx_render_scaler"]         );
	query.bindValue(":cpu_core"              , datos["Dbx_cpu_core"]              );
	query.bindValue(":cpu_cputype"           , datos["Dbx_cpu_cputype"]           );
	query.bindValue(":cpu_cycles"            , datos["Dbx_cpu_cycles"]            );
	query.bindValue(":cpu_cycleup"           , datos["Dbx_cpu_cycleup"]           );
	query.bindValue(":cpu_cycledown"         , datos["Dbx_cpu_cycledown"]         );
	query.bindValue(":mixer_nosound"         , datos["Dbx_mixer_nosound"]         );
	query.bindValue(":mixer_rate"            , datos["Dbx_mixer_rate"]            );
	query.bindValue(":mixer_blocksize"       , datos["Dbx_mixer_blocksize"]       );
	query.bindValue(":mixer_prebuffer"       , datos["Dbx_mixer_prebuffer"]       );
	query.bindValue(":midi_mpu401"           , datos["Dbx_midi_mpu401"]           );
	query.bindValue(":midi_intelligent"      , datos["Dbx_midi_intelligent"]      );
	query.bindValue(":midi_device"           , datos["Dbx_midi_device"]           );
	query.bindValue(":midi_config"           , datos["Dbx_midi_config"]           );
	query.bindValue(":midi_mt32rate"         , datos["Dbx_midi_mt32rate"]         );
	query.bindValue(":sblaster_sbtype"       , datos["Dbx_sblaster_sbtype"]       );
	query.bindValue(":sblaster_sbbase"       , datos["Dbx_sblaster_sbbase"]       );
	query.bindValue(":sblaster_irq"          , datos["Dbx_sblaster_irq"]          );
	query.bindValue(":sblaster_dma"          , datos["Dbx_sblaster_dma"]          );
	query.bindValue(":sblaster_hdma"         , datos["Dbx_sblaster_hdma"]         );
	query.bindValue(":sblaster_mixer"        , datos["Dbx_sblaster_mixer"]        );
	query.bindValue(":sblaster_oplmode"      , datos["Dbx_sblaster_oplmode"]      );
	query.bindValue(":sblaster_oplemu"       , datos["Dbx_sblaster_oplemu"]       );
	query.bindValue(":sblaster_oplrate"      , datos["Dbx_sblaster_oplrate"]      );
	query.bindValue(":gus_gus"               , datos["Dbx_gus_gus"]               );
	query.bindValue(":gus_gusrate"           , datos["Dbx_gus_gusrate"]           );
	query.bindValue(":gus_gusbase"           , datos["Dbx_gus_gusbase"]           );
	query.bindValue(":gus_irq1"              , datos["Dbx_gus_irq1"]              );
	query.bindValue(":gus_irq2"              , datos["Dbx_gus_irq2"]              );
	query.bindValue(":gus_dma1"              , datos["Dbx_gus_dma1"]              );
	query.bindValue(":gus_dma2"              , datos["Dbx_gus_dma2"]              );
	query.bindValue(":gus_ultradir"          , datos["Dbx_gus_ultradir"]          );
	query.bindValue(":speaker_pcspeaker"     , datos["Dbx_speaker_pcspeaker"]     );
	query.bindValue(":speaker_pcrate"        , datos["Dbx_speaker_pcrate"]        );
	query.bindValue(":speaker_tandy"         , datos["Dbx_speaker_tandy"]         );
	query.bindValue(":speaker_tandyrate"     , datos["Dbx_speaker_tandyrate"]     );
	query.bindValue(":speaker_disney"        , datos["Dbx_speaker_disney"]        );
	query.bindValue(":joystick_type"         , datos["Dbx_joystick_type"]         );
	query.bindValue(":joystick_timed"        , datos["Dbx_joystick_timed"]        );
	query.bindValue(":joystick_autofire"     , datos["Dbx_joystick_autofire"]     );
	query.bindValue(":joystick_swap34"       , datos["Dbx_joystick_swap34"]       );
	query.bindValue(":joystick_buttonwrap"   , datos["Dbx_joystick_buttonwrap"]   );
	query.bindValue(":modem_modem"           , datos["Dbx_modem_modem"]           );
	query.bindValue(":modem_comport"         , datos["Dbx_modem_comport"]         );
	query.bindValue(":modem_listenport"      , datos["Dbx_modem_listenport"]      );
	query.bindValue(":dserial_directserial"  , datos["Dbx_dserial_directserial"]  );
	query.bindValue(":dserial_comport"       , datos["Dbx_dserial_comport"]       );
	query.bindValue(":dserial_realport"      , datos["Dbx_dserial_realport"]      );
	query.bindValue(":dserial_defaultbps"    , datos["Dbx_dserial_defaultbps"]    );
	query.bindValue(":dserial_parity"        , datos["Dbx_dserial_parity"]        );
	query.bindValue(":dserial_bytesize"      , datos["Dbx_dserial_bytesize"]      );
	query.bindValue(":dserial_stopbit"       , datos["Dbx_dserial_stopbit"]       );
	query.bindValue(":serial_1"              , datos["Dbx_serial_1"]              );
	query.bindValue(":serial_2"              , datos["Dbx_serial_2"]              );
	query.bindValue(":serial_3"              , datos["Dbx_serial_3"]              );
	query.bindValue(":serial_4"              , datos["Dbx_serial_4"]              );
	query.bindValue(":dos_xms"               , datos["Dbx_dos_xms"]               );
	query.bindValue(":dos_ems"               , datos["Dbx_dos_ems"]               );
	query.bindValue(":dos_umb"               , datos["Dbx_dos_umb"]               );
	query.bindValue(":dos_keyboardlayout"    , datos["Dbx_dos_keyboardlayout"]    );
	query.bindValue(":ipx_ipx"               , datos["Dbx_ipx_ipx"]               );
	query.bindValue(":autoexec"              , datos["Dbx_autoexec"]              );
	query.bindValue(":opt_autoexec"          , datos["Dbx_opt_autoexec"]          );
	query.bindValue(":opt_loadfix"           , datos["Dbx_opt_loadfix"]           );
	query.bindValue(":opt_loadfix_mem"       , datos["Dbx_opt_loadfix_mem"]       );
	query.bindValue(":opt_consola_dbox"      , datos["Dbx_opt_consola_dbox"]      );
	query.bindValue(":opt_cerrar_dbox"       , datos["Dbx_opt_cerrar_dbox"]       );
	query.bindValue(":opt_cycle_sincronizar" , datos["Dbx_opt_cycle_sincronizar"] );
	query.bindValue(":path_conf"             , datos["Dbx_path_conf"]             );
	query.bindValue(":path_sonido"           , datos["Dbx_path_sonido"]           );
	query.bindValue(":path_exe"              , datos["Dbx_path_exe"]              );
	query.bindValue(":path_setup"            , datos["Dbx_path_setup"]            );
	query.bindValue(":parametros_exe"        , datos["Dbx_parametros_exe"]        );
	query.bindValue(":parametros_setup"      , datos["Dbx_parametros_setup"]      );
	query.bindValue(":id"                    , IDdbx                              );
	query.exec();
}

void dbSql::ItemInsertaMontajesDbx(QTreeWidget *treeWidget, const QString IDdbx)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_dosbox_mount (");
	strSQL.append("id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount");
	strSQL.append(") VALUES ( ");
	strSQL.append(":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount");
	strSQL.append(")");

	for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
		QSqlQuery query;
		query.clear();
		query.prepare( strSQL );
		query.bindValue(":id_dosbox"   , IDdbx         );
		query.bindValue(":id_lista"    , num_mount     );
		query.bindValue(":path"        , item->text(0) );
		query.bindValue(":label"       , item->text(1) );
		query.bindValue(":tipo_as"     , item->text(2) );
		query.bindValue(":letter"      , item->text(3) );
		query.bindValue(":indx_cd"     , item->text(4) );
		query.bindValue(":opt_mount"   , item->text(5) );
		query.bindValue(":io_ctrl"     , item->text(6) );
		query.bindValue(":select_mount", item->text(7) );
		query.exec();
	}
}

QString dbSql::ItemInsertaUnMontajeDbx(const QHash<QString, QString> datos, const QString IDdbx)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_dosbox_mount (");
	strSQL.append("id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount");
	strSQL.append(") VALUES ( ");
	strSQL.append(":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount");
	strSQL.append(")");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":id_dosbox"   , IDdbx                 );
	query.bindValue(":id_lista"    , datos["id_lista"]     );
	query.bindValue(":path"        , datos["path"]         );
	query.bindValue(":label"       , datos["label"]        );
	query.bindValue(":tipo_as"     , datos["tipo_as"]      );
	query.bindValue(":letter"      , datos["letter"]       );
	query.bindValue(":indx_cd"     , datos["indx_cd"]      );
	query.bindValue(":opt_mount"   , datos["opt_mount"]    );
	query.bindValue(":io_ctrl"     , datos["io_ctrl"]      );
	query.bindValue(":select_mount", datos["select_mount"] );
	query.exec();

	return query.lastInsertId().toString();
}

void dbSql::ItemActualizaMontajeDbx(QTreeWidget *treeWidget)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_dosbox_mount SET ");
	strSQL.append("id_lista = :id_lista, path = :path, label = :label, ");
	strSQL.append("tipo_as = :tipo_as, letter = :letter, indx_cd = :indx_cd, opt_mount = :opt_mount, ");
	strSQL.append("io_ctrl = :io_ctrl, select_mount = :select_mount ");
	strSQL.append("WHERE id = :id;");

	for(int num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
		QSqlQuery query;
		query.clear();
		query.prepare( strSQL );
		query.bindValue(":id_lista"    , num_mount     );
		query.bindValue(":path"        , item->text(0) );
		query.bindValue(":label"       , item->text(1) );
		query.bindValue(":tipo_as"     , item->text(2) );
		query.bindValue(":letter"      , item->text(3) );
		query.bindValue(":indx_cd"     , item->text(4) );
		query.bindValue(":opt_mount"   , item->text(5) );
		query.bindValue(":io_ctrl"     , item->text(6) );
		query.bindValue(":select_mount", item->text(7) );
		query.bindValue(":id"          , item->text(8) ); // id dbgrl_emu_dosbox_mount
		query.exec();
	}
}

void dbSql::ItemEliminarMontaje( const QString IDmountdbx )
{
	QSqlQuery query;
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id="+IDmountdbx);
}

QHash<QString, QString> dbSql::showConfg_ScummVM(QString IDgrl)
{
	QHash<QString, QString> tmpScummVM;
	QSqlQuery query;
	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_scummvm WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();

	tmpScummVM["Svm_id"]              = query.record().value("id" ).toString();
	tmpScummVM["Svm_idgrl"]           = query.record().value("idgrl" ).toString();
	tmpScummVM["Svm_game"]            = query.record().value("game" ).toString();
	tmpScummVM["Svm_game_label"]      = query.record().value("game_label" ).toString();
	tmpScummVM["Svm_language"]        = query.record().value("language").toString();
	tmpScummVM["Svm_subtitles"]       = query.record().value("subtitles").toString();
	tmpScummVM["Svm_platform"]        = query.record().value("platform").toString();
	tmpScummVM["Svm_gfx_mode"]        = query.record().value("gfx_mode").toString();
	tmpScummVM["Svm_render_mode"]     = query.record().value("render_mode").toString();
	tmpScummVM["Svm_fullscreen"]      = query.record().value("fullscreen").toString();
	tmpScummVM["Svm_aspect_ratio"]    = query.record().value("aspect_ratio").toString();
	tmpScummVM["Svm_path"]            = query.record().value("path").toString();
	tmpScummVM["Svm_path_setup"]      = query.record().value("path_setup").toString();
	tmpScummVM["Svm_path_extra"]      = query.record().value("path_extra").toString();
	tmpScummVM["Svm_path_save"]       = query.record().value("path_save").toString();
	tmpScummVM["Svm_path_capturas"]   = query.record().value("path_capturas").toString();
	tmpScummVM["Svm_path_sonido"]     = query.record().value("path_sonido").toString();
	tmpScummVM["Svm_music_driver"]    = query.record().value("music_driver").toString();
	tmpScummVM["Svm_enable_gs"]       = query.record().value("enable_gs").toString();
	tmpScummVM["Svm_multi_midi"]      = query.record().value("multi_midi").toString();
	tmpScummVM["Svm_native_mt32"]     = query.record().value("native_mt32").toString();
	tmpScummVM["Svm_master_volume"]   = query.record().value("master_volume").toString();
	tmpScummVM["Svm_music_volume"]    = query.record().value("music_volume").toString();
	tmpScummVM["Svm_sfx_volume"]      = query.record().value("sfx_volume").toString();
	tmpScummVM["Svm_speech_volume"]   = query.record().value("speech_volume").toString();
	tmpScummVM["Svm_tempo"]           = query.record().value("tempo").toString();
	tmpScummVM["Svm_talkspeed"]       = query.record().value("talkspeed").toString();
	tmpScummVM["Svm_debuglevel"]      = query.record().value("debuglevel").toString();
	tmpScummVM["Svm_cdrom"]           = query.record().value("cdrom").toString();
	tmpScummVM["Svm_joystick_num"]    = query.record().value("joystick_num").toString();
	tmpScummVM["Svm_output_rate"]     = query.record().value("output_rate").toString();
	tmpScummVM["Svm_midi_gain"]       = query.record().value("midi_gain").toString();
	tmpScummVM["Svm_copy_protection"] = query.record().value("copy_protection").toString();
	tmpScummVM["Svm_sound_font"]      = query.record().value("sound_font").toString();
	tmpScummVM["Svm_walkspeed"]       = query.record().value("walkspeed").toString();
	tmpScummVM["Svm_opl_driver"]      = query.record().value("opl_driver").toString();

	return tmpScummVM;
}

void dbSql::ItemInsertaSvm(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_scummvm (");
	strSQL.append("idgrl, game, game_label, language, subtitles, platform, gfx_mode, render_mode, fullscreen, aspect_ratio, path, ");
	strSQL.append("path_setup, path_extra, path_save, path_capturas, path_sonido, music_driver, enable_gs, multi_midi, ");
	strSQL.append("native_mt32, master_volume, music_volume, sfx_volume, speech_volume, tempo, talkspeed, debuglevel, ");
	strSQL.append("cdrom, joystick_num, output_rate, midi_gain, copy_protection, sound_font, walkspeed, opl_driver");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :game, :game_label, :language, :subtitles, :platform, :gfx_mode, :render_mode, :fullscreen, :aspect_ratio, :path, ");
	strSQL.append(":path_setup, :path_extra, :path_save, :path_capturas, :path_sonido, :music_driver, :enable_gs, :multi_midi, ");
	strSQL.append(":native_mt32, :master_volume, :music_volume, :sfx_volume, :speech_volume, :tempo, :talkspeed, :debuglevel, ");
	strSQL.append(":cdrom, :joystick_num, :output_rate, :midi_gain, :copy_protection, :sound_font, :walkspeed, :opl_driver )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":idgrl"           , IDgrl                        );
	query.bindValue(":game"            , datos["Svm_game"]            );
	query.bindValue(":game_label"      , datos["Svm_game_label"]      );
	query.bindValue(":language"        , datos["Svm_language"]        );
	query.bindValue(":subtitles"       , datos["Svm_subtitles"]       );
	query.bindValue(":platform"        , datos["Svm_platform"]        );
	query.bindValue(":gfx_mode"        , datos["Svm_gfx_mode"]        );
	query.bindValue(":render_mode"     , datos["Svm_render_mode"]     );
	query.bindValue(":fullscreen"      , datos["Svm_fullscreen"]      );
	query.bindValue(":aspect_ratio"    , datos["Svm_aspect_ratio"]    );
	query.bindValue(":path"            , datos["Svm_path"]            );
	query.bindValue(":path_setup"      , datos["Svm_path_setup"]      );
	query.bindValue(":path_extra"      , datos["Svm_path_extra"]      );
	query.bindValue(":path_save"       , datos["Svm_path_save"]       );
	query.bindValue(":path_capturas"   , datos["Svm_path_capturas"]   );
	query.bindValue(":path_sonido"     , datos["Svm_path_sonido"]     );
	query.bindValue(":music_driver"    , datos["Svm_music_driver"]    );
	query.bindValue(":enable_gs"       , datos["Svm_enable_gs"]       );
	query.bindValue(":multi_midi"      , datos["Svm_multi_midi"]      );
	query.bindValue(":native_mt32"     , datos["Svm_native_mt32"]     );
	query.bindValue(":master_volume"   , datos["Svm_master_volume"]   );
	query.bindValue(":music_volume"    , datos["Svm_music_volume"]    );
	query.bindValue(":sfx_volume"      , datos["Svm_sfx_volume"]      );
	query.bindValue(":speech_volume"   , datos["Svm_speech_volume"]   );
	query.bindValue(":tempo"           , datos["Svm_tempo"]           );
	query.bindValue(":talkspeed"       , datos["Svm_talkspeed"]       );
	query.bindValue(":debuglevel"      , datos["Svm_debuglevel"]      );
	query.bindValue(":cdrom"           , datos["Svm_cdrom"]           );
	query.bindValue(":joystick_num"    , datos["Svm_joystick_num"]    );
	query.bindValue(":output_rate"     , datos["Svm_output_rate"]     );
	query.bindValue(":midi_gain"       , datos["Svm_midi_gain"]       );
	query.bindValue(":copy_protection" , datos["Svm_copy_protection"] );
	query.bindValue(":sound_font"      , datos["Svm_sound_font"]      );
	query.bindValue(":walkspeed"       , datos["Svm_walkspeed"]       );
	query.bindValue(":opl_driver"      , datos["Svm_opl_driver"]      );

	query.exec();
}

void dbSql::ItemActualizaSvm(const QHash<QString, QString> datos, const QString IDsvm)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_scummvm SET ");
	strSQL.append("game = :game, game_label = :game_label, language = :language, subtitles = :subtitles, platform = :platform, ");
	strSQL.append("gfx_mode  = :gfx_mode, render_mode = :render_mode, fullscreen = :fullscreen, aspect_ratio = :aspect_ratio, ");
	strSQL.append("path = :path, path_setup = :path_setup, path_extra = :path_extra, path_save = :path_save, ");
	strSQL.append("path_capturas = :path_capturas, path_sonido = :path_sonido, music_driver = :music_driver, enable_gs = :enable_gs, ");
	strSQL.append("multi_midi = :multi_midi, native_mt32 = :native_mt32, master_volume = :master_volume, music_volume = :music_volume, ");
	strSQL.append("sfx_volume = :sfx_volume, speech_volume = :speech_volume, tempo = :tempo, talkspeed = :talkspeed, ");
	strSQL.append("debuglevel = :debuglevel, cdrom = :cdrom, joystick_num = :joystick_num, output_rate = :output_rate, ");
	strSQL.append("midi_gain = :midi_gain, copy_protection = :copy_protection, sound_font = :sound_font, walkspeed = :walkspeed, opl_driver = :opl_driver ");
	strSQL.append("WHERE id = :id;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
//	query.bindValue(":idgrl"           , IDgrl                        );
	query.bindValue(":game"            , datos["Svm_game"]            );
	query.bindValue(":game_label"      , datos["Svm_game_label"]      );
	query.bindValue(":language"        , datos["Svm_language"]        );
	query.bindValue(":subtitles"       , datos["Svm_subtitles"]       );
	query.bindValue(":platform"        , datos["Svm_platform"]        );
	query.bindValue(":gfx_mode"        , datos["Svm_gfx_mode"]        );
	query.bindValue(":render_mode"     , datos["Svm_render_mode"]     );
	query.bindValue(":fullscreen"      , datos["Svm_fullscreen"]      );
	query.bindValue(":aspect_ratio"    , datos["Svm_aspect_ratio"]    );
	query.bindValue(":path"            , datos["Svm_path"]            );
	query.bindValue(":path_setup"      , datos["Svm_path_setup"]      );
	query.bindValue(":path_extra"      , datos["Svm_path_extra"]      );
	query.bindValue(":path_save"       , datos["Svm_path_save"]       );
	query.bindValue(":path_capturas"   , datos["Svm_path_capturas"]   );
	query.bindValue(":path_sonido"     , datos["Svm_path_sonido"]     );
	query.bindValue(":music_driver"    , datos["Svm_music_driver"]    );
	query.bindValue(":enable_gs"       , datos["Svm_enable_gs"]       );
	query.bindValue(":multi_midi"      , datos["Svm_multi_midi"]      );
	query.bindValue(":native_mt32"     , datos["Svm_native_mt32"]     );
	query.bindValue(":master_volume"   , datos["Svm_master_volume"]   );
	query.bindValue(":music_volume"    , datos["Svm_music_volume"]    );
	query.bindValue(":sfx_volume"      , datos["Svm_sfx_volume"]      );
	query.bindValue(":speech_volume"   , datos["Svm_speech_volume"]   );
	query.bindValue(":tempo"           , datos["Svm_tempo"]           );
	query.bindValue(":talkspeed"       , datos["Svm_talkspeed"]       );
	query.bindValue(":debuglevel"      , datos["Svm_debuglevel"]      );
	query.bindValue(":cdrom"           , datos["Svm_cdrom"]           );
	query.bindValue(":joystick_num"    , datos["Svm_joystick_num"]    );
	query.bindValue(":output_rate"     , datos["Svm_output_rate"]     );
	query.bindValue(":midi_gain"       , datos["Svm_midi_gain"]       );
	query.bindValue(":copy_protection" , datos["Svm_copy_protection"] );
	query.bindValue(":sound_font"      , datos["Svm_sound_font"]      );
	query.bindValue(":walkspeed"       , datos["Svm_walkspeed"]       );
	query.bindValue(":opl_driver"      , datos["Svm_opl_driver"]      );
	query.bindValue(":id"              , IDsvm                        ); // id del scummvm
	query.exec();
}

QHash<QString, QString> dbSql::showConfg_VDMSound(QString IDgrl)
{
	QHash<QString, QString> tmpVdmsound;
	QSqlQuery query;
	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();

	tmpVdmsound["Vdms_id"]            = query.record().value("id").toString();
	tmpVdmsound["Vdms_idgrl"]         = query.record().value("idgrl").toString();
	tmpVdmsound["Vdms_path_conf"]     = query.record().value("path_conf").toString();
	tmpVdmsound["Vdms_path_exe"]      = query.record().value("path_exe").toString();
	tmpVdmsound["Vdms_program"]       = query.record().value("program").toString();
	tmpVdmsound["Vdms_vdms_debug"]    = query.record().value("vdms_debug").toString();
	tmpVdmsound["Vdms_winnt_dos"]     = query.record().value("winnt_dos").toString();
	tmpVdmsound["Vdms_winnt_dosbox"]  = query.record().value("winnt_dosbox").toString();
	tmpVdmsound["Vdms_winnt_storage"] = query.record().value("winnt_storage").toString();

	return tmpVdmsound;
}

void dbSql::ItemInsertaVdms(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_vdmsound (");
	strSQL.append("idgrl, path_conf, path_exe, program, vdms_debug, winnt_dos, winnt_dosbox, winnt_storage ");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :path_conf, :path_exe, :program, :vdms_debug, :winnt_dos, :winnt_dosbox, :winnt_storage )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":idgrl"         , IDgrl );
	query.bindValue(":path_conf"     , datos["Vdms_path_conf"] );
	query.bindValue(":path_exe"      , datos["Vdms_path_exe"] );
	query.bindValue(":program"       , datos["Vdms_program_1"] +"|"+ datos["Vdms_program_2"] );
	query.bindValue(":vdms_debug"    , datos["Vdms_vdms_debug_1"] +"|"+ datos["Vdms_vdms_debug_2"] );
	query.bindValue(":winnt_dos"     , datos["Vdms_winnt_dos_1"] +"|"+ datos["Vdms_winnt_dos_2"] );
	query.bindValue(":winnt_dosbox"  , datos["Vdms_winnt_dosbox_1"] +"|"+ datos["Vdms_winnt_dosbox_2"] +"|"+ datos["Vdms_winnt_dosbox_3"] );
	query.bindValue(":winnt_storage" , datos["Vdms_winnt_storage_1"] +"|"+ datos["Vdms_winnt_storage_2"] );
	query.exec();
}

void dbSql::ItemActualizaVdms(const QHash<QString, QString> datos, const QString IDvdms)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_vdmsound SET ");
	strSQL.append("path_conf = :path_conf, path_exe = :path_exe, program = :program, vdms_debug = :vdms_debug, ");
	strSQL.append("winnt_dos = :winnt_dos, winnt_dosbox = :winnt_dosbox, winnt_storage = :winnt_storage ");
	strSQL.append("WHERE id = :id;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":path_conf"     , datos["Vdms_path_conf"] );
	query.bindValue(":path_exe"      , datos["Vdms_path_exe"] );
	query.bindValue(":program"       , datos["Vdms_program_1"] +"|"+ datos["Vdms_program_2"] );
	query.bindValue(":vdms_debug"    , datos["Vdms_vdms_debug_1"] +"|"+ datos["Vdms_vdms_debug_2"] );
	query.bindValue(":winnt_dos"     , datos["Vdms_winnt_dos_1"] +"|"+ datos["Vdms_winnt_dos_2"] );
	query.bindValue(":winnt_dosbox"  , datos["Vdms_winnt_dosbox_1"] +"|"+ datos["Vdms_winnt_dosbox_2"] +"|"+ datos["Vdms_winnt_dosbox_3"] );
	query.bindValue(":winnt_storage" , datos["Vdms_winnt_storage_1"] +"|"+ datos["Vdms_winnt_storage_2"] );
	query.bindValue(":id"            , IDvdms   );
	query.exec();
}

void dbSql::ItemInsertaFiles(QTreeWidget *treeWidget, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_file (");
	strSQL.append("idgrl, nombre, crc, descripcion, path, size, tipo");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :nombre, :crc, :descripcion, :path, :size, :tipo )");

	for(int num_file = 0; num_file < treeWidget->topLevelItemCount(); num_file++ )
	{
		QTreeWidgetItem *item_files = treeWidget->topLevelItem( num_file );
		QSqlQuery query;
		query.clear();
		query.prepare( strSQL );
		query.bindValue(":idgrl"       , IDgrl               );
		query.bindValue(":nombre"      , item_files->text(0) );
		query.bindValue(":crc"         , item_files->text(1) );
		query.bindValue(":descripcion" , item_files->text(2) );
		query.bindValue(":path"        , item_files->text(4) );
		query.bindValue(":size"        , item_files->text(3) );
		query.bindValue(":tipo"        , item_files->text(7) );
		query.exec();
	}
}

QString dbSql::ItemInsertaUnFiles(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_file (");
	strSQL.append("idgrl, nombre, crc, descripcion, path, size, tipo");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :nombre, :crc, :descripcion, :path, :size, :tipo )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":idgrl"       , IDgrl                    );
	query.bindValue(":nombre"      , datos["Fil_nombre"]      );
	query.bindValue(":crc"         , datos["Fil_crc"]         );
	query.bindValue(":descripcion" , datos["Fil_descripcion"] );
	query.bindValue(":path"        , datos["Fil_path"]        );
	query.bindValue(":size"        , datos["Fil_size"]        );
	query.bindValue(":tipo"        , datos["Fil_tipo"]        );
	query.exec();

	return query.lastInsertId().toString();
}

void dbSql::ItemActualizaFiles(const QHash<QString, QString> datos, const QString IDFiles)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_file SET ");
	strSQL.append("nombre = :nombre, crc = :crc, descripcion = :descripcion, ");
	strSQL.append("path = :path, size = :size, tipo = :tipo ");
	strSQL.append("WHERE id = :id;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":nombre"      , datos["Fil_nombre"]      );
	query.bindValue(":crc"         , datos["Fil_crc"]         );
	query.bindValue(":descripcion" , datos["Fil_descripcion"] );
	query.bindValue(":path"        , datos["Fil_path"]        );
	query.bindValue(":size"        , datos["Fil_size"]        );
	query.bindValue(":tipo"        , datos["Fil_tipo"]        );
	query.bindValue(":id"          , IDFiles                  );
	query.exec();
}

void dbSql::ItemEliminarFiles( const QString IDFiles )
{
	QSqlQuery query;
	query.clear();
	query.exec("DELETE FROM dbgrl_file WHERE id="+IDFiles);
}

void dbSql::ItemInsertaURL(QTreeWidget *treeWidget, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_url ( idgrl, url, descripcion ) VALUES ( :idgrl, :url, :descripcion )");

	for(int num_url = 0; num_url < treeWidget->topLevelItemCount(); num_url++ )
	{
		QTreeWidgetItem *item_files = treeWidget->topLevelItem( num_url );
		QSqlQuery query;
		query.clear();
		query.prepare( strSQL );
		query.bindValue(":idgrl"       , IDgrl               );
		query.bindValue(":url"         , item_files->text(0) );
		query.bindValue(":descripcion" , item_files->text(1) );
		query.exec();
	}
}

QString dbSql::ItemInsertaUnURL(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_url ( idgrl, url, descripcion ) VALUES ( :idgrl, :url, :descripcion )");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":idgrl"       , IDgrl                    );
	query.bindValue(":url"         , datos["Url_url"]         );
	query.bindValue(":descripcion" , datos["Url_descripcion"] );
	query.exec();

	return query.lastInsertId().toString();
}

void dbSql::ItemActualizaURL(const QHash<QString, QString> datos, const QString IDURL)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_url SET url = :url, descripcion = :descripcion WHERE id = :id;");

	QSqlQuery query;
	query.clear();
	query.prepare( strSQL );
	query.bindValue(":url"         , datos["Url_url"]         );
	query.bindValue(":descripcion" , datos["Url_descripcion"] );
	query.bindValue(":id"          , IDURL                    );
	query.exec();
}

void dbSql::ItemEliminarURL( const QString IDURL )
{
	QSqlQuery query;
	query.clear();
	query.exec("DELETE FROM dbgrl_url WHERE id="+IDURL);
}
