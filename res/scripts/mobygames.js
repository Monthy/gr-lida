/*
 * GR-lida importation script
 * http://sharesource.org/project/grlida/
 * http://www.gr-lida.org
 * 
 */
// Info -------------------------------
var authors		= "Monthy";						// Script authors
var title		= "MobyGames (EN)";
var url_site	= "https://www.mobygames.com";	// Site address
var url_img_site= "https://www.mobygames.com";	// Site pics address
var url_charset	= "UTF-8";
var language	= "en-EN";						// Site language
var version		= "0.4.4";						// Script version 17-11-2008 update 25-09-2018
var requires	= "0.11.0";						// GR-lida version
var comments	= "";
var license		= "GPL v2";
var description	= "Script para obtener los datos del juego (titulo, genero, año, compañía, etc...) así como la caratula principal del juego.";

function UrlWebBusqueda(texto_busqueda)
{
	var m_url = new Array();

	m_url["metodo"] = "GET";	// POST, GET
	m_url["c_post"] = "";		// Contenido del POST si es post
	m_url["url"]    = url_site +"/search/quick?q="+ texto_busqueda.replace(" ","+") +"&p=-1&search=Go&sFilter=1&sG=on&sGG=on&sA=on&sD=on&sC=on";

	return m_url;
}

function AnalyzeFindPage(texto)
{
// Expresión Regular para obtener los datos de la búsqueda.
	var textoFind, m_titulo, m_contenido, m_url_game;
	textoFind = texto.replace(/<\/div><\/div><br clear="all"><\/div>/gi, '<fin_searchDetails>\n');// final a
	textoFind = textoFind.replace(/&nbsp;/gi, ' ');

/* Busqueda CON imagenes
--------------------------------------------------------*/
	myReFind = new RegExp('<div class="([0-9a-z]+)"><div class="searchResult"><div class="searchNumber">([0-9]+).</div><div class="searchImage"><a href="([^"]*)"><img class="searchResultImage" alt="([^"]*)" border="([^"]*)" src="([^"]*)" height="([^"]*)" width="([^"]*)" ></a></div><div class="searchData"><div class="searchTitle">Game: <a href="([^"]*)">([^<]*)</a>([^"]*)</div><div class="searchDetails">([^\n[]*)</div><fin_searchDetails>\n', 'g');
	resultsFind = textoFind.match(myReFind);
	if (resultsFind != null)
	{
		for (var i = 0; i < resultsFind.length; i++)
		{
			rx_Title = new RegExp('<div class="([0-9a-z]+)"><div class="searchResult"><div class="searchNumber">([0-9]+).</div><div class="searchImage"><a href="([^"]*)"><img class="searchResultImage" alt="([^"]*)" border="([^"]*)" src="([^"]*)" height="([^"]*)" width="([^"]*)" ></a></div><div class="searchData"><div class="searchTitle">Game: <a href="([^"]*)">([^<]*)</a>([^"]*)</div><div class="searchDetails">([^\n[]*)</div><fin_searchDetails>\n');
			results_game_Title = resultsFind[i].match(rx_Title);

			m_titulo    = results_game_Title[10];
			m_contenido = results_game_Title[12].replace(' and ','; ').replace('</div>','');
			m_url_game  = results_game_Title[9];

			twListaBusqueda.addItemFind(m_titulo, '-', '-', m_url_game, 'datos');
			AnalyzeOtherGame(m_titulo, m_contenido);
		}
	}

/* Busqueda SIN imagenes
--------------------------------------------------------*/
	myReFind = new RegExp('<div class="([0-9a-z]+)"><div class="searchResult"><div class="searchNumber">([0-9]+).</div><div class="searchData"><div class="searchTitle">Game: <a href="([^"]*)">([^<]*)</a>([^"]*)</div><div class="searchDetails">([^\n[]*)</div><fin_searchDetails>\n', 'g');
	resultsFind = textoFind.match(myReFind);
	if (resultsFind != null)
	{
		for (var i = 0; i < resultsFind.length; i++)
		{
			rx_Title = new RegExp('<div class="([0-9a-z]+)"><div class="searchResult"><div class="searchNumber">([0-9]+).</div><div class="searchData"><div class="searchTitle">Game: <a href="([^"]*)">([^<]*)</a>([^"]*)</div><div class="searchDetails">([^\n[]*)</div><fin_searchDetails>\n');
			results_game_Title = resultsFind[i].match(rx_Title);

			m_titulo    = results_game_Title[4];
			m_contenido = results_game_Title[6].replace(' and ','; ').replace('</div>','');
			m_url_game  = results_game_Title[3];

			twListaBusqueda.addItemFind(m_titulo, '-', '-', m_url_game, 'datos');
			AnalyzeOtherGame(m_titulo, m_contenido);
		}
	}
}

function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();
// INI Temp ---------------
	m_array["Dat_grupo"]            = '';
	m_array["Dat_idioma"]           = '';
	m_array["Dat_idioma_voces"]     = '';
	m_array["Dat_formato"]          = '';
	m_array["Dat_numdisc"]          = '';
	m_array["Dat_tamano"]           = '';
	m_array["Dat_graficos"]         = '0';
	m_array["Dat_sonido"]           = '0';
	m_array["Dat_jugabilidad"]      = '0';
	m_array["Dat_original"]         = 'false';
	m_array["Dat_estado"]           = '';
	m_array["Dat_thumbs"]           = '';
	m_array["Dat_thumbs_new"]       = 'false';
	m_array["Dat_cover_front"]      = '';
	m_array["Dat_cover_front_new"]  = 'false';
	m_array["Dat_cover_back"]       = '';
	m_array["Dat_cover_back_new"]   = 'false';
	m_array["Dat_fecha"]            = '';
	m_array["Dat_tipo_emu"]         = 'datos'; // Importante = datos
	m_array["Dat_favorito"]         = 'false';
	m_array["Dat_path_exe"]         = '';
	m_array["Dat_parametros_exe"]   = '';
	m_array["Dat_path_capturas"]    = '';
	m_array["Dat_path_setup"]       = '';
	m_array["Dat_parametros_setup"] = '';
// FIN Temp ---------------

// Icono ------------------
	m_array['Dat_icono'] = 'datos';
// Titulo -----------------
	if (texto.indexOf('niceHeaderTitle',0) != -1)
	{
		m_array['Dat_titulo'] = AnalyzeTag(texto, '<h1 class="niceHeaderTitle"><a href="([^"]*)">([^\n[]*)</a> <small>', 2, '');
		if (m_array['Dat_titulo'] == '')
			m_array['Dat_titulo'] = AnalyzeTag(texto, '<h1 class="niceHeaderTitle"><a href="([^"]*)">([^<]*)</a>', 2, '');
	} else
		m_array['Dat_titulo'] = '';

	m_array['Dat_subtitulo'] = '';

// Developed, Published ---
	if (texto.indexOf('Developed by</div>') != -1)
	{
		m_array['Dat_compania'] = AnalyzeCategoriasMobyGames(texto, 'Published by</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Developed by',
									'/company/([^/]+)">([^<]*)</a>', 2);
		m_array['Dat_desarrollador'] = AnalyzeCategoriasMobyGames(texto, 'Developed by</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Released',
									'/company/([^/]+)">([^<]*)</a>', 2);
	} else {
		if (texto.indexOf('Published by</div>') != -1)
		{
			m_array['Dat_compania'] = AnalyzeCategoriasMobyGames(texto, 'Published by</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Released',
										'/company/([^/]+)">([^<]*)</a>', 2);
		} else {
			m_array['Dat_compania'] = '';
		}
		m_array['Dat_desarrollador'] = '';
	}

// Released ---------------
	m_array['Dat_anno'] = AnalyzeCategoriasMobyGames(texto, 'Released</div><div [^>]*>([^?]*)</div>', '/game/([^"]+)/release-info">([^<]*)</a>', 2);

// Platforms, Also For ----
	if (texto.indexOf('Platforms</div>') != -1)
	{
		m_array['Dat_sistemaop'] = AnalyzeCategoriasMobyGames(texto, 'Platforms</div><div [^>]*>([^?]*)</div></div></td>',
									'/game/([^">]*)">([^<]*)</a>', 2);
	}
	else if (texto.indexOf('Platform</div>') != -1)
	{
		m_array['Dat_sistemaop'] = AnalyzeCategoriasMobyGames(texto, 'Platform</div><div [^>]*>([^?]*)</div></div></td>\n    <td width="48%"><div id="coreGameGenre">',
									'<a href="([^">]*)">([^<]*)</a>', 2);
	}
	else if (texto.indexOf('Also For</div>') != -1)
	{
		m_array['Dat_sistemaop'] = AnalyzeCategoriasMobyGames(texto, 'Also For</div><div [^>]*>([^?]*)</div></div></td>',
									'/game/([^">]*)">([^<]*)</a>', 2);
	} else {
		m_array['Dat_sistemaop'] = '';
	}

// Genre, Perspective -----
	if (texto.indexOf('Perspective</div>') != -1)
	{
		m_array['Dat_genero'] = AnalyzeCategoriasMobyGames(texto, 'Genre</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Perspective',
									'/genre/([^">]*)">([^"<>]*)</a>', 2);
		if (texto.indexOf('Non-Sport</div>') != -1)
		{
			m_array['Dat_perspectiva'] = AnalyzeCategoriasMobyGames(texto, 'Perspective</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Non-Sport',
									'/genre/([^"<>]*)">([^<]*)</a>', 2);
		} else {
			if (texto.indexOf('Misc</div>') != -1)
			{
				m_array['Dat_perspectiva'] = AnalyzeCategoriasMobyGames(texto, 'Perspective</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Misc',
									'/genre/([^"<>]*)">([^<]*)</a>', 2);
			} else {
				m_array['Dat_perspectiva'] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div>",
									'/genre/([^">]*)">([^<]*)</a>', 2);
			}
		}
	} else {
		if (texto.indexOf('Non-Sport</div>') != -1)
		{
			m_array['Dat_genero'] = AnalyzeCategoriasMobyGames(texto, 'Genre</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Non-Sport',
										'/genre/([^"<>]*)">([^"<>]*)</a>', 2);
		} else {
			m_array['Dat_genero'] = AnalyzeCategoriasMobyGames(texto, "Genre</div><div [^>]*>([^?]*)</div><div style=",
										'/genre/([^"<>]*)">([^"<>]*)</a>', 2);
		}
		m_array['Dat_perspectiva'] = '';
	}

// Theme, Misc, Non-Sport -
	if (texto.indexOf('Misc</div>') != -1)
	{
		if (texto.indexOf('Theme</div>') != -1)
		{
			m_array['Dat_tema'] = AnalyzeCategoriasMobyGames(texto, 'Theme</div><div [^>]*>([^?]*)</div><div style="([^?]*)">Misc',
									'/genre/([^"<>]*)">([^"<>]*)</a>', 2);
		} else {
			m_array['Dat_tema'] = '';
		}
		m_array['Dat_misc'] = AnalyzeCategoriasMobyGames(texto, 'Misc</div><div [^>]*>([^?]*)</div></div></div></td>',
									'/genre/([^"<>]*)">([^"<>]*)</a>', 2);
	} else {
		if (texto.indexOf('Theme</div>') != -1)
		{
			m_array['Dat_tema'] = AnalyzeCategoriasMobyGames(texto, 'Theme</div><div [^>]*>([^?]*)</div></div></div></td>',
									'/genre/([^"<>]*)">([^"<>]*)</a>', 2);
		} else {
			m_array['Dat_tema'] = '';
		}
		m_array['Dat_misc'] = '';
	}

// Description ------------
	m_array['Dat_comentario'] = AnalyzeTag(texto, 'Description</h2>([^[]*)<div class="sideBarLinks">', 1, '');

// MobyScore --------------
// <div class="fr scoreBoxMed scoreHi">4.0</div>
// <div class="fr scoreBoxMed scoreMed">3.1</div>
// <div class="fr scoreBoxMed scoreNone">...</div>
	m_array['Dat_rating'] = Math.round(AnalyzeTag(texto, '<div class="fr scoreBoxMed([^"<>]*)">([^"<>]*)</div>', 2, '0'));

// aDeSe Rating -----------
	var m_edad_recomendada = '';
	var temp_esrb = (texto.indexOf('Genre</div>') != -1) ? "Genre" : "Misc";
	if (texto.indexOf('aDeSe Rating</div>') != -1)
	{
		m_edad_recomendada = AnalyzeCategoriasMobyGames(texto, 'aDeSe Rating</div><div [^>]*>([^?]*)</div><div style="([^?]*)">'+ temp_esrb,
										'/attribute/([^"<>]*)">([^"<>]*)</a>', 2).toLowerCase();
	}
	else if (texto.indexOf('ESRB Rating</div>') != -1)
	{
		m_edad_recomendada = AnalyzeCategoriasMobyGames(texto, 'ESRB Rating</div><div [^>]*>([^?]*)</div><div style="([^?]*)">'+ temp_esrb,
										'/attribute/([^">]*)/">([^<]*)</a>', 2).toLowerCase();
	}

	switch (m_edad_recomendada)
	{
		case 'early childhood': m_array['Dat_edad_recomendada'] = 'tp'; break;
		case 'everyone': m_array['Dat_edad_recomendada'] = '3'; break;
		case 'everyone 10+': m_array['Dat_edad_recomendada'] = '7'; break;
		case 'teen': m_array['Dat_edad_recomendada'] = '12'; break;
		case 'mature': m_array['Dat_edad_recomendada'] = '16'; break;
		case 'adults only': m_array['Dat_edad_recomendada'] = '18'; break;
		default: m_array['Dat_edad_recomendada'] = 'nd'; break;
	}

// Usuario ----------------
	if (texto.indexOf('c by', 0) != -1)
		m_array['Dat_usuario'] = AnalyzeCategoriasMobyGames(texto, 'contributed by([^[]*)</div>', '<a href="([^">]*)">([^</]*)</a>', 2);
	else
		m_array['Dat_usuario'] = '';

// Imagenes Caratula ------
	if (texto.indexOf('/images/covers/s/', 0) != -1)
	{
		var url_covers = '';
		myRE = new RegExp('/game/([^"]+)/([^"]+)/cover-art/gameCoverId,([0-9]+)/"><img alt="([^<]*)" border="0" src="/images/covers/s/([^"]+)"');
		img_results = texto.match(myRE);
		if (img_results != null)
		{
			m_array['Dat_thumbs']      = img_results[5];
			m_array['Dat_cover_front'] = img_results[5];
			url_covers = img_results[1] +'/'+ img_results[2];
		} else {
			myRE = new RegExp('/game/([^"]+)/cover-art/gameCoverId,([0-9]+)/"><img alt="([^<]*)" border="0" src="/images/covers/s/([^"]+)"');
			img_results = texto.match(myRE);
			if (img_results != null)
			{
				m_array['Dat_thumbs']      = img_results[4];
				m_array['Dat_cover_front'] = img_results[4];
				url_covers = img_results[1];
			} else {
				m_array['Dat_thumbs']      = '';
				m_array['Dat_cover_front'] = '';
			}
		}

		m_array["Dat_cover_back"]       = '';
		m_array["Dat_url_cover_thumbs"] = url_img_site +'/images/covers/s/'+ m_array['Dat_thumbs'];
		m_array["Dat_url_cover_front"]  = url_img_site +'/images/covers/l/'+ m_array['Dat_cover_front'];
		m_array["Dat_url_cover_back"]   = '';
	} else {
		m_array["Dat_thumbs"]           = '';
		m_array["Dat_cover_front"]      = '';
		m_array["Dat_cover_back"]       = '';
		m_array["Dat_url_cover_thumbs"] = '';
		m_array["Dat_url_cover_front"]  = '';
		m_array["Dat_url_cover_back"]   = '';
	}
// FIN Imágenes Caratula --

	return m_array;
}

// Funciones Extras -----------------------------
function AnalyzeTag(texto, stRegExp, indxExp, stDefault)
{
	if (texto != '')
	{
		myRE = new RegExp(stRegExp);
		results = texto.match(myRE);
		if (results != null)
			return results[indxExp];
		else
			return stDefault;
	} else
		return stDefault;
}

function AnalyzeCategoriasMobyGames(texto, stRegExp, stRegExpDos, indxExp)
{
	var strTemp;
	var list_array = new Array();

	myRE_temp = new RegExp(stRegExp);
	temp_results = texto.match(myRE_temp);
	if (temp_results != null)
		strTemp = temp_results[1];
	else
		strTemp = '';

	myRE = new RegExp(stRegExpDos, 'g');
	final_results = strTemp.match(myRE);
	if (final_results != null)
	{
		for (var i = 0; i < final_results.length; i++)
		{
			rx = new RegExp(stRegExpDos);
			cat_results =  final_results[i].match(rx);

			if (cat_results[indxExp] != 'Combined View')
				list_array[i] = cat_results[indxExp];
		}
	}

	return list_array.join('; ');
}

function AnalyzeOtherGame(titulo, contenido)
{
	my_rx = new RegExp('<span style="([^"]*)"><a href="([^"]*)">([^"]*)</a> ([^"]*)</span>', 'g');
	results_contenido = contenido.match(my_rx);
	if (results_contenido != null)
	{
		for (var n = 0; n < results_contenido.length; n++)
		{
			rx_otro = new RegExp('<span style="([^"]*)"><a href="([^"]*)">([^"]*)</a> ([^"]*)</span>');
			results_game_Otro = results_contenido[n].match(rx_otro);
			twListaBusqueda.addItemFind(titulo, results_game_Otro[3], results_game_Otro[4].replace('(<em>','').replace('</em>)',''), results_game_Otro[2], 'datos');
		}
	}
}
