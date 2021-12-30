﻿//
//  Specific.hpp ( for Windows )
//  Albus Box
//
//  Created by YotioSoft on 2019/11/30.
//
//  注 : このファイルはWindows向けのヘッダファイルです。
//

#pragma once

#include <filesystem>
#include <direct.h>

#include <OtherFunctions.hpp>

/* OS依存の定数など */
#define OS						"Windows"	// OS

namespace specific {
	Array<FileStruct> getAllFilesName(string folderPath, string extension);

	void chdir(string dir);
	String getCurrentDir();
	String getSettingFilePath();
	String getAudioProfilesFilePath();
	String getFontsDir();

	bool isCursorEntered(Vec2& before_cursor_pos);
	void changeCursor();

	double getDpiDist();

	bool setWindowStyle(int x1, int y1, int x2, int y2, int w, int h);

	void moveWindow(Point& mouse_clicked, bool& window_moving);

	const String SaveDataFilePath = U"";
}
