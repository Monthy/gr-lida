#include "grlida_importar_scummvm.h"
#include "ui_importar_scummvm.h"

frmImportarScummVM::frmImportarScummVM(stGrlCfg m_cfg, stGrlCats m_categoria, stConfigScummVM datos_svm, bool m_editando, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::frmImportarScummVM)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg    = m_cfg;
	categoria = m_categoria;
	Editando  = m_editando;

	DatosScummVM = datos_svm;

	grlDir.Home  = fGrl->dirApp();
	grlDir.Datos = grlDir.Home +"datos/";

	cargarConfig();
}

frmImportarScummVM::~frmImportarScummVM()
{
	delete ui;
}

void frmImportarScummVM::cargarConfig()
{
	cargarEditorScummVM();
}

void frmImportarScummVM::cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col, QString sep, bool isClear, bool keyVisible)
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

void frmImportarScummVM::cargarEditorScummVM()
{
	ListData lista_svm, lista;
	int idx_select = -1;
//	psvm_emu_svmpath = new FileDialogProperty(p_head_opt_sonido, tr(""), "", ""); //Svm_emu_svmpath

	PropertyHeader *p_head_lista_games = new PropertyHeader(tr("Lista juegos"));
	p_head_lista_games->setSizeHint(0, QSize(30, 30));

	cargarListDataItem(lista_svm, grlDir.Datos +"svm_lista.txt", "img_svm/", 4, "|");
	psvm_list_svm = new ComboBoxProperty(p_head_lista_games, tr("Seleccionar juegos"), lista_svm, 0, 0, QSize(24, 24));
	psvm_list_svm->setKey("psvm_list_svm");
//	psvm_list_svm->setMinimumHeight(32);

	PropertyHeader *p_head_opt_general = new PropertyHeader(tr("Opciones generales"));

	psvm_gameid = new StringProperty(p_head_opt_general, tr("GameID:"), "", "");
	psvm_gameid->setKey("Svm_game");

	psvm_game_label = new StringProperty(p_head_opt_general, tr("Etiqueta:"), "", "");
	psvm_game_label->setKey("Svm_game_label");

	psvm_description = new StringProperty(p_head_opt_general, tr("Descripción"), "", "");
	psvm_description->setKey("Svm_description");

	psvm_boot_param  = new StringProperty(p_head_opt_general, tr("Boot param"), "", "");
	psvm_boot_param->setKey("Svm_boot_param");

	psvm_path = new FileDialogProperty(p_head_opt_general, tr("Directorio:"), "", "");
	psvm_path->setKey("Svm_path");

	psvm_path_save = new FileDialogProperty(p_head_opt_general, tr("Guardar partida:"), "", "");
	psvm_path_save->setKey("Svm_path_save");

	psvm_path_extra = new FileDialogProperty(p_head_opt_general, tr("Directorio extra:"), "", "");
	psvm_path_extra->setKey("Svm_path_extra");

	cargarListDataItem(lista, grlDir.Datos + fGrl->idioma() +"/svm_idioma.txt", "img16/", 3, "|", true, true);
	psvm_language = new ComboBoxProperty(p_head_opt_general, tr("Idioma:"), lista);
	psvm_language->setKey("Svm_language");
	idx_select = psvm_language->indexOfData("es");
	psvm_language->setIndex(idx_select);
	psvm_language->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"svm_platform.txt", "img16/", 3, "|");
	psvm_platform = new ComboBoxProperty(p_head_opt_general, tr("Plataforma:"), lista);
	psvm_platform->setKey("Svm_platform");
	idx_select = psvm_platform->indexOfData("pc");
	psvm_platform->setIndex(idx_select);
	psvm_platform->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"svm_gfxmode.txt", "img16/", 3, "|");
	psvm_gfx_mode = new ComboBoxProperty(p_head_opt_general, tr("Modo gráfico:"), lista);
	psvm_gfx_mode->setKey("Svm_gfx_mode");
	idx_select = psvm_gfx_mode->indexOfData("2x");
	psvm_gfx_mode->setIndex(idx_select);
	psvm_gfx_mode->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"svm_render_mode.txt", "img16/", 3, "|");
	psvm_render_mode = new ComboBoxProperty(p_head_opt_general, tr("Modo de render:"), lista);
	psvm_render_mode->setKey("Svm_render_mode");
	idx_select = psvm_render_mode->indexOfData("");
	psvm_render_mode->setIndex(idx_select);
	psvm_render_mode->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 0", "0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 1", "1")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 2", "2")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 3", "3")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/drive_cdrom.png"), "CD Index 4", "4");
	psvm_cdrom = new ComboBoxProperty(p_head_opt_general, tr("Lector de CD:"), lista);
	psvm_cdrom->setKey("Svm_cdrom");
	idx_select = psvm_cdrom->indexOfData("0");
	psvm_cdrom->setIndex(idx_select);
	psvm_cdrom->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/controller.png"), tr("Sin Joystick"), "")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/controller.png"), "Joystick 1", "0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/controller.png"), "Joystick 2", "1");
	psvm_joystick_num = new ComboBoxProperty(p_head_opt_general, tr("Joystick:"), lista);
	psvm_joystick_num->setKey("Svm_joystick_num");
	idx_select = psvm_cdrom->indexOfData("0");
	psvm_cdrom->setIndex(idx_select);
	psvm_cdrom->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/sinimg.png"), tr("Nunca"), "0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 5 minutos"), "300")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 10 minutos"), "600")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 15 minutos"), "900")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/floppy_1.png"), tr("Cada 30 minutos"), "1800");
	psvm_autosave_period = new ComboBoxProperty(p_head_opt_general, tr("Autoguardado:"), lista);
	psvm_autosave_period->setKey("Svm_autosave_period");
	idx_select = psvm_autosave_period->indexOfData("300");
	psvm_autosave_period->setIndex(idx_select);
	psvm_autosave_period->setDefaultIndex(idx_select);

	psvm_subtitles = new BoolProperty(p_head_opt_general, tr("Mostras subtítulo"), false, false);
	psvm_subtitles->setKey("Svm_subtitles");

	psvm_fullscreen = new BoolProperty(p_head_opt_general, tr("Pantalla completa"), false, false);
	psvm_fullscreen->setKey("Svm_fullscreen");

	psvm_filtering = new BoolProperty(p_head_opt_general, tr("Filtro de gráficos"), false, false);
	psvm_filtering->setKey("Svm_filtering");

	psvm_aspect_ratio = new BoolProperty(p_head_opt_general, tr("Corrección de aspecto"), false, false);
	psvm_aspect_ratio->setKey("Svm_aspect_ratio");

	psvm_copy_protection = new BoolProperty(p_head_opt_general, tr("Copy-Protection"), false, false);
	psvm_copy_protection->setKey("Svm_copy_protection");

	psvm_alt_intro = new BoolProperty(p_head_opt_general, tr("Intro alternativa"), false, false);
	psvm_alt_intro->setKey("Svm_alt_intro");

	psvm_disable_dithering = new BoolProperty(p_head_opt_general, tr("Difuminado EGA"), false, false);
	psvm_disable_dithering->setKey("Svm_disable_dithering");

	psvm_originalsaveload = new BoolProperty(p_head_opt_general, tr("Pantalla guardar/cargar originales"), false, false);
	psvm_originalsaveload->setKey("Svm_originalsaveload");

	psvm_bright_palette = new BoolProperty(p_head_opt_general, tr("Paleta original"), false, false);
	psvm_bright_palette->setKey("Svm_bright_palette");
// --
	PropertyHeader *p_head_opt_sonido = new PropertyHeader(tr("Opciones de sonido"));

	cargarListDataItem(lista, grlDir.Datos +"svm_music_driver.txt", "img16/", 3, "|");
	psvm_music_driver = new ComboBoxProperty(p_head_opt_sonido, tr("Modo de audio:") , lista);
	psvm_music_driver->setKey("Svm_music_driver");
	idx_select = psvm_music_driver->indexOfData("adlib");
	psvm_music_driver->setIndex(idx_select);
	psvm_music_driver->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "<defecto>", "auto")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/img_advmame2x_3x.png"), "MAME OPL emulator", "mame")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/cat/dosbox.png"), "DOSBox OPL emulator (experimental)", "db");
	psvm_opl_driver = new ComboBoxProperty(p_head_opt_sonido, tr("AdLib emulator:"), lista);
	psvm_opl_driver->setKey("Svm_opl_driver");
	idx_select = psvm_opl_driver->indexOfData("auto");
	psvm_opl_driver->setIndex(idx_select);
	psvm_opl_driver->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("<defecto>", "") << ListDataItem("8000", "8000") << ListDataItem("11025", "11025")
		<< ListDataItem("22050", "22050") << ListDataItem("44100", "44100") << ListDataItem("48000", "48000");
	psvm_output_rate = new ComboBoxProperty(p_head_opt_sonido, tr("Frecuencia:"), lista);
	psvm_output_rate->setKey("Svm_output_rate");
	idx_select = psvm_output_rate->indexOfData("");
	psvm_output_rate->setIndex(idx_select);
	psvm_output_rate->setDefaultIndex(idx_select);

	psvm_sound_font = new FileDialogProperty(p_head_opt_sonido, tr("SoundFont:"), "", "");
	psvm_sound_font->setKey("Svm_sound_font");

	psvm_music_volume = new IntegerProperty(p_head_opt_sonido, tr("Volumen música:"), 192, 192, 0, 256);
	psvm_music_volume->setKey("Svm_music_volume");

	psvm_sfx_volume = new IntegerProperty(p_head_opt_sonido, tr("Volumen SFX:"), 192, 192, 0, 256);
	psvm_sfx_volume->setKey("Svm_sfx_volume");

	psvm_speech_volume = new IntegerProperty(p_head_opt_sonido, tr("Volumen voces:"), 192, 192, 0, 256);
	psvm_speech_volume->setKey("Svm_speech_volume");

	psvm_tempo = new IntegerProperty(p_head_opt_sonido, tr("Tiempo de la música:"), 100, 100, 50, 200);
	psvm_tempo->setKey("Svm_tempo");

	psvm_talkspeed = new IntegerProperty(p_head_opt_sonido, tr("Velocidad de subtítulos:"), 60, 60, 0, 255);
	psvm_talkspeed->setKey("Svm_talkspeed");

	psvm_debuglevel = new IntegerProperty(p_head_opt_sonido, tr("Nivel de errores:"), 0, 0, 0, 10);
	psvm_debuglevel->setKey("Svm_debuglevel");

	psvm_midi_gain = new IntegerProperty(p_head_opt_sonido, tr("Midi Gain:"), 100, 100, 0, 1000);
	psvm_midi_gain->setKey("Svm_midi_gain");

	psvm_walkspeed = new IntegerProperty(p_head_opt_sonido, tr("Walk Speed:"), 0, 0, 0, 4);
	psvm_walkspeed->setKey("Svm_walkspeed");

//	psvm_master_volume = new IntegerProperty(p_head_opt_sonido, tr("Volumen maestro:"), 192, 192, 0, 256);
//	psvm_master_volume->setKey("Svm_master_volume");

	psvm_mute = new BoolProperty(p_head_opt_sonido, tr("Silenciar"), false, false);
	psvm_mute->setKey("Svm_mute");

	psvm_speech_mute = new BoolProperty(p_head_opt_sonido, tr("Desactivar voces"), false, false);
	psvm_speech_mute->setKey("Svm_speech_mute");

	psvm_multi_midi = new BoolProperty(p_head_opt_sonido, tr("Mixed Adlib / MIDI mode"), false, false);
	psvm_multi_midi->setKey("Svm_multi_midi");

	psvm_native_mt32 = new BoolProperty(p_head_opt_sonido, tr("True Roland MT-32 (diseable GM emulation)"), false, false);
	psvm_native_mt32->setKey("Svm_native_mt32");

	psvm_enable_gs = new BoolProperty(p_head_opt_sonido, tr("Permitir el modo de Roland GS"), false, false);
	psvm_enable_gs->setKey("Svm_enable_gs");
// --
	ui->twScummVM->setIconSize(QSize(30, 30));

	ui->twScummVM->addProperty(p_head_lista_games, "Svm_p_head_lista_games");
	ui->twScummVM->addProperty(p_head_opt_general, "Svm_p_head_opt_genera" );
	ui->twScummVM->addProperty(p_head_opt_sonido , "Svm_p_head_opt_sonido" );

	ui->twScummVM->setRootIsDecorated(false);
	ui->twScummVM->adjustToContents();
	connect(ui->twScummVM, SIGNAL(valueChanged(PropertyItem*,QVariant)), this, SLOT(onValueScummVMChanged(PropertyItem*,QVariant)));
}

void frmImportarScummVM::setEditorScummVM(stConfigScummVM cfgSvm)
{
	psvm_list_svm->setIndex(psvm_list_svm->indexOfData(cfgSvm.game));
// Opciones generales
	psvm_gameid->setValue(cfgSvm.game);
	psvm_game_label->setValue(cfgSvm.game_label);
	psvm_description->setValue(cfgSvm.description);
	psvm_boot_param->setValue(cfgSvm.boot_param);
	psvm_path->setValue(cfgSvm.path.replace("{DirBaseGames}", grlCfg.DirBaseGames));
	psvm_path_save->setValue(cfgSvm.path_save.replace("{DirBaseGames}", grlCfg.DirBaseGames));
	psvm_path_extra->setValue(cfgSvm.path_extra.replace("{DirBaseGames}", grlCfg.DirBaseGames));
	psvm_language->setIndex(psvm_language->indexOfData(cfgSvm.language));
	psvm_platform->setIndex(psvm_platform->indexOfData(cfgSvm.platform));
	psvm_gfx_mode->setIndex(psvm_gfx_mode->indexOfData(cfgSvm.gfx_mode));
	psvm_render_mode->setIndex(psvm_render_mode->indexOfData(cfgSvm.render_mode));
	psvm_cdrom->setIndex(psvm_cdrom->indexOfData(cfgSvm.cdrom));
	psvm_joystick_num->setIndex(psvm_joystick_num->indexOfData(cfgSvm.joystick_num));
	psvm_autosave_period->setIndex(psvm_autosave_period->indexOfData(cfgSvm.autosave_period));
	psvm_subtitles->setValue(fGrl->strToBool(cfgSvm.subtitles));
	psvm_fullscreen->setValue(fGrl->strToBool(cfgSvm.fullscreen));
	psvm_filtering->setValue(fGrl->strToBool(cfgSvm.filtering));
	psvm_aspect_ratio->setValue(fGrl->strToBool(cfgSvm.aspect_ratio));
	psvm_copy_protection->setValue(fGrl->strToBool(cfgSvm.copy_protection));
	psvm_alt_intro->setValue(fGrl->strToBool(cfgSvm.alt_intro));
	psvm_disable_dithering->setValue(fGrl->strToBool(cfgSvm.disable_dithering));
	psvm_originalsaveload->setValue(fGrl->strToBool(cfgSvm.originalsaveload));
	psvm_bright_palette->setValue(fGrl->strToBool(cfgSvm.bright_palette));
// Opciones de sonido
	psvm_music_driver->setIndex(psvm_music_driver->indexOfData(cfgSvm.music_driver));
	psvm_opl_driver->setIndex(psvm_opl_driver->indexOfData(cfgSvm.opl_driver));
	psvm_output_rate->setIndex(psvm_output_rate->indexOfData(cfgSvm.output_rate));
	psvm_sound_font->setValue(cfgSvm.sound_font);
	psvm_music_volume->setValue(cfgSvm.music_volume.toInt());
	psvm_sfx_volume->setValue(cfgSvm.sfx_volume.toInt());
	psvm_speech_volume->setValue(cfgSvm.speech_volume.toInt());
	psvm_tempo->setValue(cfgSvm.tempo.toInt());
	psvm_talkspeed->setValue(cfgSvm.talkspeed.toInt());
	psvm_debuglevel->setValue(cfgSvm.debuglevel.toInt());
	psvm_midi_gain->setValue(cfgSvm.midi_gain.toInt());
	psvm_walkspeed->setValue(cfgSvm.walkspeed.toInt());
//	psvm_master_volume->setValue(cfgSvm.master_volume.toInt());
	psvm_mute->setValue(fGrl->strToBool(cfgSvm.mute));
	psvm_speech_mute->setValue(fGrl->strToBool(cfgSvm.speech_mute));
	psvm_multi_midi->setValue(fGrl->strToBool(cfgSvm.multi_midi));
	psvm_native_mt32->setValue(fGrl->strToBool(cfgSvm.native_mt32));
	psvm_enable_gs->setValue(fGrl->strToBool(cfgSvm.enable_gs));

	DatosScummVM = cfgSvm;
}

void frmImportarScummVM::onValueScummVMChanged(PropertyItem *prop, const QVariant &value)
{
	QString gameid = "";
	if (prop->getKey() == "psvm_list_svm")
	{
		gameid = psvm_list_svm->getList().at(value.toInt()).m_userData.toString();
		if (!gameid.isEmpty())
		{
			DatosScummVM.game = gameid;
			psvm_gameid->setValue(DatosScummVM.game);
		}
	}

	if (prop->getKey() == "Svm_game")
		DatosScummVM.game = value.toString();
	if (prop->getKey() == "Svm_game_label")
		DatosScummVM.game_label = value.toString();
	if (prop->getKey() == "Svm_description")
		DatosScummVM.description = value.toString();
	if (prop->getKey() == "Svm_boot_param")
		DatosScummVM.boot_param = value.toString();
	if (prop->getKey() == "Svm_language")
		DatosScummVM.language = psvm_language->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_subtitles")
		DatosScummVM.subtitles = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_platform")
		DatosScummVM.platform = psvm_platform->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_gfx_mode")
		DatosScummVM.gfx_mode = psvm_gfx_mode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_render_mode")
		DatosScummVM.render_mode = psvm_render_mode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_fullscreen")
		DatosScummVM.fullscreen = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_filtering")
		DatosScummVM.filtering = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_aspect_ratio")
		DatosScummVM.aspect_ratio = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_path")
		DatosScummVM.path = value.toString();
	if (prop->getKey() == "Svm_path_extra")
		DatosScummVM.path_extra = value.toString();
	if (prop->getKey() == "Svm_path_save")
		DatosScummVM.path_save = value.toString();
	if (prop->getKey() == "Svm_music_driver")
		DatosScummVM.music_driver = psvm_music_driver->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_enable_gs")
		DatosScummVM.enable_gs = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_multi_midi")
		DatosScummVM.multi_midi = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_native_mt32")
		DatosScummVM.native_mt32 = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_mute")
		DatosScummVM.mute = fGrl->boolToStr(value.toBool());
//	if (prop->getKey() == "Svm_master_volume")
//		DatosScummVM.master_volume = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_music_volume")
		DatosScummVM.music_volume = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_sfx_volume")
		DatosScummVM.sfx_volume = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_speech_volume")
		DatosScummVM.speech_volume = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_speech_mute")
		DatosScummVM.speech_mute = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_tempo")
		DatosScummVM.tempo = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_talkspeed")
		DatosScummVM.talkspeed = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_debuglevel")
		DatosScummVM.debuglevel = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_cdrom")
		DatosScummVM.cdrom = psvm_cdrom->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_joystick_num")
		DatosScummVM.joystick_num = psvm_joystick_num->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_output_rate")
		DatosScummVM.output_rate = psvm_output_rate->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_midi_gain")
		DatosScummVM.midi_gain = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_copy_protection")
		DatosScummVM.copy_protection = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_sound_font")
		DatosScummVM.sound_font = value.toString();
	if (prop->getKey() == "Svm_walkspeed")
		DatosScummVM.walkspeed = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Svm_opl_driver")
		DatosScummVM.opl_driver = psvm_opl_driver->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Svm_disable_dithering")
		DatosScummVM.disable_dithering = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_originalsaveload")
		DatosScummVM.originalsaveload = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_bright_palette")
		DatosScummVM.bright_palette = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_alt_intro")
		DatosScummVM.alt_intro = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Svm_autosave_period")
		DatosScummVM.autosave_period = psvm_autosave_period->getList().at(value.toInt()).m_userData.toString();
//	if (prop->getKey() == "Svm_emu_svmpath")
//		DatosScummVM.emu_svmpath =

	emit cfgSvmChanged(DatosScummVM);
}
