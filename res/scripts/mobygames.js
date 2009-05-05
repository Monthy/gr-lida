/*
 * GR-lida importation script
 * http://sharesource.org/project/grlida/
 * http://www.laisladelabandoware.es
 * 
 */
// Info -------------------------------
var authors  = "Monthy";					// Script authors
var title    = "MobyGames (EN)";
var url_site = "http://www.mobygames.com";	// Site address
var language = "en-EN";						// Site language
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
	m_url["url"]    = "http://www.mobygames.com/search/quick/p,-1/q,"+texto_busqueda.replace(" ","+")+"/showOnly,9/";

	return m_url;
}

function AnalyzeFindPage(texto)
{
// Expresion Regular para obtener los datos de la Busqueda
// <a href="/game/dos/alone-in-the-dark">Alone in the Dark</a><br>by Infogrames -- 1992<br>DOS<br>
// results_game[0] = toda la url
// <a href="/game/[1]/[2]">[3]</a><br>by [4] -- [5]<br>[6]<br>

	myReFind = new RegExp("<a href=\"/game/([^/]+)/([^\"]+)\">([^\"<>]*)</a><br>by ([^\"<>]*) -- ([^\"<>]*)<br>([^\"<>]*)<br>","g");
	resultsFind = texto.match(myReFind);
	for(var i = 0; i < resultsFind.length; i++)
	{
		rx = new RegExp("<a href=\"/game/([^/]+)/([^\"]+)\">([^\"<>]*)</a><br>by ([^\"<>]*) -- ([^\"<>]*)<br>([^\"<>]*)<br>");
		results_game =  resultsFind[i].match(rx);
		twListaBusqueda.addItemFind(results_game[3],results_game[6],results_game[5],"http://www.mobygames.com/game/"+results_game[1]+"/"+results_game[2]);
	}
}

function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();
// Usuario
	if ( texto.indexOf("was contributed by",0) != -1)
	{
		m_array["Dat_usuario"] = AnalyzeCategoriasMobyGames(texto, "was contributed by([^?]*)</div>",
								"<span class=\"sn\"><a href=\"/user/sheet/userSheetId,([0-9]+)/\">([^\"<>]*)</a>", 2);
	} else
		m_array["Dat_usuario"] = "";
// Icono
	m_array["Dat_icono"] = "datos";

// Titulo
// <div id="gameTitle"><a href="/game/dos/alone-in-the-dark">Alone in the Dark</a></div>
	myRE = new RegExp("<div id=\"gameTitle\"><a href=\"/game/([^/]+)/([^\"]+)\">([^\"<>]*)</a></div>");
	titulo_results = texto.match(myRE);
	m_array["Dat_titulo"]    = titulo_results[3];
	m_array["Dat_subtitulo"] = "";	

// Developed, Published
	if ( texto.indexOf("Developed by</div>") != -1)
	{
// Published by ------------
		m_array["Dat_compania"] = AnalyzeCategoriasMobyGames(texto, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Developed by",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
// Developed by ------------
		m_array["Dat_desarrollador"] = AnalyzeCategoriasMobyGames(texto, "Developed by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
	} else {
// Published by ------------
		m_array["Dat_compania"] = AnalyzeCategoriasMobyGames(texto, "Published by</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Released",
									"<a href=\"/company/([^/]+)\">([^\"<>]*)</a>", 2);
// Developed by ------------
		m_array["Dat_desarrollador"] = "";
	}

// Released ---------------
	m_array["Dat_anno"] = AnalyzeCategoriasMobyGames(texto, "Released</div><div [^>]*>([^?]*)</div>","<a href=\"/game/([^\"<>]*)/release-info\">([^\"<>]*)</a>", 2);

	if ( texto.indexOf("Platforms</div>") != -1)
	{
// Platforms --------------
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Platforms</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	}
	else if ( texto.indexOf("Platform</div>") != -1)
	{
// Platform ---------------
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Platform</div><div [^>]*>([^?]*)</div></div></td>\n    <td width=\"48%\"><div id=\"coreGameGenre\">",
									"<a href=\"([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_sistemaop"] = "";
	}

	if ( texto.indexOf("Also For</div>") != -1)
	{
// Also For ---------------
		m_array["Dat_sistemaop"] = AnalyzeCategoriasMobyGames(texto, "Also For</div><div [^>]*>([^?]*)</div></div></td>",
									"<a href=\"/game/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_sistemaop"] = "";
	}

	if ( texto.indexOf("aDeSe Rating</div>") != -1)
	{
// aDeSe Rating -----------
		m_array["Dat_adese_rating"] = AnalyzeCategoriasMobyGames(texto, "aDeSe Rating</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Genre",
									"<a href=\"/attribute/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
		m_array["Dat_adese_rating"] = "";
	}

	if ( texto.indexOf("Perspective</div>") != -1)
	{
// Genre ------------------
		m_array["Dat_genero"] = AnalyzeCategoriasMobyGames(texto, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Perspective",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);

		if ( texto.indexOf("Non-Sport</div>") != -1)
		{
// Perspective ------------
			m_array["Dat_perspective"] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		} else {
// Perspective ------------
			m_array["Dat_perspective"] = AnalyzeCategoriasMobyGames(texto, "Perspective</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Misc",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		}						
	} else {
// Genre ------------------
		m_array["Dat_genero"] = AnalyzeCategoriasMobyGames(texto, "Genre</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Non-Sport",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
// Perspective ------------
		m_array["Dat_perspective"] = "";
	}

	if ( texto.indexOf("Misc</div>") != -1 )
	{
		if ( texto.indexOf("Non-Sport</div>") != -1)
		{
// Non-Sport --------------
			m_array["Dat_tema"] = AnalyzeCategoriasMobyGames(texto, "Non-Sport</div><div [^>]*>([^?]*)</div><div style=\"([^?]*)\">Misc",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		} else {
			m_array["Dat_tema"] = "";
		}
// Misc -------------------
		m_array["Dat_misc"] = AnalyzeCategoriasMobyGames(texto, "Misc</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
	} else {
// Non-Sport --------------
		m_array["Dat_tema"] = AnalyzeCategoriasMobyGames(texto, "Non-Sport</div><div [^>]*>([^?]*)</div></div></div></td>",
									"<a href=\"/genre/([^\"<>]*)\">([^\"<>]*)</a>", 2);
		m_array["Dat_misc"] = "";
	}

// Description ------------
	myRE = new RegExp("<h2 class=\"m5\">Description</h2>([^[]*)<div class=\"sideBarLinks\">");
	comentario_results = texto.match(myRE);
	m_array["Dat_comentario"] = comentario_results[1].replace("<a href=\"","<a href=\"http://www.mobygames.com");

// MobyScore --------------
	// <div class="fr scoreBoxMed scoreHi">4.0</div>
	// <div class="fr scoreBoxMed scoreMed">3.1</div>
	// <div class="fr scoreBoxMed scoreNone">...</div>
	myRE = new RegExp("<div class=\"fr scoreBoxMed([^\"<>]*)\">([^\"<>]*)</div>");
	rating_results = texto.match(myRE);
	m_array["Dat_rating"] = Math.round(rating_results[2]);

//-------------------------------
	m_array["Dat_idioma"]      = "Castellano";
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

// Imagenes Caratula --------
	if ( texto.indexOf("http://www.mobygames.com/images/covers/small/notonfile.gif",0) != -1 || texto.indexOf("noCoverArt",0) != -1 )
	{
		m_array["Dat_thumbs"]      = "";
		m_array["Dat_cover_front"] = "";
		m_array["Dat_cover_back"]  = "";
		m_array["Dat_url_cover_thumbs"] = "";
		m_array["Dat_url_cover_front"]  = "";
		m_array["Dat_url_cover_back"]   = "";
	}
	else if ( texto.indexOf("http://www.mobygames.com/images/covers/small/",0) != -1)
	{
		myRE = new RegExp("<a href=\"/game/([^/]+)/([^\"]+)/cover-art/gameCoverId,([0-9]+)/\"><img alt=\"([^?]*)\" src=\"http://www.mobygames.com/images/covers/small/([^\"]+)\"");
		img_results = texto.match(myRE);

		m_array["Dat_thumbs"]          = img_results[5];
		m_array["Dat_cover_front"]     = img_results[5];
		m_array["Dat_cover_back"]      = "";

		m_array["Dat_url_cover_thumbs"] = "http://www.mobygames.com/images/covers/small/"+m_array["Dat_thumbs"];
		m_array["Dat_url_cover_front"]  = "http://www.mobygames.com/images/covers/large/"+m_array["Dat_cover_front"];
		m_array["Dat_url_cover_back"]   = "";
	}
// FIN Imagenes Caratula ----

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
