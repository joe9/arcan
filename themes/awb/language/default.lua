--
-- Simple table of messages for internationalization
-- These follow the regular format- string format and won't be "unescaped".
--
return {
	GROUP_MUSIC = "Music",
	GROUP_RECORDINGS = "Recordings",
	GROUP_VIDEOS = "Videos",
	GROUP_SYSTEMS = "Systems",
	GROUP_SAVES = "Saves",
	GROUP_TOOLS = "Tools",
	HOVER_CLONE = "Clone: Attach a copy of the canvas\\n\\r to the mouse cursor",
	HOVER_FILTER = "Display Filters: click to activate / set group" ..
	"\\n\\rright-click specific filter to configure",
	HOVER_AFILTER = "Switch Audio Visualization",
	HOVER_PLAYPAUSE = "Pause / Play at normal speed",
	HOVER_FASTFWD = "Increase playback speed",
	HOVER_STATESAVE = "Save current target state",
	HOVER_STATELOAD = "Load target state",
	HOVER_TARGETCFG = "Target/Timing specific configuration\\n\\r",
	HOVER_CPUFILTER = "CPU- Based Display Filters",
	HOVER_GLOBALINPUT = "Toggle Global Input ON/OFF",
	HOVER_INPUTCFG = "Switch active input layout",
	HOVER_PLAYLIST = "Show Playlist",

	VIDREC_RES     = "Vertical Resolution",
	VIDREC_ASPECT  = "Aspect Ratio",
	VIDREC_CODEC   = "Video Codec",
	VIDREC_QUALITY = "Video Quality",
	VIDREC_ACODEC  = "Audio Codec",
	VIDREC_AQUALITY= "Audio Quality",
	VIDREC_FPS     = "Framerate",
	VIDREC_SAVE    = "Destination",

	HELPER_MSG = [[
		\#00ff00(global)\n\r\#ffffff
			ESCAPE\tunlock,default cancel,drop popup\n\r
			LCTRL\tgrab/release input\n\r
			SHIFT+TAB\t cycle visible windows\n\r
			ALT+F4\t close normal (non-media) window\n\r
			RCLICK\t context menu (where applicable)\n\r
			F12\t\tforce-focus window\n\r
			F11\t\tgather/scatter visible windows\n\r\n\r
		\#00ff00(titlebar)\#ffffff\n\r
			SHIFT+CLICK+DRAG\t fling window\n\r\n\r
		\#00ff00(target window)\#ffffff\n\r
			RCLICK\t scale options (resize-button)\n\r
			DBLCLICK\t grab global keyboard/mouse\n\r
		\#00ff00(icon/list windows)\#ffffff\n\r
		\#00ff00(media window)\#ffffff\n\r
			SHIFT\t *10 multiplier to seek\n\r
			UP/DOWN\t seek +10s\n\r
	]];
};

