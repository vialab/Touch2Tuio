Touch2Tuio
==========

Touch2Tuio source for executables shipped in vialab/SMT, based on http://dm.tzi.de/research/hci/touch2tuio/

Added support for Windows 8


Original Readme
==========

Touch2Tuio

A small program to translate and forward native Windows 7 touch messages to TUIO clients.

Requirements:

    * Multi-touch hardware supporting the Windows 7 API.
    * A TUIO-aware client application

Disclaimer:

This is very much work in progress and probably should not be used for production environments. We have tested it with our hardware and client applications and it worked fine for us but we do not guarantee that is runs with all hardware/software or runs stable in all situations.

Manual:

The current interface is command line only and there are not many options available as of yet. We plan to expose further options in the future. If you need more options right now, feel free to add them yourself (that's the beauty of the open source model) and let us know about it!

Basic usage:

1.) Start your TUIO-aware client application and write down the main window's title (for full screen applications you might have to check in the task manager).

2.) Start Touch2Tuio and provide the title of the client window as an argument, e.g., "touch2tuio.exe mytuioclientapp" (capitalization doesn't matter; most of the time the first couple of characters should suffice; if the window title contains spaces, only enter the part up to the first space).

3.) Hit "enter" to quit touch forwarding.

Parameters:

Touch2Tuio.exe <Window Title> (Optional: <Network Address> <Port> <OffsetX> <OffsetY> <Width> <Height>)

Window Title (required): The first couple of characters of you client application's window title (ONLY the part before the first space!!)

Network Address (optional): The IP address to forward TUIO messages to (defaults to 127.0.0.1)

Port (optional): The port to forward TUIO messages to (defaults to 3333)

OffsetX, OffsetY, Width, Height (optional, but if you enter one of the four, you'll have to specify all of them!): This is highly experimental and intended to provide some measure of control, especially in multi-monitor setups. This will disable auto-detection of the desktop size. These parameters allow you to specify a certain rectangle of your desktop that will be mapped to the application input. Note: In order to get the touch input at all, your application's window will still have to cover the touch area. This is mostly intended for rather exotic development and testing purposes.

Background:

Touch2Tuio uses Windows Hooks to grab the touch messages from the client application and to disable the default touch mouse emulation.

Note:

You need the 32-bit Touch2Tuio version for 32-bit TUIO clients (even on 64-bit systems) and 64-bit Touch2Tuio version for 64-bit TUIO clients. For some applications, especially game engines, you will also have to disable mouse support (if possible) from within your application (please consult your engine's manual/documentation).

Contact:

Marc or Benjamin
http://dm.tzi.de/research/hci/touch2tuio/