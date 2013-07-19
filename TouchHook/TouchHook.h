/*
 Touch2Tuio - Windows 7 Touch to TUIO Bridge
 http://dm.tzi.de/research/hci/touch2tuio
 
 Copyright (c) 2010 Marc Herrlich and Benjamin Walther-Franks, Research Group Digital Media, TZI, University of Bremen
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TOUCHHOOK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TOUCHHOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TOUCHHOOK_EXPORTS
#define TOUCHHOOK_API __declspec(dllexport)
#else
#define TOUCHHOOK_API __declspec(dllimport)
#endif

// Public API
TOUCHHOOK_API void SetConsoleId(DWORD consoleId);
TOUCHHOOK_API void SetTuioServerAddress(const char *host, int port);
TOUCHHOOK_API void SetWindowRect(int offsetX, int offsetY, int width, int height);
TOUCHHOOK_API bool InstallHookFromWindowTitle(const char *windowTitle);
TOUCHHOOK_API bool RemoveHook(void);
