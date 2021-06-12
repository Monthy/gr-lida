#ifndef GRLIDA_IMPORTAR_DOSBOX_H
#define GRLIDA_IMPORTAR_DOSBOX_H

#include <QWidget>

#include "funciones.h"

#include "propertyeditor/propertyeditor.h"
#include "propertyeditor/propertyheader.h"
#include "propertyeditor/boolproperty.h"
#include "propertyeditor/comboboxcheckproperty.h"
#include "propertyeditor/comboboxproperty.h"
#include "propertyeditor/filedialogproperty.h"
#include "propertyeditor/integerproperty.h"
#include "propertyeditor/stringproperty.h"

namespace Ui {
class frmImportarDosBox;
}

class frmImportarDosBox : public QWidget
{
	Q_OBJECT

public:
	explicit frmImportarDosBox(stGrlCfg m_cfg, stGrlCats m_categoria, stConfigDOSBox datos_dbx, bool m_editando, QWidget *parent = 0);
	~frmImportarDosBox();

	void setEditorDOSBox(stConfigDOSBox cfgDbx);
	stConfigDOSBox getEditorDOSBox() { return DatosDosBox; }

	void setEmuKeyDOSBox(QString emu_key);

	QHash<QString, stGrlDatos> getDbxList() { return dbx_list; }
	void mountAdd(QTreeWidget *twMount, QList<stConfigDOSBoxMount> DatosMontaje);

signals:
	void cfgDbxChanged(stConfigDOSBox cfgDbx);

private:
	Ui::frmImportarDosBox *ui;

	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stConfigDOSBox DatosDosBox;

//	QList<stGrlDatos> dbx_list;
	QHash<QString, stGrlDatos> dbx_list;
	bool Editando;

// Property Editor DOSBox
	StringProperty     *pdbx_mixer_blocksize, *pdbx_mixer_prebuffer, *pdbx_midi_config, *pdbx_gus_ultradir;
	StringProperty     *pdbx_serial_1, *pdbx_serial_2, *pdbx_serial_3, *pdbx_serial_4, *pdbx_ipx_ip;
	StringProperty     *pdbx_autoexec, *pdbx_autoexec_ini, *pdbx_autoexec_fin, *pdbx_autoexec_ini_exe, *pdbx_autoexec_fin_exe;
	StringProperty     *pdbx_parametros_exe, *pdbx_parametros_setup, *pdbx_modem_irq;
	IntegerProperty    *pdbx_cpu_cycleup, *pdbx_cpu_cycledown, *pdbx_modem_listenport, *pdbx_ipx_port, *pdbx_opt_loadfix_mem;
	BoolProperty       *pdbx_sdl_fullscreen, *pdbx_sdl_fulldouble, *pdbx_sdl_fullfixed, *pdbx_sdl_autolock;
	BoolProperty       *pdbx_sdl_waitonerror, *pdbx_sdl_usescancodes, *pdbx_render_aspect, *pdbx_mixer_nosound;
	BoolProperty       *pdbx_midi_intelligent, *pdbx_sblaster_mixer, *pdbx_gus_gus, *pdbx_speaker_pcspeaker;
	BoolProperty       *pdbx_speaker_disney, *pdbx_joystick_timed, *pdbx_joystick_autofire, *pdbx_joystick_swap34;
	BoolProperty       *pdbx_joystick_buttonwrap, *pdbx_modem_modem,*pdbx_dserial_directserial, *pdbx_dos_xms;
	BoolProperty       *pdbx_dos_ems, *pdbx_ipx_ipx, *pdbx_opt_autoexec;
	BoolProperty       *pdbx_opt_loadfix, *pdbx_opt_consola_dbox, *pdbx_opt_cerrar_dbox, *pdbx_opt_cycle_sincronizar;
	ComboBoxProperty   *pdbx_sdl_fullresolution, *pdbx_sdl_windowresolution, *pdbx_sdl_output, *pdbx_sdl_hwscale;
	ComboBoxProperty   *pdbx_sdl_sensitivity, *pdbx_sdl_priority_1, *pdbx_sdl_priority_2, *pdbx_dosbox_machine;
	ComboBoxProperty   *pdbx_dosbox_memsize, *pdbx_dosbox_emu_key, *pdbx_render_frameskip, *pdbx_render_scaler;
	ComboBoxProperty   *pdbx_cpu_core, *pdbx_cpu_cputype, *pdbx_cpu_cycles, *pdbx_cpu_cycles_realmode;
	ComboBoxProperty   *pdbx_cpu_cycles_protmode, *pdbx_cpu_cycles_limitmode, *pdbx_mixer_rate, *pdbx_midi_mpu401;
	ComboBoxProperty   *pdbx_midi_device, *pdbx_midi_mt32rate, *pdbx_sblaster_sbtype, *pdbx_sblaster_sbbase;
	ComboBoxProperty   *pdbx_sblaster_irq, *pdbx_sblaster_dma, *pdbx_sblaster_hdma, *pdbx_sblaster_oplmode;
	ComboBoxProperty   *pdbx_sblaster_oplemu, *pdbx_sblaster_oplrate, *pdbx_gus_gusrate, *pdbx_gus_gusbase, *pdbx_gus_irq1;
	ComboBoxProperty   *pdbx_gus_irq2, *pdbx_gus_dma1, *pdbx_gus_dma2, *pdbx_speaker_pcrate, *pdbx_speaker_tandy;
	ComboBoxProperty   *pdbx_speaker_tandyrate, *pdbx_joystick_type, *pdbx_modem_comport, *pdbx_select_serial, *pdbx_type_serial;
	ComboBoxProperty   *pdbx_dserial_comport, *pdbx_dserial_realport, *pdbx_dserial_defaultbps, *pdbx_dserial_parity;
	ComboBoxProperty   *pdbx_dserial_bytesize, *pdbx_dserial_stopbit, *pdbx_dos_umb, *pdbx_dos_keyboardlayout, *pdbx_dos_version, *pdbx_ipx_type;
	FileDialogProperty *pdbx_sdl_mapperfile, *pdbx_dosbox_language, *pdbx_path_exe, *pdbx_path_setup;

	void cargarConfig();
	void cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col = 2, QString sep = "|", bool isClear = true, bool keyVisible = false);
	void cargarEditorDOSBox();

	void setOpcionesSerial(int index_type_serial);

private slots:
	void onValueDOSBoxChanged(PropertyItem *prop, const QVariant &value);

};

#endif // GRLIDA_IMPORTAR_DOSBOX_H
