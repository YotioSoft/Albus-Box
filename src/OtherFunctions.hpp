//
//  OtherFunctions.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef OtherFunctions_hpp
#define OtherFunctions_hpp

#pragma once

#include "Header.hpp"

Array<string> splitUTF8(string str, string separator);
Array<String> split(String str, String separator);

void cMes(Font& font, String str, Point topLeft, Size size, Color& color);

Point getStringTopLeft(Font& font, String str, Point topLeft, Point bottomRight);
Point getStringTopLeft(Font& font, String str, Point topLeft, Size size);

void saveTextFile(string filePath, string str);
void addTextFile(string filePath, string str);
void saveTextFile(string filePath, Array<string> strV);
void saveTextFile(string filePath, Array<FileStruct> fs);
void saveTextFile(string filePath, FileStruct fs);

void debugInit();
void debugLog(String str);

string replaceString(string beforeStr, string searchStr, string afterStr);

#endif /* OtherFunctions_hpp */
