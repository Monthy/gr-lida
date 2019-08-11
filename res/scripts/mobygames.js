/*
 * GR-lida importation script
 * https://github.com/Monthy/gr-lida
 * http://www.gr-lida.org
 *
 */
// Info -------------------------------
var authors			= 'Monthy';						// Script authors / Autor del script.
var title			= 'MobyGames (EN)';				// Title script / Título del script.
var url_site		= 'https://www.mobygames.com';	// Site address / Dirección del sitio.
var url_img_site	= 'https://www.mobygames.com';	// Site pics address / Dirección de las imágenes sitio.
var url_charset		= 'UTF-8';						// Character encoding / Codificación de caracteres.
var url_page_skip	= 50;							// Number of items per page / Número de item por cada página.
var language		= 'en-EN';						// Site language / Idioma del sitio, es-ES, en-EN ...
var version			= '0.5.0';						// Script version 17-11-2008 update 16-12-2018 / Versión del script y la actualización.
var requires		= '0.12.0';						// GR-lida version.
var comments		= '';							// Comments / Comentarios.
var license			= 'GPL v2';						// License / Licencia.
var description		= 'Script para obtener los datos del juego (título, genero, año, compañía, etc...) así como la caratula principal del juego.'; // Description / Descripción.


/**
 * Genera la configuración para hacer la búsqueda en la web indicada.
 *
 * @param {string} texto_busqueda - Texto de búsqueda.
 * @param {string} page - Indica la página de la búsqueda.
 * @returns {Array} Retorna los datos de configuración de la url para hacer la búsqueda.
 */
function UrlWebBusqueda(texto_busqueda, page)
{
	var m_url = new Array();

	m_url['metodo'] = 'GET';	// POST, GET.
	m_url['c_post'] = '';		// Contenido del POST si es post.
	m_url['url']    = url_site +'/search/quick/offset,'+ page +'/p,-1/q,'+ texto_busqueda +'/sA,on/sC,on/sD,on/sFilter,1/sG,on/sGG,on/search,Go/';

	return m_url;
}

/**
 * Analiza el código HTML del resultado de la búsqueda.
 *
 * @param {string} texto - Código HTML a analizar.
 */
function AnalyzeFindPage(texto)
{
// Expresión Regular para obtener los datos de la búsqueda.
	var textoFind, m_titulo, m_contenido, m_url_game;
	textoFind = texto.replace(/<\/div><\/div><br clear="all"><\/div>/gi, '<fin_searchDetails>\n');// final a
	textoFind = textoFind.replace(/&nbsp;/gi, ' ');

/* Búsqueda Páginas
--------------------------------------------------------*/
	myReFind = new RegExp('search,Go/">([0-9]+)</a>', 'g');
	resultsFind = textoFind.match(myReFind);
	if (resultsFind != null)
	{
		var pag = 0;
		for (var i = 0; i < resultsFind.length; i++)
		{
			cbxPaginas.addItemPages((i+1), pag.toString());
			pag = pag + url_page_skip;
		}
	} else
		cbxPaginas.addItemPages('1', '0');

/* Búsqueda CON imágenes
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

/* Búsqueda SIN imágenes
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

/**
 * Analiza el código HTML de la ficha del juego para obtener los distintos
 * datos como el título, el genero, compañía, etc...
 *
 * @param {string} texto - Código HTML a analizar.
 * @param {boolean} local - Indica si se hace de forma local.
 * @returns {Array} Retorna el Array con los datos obtenidos.
 */
function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();
// INI Temp ---------------
	m_array['Dat_grupo']            = '';
	m_array['Dat_idioma']           = '';
	m_array['Dat_idioma_voces']     = '';
	m_array['Dat_formato']          = '';
	m_array['Dat_numdisc']          = '';
	m_array['Dat_tamano']           = '';
	m_array['Dat_graficos']         = '0';
	m_array['Dat_sonido']           = '0';
	m_array['Dat_jugabilidad']      = '0';
	m_array['Dat_original']         = 'false';
	m_array['Dat_estado']           = '';
	m_array['Dat_fecha']            = '';
	m_array['Dat_tipo_emu']         = 'datos'; // Importante = datos
	m_array['Dat_favorito']         = 'false';
	m_array['Dat_path_exe']         = '';
	m_array['Dat_parametros_exe']   = '';
	m_array['Dat_path_capturas']    = '';
	m_array['Dat_path_setup']       = '';
	m_array['Dat_parametros_setup'] = '';
// FIN Temp ---------------

// Algunos reemplazos.
	texto = texto.replace(/&nbsp;/gi, ' ');

// Icono ------------------
	m_array['Dat_icono'] = 'datos';
// Título -----------------
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

// INI Imágenes Caratula --
// /game/dos/alone-in-the-dark/cover-art/gameCoverId,440/

// INI Temp ---------------
	m_array['Dat_thumbs']                 = '';
	m_array['Dat_thumbs_url']             = '';

	m_array['Dat_cover_front']            = '';
	m_array['Dat_cover_front_url']        = '';
	m_array['Dat_cover_front_url_small']  = '';

	m_array['Dat_cover_back']             = '';
	m_array['Dat_cover_back_url']         = '';
	m_array['Dat_cover_back_url_small']   = '';

	m_array['Dat_cover_left']             = '';
	m_array['Dat_cover_left_url']         = '';
	m_array['Dat_cover_left_url_small']   = '';

	m_array['Dat_cover_right']            = '';
	m_array['Dat_cover_right_url']        = '';
	m_array['Dat_cover_right_url_small']  = '';

	m_array['Dat_cover_top']              = '';
	m_array['Dat_cover_top_url']          = '';
	m_array['Dat_cover_top_url_small']    = '';

	m_array['Dat_cover_bottom']           = '';
	m_array['Dat_cover_bottom_url']       = '';
	m_array['Dat_cover_bottom_url_small'] = '';

	m_array['Dat_covers_other_page']      = 'false';
	m_array['Dat_url_all_covers']         = '';
// FIN Temp ---------------

	if (texto.indexOf('/images/covers/s/', 0) != -1)
	{
		var url_covers = '';
//		var id_covers  = '';
		myRE = new RegExp('/game/([^"]+)/([^"]+)/cover-art/gameCoverId,([0-9]+)/"><img alt="([^<]*)" border="0" src="/images/covers/s/([^"]+)"');
		img_results = texto.match(myRE);
		if (img_results != null)
		{
			m_array['Dat_thumbs']      = img_results[5];
			m_array['Dat_cover_front'] = img_results[5];
			url_covers = img_results[1] +'/'+ img_results[2];
//			id_covers  = img_results[3];
		} else {
			myRE = new RegExp('/game/([^"]+)/cover-art/gameCoverId,([0-9]+)/"><img alt="([^<]*)" border="0" src="/images/covers/s/([^"]+)"');
			img_results = texto.match(myRE);
			if (img_results != null)
			{
				m_array['Dat_thumbs']      = img_results[4];
				m_array['Dat_cover_front'] = img_results[4];
				url_covers = img_results[1];
//				id_covers  = img_results[2];
			} else {
				m_array['Dat_thumbs']      = '';
				m_array['Dat_cover_front'] = '';
			}
		}

//		url_covers = AnalyzeTag(texto, '/game/([^"]+)/cover-art">Cover Art</a>', 1, '');	// Alternativa.

		m_array['Dat_thumbs_url']             = url_img_site +'/images/covers/s/'+ m_array['Dat_thumbs'];
		m_array['Dat_cover_front_url']        = url_img_site +'/images/covers/l/'+ m_array['Dat_cover_front'];
		m_array['Dat_cover_front_url_small']  = url_img_site +'/images/covers/s/'+ m_array['Dat_cover_front'];

// http://www.mobygames.com/game/dos/alone-in-the-dark/cover-art
// http://www.mobygames.com/game/dos/alone-in-the-dark/cover-art/gameCoverId,440/
		m_array['Dat_covers_other_page'] = 'true';
		m_array['Dat_url_all_covers']    = url_img_site +'/game/'+ url_covers +'/cover-art';
	}
// FIN Imágenes Caratula --

	return m_array;
}

/**
 * Analiza el código HTML para obtener las distintas caratulas
 * como la caratula frontal, trasera, lateral izquiedo,
 * lateral derecho, etc...
 *
 * @param {string} texto - Código HTML a analizar.
 * @returns {Array} Retorna el Array con los datos obtenidos.
 */
function AnalyzeGameCoverPage(texto)
{
	var m_array    = new Array();
	var all_covers = '';
	var textoFind  = '';

// Algunos reemplazos.
	textoFind = texto.replace(/<p>/gi, '{p}');
	textoFind = textoFind.replace(/<\/p>/gi, '{/p}');
	textoFind = textoFind.replace(/url\(/gi, 'url:');
	textoFind = textoFind.replace(/\);/gi, ':;');
	textoFind = textoFind.replace(/&nbsp;/gi, ' ');

// INI Temp ---------------
	m_array['Dat_thumbs']                 = '';
	m_array['Dat_thumbs_url']             = '';
	m_array['Dat_cover_front']            = '';
	m_array['Dat_cover_front_url']        = '';
	m_array['Dat_cover_front_url_small']  = '';
	m_array['Dat_cover_back']             = '';
	m_array['Dat_cover_back_url']         = '';
	m_array['Dat_cover_back_url_small']   = '';
	m_array['Dat_cover_left']             = '';
	m_array['Dat_cover_left_url']         = '';
	m_array['Dat_cover_left_url_small']   = '';
	m_array['Dat_cover_right']            = '';
	m_array['Dat_cover_right_url']        = '';
	m_array['Dat_cover_right_url_small']  = '';
	m_array['Dat_cover_top']              = '';
	m_array['Dat_cover_top_url']          = '';
	m_array['Dat_cover_top_url_small']    = '';
	m_array['Dat_cover_bottom']           = '';
	m_array['Dat_cover_bottom_url']       = '';
	m_array['Dat_cover_bottom_url_small'] = '';
// FIN Temp ---------------

	myRE = new RegExp('<div class="row">([^[\[]*)<div class="sideBarLinks"', 'g');
	row_results = textoFind.match(myRE);
	if (row_results != null)
	{
		if (row_results.length > -1)
		{
			myRE = new RegExp('/game/([^/]+)/([^/]+)/cover-art/gameCoverId,([0-9]+)/" title="([^"]+)" class="thumbnail-([^"]+)" style="background-image:url:/images/covers/s/([^:]+):;"></a>([^{]*){p}([^{]*){/p}', 'g');
			img_results = row_results[0].match(myRE);
			var list_covers_array = AnalyzeGameCover(img_results);
			for (var i = 0; i < list_covers_array.length; i++)
			{
				var img_type = list_covers_array[i]['img_type'];
				if (img_type == 'Front Cover')
				{
					m_array['Dat_thumbs']                = list_covers_array[i]['img_name'];
					m_array['Dat_thumbs_url']            = list_covers_array[i]['img_url_small'];
					m_array['Dat_cover_front']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_front_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_front_url_small'] = list_covers_array[i]['img_url_small'];
				}
				if (img_type == 'Back Cover')
				{
					m_array['Dat_cover_back']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_back_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_back_url_small'] = list_covers_array[i]['img_url_small'];
				}
				if (img_type == 'Spine/Sides Left' || img_type == 'Spine/Sides Box Left' || img_type == 'Spine/Sides Left/Right')
				{
					m_array['Dat_cover_left']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_left_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_left_url_small'] = list_covers_array[i]['img_url_small'];
				}
				if (img_type == 'Spine/Sides Right' || img_type == 'Spine/Sides Box Right' || img_type == 'Spine/Sides Left/Right')
				{
					m_array['Dat_cover_right']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_right_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_right_url_small'] = list_covers_array[i]['img_url_small'];
				}
				if (img_type == 'Spine/Sides Top' || img_type == 'Spine/Sides Box Top' || img_type == 'Spine/Sides Top/Bottom')
				{
					m_array['Dat_cover_top']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_top_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_top_url_small'] = list_covers_array[i]['img_url_small'];
				}
				if (img_type == 'Spine/Sides Bottom' || img_type == 'Spine/Sides Box Bottom' || img_type == 'Spine/Sides Top/Bottom')
				{
					m_array['Dat_cover_bottom']           = list_covers_array[i]['img_name'];
					m_array['Dat_cover_bottom_url']       = list_covers_array[i]['img_url_full'];
					m_array['Dat_cover_bottom_url_small'] = list_covers_array[i]['img_url_small'];
				}

				all_covers += list_covers_array[i]['img_name'] +'|'+ list_covers_array[i]['img_url_full'] +'|'+ img_type +'|'+ list_covers_array[i]['img_url_small'] +'\n';
			}
		}
	}

	m_array['Dat_all_covers'] = all_covers;

	return m_array;
}

/**
 * Analiza el código HTML para obtener todas las imágenes
 * posibles y disponibles como la caratula frontal, trasera,
 * lateral izquiedo, lateral derecho, etc...
 *
 * @param {string} texto - Código HTML a analizar.
 * @returns {Array} Retorna el Array con los datos obtenidos.
 */
function AnalyzeGameMoreCoverPage(texto)
{
	var m_array    = new Array();
	var all_covers = '';
	var textoFind  = '';

	textoFind = texto.replace(/<p>/gi, '{p}');
	textoFind = textoFind.replace(/<\/p>/gi, '{/p}');
	textoFind = textoFind.replace(/url\(/gi, 'url:');
	textoFind = textoFind.replace(/\);/gi, ':;');
	textoFind = textoFind.replace(/&nbsp;/gi, ' ');

	myRE = new RegExp('/game/([^/]+)/([^/]+)/cover-art/gameCoverId,([0-9]+)/" title="([^"]+)" class="thumbnail-([^"]+)" style="background-image:url:/images/covers/s/([^:]+):;"></a>([^{]*){p}([^{]*){/p}', 'g');
	img_results = textoFind.match(myRE);
	var list_covers_array = AnalyzeGameCover(img_results);
	for (var i = 0; i < list_covers_array.length; i++)
	{
		all_covers += list_covers_array[i]['img_name'] +'|'+ list_covers_array[i]['img_url_full'] +'|'+ list_covers_array[i]['img_type'] +'|'+ list_covers_array[i]['img_url_small'] +'\n';
	}

	m_array['Dat_all_covers'] = all_covers;

	return m_array;
}

// Funciones Extras -----------------------------
/**
 * Función extra para obtener los datos dentro de las etiquetas HTML
 * entre otras.
 *
 * @param {string} texto - Código HTML a analizar.
 * @param {string|RegExp} stRegExp - Expresión regular para hacer la busqueda deseada.
 * @param {intiger} indxExp - Indice del resultado que sera devuelto.
 * @param {string} stDefault - Texto por defecto si no se obtene nada.
 * @returns {string} Retorna el texto encontrado o el indicado por defecto.
 */
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

/**
 * Función extra para obtener los datos de las categorías de MobyGames.
 *
 * @param {string} texto - Código HTML a analizar.
 * @param {string|RegExp} stRegExp - Expresión regular para hacer la primera busqueda deseada.
 * @param {string} stRegExpDos - Expresión regular para hacer la segunda busqueda deseada y obtener los datos.
 * @param {intiger} indxExp - Indice del resultado que sera devuelto.
 * @returns
 */
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

/**
 * Función extra para obtener otros títulos en la busqueda.
 *
 * @param {string} titulo - Título de la busqueda.
 * @param {string} contenido - Contenido donde buscar y extraer los otros títulos.
 */
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

/**
 *
 *
 * @param {*} img_results
 * @returns
 */
function AnalyzeGameCover(img_results)
{
	var list_array = new Array();
	if (img_results != null)
	{
		for (var i = 0; i < img_results.length; i++)
		{
			rx_img = new RegExp('/game/([^/]+)/([^/]+)/cover-art/gameCoverId,([0-9]+)/" title="([^"]+)" class="thumbnail-([^"]+)" style="background-image:url:/images/covers/s/([^:]+):;"></a>([^{]*){p}([^{]*){/p}');
			results_img = img_results[i].match(rx_img);
			if (results_img != null)
			{
				var m_array = new Array();
				m_array['img_name']      = results_img[6];
				m_array['img_type']      = results_img[8].replace(/<br>/gi, ' ').replace(/ \/ /gi, '/');
				m_array['img_url_full']  = url_img_site +'/images/covers/l/'+ m_array['img_name'];
				m_array['img_url_small'] = url_img_site +'/images/covers/s/'+ m_array['img_name'];

				list_array.push(m_array);
			}
		}
	}

	return list_array;
}
