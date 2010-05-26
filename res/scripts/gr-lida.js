/*
 * GR-lida importation script
 * http://sharesource.org/project/grlida/
 * http://www.gr-lida.org
 * 
 */
// Info -------------------------------
var authors		= "Monthy";						// Script authors
var title		= "GR-lida (ES)";
var url_site	= "http://www.gr-lida.org";		// Site address
var url_charset	= "UTF-8";
var language	= "es-ES";						// Site language
var version		= "0.1.0";						// Script version 19-04-2010
var requires	= "0.9.0";						// GR-lida version
var comments	= "";
var license		= "GPL v2";
var description	= "Script para obtener los datos del juego (titulo, genero, año, compañía, etc...) así como las caratulas. También da la posibilidad de bajar configuración ya establecida de distintos juegos para el DOSBox y el ScummVM.";

function UrlWebBusqueda(texto_busqueda)
{
	var m_url = new Array();

	m_url["metodo"] = "GET";	// POST, GET
	m_url["c_post"] = "";		// Contenido del POST si es post
	m_url["url"]    = url_site+"/grlidadb.php?ver=lista&titulo="+texto_busqueda;

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

	m_array["Dat_icono"]            = AnalyzeTagName(texto, "<Dat_icono>([^\"<>]*)</Dat_icono>", "datos");
	m_array["Dat_titulo"]           = AnalyzeTagName(texto, "<Dat_titulo>([^\"<>]*)</Dat_titulo>");
	m_array["Dat_subtitulo"]        = AnalyzeTagName(texto, "<Dat_subtitulo>([^\"<>]*)</Dat_subtitulo>");
	m_array["Dat_genero"]           = AnalyzeTagName(texto, "<Dat_genero>([^\"<>]*)</Dat_genero>");
	m_array["Dat_compania"]         = AnalyzeTagName(texto, "<Dat_compania>([^\"<>]*)</Dat_compania>");
	m_array["Dat_desarrollador"]    = AnalyzeTagName(texto, "<Dat_desarrollador>([^\"<>]*)</Dat_desarrollador>");
	m_array["Dat_tema"]             = AnalyzeTagName(texto, "<Dat_tema>([^\"<>]*)</Dat_tema>");
	m_array["Dat_perspectiva"]      = AnalyzeTagName(texto, "<Dat_perspectiva>([^\"<>]*)</Dat_perspectiva>");
	m_array["Dat_idioma"]           = AnalyzeTagName(texto, "<Dat_idioma>([^\"<>]*)</Dat_idioma>");
	m_array["Dat_formato"]          = AnalyzeTagName(texto, "<Dat_formato>([^\"<>]*)</Dat_formato>");
	m_array["Dat_anno"]             = AnalyzeTagName(texto, "<Dat_anno>([^\"<>]*)</Dat_anno>");
	m_array["Dat_numdisc"]          = AnalyzeTagName(texto, "<Dat_numdisc>([^\"<>]*)</Dat_numdisc>");
	m_array["Dat_sistemaop"]        = AnalyzeTagName(texto, "<Dat_sistemaop>([^\"<>]*)</Dat_sistemaop>");
	m_array["Dat_tamano"]           = AnalyzeTagName(texto, "<Dat_tamano>([^\"<>]*)</Dat_tamano>");
	m_array["Dat_graficos"]         = AnalyzeTagName(texto, "<Dat_graficos>([^\"<>]*)</Dat_graficos>");
	m_array["Dat_sonido"]           = AnalyzeTagName(texto, "<Dat_sonido>([^\"<>]*)</Dat_sonido>");
	m_array["Dat_jugabilidad"]      = AnalyzeTagName(texto, "<Dat_jugabilidad>([^\"<>]*)</Dat_jugabilidad>");
	m_array["Dat_original"]         = AnalyzeTagName(texto, "<Dat_original>([^\"<>]*)</Dat_original>");
	m_array["Dat_estado"]           = AnalyzeTagName(texto, "<Dat_estado>([^\"<>]*)</Dat_estado>");
	m_array["Dat_thumbs"]           = AnalyzeTagName(texto, "<Dat_thumbs>([^\"<>]*)</Dat_thumbs>");
	m_array["Dat_cover_front"]      = AnalyzeTagName(texto, "<Dat_cover_front>([^\"<>]*)</Dat_cover_front>");
	m_array["Dat_cover_back"]       = AnalyzeTagName(texto, "<Dat_cover_back>([^\"<>]*)</Dat_cover_back>");
	m_array["Dat_fecha"]            = AnalyzeTagName(texto, "<Dat_fecha>([^\"<>]*)</Dat_fecha>");
	m_array["Dat_tipo_emu"]         = AnalyzeTagName(texto, "<Dat_tipo_emu>([^\"<>]*)</Dat_tipo_emu>", "datos");
	m_array["Dat_comentario"]       = AnalyzeTagName(texto, "<Dat_comentario>([^[]*)</Dat_comentario>");
	m_array["Dat_favorito"]         = "";
	m_array["Dat_rating"]           = AnalyzeTagName(texto, "<Dat_rating>([0-9]+)</Dat_rating>","0");
	m_array["Dat_edad_recomendada"] = AnalyzeTagName(texto, "<Dat_edad_recomendada>([^\"<>]*)</Dat_edad_recomendada>","nd");
	m_array["Dat_usuario"]          = AnalyzeTagName(texto, "<Dat_usuario>([^\"<>]*)</Dat_usuario>");
	m_array["Dat_misc"]             = "";
	m_array["Dat_path_exe"]         = "";
	m_array["Dat_parametros_exe"]   = "";

// Imagenes Caratula --------
	if( m_array["Dat_thumbs"] == "" || m_array["Dat_thumbs"] == "null" )
	{
		m_array["Dat_thumbs"] = "";
		m_array["Dat_url_cover_thumbs"] = "";
	} else {
		if(local)
			m_array["Dat_url_cover_thumbs"] = m_array["Dat_thumbs"];
		else
			m_array["Dat_url_cover_thumbs"] = url_site+"/images/covers/small/"+m_array["Dat_thumbs"];
	}

	if( m_array["Dat_cover_front"] == "" || m_array["Dat_cover_front"] == "null" )
	{
		m_array["Dat_cover_front"] = "";
		m_array["Dat_url_cover_front"]  = "";
	} else {
		if(local)
			m_array["Dat_url_cover_front"] = m_array["Dat_cover_front"];
		else
			m_array["Dat_url_cover_front"]  = url_site+"/images/covers/large/"+m_array["Dat_cover_front"];
	}

	if( m_array["Dat_cover_back"] == "" || m_array["Dat_cover_back"] == "null" )
	{
		m_array["Dat_cover_back"] = "";
		m_array["Dat_url_cover_back"]   = "";	
	} else {
		if(local)
			m_array["Dat_url_cover_back"] = m_array["Dat_cover_back"];
		else	
			m_array["Dat_url_cover_back"]   = url_site+"/images/covers/large/"+m_array["Dat_cover_back"];
	}
// FIN Imagenes Caratula ----

	if( m_array["Dat_tipo_emu"] == "dosbox" )
	{
	// Datos del Configuración del DOSBox
		m_array["Dbx_sdl_fullscreen"]        = AnalyzeTagName(texto, "<Dbx_sdl_fullscreen>([^\"<>]*)</Dbx_sdl_fullscreen>", "");
		m_array["Dbx_sdl_fulldouble"]        = AnalyzeTagName(texto, "<Dbx_sdl_fulldouble>([^\"<>]*)</Dbx_sdl_fulldouble>", "");
		m_array["Dbx_sdl_fullfixed"]         = AnalyzeTagName(texto, "<Dbx_sdl_fullfixed>([^\"<>]*)</Dbx_sdl_fullfixed>", "");
		m_array["Dbx_sdl_fullresolution"]    = AnalyzeTagName(texto, "<Dbx_sdl_fullresolution>([^\"<>]*)</Dbx_sdl_fullresolution>", "");
		m_array["Dbx_sdl_windowresolution"]  = AnalyzeTagName(texto, "<Dbx_sdl_windowresolution>([^\"<>]*)</Dbx_sdl_windowresolution>", "");
		m_array["Dbx_sdl_output"]            = AnalyzeTagName(texto, "<Dbx_sdl_output>([^\"<>]*)</Dbx_sdl_output>", "");
		m_array["Dbx_sdl_hwscale"]           = AnalyzeTagName(texto, "<Dbx_sdl_hwscale>([^\"<>]*)</Dbx_sdl_hwscale>", "");
		m_array["Dbx_sdl_autolock"]          = AnalyzeTagName(texto, "<Dbx_sdl_autolock>([^\"<>]*)</Dbx_sdl_autolock>", "");
		m_array["Dbx_sdl_sensitivity"]       = AnalyzeTagName(texto, "<Dbx_sdl_sensitivity>([^\"<>]*)</Dbx_sdl_sensitivity>", "");
		m_array["Dbx_sdl_waitonerror"]       = AnalyzeTagName(texto, "<Dbx_sdl_waitonerror>([^\"<>]*)</Dbx_sdl_waitonerror>", "");
		m_array["Dbx_sdl_priority"]          = AnalyzeTagName(texto, "<Dbx_sdl_priority>([^\"<>]*)</Dbx_sdl_priority>", "");
		m_array["Dbx_sdl_mapperfile"]        = AnalyzeTagName(texto, "<Dbx_sdl_mapperfile>([^\"<>]*)</Dbx_sdl_mapperfile>", "");
		m_array["Dbx_sdl_usescancodes"]      = AnalyzeTagName(texto, "<Dbx_sdl_usescancodes>([^\"<>]*)</Dbx_sdl_usescancodes>", "");
		m_array["Dbx_dosbox_language"]       = AnalyzeTagName(texto, "<Dbx_dosbox_language>([^\"<>]*)</Dbx_dosbox_language>", "");
		m_array["Dbx_dosbox_machine"]        = AnalyzeTagName(texto, "<Dbx_dosbox_machine>([^\"<>]*)</Dbx_dosbox_machine>", "");
		m_array["Dbx_dosbox_captures"]       = AnalyzeTagName(texto, "<Dbx_dosbox_captures>([^\"<>]*)</Dbx_dosbox_captures>", "");
		m_array["Dbx_dosbox_memsize"]        = AnalyzeTagName(texto, "<Dbx_dosbox_memsize>([^\"<>]*)</Dbx_dosbox_memsize>", "");
		m_array["Dbx_render_frameskip"]      = AnalyzeTagName(texto, "<Dbx_render_frameskip>([^\"<>]*)</Dbx_render_frameskip>", "");
		m_array["Dbx_render_aspect"]         = AnalyzeTagName(texto, "<Dbx_render_aspect>([^\"<>]*)</Dbx_render_aspect>", "");
		m_array["Dbx_render_scaler"]         = AnalyzeTagName(texto, "<Dbx_render_scaler>([^\"<>]*)</Dbx_render_scaler>", "");
		m_array["Dbx_cpu_core"]              = AnalyzeTagName(texto, "<Dbx_cpu_core>([^\"<>]*)</Dbx_cpu_core>", "");
		m_array["Dbx_cpu_cputype"]           = AnalyzeTagName(texto, "<Dbx_cpu_cputype>([^\"<>]*)</Dbx_cpu_cputype>", "");
		m_array["Dbx_cpu_cycles"]            = AnalyzeTagName(texto, "<Dbx_cpu_cycles>([^\"<>]*)</Dbx_cpu_cycles>", "");
		m_array["Dbx_cpu_cycles_realmode"]   = AnalyzeTagName(texto, "<Dbx_cpu_cycles_realmode>([^\"<>]*)</Dbx_cpu_cycles_realmode>", "");
		m_array["Dbx_cpu_cycles_protmode"]   = AnalyzeTagName(texto, "<Dbx_cpu_cycles_protmode>([^\"<>]*)</Dbx_cpu_cycles_protmode>", "");
		m_array["Dbx_cpu_cycles_limitmode"]  = AnalyzeTagName(texto, "<Dbx_cpu_cycles_limitmode>([^\"<>]*)</Dbx_cpu_cycles_limitmode>", "");
		m_array["Dbx_cpu_cycleup"]           = AnalyzeTagName(texto, "<Dbx_cpu_cycleup>([^\"<>]*)</Dbx_cpu_cycleup>", "");
		m_array["Dbx_cpu_cycledown"]         = AnalyzeTagName(texto, "<Dbx_cpu_cycledown>([^\"<>]*)</Dbx_cpu_cycledown>", "");
		m_array["Dbx_mixer_nosound"]         = AnalyzeTagName(texto, "<Dbx_mixer_nosound>([^\"<>]*)</Dbx_mixer_nosound>", "");
		m_array["Dbx_mixer_rate"]            = AnalyzeTagName(texto, "<Dbx_mixer_rate>([^\"<>]*)</Dbx_mixer_rate>", "");
		m_array["Dbx_mixer_blocksize"]       = AnalyzeTagName(texto, "<Dbx_mixer_blocksize>([^\"<>]*)</Dbx_mixer_blocksize>", "");
		m_array["Dbx_mixer_prebuffer"]       = AnalyzeTagName(texto, "<Dbx_mixer_prebuffer>([^\"<>]*)</Dbx_mixer_prebuffer>", "");
		m_array["Dbx_midi_mpu401"]           = AnalyzeTagName(texto, "<Dbx_midi_mpu401>([^\"<>]*)</Dbx_midi_mpu401>", "");
		m_array["Dbx_midi_intelligent"]      = AnalyzeTagName(texto, "<Dbx_midi_intelligent>([^\"<>]*)</Dbx_midi_intelligent>", "");
		m_array["Dbx_midi_device"]           = AnalyzeTagName(texto, "<Dbx_midi_device>([^\"<>]*)</Dbx_midi_device>", "");
		m_array["Dbx_midi_config"]           = AnalyzeTagName(texto, "<Dbx_midi_config>([^\"<>]*)</Dbx_midi_config>", "");
		m_array["Dbx_midi_mt32rate"]         = AnalyzeTagName(texto, "<Dbx_midi_mt32rate>([^\"<>]*)</Dbx_midi_mt32rate>", "");
		m_array["Dbx_sblaster_sbtype"]       = AnalyzeTagName(texto, "<Dbx_sblaster_sbtype>([^\"<>]*)</Dbx_sblaster_sbtype>", "");
		m_array["Dbx_sblaster_sbbase"]       = AnalyzeTagName(texto, "<Dbx_sblaster_sbbase>([^\"<>]*)</Dbx_sblaster_sbbase>", "");
		m_array["Dbx_sblaster_irq"]          = AnalyzeTagName(texto, "<Dbx_sblaster_irq>([^\"<>]*)</Dbx_sblaster_irq>", "");
		m_array["Dbx_sblaster_dma"]          = AnalyzeTagName(texto, "<Dbx_sblaster_dma>([^\"<>]*)</Dbx_sblaster_dma>", "");
		m_array["Dbx_sblaster_hdma"]         = AnalyzeTagName(texto, "<Dbx_sblaster_hdma>([^\"<>]*)</Dbx_sblaster_hdma>", "");
		m_array["Dbx_sblaster_mixer"]        = AnalyzeTagName(texto, "<Dbx_sblaster_mixer>([^\"<>]*)</Dbx_sblaster_mixer>", "");
		m_array["Dbx_sblaster_oplmode"]      = AnalyzeTagName(texto, "<Dbx_sblaster_oplmode>([^\"<>]*)</Dbx_sblaster_oplmode>", "");
		m_array["Dbx_sblaster_oplemu"]       = AnalyzeTagName(texto, "<Dbx_sblaster_oplemu>([^\"<>]*)</Dbx_sblaster_oplemu>", "");
		m_array["Dbx_sblaster_oplrate"]      = AnalyzeTagName(texto, "<Dbx_sblaster_oplrate>([^\"<>]*)</Dbx_sblaster_oplrate>", "");
		m_array["Dbx_gus_gus"]               = AnalyzeTagName(texto, "<Dbx_gus_gus>([^\"<>]*)</Dbx_gus_gus>", "");
		m_array["Dbx_gus_gusrate"]           = AnalyzeTagName(texto, "<Dbx_gus_gusrate>([^\"<>]*)</Dbx_gus_gusrate>", "");
		m_array["Dbx_gus_gusbase"]           = AnalyzeTagName(texto, "<Dbx_gus_gusbase>([^\"<>]*)</Dbx_gus_gusbase>", "");
		m_array["Dbx_gus_irq1"]              = AnalyzeTagName(texto, "<Dbx_gus_irq1>([^\"<>]*)</Dbx_gus_irq1>", "");
		m_array["Dbx_gus_irq2"]              = AnalyzeTagName(texto, "<Dbx_gus_irq2>([^\"<>]*)</Dbx_gus_irq2>", "");
		m_array["Dbx_gus_dma1"]              = AnalyzeTagName(texto, "<Dbx_gus_dma1>([^\"<>]*)</Dbx_gus_dma1>", "");
		m_array["Dbx_gus_dma2"]              = AnalyzeTagName(texto, "<Dbx_gus_dma2>([^\"<>]*)</Dbx_gus_dma2>", "");
		m_array["Dbx_gus_ultradir"]          = AnalyzeTagName(texto, "<Dbx_gus_ultradir>([^\"<>]*)</Dbx_gus_ultradir>", "");
		m_array["Dbx_speaker_pcspeaker"]     = AnalyzeTagName(texto, "<Dbx_speaker_pcspeaker>([^\"<>]*)</Dbx_speaker_pcspeaker>", "");
		m_array["Dbx_speaker_pcrate"]        = AnalyzeTagName(texto, "<Dbx_speaker_pcrate>([^\"<>]*)</Dbx_speaker_pcrate>", "");
		m_array["Dbx_speaker_tandy"]         = AnalyzeTagName(texto, "<Dbx_speaker_tandy>([^\"<>]*)</Dbx_speaker_tandy>", "");
		m_array["Dbx_speaker_tandyrate"]     = AnalyzeTagName(texto, "<Dbx_speaker_tandyrate>([^\"<>]*)</Dbx_speaker_tandyrate>", "");
		m_array["Dbx_speaker_disney"]        = AnalyzeTagName(texto, "<Dbx_speaker_disney>([^\"<>]*)</Dbx_speaker_disney>", "");
		m_array["Dbx_joystick_type"]         = AnalyzeTagName(texto, "<Dbx_joystick_type>([^\"<>]*)</Dbx_joystick_type>", "");
		m_array["Dbx_joystick_timed"]        = AnalyzeTagName(texto, "<Dbx_joystick_timed>([^\"<>]*)</Dbx_joystick_timed>", "");
		m_array["Dbx_joystick_autofire"]     = AnalyzeTagName(texto, "<Dbx_joystick_autofire>([^\"<>]*)</Dbx_joystick_autofire>", "");
		m_array["Dbx_joystick_swap34"]       = AnalyzeTagName(texto, "<Dbx_joystick_swap34>([^\"<>]*)</Dbx_joystick_swap34>", "");
		m_array["Dbx_joystick_buttonwrap"]   = AnalyzeTagName(texto, "<Dbx_joystick_buttonwrap>([^\"<>]*)</Dbx_joystick_buttonwrap>", "");
		m_array["Dbx_modem_modem"]           = AnalyzeTagName(texto, "<Dbx_modem_modem>([^\"<>]*)</Dbx_modem_modem>", "");
		m_array["Dbx_modem_comport"]         = AnalyzeTagName(texto, "<Dbx_modem_comport>([^\"<>]*)</Dbx_modem_comport>", "");
		m_array["Dbx_modem_listenport"]      = AnalyzeTagName(texto, "<Dbx_modem_listenport>([^\"<>]*)</Dbx_modem_listenport>", "");
		m_array["Dbx_dserial_directserial"]  = AnalyzeTagName(texto, "<Dbx_dserial_directserial>([^\"<>]*)</Dbx_dserial_directserial>", "");
		m_array["Dbx_dserial_comport"]       = AnalyzeTagName(texto, "<Dbx_dserial_comport>([^\"<>]*)</Dbx_dserial_comport>", "");
		m_array["Dbx_dserial_realport"]      = AnalyzeTagName(texto, "<Dbx_dserial_realport>([^\"<>]*)</Dbx_dserial_realport>", "");
		m_array["Dbx_dserial_defaultbps"]    = AnalyzeTagName(texto, "<Dbx_dserial_defaultbps>([^\"<>]*)</Dbx_dserial_defaultbps>", "");
		m_array["Dbx_dserial_parity"]        = AnalyzeTagName(texto, "<Dbx_dserial_parity>([^\"<>]*)</Dbx_dserial_parity>", "");
		m_array["Dbx_dserial_bytesize"]      = AnalyzeTagName(texto, "<Dbx_dserial_bytesize>([^\"<>]*)</Dbx_dserial_bytesize>", "");
		m_array["Dbx_dserial_stopbit"]       = AnalyzeTagName(texto, "<Dbx_dserial_stopbit>([^\"<>]*)</Dbx_dserial_stopbit>", "");
		m_array["Dbx_serial_1"]              = AnalyzeTagName(texto, "<Dbx_serial_1>([^\"<>]*)</Dbx_serial_1>", "");
		m_array["Dbx_serial_2"]              = AnalyzeTagName(texto, "<Dbx_serial_2>([^\"<>]*)</Dbx_serial_2>", "");
		m_array["Dbx_serial_3"]              = AnalyzeTagName(texto, "<Dbx_serial_3>([^\"<>]*)</Dbx_serial_3>", "");
		m_array["Dbx_serial_4"]              = AnalyzeTagName(texto, "<Dbx_serial_4>([^\"<>]*)</Dbx_serial_4>", "");
		m_array["Dbx_dos_xms"]               = AnalyzeTagName(texto, "<Dbx_dos_xms>([^\"<>]*)</Dbx_dos_xms>", "");
		m_array["Dbx_dos_ems"]               = AnalyzeTagName(texto, "<Dbx_dos_ems>([^\"<>]*)</Dbx_dos_ems>", "");
		m_array["Dbx_dos_umb"]               = AnalyzeTagName(texto, "<Dbx_dos_umb>([^\"<>]*)</Dbx_dos_umb>", "");
		m_array["Dbx_dos_keyboardlayout"]    = AnalyzeTagName(texto, "<Dbx_dos_keyboardlayout>([^\"<>]*)</Dbx_dos_keyboardlayout>", "");
		m_array["Dbx_ipx_ipx"]               = AnalyzeTagName(texto, "<Dbx_ipx_ipx>([^\"<>]*)</Dbx_ipx_ipx>", "");
		m_array["Dbx_autoexec"]              = AnalyzeTagName(texto, "<Dbx_autoexec>([^\"<>]*)</Dbx_autoexec>", "");
		m_array["Dbx_opt_autoexec"]          = AnalyzeTagName(texto, "<Dbx_opt_autoexec>([^\"<>]*)</Dbx_opt_autoexec>", "");
		m_array["Dbx_opt_loadfix"]           = AnalyzeTagName(texto, "<Dbx_opt_loadfix>([^\"<>]*)</Dbx_opt_loadfix>", "");
		m_array["Dbx_opt_loadfix_mem"]       = AnalyzeTagName(texto, "<Dbx_opt_loadfix_mem>([^\"<>]*)</Dbx_opt_loadfix_mem>", "");
		m_array["Dbx_opt_consola_dbox"]      = AnalyzeTagName(texto, "<Dbx_opt_consola_dbox>([^\"<>]*)</Dbx_opt_consola_dbox>", "");
		m_array["Dbx_opt_cerrar_dbox"]       = AnalyzeTagName(texto, "<Dbx_opt_cerrar_dbox>([^\"<>]*)</Dbx_opt_cerrar_dbox>", "");
		m_array["Dbx_opt_cycle_sincronizar"] = AnalyzeTagName(texto, "<Dbx_opt_cycle_sincronizar>([^\"<>]*)</Dbx_opt_cycle_sincronizar>", "");
		m_array["Dbx_path_conf"]             = AnalyzeTagName(texto, "<Dbx_path_conf>([^<]*)</Dbx_path_conf>", "");
		m_array["Dbx_path_sonido"]           = AnalyzeTagName(texto, "<Dbx_path_sonido>([^<]*)</Dbx_path_sonido>", "");
		m_array["Dbx_path_exe"]              = AnalyzeTagName(texto, "<Dbx_path_exe>([^<]*)</Dbx_path_exe>", "");
		m_array["Dbx_path_setup"]            = AnalyzeTagName(texto, "<Dbx_path_setup>([^<]*)</Dbx_path_setup>", "");
		m_array["Dbx_parametros_exe"]        = AnalyzeTagName(texto, "<Dbx_parametros_exe>([^<>]*)</Dbx_parametros_exe>", "");
		m_array["Dbx_parametros_setup"]      = AnalyzeTagName(texto, "<Dbx_parametros_setup>([^<>]*)</Dbx_parametros_setup>", "");

	// Añadimos los distintos Montajes
		myDbxRE = new RegExp("<dosbox_montajes>([^\"]*)</dosbox_montajes>","g");
		resultsDbx = texto.match(myDbxRE);
		for(var i_dbx = 0; i_dbx < resultsDbx.length; i_dbx++)
		{
			path         = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_path>([^<]*)</Dbx_mount_path>");
			label        = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_label>([^\"<>]*)</Dbx_mount_label>");
			tipo_as      = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_tipo_as>([^\"<>]*)</Dbx_mount_tipo_as>");
			letter       = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_letter>([^\"<>]*)</Dbx_mount_letter>");
			indx_cd      = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_indx_cd>([^\"<>]*)</Dbx_mount_indx_cd>");
			opt_mount    = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_opt_mount>([^\"<>]*)</Dbx_mount_opt_mount>");
			io_ctrl      = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_io_ctrl>([^\"<>]*)</Dbx_mount_io_ctrl>");
			select_mount = AnalyzeTagName(resultsDbx[i_dbx], "<Dbx_mount_select_mount>([^\"<>]*)</Dbx_mount_select_mount>");

			twMounts.addItemMounts(path, label, tipo_as, letter, indx_cd, opt_mount, io_ctrl, select_mount);
		}
	}

	if( m_array["Dat_tipo_emu"] == "scummvm" )
	{
		m_array["Svm_game"]            = AnalyzeTagName(texto, "<Svm_game>([^\"<>]*)</Svm_game>");
		m_array["Svm_game_label"]      = AnalyzeTagName(texto, "<Svm_game_label>([^\"<>]*)</Svm_game_label>");
		m_array["Svm_language"]        = AnalyzeTagName(texto, "<Svm_language>([^\"<>]*)</Svm_language>");
		m_array["Svm_subtitles"]       = AnalyzeTagName(texto, "<Svm_subtitles>([^\"<>]*)</Svm_subtitles>");
		m_array["Svm_platform"]        = AnalyzeTagName(texto, "<Svm_platform>([^\"<>]*)</Svm_platform>");
		m_array["Svm_gfx_mode"]        = AnalyzeTagName(texto, "<Svm_gfx_mode>([^\"<>]*)</Svm_gfx_mode>");
		m_array["Svm_render_mode"]     = AnalyzeTagName(texto, "<Svm_render_mode>([^\"<>]*)</Svm_render_mode>");
		m_array["Svm_fullscreen"]      = AnalyzeTagName(texto, "<Svm_fullscreen>([^\"<>]*)</Svm_fullscreen>");
		m_array["Svm_aspect_ratio"]    = AnalyzeTagName(texto, "<Svm_aspect_ratio>([^\"<>]*)</Svm_aspect_ratio>");
		m_array["Svm_path"]            = AnalyzeTagName(texto, "<Svm_path>([^<]*)</Svm_path>");
		m_array["Svm_path_setup"]      = AnalyzeTagName(texto, "<Svm_path_setup>([^<]*)</Svm_path_setup>");
		m_array["Svm_path_extra"]      = AnalyzeTagName(texto, "<Svm_path_extra>([^<]*)</Svm_path_extra>");
		m_array["Svm_path_save"]       = AnalyzeTagName(texto, "<Svm_path_save>([^<]*)</Svm_path_save>");
		m_array["Svm_path_capturas"]   = AnalyzeTagName(texto, "<Svm_path_capturas>([^<]*)</Svm_path_capturas>");
		m_array["Svm_path_sonido"]     = AnalyzeTagName(texto, "<Svm_path_sonido>([^<]*)</Svm_path_sonido>");
		m_array["Svm_music_driver"]    = AnalyzeTagName(texto, "<Svm_music_driver>([^\"<>]*)</Svm_music_driver>");
		m_array["Svm_enable_gs"]       = AnalyzeTagName(texto, "<Svm_enable_gs>([^\"<>]*)</Svm_enable_gs>");
		m_array["Svm_multi_midi"]      = AnalyzeTagName(texto, "<Svm_multi_midi>([^\"<>]*)</Svm_multi_midi>");
		m_array["Svm_native_mt32"]     = AnalyzeTagName(texto, "<Svm_native_mt32>([^\"<>]*)</Svm_native_mt32>");
		m_array["Svm_master_volume"]   = AnalyzeTagName(texto, "<Svm_master_volume>([^\"<>]*)</Svm_master_volume>");
		m_array["Svm_music_volume"]    = AnalyzeTagName(texto, "<Svm_music_volume>([^\"<>]*)</Svm_music_volume>");
		m_array["Svm_sfx_volume"]      = AnalyzeTagName(texto, "<Svm_sfx_volume>([^\"<>]*)</Svm_sfx_volume>");
		m_array["Svm_speech_volume"]   = AnalyzeTagName(texto, "<Svm_speech_volume>([^\"<>]*)</Svm_speech_volume>");
		m_array["Svm_tempo"]           = AnalyzeTagName(texto, "<Svm_tempo>([^\"<>]*)</Svm_tempo>");
		m_array["Svm_talkspeed"]       = AnalyzeTagName(texto, "<Svm_talkspeed>([^\"<>]*)</Svm_talkspeed>");
		m_array["Svm_debuglevel"]      = AnalyzeTagName(texto, "<Svm_debuglevel>([^\"<>]*)</Svm_debuglevel>");
		m_array["Svm_cdrom"]           = AnalyzeTagName(texto, "<Svm_cdrom>([^\"<>]*)</Svm_cdrom>");
		m_array["Svm_joystick_num"]    = AnalyzeTagName(texto, "<Svm_joystick_num>([^\"<>]*)</Svm_joystick_num>");
		m_array["Svm_output_rate"]     = AnalyzeTagName(texto, "<Svm_output_rate>([^\"<>]*)</Svm_output_rate>");
		m_array["Svm_midi_gain"]       = AnalyzeTagName(texto, "<Svm_midi_gain>([^\"<>]*)</Svm_midi_gain>");
		m_array["Svm_copy_protection"] = AnalyzeTagName(texto, "<Svm_copy_protection>([^\"<>]*)</Svm_copy_protection>");
		m_array["Svm_sound_font"]      = AnalyzeTagName(texto, "<Svm_sound_font>([^\"<>]*)</Svm_sound_font>");
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
