#include "grlida_importar_dosbox.h"
#include "ui_importar_dosbox.h"

frmImportarDosBox::frmImportarDosBox(stGrlCfg m_cfg, stGrlCats m_categoria, stConfigDOSBox datos_dbx, bool m_editando, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::frmImportarDosBox)
{
	ui->setupUi(this);
	fGrl = new Funciones;

	grlCfg    = m_cfg;
	categoria = m_categoria;
	Editando  = m_editando;

	DatosDosBox = datos_dbx;

	grlDir.Home       = fGrl->dirApp();
	grlDir.Datos      = grlDir.Home +"datos/";
	grlDir.DatosDbGrl = grlDir.Home +"datosdb/"+ categoria.tabla +"/";

	cargarConfig();
}

frmImportarDosBox::~frmImportarDosBox()
{
	delete ui;
}

void frmImportarDosBox::cargarConfig()
{
	dbx_list.clear();
	dbx_list = fGrl->cargaDatosQHash(grlDir.Datos +"dbx_list.txt", 6, "|");

	cargarEditorDOSBox();
}

void frmImportarDosBox::cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col, QString sep, bool isClear, bool keyVisible)
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

void frmImportarDosBox::cargarEditorDOSBox()
{
	QIcon sinimg(fGrl->theme() +"img16/sinimg.png");
	ListData lista, sonido_frecuencias, sonido_Address, sonido_IRQ, sonido_DMA;

	cargarListDataItem(sonido_frecuencias, ":/datos/dbx_sb_oplrate.txt", "img16/", 2, "|");

	sonido_Address.clear();
	sonido_Address << ListDataItem("210", "210") << ListDataItem("220", "220") << ListDataItem("240", "240")
				<< ListDataItem("260", "260") << ListDataItem("280", "280") << ListDataItem("2a0", "2a0")
				<< ListDataItem("2c0", "2c0") << ListDataItem("2e0", "2e0") << ListDataItem("300", "300");

	sonido_IRQ.clear();
	sonido_IRQ << ListDataItem("3", "3") << ListDataItem("5", "5") << ListDataItem("7", "7")
			<< ListDataItem("9", "9") << ListDataItem("10", "10") << ListDataItem("11", "11")
			<< ListDataItem("12", "12");

	sonido_DMA.clear();
	sonido_DMA << ListDataItem("0", "0") << ListDataItem("1", "1") << ListDataItem("3", "3")
			<< ListDataItem("5", "5") << ListDataItem("6", "6") << ListDataItem("7", "7");

	int idx_select = -1;

// [sdl]
	PropertyHeader *p_head_sdl = new PropertyHeader(tr("Config - SDL"));

	pdbx_sdl_fullscreen = new BoolProperty(p_head_sdl, tr("fullscreen"), false, false);
	pdbx_sdl_fullscreen->setKey("Dbx_sdl_fullscreen");

	pdbx_sdl_fulldouble = new BoolProperty(p_head_sdl, tr("fulldouble"), false, false);
	pdbx_sdl_fulldouble->setKey("Dbx_sdl_fulldouble");

	pdbx_sdl_fullfixed  = new BoolProperty(p_head_sdl, tr("fullfixed"), false, false);
	pdbx_sdl_fullfixed->setKey("Dbx_sdl_fullfixed");

	cargarListDataItem(lista, grlDir.Datos +"dbx_resolution_svn.txt", "img16/", 2, "|");
	cargarListDataItem(lista, grlDir.Datos +"dbx_resolution.txt"    , "img16/", 2, "|", false);
	lista.append(ListDataItem(sinimg, "0x0", "0x0"));
	pdbx_sdl_fullresolution = new ComboBoxProperty(p_head_sdl, tr("fullresolution"), lista);
	pdbx_sdl_fullresolution->setKey("Dbx_sdl_fullresolution");
	idx_select = pdbx_sdl_fullresolution->indexOfData("original");
	pdbx_sdl_fullresolution->setIndex(idx_select);
	pdbx_sdl_fullresolution->setDefaultIndex(idx_select);

	pdbx_sdl_windowresolution = new ComboBoxProperty(p_head_sdl, tr("windowresolution"), lista);
	pdbx_sdl_windowresolution->setKey("Dbx_sdl_windowresolution");
	idx_select = pdbx_sdl_windowresolution->indexOfData("original");
	pdbx_sdl_windowresolution->setIndex(idx_select);
	pdbx_sdl_windowresolution->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_output_svn.txt", "img16/", 3, "|");
	cargarListDataItem(lista, grlDir.Datos +"dbx_output.txt"    , "img16/", 3, "|", false);
	pdbx_sdl_output = new ComboBoxProperty(p_head_sdl, tr("output"), lista);
	pdbx_sdl_output->setKey("Dbx_sdl_output");
	idx_select = pdbx_sdl_output->indexOfData("surface");
	pdbx_sdl_output->setIndex(idx_select);
	pdbx_sdl_output->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_hwscale.txt", "img16/", 2, "|");
	pdbx_sdl_hwscale = new ComboBoxProperty(p_head_sdl, tr("hwscale"), lista);
	pdbx_sdl_hwscale->setKey("Dbx_sdl_hwscale");
	idx_select = pdbx_sdl_hwscale->indexOfData("1.00");
	pdbx_sdl_hwscale->setIndex(idx_select);
	pdbx_sdl_hwscale->setDefaultIndex(idx_select);

	pdbx_sdl_autolock = new BoolProperty(p_head_sdl, tr("autolock"), true, true);
	pdbx_sdl_autolock->setKey("Dbx_sdl_autolock");

	cargarListDataItem(lista, grlDir.Datos +"dbx_sensitivity.txt", "img16/", 2, "|");
	pdbx_sdl_sensitivity = new ComboBoxProperty(p_head_sdl, tr("sensitivity"), lista);
	pdbx_sdl_sensitivity->setKey("Dbx_sdl_sensitivity");
	idx_select = pdbx_sdl_sensitivity->indexOfData("100");
	pdbx_sdl_sensitivity->setIndex(idx_select);
	pdbx_sdl_sensitivity->setDefaultIndex(idx_select);

	pdbx_sdl_waitonerror = new BoolProperty(p_head_sdl, tr("waitonerror"), true, true);
	pdbx_sdl_waitonerror->setKey("Dbx_sdl_waitonerror");

	lista.clear();
	lista << ListDataItem("lowest", "lowest") << ListDataItem("lower", "lower") << ListDataItem("normal", "normal")
		<< ListDataItem("higher", "higher") << ListDataItem("highest", "highest") << ListDataItem("pause", "pause");
	pdbx_sdl_priority_1 = new ComboBoxProperty(p_head_sdl, tr("priority_1"), lista);
	pdbx_sdl_priority_1->setKey("Dbx_sdl_priority_1");
	idx_select = pdbx_sdl_priority_1->indexOfData("higher");
	pdbx_sdl_priority_1->setIndex(idx_select);
	pdbx_sdl_priority_1->setDefaultIndex(idx_select);

	pdbx_sdl_priority_2 = new ComboBoxProperty(p_head_sdl, tr("priority_2"), lista);
	pdbx_sdl_priority_2->setKey("Dbx_sdl_priority_2");
	idx_select =  pdbx_sdl_priority_2->indexOfData("normal");
	pdbx_sdl_priority_2->setIndex(idx_select);
	pdbx_sdl_priority_2->setDefaultIndex(idx_select);

	pdbx_sdl_mapperfile = new FileDialogProperty(p_head_sdl, tr("mapperfile"), "mapper.txt", "mapper.txt");
	pdbx_sdl_mapperfile->setKey("Dbx_sdl_mapperfile");

	pdbx_sdl_usescancodes = new BoolProperty(p_head_sdl, tr("usescancodes"), true, true);
	pdbx_sdl_usescancodes->setKey("Dbx_sdl_usescancodes");

// [dosbox]
	PropertyHeader *p_head_dosbox = new PropertyHeader(tr("Config - DOSBox"));

	pdbx_dosbox_language = new FileDialogProperty(p_head_dosbox, tr("language"), "", "");
	pdbx_dosbox_language->setKey("Dbx_dosbox_language");

	cargarListDataItem(lista, grlDir.Datos +"dbx_machine_svn.txt", "img16/", 3, "|", true , true);
	cargarListDataItem(lista, grlDir.Datos +"dbx_machine.txt"    , "img16/", 3, "|", false, true);
	pdbx_dosbox_machine = new ComboBoxProperty(p_head_dosbox, tr("machine"), lista);
	pdbx_dosbox_machine->setKey("Dbx_dosbox_machine");
	idx_select =  pdbx_dosbox_machine->indexOfData("svga_s3");
	pdbx_dosbox_machine->setIndex(idx_select);
	pdbx_dosbox_machine->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_memsize.txt", "img16/", 2, "|");
	pdbx_dosbox_memsize = new ComboBoxProperty(p_head_dosbox, tr("memsize"), lista);
	pdbx_dosbox_memsize->setKey("Dbx_dosbox_memsize");
	pdbx_dosbox_memsize->setEditable(true);
	idx_select = pdbx_dosbox_memsize->indexOfData("16");
	pdbx_dosbox_memsize->setIndex(idx_select);
	pdbx_dosbox_memsize->setDefaultIndex(idx_select);

	lista.clear();
	foreach (const stGrlDatos &dbx, dbx_list)
		lista.append(ListDataItem(QIcon(fGrl->theme() +"img16/"+ dbx.icono), dbx.titulo, dbx.key));

	pdbx_dosbox_emu_key = new ComboBoxProperty(p_head_dosbox, tr("emu_key"), lista);
	pdbx_dosbox_emu_key->setKey("Dbx_dosbox_emu_key");
	idx_select = pdbx_dosbox_emu_key->indexOfData("dosbox");
	idx_select = (idx_select > -1) ? idx_select : 0;
	pdbx_dosbox_emu_key->setIndex(idx_select);
	pdbx_dosbox_emu_key->setDefaultIndex(idx_select);

// [render]
	PropertyHeader *p_head_render = new PropertyHeader(tr("Config - Render"));

	cargarListDataItem(lista, ":/datos/dbx_frameskip.txt", "img16/", 2, "|");
	pdbx_render_frameskip = new ComboBoxProperty(p_head_render, tr("frameskip"), lista);
	pdbx_render_frameskip->setKey("Dbx_render_frameskip");
	idx_select = pdbx_render_frameskip->indexOfData("0");
	pdbx_render_frameskip->setIndex(idx_select);
	pdbx_render_frameskip->setDefaultIndex(idx_select);

	pdbx_render_aspect = new BoolProperty(p_head_render, tr("aspect"), false, false);
	pdbx_render_aspect->setKey("Dbx_render_aspect");

	cargarListDataItem(lista, grlDir.Datos +"dbx_scaler_svn.txt", "img16/", 3, "|", true , true);
	cargarListDataItem(lista, grlDir.Datos +"dbx_scaler.txt"    , "img16/", 3, "|", false, true);
	pdbx_render_scaler = new ComboBoxProperty(p_head_render, tr("scaler"), lista);
	pdbx_render_scaler->setKey("Dbx_render_scaler");
	idx_select = pdbx_render_scaler->indexOfData("normal2x");
	pdbx_render_scaler->setIndex(idx_select);
	pdbx_render_scaler->setDefaultIndex(idx_select);

// [cpu]
	PropertyHeader *p_head_cpu = new PropertyHeader(tr("Config - CPU"));

	cargarListDataItem(lista, grlDir.Datos +"dbx_cpu_core.txt", "img16/", 3, "|");
	pdbx_cpu_core = new ComboBoxProperty(p_head_cpu, tr("core"), lista);
	pdbx_cpu_core->setKey("Dbx_cpu_core");
	idx_select = pdbx_cpu_core->indexOfData("auto");
	pdbx_cpu_core->setIndex(idx_select);
	pdbx_cpu_core->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_cpu_type_svn.txt", "img16/", 3, "|");
	cargarListDataItem(lista, grlDir.Datos +"dbx_cpu_type.txt"    , "img16/", 3, "|", false);
	pdbx_cpu_cputype = new ComboBoxProperty(p_head_cpu, tr("cputype"), lista);
	pdbx_cpu_cputype->setKey("Dbx_cpu_cputype");
	idx_select = pdbx_cpu_cputype->indexOfData("auto");
	pdbx_cpu_cputype->setIndex(idx_select);
	pdbx_cpu_cputype->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_cycles.txt", "img16/", 2, "|");
	lista.insert(0, ListDataItem(QIcon(), "", ""));
	pdbx_cpu_cycles = new ComboBoxProperty(p_head_cpu, tr("cycles"), lista);
	pdbx_cpu_cycles->setKey("Dbx_cpu_cycles");
	pdbx_cpu_cycles->setEditable(true);
	idx_select = pdbx_cpu_cycles->indexOfData("auto");
	pdbx_cpu_cycles->setIndex(idx_select);
	pdbx_cpu_cycles->setDefaultIndex(idx_select);

	pdbx_cpu_cycles_realmode = new ComboBoxProperty(p_head_cpu, tr("cycles_realmode"), lista);
	pdbx_cpu_cycles_realmode->setKey("Dbx_cpu_cycles_realmode");
	pdbx_cpu_cycles_realmode->setEditable(true);
	idx_select = pdbx_cpu_cycles_realmode->indexOfData("");
	pdbx_cpu_cycles_realmode->setIndex(idx_select);
	pdbx_cpu_cycles_realmode->setDefaultIndex(idx_select);

	pdbx_cpu_cycles_limitmode = new ComboBoxProperty(p_head_cpu, tr("cycles_limitmode"), lista);
	pdbx_cpu_cycles_limitmode->setKey("Dbx_cpu_cycles_limitmode");
	pdbx_cpu_cycles_limitmode->setEditable(true);
	idx_select = pdbx_cpu_cycles_limitmode->indexOfData("");
	pdbx_cpu_cycles_limitmode->setIndex(idx_select);
	pdbx_cpu_cycles_limitmode->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("", "") << ListDataItem("10%", "10%") << ListDataItem("20%", "20%")
		<< ListDataItem("30%", "30%") << ListDataItem("40%", "40%") << ListDataItem("50%", "50%")
		<< ListDataItem("60%", "60%") << ListDataItem("70%", "70%") << ListDataItem("80%", "80%")
		<< ListDataItem("90%", "90%") << ListDataItem("100%", "100%");
	pdbx_cpu_cycles_protmode = new ComboBoxProperty(p_head_cpu, tr("cycles_protmode"), lista);
	pdbx_cpu_cycles_protmode->setKey("Dbx_cpu_cycles_protmode");
	pdbx_cpu_cycles_protmode->setEditable(true);
	idx_select = pdbx_cpu_cycles_protmode->indexOfData("");
	pdbx_cpu_cycles_protmode->setIndex(idx_select);
	pdbx_cpu_cycles_protmode->setDefaultIndex(idx_select);

	pdbx_cpu_cycleup   = new IntegerProperty(p_head_cpu, tr("cycleup"), 10, 10, 0, 9000);
	pdbx_cpu_cycleup->setKey("Dbx_cpu_cycleup");

	pdbx_cpu_cycledown = new IntegerProperty(p_head_cpu, tr("cycledown"), 20, 20, 0, 9000);
	pdbx_cpu_cycledown->setKey("Dbx_cpu_cycledown");

// [mixer]
	PropertyHeader *p_head_mixer = new PropertyHeader(tr("Config - Mixer"));

	pdbx_mixer_nosound = new BoolProperty(p_head_mixer, tr("nosound"), false, false);
	pdbx_mixer_nosound->setKey("Dbx_mixer_nosound");

	pdbx_mixer_rate = new ComboBoxProperty(p_head_mixer, tr("rate"), sonido_frecuencias);
	pdbx_mixer_rate->setKey("Dbx_mixer_rate");
	idx_select = pdbx_mixer_rate->indexOfData("44100");
	pdbx_mixer_rate->setIndex(idx_select);
	pdbx_mixer_rate->setDefaultIndex(idx_select);

	pdbx_mixer_blocksize = new StringProperty(p_head_mixer, tr("blocksize"), "1024", "1024");
	pdbx_mixer_blocksize->setKey("Dbx_mixer_blocksize");

	pdbx_mixer_prebuffer = new StringProperty(p_head_mixer, tr("prebuffer"), "20", "20");
	pdbx_mixer_prebuffer->setKey("Dbx_mixer_prebuffer");

// [midi]
	PropertyHeader *p_head_midi = new PropertyHeader(tr("Config - MIDI"));

	cargarListDataItem(lista, grlDir.Datos +"dbx_mpu401.txt", "img16/", 2, "|");
	pdbx_midi_mpu401 = new ComboBoxProperty(p_head_midi, tr("mpu401"), lista);
	pdbx_midi_mpu401->setKey("Dbx_midi_mpu401");
	idx_select = pdbx_midi_mpu401->indexOfData("intelligent");
	pdbx_midi_mpu401->setIndex(idx_select);
	pdbx_midi_mpu401->setDefaultIndex(idx_select);

	pdbx_midi_intelligent = new BoolProperty(p_head_midi, tr("intelligent"), true, true);
	pdbx_midi_intelligent->setKey("Dbx_midi_intelligent");

	cargarListDataItem(lista, grlDir.Datos +"dbx_midi_device.txt", "img16/", 2, "|");
	pdbx_midi_device = new ComboBoxProperty(p_head_midi, tr("device"), lista);
	pdbx_midi_device->setKey("Dbx_midi_device");
	idx_select = pdbx_midi_device->indexOfData("default");
	pdbx_midi_device->setIndex(idx_select);
	pdbx_midi_device->setDefaultIndex(idx_select);

	pdbx_midi_config = new StringProperty(p_head_midi, tr("config"), "", "");
	pdbx_midi_config->setKey("Dbx_midi_config");

	pdbx_midi_mt32rate = new ComboBoxProperty(p_head_midi, tr("mt32rate"), sonido_frecuencias);
	pdbx_midi_mt32rate->setKey("Dbx_midi_mt32rate");
	idx_select = pdbx_midi_mt32rate->indexOfData("44100");
	pdbx_midi_mt32rate->setIndex(idx_select);
	pdbx_midi_mt32rate->setDefaultIndex(idx_select);

// [sblaster]
	PropertyHeader *p_head_sblaster = new PropertyHeader(tr("Config - Sound Blaster"));

	cargarListDataItem(lista, grlDir.Datos +"dbx_sbtype.txt", "img16/", 3, "|", true, true);
	pdbx_sblaster_sbtype = new ComboBoxProperty(p_head_sblaster, tr("sbtype"), lista);
	pdbx_sblaster_sbtype->setKey("Dbx_sblaster_sbtype");
	idx_select = pdbx_sblaster_sbtype->indexOfData("sb16");
	pdbx_sblaster_sbtype->setIndex(idx_select);
	pdbx_sblaster_sbtype->setDefaultIndex(idx_select);

	pdbx_sblaster_sbbase = new ComboBoxProperty(p_head_sblaster, tr("sbbase"), sonido_Address);
	pdbx_sblaster_sbbase->setKey("Dbx_sblaster_sbbase");
	idx_select = pdbx_sblaster_sbbase->indexOfData("220");
	pdbx_sblaster_sbbase->setIndex(idx_select);
	pdbx_sblaster_sbbase->setDefaultIndex(idx_select);

	pdbx_sblaster_irq = new ComboBoxProperty(p_head_sblaster, tr("irq"), sonido_IRQ);
	pdbx_sblaster_irq->setKey("Dbx_sblaster_irq");
	idx_select = pdbx_sblaster_irq->indexOfData("7");
	pdbx_sblaster_irq->setIndex(idx_select);
	pdbx_sblaster_irq->setDefaultIndex(idx_select);

	pdbx_sblaster_dma = new ComboBoxProperty(p_head_sblaster, tr("dma"), sonido_DMA);
	pdbx_sblaster_dma->setKey("Dbx_sblaster_dma");
	idx_select = pdbx_sblaster_dma->indexOfData("1");
	pdbx_sblaster_dma->setIndex(idx_select);
	pdbx_sblaster_dma->setDefaultIndex(idx_select);

	pdbx_sblaster_hdma = new ComboBoxProperty(p_head_sblaster, tr("hdma"), sonido_DMA);
	pdbx_sblaster_hdma->setKey("Dbx_sblaster_hdma");
	idx_select = pdbx_sblaster_hdma->indexOfData("5");
	pdbx_sblaster_hdma->setIndex(idx_select);
	pdbx_sblaster_hdma->setDefaultIndex(idx_select);

	pdbx_sblaster_mixer = new BoolProperty(p_head_sblaster, tr("mixer"), true, true);
	pdbx_sblaster_mixer->setKey("Dbx_sblaster_mixer");

	cargarListDataItem(lista, grlDir.Datos +"dbx_sb_oplmode.txt", "img16/", 3, "|", true, true);
	pdbx_sblaster_oplmode = new ComboBoxProperty(p_head_sblaster, tr("oplmode"), lista);
	pdbx_sblaster_oplmode->setKey("Dbx_sblaster_oplmode");
	idx_select = pdbx_sblaster_oplmode->indexOfData("auto");
	pdbx_sblaster_oplmode->setIndex(idx_select);
	pdbx_sblaster_oplmode->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_sb_oplemu.txt", "img16/", 2, "|");
	pdbx_sblaster_oplemu = new ComboBoxProperty(p_head_sblaster, tr("oplemu"), lista);
	pdbx_sblaster_oplemu->setKey("Dbx_sblaster_oplemu");
	idx_select = pdbx_sblaster_oplemu->indexOfData("default");
	pdbx_sblaster_oplemu->setIndex(idx_select);
	pdbx_sblaster_oplemu->setDefaultIndex(idx_select);

	pdbx_sblaster_oplrate = new ComboBoxProperty(p_head_sblaster, tr("oplrate"), sonido_frecuencias);
	pdbx_sblaster_oplrate->setKey("Dbx_sblaster_oplrate");
	idx_select = pdbx_sblaster_oplrate->indexOfData("44100");
	pdbx_sblaster_oplrate->setIndex(idx_select);
	pdbx_sblaster_oplrate->setDefaultIndex(idx_select);

// [gus]
	PropertyHeader *p_head_gus = new PropertyHeader(tr("Config - GUS"));

	pdbx_gus_gus = new BoolProperty(p_head_gus, tr("gus"), false, false);
	pdbx_gus_gus->setKey("Dbx_gus_gus");

	pdbx_gus_gusrate = new ComboBoxProperty(p_head_gus, tr("gusrate"), sonido_frecuencias);
	pdbx_gus_gusrate->setKey("Dbx_gus_gusrate");
	idx_select = pdbx_gus_gusrate->indexOfData("44100");
	pdbx_gus_gusrate->setIndex(idx_select);
	pdbx_gus_gusrate->setDefaultIndex(idx_select);

	pdbx_gus_gusbase = new ComboBoxProperty(p_head_gus, tr("gusbase"), sonido_Address);
	pdbx_gus_gusbase->setKey("Dbx_gus_gusbase");
	idx_select = pdbx_gus_gusbase->indexOfData("240");
	pdbx_gus_gusbase->setIndex(idx_select);
	pdbx_gus_gusbase->setDefaultIndex(idx_select);

	pdbx_gus_irq1 = new ComboBoxProperty(p_head_gus, tr("irq1"), sonido_IRQ);
	pdbx_gus_irq1->setKey("Dbx_gus_irq1");
	idx_select = pdbx_gus_irq1->indexOfData("5");
	pdbx_gus_irq1->setIndex(idx_select);
	pdbx_gus_irq1->setDefaultIndex(idx_select);

	pdbx_gus_irq2 = new ComboBoxProperty(p_head_gus, tr("irq2"), sonido_IRQ);
	pdbx_gus_irq2->setKey("Dbx_gus_irq2");
	idx_select = pdbx_gus_irq2->indexOfData("5");
	pdbx_gus_irq2->setIndex(idx_select);
	pdbx_gus_irq2->setDefaultIndex(idx_select);

	pdbx_gus_dma1 = new ComboBoxProperty(p_head_gus, tr("dma1"), sonido_DMA);
	pdbx_gus_dma1->setKey("Dbx_gus_dma1");
	idx_select = pdbx_gus_dma1->indexOfData("3");
	pdbx_gus_dma1->setIndex(idx_select);
	pdbx_gus_dma1->setDefaultIndex(idx_select);

	pdbx_gus_dma2 = new ComboBoxProperty(p_head_gus, tr("dma2"), sonido_DMA);
	pdbx_gus_dma2->setKey("Dbx_gus_dma2");
	idx_select = pdbx_gus_dma2->indexOfData("3");
	pdbx_gus_dma2->setIndex(idx_select);
	pdbx_gus_dma2->setDefaultIndex(idx_select);

	pdbx_gus_ultradir = new StringProperty(p_head_gus, tr("ultradir"), "C:\\ULTRASND", "C:\\ULTRASND");
	pdbx_gus_ultradir->setKey("Dbx_gus_ultradir");

// [speaker]
	PropertyHeader *p_head_speaker = new PropertyHeader(tr("Config - Speaker"));

	pdbx_speaker_pcspeaker = new BoolProperty(p_head_speaker, tr("pcspeaker"), true, true);
	pdbx_speaker_pcspeaker->setKey("Dbx_speaker_pcspeaker");

	pdbx_speaker_pcrate = new ComboBoxProperty(p_head_speaker, tr("pcrate"), sonido_frecuencias);
	pdbx_speaker_pcrate->setKey("Dbx_speaker_pcrate");
	idx_select = pdbx_speaker_pcrate->indexOfData("44100");
	pdbx_speaker_pcrate->setIndex(idx_select);
	pdbx_speaker_pcrate->setDefaultIndex(idx_select);

	pdbx_speaker_tandy = new ComboBoxProperty(p_head_speaker, tr("tandy"), lista);
	pdbx_speaker_tandy->setKey("Dbx_speaker_tandy");
	idx_select = pdbx_speaker_tandy->indexOfData("auto");
	pdbx_speaker_tandy->setIndex(idx_select);
	pdbx_speaker_tandy->setDefaultIndex(idx_select);

	pdbx_speaker_tandyrate = new ComboBoxProperty(p_head_speaker, tr("tandyrate"), sonido_frecuencias);
	pdbx_speaker_tandyrate->setKey("Dbx_speaker_tandyrate");
	idx_select = pdbx_speaker_tandyrate->indexOfData("44100");
	pdbx_speaker_tandyrate->setIndex(idx_select);
	pdbx_speaker_tandyrate->setDefaultIndex(idx_select);

	pdbx_speaker_disney = new BoolProperty(p_head_speaker, tr("disney"), true, true);
	pdbx_speaker_disney->setKey("Dbx_speaker_disney");

// [joystick]
	PropertyHeader *p_head_joystick = new PropertyHeader(tr("Config - Joystick"));

	cargarListDataItem(lista, grlDir.Datos +"dbx_joystick.txt", "img16/", 3, "|", true, true);
	pdbx_joystick_type = new ComboBoxProperty(p_head_joystick, tr("type"), lista);
	pdbx_joystick_type->setKey("Dbx_joystick_type");
	idx_select = pdbx_joystick_type->indexOfData("auto");
	pdbx_joystick_type->setIndex(idx_select);
	pdbx_joystick_type->setDefaultIndex(idx_select);

	pdbx_joystick_timed = new BoolProperty(p_head_joystick, tr("timed"), true, true);
	pdbx_joystick_timed->setKey("Dbx_joystick_timed");

	pdbx_joystick_autofire = new BoolProperty(p_head_joystick, tr("autofire"), false, false);
	pdbx_joystick_autofire->setKey("Dbx_joystick_autofire");

	pdbx_joystick_swap34 = new BoolProperty(p_head_joystick, tr("swap34"), false, false);
	pdbx_joystick_swap34->setKey("Dbx_joystick_swap34");

	pdbx_joystick_buttonwrap = new BoolProperty(p_head_joystick, tr("buttonwrap"), false, false);
	pdbx_joystick_buttonwrap->setKey("Dbx_joystick_buttonwrap");

// [modem]
	PropertyHeader *p_head_modem = new PropertyHeader(tr("Config - Modem"));

	pdbx_modem_modem = new BoolProperty(p_head_modem, tr("modem"), false, false);
	pdbx_modem_modem->setKey("Dbx_modem_modem");

	lista.clear();
	lista << ListDataItem("1", "1") << ListDataItem("2", "2");
	pdbx_modem_comport = new ComboBoxProperty(p_head_modem, tr("comport"), lista);
	pdbx_modem_comport->setKey("Dbx_modem_comport");
	idx_select = pdbx_modem_comport->indexOfData("2");
	pdbx_modem_comport->setIndex(idx_select);
	pdbx_modem_comport->setDefaultIndex(idx_select);

	pdbx_modem_listenport = new IntegerProperty(p_head_modem, tr("listenport"), 23, 23);
	pdbx_modem_listenport->setKey("Dbx_modem_listenport");

	pdbx_modem_irq = new StringProperty(p_head_modem, tr("irq"), "", "");
	pdbx_modem_irq->setKey("Dbx_modem_irq");

// [dserial]
	PropertyHeader *p_head_dserial = new PropertyHeader(tr("Config - Direct Serial"));

	pdbx_dserial_directserial = new BoolProperty(p_head_dserial, tr("directserial"), false, false);
	pdbx_dserial_directserial->setKey("Dbx_dserial_directserial");

	lista.clear();
	lista << ListDataItem("COM1", "COM1") << ListDataItem("COM2", "COM2")
		<< ListDataItem("COM3", "COM3") << ListDataItem("COM4", "COM4");
	pdbx_dserial_comport = new ComboBoxProperty(p_head_dserial, tr("comport"), lista);
	pdbx_dserial_comport->setKey("Dbx_dserial_comport");
	idx_select = pdbx_dserial_comport->indexOfData("1");
	pdbx_dserial_comport->setIndex(idx_select);
	pdbx_dserial_comport->setDefaultIndex(idx_select);

	pdbx_dserial_realport = new ComboBoxProperty(p_head_dserial, tr("realport"), lista);
	pdbx_dserial_realport->setKey("Dbx_dserial_realport");
	idx_select = pdbx_dserial_realport->indexOfData("COM1");
	pdbx_dserial_realport->setIndex(idx_select);
	pdbx_dserial_realport->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("1200", "1200") << ListDataItem("1800", "1800")
		<< ListDataItem("2400", "2400") << ListDataItem("4800", "4800")
		<< ListDataItem("7200", "7200") << ListDataItem("9600", "9600")
		<< ListDataItem("14400", "14400") << ListDataItem("19200", "19200")
		<< ListDataItem("38400", "38400") << ListDataItem("57600", "57600")
		<< ListDataItem("115200", "115200") << ListDataItem("128000", "128000");
	pdbx_dserial_defaultbps = new ComboBoxProperty(p_head_dserial, tr("defaultbps"), lista);
	pdbx_dserial_defaultbps->setEditable(true);
	pdbx_dserial_defaultbps->setKey("Dbx_dserial_defaultbps");
	idx_select = pdbx_dserial_defaultbps->indexOfData("1200");
	pdbx_dserial_defaultbps->setIndex(idx_select);
	pdbx_dserial_defaultbps->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("N", "N") << ListDataItem("E", "E") << ListDataItem("O", "O");
	pdbx_dserial_parity = new ComboBoxProperty(p_head_dserial, tr("parity"), lista);
	pdbx_dserial_parity->setKey("Dbx_dserial_parity");
	idx_select = pdbx_dserial_parity->indexOfData("N");
	pdbx_dserial_parity->setIndex(idx_select);
	pdbx_dserial_parity->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("5", "5") << ListDataItem("8", "8");
	pdbx_dserial_bytesize = new ComboBoxProperty(p_head_dserial, tr("bytesize"), lista);
	pdbx_dserial_bytesize->setKey("Dbx_dserial_bytesize");
	idx_select = pdbx_dserial_bytesize->indexOfData("8");
	pdbx_dserial_bytesize->setIndex(idx_select);
	pdbx_dserial_bytesize->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("1", "1") << ListDataItem("2", "2");
	pdbx_dserial_stopbit = new ComboBoxProperty(p_head_dserial, tr("stopbit"), lista);
	pdbx_dserial_stopbit->setKey("Dbx_dserial_stopbit");
	idx_select = pdbx_dserial_stopbit->indexOfData("1");
	pdbx_dserial_stopbit->setIndex(idx_select);
	pdbx_dserial_stopbit->setDefaultIndex(idx_select);

// [serial]
	PropertyHeader *p_head_edit_serial = new PropertyHeader(tr("Edit - Serial"));

	lista.clear();
	lista << ListDataItem("Serial 1", "serial_1") << ListDataItem("Serial 2", "serial_2")
		  << ListDataItem("Serial 3", "serial_3") << ListDataItem("Serial 4", "serial_4");
	pdbx_select_serial = new ComboBoxProperty(p_head_edit_serial, tr("Serial"), lista);
	pdbx_select_serial->setKey("Dbx_select_serial");
	idx_select = pdbx_select_serial->indexOfData("serial_1");
	pdbx_select_serial->setIndex(idx_select);
	pdbx_select_serial->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem("disabled", "disabled") << ListDataItem("dummy", "dummy")
		  << ListDataItem("modem", "modem") << ListDataItem("directserial", "directserial");
	pdbx_type_serial = new ComboBoxProperty(p_head_edit_serial, tr("serial"), lista);
	pdbx_type_serial->setKey("Dbx_type_serial");
	idx_select = pdbx_type_serial->indexOfData("dummy");
	pdbx_type_serial->setIndex(idx_select);
	pdbx_type_serial->setDefaultIndex(idx_select);

	PropertyHeader *p_head_serial = new PropertyHeader(tr("Config - Serial"));

	pdbx_serial_1 = new StringProperty(p_head_serial, tr("serial_1"), "", "");
	pdbx_serial_1->setKey("Dbx_serial_1");

	pdbx_serial_2 = new StringProperty(p_head_serial, tr("serial_2"), "", "");
	pdbx_serial_2->setKey("Dbx_serial_2");

	pdbx_serial_3 = new StringProperty(p_head_serial, tr("serial_3"), "", "");
	pdbx_serial_3->setKey("Dbx_serial_3");

	pdbx_serial_4 = new StringProperty(p_head_serial, tr("serial_4"), "", "");
	pdbx_serial_4->setKey("Dbx_serial_4");

// [dos]
	PropertyHeader *p_head_dos = new PropertyHeader(tr("Config - Dos"));

	pdbx_dos_xms = new BoolProperty(p_head_dos, tr("xms"), true, true);
	pdbx_dos_xms->setKey("Dbx_dos_xms");

	pdbx_dos_ems = new BoolProperty(p_head_dos, tr("ems"), true, true);
	pdbx_dos_ems->setKey("Dbx_dos_ems");

	lista.clear();
	lista << ListDataItem("true", "true") << ListDataItem("false", "false") << ListDataItem("max", "max");
	pdbx_dos_umb = new ComboBoxProperty(p_head_dos, tr("umb"), lista);
	pdbx_dos_umb->setKey("Dbx_dos_umb");
	idx_select = pdbx_dos_umb->indexOfData("true");
	pdbx_dos_umb->setIndex(idx_select);
	pdbx_dos_umb->setDefaultIndex(idx_select);

	cargarListDataItem(lista, grlDir.Datos +"dbx_keyboardlayout.txt", "img16/", 4, "|");
	pdbx_dos_keyboardlayout = new ComboBoxProperty(p_head_dos, tr("keyboardlayout"), lista);
	pdbx_dos_keyboardlayout->setKey("Dbx_dos_keyboardlayout");
	idx_select = pdbx_dos_keyboardlayout->indexOfData("auto");
	pdbx_dos_keyboardlayout->setIndex(idx_select);
	pdbx_dos_keyboardlayout->setDefaultIndex(idx_select);

	lista.clear();
	lista << ListDataItem(QIcon(fGrl->theme() +"img16/sinimg.png"), "defecto", "")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.22", "6 22")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.2" , "6 2")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "6.0" , "6 0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "5.0" , "5 0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "4.0" , "4 0")
		<< ListDataItem(QIcon(fGrl->theme() +"img16/ms-dos_win9x.png"), "3.3" , "3 3");
	pdbx_dos_version = new ComboBoxProperty(p_head_dos, tr("version"), lista);
	pdbx_dos_version->setKey("Dbx_dos_version");
	idx_select = pdbx_dos_version->indexOfData("6.22");
	pdbx_dos_version->setIndex(idx_select);
	pdbx_dos_version->setDefaultIndex(idx_select);

// [ipx]
	PropertyHeader *p_head_ipx = new PropertyHeader(tr("Config - IPX"));

	pdbx_ipx_ipx = new BoolProperty(p_head_ipx, tr("ipx"), false, false);
	pdbx_ipx_ipx->setKey("Dbx_ipx_ipx");

	lista.clear();
	lista << ListDataItem("Ninguna" , "none") << ListDataItem("Cliente" , "client") << ListDataItem("Servidor", "server");
	pdbx_ipx_type = new ComboBoxProperty(p_head_ipx, tr("type"), lista);
	pdbx_ipx_type->setKey("Dbx_ipx_type");
	idx_select = pdbx_ipx_type->indexOfData("none");
	pdbx_ipx_type->setDefaultIndex(idx_select);
	pdbx_ipx_type->setDefaultIndex(idx_select);

	pdbx_ipx_port = new IntegerProperty(p_head_ipx, tr("port"), 213, 213);
	pdbx_ipx_port->setKey("Dbx_ipx_port");

	pdbx_ipx_ip = new StringProperty(p_head_ipx, tr("ip"), "", "");
	pdbx_ipx_ip->setKey("Dbx_ipx_ip");

// [autoexec]
	PropertyHeader *p_head_autoexec = new PropertyHeader(tr("Config - Autoexec"));

	pdbx_autoexec = new StringProperty(p_head_autoexec, tr("autoexec"), "", "");
	pdbx_autoexec->setKey("Dbx_autoexec");
	pdbx_autoexec->setTextMultiLine(true);

	PropertyHeader *p_head_autoexec_ini = new PropertyHeader(tr("Config - Autoexec Inicio"));

	pdbx_autoexec_ini = new StringProperty(p_head_autoexec_ini, tr("autoexec_ini"), "", "");
	pdbx_autoexec_ini->setKey("Dbx_autoexec_ini");
	pdbx_autoexec_ini->setTextMultiLine(true);

	PropertyHeader *p_head_autoexec_fin = new PropertyHeader(tr("Config - Autoexec Final"));

	pdbx_autoexec_fin = new StringProperty(p_head_autoexec_fin, tr("autoexec_fin"), "", "");
	pdbx_autoexec_fin->setKey("Dbx_autoexec_fin");
	pdbx_autoexec_fin->setTextMultiLine(true);

	PropertyHeader *p_head_autoexec_ini_exe = new PropertyHeader(tr("Config - Antes del exe"));

	pdbx_autoexec_ini_exe = new StringProperty(p_head_autoexec_ini_exe, tr("autoexec_ini_exe"), "", "");
	pdbx_autoexec_ini_exe->setKey("Dbx_autoexec_ini_exe");
	pdbx_autoexec_ini_exe->setTextMultiLine(true);

	PropertyHeader *p_head_autoexec_fin_exe = new PropertyHeader(tr("Config - Después del exe"));

	pdbx_autoexec_fin_exe = new StringProperty(p_head_autoexec_fin_exe, tr("autoexec_fin_exe"), "", "");
	pdbx_autoexec_fin_exe->setKey("Dbx_autoexec_fin_exe");
	pdbx_autoexec_fin_exe->setTextMultiLine(true);

// Opciones
	PropertyHeader *p_head_opt = new PropertyHeader(tr("Config - Opciones"));

	pdbx_opt_autoexec = new BoolProperty(p_head_opt, tr("autoexec"), false, false);
	pdbx_opt_autoexec->setKey("Dbx_opt_autoexec");

	pdbx_opt_loadfix = new BoolProperty(p_head_opt, tr("loadfix"), false, false);
	pdbx_opt_loadfix->setKey("Dbx_opt_loadfix");

	pdbx_opt_loadfix_mem = new IntegerProperty(p_head_opt, tr("loadfix_mem"), 64, 64);
	pdbx_opt_loadfix_mem->setKey("Dbx_opt_loadfix_mem");

	pdbx_opt_consola_dbox = new BoolProperty(p_head_opt, tr("consola_dbox"), false, false);
	pdbx_opt_consola_dbox->setKey("Dbx_opt_consola_dbox");

	pdbx_opt_cerrar_dbox = new BoolProperty(p_head_opt, tr("cerrar_dbox"), true, true);
	pdbx_opt_cerrar_dbox->setKey("Dbx_opt_cerrar_dbox");

	pdbx_opt_cycle_sincronizar = new BoolProperty(p_head_opt, tr("cycle_sincronizar"), false, false);
	pdbx_opt_cycle_sincronizar->setKey("Dbx_opt_cycle_sincronizar");

// Path
	PropertyHeader *p_head_path = new PropertyHeader(tr("Config - Path"));

	pdbx_path_exe = new FileDialogProperty(p_head_path, tr("path_exe"), "", "");
	pdbx_path_exe->setKey("Dbx_path_exe");

	pdbx_path_setup = new FileDialogProperty(p_head_path, tr("path_setup"), "", "");
	pdbx_path_setup->setKey("Dbx_path_setup");

	pdbx_parametros_exe = new StringProperty(p_head_path, tr("parametros_exe"), "", "");
	pdbx_parametros_exe->setKey("Dbx_parametros_exe");

	pdbx_parametros_setup = new StringProperty(p_head_path, tr("parametros_setup"), "", "");
	pdbx_parametros_setup->setKey("Dbx_parametros_setup");

// --
	ui->twDOSBox->addProperty(p_head_sdl             , "p_head_sdl"             );
	ui->twDOSBox->addProperty(p_head_dosbox          , "p_head_dosbox"          );
	ui->twDOSBox->addProperty(p_head_render          , "p_head_render"          );
	ui->twDOSBox->addProperty(p_head_cpu             , "p_head_cpu"             );
	ui->twDOSBox->addProperty(p_head_mixer           , "p_head_mixer"           );
	ui->twDOSBox->addProperty(p_head_midi            , "p_head_midi"            );
	ui->twDOSBox->addProperty(p_head_sblaster        , "p_head_sblaster"        );
	ui->twDOSBox->addProperty(p_head_gus             , "p_head_gus"             );
	ui->twDOSBox->addProperty(p_head_speaker         , "p_head_speaker"         );
	ui->twDOSBox->addProperty(p_head_joystick        , "p_head_joystick"        );
	ui->twDOSBox->addProperty(p_head_modem           , "p_head_modem"           );
	ui->twDOSBox->addProperty(p_head_dserial         , "p_head_dserial"         );
	ui->twDOSBox->addProperty(p_head_edit_serial     , "p_head_edit_serial"     );
	ui->twDOSBox->addProperty(p_head_serial          , "p_head_serial"          );
	ui->twDOSBox->addProperty(p_head_dos             , "p_head_dos"             );
	ui->twDOSBox->addProperty(p_head_ipx             , "p_head_ipx"             );
	ui->twDOSBox->addProperty(p_head_autoexec        , "p_head_autoexec"        );
	ui->twDOSBox->addProperty(p_head_autoexec_ini    , "p_head_autoexec_ini"    );
	ui->twDOSBox->addProperty(p_head_autoexec_fin    , "p_head_autoexec_fin"    );
	ui->twDOSBox->addProperty(p_head_autoexec_ini_exe, "p_head_autoexec_ini_exe");
	ui->twDOSBox->addProperty(p_head_autoexec_fin_exe, "p_head_autoexec_fin_exe");
	ui->twDOSBox->addProperty(p_head_opt             , "p_head_opt"             );
	ui->twDOSBox->addProperty(p_head_path            , "p_head_path"            );

	ui->twDOSBox->setRootIsDecorated(false);
	ui->twDOSBox->adjustToContents();

	connect(ui->twDOSBox, SIGNAL(valueChanged(PropertyItem*,QVariant)), this, SLOT(onValueDOSBoxChanged(PropertyItem*,QVariant)));
}

void frmImportarDosBox::setEditorDOSBox(stConfigDOSBox cfgDbx)
{
	QStringList sdl_priority = cfgDbx.sdl_priority.split(",");
// [sdl]
	pdbx_sdl_fullscreen->setValue(fGrl->strToBool(cfgDbx.sdl_fullscreen));
	pdbx_sdl_fulldouble->setValue(fGrl->strToBool(cfgDbx.sdl_fulldouble));
	pdbx_sdl_fullfixed->setValue(fGrl->strToBool(cfgDbx.sdl_fullfixed));
	pdbx_sdl_fullresolution->setIndex(pdbx_sdl_fullresolution->indexOfData(cfgDbx.sdl_fullresolution));
	pdbx_sdl_windowresolution->setIndex(pdbx_sdl_windowresolution->indexOfData(cfgDbx.sdl_windowresolution));
	pdbx_sdl_output->setIndex(pdbx_sdl_output->indexOfData(cfgDbx.sdl_output));
	pdbx_sdl_hwscale->setIndex(pdbx_sdl_hwscale->indexOfData(cfgDbx.sdl_hwscale));
	pdbx_sdl_autolock->setValue(fGrl->strToBool(cfgDbx.sdl_autolock));
	pdbx_sdl_sensitivity->setIndex(pdbx_sdl_sensitivity->indexOfData(cfgDbx.sdl_sensitivity));
	pdbx_sdl_waitonerror->setValue(fGrl->strToBool(cfgDbx.sdl_waitonerror));
	pdbx_sdl_priority_1->setIndex(pdbx_sdl_priority_1->indexOfData(sdl_priority.at(0)));
	pdbx_sdl_priority_2->setIndex(pdbx_sdl_priority_2->indexOfData(sdl_priority.at(1)));
	pdbx_sdl_mapperfile->setValue(cfgDbx.sdl_mapperfile);
	pdbx_sdl_usescancodes->setValue(fGrl->strToBool(cfgDbx.sdl_usescancodes));
// [dosbox]
	pdbx_dosbox_language->setValue(cfgDbx.dosbox_language);
	pdbx_dosbox_machine->setIndex(pdbx_dosbox_machine->indexOfData(cfgDbx.dosbox_machine));
	pdbx_dosbox_memsize->setIndex(pdbx_dosbox_memsize->indexOfData(cfgDbx.dosbox_memsize));
	pdbx_dosbox_emu_key->setIndex(pdbx_dosbox_emu_key->indexOfData(cfgDbx.dosbox_emu_key));
// [render]
	pdbx_render_frameskip->setIndex(pdbx_render_frameskip->indexOfData(cfgDbx.render_frameskip));
	pdbx_render_aspect->setValue(fGrl->strToBool(cfgDbx.render_aspect));
	pdbx_render_scaler->setIndex(pdbx_render_scaler->indexOfData(cfgDbx.render_scaler));
// [cpu]
	pdbx_cpu_core->setIndex(pdbx_cpu_core->indexOfData(cfgDbx.cpu_core));
	pdbx_cpu_cputype->setIndex(pdbx_cpu_cputype->indexOfData(cfgDbx.cpu_cputype));
	pdbx_cpu_cycles->setIndex(pdbx_cpu_cycles->indexOfData(cfgDbx.cpu_cycles));
	pdbx_cpu_cycles_realmode->setIndex(pdbx_cpu_cycles_realmode->indexOfData(cfgDbx.cpu_cycles_realmode));
	pdbx_cpu_cycles_protmode->setIndex(pdbx_cpu_cycles_protmode->indexOfData(cfgDbx.cpu_cycles_protmode));
	pdbx_cpu_cycles_limitmode->setIndex(pdbx_cpu_cycles_limitmode->indexOfData(cfgDbx.cpu_cycles_limitmode));
	pdbx_cpu_cycleup->setValue(cfgDbx.cpu_cycleup.toInt());
	pdbx_cpu_cycledown->setValue(cfgDbx.cpu_cycledown.toInt());
// [mixer]
	pdbx_mixer_nosound->setValue(fGrl->strToBool(cfgDbx.mixer_nosound));
	pdbx_mixer_rate->setIndex(pdbx_mixer_rate->indexOfData(cfgDbx.mixer_rate));
	pdbx_mixer_blocksize->setValue(cfgDbx.mixer_blocksize);
	pdbx_mixer_prebuffer->setValue(cfgDbx.mixer_prebuffer);
// [midi]
	pdbx_midi_mpu401->setIndex(pdbx_midi_mpu401->indexOfData(cfgDbx.midi_mpu401));
	pdbx_midi_intelligent->setValue(fGrl->strToBool(cfgDbx.midi_intelligent));
	pdbx_midi_device->setIndex(pdbx_midi_device->indexOfData(cfgDbx.midi_device));
	pdbx_midi_config->setValue(cfgDbx.midi_config);
	pdbx_midi_mt32rate->setIndex(pdbx_midi_mt32rate->indexOfData(cfgDbx.midi_mt32rate));
// [sblaster]
	pdbx_sblaster_sbtype->setIndex(pdbx_sblaster_sbtype->indexOfData(cfgDbx.sblaster_sbtype));
	pdbx_sblaster_sbbase->setIndex(pdbx_sblaster_sbbase->indexOfData(cfgDbx.sblaster_sbbase));
	pdbx_sblaster_irq->setIndex(pdbx_sblaster_irq->indexOfData(cfgDbx.sblaster_irq));
	pdbx_sblaster_dma->setIndex(pdbx_sblaster_dma->indexOfData(cfgDbx.sblaster_dma));
	pdbx_sblaster_hdma->setIndex(pdbx_sblaster_hdma->indexOfData(cfgDbx.sblaster_hdma));
	pdbx_sblaster_mixer->setValue(fGrl->strToBool(cfgDbx.sblaster_mixer));
	pdbx_sblaster_oplmode->setIndex(pdbx_sblaster_oplmode->indexOfData(cfgDbx.sblaster_oplmode));
	pdbx_sblaster_oplemu->setIndex(pdbx_sblaster_oplemu->indexOfData(cfgDbx.sblaster_oplemu));
	pdbx_sblaster_oplrate->setIndex(pdbx_sblaster_oplrate->indexOfData(cfgDbx.sblaster_oplrate));
// [gus]
	pdbx_gus_gus->setValue(fGrl->strToBool(cfgDbx.gus_gus));
	pdbx_gus_gusrate->setIndex(pdbx_gus_gusrate->indexOfData(cfgDbx.gus_gusrate));
	pdbx_gus_gusbase->setIndex(pdbx_gus_gusbase->indexOfData(cfgDbx.gus_gusbase));
	pdbx_gus_irq1->setIndex(pdbx_gus_irq1->indexOfData(cfgDbx.gus_irq1));
	pdbx_gus_irq2->setIndex(pdbx_gus_irq2->indexOfData(cfgDbx.gus_irq2));
	pdbx_gus_dma1->setIndex(pdbx_gus_dma1->indexOfData(cfgDbx.gus_dma1));
	pdbx_gus_dma2->setIndex(pdbx_gus_dma2->indexOfData(cfgDbx.gus_dma2));
	pdbx_gus_ultradir->setValue(cfgDbx.gus_ultradir);
// [speaker]
	pdbx_speaker_pcspeaker->setValue(fGrl->strToBool(cfgDbx.speaker_pcspeaker));
	pdbx_speaker_pcrate->setIndex(pdbx_speaker_pcrate->indexOfData(cfgDbx.speaker_pcrate));
	pdbx_speaker_tandy->setIndex(pdbx_speaker_tandy->indexOfData(cfgDbx.speaker_tandy));
	pdbx_speaker_tandyrate->setIndex(pdbx_speaker_tandyrate->indexOfData(cfgDbx.speaker_tandyrate));
	pdbx_speaker_disney->setValue(fGrl->strToBool(cfgDbx.speaker_disney));
// [joystick]
	pdbx_joystick_type->setIndex(pdbx_joystick_type->indexOfData(cfgDbx.joystick_type));
	pdbx_joystick_timed->setValue(fGrl->strToBool(cfgDbx.joystick_timed));
	pdbx_joystick_autofire->setValue(fGrl->strToBool(cfgDbx.joystick_autofire));
	pdbx_joystick_swap34->setValue(fGrl->strToBool(cfgDbx.joystick_swap34));
	pdbx_joystick_buttonwrap->setValue(fGrl->strToBool(cfgDbx.joystick_buttonwrap));
// [modem]
	pdbx_modem_modem->setValue(fGrl->strToBool(cfgDbx.modem_modem));
	pdbx_modem_comport->setIndex(pdbx_modem_comport->indexOfData(cfgDbx.modem_comport));
	pdbx_modem_listenport->setValue(cfgDbx.modem_listenport.toInt());
// [dserial]
	pdbx_dserial_directserial->setValue(fGrl->strToBool(cfgDbx.dserial_directserial));
	pdbx_dserial_comport->setIndex(pdbx_dserial_comport->indexOfData(cfgDbx.dserial_comport));
	pdbx_dserial_realport->setIndex(pdbx_dserial_realport->indexOfData(cfgDbx.dserial_realport));
	pdbx_dserial_defaultbps->setIndex(pdbx_dserial_defaultbps->indexOfData(cfgDbx.dserial_defaultbps));
	pdbx_dserial_parity->setIndex(pdbx_dserial_parity->indexOfData(cfgDbx.dserial_parity));
	pdbx_dserial_bytesize->setIndex(pdbx_dserial_bytesize->indexOfData(cfgDbx.dserial_bytesize));
	pdbx_dserial_stopbit->setIndex(pdbx_dserial_stopbit->indexOfData(cfgDbx.dserial_stopbit));
// [serial]
	pdbx_serial_1->setValue(cfgDbx.serial_1);
	pdbx_serial_2->setValue(cfgDbx.serial_2);
	pdbx_serial_3->setValue(cfgDbx.serial_3);
	pdbx_serial_4->setValue(cfgDbx.serial_4);
// [dos]
	pdbx_dos_xms->setValue(fGrl->strToBool(cfgDbx.dos_xms));
	pdbx_dos_ems->setValue(fGrl->strToBool(cfgDbx.dos_ems));
	pdbx_dos_umb->setIndex(pdbx_dos_umb->indexOfData(cfgDbx.dos_umb));
	pdbx_dos_keyboardlayout->setIndex(pdbx_dos_keyboardlayout->indexOfData(cfgDbx.dos_keyboardlayout));
	pdbx_dos_version->setIndex(pdbx_dos_version->indexOfData(cfgDbx.dos_version));
// [ipx]
	pdbx_ipx_ipx->setValue(fGrl->strToBool(cfgDbx.ipx_ipx));
	pdbx_ipx_type->setIndex(pdbx_ipx_type->indexOfData(cfgDbx.ipx_type));
	pdbx_ipx_port->setValue(cfgDbx.ipx_port.toInt());
	pdbx_ipx_ip->setValue(cfgDbx.ipx_ip);
// [autoexec]
	pdbx_autoexec->setValue(cfgDbx.autoexec);
	pdbx_autoexec_ini->setValue(cfgDbx.autoexec_ini);
	pdbx_autoexec_fin->setValue(cfgDbx.autoexec_fin);
	pdbx_autoexec_ini_exe->setValue(cfgDbx.autoexec_ini_exe);
	pdbx_autoexec_fin_exe->setValue(cfgDbx.autoexec_fin_exe);
// Opciones
	pdbx_opt_autoexec->setValue(fGrl->strToBool(cfgDbx.opt_autoexec));
	pdbx_opt_loadfix->setValue(fGrl->strToBool(cfgDbx.opt_loadfix));
	pdbx_opt_loadfix_mem->setValue(cfgDbx.opt_loadfix_mem.toInt());
	pdbx_opt_consola_dbox->setValue(fGrl->strToBool(cfgDbx.opt_consola_dbox));
	pdbx_opt_cerrar_dbox->setValue(fGrl->strToBool(cfgDbx.opt_cerrar_dbox));
	pdbx_opt_cycle_sincronizar->setValue(fGrl->strToBool(cfgDbx.opt_cycle_sincronizar));
// Path
	pdbx_path_exe->setValue(cfgDbx.path_exe.replace("{DirBaseGames}", grlCfg.DirBaseGames));
	pdbx_parametros_exe->setValue(cfgDbx.parametros_exe);
	pdbx_path_setup->setValue(cfgDbx.path_setup.replace("{DirBaseGames}", grlCfg.DirBaseGames));
	pdbx_parametros_setup->setValue(cfgDbx.parametros_setup);

	DatosDosBox = cfgDbx;
}

void frmImportarDosBox::setEmuKeyDOSBox(QString emu_key)
{
	pdbx_dosbox_emu_key->setIndex(pdbx_dosbox_emu_key->indexOfData(emu_key));
	DatosDosBox.dosbox_emu_key = emu_key;
}

void frmImportarDosBox::setOpcionesSerial(int index_type_serial)
{
	QString Serial_Dbx;
	switch (index_type_serial)
	{
		case 0: // diasabled
			Serial_Dbx = pdbx_type_serial->getList().at(index_type_serial).m_userData.toString();
		break;
		case 1: // dummy
			Serial_Dbx = pdbx_type_serial->getList().at(index_type_serial).m_userData.toString();
		break;
		case 2: // modem
			Serial_Dbx = pdbx_type_serial->getList().at(index_type_serial).m_userData.toString() +" "+
						"listenport:"+ DatosDosBox.modem_listenport +" "+
						"realport:"+ DatosDosBox.dserial_realport +" "+
						"startbps:"+ DatosDosBox.dserial_defaultbps +" "+
						"parity:"+ DatosDosBox.dserial_parity +" "+
						"bytesize:"+ DatosDosBox.dserial_bytesize +" "+
						"stopbits:"+ DatosDosBox.dserial_stopbit +" "+
						"irq:"+ DatosDosBox.modem_irq;
		break;
		case 3: // directserial
			Serial_Dbx = pdbx_type_serial->getList().at(index_type_serial).m_userData.toString() +" "+
						"realport:"+ DatosDosBox.dserial_realport +" "+
						"startbps:"+ DatosDosBox.dserial_defaultbps +" "+
						"parity:"+ DatosDosBox.dserial_parity +" "+
						"bytesize:"+ DatosDosBox.dserial_bytesize +" "+
						"stopbits:"+ DatosDosBox.dserial_stopbit +" "+
						"irq:"+ DatosDosBox.modem_irq;
		break;
	}

	switch (pdbx_select_serial->getIndex())
	{
		case 0:
			pdbx_serial_1->setValue(Serial_Dbx);
		break;
		case 1:
			pdbx_serial_2->setValue(Serial_Dbx);
		break;
		case 2:
			pdbx_serial_3->setValue(Serial_Dbx);
		break;
		case 3:
			pdbx_serial_4->setValue(Serial_Dbx);
		break;
	}
}

void frmImportarDosBox::onValueDOSBoxChanged(PropertyItem *prop, const QVariant &value)
{
// [sdl]
	if (prop->getKey() == "Dbx_sdl_fullscreen")
		DatosDosBox.sdl_fullscreen = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sdl_fulldouble")
		DatosDosBox.sdl_fulldouble = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sdl_fullfixed")
		DatosDosBox.sdl_fullfixed = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sdl_fullresolution")
		DatosDosBox.sdl_fullresolution = pdbx_sdl_fullresolution->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_windowresolution")
		DatosDosBox.sdl_windowresolution = pdbx_sdl_windowresolution->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_output")
		DatosDosBox.sdl_output = pdbx_sdl_output->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_hwscale")
		DatosDosBox.sdl_hwscale = pdbx_sdl_hwscale->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_autolock")
		DatosDosBox.sdl_autolock = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sdl_sensitivity")
		DatosDosBox.sdl_sensitivity = pdbx_sdl_sensitivity->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_waitonerror")
		DatosDosBox.sdl_waitonerror = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sdl_priority_1")
		DatosDosBox.sdl_priority = pdbx_sdl_priority_1->getList().at(value.toInt()).m_userData.toString() +","+ pdbx_sdl_priority_2->getList().at(pdbx_sdl_priority_2->getIndex()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_priority_2")
		DatosDosBox.sdl_priority = pdbx_sdl_priority_1->getList().at(pdbx_sdl_priority_1->getIndex()).m_userData.toString() +","+ pdbx_sdl_priority_2->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sdl_mapperfile")
		DatosDosBox.sdl_mapperfile = value.toString();
	if (prop->getKey() == "Dbx_sdl_usescancodes")
		DatosDosBox.sdl_usescancodes = fGrl->boolToStr(value.toBool());
// [dosbox]
	if (prop->getKey() == "Dbx_dosbox_language")
		DatosDosBox.dosbox_language = value.toString();
	if (prop->getKey() == "Dbx_dosbox_machine")
		DatosDosBox.dosbox_machine = pdbx_dosbox_machine->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_dosbox_memsize")
		DatosDosBox.dosbox_memsize = pdbx_dosbox_memsize->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_dosbox_emu_key")
		DatosDosBox.dosbox_emu_key = pdbx_dosbox_emu_key->getList().at(value.toInt()).m_userData.toString();
// [render]
	if (prop->getKey() == "Dbx_render_frameskip")
		DatosDosBox.render_frameskip = pdbx_render_frameskip->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_render_aspect")
		DatosDosBox.render_aspect = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_render_scaler")
		DatosDosBox.render_scaler = pdbx_render_scaler->getList().at(value.toInt()).m_userData.toString();
// [cpu]
	if (prop->getKey() == "Dbx_cpu_core")
		DatosDosBox.cpu_core = pdbx_cpu_core->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cputype")
		DatosDosBox.cpu_cputype = pdbx_cpu_cputype->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cycles")
		DatosDosBox.cpu_cycles = pdbx_cpu_cycles->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cycles_realmode")
		DatosDosBox.cpu_cycles_realmode = pdbx_cpu_cycles_realmode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cycles_protmode")
		DatosDosBox.cpu_cycles_protmode = pdbx_cpu_cycles_protmode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cycles_limitmode")
		DatosDosBox.cpu_cycles_limitmode = pdbx_cpu_cycles_limitmode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_cpu_cycleup")
		DatosDosBox.cpu_cycleup = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Dbx_cpu_cycledown")
		DatosDosBox.cpu_cycledown = fGrl->intToStr(value.toInt());
// [mixer]
	if (prop->getKey() == "Dbx_mixer_nosound")
		DatosDosBox.mixer_nosound = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_mixer_rate")
		DatosDosBox.mixer_rate = pdbx_mixer_rate->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_mixer_blocksize")
		DatosDosBox.mixer_blocksize = value.toString();
	if (prop->getKey() == "Dbx_mixer_prebuffer")
		DatosDosBox.mixer_prebuffer = value.toString();
// [midi]
	if (prop->getKey() == "Dbx_midi_mpu401")
		DatosDosBox.midi_mpu401 = pdbx_midi_mpu401->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_midi_intelligent")
		DatosDosBox.midi_intelligent = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_midi_device")
		DatosDosBox.midi_device = pdbx_midi_device->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_midi_config")
		DatosDosBox.midi_config = value.toString();
	if (prop->getKey() == "Dbx_midi_mt32rate")
		DatosDosBox.midi_mt32rate = pdbx_midi_mt32rate->getList().at(value.toInt()).m_userData.toString();
// [sblaster]
	if (prop->getKey() == "Dbx_sblaster_sbtype")
		DatosDosBox.sblaster_sbtype = pdbx_sblaster_sbtype->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_sbbase")
		DatosDosBox.sblaster_sbbase = pdbx_sblaster_sbbase->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_irq")
		DatosDosBox.sblaster_irq = pdbx_sblaster_irq->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_dma")
		DatosDosBox.sblaster_dma = pdbx_sblaster_dma->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_hdma")
		DatosDosBox.sblaster_hdma = pdbx_sblaster_hdma->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_mixer")
		DatosDosBox.sblaster_mixer = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_sblaster_oplmode")
		DatosDosBox.sblaster_oplmode = pdbx_sblaster_oplmode->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_oplemu")
		DatosDosBox.sblaster_oplemu = pdbx_sblaster_oplemu->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_sblaster_oplrate")
		DatosDosBox.sblaster_oplrate = pdbx_sblaster_oplrate->getList().at(value.toInt()).m_userData.toString();
// [gus]
	if (prop->getKey() == "Dbx_gus_gus")
		DatosDosBox.gus_gus = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_gus_gusrate")
		DatosDosBox.gus_gusrate = pdbx_gus_gusrate->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_gusbase")
		DatosDosBox.gus_gusbase = pdbx_gus_gusbase->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_irq1")
		DatosDosBox.gus_irq1 = pdbx_gus_irq1->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_irq2")
		DatosDosBox.gus_irq2 = pdbx_gus_irq2->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_dma1")
		DatosDosBox.gus_dma1 = pdbx_gus_dma1->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_dma2")
		DatosDosBox.gus_dma2 = pdbx_gus_dma2->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_gus_ultradir")
		DatosDosBox.gus_ultradir = value.toString();
// [speaker]
	if (prop->getKey() == "Dbx_speaker_pcspeaker")
		DatosDosBox.speaker_pcspeaker = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_speaker_pcrate")
		DatosDosBox.speaker_pcrate = pdbx_speaker_pcrate->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_speaker_tandy")
		DatosDosBox.speaker_tandy = pdbx_speaker_tandy->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_speaker_tandyrate")
		DatosDosBox.speaker_tandyrate = pdbx_speaker_tandyrate->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_speaker_disney")
		DatosDosBox.speaker_disney = fGrl->boolToStr(value.toBool());
// [joystick]
	if (prop->getKey() == "Dbx_joystick_type")
		DatosDosBox.joystick_type = pdbx_joystick_type->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_joystick_timed")
		DatosDosBox.joystick_timed = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_joystick_autofire")
		DatosDosBox.joystick_autofire = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_joystick_swap34")
		DatosDosBox.joystick_swap34 = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_joystick_buttonwrap")
		DatosDosBox.joystick_buttonwrap = fGrl->boolToStr(value.toBool());
// --
	if (prop->getKey() == "Dbx_select_serial")
		setOpcionesSerial(pdbx_type_serial->getIndex());
	if (prop->getKey() == "Dbx_type_serial")
		setOpcionesSerial(value.toInt());
// [modem]
	if (prop->getKey() == "Dbx_modem_modem")
		DatosDosBox.modem_modem = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_modem_comport")
		DatosDosBox.modem_comport = pdbx_modem_comport->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_modem_listenport")
	{
		DatosDosBox.modem_listenport = fGrl->intToStr(value.toInt());
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
	if (prop->getKey() == "Dbx_modem_irq")
	{
		DatosDosBox.modem_irq = value.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
// [dserial]
	if (prop->getKey() == "Dbx_dserial_directserial")
		DatosDosBox.dserial_directserial = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_dserial_comport")
		DatosDosBox.dserial_comport = pdbx_dserial_comport->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_dserial_realport")
	{
		DatosDosBox.dserial_realport = pdbx_dserial_realport->getList().at(value.toInt()).m_userData.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
	if (prop->getKey() == "Dbx_dserial_defaultbps")
	{
		DatosDosBox.dserial_defaultbps = pdbx_dserial_defaultbps->getList().at(value.toInt()).m_userData.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
	if (prop->getKey() == "Dbx_dserial_parity")
	{
		DatosDosBox.dserial_parity = pdbx_dserial_parity->getList().at(value.toInt()).m_userData.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
	if (prop->getKey() == "Dbx_dserial_bytesize")
	{
		DatosDosBox.dserial_bytesize = pdbx_dserial_bytesize->getList().at(value.toInt()).m_userData.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
	if (prop->getKey() == "Dbx_dserial_stopbit")
	{
		DatosDosBox.dserial_stopbit = pdbx_dserial_stopbit->getList().at(value.toInt()).m_userData.toString();
		setOpcionesSerial(pdbx_type_serial->getIndex());
	}
// [serial]
	if (prop->getKey() == "Dbx_serial_1")
		DatosDosBox.serial_1 = value.toString();
	if (prop->getKey() == "Dbx_serial_2")
		DatosDosBox.serial_2 = value.toString();
	if (prop->getKey() == "Dbx_serial_3")
		DatosDosBox.serial_3 = value.toString();
	if (prop->getKey() == "Dbx_serial_4")
		DatosDosBox.serial_4 = value.toString();
// [dos]
	if (prop->getKey() == "Dbx_dos_xms")
		DatosDosBox.dos_xms = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_dos_ems")
		DatosDosBox.dos_ems = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_dos_umb")
		DatosDosBox.dos_umb = pdbx_dos_umb->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_dos_keyboardlayout")
		DatosDosBox.dos_keyboardlayout = pdbx_dos_keyboardlayout->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_dos_version")
		DatosDosBox.dos_version = pdbx_dos_version->getList().at(value.toInt()).m_userData.toString();
// [ipx]
	if (prop->getKey() == "Dbx_ipx_ipx")
		DatosDosBox.ipx_ipx = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_ipx_type")
		DatosDosBox.ipx_type = pdbx_ipx_type->getList().at(value.toInt()).m_userData.toString();
	if (prop->getKey() == "Dbx_ipx_port")
		DatosDosBox.ipx_port = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Dbx_ipx_ip")
		DatosDosBox.ipx_ip = value.toString();
// [autoexec]
	if (prop->getKey() == "Dbx_autoexec")
		DatosDosBox.autoexec = value.toString();
	if (prop->getKey() == "Dbx_autoexec_ini")
		DatosDosBox.autoexec_ini = value.toString();
	if (prop->getKey() == "Dbx_autoexec_fin")
		DatosDosBox.autoexec_fin = value.toString();
	if (prop->getKey() == "Dbx_autoexec_ini_exe")
		DatosDosBox.autoexec_ini_exe = value.toString();
	if (prop->getKey() == "Dbx_autoexec_fin_exe")
		DatosDosBox.autoexec_fin_exe = value.toString();
//Opciones
	if (prop->getKey() == "Dbx_opt_autoexec")
		DatosDosBox.opt_autoexec = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_opt_loadfix")
		DatosDosBox.opt_loadfix = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_opt_loadfix_mem")
		DatosDosBox.opt_loadfix_mem = fGrl->intToStr(value.toInt());
	if (prop->getKey() == "Dbx_opt_consola_dbox")
		DatosDosBox.opt_consola_dbox = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_opt_cerrar_dbox")
		DatosDosBox.opt_cerrar_dbox = fGrl->boolToStr(value.toBool());
	if (prop->getKey() == "Dbx_opt_cycle_sincronizar")
		DatosDosBox.opt_cycle_sincronizar = fGrl->boolToStr(value.toBool());
//Path
	if (prop->getKey() == "Dbx_path_exe")
	{
		DatosDosBox.path_exe = fGrl->setDirRelative(value.toString(), "DosGames");
		pdbx_path_exe->setValue(DatosDosBox.path_exe);
	}
	if (prop->getKey() == "Dbx_path_setup")
	{
		DatosDosBox.path_setup = fGrl->setDirRelative(value.toString(), "DosGames");
		pdbx_path_setup->setValue(DatosDosBox.path_setup);
	}
	if (prop->getKey() == "Dbx_parametros_exe")
		DatosDosBox.parametros_exe = value.toString();
	if (prop->getKey() == "Dbx_parametros_setup")
		DatosDosBox.parametros_setup = value.toString();

	emit cfgDbxChanged(DatosDosBox);
}

void frmImportarDosBox::mountAdd(QTreeWidget *twMount, QList<stConfigDOSBoxMount> DatosMontaje)
{
	twMount->clear();
	for (int m = 0; m < DatosMontaje.size(); ++m)
	{
		DatosMontaje[m].id_lista = fGrl->intToStr(m);
		QString select_mount = DatosMontaje.at(m).select_mount == "v" ? "s_" : "";

		DatosMontaje[m].path.replace("\\","/");
		if ((DatosMontaje.at(m).tipo_as == "floppy") || (DatosMontaje.at(m).tipo_as == "drive") || (DatosMontaje.at(m).tipo_as == "cdrom"))
		{
			if (!DatosMontaje.at(m).path.endsWith("/"))
				DatosMontaje[m].path.append("/");
		}

		QString path_mount = DatosMontaje.at(m).path;

		QTreeWidgetItem *item = new QTreeWidgetItem(twMount);
		item->setTextAlignment(1, Qt::AlignCenter);
		item->setTextAlignment(2, Qt::AlignCenter);
		item->setIcon( 0, QIcon(fGrl->getIconMount(DatosMontaje.at(m).tipo_as, select_mount)));
		item->setText( 0, path_mount.replace("{DirBaseGames}", grlCfg.DirBaseGames));	// directorio o iso
		item->setText( 1, DatosMontaje.at(m).letter      );	// letra de montaje
		item->setText( 2, DatosMontaje.at(m).tipo_as     );	// tipo de montaje
		item->setText( 3, DatosMontaje.at(m).label       );	// etiqueta
		item->setText( 4, DatosMontaje.at(m).indx_cd     );	// index de la unidad de cd-rom
		item->setText( 5, DatosMontaje.at(m).opt_mount   );	// opciones del cd-rom
		item->setText( 6, DatosMontaje.at(m).io_ctrl     );	// cd/dvd
		item->setText( 7, DatosMontaje.at(m).select_mount);	// primer montaje
		item->setText( 9, DatosMontaje.at(m).id_lista    );
		item->setText(10, DatosMontaje.at(m).opt_size    );
		item->setText(11, DatosMontaje.at(m).opt_freesize);
		item->setText(12, DatosMontaje.at(m).freesize    );
	}
}

