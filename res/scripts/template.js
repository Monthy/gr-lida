/*
 * GR-lida importation script
 * https://github.com/Monthy/gr-lida
 * http://www.gr-lida.org
 * 
 */
// Info -------------------------------
var authors			= 'Autor';						// Script authors / Autor del script
var title			= 'Título de la web';			// Title script / Título del script
var url_site		= 'http://www.ejemplo.org';		// Site address / Dirección del sitio
var url_img_site	= 'http://www.img.ejemplo.com';	// Site pics address / Dirección de las imágenes sitio
var url_charset		= 'UTF-8';
var url_page_skip	= 10;							// Number of items per page / Número de item por cada página
var language		= 'es-ES';						// Site language / Idioma del sitio, es-ES, en-EN ...
var version			= '0.1.0';						// Script version dd-mm-yyyy update dd-mm-yyyy / Versión del script y la actualización
var requires		= '0.12.0';						// GR-lida version
var comments		= '';							// Comments / Comentarios
var license			= 'GPL v2';						// License / Licencia
var description		= 'Descripción del script';		// Description / Descripción

/*

Funciones obligatorias, solo se modifica el contenido
para adaptarse a la web y obtener los datos.
Es decir no puede cambiar los nombres y parámetros de la función.

Lista de funciones obligatorias:
	function UrlWebBusqueda(texto_busqueda, page);
	function AnalyzeFindPage(texto);
	function AnalyzeGamePage(texto, local);
	function AnalyzeGameCoverPage(texto);
	function AnalyzeGameMoreCoverPage(texto);

Funciones opcionales se pueden modificar, añadir o eliminar
ya que solo son funciones que ayudan en la optención de datos.

Lista de funciones opcionales:
	function AnalyzeTagName(texto, stRegExp, indxExp, stDefault);

-- In English --
Required functions, only the content is modified
To adapt to the web and obtain the data.
In other words, you can not change the names and parameters of the function.

List of required functions:
	function UrlWebBusqueda(texto_busqueda, page);
	function AnalyzeFindPage(texto);
	function AnalyzeGamePage(texto, local);
	function AnalyzeGameCoverPage(texto);
	function AnalyzeGameMoreCoverPage(texto);

Optional functions can be modified, added or deleted
since they are only functions that help in data opting.

List of optional functions:
	function AnalyzeTagName(texto, stRegExp, indxExp, stDefault);

	
/* Busqueda del Juego / Game Search
--------------------------------------------------------*/
function UrlWebBusqueda(texto_busqueda, page)
{
	var m_url = new Array();

	m_url['metodo'] = 'GET';	// POST, GET
	m_url['c_post'] = '';		// Contenido del POST si es post, ejemplo:
								// title=Alone&otro=Otro
	m_url['url'] = url_site +'/index.php?titulo='+ texto_busqueda;

	return m_url;
}

/* AnalyzeFindPage
--------------------------------------------------------
	Analiza el html de la busqueda descargada añadiendo el 
	número de páginas y los juegos encontrados.

-- In English --
	Analyze the html of the downloaded search by adding the
	number of pages and games found.
*/
function AnalyzeFindPage(texto)
{
/* Busqueda Páginas / Search Pages
--------------------------------------------------------
	Para añadir lista de paginas para la busqueda / To add list of pages for the search:

	@param {string} Título de la página / Page title.
	@param {string} Numero de la página / Number of the page.

		cbxPaginas.addItemPages(title, num_pag);

 Ejemplo con MobyGames / Example with MobyGames:
--------------------------------------------------------*/
	myReFind = new RegExp('search,Go/">([0-9]+)</a>', 'g');
	resultsFind = textoFind.match(myReFind);
	if (resultsFind != null)
	{
		var pag = 0;
		for(var i = 0; i < resultsFind.length; i++)
		{
			cbxPaginas.addItemPages((i+1), pag.toString());
			pag = pag + url_page_skip;
		}
	} else
		cbxPaginas.addItemPages('1', '0');

/* Busqueda de Juegos / Game Search
--------------------------------------------------------
	Busca los juegos en la página y los añade a la lista / Find the games on the page and add them to the list:

	@param {string} Título del juego / Game title.
	@param {string} Plataforma del juego / Game platform.
	@param {string} Fecha de publicacion / Date of publication.
	@param {string} Dirección URL de la ficha / URL of the file.
	@param {string} Tipo de emulador / Type of emulator. (datos|dosbox|scummvm|vdmsound)

		twListaBusqueda.addItemFind(titulo, plataforma, publicado, url_site, tipo_emu);		

Ejemplo con GR-lida / Example with GR-lida:
--------------------------------------------------------*/
	myReFind = new RegExp('<juego ID="([0-9]+)">([^"]*)</juego>', 'g');
	resultsFind = texto.match(myReFind);
	for(var i = 0; i < resultsFind.length; i++)
	{
		results_id         = AnalyzeTagName(resultsFind[i], 'ID="([0-9]+)"', 1, '');
		results_id_emu     = AnalyzeTagName(resultsFind[i], '<id_emu>([0-9]+)</id_emu>', 1, '');
		results_titulo     = AnalyzeTagName(resultsFind[i], '<titulo>([^"<>]*)</titulo>', 1, '');
		results_plataforma = AnalyzeTagName(resultsFind[i], '<plataforma>([^"<>]*)</plataforma>', 1, '');
		results_publicado  = AnalyzeTagName(resultsFind[i], '<publicado>([0-9]+)</publicado>', 1, '');
		results_tipo_emu   = AnalyzeTagName(resultsFind[i], '<tipo_emu>([^"<>]*)</tipo_emu>', 1, 'datos');
		results_alt_id     = AnalyzeTagName(resultsFind[i], '<alt_id>([0-9]+)</alt_id>', 1, '');

		twListaBusqueda.addItemFind(results_titulo, results_plataforma, results_publicado, url_site +'/grlidadb.php?ver=juego&gid='+ results_id +'&id_emu='+ results_id_emu +'&tipo_emu='+ results_tipo_emu +'&alt_id='+ results_alt_id, results_tipo_emu);		
	}
}

/* AnalyzeGamePage
--------------------------------------------------------
	Analiza el html de la ficha descargada, mediante expresiones
	regulares obtendremos los distintos datos y lo asignamos a cada
	array.

-- In English --
	Analyze the html of the downloaded file, using expressions
	we will obtain the different data and assign it to each
	array.
*/
function AnalyzeGamePage(texto, local)
{
	var m_array = new Array();

/* Obtención de datos del Juegos // Obtaining Game Data
--------------------------------------------------------*/
	m_array['Dat_icono']            = ''; // Varchar(255)
	m_array['Dat_titulo']           = ''; // Varchar(255)
	m_array['Dat_subtitulo']        = ''; // Varchar(255)
	m_array['Dat_genero']           = ''; // Varchar(255)
	m_array['Dat_compania']         = ''; // Varchar(255)
	m_array['Dat_desarrollador']    = ''; // Varchar(255)
	m_array['Dat_tema']             = ''; // Varchar(255)
	m_array['Dat_grupo']            = ''; // Varchar(255)
	m_array['Dat_perspectiva']      = ''; // Varchar(255)
	m_array['Dat_idioma']           = ''; // Varchar(50)
	m_array['Dat_idioma_voces']     = ''; // Varchar(50)
	m_array['Dat_formato']          = ''; // Varchar(50)
	m_array['Dat_anno']             = ''; // Varchar(4)
	m_array['Dat_numdisc']          = ''; // Varchar(50)
	m_array['Dat_sistemaop']        = ''; // Varchar(200)
	m_array['Dat_tamano']           = ''; // Varchar(50)
	m_array['Dat_graficos']         = ''; // Integer
	m_array['Dat_sonido']           = ''; // Integer
	m_array['Dat_jugabilidad']      = ''; // Integer
	m_array['Dat_original']         = ''; // Varchar(5)
	m_array['Dat_estado']           = ''; // Varchar(255)
	m_array['Dat_fecha']            = ''; // Varchar(50)
	m_array['Dat_tipo_emu']         = ''; // Varchar(255)
	m_array['Dat_comentario']       = ''; // LongText
	m_array['Dat_favorito']         = ''; // Varchar(5)
	m_array['Dat_rating']           = ''; // Integer
	m_array['Dat_edad_recomendada'] = ''; // Varchar(2)
	m_array['Dat_usuario']          = ''; // Varchar(255)
	m_array['Dat_path_exe']         = ''; // Varchar(255)
	m_array['Dat_parametros_exe']   = ''; // Varchar(255)
	m_array['Dat_path_capturas']    = ''; // Varchar(255)
	m_array['Dat_path_setup']       = ''; // Varchar(255)
	m_array['Dat_parametros_setup'] = ''; // Varchar(255)

	m_array['Dat_thumbs']           = ''; // Varchar(255)
	m_array['Dat_cover_front']      = ''; // Varchar(255)
	m_array['Dat_cover_back']       = ''; // Varchar(255)
	m_array['Dat_url_cover_thumbs'] = ''; // Varchar(255)
	m_array['Dat_url_cover_front']  = ''; // Varchar(255)
	m_array['Dat_url_cover_back']   = ''; // Varchar(255)

/*
	Si la caratula frontal o trasera esta en otra página 
	'Dat_covers_other_page' debe estar como 'true' en caso contrario 'false'

-- In English --
	If the front or back cover is on another page
	'Dat_covers_other_page' should be like 'true' otherwise 'false'
*/
	m_array['Dat_covers_other_page'] = 'true'; // true|false
	m_array['Dat_url_all_covers']    = ''; // url de todas las caratulas / url of all the covers

	return m_array;
}

function AnalyzeGameCoverPage(texto)
{
	var m_array = new Array();

	m_array['Dat_cover_back']             = ''; // Varchar(255)
	m_array['Dat_cover_back_url']         = ''; // Varchar(255)
	m_array['Dat_cover_back_url_small']   = ''; // Varchar(255)
	m_array['Dat_cover_left']             = ''; // Varchar(255)
	m_array['Dat_cover_left_url']         = ''; // Varchar(255)
	m_array['Dat_cover_left_url_small']   = ''; // Varchar(255)
	m_array['Dat_cover_right']            = ''; // Varchar(255)
	m_array['Dat_cover_right_url']        = ''; // Varchar(255)
	m_array['Dat_cover_right_url_small']  = ''; // Varchar(255)
	m_array['Dat_cover_top']              = ''; // Varchar(255)
	m_array['Dat_cover_top_url']          = ''; // Varchar(255)
	m_array['Dat_cover_top_url_small']    = ''; // Varchar(255)
	m_array['Dat_cover_bottom']           = ''; // Varchar(255)
	m_array['Dat_cover_bottom_url']       = ''; // Varchar(255)
	m_array['Dat_cover_bottom_url_small'] = ''; // Varchar(255)

	m_array['Dat_all_covers'] = 'img_name|img_url_full|img_type|img_url_small';

	return m_array;
}

function AnalyzeGameMoreCoverPage(texto)
{
	var m_array = new Array();

	m_array['Dat_all_covers'] = 'img_name|img_url_full|img_type|img_url_small';

	return m_array;
}

// Funciones Extras -----------------------------
function AnalyzeTagName(texto, stRegExp, indxExp, stDefault)
{
	if (texto != '')
	{
		myRE = new RegExp(stRegExp);
		results = texto.match(myRE);
		if( results != null )
			return results[indxExp];
		else
			return stDefault;
	} else
		return stDefault;
}
