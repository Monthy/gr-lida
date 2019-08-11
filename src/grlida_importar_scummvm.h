#ifndef GRLIDA_IMPORTAR_SCUMMVM_H
#define GRLIDA_IMPORTAR_SCUMMVM_H

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
class frmImportarScummVM;
}

class frmImportarScummVM : public QWidget
{
	Q_OBJECT

public:
	explicit frmImportarScummVM(stGrlCfg m_cfg, stGrlCats m_categoria, stConfigScummVM datos_svm, bool m_editando, QWidget *parent = 0);
	~frmImportarScummVM();

	void setEditorScummVM(stConfigScummVM cfgSvm);
	stConfigScummVM getEditorScummVM() { return DatosScummVM; }

signals:
	void cfgSvmChanged(stConfigScummVM cfgSvm);

private:
	Ui::frmImportarScummVM *ui;

	Funciones *fGrl;

	stGrlDir grlDir;
	stGrlCfg grlCfg;
	stGrlCats categoria;

	stConfigScummVM DatosScummVM;

	bool Editando;

// Property Editor ScummVM
	StringProperty     *psvm_gameid, *psvm_game_label, *psvm_boot_param, *psvm_description;
	IntegerProperty    *psvm_master_volume, *psvm_music_volume, *psvm_sfx_volume, *psvm_speech_volume;
	IntegerProperty    *psvm_tempo, *psvm_talkspeed, *psvm_debuglevel, *psvm_midi_gain, *psvm_walkspeed;
	BoolProperty       *psvm_subtitles, *psvm_fullscreen, *psvm_filtering, *psvm_aspect_ratio, *psvm_enable_gs;
	BoolProperty       *psvm_multi_midi, *psvm_native_mt32, *psvm_mute, *psvm_speech_mute;
	BoolProperty       *psvm_copy_protection, *psvm_disable_dithering, *psvm_alt_intro, *psvm_originalsaveload, *psvm_bright_palette;
	ComboBoxProperty   *psvm_list_svm, *psvm_language, *psvm_platform, *psvm_gfx_mode, *psvm_render_mode, *psvm_music_driver;
	ComboBoxProperty   *psvm_cdrom, *psvm_joystick_num, *psvm_output_rate, *psvm_opl_driver, *psvm_autosave_period;
	FileDialogProperty *psvm_path, *psvm_path_extra, *psvm_path_save, *psvm_sound_font, *psvm_emu_svmpath;

	void cargarConfig();
	void cargarListDataItem(ListData &l_data, QString archivo, QString dirImg, int num_col = 2, QString sep = "|", bool isClear = true, bool keyVisible = false);
	void cargarEditorScummVM();

private slots:
	void onValueScummVMChanged(PropertyItem *prop, const QVariant &value);

};

#endif // GRLIDA_IMPORTAR_SCUMMVM_H
