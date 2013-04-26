/*
 *
 * GR-lida by Monthy
 *
 * This file is part of GR-lida is a Frontend for DOSBox, ScummVM and VDMSound
 * Copyright (C) 2006-2013 Pedro A. Garcia Rosado Aka Monthy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *
**/

#ifndef G_STRUCTS_H
#define G_STRUCTS_H

enum col_TwJuegos {
	col_IdGrl           =  0,
	col_Icono           =  1,
	col_Titulo          =  2,
	col_Subtitulo       =  3,
	col_Genero          =  4,
	col_Compania        =  5,
	col_Desarrollador   =  6,
	col_Tema            =  7,
	col_Grupo           =  8,
	col_Perspectiva     =  9,
	col_Idioma          = 10,
	col_IdiomaVoces     = 11,
	col_Formato         = 12,
	col_Anno            = 13,
	col_NumDisc         = 14,
	col_SistemaOp       = 15,
	col_Tamano          = 16,
	col_Graficos        = 17,
	col_Sonido          = 18,
	col_Jugabilidad     = 19,
	col_Original        = 20,
	col_Estado          = 21,
	col_Thumbs          = 22,
	col_Cover_front     = 23,
	col_Cover_back      = 24,
	col_Fecha           = 25,
	col_TipoEmu         = 26,
	col_Comentario      = 27,
	col_Favorito        = 28,
	col_Rating          = 29,
	col_Edad            = 30,
	col_Usuario         = 31,
	col_PathExe         = 32,
	col_PathSetup       = 33,
	col_ParametrosExe   = 34,
	col_ParametrosSetup = 35,
	col_PathCapturas    = 36
};

struct stGrlCats {
	QString id;
	QString tabla;
	QString titulo;
	QString img;
	QString orden;
	QString emu_show;
};

struct stGrlDatos {
	QString titulo;
	QString icono;
	QString extra;
	QString key;
};

struct stCfgExec {
	QString f_exe;
	QString f_param;
	QString f_exe_setup;
	QString f_param_setup;
	QString f_path;
	QString f_path_setup;
	bool isCfgExec;
	bool isCfgSetup;
};

struct stGrlDir {
	QString Home;
	QString Capturas;
	QString Confdbx;
	QString Confvdms;
	QString Covers;
	QString Datos;
	QString Iconos;
	QString Idiomas;
	QString Scripts;
	QString Smiles;
	QString Temp;
	QString Templates;
	QString Themes;
	QString Thumbs;
	QString ThumbsList;
};

struct stFileInfo {
	QString Drive;
	QString Path;
	QString FilePath;
	QString Name;
	QString NameExt;
	QString cExt;
	QString Ext;
	QString Size;
	bool    Exists;
	QString Crc32;
	QString Sha1;
	QString Md5;
};

struct stGrlCfg {
// SqlDatabase
	QString db_type;
	QString db_server;
	QString db_host;
	QString db_name;
	QString db_username;
	QString db_password;
	QString db_port;
	QString db_orden_col;
	QString db_orden_by;
	QString db_orden;
// OpcGeneral
	bool    Primeravez;
	QString DirApp;
	QString DirDOSBox;
	QString DirScummVM;
	QString DirBaseGames;
	bool    DOSBoxDisp;
	bool    ScummVMDisp;
	bool    VDMSoundDisp;
	QString IdiomaSelect;
	bool    OpenPdfExternal;
	QString url_xmldb;
	QString FormatoFecha;
	bool    ToolbarBigIcon;
	QString Style;
	bool    StylePalette;
	QString NameDirTheme;
	QString IconoFav;
	QString PicFlowType;
	QStringList PicFlowBgColor;
	bool    PicFlowShowNumPos;
	bool    PicFlowShowTriangle;
	int     PicFlowMinHeight;
	int     Skip_PicFlow;
	int     IndexTabArchivos;
	QString VersionDBx;
	QString LastSelectCatID;
	int     LastTabOptSelect;
// OpcFuente
	bool    font_usar;
	QString font_family;
	QString font_size;
// HttpProxy
	bool    ProxyEnable;
	int     ProxyType;
	QString ProxyHost;
	QString ProxyPort;
	bool    ProxyLogin;
	QString ProxyUserName;
	QString ProxyPassword;
// MainState
	bool       main_fullscreen;
	bool       main_maximized;
	QByteArray main_geometry;
	QByteArray main_state;
	QByteArray main_twJuegos_state;
// OpcVer
	bool Pnl_Nav;
	bool Pnl_FilesUrl;
	bool Pnl_Datos;
	bool Pnl_PictureFlow;
	bool Pnl_Capturas;
	bool Pnl_Menubar;
	bool Pnl_Toolbar;
	bool Pnl_Ordenar;
	bool Pnl_StatusBar;
	bool PictureFlowToCenter;
	bool ver_IconMode;
	bool ver_col_Icono;
	bool ver_col_Subtitulo;
	bool ver_col_Genero;
	bool ver_col_Compania;
	bool ver_col_Desarrollador;
	bool ver_col_Tema;
	bool ver_col_Grupo;
	bool ver_col_Perspectiva;
	bool ver_col_Idioma;
	bool ver_col_IdiomaVoces;
	bool ver_col_Formato;
	bool ver_col_Anno;
	bool ver_col_NumDisc;
	bool ver_col_SistemaOp;
	bool ver_col_Tamano;
	bool ver_col_Graficos;
	bool ver_col_Sonido;
	bool ver_col_Jugabilidad;
	bool ver_col_Original;
	bool ver_col_Estado;
	bool ver_col_Fecha;
	bool ver_col_TipoEmu;
	bool ver_col_Favorito;
	bool ver_col_Rating;
	bool ver_col_Edad;
	bool ver_col_Usuario;
// ImageViewerState
	bool img_maximized;
	bool img_fitToWindow;
	bool img_verListaImagenes;
// OpcMultimedia
	QStringList FormatsVideo;
	QStringList FormatsMusic;
	QStringList FormatsImage; 
// UltimoDirectorio
	QString DirBD;
	QString DirImportar;
	QString DirExportPath;
	QString DirOtherEmus;
// Datos de juegos
	QString Img_Thumbs;
	QString Img_CoverFront;
	QString Img_CoverBack;
	QString DatosFiles_PathFile;
	QString DatosFiles_PathExe;
	QString DatosFiles_PathSetup;
	QString DatosFiles_PathCapturas;
// DOSBox
	QString DirDbx;
	QString Dbx_path_exe;
	QString Dbx_path_setup;
	QString Dbx_gus_ultradir;
	QString Dbx_sdl_mapperfile;
	QString Dbx_dosbox_language;
	QString Dbx_dosbox_captures;
	QString Dbx_path_sonido;
	QString Montaje_path;
// ScummVM
	QString DirSvm;
	QString Svm_path;
	QString Svm_savepath;
	QString Svm_extrapath;
	QString Svm_path_capturas;
	QString Svm_path_sonido;
	QString Svm_path_setup;
	QString Svm_soundfont;
// VDMSound
	QString Vdms_path_exe;
	QString Vdms_icon;
// Updates
	bool    chkVersion;
	bool    SoloUpdates;
	QString Version;
	QString VerListSvm;
//--
	bool isChangedCategoria;
	bool isChangedEmuList;
	bool isChangedIdioma;

};

struct stLwIconCfg {
	int tw_icon_width;
	int tw_icon_height;
	int pf_img_width;
	int pf_img_height;
// Configuración --
	int icon_width;
	int icon_height;
	QString img_cover_top;
	QString img_cover_top_select;
	int img_cover_top_pos_x;
	int img_cover_top_pos_y;
	bool img_scaled;
	int img_scale_w;
	int img_scale_h;
	int img_scale_pos_x;
	int img_scale_pos_y;
	bool tipo_emu_show;
	int tipo_emu_pos_x;
	int tipo_emu_pos_y;
	bool rating_show;
	bool rating_vertical;
	int rating_pos_x;
	int rating_pos_y;
	bool title_bg_show;
	QString title_bg;
	int title_bg_pos_x;
	int title_bg_pos_y;
	bool title_show;
	int title_pos_x;
	int title_pos_y;
	int title_width;
	int title_height;
	QString title_font;
	int title_font_size;
	bool title_font_bold;
	bool title_font_italic;
	bool title_font_underline;
	int title_max_caracteres;
};

struct stDatosJuego {
	QString idgrl;
	QString icono;
	QString titulo;
	QString subtitulo;
	QString genero;
	QString compania;
	QString desarrollador;
	QString tema;
	QString grupo;
	QString perspectiva;
	QString idioma;
	QString idioma_voces;
	QString formato;
	QString anno;
	QString numdisc;
	QString sistemaop;
	QString tamano;
	QString graficos;
	QString sonido;
	QString jugabilidad;
	QString original;
	QString estado;
	QString thumbs;
	bool    thumbs_new;
	QString cover_front;
	bool    cover_front_new;
	QString cover_back;
	bool    cover_back_new;
	QString fecha;
	QString tipo_emu;
	QString comentario;
	QString favorito;
	QString rating;
	QString edad_recomendada;
	QString usuario;
	QString path_exe;
	QString parametros_exe;
	QString path_capturas;
	QString path_setup;
	QString parametros_setup;
};

struct stConfigDOSBox {
	QString id;
	QString idgrl;
	QString idcat;
// [sdl]
	QString sdl_fullscreen;
	QString sdl_fulldouble;
	QString sdl_fullfixed;
	QString sdl_fullresolution;
	QString sdl_windowresolution;
	QString sdl_output;
	QString sdl_hwscale;
	QString sdl_autolock;
	QString sdl_sensitivity;
	QString sdl_waitonerror;
	QString sdl_priority;
	QString sdl_mapperfile;
	QString sdl_usescancodes;
// [dosbox]
	QString dosbox_language;
	QString dosbox_machine;
	QString dosbox_captures;
	QString dosbox_memsize;
// [render]
	QString render_frameskip;
	QString render_aspect;
	QString render_scaler;
// [cpu]
	QString cpu_core;
	QString cpu_cputype;
	QString cpu_cycles;
	QString cpu_cycles_realmode;
	QString cpu_cycles_protmode;
	QString cpu_cycles_limitmode;
	QString cpu_cycleup;
	QString cpu_cycledown;
// [mixer]
	QString mixer_nosound;
	QString mixer_rate;
	QString mixer_blocksize;
	QString mixer_prebuffer;
// [midi]
	QString midi_mpu401;
	QString midi_intelligent;
	QString midi_device;
	QString midi_config;
	QString midi_mt32rate;
// [sblaster]
	QString sblaster_sbtype;
	QString sblaster_sbbase;
	QString sblaster_irq;
	QString sblaster_dma;
	QString sblaster_hdma;
	QString sblaster_mixer;
	QString sblaster_oplmode;
	QString sblaster_oplemu;
	QString sblaster_oplrate;
// [gus]
	QString gus_gus;
	QString gus_gusrate;
	QString gus_gusbase;
	QString gus_irq1;
	QString gus_irq2;
	QString gus_dma1;
	QString gus_dma2;
	QString gus_ultradir;
// [speaker]
	QString speaker_pcspeaker;
	QString speaker_pcrate;
	QString speaker_tandy;
	QString speaker_tandyrate;
	QString speaker_disney;
// [joystick]
	QString joystick_type;
	QString joystick_timed;
	QString joystick_autofire;
	QString joystick_swap34;
	QString joystick_buttonwrap;
// [modem]
	QString modem_modem;
	QString modem_comport;
	QString modem_listenport;
// [dserial]
	QString dserial_directserial;
	QString dserial_comport;
	QString dserial_realport;
	QString dserial_defaultbps;
	QString dserial_parity;
	QString dserial_bytesize;
	QString dserial_stopbit;
// [serial]
	QString serial_1;
	QString serial_2;
	QString serial_3;
	QString serial_4;
// [dos]
	QString dos_xms;
	QString dos_ems;
	QString dos_umb;
	QString dos_keyboardlayout;
	QString dos_version;
// [ipx]
	QString ipx_ipx;
	QString ipx_type;
	QString ipx_port;
	QString ipx_ip;
// [autoexec]
	QString autoexec;
// Opciones
	QString opt_autoexec;
	QString opt_loadfix;
	QString opt_loadfix_mem;
	QString opt_consola_dbox;
	QString opt_cerrar_dbox;
	QString opt_cycle_sincronizar;
// Path
	QString path_conf;
	QString path_sonido;
	QString path_exe;
	QString path_setup;
	QString parametros_exe;
	QString parametros_setup;
};

struct stConfigDOSBoxMount {
	QString id;
	QString id_dosbox;
	QString id_lista;
	QString path;
	QString label;
	QString tipo_as;
	QString letter;
	QString indx_cd;
	QString opt_mount;
	QString io_ctrl;
	QString select_mount;
	QString opt_size;
	QString opt_freesize;
	QString freesize;
};

struct stConfigScummVM {
	QString id;
	QString idgrl;
	QString idcat;
//--
	QString game;
	QString game_label;
	QString language;
	QString subtitles;
	QString platform;
	QString gfx_mode;
	QString render_mode;
	QString fullscreen;
	QString aspect_ratio;
	QString path;
	QString path_setup;
	QString path_extra;
	QString path_save;
	QString path_capturas;
	QString path_sonido;
	QString music_driver;
	QString enable_gs;
	QString multi_midi;
	QString native_mt32;
	QString mute;
	QString master_volume;
	QString music_volume;
	QString sfx_volume;
	QString speech_volume;
	QString speech_mute;
	QString tempo;
	QString talkspeed;
	QString debuglevel;
	QString cdrom;
	QString joystick_num;
	QString output_rate;
	QString midi_gain;
	QString copy_protection;
	QString sound_font;
	QString walkspeed;
	QString opl_driver;
	QString disable_dithering;
	QString alt_intro;
	QString boot_param;
//--
	QString description;
	QString emu_svmpath;
};

struct stConfigVDMSound {
	QString id;
	QString idgrl;
	QString idcat;
//--
	QString path_conf;
	QString path_exe;
	QString program_1;
	QString program_2;
	QString vdms_debug_1;
	QString vdms_debug_2;
	QString winnt_dos_1;
	QString winnt_dos_2;
	QString winnt_dosbox_1;
	QString winnt_dosbox_2;
	QString winnt_dosbox_3;
	QString winnt_storage_1;
	QString winnt_storage_2;
};

struct stDatosFiles {
	QString id;
	QString idgrl;
	QString idcat;
//--
	QString nombre;
	QString crc;
	QString descripcion;
	QString path;
	QString size;
	QString tipo;
};

struct stDatosUrls {
	QString id;
	QString idgrl;
	QString idcat;
//--
	QString url;
	QString descripcion;
};

#endif // G_STRUCTS_H
