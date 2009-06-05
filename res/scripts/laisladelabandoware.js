/*
 * GR-lida importation script
 * http://sharesource.org/project/grlida/
 * http://www.laisladelabandoware.es
 * 
 */
// Info -------------------------------
var authors  = "Monthy";					// Script authors
var title    = "La Isla del Abandoware (ES)";
var url_site = "http://www.laisladelabandoware.es";	//Site address
var language = "es-ES";						// Site language
var version  = "0.1.0";						// Script version 17-11-2008
var requires = "0.8.0";						// GR-lida version
var comments = "";
var license  = "GPL v2";
var description = "";

function UrlWebBusqueda(texto_busqueda)
{
	var m_url = new Array();

	m_url["metodo"] = "GET";	// POST, GET
	m_url["c_post"] = "";		// Contenido del POST si es post
	m_url["url"]    = "http://www.laisladelabandoware.es/grlidadb.php?ver=lista&titulo="+texto_busqueda;

	return m_url;
}

function AnalyzeFindPage(texto)
{
	myReFind = new RegExp("<juego ID=\"([0-9]+)\">([^\"]*)</juego>","g");
	resultsFind = texto.match(myReFind);
	for(var i = 0; i < resultsFind.length; i++)
	{
		results_id         = AnalyzeTagName(resultsFind[i], "ID=\"([0-9]+)\"");
		results_id_emu     = AnalyzeTagName(resultsFind[i], "<id_emu>([0-9]+)</id_emu>");
		results_titulo     = AnalyzeTagName(resultsFind[i], "<titulo>([^\"<>]*)</titulo>");
		results_plataforma = AnalyzeTagName(resultsFind[i], "<plataforma>([^\"<>]*)</plataforma>");
		results_publicado  = AnalyzeTagName(resultsFind[i], "<publicado>([0-9]+)</publicado>");
		results_tipo_emu   = AnalyzeTagName(resultsFind[i], "<tipo_emu>([^\"<>]*)</tipo_emu>", "datos");

		twListaBusqueda.addItemFind(results_titulo, results_plataforma, results_publicado, url_site+"/grlidadb.php?ver=juego&gid="+results_id+"&id_emu="+results_id_emu+"&tipo_emu="+results_tipo_emu, results_tipo_emu);		
	}
}

function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();

	m_array["Dat_icono"]            = AnalyzeTagName(texto, "<icono>([^\"<>]*)</icono>", "datos");
	m_array["Dat_titulo"]           = AnalyzeTagName(texto, "<titulo>([^\"<>]*)</titulo>");
	m_array["Dat_subtitulo"]        = AnalyzeTagName(texto, "<subtitulo>([^\"<>]*)</subtitulo>");
	m_array["Dat_genero"]           = AnalyzeTagName(texto, "<genero>([^\"<>]*)</genero>");
	m_array["Dat_compania"]         = AnalyzeTagName(texto, "<compania>([^\"<>]*)</compania>");
	m_array["Dat_desarrollador"]    = AnalyzeTagName(texto, "<desarrollador>([^\"<>]*)</desarrollador>");
	m_array["Dat_tema"]             = AnalyzeTagName(texto, "<tema>([^\"<>]*)</tema>");
	m_array["Dat_idioma"]           = AnalyzeTagName(texto, "<idioma>([^\"<>]*)</idioma>");
	m_array["Dat_formato"]          = AnalyzeTagName(texto, "<formato>([^\"<>]*)</formato>");
	m_array["Dat_anno"]             = AnalyzeTagName(texto, "<anno>([^\"<>]*)</anno>");
	m_array["Dat_numdisc"]          = AnalyzeTagName(texto, "<numdisc>([^\"<>]*)</numdisc>");
	m_array["Dat_sistemaop"]        = AnalyzeTagName(texto, "<sistemaop>([^\"<>]*)</sistemaop>");
	m_array["Dat_tamano"]           = AnalyzeTagName(texto, "<tamano>([^\"<>]*)</tamano>");
	m_array["Dat_graficos"]         = AnalyzeTagName(texto, "<graficos>([^\"<>]*)</graficos>");
	m_array["Dat_sonido"]           = AnalyzeTagName(texto, "<sonido>([^\"<>]*)</sonido>");
	m_array["Dat_jugabilidad"]      = AnalyzeTagName(texto, "<jugabilidad>([^\"<>]*)</jugabilidad>");
	m_array["Dat_original"]         = AnalyzeTagName(texto, "<original>([^\"<>]*)</original>");
	m_array["Dat_estado"]           = AnalyzeTagName(texto, "<estado>([^\"<>]*)</estado>");
	m_array["Dat_thumbs"]           = AnalyzeTagName(texto, "<thumbs>([^\"<>]*)</thumbs>");
	m_array["Dat_cover_front"]      = AnalyzeTagName(texto, "<cover_front>([^\"<>]*)</cover_front>");
	m_array["Dat_cover_back"]       = AnalyzeTagName(texto, "<cover_back>([^\"<>]*)</cover_back>");
	m_array["Dat_fecha"]            = AnalyzeTagName(texto, "<fecha>([^\"<>]*)</fecha>");
	m_array["Dat_tipo_emu"]         = AnalyzeTagName(texto, "<tipo_emu>([^\"<>]*)</tipo_emu>", "datos");
	m_array["Dat_comentario"]       = AnalyzeTagName(texto, "<comentario>([^[]*)</comentario>");
	m_array["Dat_favorito"]         = "";
	m_array["Dat_rating"]           = "0";
	m_array["Dat_usuario"]          = AnalyzeTagName(texto, "<usuario>([^\"<>]*)</usuario>");
	m_array["Dat_adese_rating"]     = "0";
	m_array["Dat_perspective"]      = "";
	m_array["Dat_misc"]             = "";

// Imagenes Caratula --------
	if( m_array["Dat_thumbs"] == "" || m_array["Dat_thumbs"] == "null" )
	{
		m_array["Dat_thumbs"] = "";
		m_array["Dat_url_cover_thumbs"] = "";
	} else {
		if(local)
			m_array["Dat_url_cover_thumbs"] = m_array["Dat_thumbs"];
		else
			m_array["Dat_url_cover_thumbs"] = "http://www.laisladelabandoware.es/images/covers/small/"+m_array["Dat_thumbs"];
	}

	if( m_array["Dat_cover_front"] == "" || m_array["Dat_cover_front"] == "null" )
	{
		m_array["Dat_cover_front"] = "";
		m_array["Dat_url_cover_front"]  = "";
	} else {
		if(local)
			m_array["Dat_url_cover_front"] = m_array["Dat_cover_front"];
		else
			m_array["Dat_url_cover_front"]  = "http://www.laisladelabandoware.es/images/covers/large/"+m_array["Dat_cover_front"];
	}

	if( m_array["Dat_cover_back"] == "" || m_array["Dat_cover_back"] == "null" )
	{
		m_array["Dat_cover_back"] = "";
		m_array["Dat_url_cover_back"]   = "";	
	} else {
		if(local)
			m_array["Dat_url_cover_back"] = m_array["Dat_cover_back"];
		else	
			m_array["Dat_url_cover_back"]   = "http://www.laisladelabandoware.es/images/covers/large/"+m_array["Dat_cover_back"];
	}
// FIN Imagenes Caratula ----

	if( m_array["Dat_tipo_emu"] == "dosbox" )
	{
	// Datos del Configuración del DOSBox
		m_array["Dbx_sdl_fullscreen"]        = AnalyzeTagName(texto, "<sdl_fullscreen>([^\"<>]*)</sdl_fullscreen>", "");
		m_array["Dbx_sdl_fulldouble"]        = AnalyzeTagName(texto, "<sdl_fulldouble>([^\"<>]*)</sdl_fulldouble>", "");
		m_array["Dbx_sdl_fullfixed"]         = AnalyzeTagName(texto, "<sdl_fullfixed>([^\"<>]*)</sdl_fullfixed>", "");
		m_array["Dbx_sdl_fullresolution"]    = AnalyzeTagName(texto, "<sdl_fullresolution>([^\"<>]*)</sdl_fullresolution>", "");
		m_array["Dbx_sdl_windowresolution"]  = AnalyzeTagName(texto, "<sdl_windowresolution>([^\"<>]*)</sdl_windowresolution>", "");
		m_array["Dbx_sdl_output"]            = AnalyzeTagName(texto, "<sdl_output>([^\"<>]*)</sdl_output>", "");
		m_array["Dbx_sdl_hwscale"]           = AnalyzeTagName(texto, "<sdl_hwscale>([^\"<>]*)</sdl_hwscale>", "");
		m_array["Dbx_sdl_autolock"]          = AnalyzeTagName(texto, "<sdl_autolock>([^\"<>]*)</sdl_autolock>", "");
		m_array["Dbx_sdl_sensitivity"]       = AnalyzeTagName(texto, "<sdl_sensitivity>([^\"<>]*)</sdl_sensitivity>", "");
		m_array["Dbx_sdl_waitonerror"]       = AnalyzeTagName(texto, "<sdl_waitonerror>([^\"<>]*)</sdl_waitonerror>", "");
		m_array["Dbx_sdl_priority"]          = AnalyzeTagName(texto, "<sdl_priority>([^\"<>]*)</sdl_priority>", "");
		m_array["Dbx_sdl_mapperfile"]        = AnalyzeTagName(texto, "<sdl_mapperfile>([^\"<>]*)</sdl_mapperfile>", "");
		m_array["Dbx_sdl_usescancodes"]      = AnalyzeTagName(texto, "<sdl_usescancodes>([^\"<>]*)</sdl_usescancodes>", "");
		m_array["Dbx_dosbox_language"]       = AnalyzeTagName(texto, "<dosbox_language>([^\"<>]*)</dosbox_language>", "");
		m_array["Dbx_dosbox_machine"]        = AnalyzeTagName(texto, "<dosbox_machine>([^\"<>]*)</dosbox_machine>", "");
		m_array["Dbx_dosbox_captures"]       = AnalyzeTagName(texto, "<dosbox_captures>([^\"<>]*)</dosbox_captures>", "");
		m_array["Dbx_dosbox_memsize"]        = AnalyzeTagName(texto, "<dosbox_memsize>([^\"<>]*)</dosbox_memsize>", "");
		m_array["Dbx_render_frameskip"]      = AnalyzeTagName(texto, "<render_frameskip>([^\"<>]*)</render_frameskip>", "");
		m_array["Dbx_render_aspect"]         = AnalyzeTagName(texto, "<render_aspect>([^\"<>]*)</render_aspect>", "");
		m_array["Dbx_render_scaler"]         = AnalyzeTagName(texto, "<render_scaler>([^\"<>]*)</render_scaler>", "");
		m_array["Dbx_cpu_core"]              = AnalyzeTagName(texto, "<cpu_core>([^\"<>]*)</cpu_core>", "");
		m_array["Dbx_cpu_cputype"]           = AnalyzeTagName(texto, "<cpu_cputype>([^\"<>]*)</cpu_cputype>", "");
		m_array["Dbx_cpu_cycles"]            = AnalyzeTagName(texto, "<cpu_cycles>([^\"<>]*)</cpu_cycles>", "");
		m_array["Dbx_cpu_cycleup"]           = AnalyzeTagName(texto, "<cpu_cycleup>([^\"<>]*)</cpu_cycleup>", "");
		m_array["Dbx_cpu_cycledown"]         = AnalyzeTagName(texto, "<cpu_cycledown>([^\"<>]*)</cpu_cycledown>", "");
		m_array["Dbx_mixer_nosound"]         = AnalyzeTagName(texto, "<mixer_nosound>([^\"<>]*)</mixer_nosound>", "");
		m_array["Dbx_mixer_rate"]            = AnalyzeTagName(texto, "<mixer_rate>([^\"<>]*)</mixer_rate>", "");
		m_array["Dbx_mixer_blocksize"]       = AnalyzeTagName(texto, "<mixer_blocksize>([^\"<>]*)</mixer_blocksize>", "");
		m_array["Dbx_mixer_prebuffer"]       = AnalyzeTagName(texto, "<mixer_prebuffer>([^\"<>]*)</mixer_prebuffer>", "");
		m_array["Dbx_midi_mpu401"]           = AnalyzeTagName(texto, "<midi_mpu401>([^\"<>]*)</midi_mpu401>", "");
		m_array["Dbx_midi_intelligent"]      = AnalyzeTagName(texto, "<midi_intelligent>([^\"<>]*)</midi_intelligent>", "");
		m_array["Dbx_midi_device"]           = AnalyzeTagName(texto, "<midi_device>([^\"<>]*)</midi_device>", "");
		m_array["Dbx_midi_config"]           = AnalyzeTagName(texto, "<midi_config>([^\"<>]*)</midi_config>", "");
		m_array["Dbx_midi_mt32rate"]         = AnalyzeTagName(texto, "<midi_mt32rate>([^\"<>]*)</midi_mt32rate>", "");
		m_array["Dbx_sblaster_sbtype"]       = AnalyzeTagName(texto, "<sblaster_sbtype>([^\"<>]*)</sblaster_sbtype>", "");
		m_array["Dbx_sblaster_sbbase"]       = AnalyzeTagName(texto, "<sblaster_sbbase>([^\"<>]*)</sblaster_sbbase>", "");
		m_array["Dbx_sblaster_irq"]          = AnalyzeTagName(texto, "<sblaster_irq>([^\"<>]*)</sblaster_irq>", "");
		m_array["Dbx_sblaster_dma"]          = AnalyzeTagName(texto, "<sblaster_dma>([^\"<>]*)</sblaster_dma>", "");
		m_array["Dbx_sblaster_hdma"]         = AnalyzeTagName(texto, "<sblaster_hdma>([^\"<>]*)</sblaster_hdma>", "");
		m_array["Dbx_sblaster_mixer"]        = AnalyzeTagName(texto, "<sblaster_mixer>([^\"<>]*)</sblaster_mixer>", "");
		m_array["Dbx_sblaster_oplmode"]      = AnalyzeTagName(texto, "<sblaster_oplmode>([^\"<>]*)</sblaster_oplmode>", "");
		m_array["Dbx_sblaster_oplemu"]       = AnalyzeTagName(texto, "<sblaster_oplemu>([^\"<>]*)</sblaster_oplemu>", "");
		m_array["Dbx_sblaster_oplrate"]      = AnalyzeTagName(texto, "<sblaster_oplrate>([^\"<>]*)</sblaster_oplrate>", "");
		m_array["Dbx_gus_gus"]               = AnalyzeTagName(texto, "<gus_gus>([^\"<>]*)</gus_gus>", "");
		m_array["Dbx_gus_gusrate"]           = AnalyzeTagName(texto, "<gus_gusrate>([^\"<>]*)</gus_gusrate>", "");
		m_array["Dbx_gus_gusbase"]           = AnalyzeTagName(texto, "<gus_gusbase>([^\"<>]*)</gus_gusbase>", "");
		m_array["Dbx_gus_irq1"]              = AnalyzeTagName(texto, "<gus_irq1>([^\"<>]*)</gus_irq1>", "");
		m_array["Dbx_gus_irq2"]              = AnalyzeTagName(texto, "<gus_irq2>([^\"<>]*)</gus_irq2>", "");
		m_array["Dbx_gus_dma1"]              = AnalyzeTagName(texto, "<gus_dma1>([^\"<>]*)</gus_dma1>", "");
		m_array["Dbx_gus_dma2"]              = AnalyzeTagName(texto, "<gus_dma2>([^\"<>]*)</gus_dma2>", "");
		m_array["Dbx_gus_ultradir"]          = AnalyzeTagName(texto, "<gus_ultradir>([^\"<>]*)</gus_ultradir>", "");
		m_array["Dbx_speaker_pcspeaker"]     = AnalyzeTagName(texto, "<speaker_pcspeaker>([^\"<>]*)</speaker_pcspeaker>", "");
		m_array["Dbx_speaker_pcrate"]        = AnalyzeTagName(texto, "<speaker_pcrate>([^\"<>]*)</speaker_pcrate>", "");
		m_array["Dbx_speaker_tandy"]         = AnalyzeTagName(texto, "<speaker_tandy>([^\"<>]*)</speaker_tandy>", "");
		m_array["Dbx_speaker_tandyrate"]     = AnalyzeTagName(texto, "<speaker_tandyrate>([^\"<>]*)</speaker_tandyrate>", "");
		m_array["Dbx_speaker_disney"]        = AnalyzeTagName(texto, "<speaker_disney>([^\"<>]*)</speaker_disney>", "");
		m_array["Dbx_joystick_type"]         = AnalyzeTagName(texto, "<joystick_type>([^\"<>]*)</joystick_type>", "");
		m_array["Dbx_joystick_timed"]        = AnalyzeTagName(texto, "<joystick_timed>([^\"<>]*)</joystick_timed>", "");
		m_array["Dbx_joystick_autofire"]     = AnalyzeTagName(texto, "<joystick_autofire>([^\"<>]*)</joystick_autofire>", "");
		m_array["Dbx_joystick_swap34"]       = AnalyzeTagName(texto, "<joystick_swap34>([^\"<>]*)</joystick_swap34>", "");
		m_array["Dbx_joystick_buttonwrap"]   = AnalyzeTagName(texto, "<joystick_buttonwrap>([^\"<>]*)</joystick_buttonwrap>", "");
		m_array["Dbx_modem_modem"]           = AnalyzeTagName(texto, "<modem_modem>([^\"<>]*)</modem_modem>", "");
		m_array["Dbx_modem_comport"]         = AnalyzeTagName(texto, "<modem_comport>([^\"<>]*)</modem_comport>", "");
		m_array["Dbx_modem_listenport"]      = AnalyzeTagName(texto, "<modem_listenport>([^\"<>]*)</modem_listenport>", "");
		m_array["Dbx_dserial_directserial"]  = AnalyzeTagName(texto, "<dserial_directserial>([^\"<>]*)</dserial_directserial>", "");
		m_array["Dbx_dserial_comport"]       = AnalyzeTagName(texto, "<dserial_comport>([^\"<>]*)</dserial_comport>", "");
		m_array["Dbx_dserial_realport"]      = AnalyzeTagName(texto, "<dserial_realport>([^\"<>]*)</dserial_realport>", "");
		m_array["Dbx_dserial_defaultbps"]    = AnalyzeTagName(texto, "<dserial_defaultbps>([^\"<>]*)</dserial_defaultbps>", "");
		m_array["Dbx_dserial_parity"]        = AnalyzeTagName(texto, "<dserial_parity>([^\"<>]*)</dserial_parity>", "");
		m_array["Dbx_dserial_bytesize"]      = AnalyzeTagName(texto, "<dserial_bytesize>([^\"<>]*)</dserial_bytesize>", "");
		m_array["Dbx_dserial_stopbit"]       = AnalyzeTagName(texto, "<dserial_stopbit>([^\"<>]*)</dserial_stopbit>", "");
		m_array["Dbx_serial_1"]              = AnalyzeTagName(texto, "<serial_1>([^\"<>]*)</serial_1>", "");
		m_array["Dbx_serial_2"]              = AnalyzeTagName(texto, "<serial_2>([^\"<>]*)</serial_2>", "");
		m_array["Dbx_serial_3"]              = AnalyzeTagName(texto, "<serial_3>([^\"<>]*)</serial_3>", "");
		m_array["Dbx_serial_4"]              = AnalyzeTagName(texto, "<serial_4>([^\"<>]*)</serial_4>", "");
		m_array["Dbx_dos_xms"]               = AnalyzeTagName(texto, "<dos_xms>([^\"<>]*)</dos_xms>", "");
		m_array["Dbx_dos_ems"]               = AnalyzeTagName(texto, "<dos_ems>([^\"<>]*)</dos_ems>", "");
		m_array["Dbx_dos_umb"]               = AnalyzeTagName(texto, "<dos_umb>([^\"<>]*)</dos_umb>", "");
		m_array["Dbx_dos_keyboardlayout"]    = AnalyzeTagName(texto, "<dos_keyboardlayout>([^\"<>]*)</dos_keyboardlayout>", "");
		m_array["Dbx_ipx_ipx"]               = AnalyzeTagName(texto, "<ipx_ipx>([^\"<>]*)</ipx_ipx>", "");
		m_array["Dbx_autoexec"]              = AnalyzeTagName(texto, "<autoexec>([^\"<>]*)</autoexec>", "");
		m_array["Dbx_opt_autoexec"]          = AnalyzeTagName(texto, "<opt_autoexec>([^\"<>]*)</opt_autoexec>", "");
		m_array["Dbx_opt_loadfix"]           = AnalyzeTagName(texto, "<opt_loadfix>([^\"<>]*)</opt_loadfix>", "");
		m_array["Dbx_opt_loadfix_mem"]       = AnalyzeTagName(texto, "<opt_loadfix_mem>([^\"<>]*)</opt_loadfix_mem>", "");
		m_array["Dbx_opt_consola_dbox"]      = AnalyzeTagName(texto, "<opt_consola_dbox>([^\"<>]*)</opt_consola_dbox>", "");
		m_array["Dbx_opt_cerrar_dbox"]       = AnalyzeTagName(texto, "<opt_cerrar_dbox>([^\"<>]*)</opt_cerrar_dbox>", "");
		m_array["Dbx_opt_cycle_sincronizar"] = AnalyzeTagName(texto, "<opt_cycle_sincronizar>([^\"<>]*)</opt_cycle_sincronizar>", "");
		m_array["Dbx_path_conf"]             = AnalyzeTagName(texto, "<path_conf>([^\"<>]*)</path_conf>", "");
		m_array["Dbx_path_sonido"]           = AnalyzeTagName(texto, "<path_sonido>([^\"<>]*)</path_sonido>", "");
		m_array["Dbx_path_exe"]              = AnalyzeTagName(texto, "<path_exe>([^\"<>]*)</path_exe>", "");
		m_array["Dbx_path_setup"]            = AnalyzeTagName(texto, "<path_setup>([^\"<>]*)</path_setup>", "");
		m_array["Dbx_parametros_exe"]        = AnalyzeTagName(texto, "<parametros_exe>([^\"<>]*)</parametros_exe>", "");
		m_array["Dbx_parametros_setup"]      = AnalyzeTagName(texto, "<parametros_setup>([^\"<>]*)</parametros_setup>", "");

	// Añadimos los distintos Montajes
		myDbxRE = new RegExp("<montajes>([^\"]*)</montajes>","g");
		resultsDbx = texto.match(myDbxRE);
		for(var i_dbx = 0; i_dbx < resultsDbx.length; i_dbx++)
		{
			path         = AnalyzeTagName(resultsDbx[i_dbx], "<path>([^\"<>]*)</path>");
			label        = AnalyzeTagName(resultsDbx[i_dbx], "<label>([^\"<>]*)</label>");
			tipo_as      = AnalyzeTagName(resultsDbx[i_dbx], "<tipo_as>([^\"<>]*)</tipo_as>");
			letter       = AnalyzeTagName(resultsDbx[i_dbx], "<letter>([^\"<>]*)</letter>");
			indx_cd      = AnalyzeTagName(resultsDbx[i_dbx], "<indx_cd>([^\"<>]*)</indx_cd>");
			opt_mount    = AnalyzeTagName(resultsDbx[i_dbx], "<opt_mount>([^\"<>]*)</opt_mount>");
			io_ctrl      = AnalyzeTagName(resultsDbx[i_dbx], "<io_ctrl>([^\"<>]*)</io_ctrl>");
			select_mount = AnalyzeTagName(resultsDbx[i_dbx], "<select_mount>([^\"<>]*)</select_mount>");

			twMounts.addItemMounts(path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount);
		}
	}

	if( m_array["Dat_tipo_emu"] == "scummvm" )
	{
		m_array["Svm_game"]            = AnalyzeTagName(texto, "<game>([^\"<>]*)</game>");
		m_array["Svm_game_label"]      = AnalyzeTagName(texto, "<game_label>([^\"<>]*)</game_label>");
		m_array["Svm_language"]        = AnalyzeTagName(texto, "<language>([^\"<>]*)</language>");
		m_array["Svm_subtitles"]       = AnalyzeTagName(texto, "<subtitles>([^\"<>]*)</subtitles>");
		m_array["Svm_platform"]        = AnalyzeTagName(texto, "<platform>([^\"<>]*)</platform>");
		m_array["Svm_gfx_mode"]        = AnalyzeTagName(texto, "<gfx_mode>([^\"<>]*)</gfx_mode>");
		m_array["Svm_render_mode"]     = AnalyzeTagName(texto, "<render_mode>([^\"<>]*)</render_mode>");
		m_array["Svm_fullscreen"]      = AnalyzeTagName(texto, "<fullscreen>([^\"<>]*)</fullscreen>");
		m_array["Svm_aspect_ratio"]    = AnalyzeTagName(texto, "<aspect_ratio>([^\"<>]*)</aspect_ratio>");
		m_array["Svm_path"]            = AnalyzeTagName(texto, "<path>([^\"<>]*)</path>");
		m_array["Svm_path_setup"]      = AnalyzeTagName(texto, "<path_setup>([^\"<>]*)</path_setup>");
		m_array["Svm_path_extra"]      = AnalyzeTagName(texto, "<path_extra>([^\"<>]*)</path_extra>");
		m_array["Svm_path_save"]       = AnalyzeTagName(texto, "<path_save>([^\"<>]*)</path_save>");
		m_array["Svm_path_capturas"]   = AnalyzeTagName(texto, "<path_capturas>([^\"<>]*)</path_capturas>");
		m_array["Svm_path_sonido"]     = AnalyzeTagName(texto, "<path_sonido>([^\"<>]*)</path_sonido>");
		m_array["Svm_music_driver"]    = AnalyzeTagName(texto, "<music_driver>([^\"<>]*)</music_driver>");
		m_array["Svm_enable_gs"]       = AnalyzeTagName(texto, "<enable_gs>([^\"<>]*)</enable_gs>");
		m_array["Svm_multi_midi"]      = AnalyzeTagName(texto, "<multi_midi>([^\"<>]*)</multi_midi>");
		m_array["Svm_native_mt32"]     = AnalyzeTagName(texto, "<native_mt32>([^\"<>]*)</native_mt32>");
		m_array["Svm_master_volume"]   = AnalyzeTagName(texto, "<master_volume>([^\"<>]*)</master_volume>");
		m_array["Svm_music_volume"]    = AnalyzeTagName(texto, "<music_volume>([^\"<>]*)</music_volume>");
		m_array["Svm_sfx_volume"]      = AnalyzeTagName(texto, "<sfx_volume>([^\"<>]*)</sfx_volume>");
		m_array["Svm_speech_volume"]   = AnalyzeTagName(texto, "<speech_volume>([^\"<>]*)</speech_volume>");
		m_array["Svm_tempo"]           = AnalyzeTagName(texto, "<tempo>([^\"<>]*)</tempo>");
		m_array["Svm_talkspeed"]       = AnalyzeTagName(texto, "<talkspeed>([^\"<>]*)</talkspeed>");
		m_array["Svm_debuglevel"]      = AnalyzeTagName(texto, "<debuglevel>([^\"<>]*)</debuglevel>");
		m_array["Svm_cdrom"]           = AnalyzeTagName(texto, "<cdrom>([^\"<>]*)</cdrom>");
		m_array["Svm_joystick_num"]    = AnalyzeTagName(texto, "<joystick_num>([^\"<>]*)</joystick_num>");
		m_array["Svm_output_rate"]     = AnalyzeTagName(texto, "<output_rate>([^\"<>]*)</output_rate>");
		m_array["Svm_midi_gain"]       = AnalyzeTagName(texto, "<midi_gain>([^\"<>]*)</midi_gain>");
		m_array["Svm_copy_protection"] = AnalyzeTagName(texto, "<copy_protection>([^\"<>]*)</copy_protection>");
		m_array["Svm_sound_font"]      = AnalyzeTagName(texto, "<sound_font>([^\"<>]*)</sound_font>");
	}

	return m_array;
}

// Funciones Extras -----------------------------
function AnalyzeTagName(texto, stRegExp, defecto)
{
	if ( texto != "" )
	{
		myRE = new RegExp( stRegExp );
		results = texto.match(myRE);
		if( results != -1 )
			return results[1];
		else
			return defecto;
	} else
		return defecto;
}
