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


// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

//
// some data will be shared across all
// instances of the DLL
//
#pragma data_seg(".SHARED")
// shared data
HMODULE			g_this(0);
DWORD			g_consoleId(0);
int				g_serverPort(3333);
char			g_host[CHAR_BUFFER_SIZE] = "127.0.0.1";
HWND			g_targetWnd(0);
HHOOK			g_hook(0);
HHOOK			g_mouseHook(0);
int				g_screen_offsetX(0);
int				g_screen_offsetY(0);
unsigned int	g_screen_width(0);
unsigned int	g_screen_height(0);
bool			g_detect_screen_size(true);
#pragma data_seg()
#pragma comment(linker, "/SECTION:.SHARED,RWS")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
	g_this = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
