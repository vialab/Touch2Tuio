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

// TouchHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TouchHook.h"

// fwd declaration
LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

// from dllmain.cpp
extern HMODULE		g_this;
extern DWORD		g_consoleId;
extern int			g_serverPort;
extern char			g_host[CHAR_BUFFER_SIZE];
extern HWND			g_targetWnd;
extern HHOOK		g_hook;
extern HHOOK		g_mouseHook;
extern int			g_screen_offsetX;
extern int			g_screen_offsetY;
extern unsigned int	g_screen_width;
extern unsigned int	g_screen_height;
extern bool			g_detect_screen_size;


// process private data
const unsigned int TIMER_IDENTIFIER(123);
const unsigned int TIMER_CALL_TIME(100);
const unsigned int MAX_CURSOR_IDLE_TIME(300);
const ULONG TOUCH_FLAGS_2(/*TWF_FINETOUCH|*/TWF_WANTPALM);

bool				s_initialized(false);
HANDLE				s_out(0);
UINT_PTR			s_timer(0);

typedef std::map<DWORD, TUIO::TuioCursor*> CursorMap;
typedef std::list<HWND> HandleList;

std::auto_ptr<TUIO::TuioServer> s_server(0);
std::auto_ptr<CursorMap> s_cursors(0);
std::auto_ptr<CursorMap> s_cursorBuf(0);
std::auto_ptr<HandleList> s_windows(0);

char				s_buf[CHAR_BUFFER_SIZE] = "";
DWORD				s_ccount(0);

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	if (hwnd)
	{
		s_windows->push_back(hwnd);
	}
	return TRUE;
}

static void InitializeWindowsMap(void)
{
	s_windows.reset(new HandleList);
	EnumWindows(EnumWindowsProc, 0);
}

static void ToLowerString(std::string &s)
{
	for (size_t i = 0; i < s.length(); ++i)
	{
		s[i] = tolower(s[i]);
	}
}

static HWND FindWindowHandle(const char *windowTitle)
{
	char buf[CHAR_BUFFER_SIZE];
	std::string searchTitle(windowTitle);
	ToLowerString(searchTitle);
	for (HandleList::iterator it = s_windows->begin(); it != s_windows->end(); ++it)
	{
		GetWindowText(*it, buf, sizeof(buf));
		std::string s(buf);
		ToLowerString(s);
		if (s.find(searchTitle) == 0)
		{
			return *it;
		}
	}
	return 0;
}

TOUCHHOOK_API void SetConsoleId(DWORD consoleId)
{
	g_consoleId = consoleId;
}

TOUCHHOOK_API void SetTuioServerAddress(const char* host, int port)
{
	strcpy_s(g_host, host);
	g_serverPort = port;
}

TOUCHHOOK_API void SetWindowRect(int offsetX, int offsetY, int width, int height)
{
	g_screen_offsetX = offsetX;
	g_screen_offsetY = offsetY;
	g_screen_width = width;
	g_screen_height = height;
	g_detect_screen_size = false;
}

TOUCHHOOK_API bool InstallHookFromWindowTitle(const char *windowTitle)
{
	InitializeWindowsMap();
	g_targetWnd = FindWindowHandle(windowTitle);
	if (!g_targetWnd)
		return false;

	unsigned long threadId = GetWindowThreadProcessId(g_targetWnd, 0);

	if (!threadId)
		return false;

	g_hook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_this, threadId);
	return (g_hook) ? true : false;
}

TOUCHHOOK_API bool RemoveHook(void)
{
	if (!g_hook || ! g_targetWnd)
		return false;

	BOOL ret1 = UnhookWindowsHookEx(g_hook);
	g_hook = 0;

	BOOL ret2 = UnhookWindowsHookEx(g_mouseHook);
	g_mouseHook = 0;

	//make sure to clear everything on remove
	g_targetWnd = NULL;
	s_windows.reset();

	return (ret1 && ret2);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!s_initialized)
	{
		if (!AttachConsole(g_consoleId))
		{
			FreeConsole();
			AttachConsole(g_consoleId);
		}
		s_out = GetStdHandle(STD_OUTPUT_HANDLE); // TODO: we don't really close it
		sprintf_s(s_buf, "Attached console\n");
		WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);

		if (!g_mouseHook)
		{
			g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_this, 0); // mouse_ll global only (requires process with message loop)
			if (g_mouseHook)
			{
				sprintf_s(s_buf, "Installed mouse hook\n");
				WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);
			}
			else
			{
				sprintf_s(s_buf, "Could not install mouse hook\n");
				WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);
			}
		}

		if (g_detect_screen_size)
		{
			HWND win = GetDesktopWindow();
			RECT rec;
			GetWindowRect(win, &rec);
			g_screen_height = rec.bottom;
			g_screen_width = rec.right;
		}

		sprintf_s(s_buf, "X-Offset: %i Y-Offset: %i Width: %i Height: %i\n", g_screen_offsetX, g_screen_offsetY, g_screen_width, g_screen_height);
		WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);

		s_cursors.reset(new CursorMap);
		s_cursorBuf.reset(new CursorMap);

		s_server.reset(new TUIO::TuioServer(g_host, g_serverPort));
		sprintf_s(s_buf, "Created server (%s:%i)\n", g_host, g_serverPort);
		WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);

		s_timer = SetTimer(g_targetWnd, TIMER_IDENTIFIER, TIMER_CALL_TIME, 0);

		s_initialized = true;
	}

	if (nCode < 0) // do not process message 
	{
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	switch (nCode) 
	{ 
	case HC_ACTION: 
		{
			LPMSG msg = (LPMSG)lParam;

			// register everything for touch
			// Note: pretty ugly but seems to be the only possibility to register every window
			RegisterTouchWindow(msg->hwnd, TOUCH_FLAGS_2);
			
			switch (msg->message)
			{
			case WM_POINTERUP:
				{
					s_server->initFrame(TUIO::TuioTime::getSessionTime());
					s_server->removeTuioCursor((*s_cursors)[GET_POINTERID_WPARAM(msg->wParam)]); 
					s_server->commitFrame();
					std::map<DWORD, TUIO::TuioCursor*>::iterator cursorIt = s_cursors->find(GET_POINTERID_WPARAM(msg->wParam));
					s_cursors->erase(cursorIt);
				}
				break;
			case WM_POINTERDOWN:
				{
					s_server->initFrame(TUIO::TuioTime::getSessionTime());
					POINTS p = MAKEPOINTS(msg->lParam);
					(*s_cursors)[GET_POINTERID_WPARAM(msg->wParam)] = s_server->addTuioCursor(
															((p.x)+g_screen_offsetX)/(float)(g_screen_width+g_screen_offsetX),
															((p.y)+g_screen_offsetY)/(float)(g_screen_height+g_screen_offsetY));
					s_server->commitFrame();
				}
				break;
			case WM_POINTERUPDATE:
				{
					s_server->initFrame(TUIO::TuioTime::getSessionTime());
					POINTS p = MAKEPOINTS(msg->lParam);
					s_server->updateTuioCursor((*s_cursors)[GET_POINTERID_WPARAM(msg->wParam)],
												((p.x)+g_screen_offsetX)/(float)(g_screen_width+g_screen_offsetX),
												((p.y)+g_screen_offsetY)/(float)(g_screen_height+g_screen_offsetY));
					s_server->commitFrame();
				}
				break;
			case WM_TOUCH:
				{
					sprintf_s(s_buf, "Touches received:\n");
					WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);

					UINT cInputs = LOWORD(msg->wParam);
					PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
					if (NULL != pInputs)
					{
						if (GetTouchInputInfo((HTOUCHINPUT)msg->lParam,
							cInputs,
							pInputs,
							sizeof(TOUCHINPUT)))
						{
							s_server->initFrame(TUIO::TuioTime::getSessionTime());

							// process pInputs
							for (UINT i=0; i<cInputs; ++i)
							{
								sprintf_s(s_buf, "ID: %i, X: %i, Y: %i\n", pInputs[i].dwID, pInputs[i].x, pInputs[i].y);
								WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);

								std::map<DWORD, TUIO::TuioCursor*>::iterator cursorIt = s_cursors->find(pInputs[i].dwID);
								if (cursorIt != s_cursors->end())
								{
									// found
									s_server->updateTuioCursor(cursorIt->second,
															   ((pInputs[i].x*0.01f)+g_screen_offsetX)/(float)(g_screen_width+g_screen_offsetX),
															   ((pInputs[i].y*0.01f)+g_screen_offsetY)/(float)(g_screen_height+g_screen_offsetY));
									if(!(pInputs[i].dwFlags & TOUCHEVENTF_UP))
									{
										(*s_cursorBuf)[pInputs[i].dwID] = cursorIt->second;
										s_cursors->erase(cursorIt);
									}
								}
								else
								{
									// not found
									(*s_cursorBuf)[pInputs[i].dwID] = s_server->addTuioCursor(
															((pInputs[i].x*0.01f)+g_screen_offsetX)/(float)(g_screen_width+g_screen_offsetX),
															((pInputs[i].y*0.01f)+g_screen_offsetY)/(float)(g_screen_height+g_screen_offsetY));
								}
							}
							
							for (std::map<DWORD, TUIO::TuioCursor*>::iterator it = s_cursors->begin(); it != s_cursors->end(); it++)
							{
								s_server->removeTuioCursor(it->second);
							}
							
							s_cursors->clear();
							std::auto_ptr<CursorMap> tmp(s_cursorBuf);
							s_cursorBuf = s_cursors;
							s_cursors = tmp;	
							s_server->commitFrame();
						}
					}
					delete[] pInputs;
				}
				break;
			case WM_TIMER:
				{
					if (msg->wParam == TIMER_IDENTIFIER)
					{
						BOOL frameOpen(false);
						for (std::map<DWORD, TUIO::TuioCursor*>::iterator it = s_cursors->begin(); it != s_cursors->end();)
						{
							long delta = TUIO::TuioTime::getSessionTime().getTotalMilliseconds() - it->second->getTuioTime().getTotalMilliseconds();
							if (delta > MAX_CURSOR_IDLE_TIME)
							{
								if (!frameOpen)
								{
									s_server->initFrame(TUIO::TuioTime::getSessionTime());
									frameOpen = true;
								}
								s_server->removeTuioCursor(it->second);
								std::map<DWORD, TUIO::TuioCursor*>::iterator tmp = it++;
								s_cursors->erase(tmp);
								sprintf_s(s_buf, "Removed cursor due to time limit\n");
								WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);
							}
							else
							{
								++it;
							}
						}
						if (frameOpen)
						{
							s_server->commitFrame();
						}
					}
				}
				break;
			}
		}
		break; 
	}

	return CallNextHookEx(0, nCode, wParam, lParam); 
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0) // do not process message 
	{
		return CallNextHookEx(0, nCode, wParam, lParam);
	}

	LPMSLLHOOKSTRUCT msg = (LPMSLLHOOKSTRUCT)lParam;
	if (msg->flags & LLMHF_INJECTED) // block injected events (in most cases generated by touches)
	{
		sprintf_s(s_buf, "Blocked injected (touch) mouse event\n");
		WriteConsole(s_out, s_buf, strlen(s_buf), &s_ccount, 0);
		return 1;
	}

	// forward event
	return CallNextHookEx(0, nCode, wParam, lParam);
}
