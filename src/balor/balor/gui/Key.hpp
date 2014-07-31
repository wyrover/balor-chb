#pragma once

#include <balor/Enum.hpp>


namespace balor {
	namespace gui {



/**
* キーボードのキーを表す。
*
* <h3>・サンプルコード</h3>
* <pre><code>
Frame frame(L"Key Sample");

Label label(frame, 20, 10, 0, 0, L"Ctrl+C, Ctrl+V, Ctrl+Z, Ctrl+Y キーに反応する");

frame.onShortcutKey() = [&] (Frame::ShortcutKey& e) {
switch (e.shortcut()) {
case Key::Modifier::ctrl | Key::c : {
MsgBox::show(L"カット");
e.handled(true);
} break;
case Key::Modifier::ctrl | Key::v : {
MsgBox::show(L"ペースト");
e.handled(true);
} break;
case Key::Modifier::ctrl | Key::z : {
MsgBox::show(L"アンドゥ");
e.handled(true);
} break;
case Key::Modifier::ctrl | Key::y : {
MsgBox::show(L"リドゥ");
e.handled(true);
} break;
}
};

frame.runMessageLoop();
* </code></pre>
*/
struct Key {
	enum _enum {
		a = 'A',
		add = 0x6B, //VK_ADD
		alt = 0x12, //VK_MENU
		apps = 0x5D, //VK_APPS
		attn = 0xF6, //VK_ATTN
		b = 'B',
		back = 0x08, //VK_BACK
		browserBack = 0xA6, //VK_BROWSER_BACK
		browserFavorites = 0xAB, //VK_BROWSER_FAVORITES
		browserForward = 0xA7, //VK_BROWSER_FORWARD
		browserHome = 0xAC, //VK_BROWSER_HOME
		browserRefresh = 0xA8, //VK_BROWSER_REFRESH
		browserSearch = 0xAA, //VK_BROWSER_SEARCH
		browserStop = 0xA9, //VK_BROWSER_STOP
		c = 'C',
		cancel = 0x03, //VK_CANCEL
		capital = 0x14, //VK_CAPITAL
		capsLock = 0x14, //VK_CAPITAL
		clear = 0x0C, //VK_CLEAR
		crsel = 0xF7, //VK_CRSEL
		ctrl = 0x11, //VK_CONTROL
		d = 'D',
		d0 = '0',
		d1 = '1',
		d2 = '2',
		d3 = '3',
		d4 = '4',
		d5 = '5',
		d6 = '6',
		d7 = '7',
		d8 = '8',
		d9 = '9',
		decimal = '.',
		deleteKey = 0x2E, //VK_DELETE
		divide = 0x6F, //VK_DIVIDE
		down = 0x28, //VK_DOWN
		e = 'E',
		end = 0x23, //VK_END
		enter = 0x0D, //VK_RETURN
		eraseEof = 0xF9, //VK_EREOF
		escape = 0x1B, //VK_ESCAPE
		execute = 0x2B, //VK_EXECUTE
		exsel = 0xF8, //VK_EXSEL
		f = 'F',
		f1 = 0x70, //VK_F1
		f10 = 0x79, //VK_F10
		f11 = 0x7A, //VK_F11
		f12 = 0x7B, //VK_F12
		f13 = 0x7C, //VK_F13
		f14 = 0x7D, //VK_F14
		f15 = 0x7E, //VK_F15
		f16 = 0x7F, //VK_F16
		f17 = 0x80, //VK_F17
		f18 = 0x81, //VK_F18
		f19 = 0x82, //VK_F19
		f2 = 0x71, //VK_F2
		f20 = 0x83, //VK_F20
		f21 = 0x84, //VK_F21
		f22 = 0x85, //VK_F22
		f23 = 0x86, //VK_F23
		f24 = 0x87, //VK_F24
		f3 = 0x72, //VK_F3
		f4 = 0x73, //VK_F4
		f5 = 0x74, //VK_F5
		f6 = 0x75, //VK_F6
		f7 = 0x76, //VK_F7
		f8 = 0x77, //VK_F8
		f9 = 0x78, //VK_F9
		finalMode = 0x18, //VK_FINAL
		g = 'G',
		h = 'H',
		hangulMode = 0x15, //VK_HANGUL
		hanjaMode = 0x19, //VK_HANJA
		help = 0x2F, //VK_HELP
		home = 0x24, //VK_HOME
		i = 'I',
		ImeAccept = 0x1E, //VK_ACCEPT
		ImeConvert = 0x1C, //VK_CONVERT
		ImeModeChange = 0x1F, //VK_MODECHANGE
		ImeNonconvert = 0x1D, //VK_NONCONVERT
		insert = 0x2D, //VK_INSERT
		j = 'J',
		junjaMode = 0x17, //VK_JUNJA
		k = 'K',
		kanaMode = 0x15, //VK_KANA
		kanjiMode = 0x19, //VK_KANJI
		l = 'L',
		launchApplication1 = 0xB6, //VK_LAUNCH_APP1
		launchApplication2 = 0xB7, //VK_LAUNCH_APP2
		launchMail = 0xB4, //VK_LAUNCH_MAIL
		lButton = 0x01, //VK_LBUTTON
		lControl = 0xA2, //VK_LCONTROL
		left = 0x25, //VK_LEFT
		lineFeed = 0x0a,
		lAlt = 0xA4, //VK_LMENU
		lShift = 0xA0, //VK_LSHIFT
		lWin = 0x5B, //VK_LWIN
		m = 'M',
		mButton = 0x04, //VK_MBUTTON
		mediaNextTrack = 0xB0, //VK_MEDIA_NEXT_TRACK
		mediaPlayPause = 0xB3, //VK_MEDIA_PLAY_PAUSE
		mediaPreviousTrack = 0xB1, //VK_MEDIA_PREV_TRACK
		mediaStop = 0xB2, //VK_MEDIA_STOP
		multiply = 0x6A, //VK_MULTIPLY
		n = 'N',
		next = 0x22, //VK_NEXT
		noName = 0xFC, //VK_NONAME
		none = 0x00,
		numLock = 0x90, //VK_NUMLOCK
		numPad0 = 0x60, //VK_NUMPAD0
		numPad1 = 0x61, //VK_NUMPAD1
		numPad2 = 0x62, //VK_NUMPAD2
		numPad3 = 0x63, //VK_NUMPAD3
		numPad4 = 0x64, //VK_NUMPAD4
		numPad5 = 0x65, //VK_NUMPAD5
		numPad6 = 0x66, //VK_NUMPAD6
		numPad7 = 0x67, //VK_NUMPAD7
		numPad8 = 0x68, //VK_NUMPAD8
		numPad9 = 0x69, //VK_NUMPAD9
		o = 'O',
		oem1 = 0xBA, //VK_OEM_1
		oem102 = 0xE2, //VK_OEM_102
		oem2 = 0xBF, //VK_OEM_2
		oem3 = 0xC0, //VK_OEM_3
		oem4 = 0xDB, //VK_OEM_4
		oem5 = 0xDC, //VK_OEM_5
		oem6 = 0xDD, //VK_OEM_6
		oem7 = 0xDE, //VK_OEM_7
		oem8 = 0xDF, //VK_OEM_8
		oemBackslash = 0xE2, //VK_OEM_102
		oemClear = 0xFE, //VK_OEM_CLEAR
		oemCloseBrackets = 0xDD, //VK_OEM_6
		oemcomma = 0xBC, //VK_OEM_COMMA
		oemMinus = 0xBD, //VK_OEM_MINUS
		oemOpenBrackets = 0xDB, //VK_OEM_4
		oemPeriod = 0xBE, //VK_OEM_PERIOD
		oemPipe = 0xDC, //VK_OEM_5
		oemPlus = 0xBB, //VK_OEM_PLUS
		oemQuestion = 0xBF, //VK_OEM_2
		oemQuotes = 0xDE, //VK_OEM_7
		oemSemicolon = 0xBA, //VK_OEM_1
		oemTilde = 0xC0, //VK_OEM_3
		p = 'P',
		pa1 = 0xFD, //VK_PA1
		packet = 0xE7, //VK_PACKET
		pageDown = 0x22, //VK_NEXT
		pageUp = 0x21, //VK_PRIOR
		pause = 0x13, //VK_PAUSE
		play = 0xFA, //VK_PLAY
		print = 0x2A, //VK_PRINT
		printScreen = 0x2C, //VK_SNAPSHOT
		prior = 0x21, //VK_PRIOR
		processKey = 0xE5, //VK_PROCESSKEY
		q = 'Q',
		r = 'R',
		rButton = 0x02, //VK_RBUTTON
		rControl = 0xA3, //VK_RCONTROL
		right = 0x27, //VK_RIGHT
		rAlt = 0xA5, //VK_RMENU
		rShift = 0xA1, //VK_RSHIFT
		rWin = 0x5C, //VK_RWIN
		s = 'S',
		scroll = 0x91, //VK_SCROLL
		select = 0x29, //VK_SELECT
		selectMedia = 0xB5, //VK_LAUNCH_MEDIA_SELECT
		separator = 0x6C, //VK_SEPARATOR
		shift = 0x10, //VK_SHIFT
		sleep = 0x5F, //VK_SLEEP
		snapshot = 0x2C, //VK_SNAPSHOT
		space = 0x20, //VK_SPACE
		subtract = 0x6D, //VK_SUBTRACT
		t = 'T',
		tab = 0x09, //VK_TAB
		u = 'U',
		up = 0x26, //VK_UP
		v = 'V',
		volumeDown = 0xAE, //VK_VOLUME_DOWN
		volumeMute = 0xAD, //VK_VOLUME_MUTE
		volumeUp = 0xAF, //VK_VOLUME_UP
		w = 'W',
		x = 'X',
		xButton1 = 0x05, //VK_XBUTTON1
		xButton2 = 0x06, //VK_XBUTTON2
		y = 'Y',
		z = 'Z',
		zoom = 0xFB, //VK_ZOOM
	};
	BALOR_NAMED_ENUM_MEMBERS(Key);


	/// Key と組み合わせてショートカットコマンドを表せる。
	struct Modifier {
		enum _enum {
			alt = 0x00010000, /// ALT キーを押しながら
			ctrl = 0x00020000, /// CTRL キーを押しながら
			shift = 0x00040000, /// SHIFT キーを押しながら
			mask = 0x00070000,
		};
		BALOR_NAMED_ENUM_MEMBERS(Modifier);
	};

	/// キーがロックされているかどうか。
	static bool isLocked(Key key);
	/// キーが押されているかどうか。ウインドウメッセージループによって状態が更新される。
	static bool isPushed(Key key);
	/// キーが押されているかどうか。ウインドウメッセージループによらずに現在の状態を取得する。
	static bool isPushedAsync(Key key);
	///	キーのリピートが始まるまでの時間。0～3（約250ms～1000msでハードによって違う）が返る。
	static int repeatDelay();
	///	キーのリピート速度。0～31（約2.5～30回リピート/秒でハードによって違う）が返る。
	static int repeatSpeed();
	/// キーの値を wchar_t に変換する。変換できないキーの場合は 0 を返す。
	static wchar_t toChar(Key key);
};



	}
}
