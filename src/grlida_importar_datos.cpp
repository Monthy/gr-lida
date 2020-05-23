#include "grlida_importar_datos.h"
#include "ui_importar_datos.h"

frmImportarDatos::frmImportarDatos(stGrlCfg m_cfg, stGrlCats m_categoria, stDatosJuego datos, bool m_editando, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::frmImportarDatos)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg     = m_cfg;
	categoria  = m_categoria;
	Editando   = m_editando;
	isLoad     = false;
	DatosJuego = datos;

	grlDir.Home   = fGrl->dirApp();
	grlDir.Datos  = grlDir.Home +"datos/";
	grlDir.Iconos = grlDir.Home +"iconos/";

	cargarConfig();
}

frmImportarDatos::~frmImportarDatos()
{
	delete ui;
}

void frmImportarDatos::cargarConfig()
{
	cargarEditorDatos();
	isLoad = true;
}

void frmImportarDatos::cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col, QString sep, bool isClear, bool keyVisible)
{
	QList<stGrlDatos> list = fGrl->cargaListaDatos(archivo, num_col, sep);
	QString key_visible;

	if (isClear)
		l_data.clear();

	const int listSize = list.size();
	for (int i = 0; i < listSize; ++i)
	{
		key_visible = keyVisible ? "("+ list.at(i).key +") - " : "";
		l_data.insert(i, ListDataItem(QIcon(fGrl->theme() + dirImg + list.at(i).icono), key_visible + list.at(i).titulo, list.at(i).key));
	}
}

void frmImportarDatos::cargarEditorDatos()
{
	ListData lista;
	int idx_select = -1;

// Lista iconos
	lista.clear();
	icon_list.clear();
	icon_list = fGrl->getListFiles(fGrl->theme() +"img24/cat/", grlCfg.FormatsImage);
	const int listSize = icon_list.size();
	for (int i = 0; i < listSize; ++i)
		lista.insert(i, ListDataItem(QIcon(icon_list.at(i).absoluteFilePath()), icon_list.at(i).completeBaseName(), icon_list.at(i).fileName()));

	PropertyHeader *p_head_dat = new PropertyHeader(tr("Datos del juego"));
	p_head_dat->setSizeHint(0, QSize(30, 30));

// Icono
	pdat_icono = new ComboBoxProperty(p_head_dat, tr("Icono"), lista, 0, 0, QSize(24, 24));
	pdat_icono->setKey("Dat_icono");
	pdat_icono->setChecked(true);
//	pdat_icono->setMinimumHeight(32);
	idx_select = pdat_icono->indexOfData(DatosJuego.icono);
	pdat_icono->setIndex(idx_select);
	pdat_icono->setDefaultIndex(idx_select);
// Título
	pdat_titulo = new StringProperty(p_head_dat, tr("Título"), DatosJuego.titulo, DatosJuego.titulo);
	pdat_titulo->setKey("Dat_titulo");
	pdat_titulo->setChecked(true);
// Subtítulo
	pdat_subtitulo = new StringProperty(p_head_dat, tr("Subtítulo"), DatosJuego.subtitulo, DatosJuego.subtitulo);
	pdat_subtitulo->setKey("Dat_subtitulo");
	pdat_subtitulo->setChecked(true);
// Genero
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/generos.txt", "img16/", 2, "|");
	pdat_genero = new ComboBoxCheckProperty(p_head_dat, tr("Genero"), lista, DatosJuego.genero, DatosJuego.genero);
	pdat_genero->setKey("Dat_genero");
	pdat_genero->setChecked(true);
// Compania
	cargarListDataItem(lista, grlDir.Datos +"companias.txt", "img16/", 2, "|");
	pdat_compania = new ComboBoxCheckProperty(p_head_dat, tr("Publicado por"), lista, DatosJuego.compania, DatosJuego.compania);
	pdat_compania->setKey("Dat_compania");
	pdat_compania->setChecked(true);
// Desarrollador
	pdat_desarrollador = new ComboBoxCheckProperty(p_head_dat, tr("Desarrollado por"), lista, DatosJuego.desarrollador, DatosJuego.desarrollador);
	pdat_desarrollador->setKey("Dat_desarrollador");
	pdat_desarrollador->setChecked(true);
// Tema
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/temas.txt", "img16/", 2, "|");
	pdat_tema = new ComboBoxCheckProperty(p_head_dat, tr("Tema"), lista, DatosJuego.tema, DatosJuego.tema);
	pdat_tema->setKey("Dat_tema");
	pdat_tema->setChecked(true);
// Grupo
	cargarListDataItem(lista, grlDir.Datos +"grupos.txt", "img16/", 2, "|");
	pdat_grupo = new ComboBoxCheckProperty(p_head_dat, tr("Grupo"), lista, DatosJuego.grupo, DatosJuego.grupo);
	pdat_grupo->setKey("Dat_grupo");
	pdat_grupo->setChecked(true);
// Perspectiva
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/perspectivas.txt", "img16/", 2, "|");
	pdat_perspectiva = new ComboBoxCheckProperty(p_head_dat, tr("Perspectiva"), lista, DatosJuego.perspectiva, DatosJuego.perspectiva);
	pdat_perspectiva->setKey("Dat_perspectiva");
	pdat_perspectiva->setChecked(true);
// Idioma
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/idiomas.txt", "img16/", 3, "|");
	pdat_idioma = new ComboBoxCheckProperty(p_head_dat, tr("Idioma"), lista, DatosJuego.idioma, DatosJuego.idioma);
	pdat_idioma->setKey("Dat_idioma");
	pdat_idioma->setChecked(true);
// IdiomaVoces
	pdat_idioma_voces = new ComboBoxCheckProperty(p_head_dat, tr("Idioma voces"), lista, DatosJuego.idioma_voces, DatosJuego.idioma_voces);
	pdat_idioma_voces->setKey("Dat_idioma_voces");
	pdat_idioma_voces->setChecked(true);
// Formato
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/formatos.txt", "img16/", 2, "|");
	pdat_formato = new ComboBoxCheckProperty(p_head_dat, tr("Formato"), lista, DatosJuego.formato, DatosJuego.formato);
	pdat_formato->setKey("Dat_formato");
	pdat_formato->setChecked(true);
// Anno
//	cargarListDataItem(lista, grlDir.Datos +"fechas.txt", "img16/", 2, "|");
	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/sinimg.png"),  "", "");

	const QStringList fechas = fGrl->listaFechas();
	const int listFechasSize = fechas.size();
	for (int i = 0; i < listFechasSize; ++i)
		lista << ListDataItem(QIcon(fGrl->theme() +"img16/fecha.png"), fechas.at(i), fechas.at(i));

	pdat_anno = new ComboBoxProperty(p_head_dat, tr("Publicado"), lista);
	pdat_anno->setKey("Dat_anno");
	pdat_anno->setChecked(true);
	idx_select = pdat_anno->indexOfData(DatosJuego.anno);
	pdat_anno->setIndex(idx_select);
	pdat_anno->setDefaultIndex(idx_select);
// NumDisc
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/numdisc.txt", "img16/", 2, "|");
	pdat_numdisc = new ComboBoxProperty(p_head_dat, tr("Nº discos"), lista);
	pdat_numdisc->setKey("Dat_numdisc");
	pdat_numdisc->setChecked(true);
	idx_select = pdat_numdisc->indexOfData(DatosJuego.numdisc);
	pdat_numdisc->setIndex(idx_select);
	pdat_numdisc->setDefaultIndex(idx_select);
// SistemaOp
	cargarListDataItem(lista, grlDir.Datos +"sistemas.txt", "img16/", 2, "|");
	pdat_sistemaop = new ComboBoxCheckProperty(p_head_dat, tr("Sistema operativo"), lista, DatosJuego.sistemaop, DatosJuego.sistemaop);
	pdat_sistemaop->setKey("Dat_sistemaop");
	pdat_sistemaop->setChecked(true);
// Tamaño
	pdat_tamano = new StringProperty(p_head_dat, tr("Tamaño"), DatosJuego.tamano, DatosJuego.tamano);
	pdat_tamano->setKey("Dat_tamano");
	pdat_tamano->setChecked(true);
// Graficos
	cargarListDataItem(lista, ":datos/puntuacion.txt", "img16/", 2, "|");
	pdat_graficos = new ComboBoxProperty(p_head_dat, tr("Gráficos"), lista, DatosJuego.graficos.toInt(), DatosJuego.graficos.toInt());
	pdat_graficos->setKey("Dat_graficos");
	pdat_graficos->setChecked(true);
// Sonido
	pdat_sonido = new ComboBoxProperty(p_head_dat, tr("Sonido"), lista, DatosJuego.sonido.toInt(), DatosJuego.sonido.toInt());
	pdat_sonido->setKey("Dat_sonido");
	pdat_sonido->setChecked(true);
// Jugabilidad
	pdat_jugabilidad = new ComboBoxProperty(p_head_dat, tr("Jugabilidad"), lista, DatosJuego.jugabilidad.toInt(), DatosJuego.jugabilidad.toInt());
	pdat_jugabilidad->setKey("Dat_jugabilidad");
	pdat_jugabilidad->setChecked(true);
// Original
	pdat_original = new BoolProperty(p_head_dat, tr("Original"), fGrl->strToBool(DatosJuego.original), fGrl->strToBool(DatosJuego.original), tr("Si"), tr("No"));
	pdat_original->setKey("Dat_original");
	pdat_original->setChecked(true);
// Estado
	pdat_estado = new StringProperty(p_head_dat, tr("Estado"), DatosJuego.estado, DatosJuego.estado);
	pdat_estado->setKey("Dat_estado");
	pdat_estado->setChecked(true);
// Captura o thumbs
	pdat_thumbs = new FileDialogProperty(p_head_dat, tr("Captura/Pantallazo"), DatosJuego.thumbs, DatosJuego.thumbs);
	pdat_thumbs->setKey("Dat_thumbs");
//	pdat_thumbs->setDisabled(true);
	pdat_thumbs->setChecked(true);
// Carátula frontal
	pdat_cover_front = new FileDialogProperty(p_head_dat, tr("Carátula frontal"), DatosJuego.cover_front, DatosJuego.cover_front);
	pdat_cover_front->setKey("Dat_cover_front");
//	pdat_cover_front->setDisabled(true);
	pdat_cover_front->setChecked(true);
// Carátula trasera
	pdat_cover_back = new FileDialogProperty(p_head_dat, tr("Carátula trasera"), DatosJuego.cover_back, DatosJuego.cover_back);
	pdat_cover_back->setKey("Dat_cover_back");
//	pdat_cover_back->setDisabled(true);
	pdat_cover_back->setChecked(true);
// Carátula lateral izquierdo
	pdat_cover_left = new FileDialogProperty(p_head_dat, tr("Carátula lateral izquierdo"), DatosJuego.cover_left, DatosJuego.cover_left);
	pdat_cover_left->setKey("Dat_cover_left");
//	pdat_cover_left->setDisabled(true);
	pdat_cover_left->setChecked(true);
// Carátula lateral derecho
	pdat_cover_right = new FileDialogProperty(p_head_dat, tr("Carátula lateral derecho"), DatosJuego.cover_right, DatosJuego.cover_right);
	pdat_cover_right->setKey("Dat_cover_right");
//	pdat_cover_right->setDisabled(true);
	pdat_cover_right->setChecked(true);
// Carátula superior
	pdat_cover_top = new FileDialogProperty(p_head_dat, tr("Carátula superior"), DatosJuego.cover_top, DatosJuego.cover_top);
	pdat_cover_top->setKey("Dat_cover_top");
//	pdat_cover_top->setDisabled(true);
	pdat_cover_top->setChecked(true);
// Carátula inferior
	pdat_cover_bottom = new FileDialogProperty(p_head_dat, tr("Carátula inferior"), DatosJuego.cover_bottom, DatosJuego.cover_bottom);
	pdat_cover_bottom->setKey("Dat_cover_bottom");
//	pdat_cover_bottom->setDisabled(true);
	pdat_cover_bottom->setChecked(true);
// Tipo Emu
	QList<stGrlDatos> list = fGrl->cargaListaDatos(grlDir.Datos +"emu_list.txt", 4, "|");
	const int listEmuSize = list.size();
	for (int i = 0; i < listEmuSize; ++i)
	{
		if (QFile::exists(fGrl->theme() +"img16/"+ list.at(i).icono))
			lista.insert(i, ListDataItem(QIcon(fGrl->theme() +"img16/"+ list.at(i).icono), list.at(i).titulo, list.at(i).key));
		else
			lista.insert(i, ListDataItem(QIcon(":/img16/sinimg.png"), list.at(i).titulo, list.at(i).key));
	}
	lista.insert(0, ListDataItem(QIcon(fGrl->theme() +"img16/cat/datos.png"), tr("Datos"), "datos"));
	lista.insert(1, ListDataItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), tr("DOSBox"), "dosbox"));
	lista.insert(2, ListDataItem(QIcon(fGrl->theme() +"img16/cat/scummvm.png"), tr("ScummVM"), "scummvm"));
	lista.insert(3, ListDataItem(QIcon(fGrl->theme() +"img16/cat/vdmsound.png"), tr("VDMSound"), "vdmsound"));

	pdat_tipo_emu = new ComboBoxProperty(p_head_dat, tr("Tipo Emu"), lista);
	pdat_tipo_emu->setKey("Dat_tipo_emu");
	pdat_tipo_emu->setChecked(true);
	idx_select = pdat_tipo_emu->indexOfData(DatosJuego.tipo_emu);
	pdat_tipo_emu->setIndex(idx_select);
	pdat_tipo_emu->setDefaultIndex(idx_select);
	pdat_tipo_emu->setDisabled(true);
// Descripción
	pdat_comentario = new StringProperty(p_head_dat, tr("Descripción"), DatosJuego.comentario, DatosJuego.comentario);
	pdat_comentario->setKey("Dat_comentario");
	pdat_comentario->setChecked(true);
	pdat_comentario->setTextMultiLine(true);
// Favorito
	pdat_favorito = new BoolProperty(p_head_dat, tr("Favorito"), fGrl->strToBool(DatosJuego.favorito), fGrl->strToBool(DatosJuego.favorito), tr("Si"), tr("No"));
	pdat_favorito->setKey("Dat_favorito");
	pdat_favorito->setChecked(true);
// GamePad
	pdat_gamepad = new BoolProperty(p_head_dat, tr("Joystick") +"/"+ tr("GamePad"), fGrl->strToBool(DatosJuego.gamepad), fGrl->strToBool(DatosJuego.gamepad), tr("Si"), tr("No"));
	pdat_gamepad->setKey("Dat_gamepad");
	pdat_gamepad->setChecked(true);
// Rating
	cargarListDataItem(lista, ":datos/rating.txt", "img16/", 2, "|");
	pdat_rating = new ComboBoxProperty(p_head_dat, tr("Rating"), lista, DatosJuego.rating.toInt(), DatosJuego.rating.toInt());
	pdat_rating->setKey("Dat_rating");
	pdat_rating->setChecked(true);
// EdadRecomendada
	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/edad_recomendada.txt", "img16/", 3, "|");
	pdat_edad_recomendada = new ComboBoxProperty(p_head_dat, tr("Edad recomendada"), lista);
	pdat_edad_recomendada->setKey("Dat_edad_recomendada");
	pdat_edad_recomendada->setChecked(true);
	idx_select = pdat_edad_recomendada->indexOfData(DatosJuego.edad_recomendada);
	pdat_edad_recomendada->setIndex(idx_select);
	pdat_edad_recomendada->setDefaultIndex(idx_select);
// Usuario
	pdat_usuario = new StringProperty(p_head_dat, tr("Subido por"), DatosJuego.usuario, DatosJuego.usuario);
	pdat_usuario->setKey("Dat_usuario");
	pdat_usuario->setChecked(true);
// Path
	PropertyHeader *p_head_path = new PropertyHeader("Config - Path");

	pdat_path_exe = new FileDialogProperty(p_head_path, tr("Ruta del ejecutable"), DatosJuego.path_exe, DatosJuego.path_exe);
	pdat_path_exe->setKey("Dat_path_exe");
	pdat_path_exe->setChecked(true);

	pdat_parametros_exe = new StringProperty(p_head_path, tr("Parametros del ejecutable"), DatosJuego.parametros_exe, DatosJuego.parametros_exe);
	pdat_parametros_exe->setKey("Dat_parametros_exe");
	pdat_parametros_exe->setChecked(true);

	pdat_path_setup = new FileDialogProperty(p_head_path, tr("Ruta del setup"), DatosJuego.path_setup, DatosJuego.path_setup);
	pdat_path_setup->setKey("Dat_path_setup");
	pdat_path_setup->setChecked(true);

	pdat_parametros_setup = new StringProperty(p_head_path, tr("Parametros del setup"), DatosJuego.parametros_setup, DatosJuego.parametros_setup);
	pdat_parametros_setup->setKey("Dat_parametros_setup");
	pdat_parametros_setup->setChecked(true);
// --
	ui->twDatos->setIconSize(QSize(30, 30));

	ui->twDatos->addProperty(p_head_dat , "p_head_dat" );
	ui->twDatos->addProperty(p_head_path, "p_head_path");

	ui->twDatos->setRootIsDecorated(false);
	ui->twDatos->adjustToContents();
	connect(ui->twDatos, SIGNAL(valueChanged(PropertyItem*,QVariant)), this, SLOT(onValueDatosChanged(PropertyItem*,QVariant)));
}

void frmImportarDatos::setEditorDatos(stDatosJuego cfgDat, stDatosJuego dat_def)
{
	pdat_icono->setIndex(pdat_icono->indexOfData(cfgDat.icono));
	pdat_icono->setDefaultIndex(pdat_icono->indexOfData(dat_def.icono));

	pdat_titulo->setValue(cfgDat.titulo);
	pdat_titulo->setDefaultValue(dat_def.titulo);

	pdat_subtitulo->setValue(cfgDat.subtitulo);
	pdat_subtitulo->setDefaultValue(dat_def.subtitulo);

	pdat_genero->setValue(cfgDat.genero);
	pdat_genero->setDefaultValue(dat_def.genero);

	pdat_compania->setValue(cfgDat.compania);
	pdat_compania->setDefaultValue(dat_def.compania);

	pdat_desarrollador->setValue(cfgDat.desarrollador);
	pdat_desarrollador->setDefaultValue(dat_def.desarrollador);

	pdat_tema->setValue(cfgDat.tema);
	pdat_tema->setDefaultValue(dat_def.tema);

	pdat_grupo->setValue(cfgDat.grupo);
	pdat_grupo->setDefaultValue(dat_def.grupo);

	pdat_perspectiva->setValue(cfgDat.perspectiva);
	pdat_perspectiva->setDefaultValue(dat_def.perspectiva);

	pdat_idioma->setValue(cfgDat.idioma);
	pdat_idioma->setDefaultValue(dat_def.idioma);

	pdat_idioma_voces->setValue(cfgDat.idioma_voces);
	pdat_idioma_voces->setDefaultValue(dat_def.idioma_voces);

	pdat_formato->setValue(cfgDat.formato);
	pdat_formato->setDefaultValue(dat_def.formato);

	pdat_anno->setIndex(pdat_anno->indexOfData(cfgDat.anno));
	pdat_anno->setDefaultIndex(pdat_anno->indexOfData(dat_def.anno));

	pdat_numdisc->setIndex(pdat_numdisc->indexOfData(cfgDat.numdisc));
	pdat_numdisc->setDefaultIndex(pdat_numdisc->indexOfData(dat_def.numdisc));

	pdat_sistemaop->setValue(cfgDat.sistemaop);
	pdat_sistemaop->setDefaultValue(dat_def.sistemaop);

	pdat_tamano->setValue(cfgDat.tamano);
	pdat_tamano->setDefaultValue(dat_def.tamano);

	pdat_graficos->setIndex(cfgDat.graficos.toInt());
	pdat_graficos->setDefaultIndex(dat_def.graficos.toInt());

	pdat_sonido->setIndex(cfgDat.sonido.toInt());
	pdat_sonido->setDefaultIndex(dat_def.sonido.toInt());

	pdat_jugabilidad->setIndex(cfgDat.jugabilidad.toInt());
	pdat_jugabilidad->setDefaultIndex(dat_def.jugabilidad.toInt());

	pdat_original->setValue(fGrl->strToBool(cfgDat.original));
	pdat_original->setDefaultValue(fGrl->strToBool(dat_def.original));

	pdat_estado->setValue(cfgDat.estado);
	pdat_estado->setDefaultValue(dat_def.estado);

	pdat_thumbs->setValue(cfgDat.thumbs);
	pdat_thumbs->setDefaultValue(dat_def.thumbs);

	pdat_cover_front->setValue(cfgDat.cover_front);
	pdat_cover_front->setDefaultValue(dat_def.cover_front);

	pdat_cover_back->setValue(cfgDat.cover_back);
	pdat_cover_back->setDefaultValue(dat_def.cover_back);

	pdat_cover_left->setValue(cfgDat.cover_left);
	pdat_cover_left->setDefaultValue(dat_def.cover_left);

	pdat_cover_right->setValue(cfgDat.cover_right);
	pdat_cover_right->setDefaultValue(dat_def.cover_right);

	pdat_cover_top->setValue(cfgDat.cover_top);
	pdat_cover_top->setDefaultValue(dat_def.cover_top);

	pdat_cover_bottom->setValue(cfgDat.cover_bottom);
	pdat_cover_bottom->setDefaultValue(dat_def.cover_bottom);

	pdat_tipo_emu->setIndex(pdat_tipo_emu->indexOfData(cfgDat.tipo_emu));
	pdat_tipo_emu->setDefaultIndex(pdat_tipo_emu->indexOfData(dat_def.tipo_emu));

	pdat_comentario->setValue(cfgDat.comentario);
	pdat_comentario->setDefaultValue(dat_def.comentario);

	pdat_favorito->setValue(fGrl->strToBool(cfgDat.favorito));
	pdat_favorito->setDefaultValue(fGrl->strToBool(dat_def.favorito));

	pdat_gamepad->setValue(fGrl->strToBool(cfgDat.gamepad));
	pdat_gamepad->setDefaultValue(fGrl->strToBool(dat_def.gamepad));

	pdat_rating->setIndex(cfgDat.rating.toInt());
	pdat_rating->setDefaultIndex(dat_def.rating.toInt());

	pdat_edad_recomendada->setIndex(pdat_edad_recomendada->indexOfData(cfgDat.edad_recomendada));
	pdat_edad_recomendada->setDefaultIndex(pdat_edad_recomendada->indexOfData(dat_def.edad_recomendada));

	pdat_usuario->setValue(cfgDat.usuario);
	pdat_usuario->setDefaultValue(dat_def.usuario);

	pdat_path_exe->setValue(cfgDat.path_exe);
	pdat_path_exe->setDefaultValue(dat_def.path_exe);

	pdat_parametros_exe->setValue(cfgDat.parametros_exe);
	pdat_parametros_exe->setDefaultValue(dat_def.parametros_exe);

	pdat_path_setup->setValue(cfgDat.path_setup);
	pdat_path_setup->setDefaultValue(dat_def.path_setup);

	pdat_parametros_setup->setValue(cfgDat.parametros_setup);
	pdat_parametros_setup->setDefaultValue(dat_def.parametros_setup);

	DatosJuego = cfgDat;
}

void frmImportarDatos::onValueDatosChanged(PropertyItem *prop, const QVariant &value)
{
	if (prop->getKey() == "Dat_icono")
		DatosJuego.icono = pdat_icono->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dat_titulo")
		DatosJuego.titulo = value.toString();
	if (prop->getKey() == "Dat_subtitulo")
		DatosJuego.subtitulo = value.toString();
	if (prop->getKey() == "Dat_genero")
		DatosJuego.genero = value.toString();
	if (prop->getKey() == "Dat_compania")
		DatosJuego.compania = value.toString();
	if (prop->getKey() == "Dat_desarrollador")
		DatosJuego.desarrollador = value.toString();
	if (prop->getKey() == "Dat_tema")
		DatosJuego.tema = value.toString();
	if (prop->getKey() == "Dat_grupo")
		DatosJuego.grupo = value.toString();
	if (prop->getKey() == "Dat_perspectiva")
		DatosJuego.perspectiva = value.toString();
	if (prop->getKey() == "Dat_idioma")
		DatosJuego.idioma = value.toString();
	if (prop->getKey() == "Dat_idioma_voces")
		DatosJuego.idioma_voces = value.toString();
	if (prop->getKey() == "Dat_formato")
		DatosJuego.formato = value.toString();
	if (prop->getKey() == "Dat_anno")
		DatosJuego.anno = pdat_anno->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dat_numdisc")
		DatosJuego.numdisc = value.toString();
	if (prop->getKey() == "Dat_sistemaop")
		DatosJuego.sistemaop = value.toString();
	if (prop->getKey() == "Dat_tamano")
		DatosJuego.tamano = value.toString();
	if (prop->getKey() == "Dat_graficos")
		DatosJuego.graficos = value.toString();
	if (prop->getKey() == "Dat_sonido")
		DatosJuego.sonido = value.toString();
	if (prop->getKey() == "Dat_jugabilidad")
		DatosJuego.jugabilidad = value.toString();
	if (prop->getKey() == "Dat_original")
		DatosJuego.original = value.toString();
	if (prop->getKey() == "Dat_estado")
		DatosJuego.estado = value.toString();
	if (prop->getKey() == "Dat_thumbs")
		DatosJuego.thumbs = value.toString();
	if (prop->getKey() == "Dat_cover_front")
		DatosJuego.cover_front = value.toString();
	if (prop->getKey() == "Dat_cover_back")
		DatosJuego.cover_back = value.toString();
	if (prop->getKey() == "Dat_cover_left")
		DatosJuego.cover_left = value.toString();
	if (prop->getKey() == "Dat_cover_right")
		DatosJuego.cover_right = value.toString();
	if (prop->getKey() == "Dat_cover_top")
		DatosJuego.cover_top = value.toString();
	if (prop->getKey() == "Dat_cover_bottom")
		DatosJuego.cover_bottom = value.toString();
	if (prop->getKey() == "Dat_tipo_emu")
		DatosJuego.tipo_emu = pdat_tipo_emu->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dat_comentario")
		DatosJuego.comentario = value.toString();
	if (prop->getKey() == "Dat_favorito")
		DatosJuego.favorito = value.toString();
	if (prop->getKey() == "Dat_gamepad")
		DatosJuego.gamepad = value.toString();
	if (prop->getKey() == "Dat_rating")
		DatosJuego.rating = value.toString();
	if (prop->getKey() == "Dat_edad_recomendada")
		DatosJuego.edad_recomendada = pdat_edad_recomendada->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dat_usuario")
		DatosJuego.usuario = value.toString();
	if (prop->getKey() == "Dat_path_exe")
		DatosJuego.path_exe = value.toString();
	if (prop->getKey() == "Dat_parametros_exe")
		DatosJuego.parametros_exe = value.toString();
	if (prop->getKey() == "Dat_path_setup")
		DatosJuego.path_setup = value.toString();
	if (prop->getKey() == "Dat_parametros_setup")
		DatosJuego.parametros_setup = value.toString();
//	if (prop->getKey() == "Dat_path_capturas")
//		DatosJuego.path_capturas = value.toString();

	emit cfgDatChanged(DatosJuego);
}

void frmImportarDatos::on_twDatos_itemChanged(QTreeWidgetItem *item, int column)
{
	if (item && column == 0 && isLoad)
	{
		DatosJuego.isChecked.icono            = pdat_icono->isChecked();
		DatosJuego.isChecked.titulo           = pdat_titulo->isChecked();
		DatosJuego.isChecked.subtitulo        = pdat_subtitulo->isChecked();
		DatosJuego.isChecked.genero           = pdat_genero->isChecked();
		DatosJuego.isChecked.compania         = pdat_compania->isChecked();
		DatosJuego.isChecked.desarrollador    = pdat_desarrollador->isChecked();
		DatosJuego.isChecked.tema             = pdat_tema->isChecked();
		DatosJuego.isChecked.grupo            = pdat_grupo->isChecked();
		DatosJuego.isChecked.perspectiva      = pdat_perspectiva->isChecked();
		DatosJuego.isChecked.idioma           = pdat_idioma->isChecked();
		DatosJuego.isChecked.idioma_voces     = pdat_idioma_voces->isChecked();
		DatosJuego.isChecked.formato          = pdat_formato->isChecked();
		DatosJuego.isChecked.anno             = pdat_anno->isChecked();
		DatosJuego.isChecked.numdisc          = pdat_numdisc->isChecked();
		DatosJuego.isChecked.sistemaop        = pdat_sistemaop->isChecked();
		DatosJuego.isChecked.tamano           = pdat_tamano->isChecked();
		DatosJuego.isChecked.graficos         = pdat_graficos->isChecked();
		DatosJuego.isChecked.sonido           = pdat_sonido->isChecked();
		DatosJuego.isChecked.jugabilidad      = pdat_jugabilidad->isChecked();
		DatosJuego.isChecked.original         = pdat_original->isChecked();
		DatosJuego.isChecked.estado           = pdat_estado->isChecked();
		DatosJuego.isChecked.thumbs           = pdat_thumbs->isChecked();
		DatosJuego.isChecked.cover_front      = pdat_cover_front->isChecked();
		DatosJuego.isChecked.cover_back       = pdat_cover_back->isChecked();
		DatosJuego.isChecked.cover_left       = pdat_cover_left->isChecked();
		DatosJuego.isChecked.cover_right      = pdat_cover_right->isChecked();
		DatosJuego.isChecked.cover_top        = pdat_cover_top->isChecked();
		DatosJuego.isChecked.cover_bottom     = pdat_cover_bottom->isChecked();
	//	DatosJuego.isChecked.tipo_emu         = pdat_tipo_emu->isChecked();
		DatosJuego.isChecked.comentario       = pdat_comentario->isChecked();
		DatosJuego.isChecked.favorito         = pdat_favorito->isChecked();
		DatosJuego.isChecked.gamepad          = pdat_gamepad->isChecked();
		DatosJuego.isChecked.rating           = pdat_rating->isChecked();
		DatosJuego.isChecked.edad_recomendada = pdat_edad_recomendada->isChecked();
		DatosJuego.isChecked.usuario          = pdat_usuario->isChecked();
		DatosJuego.isChecked.path_exe         = pdat_path_exe->isChecked();
		DatosJuego.isChecked.parametros_exe   = pdat_parametros_exe->isChecked();
		DatosJuego.isChecked.path_setup       = pdat_path_setup->isChecked();
		DatosJuego.isChecked.parametros_setup = pdat_parametros_setup->isChecked();

		emit cfgDatChanged(DatosJuego);
	}
}
