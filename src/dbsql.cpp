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

dbSql::dbSql(QString db_type, QString db_host, QString db_name, QString db_username, QString db_password, QString db_port)
{
	ok_OpenDB = false;
	if( db_type == "QMYSQL" || db_type == "QPSQL" )
	{
		sqldb = QSqlDatabase::addDatabase( db_type );
		sqldb.setHostName( db_host 	 ); // localhost, archivo
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
db_host = "localhost"	; // localhost, archivo
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

bool dbSql::Chequear_Query( QSqlQuery q )
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

QString dbSql::ItemIDIndex(QString SQLtabla, QString SQLindex)
{
	QSqlQuery query;
	query.exec("SELECT * FROM "+SQLtabla+" WHERE idgrl="+SQLindex+" LIMIT 0,1");
	query.first();
	return query.value(0).toString();
}

void dbSql::ItemEliminar( const QString IDgrl )
{
	QSqlQuery query;
	QString IDdbx, temp_FileConf;
	
	query.exec("DELETE FROM dbgrl WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_url WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_file WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_scummvm WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_dosbox WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();
		IDdbx = query.value(0).toString();
		temp_FileConf = query.value(87).toString();
	eliminararchivo( "confdbx/" + temp_FileConf );
	query.exec("DELETE FROM dbgrl_emu_dosbox_mount WHERE id_dosbox="+IDdbx);
	query.clear();
	query.exec("DELETE FROM dbgrl_emu_dosbox WHERE idgrl="+IDgrl);
	query.clear();
	query.exec("SELECT * FROM dbgrl_emu_vdmsound WHERE idgrl="+IDgrl+" LIMIT 0,1");
	query.first();
		temp_FileConf = query.value(2).toString();	
	eliminararchivo( "confvdms/" + temp_FileConf );
	query.exec("DELETE FROM dbgrl_emu_vdmsound WHERE idgrl="+IDgrl);
	query.clear();
}

void dbSql::eliminararchivo(QString archivo)
{
// Directorio de configuracion para el DOSBox
	QString stArchivoConfg = QDir::homePath()+"/.gr-lida/" + archivo;
//	QString stArchivoConfg = QDir::currentPath()+"/confdbx/"+ archivo;
//	QString stArchivoConfg = "./confdbx/"+ archivo;
	QFile f( stArchivoConfg );
	if( f.exists() )
		f.remove();
}

QString dbSql::ItemInsertaDatos(const QHash<QString, QString> datos)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl (");
	strSQL.append("icono, titulo, subtitulo, genero, compania, desarrollador, tema, idioma, formato, anno, numdisc, ");
	strSQL.append("sistemaop, tamano, graficos, sonido, jugabilidad, original, estado, thumbs, cover_front, cover_back,");
	strSQL.append("fecha, tipo_emu, comentario, favorito ");
	strSQL.append(") VALUES ( ");
	strSQL.append(":icono, :titulo, :subtitulo, :genero, :compania, :desarrollador, :tema, :idioma, :formato, :anno, :numdisc, ");
	strSQL.append(":sistemaop, :tamano, :graficos, :sonido, :jugabilidad, :original, :estado, :thumbs, :cover_front, :cover_back, ");
	strSQL.append(":fecha, :tipo_emu, :comentario, :favorito)");
	
	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":icono"		, datos["icono"]  );
	query.bindValue(":titulo"		, datos["titulo"]  );
	query.bindValue(":subtitulo"	, datos["subtitulo"]  );
	query.bindValue(":genero"		, datos["genero"]  );
	query.bindValue(":compania"		, datos["compania"]  );
	query.bindValue(":desarrollador", datos["desarrollador"]  );
	query.bindValue(":tema"			, datos["tema"]  );
	query.bindValue(":idioma"		, datos["idioma"]  );
	query.bindValue(":formato"		, datos["formato"]  );
	query.bindValue(":anno"			, datos["anno"]  );
	query.bindValue(":numdisc"		, datos["numdisc"] );
	query.bindValue(":sistemaop"	, datos["sistemaop"] );
	query.bindValue(":tamano"		, datos["tamano"] );
	query.bindValue(":graficos"		, datos["graficos"] );
	query.bindValue(":sonido"		, datos["sonido"] );
	query.bindValue(":jugabilidad"	, datos["jugabilidad"] );
	query.bindValue(":original"		, datos["original"] );
	query.bindValue(":estado"		, datos["estado"] );
	query.bindValue(":thumbs"		, datos["thumbs"] );
	query.bindValue(":cover_front"	, datos["cover_front"] );
	query.bindValue(":cover_back"	, datos["cover_back"] );
	query.bindValue(":fecha"		, datos["fecha"] );
	query.bindValue(":tipo_emu"		, datos["tipo_emu"] );
	query.bindValue(":comentario"	, datos["comentario"] );
	query.bindValue(":favorito"		, datos["favorito"] );

	query.exec();
	QVariant v;
	v = query.lastInsertId();
	return v.toString();
}

void dbSql::ItemActualizaDatos(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl SET "); 
	strSQL.append("icono = :icono, titulo = :titulo, subtitulo = :subtitulo, genero = :genero, "); 
	strSQL.append("compania  = :compania, desarrollador = :desarrollador, tema = :tema, idioma = :idioma, ");
	strSQL.append("formato = :formato, anno = :anno, numdisc = :numdisc, sistemaop = :sistemaop, ");
	strSQL.append("tamano = :tamano, graficos = :graficos, sonido = :sonido, jugabilidad = :jugabilidad, ");
	strSQL.append("original = :original, estado = :estado, thumbs = :thumbs, cover_front = :cover_front, ");
	strSQL.append("cover_back = :cover_back, comentario = :comentario, favorito = :favorito ");
	strSQL.append("WHERE idgrl = :idgrl;"); 
	
	QSqlQuery query;
	query.prepare( strSQL );
	
	query.bindValue(":icono"		, datos["icono"]  );
	query.bindValue(":titulo"		, datos["titulo"]  );
	query.bindValue(":subtitulo"	, datos["subtitulo"]  );
	query.bindValue(":genero"		, datos["genero"]  );
	query.bindValue(":compania"		, datos["compania"]  );
	query.bindValue(":desarrollador", datos["desarrollador"]  );
	query.bindValue(":tema"			, datos["tema"]  );
	query.bindValue(":idioma"		, datos["idioma"]  );
	query.bindValue(":formato"		, datos["formato"]  );
	query.bindValue(":anno"			, datos["anno"]  );
	query.bindValue(":numdisc"		, datos["numdisc"] );
	query.bindValue(":sistemaop"	, datos["sistemaop"] );
	query.bindValue(":tamano"		, datos["tamano"] );
	query.bindValue(":graficos"		, datos["graficos"] );
	query.bindValue(":sonido"		, datos["sonido"] );
	query.bindValue(":jugabilidad"	, datos["jugabilidad"] );
	query.bindValue(":original"		, datos["original"] );
	query.bindValue(":estado"		, datos["estado"] );
	query.bindValue(":thumbs"		, datos["thumbs"] );
	query.bindValue(":cover_front"	, datos["cover_front"] );
	query.bindValue(":cover_back"	, datos["cover_back"] );
	//query.bindValue(":fecha"		, datos["fecha"] );
	//query.bindValue(":tipo_emu"	, datos["tipo_emu"] );
	query.bindValue(":comentario"	, datos["comentario"] );
	query.bindValue(":favorito"		, datos["favorito"] );
	query.bindValue(":idgrl"		,   IDgrl   );	// idgrl
	query.exec();
}

void dbSql::ItemActualizaDatosFavorito(const QString EstadoFav , const QString IDgrl )
{
	QSqlQuery query;
	query.prepare( "UPDATE dbgrl SET favorito = :favorito WHERE idgrl = :idgrl;" );
	query.bindValue(":favorito"	, EstadoFav );
	query.bindValue(":idgrl"	, IDgrl     );	// idgrl
	query.exec();
}

QString dbSql::ItemInsertaDbx(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_dosbox (");
	strSQL.append("idgrl, sdl_fullscreen, sdl_fulldouble, sdl_fullfixed, sdl_fullresolution, sdl_windowresolution, sdl_output, ");
	strSQL.append("sdl_hwscale, sdl_autolock, sdl_sensitivity, sdl_waitonerror, sdl_priority, sdl_mapperfile, sdl_usescancodes, ");
	strSQL.append("dosbox_language, dosbox_machine, dosbox_captures, dosbox_memsize, render_frameskip, render_aspect, render_scaler, ");
	strSQL.append("cpu_core, cpu_cycles, cpu_cycleup, cpu_cycledown, mixer_nosound, mixer_rate, mixer_blocksize, mixer_prebuffer, ");
	strSQL.append("midi_mpu401, midi_intelligent, midi_device, midi_config, midi_mt32rate, sblaster_sbtype, sblaster_sbbase, sblaster_irq, ");
	strSQL.append("sblaster_dma, sblaster_hdma, sblaster_mixer, sblaster_oplmode, sblaster_oplrate, gus_gus, gus_gusrate, gus_gusbase, ");
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
	strSQL.append(":cpu_core, :cpu_cycles, :cpu_cycleup, :cpu_cycledown, :mixer_nosound, :mixer_rate, :mixer_blocksize, :mixer_prebuffer, ");
	strSQL.append(":midi_mpu401, :midi_intelligent, :midi_device, :midi_config, :midi_mt32rate, :sblaster_sbtype, :sblaster_sbbase, :sblaster_irq, ");
	strSQL.append(":sblaster_dma, :sblaster_hdma, :sblaster_mixer, :sblaster_oplmode, :sblaster_oplrate, :gus_gus, :gus_gusrate, :gus_gusbase, ");
	strSQL.append(":gus_irq1, :gus_irq2, :gus_dma1, :gus_dma2, :gus_ultradir, :speaker_pcspeaker, :speaker_pcrate, :speaker_tandy, :speaker_tandyrate, ");
	strSQL.append(":speaker_disney, :joystick_type, :joystick_timed, :joystick_autofire, :joystick_swap34, :joystick_buttonwrap, ");
	strSQL.append(":modem_modem, :modem_comport, :modem_listenport, :dserial_directserial, :dserial_comport, ");
	strSQL.append(":dserial_realport, :dserial_defaultbps, :dserial_parity, :dserial_bytesize, :dserial_stopbit, :serial_1, :serial_2, :serial_3, ");
	strSQL.append(":serial_4, :dos_xms, :dos_ems, :dos_umb, :dos_keyboardlayout, :ipx_ipx, :autoexec, :opt_autoexec, :opt_loadfix, :opt_loadfix_mem, :opt_consola_dbox, ");
	strSQL.append(":opt_cerrar_dbox, :opt_cycle_sincronizar, :path_conf, :path_sonido, :path_exe, :path_setup, :parametros_exe, :parametros_setup )");

	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":idgrl"					, IDgrl      );
	query.bindValue(":sdl_fullscreen"			, datos["sdl_fullscreen"]  );
	query.bindValue(":sdl_fulldouble"			, datos["sdl_fulldouble"]  );
	query.bindValue(":sdl_fullfixed"			, datos["sdl_fullfixed"]  );
	query.bindValue(":sdl_fullresolution"		, datos["sdl_fullresolution"]  );
	query.bindValue(":sdl_windowresolution"		, datos["sdl_windowresolution"]  );
	query.bindValue(":sdl_output"				, datos["sdl_output"]  );
	query.bindValue(":sdl_hwscale"				, datos["sdl_hwscale"]  );
	query.bindValue(":sdl_autolock"				, datos["sdl_autolock"]  );
	query.bindValue(":sdl_sensitivity"			, datos["sdl_sensitivity"]  );
	query.bindValue(":sdl_waitonerror"			, datos["sdl_waitonerror"]  );
	query.bindValue(":sdl_priority"				, datos["sdl_priority"]  );
	query.bindValue(":sdl_mapperfile"			, datos["sdl_mapperfile"]  );
	query.bindValue(":sdl_usescancodes"			, datos["sdl_usescancodes"]  );
	query.bindValue(":dosbox_language"			, datos["dosbox_language"]  );
	query.bindValue(":dosbox_machine"			, datos["dosbox_machine"]  );
	query.bindValue(":dosbox_captures"			, datos["dosbox_captures"]  );
	query.bindValue(":dosbox_memsize"			, datos["dosbox_memsize"]  );
	query.bindValue(":render_frameskip"			, datos["render_frameskip"]  );
	query.bindValue(":render_aspect"			, datos["render_aspect"]  );
	query.bindValue(":render_scaler"			, datos["render_scaler"]  );
	query.bindValue(":cpu_core"					, datos["cpu_core"]  );
	query.bindValue(":cpu_cycles"				, datos["cpu_cycles"]  );
	query.bindValue(":cpu_cycleup"				, datos["cpu_cycleup"]  );
	query.bindValue(":cpu_cycledown"			, datos["cpu_cycledown"]  );
	query.bindValue(":mixer_nosound"			, datos["mixer_nosound"]  );
	query.bindValue(":mixer_rate"				, datos["mixer_rate"]  );
	query.bindValue(":mixer_blocksize"			, datos["mixer_blocksize"]  );
	query.bindValue(":mixer_prebuffer"			, datos["mixer_prebuffer"]  );
	query.bindValue(":midi_mpu401"				, datos["midi_mpu401"]  );
	query.bindValue(":midi_intelligent"			, datos["midi_intelligent"]  );
	query.bindValue(":midi_device"				, datos["midi_device"]  );
	query.bindValue(":midi_config"				, datos["midi_config"]  );
	query.bindValue(":midi_mt32rate"			, datos["midi_mt32rate"]  );
	query.bindValue(":sblaster_sbtype"			, datos["sblaster_sbtype"]  );
	query.bindValue(":sblaster_sbbase"			, datos["sblaster_sbbase"]  );
	query.bindValue(":sblaster_irq"				, datos["sblaster_irq"]  );
	query.bindValue(":sblaster_dma"				, datos["sblaster_dma"]  );
	query.bindValue(":sblaster_hdma"			, datos["sblaster_hdma"]  );
	query.bindValue(":sblaster_mixer"			, datos["sblaster_mixer"]  );
	query.bindValue(":sblaster_oplmode"			, datos["sblaster_oplmode"]  );
	query.bindValue(":sblaster_oplrate"			, datos["sblaster_oplrate"]  );
	query.bindValue(":gus_gus"					, datos["gus_gus"]  );
	query.bindValue(":gus_gusrate"				, datos["gus_gusrate"]  );
	query.bindValue(":gus_gusbase"				, datos["gus_gusbase"]  );
	query.bindValue(":gus_irq1"					, datos["gus_irq1"]  );
	query.bindValue(":gus_irq2"					, datos["gus_irq2"]  );
	query.bindValue(":gus_dma1"					, datos["gus_dma1"]  );
	query.bindValue(":gus_dma2"					, datos["gus_dma2"]  );
	query.bindValue(":gus_ultradir"				, datos["gus_ultradir"]  );
	query.bindValue(":speaker_pcspeaker"		, datos["speaker_pcspeaker"]  );
	query.bindValue(":speaker_pcrate"			, datos["speaker_pcrate"]  );
	query.bindValue(":speaker_tandy"			, datos["speaker_tandy"]  );
	query.bindValue(":speaker_tandyrate"		, datos["speaker_tandyrate"]  );
	query.bindValue(":speaker_disney"			, datos["speaker_disney"]  );
	query.bindValue(":joystick_type"			, datos["joystick_type"]  );
	query.bindValue(":joystick_timed"			, datos["joystick_timed"]  );
	query.bindValue(":joystick_autofire"		, datos["joystick_autofire"]  );
	query.bindValue(":joystick_swap34"			, datos["joystick_swap34"]  );
	query.bindValue(":joystick_buttonwrap"		, datos["joystick_buttonwrap"]  );
	query.bindValue(":modem_modem"				, datos["modem_modem"]  );
	query.bindValue(":modem_comport"			, datos["modem_comport"]  );
	query.bindValue(":modem_listenport"			, datos["modem_listenport"]  );
	query.bindValue(":dserial_directserial"		, datos["dserial_directserial"]  );
	query.bindValue(":dserial_comport"			, datos["dserial_comport"]  );
	query.bindValue(":dserial_realport"			, datos["dserial_realport"]  );
	query.bindValue(":dserial_defaultbps"		, datos["dserial_defaultbps"]  );
	query.bindValue(":dserial_parity"			, datos["dserial_parity"]  );
	query.bindValue(":dserial_bytesize"			, datos["dserial_bytesize"]  );
	query.bindValue(":dserial_stopbit"			, datos["dserial_stopbit"]  );
	query.bindValue(":serial_1"					, datos["serial_1"]  );
	query.bindValue(":serial_2"					, datos["serial_2"]  );
	query.bindValue(":serial_3"					, datos["serial_3"]  );
	query.bindValue(":serial_4"					, datos["serial_4"]  );
	query.bindValue(":dos_xms"					, datos["dos_xms"]  );
	query.bindValue(":dos_ems"					, datos["dos_ems"]  );
	query.bindValue(":dos_umb"					, datos["dos_umb"]  );
	query.bindValue(":dos_keyboardlayout"		, datos["dos_keyboardlayout"]  );
	query.bindValue(":ipx_ipx"					, datos["ipx_ipx"]  );
	query.bindValue(":autoexec"					, datos["autoexec"]  );
	query.bindValue(":opt_autoexec"				, datos["opt_autoexec"]  );
	query.bindValue(":opt_loadfix"				, datos["opt_loadfix"]  );
	query.bindValue(":opt_loadfix_mem"			, datos["opt_loadfix_mem"]  );
	query.bindValue(":opt_consola_dbox"			, datos["opt_consola_dbox"]  );
	query.bindValue(":opt_cerrar_dbox"			, datos["opt_cerrar_dbox"]  );
	query.bindValue(":opt_cycle_sincronizar"	, datos["opt_cycle_sincronizar"]  );
	query.bindValue(":path_conf"				, datos["path_conf"]  );
	query.bindValue(":path_sonido"				, datos["path_sonido"]  );
	query.bindValue(":path_exe"					, datos["path_exe"]  );
	query.bindValue(":path_setup"				, datos["path_setup"]  );
	query.bindValue(":parametros_exe"			, datos["parametros_exe"]  );
	query.bindValue(":parametros_setup"			, datos["parametros_setup"]  );
	query.exec();

	if( Chequear_Query( query ) )
	{
		QVariant v;
		v = query.lastInsertId();
		return v.toString();
	}
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
	strSQL.append("cpu_core = :cpu_core , cpu_cycles = :cpu_cycles , cpu_cycleup = :cpu_cycleup , cpu_cycledown = :cpu_cycledown , mixer_nosound = :mixer_nosound ,");
	strSQL.append("mixer_rate = :mixer_rate , mixer_blocksize = :mixer_blocksize , mixer_prebuffer = :mixer_prebuffer , midi_mpu401 = :midi_mpu401 , midi_intelligent = :midi_intelligent ,");
	strSQL.append("midi_device = :midi_device , midi_config = :midi_config , midi_mt32rate = :midi_mt32rate , sblaster_sbtype = :sblaster_sbtype , sblaster_sbbase = :sblaster_sbbase ,");
	strSQL.append("sblaster_irq = :sblaster_irq , sblaster_dma = :sblaster_dma , sblaster_hdma = :sblaster_hdma , sblaster_mixer = :sblaster_mixer , sblaster_oplmode = :sblaster_oplmode ,"); 
	strSQL.append("sblaster_oplrate = :sblaster_oplrate , gus_gus = :gus_gus , gus_gusrate = :gus_gusrate , gus_gusbase = :gus_gusbase , gus_irq1 = :gus_irq1 , gus_irq2 = :gus_irq2 ,");
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
	query.prepare( strSQL );
	//query.bindValue(":idgrl"					, IDgrl      );
	query.bindValue(":sdl_fullscreen"			, datos["sdl_fullscreen"]  );
	query.bindValue(":sdl_fulldouble"			, datos["sdl_fulldouble"]  );
	query.bindValue(":sdl_fullfixed"			, datos["sdl_fullfixed"]  );
	query.bindValue(":sdl_fullresolution"		, datos["sdl_fullresolution"]  );
	query.bindValue(":sdl_windowresolution"		, datos["sdl_windowresolution"]  );
	query.bindValue(":sdl_output"				, datos["sdl_output"]  );
	query.bindValue(":sdl_hwscale"				, datos["sdl_hwscale"]  );
	query.bindValue(":sdl_autolock"				, datos["sdl_autolock"]  );
	query.bindValue(":sdl_sensitivity"			, datos["sdl_sensitivity"]  );
	query.bindValue(":sdl_waitonerror"			, datos["sdl_waitonerror"]  );
	query.bindValue(":sdl_priority"				, datos["sdl_priority"]  );
	query.bindValue(":sdl_mapperfile"			, datos["sdl_mapperfile"]  );
	query.bindValue(":sdl_usescancodes"			, datos["sdl_usescancodes"]  );
	query.bindValue(":dosbox_language"			, datos["dosbox_language"]  );
	query.bindValue(":dosbox_machine"			, datos["dosbox_machine"]  );
	query.bindValue(":dosbox_captures"			, datos["dosbox_captures"]  );
	query.bindValue(":dosbox_memsize"			, datos["dosbox_memsize"]  );
	query.bindValue(":render_frameskip"			, datos["render_frameskip"]  );
	query.bindValue(":render_aspect"			, datos["render_aspect"]  );
	query.bindValue(":render_scaler"			, datos["render_scaler"]  );
	query.bindValue(":cpu_core"					, datos["cpu_core"]  );
	query.bindValue(":cpu_cycles"				, datos["cpu_cycles"]  );
	query.bindValue(":cpu_cycleup"				, datos["cpu_cycleup"]  );
	query.bindValue(":cpu_cycledown"			, datos["cpu_cycledown"]  );
	query.bindValue(":mixer_nosound"			, datos["mixer_nosound"]  );
	query.bindValue(":mixer_rate"				, datos["mixer_rate"]  );
	query.bindValue(":mixer_blocksize"			, datos["mixer_blocksize"]  );
	query.bindValue(":mixer_prebuffer"			, datos["mixer_prebuffer"]  );
	query.bindValue(":midi_mpu401"				, datos["midi_mpu401"]  );
	query.bindValue(":midi_intelligent"			, datos["midi_intelligent"]  );
	query.bindValue(":midi_device"				, datos["midi_device"]  );
	query.bindValue(":midi_config"				, datos["midi_config"]  );
	query.bindValue(":midi_mt32rate"			, datos["midi_mt32rate"]  );
	query.bindValue(":sblaster_sbtype"			, datos["sblaster_sbtype"]  );
	query.bindValue(":sblaster_sbbase"			, datos["sblaster_sbbase"]  );
	query.bindValue(":sblaster_irq"				, datos["sblaster_irq"]  );
	query.bindValue(":sblaster_dma"				, datos["sblaster_dma"]  );
	query.bindValue(":sblaster_hdma"			, datos["sblaster_hdma"]  );
	query.bindValue(":sblaster_mixer"			, datos["sblaster_mixer"]  );
	query.bindValue(":sblaster_oplmode"			, datos["sblaster_oplmode"]  );
	query.bindValue(":sblaster_oplrate"			, datos["sblaster_oplrate"]  );
	query.bindValue(":gus_gus"					, datos["gus_gus"]  );
	query.bindValue(":gus_gusrate"				, datos["gus_gusrate"]  );
	query.bindValue(":gus_gusbase"				, datos["gus_gusbase"]  );
	query.bindValue(":gus_irq1"					, datos["gus_irq1"]  );
	query.bindValue(":gus_irq2"					, datos["gus_irq2"]  );
	query.bindValue(":gus_dma1"					, datos["gus_dma1"]  );
	query.bindValue(":gus_dma2"					, datos["gus_dma2"]  );
	query.bindValue(":gus_ultradir"				, datos["gus_ultradir"]  );
	query.bindValue(":speaker_pcspeaker"		, datos["speaker_pcspeaker"]  );
	query.bindValue(":speaker_pcrate"			, datos["speaker_pcrate"]  );
	query.bindValue(":speaker_tandy"			, datos["speaker_tandy"]  );
	query.bindValue(":speaker_tandyrate"		, datos["speaker_tandyrate"]  );
	query.bindValue(":speaker_disney"			, datos["speaker_disney"]  );
	query.bindValue(":joystick_type"			, datos["joystick_type"]  );
	query.bindValue(":joystick_timed"			, datos["joystick_timed"]  );
	query.bindValue(":joystick_autofire"		, datos["joystick_autofire"]  );
	query.bindValue(":joystick_swap34"			, datos["joystick_swap34"]  );
	query.bindValue(":joystick_buttonwrap"		, datos["joystick_buttonwrap"]  );
	query.bindValue(":modem_modem"				, datos["modem_modem"]  );
	query.bindValue(":modem_comport"			, datos["modem_comport"]  );
	query.bindValue(":modem_listenport"			, datos["modem_listenport"]  );
	query.bindValue(":dserial_directserial"		, datos["dserial_directserial"]  );
	query.bindValue(":dserial_comport"			, datos["dserial_comport"]  );
	query.bindValue(":dserial_realport"			, datos["dserial_realport"]  );
	query.bindValue(":dserial_defaultbps"		, datos["dserial_defaultbps"]  );
	query.bindValue(":dserial_parity"			, datos["dserial_parity"]  );
	query.bindValue(":dserial_bytesize"			, datos["dserial_bytesize"]  );
	query.bindValue(":dserial_stopbit"			, datos["dserial_stopbit"]  );
	query.bindValue(":serial_1"					, datos["serial_1"]  );
	query.bindValue(":serial_2"					, datos["serial_2"]  );
	query.bindValue(":serial_3"					, datos["serial_3"]  );
	query.bindValue(":serial_4"					, datos["serial_4"]  );
	query.bindValue(":dos_xms"					, datos["dos_xms"]  );
	query.bindValue(":dos_ems"					, datos["dos_ems"]  );
	query.bindValue(":dos_umb"					, datos["dos_umb"]  );
	query.bindValue(":dos_keyboardlayout"		, datos["dos_keyboardlayout"]  );
	query.bindValue(":ipx_ipx"					, datos["ipx_ipx"]  );
	query.bindValue(":autoexec"					, datos["autoexec"]  );
	query.bindValue(":opt_autoexec"				, datos["opt_autoexec"]  );
	query.bindValue(":opt_loadfix"				, datos["opt_loadfix"]  );
	query.bindValue(":opt_loadfix_mem"			, datos["opt_loadfix_mem"]  );
	query.bindValue(":opt_consola_dbox"			, datos["opt_consola_dbox"]  );
	query.bindValue(":opt_cerrar_dbox"			, datos["opt_cerrar_dbox"]  );
	query.bindValue(":opt_cycle_sincronizar"	, datos["opt_cycle_sincronizar"]  );
	query.bindValue(":path_conf"				, datos["path_conf"]  );
	query.bindValue(":path_sonido"				, datos["path_sonido"]  );
	query.bindValue(":path_exe"					, datos["path_exe"]  );
	query.bindValue(":path_setup"				, datos["path_setup"]  );
	query.bindValue(":parametros_exe"			, datos["parametros_exe"]  );
	query.bindValue(":parametros_setup"			, datos["parametros_setup"]  );
	query.bindValue(":id"						, IDdbx      );
	query.exec();
}

void dbSql::ItemInsertaMontajesDbx(QTreeWidget *treeWidget, const QString IDdbx)
{
	int num_mount = 0, num_items = 0;
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_dosbox_mount (");
	strSQL.append("id_dosbox, id_lista, path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount");
	strSQL.append(") VALUES ( ");
	strSQL.append(":id_dosbox, :id_lista, :path, :label, :tipo_as, :letter, :indx_cd, :opt_mount, :io_ctrl, :select_mount");
	strSQL.append(")");

	num_items = treeWidget->topLevelItemCount();

	for ( num_mount = 0; num_mount < num_items; num_mount++ )
	{
		QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
		QSqlQuery query;
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
	query.prepare( strSQL );
	query.bindValue(":id_dosbox"   , IDdbx   );
	query.bindValue(":id_lista"    , datos["id_lista"] );
	query.bindValue(":path"        , datos["path"] );
	query.bindValue(":label"       , datos["label"] );
	query.bindValue(":tipo_as"     , datos["tipo_as"] );
	query.bindValue(":letter"      , datos["letter"] );
	query.bindValue(":indx_cd"     , datos["indx_cd"] );
	query.bindValue(":opt_mount"   , datos["opt_mount"] );
	query.bindValue(":io_ctrl"     , datos["io_ctrl"] );
	query.bindValue(":select_mount", datos["select_mount"] );
	query.exec();
	QVariant v;
	v = query.lastInsertId();
	return v.toString();
}

void dbSql::ItemActualizaMontajeDbx(QTreeWidget *treeWidget)
{
	int num_mount;
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_dosbox_mount SET ");
	strSQL.append("id_lista = :id_lista, path = :path, label = :label, ");
	strSQL.append("tipo_as = :tipo_as, letter = :letter, indx_cd = :indx_cd, opt_mount = :opt_mount, ");
	strSQL.append("io_ctrl = :io_ctrl, select_mount = :select_mount ");
	strSQL.append("WHERE id = :id;");
	for ( num_mount = 0; num_mount < treeWidget->topLevelItemCount(); num_mount++ )
	{
		QTreeWidgetItem *item = treeWidget->topLevelItem( num_mount );
		QSqlQuery query;
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

void dbSql::ItemInsertaSvm(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_emu_scummvm (");
	strSQL.append("idgrl, game, language, subtitles, platform, gfx_mode, render_mode, fullscreen, aspect_ratio, path, ");
	strSQL.append("path_setup, path_extra, path_save, path_capturas, path_sonido, music_driver, enable_gs, multi_midi, ");
	strSQL.append("native_mt32, master_volume, music_volume, sfx_volume, speech_volume, tempo, talkspeed, debuglevel, ");
	strSQL.append("cdrom, joystick_num, output_rate, midi_gain, copy_protection, sound_font ");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :game, :language, :subtitles, :platform, :gfx_mode, :render_mode, :fullscreen, :aspect_ratio, :path, ");
	strSQL.append(":path_setup, :path_extra, :path_save, :path_capturas, :path_sonido, :music_driver, :enable_gs, :multi_midi, ");
	strSQL.append(":native_mt32, :master_volume, :music_volume, :sfx_volume, :speech_volume, :tempo, :talkspeed, :debuglevel, ");
	strSQL.append(":cdrom, :joystick_num, :output_rate, :midi_gain, :copy_protection, :sound_font )");

	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":idgrl"			, IDgrl     );
	query.bindValue(":game"				, datos["game"]  );
	query.bindValue(":language"			, datos["language"]  );
	query.bindValue(":subtitles"		, datos["subtitles"]  );
	query.bindValue(":platform"			, datos["platform"]  );
	query.bindValue(":gfx_mode"			, datos["gfx_mode"]  );
	query.bindValue(":render_mode"		, datos["render_mode"]  );
	query.bindValue(":fullscreen"		, datos["fullscreen"]  );
	query.bindValue(":aspect_ratio"		, datos["aspect_ratio"]  );
	query.bindValue(":path"				, datos["path"]  );
	query.bindValue(":path_setup"		, datos["path_setup"]  );
	query.bindValue(":path_extra"		, datos["path_extra"] );
	query.bindValue(":path_save"		, datos["path_save"] );
	query.bindValue(":path_capturas"	, datos["path_capturas"] );
	query.bindValue(":path_sonido"		, datos["path_sonido"] );
	query.bindValue(":music_driver"		, datos["music_driver"] );
	query.bindValue(":enable_gs"		, datos["enable_gs"] );
	query.bindValue(":multi_midi"		, datos["multi_midi"] );
	query.bindValue(":native_mt32"		, datos["native_mt32"] );
	query.bindValue(":master_volume"	, datos["master_volume"] );
	query.bindValue(":music_volume"		, datos["music_volume"] );
	query.bindValue(":sfx_volume"		, datos["sfx_volume"] );
	query.bindValue(":speech_volume"	, datos["speech_volume"] );
	query.bindValue(":tempo"			, datos["tempo"] );
	query.bindValue(":talkspeed"		, datos["talkspeed"] );
	query.bindValue(":debuglevel"		, datos["debuglevel"] );
	query.bindValue(":cdrom"			, datos["cdrom"] );
	query.bindValue(":joystick_num"		, datos["joystick_num"] );
	query.bindValue(":output_rate"		, datos["output_rate"] );
	query.bindValue(":midi_gain"		, datos["midi_gain"] );
	query.bindValue(":copy_protection"	, datos["copy_protection"] );
	query.bindValue(":sound_font"		, datos["sound_font"] );
	query.exec();
}

void dbSql::ItemActualizaSvm(const QHash<QString, QString> datos, const QString IDsvm)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_emu_scummvm SET "); 
	strSQL.append("game = :game, language = :language, subtitles = :subtitles, platform = :platform, "); 
	strSQL.append("gfx_mode  = :gfx_mode, render_mode = :render_mode, fullscreen = :fullscreen, aspect_ratio = :aspect_ratio, ");
	strSQL.append("path = :path, path_setup = :path_setup, path_extra = :path_extra, path_save = :path_save, ");
	strSQL.append("path_capturas = :path_capturas, path_sonido = :path_sonido, music_driver = :music_driver, enable_gs = :enable_gs, ");
	strSQL.append("multi_midi = :multi_midi, native_mt32 = :native_mt32, master_volume = :master_volume, music_volume = :music_volume, ");
	strSQL.append("sfx_volume = :sfx_volume, speech_volume = :speech_volume, tempo = :tempo, talkspeed = :talkspeed, ");
	strSQL.append("debuglevel = :debuglevel, cdrom = :cdrom, joystick_num = :joystick_num, output_rate = :output_rate, ");
	strSQL.append("midi_gain = :midi_gain, copy_protection = :copy_protection, sound_font = :sound_font ");
	
	strSQL.append("WHERE id = :id;"); 
	
	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":game"				, datos["game"]  );
	query.bindValue(":language"			, datos["language"]  );
	query.bindValue(":subtitles"		, datos["subtitles"]  );
	query.bindValue(":platform"			, datos["platform"]  );
	query.bindValue(":gfx_mode"			, datos["gfx_mode"]  );
	query.bindValue(":render_mode"		, datos["render_mode"]  );
	query.bindValue(":fullscreen"		, datos["fullscreen"]  );
	query.bindValue(":aspect_ratio"		, datos["aspect_ratio"]  );
	query.bindValue(":path"				, datos["path"]  );
	query.bindValue(":path_setup"		, datos["path_setup"]  );
	query.bindValue(":path_extra"		, datos["path_extra"] );
	query.bindValue(":path_save"		, datos["path_save"] );
	query.bindValue(":path_capturas"	, datos["path_capturas"] );
	query.bindValue(":path_sonido"		, datos["path_sonido"] );
	query.bindValue(":music_driver"		, datos["music_driver"] );
	query.bindValue(":enable_gs"		, datos["enable_gs"] );
	query.bindValue(":multi_midi"		, datos["multi_midi"] );
	query.bindValue(":native_mt32"		, datos["native_mt32"] );
	query.bindValue(":master_volume"	, datos["master_volume"] );
	query.bindValue(":music_volume"		, datos["music_volume"] );
	query.bindValue(":sfx_volume"		, datos["sfx_volume"] );
	query.bindValue(":speech_volume"	, datos["speech_volume"] );
	query.bindValue(":tempo"			, datos["tempo"] );
	query.bindValue(":talkspeed"		, datos["talkspeed"] );
	query.bindValue(":debuglevel"		, datos["debuglevel"] );
	query.bindValue(":cdrom"			, datos["cdrom"] );
	query.bindValue(":joystick_num"		, datos["joystick_num"] );
	query.bindValue(":output_rate"		, datos["output_rate"] );
	query.bindValue(":midi_gain"		, datos["midi_gain"] );
	query.bindValue(":copy_protection"	, datos["copy_protection"] );
	query.bindValue(":sound_font"		, datos["sound_font"] );
	query.bindValue(":id"				,   IDsvm   );	// id del scummvm
	query.exec();	
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
	query.prepare( strSQL );
	query.bindValue(":idgrl"			, IDgrl    );
	query.bindValue(":path_conf"		, datos["path_conf"] );
	query.bindValue(":path_exe"			, datos["path_exe"] );
	query.bindValue(":program"			, datos["program_1"]    +"|"+ datos["program_2"]	);
	query.bindValue(":vdms_debug"		, datos["vdms_debug_1"] +"|"+ datos["vdms_debug_2"] );
	query.bindValue(":winnt_dos"		, datos["winnt_dos_1"]  +"|"+ datos["winnt_dos_2"]	);
	query.bindValue(":winnt_dosbox"		, datos["winnt_dosbox_1"] +"|"+ datos["winnt_dosbox_2"] +"|"+ datos["winnt_dosbox_3"]);
	query.bindValue(":winnt_storage"	, datos["winnt_storage_1"] +"|"+ datos["winnt_storage_2"]);
		
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
	query.prepare( strSQL );
	query.bindValue(":path_conf"		, datos["path_conf"] );
	query.bindValue(":path_exe"			, datos["path_exe"] );
	query.bindValue(":program"			, datos["program_1"]    +"|"+ datos["program_2"]	);
	query.bindValue(":vdms_debug"		, datos["vdms_debug_1"] +"|"+ datos["vdms_debug_2"] );
	query.bindValue(":winnt_dos"		, datos["winnt_dos_1"]  +"|"+ datos["winnt_dos_2"]	);
	query.bindValue(":winnt_dosbox"		, datos["winnt_dosbox_1"] +"|"+ datos["winnt_dosbox_2"] +"|"+ datos["winnt_dosbox_3"]);
	query.bindValue(":winnt_storage"	, datos["winnt_storage_1"] +"|"+ datos["winnt_storage_2"]);
	query.bindValue(":id"				, IDvdms   );
	query.exec();
}

void dbSql::ItemInsertaFiles(QTreeWidget *treeWidget, const QString IDgrl)
{
	int num_file;
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_file (");
	strSQL.append("idgrl, nombre, crc, descripcion, path, size");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :nombre, :crc, :descripcion, :path, :size )");

	for ( num_file = 0; num_file < treeWidget->topLevelItemCount(); num_file++ )
	{
		QTreeWidgetItem *item_files = treeWidget->topLevelItem( num_file );
		QSqlQuery query;
		query.prepare( strSQL );
		query.bindValue(":idgrl"		, IDgrl    );
		query.bindValue(":nombre"		, item_files->text(0) );
		query.bindValue(":crc"			, item_files->text(1) );
		query.bindValue(":descripcion"	, item_files->text(2) );
		query.bindValue(":path"			, item_files->text(4) );
		query.bindValue(":size"			, item_files->text(3) );
		query.exec();
	}
}

QString dbSql::ItemInsertaUnFiles(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_file (");
	strSQL.append("idgrl, nombre, crc, descripcion, path, size");
	strSQL.append(") VALUES ( ");
	strSQL.append(":idgrl, :nombre, :crc, :descripcion, :path, :size )");

	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":idgrl"		, IDgrl    );
	query.bindValue(":nombre"		, datos["nombre"] );
	query.bindValue(":crc"			, datos["crc"] );
	query.bindValue(":descripcion"	, datos["descripcion"] );
	query.bindValue(":path"			, datos["path"] );
	query.bindValue(":size"			, datos["size"] );
	query.exec();
	QVariant v;
	v = query.lastInsertId();
	return v.toString();
}

void dbSql::ItemActualizaFiles(const QHash<QString, QString> datos, const QString IDFiles)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_file SET "); 
	strSQL.append("nombre = :nombre, crc = :crc, descripcion = :descripcion, path = :path, size = :size "); 
	strSQL.append("WHERE id = :id;"); 

	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":nombre"		, datos["nombre"] );
	query.bindValue(":crc"			, datos["crc"] );
	query.bindValue(":descripcion"	, datos["descripcion"] );
	query.bindValue(":path"			, datos["path"] );
	query.bindValue(":size"			, datos["size"] );
	query.bindValue(":id"			, IDFiles  );
	query.exec();
}

void dbSql::ItemEliminarFiles( const QString IDFiles )
{
	QSqlQuery query;
	query.exec("DELETE FROM dbgrl_file WHERE id="+IDFiles);
	query.clear();
}

void dbSql::ItemInsertaURL(QTreeWidget *treeWidget, const QString IDgrl)
{
	int num_url;
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_url ( idgrl, url, descripcion ) VALUES ( :idgrl, :url, :descripcion )");

	for ( num_url = 0; num_url < treeWidget->topLevelItemCount(); num_url++ )
	{
		QTreeWidgetItem *item_files = treeWidget->topLevelItem( num_url );
		QSqlQuery query;
		query.prepare( strSQL );
		query.bindValue(":idgrl"		, IDgrl    );
		query.bindValue(":url"			, item_files->text(0) );
		query.bindValue(":descripcion"	, item_files->text(1) );
		query.exec();
	}
}

QString dbSql::ItemInsertaUnURL(const QHash<QString, QString> datos, const QString IDgrl)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("INSERT INTO dbgrl_url ( idgrl, url, descripcion ) VALUES ( :idgrl, :url, :descripcion )");
	
	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":idgrl"		, IDgrl    );
	query.bindValue(":url"			, datos["url"] );
	query.bindValue(":descripcion"	, datos["descripcion"] );
	query.exec();
	QVariant v;
	v = query.lastInsertId();
	return v.toString();
}

void dbSql::ItemActualizaURL(const QHash<QString, QString> datos, const QString IDURL)
{
	QString strSQL;
	strSQL.clear();
	strSQL.append("UPDATE dbgrl_url SET url = :url, descripcion = :descripcion WHERE id = :id;"); 

	QSqlQuery query;
	query.prepare( strSQL );
	query.bindValue(":url"			, datos["url"] );
	query.bindValue(":descripcion"	, datos["descripcion"] );
	query.bindValue(":id"			, IDURL  );
	query.exec();
}

void dbSql::ItemEliminarURL( const QString IDURL )
{
	QSqlQuery query;
	query.exec("DELETE FROM dbgrl_url WHERE id="+IDURL);
	query.clear();
}

void dbSql::CrearTablas()
{
// Crea las Tablas de la Base de Datos si es Necesario
	QSqlQuery query;

//	Añade las columnas si no están disponibles
	query.exec("ALTER TABLE 'dbgrl' ADD COLUMN 'favorito' VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'output_rate' VARCHAR(10) NOT NULL DEFAULT '<defecto>';");
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'midi_gain' integer NOT NULL default 100;");
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'copy_protection' VARCHAR(5) NOT NULL DEFAULT 'false';");
	query.exec("ALTER TABLE 'dbgrl_emu_scummvm' ADD COLUMN 'sound_font' VARCHAR(255) NOT NULL DEFAULT '';");

// Añade las distintas tablas si no están disponibles
// Tabla principal - dbgrl
	query.exec(
	"CREATE TABLE `dbgrl` ("
	"	`idgrl`					integer NOT NULL primary key,"
	"	`icono`					varchar(255) NOT NULL default '',"
	"	`titulo`				varchar(255) NOT NULL default '',"
	"	`subtitulo`				varchar(255) NOT NULL default '',"
	"	`genero`				varchar(255) NOT NULL default '',"
	"	`compania`				varchar(255) NOT NULL default '',"
	"	`desarrollador`			varchar(255) NOT NULL default '',"
	"	`tema`					varchar(255) NOT NULL default '',"
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
	"	`favorito`				varchar(5) NOT NULL default 'false'"
	");");

// Tabla - dbgrl_emu_dosbox
	query.exec(
	"CREATE TABLE `dbgrl_emu_dosbox` ("
	"	`id`					integer NOT NULL primary key,"
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
	"	`dosbox_machine`		varchar(10) NOT NULL default 'vga',"
	"	`dosbox_captures`		varchar(255) NOT NULL default 'capture',"
	"	`dosbox_memsize`		varchar(4) NOT NULL default '16',"
	"	`render_frameskip`		varchar(2) NOT NULL default '0',"
	"	`render_aspect`			varchar(5) NOT NULL default 'false',"
	"	`render_scaler`			varchar(15) NOT NULL default 'normal2x'," 
	"	`cpu_core`				varchar(10) NOT NULL default 'auto',"
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
	"	`sblaster_oplrate`		varchar(10) NOT NULL default '22050',"
	"	`gus_gus`				varchar(5) NOT NULL default 'true',"
	"	`gus_gusrate`			varchar(10) NOT NULL default '22050',"
	"	`gus_gusbase`			varchar(4) NOT NULL default '240',"
	"	`gus_irq1`				varchar(4) NOT NULL default '5',"
	"	`gus_irq2`				varchar(4) NOT NULL default '5',"
	"	`gus_dma1`				varchar(4) NOT NULL default '3',"
	"	`gus_dma2`				varchar(4) NOT NULL default '3',"
	"	`gus_ultradir`			varchar(255) NOT NULL default 'C:/ULTRASND',"
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
	"	`dos_keyboardlayout`	varchar(5) NOT NULL default 'none',"
	"	`ipx_ipx`				varchar(5) NOT NULL default 'true',"
	"	`autoexec`				longtext,"
	"	`opt_autoexec`			varchar(5) NOT NULL default 'false',"
	"	`opt_loadfix`			varchar(5) NOT NULL default 'false',"
	"	`opt_loadfix_mem`		varchar(5) NOT NULL default '64',"
	"	`opt_consola_dbox`		varchar(5) NOT NULL default 'false',"
	"	`opt_cerrar_dbox`		varchar(5) NOT NULL default 'false',"
	"	`opt_cycle_sincronizar` varchar(5) NOT NULL default 'false',"
	"	`path_conf`				varchar(255) NOT NULL default '',"
	"	`path_sonido`			varchar(255) NOT NULL default 'waves',"
	"	`path_exe`				varchar(255) NOT NULL default '',"
	"	`path_setup`			varchar(255) NOT NULL default '',"
	"	`parametros_exe`		varchar(255) NOT NULL default '',"
	"	`parametros_setup`		varchar(255) NOT NULL default ''"
	");");
	
// Tabla - dbgrl_emu_dosbox_mount
    query.exec(
	"CREATE TABLE `dbgrl_emu_dosbox_mount` ("
	"	`id`					integer NOT NULL primary key,"
	"	`id_dosbox`				integer NOT NULL default 1,"
	"	`id_lista`				varchar(255) NOT NULL default '0',"
	"	`path`					varchar(255) NOT NULL default 'C:/',"
	"	`label`					varchar(255) NOT NULL default '',"
	"	`tipo_as`				varchar(50) NOT NULL default 'drive',"
	"	`letter`				varchar(255) NOT NULL default 'C',"
	"	`indx_cd`				varchar(255) NOT NULL default '0',"
	"	`opt_mount`				varchar(255) NOT NULL default '',"
	"	`io_ctrl`				varchar(50) NOT NULL default 'false',"
	"	`select_mount`			varchar(10) NOT NULL default 'x'"
	");");

// Tabla - dbgrl_emu_scummvm
    query.exec(
	"CREATE TABLE `dbgrl_emu_scummvm` ("
	"	`id`					integer NOT NULL primary key,"
	"	`idgrl`					integer NOT NULL default 1,"
	"	`game`					varchar(255) NOT NULL default '',"
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
    "	`sound_font`			varchar(255) NOT NULL default ''"
	");");

// Tabla - dbgrl_emu_vdmsound
    query.exec(
	"CREATE TABLE `dbgrl_emu_vdmsound` ("
	"	`id`					integer NOT NULL primary key,"
	"	`idgrl`					integer NOT NULL default 1,"
	"	`path_conf`				text,"
	"	`path_exe`				text,"
	"	`program`				text,"
	"	`vdms_debug`			text,"
	"	`winnt_dos`				text,"
	"	`winnt_dosbox`			text,"
	"	`winnt_storage`			text"
	");");

// Tabla - dbgrl_file
    query.exec(
	"CREATE TABLE `dbgrl_file` ("
	"	`id`					integer NOT NULL primary key,"
	"	`idgrl`					integer NOT NULL default 1,"
	"	`nombre`				varchar(255) NOT NULL default '',"
	"	`crc`					varchar(8) NOT NULL default '00000000',"
	"	`descripcion`			varchar(255) NOT NULL default '',"
	"	`path`					varchar(255) NOT NULL default '',"
	"	`size`					varchar(50) NOT NULL default ''"
	");");

// Tabla - dbgrl_url
    query.exec(
	"CREATE TABLE `dbgrl_url` ("
	"	`id`					integer NOT NULL primary key,"
	"	`idgrl`					integer NOT NULL default 1,"
	"	`url`					varchar(255) NOT NULL default '',"
	"	`descripcion`			varchar(255) NOT NULL default ''"
	");");

//	bool dbgrl_url_ok;
//	dbgrl_url_ok = query.exec("SHOW TABLES LIKE dbgrl_url");
//	if( dbgrl_url_ok == false )
//	{
//		QMessageBox::critical( 0, QCoreApplication::applicationName(), "SQL: creada la tabla dbgrl_url");
//	} else
//		QMessageBox::critical( 0, QCoreApplication::applicationName(), "SQL: No se ha creado nada (dbgrl_url)");

}
