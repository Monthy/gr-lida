/*
 * GR-lida importation script
 * http://sharesource.org/project/grlida/
 * http://www.gr-lida.org
 * 
 */
// Info -------------------------------
var authors		= "Monthy";						// Script authors
var title		= "MobyGames (EN)";
var url_site	= "http://www.mobygames.com";	// Site address
var url_charset	= "UTF-8";
var language	= "en-EN";						// Site language
var version		= "0.3.0";						// Script version 17-11-2008 update 19-04-2010
var requires	= "0.8.0";						// GR-lida version
var comments	= "";
var license		= "GPL v2";
var description	= "Script para obtener los datos del juego (titulo, genero, año, compañía, etc...) así como la caratula principal del juego.";

function UrlWebBusqueda(texto_busqueda)
{
	var m_url = new Array();

	m_url["metodo"] = "GET";	// POST, GET
	m_url["c_post"] = "";		// Contenido del POST si es post
	m_url["url"]    = url_site+"/search/quick?q="+texto_busqueda.replace(" ","+")+"&p=-1&search=Go&sFilter=1&sG=on&sGG=on&sA=on&sD=on&sC=on";

	return m_url;
}

function AnalyzeFindPage(texto)
{
// Expresion Regular para obtener los datos de la Busqueda
	var textoFind, m_titulo, m_contenido, m_url_game;
	textoFind = texto.replace(/<\/div><\/div><br clear="all"><\/div>/gi,"<fin_searchDetails>\n");// final a

/* Busqueda CON imagenes
--------------------------------------------------------*/
	myReFind = new RegExp("<div class=\"([0-9a-z]+)\"><div class=\"searchResult\"><div class=\"searchNumber\">([0-9]+).</div><div class=\"searchImage\"><a href=\"([^\"]*)\"><img class=\"searchResultImage\" alt=\"([^\"]*)\" border=\"([^\"]*)\" src=\"([^\"]*)\" height=\"([^\"]*)\" width=\"([^\"]*)\" ></a></div><div class=\"searchData\"><div class=\"searchTitle\">Game: <a href=\"([^\"]*)\">([^<]*)</a>([^\"]*)</div><div class=\"searchDetails\">([^\n[]*)</div><fin_searchDetails>\n","g");
	resultsFind = textoFind.match(myReFind);
	for(var i = 0; i < resultsFind.length; i++)
	{
		rx_Title = new RegExp("<div class=\"([0-9a-z]+)\"><div class=\"searchResult\"><div class=\"searchNumber\">([0-9]+).</div><div class=\"searchImage\"><a href=\"([^\"]*)\"><img class=\"searchResultImage\" alt=\"([^\"]*)\" border=\"([^\"]*)\" src=\"([^\"]*)\" height=\"([^\"]*)\" width=\"([^\"]*)\" ></a></div><div class=\"searchData\"><div class=\"searchTitle\">Game: <a href=\"([^\"]*)\">([^<]*)</a>([^\"]*)</div><div class=\"searchDetails\">([^\n[]*)</div><fin_searchDetails>\n");
		results_game_Title = resultsFind[i].match(rx_Title);

		m_titulo    = results_game_Title[10];
		m_contenido = results_game_Title[12].replace(' and ',', ').replace('</div>','');
		m_url_game  = url_site + results_game_Title[9];

		twListaBusqueda.addItemFind(m_titulo, "-", "-", m_url_game);
		AnalyzeOtherGame(m_titulo, m_contenido);
	}

/* Busqueda SIN imagenes
--------------------------------------------------------*/
	myReFind = new RegExp("<div class=\"([0-9a-z]+)\"><div class=\"searchResult\"><div class=\"searchNumber\">([0-9]+).</div><div class=\"searchData\"><div class=\"searchTitle\">Game: <a href=\"([^\"]*)\">([^<]*)</a>([^\"]*)</div><div class=\"searchDetails\">([^\n[]*)</div><fin_searchDetails>\n","g");
	resultsFind = textoFind.match(myReFind);
	for(var i = 0; i < resultsFind.length; i++)
	{
		rx_Title = new RegExp("<div class=\"([0-9a-z]+)\"><div class=\"searchResult\"><div class=\"searchNumber\">([0-9]+).</div><div class=\"searchData\"><div class=\"searchTitle\">Game: <a href=\"([^\"]*)\">([^<]*)</a>([^\"]*)</div><div class=\"searchDetails\">([^\n[]*)</div><fin_searchDetails>\n");
		results_game_Title = resultsFind[i].match(rx_Title);

		m_titulo    = results_game_Title[4];
		m_contenido = results_game_Title[6].replace(' and ',', ').replace('</div>','');
		m_url_game  = url_site + results_game_Title[3];

		twListaBusqueda.addItemFind(m_titulo, "-", "-", m_url_game);
		AnalyzeOtherGame(m_titulo, m_contenido);
	}
}

function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();

// Usuario ----------------
	if ( texto.indexOf("was contributed by",0) != -1)
	{
		m_array["Dat_usuario"] = AnalyzeCategoriasMobyGames(texto, "contributed by([^[]*)</div>",
								"<a href=\"([^\"]*)\">([^</]*)</a>", 2);
	} else
		m_array["Dat_usuario"] = "";
// Icono ------------------
	m_array["Dat_icono"] = "datos";

// Titulo -----------------
// <div id="gameTitle"><a href="/game/dos/alone-in-the-dark">Alone in the Dark</a></div>
	myRE = new RegExp("<div id=\"gameTitle\"><a href=\"([^\"]*)\">([^\n[]*)</a></div>");
	titulo_results = texto.match(myRE);
	m_array["Dat_titulo"]    = titulo_results[2];
	m_array["Dat_subtitulo"] = "";

// Developed, Published ---
	if ( texto.indexOf("Developed by</div>") != -1)
	{
		m_array["Dat_compania"] = AnalyzeCategoriasMobyGames(texto, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Developed by",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
		m_array["Dat_desarrollador"] = AnalyzeCategoriasMobyGames(texto, "Developed by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
	} else {
		if ( texto.indexOf("Published by</div>") != -1)
		{
			m_array["Dat_compania"] = AnalyzeCategoriasMobyGames(texto, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
										"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
		} else {
			m_array["Dat_compania"] = "";
		}
		m_array["Dat_desarrollador"] = "";
	}

// Released ---------------
	m_array["Dat_anno"] = AnalyzeCategoriasMobyGames(texto, "Released</div><div [^>]*>([^?]*)</div>","<a href=\"/game/([^\"<>]*)/release-info\">([^\"<>]*)</a>", 2);

// Platforms --------------
	if ( texto.indexOf("Platforms</div>") != -1)
	{
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Platforms</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} 
	else if ( texto.indexOf("Platform</div>") != -1)
	{
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Platform</div><div [^>]*>([^?]*)</div></div></td>\n    <td width=\"48%\"><div id=\"coreGameGenre\">",
									"<a href=\"([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_sistemaop"] = "";
	}

// Also For ---------------
	if ( texto.indexOf("Also For</div>") != -1)
	{
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Also For</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_sistemaop"] = "";
	}

// aDeSe Rating -----------
	if ( texto.indexOf("aDeSe Rating</div>") != -1)
	{
		m_array["Dat_edad_recomendada"] = AnalyzeCategoriasMobyGames(texto, "aDeSe Rating</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Genre",
										"<a href=\"/attribute/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_edad_recomendada"] = "nd";
	}

// Genre, Perspective -----
	if ( texto.indexOf("Perspective</div>") != -1)
	{
		m_array["Dat_genero"] = AnalyzeCategoriasMobyGames(texto, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Perspective",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		if ( texto.indexOf("Non-Sport</div>") != -1)
		{
			m_array["Dat_perspectiva"] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		} else {
			if ( texto.indexOf("Misc</div>") != -1)
			{
				m_array["Dat_perspectiva"] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Misc",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
			} else {
				m_array["Dat_perspectiva"] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
			}
		}		
	} else {
		m_array["Dat_genero"] = AnalyzeCategoriasMobyGames(texto, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		m_array["Dat_perspectiva"] = "";
	}

// Misc, Non-Sport --------
	if ( texto.indexOf("Misc</div>") != -1 )
	{
		if ( texto.indexOf("Non-Sport</div>") != -1)
		{
			m_array["Dat_tema"] = AnalyzeCategoriasMobyGames(texto, "Non-Sport</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Misc",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		} else {
			m_array["Dat_tema"] = "";
		}
		m_array["Dat_misc"] = AnalyzeCategoriasMobyGames(texto, "Misc</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		if ( texto.indexOf("Non-Sport</div>") != -1)
		{
			m_array["Dat_tema"] = AnalyzeCategoriasMobyGames(texto, "Non-Sport</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		} else {
			m_array["Dat_tema"] = "";
		}
		m_array["Dat_misc"] = "";
	}

// Description ------------
	myRE = new RegExp("<h2 class=\"m5\">Description</h2>([^[]*)<div class=\"sideBarLinks\">");
	comentario_results = texto.match(myRE);
	if(comentario_results != null)
		m_array["Dat_comentario"] = comentario_results[1].replace("<a href=\"","<a href=\""+url_site);
	else
		m_array["Dat_comentario"] = "";

// MobyScore --------------
	// <div class="fr scoreBoxMed scoreHi">4.0</div>
	// <div class="fr scoreBoxMed scoreMed">3.1</div>
	// <div class="fr scoreBoxMed scoreNone">...</div>
	myRE = new RegExp("<div class=\"fr scoreBoxMed([^\"<>]*)\">([^\"<>]*)</div>");
	rating_results = texto.match(myRE);
	if(rating_results != null)
	{
		m_array["Dat_rating"] = Math.round(rating_results[2]);
	} else {
		m_array["Dat_rating"] = "0";
	}

//-------------------------
	m_array["Dat_idioma"]      = "";
	m_array["Dat_idioma_voces"]= "";
	m_array["Dat_formato"]     = "PC";
	m_array["Dat_numdisc"]     = "";
	m_array["Dat_tamano"]      = "";
	m_array["Dat_graficos"]    = "1";
	m_array["Dat_sonido"]      = "1";
	m_array["Dat_jugabilidad"] = "1";
	m_array["Dat_original"]    = "false";
	m_array["Dat_estado"]      = "";
	m_array["Dat_fecha"]       = "";
	m_array["Dat_tipo_emu"]    = "datos"; // Importante = datos, dosbox, scummvm, vdmsound
	m_array["Dat_favorito"]    = "";
	m_array["Dat_path_exe"]    = "";
	m_array["Dat_parametros_exe"] = "";

// Imagenes Caratula ------
	if ( texto.indexOf(url_site+"/images/covers/small/notonfile.gif",0) != -1 || texto.indexOf("noCoverArt",0) != -1 )
	{
		m_array["Dat_thumbs"]      = "";
		m_array["Dat_cover_front"] = "";
		m_array["Dat_cover_back"]  = "";
		m_array["Dat_url_cover_thumbs"] = "";
		m_array["Dat_url_cover_front"]  = "";
		m_array["Dat_url_cover_back"]   = "";
	}
	else if ( texto.indexOf(url_site+"/images/covers/small/",0) != -1)
	{
		myRE = new RegExp("<a href=\"/game/([^/]+)/([^\"]+)/cover-art/gameCoverId,([0-9]+)/\"><img alt=\"([^?]*)\" src=\""+url_site+"/images/covers/small/([^\"]+)\"");
		img_results = texto.match(myRE);
		if(img_results != null)
		{
			m_array["Dat_thumbs"]      = img_results[5];
			m_array["Dat_cover_front"] = img_results[5];
		} else {
			myRE = new RegExp("<a href=\"/game/([^\"]+)/cover-art/gameCoverId,([0-9]+)/\"><img alt=\"([^?]*)\" src=\""+url_site+"/images/covers/small/([^\"]+)\"");
			img_results = texto.match(myRE);
			if(img_results != null)
			{
				m_array["Dat_thumbs"]      = img_results[4];
				m_array["Dat_cover_front"] = img_results[4];
			} else {
				m_array["Dat_thumbs"]      = "";
				m_array["Dat_cover_front"] = "";
			}
		}
		m_array["Dat_cover_back"]       = "";
		m_array["Dat_url_cover_thumbs"] = url_site+"/images/covers/small/"+m_array["Dat_thumbs"];
		m_array["Dat_url_cover_front"]  = url_site+"/images/covers/large/"+m_array["Dat_cover_front"];
		m_array["Dat_url_cover_back"]   = "";
	}
// FIN Imagenes Caratula --
	return m_array;
}

// Funciones Extras -----------------------------
function AnalyzeCategoriasMobyGames(texto, stRegExp, stRegExpDos, indxExp)
{
	var strTemp;
	var list_array = new Array();

	myRE_temp = new RegExp( stRegExp );
	temp_results = texto.match( myRE_temp );
	strTemp = temp_results[1];

	myRE = new RegExp( stRegExpDos,"g");
	results = strTemp.match( myRE );
	for(var i = 0; i < results.length; i++)
	{
		rx = new RegExp( stRegExpDos );
		cat_results =  results[i].match(rx);
		list_array[i] = cat_results[indxExp].replace("&nbsp;"," ");
	}

	return list_array.join(", ");
}

function AnalyzeOtherGame(titulo, contenido)
{
	my_rx = new RegExp("<span style=\"([^\"]*)\"><a href=\"([^\"]*)\">([^\"]*)</a> ([^\"]*)</span>","g");
	results_contenido = contenido.match(my_rx);
	if(results_contenido != null)
	{
		for(var n = 0; n < results_contenido.length; n++)
		{
			rx_otro = new RegExp("<span style=\"([^\"]*)\"><a href=\"([^\"]*)\">([^\"]*)</a> ([^\"]*)</span>");
			results_game_Otro = results_contenido[n].match(rx_otro);
			twListaBusqueda.addItemFind(titulo, results_game_Otro[3], results_game_Otro[4].replace('(<em>','').replace('</em>)',''), url_site+results_game_Otro[2]);
		}
	}
}
