# Garb

Garb writes text into the "unused" space of win32 binary files without any error checking. Used for creating easter eggs that no one every sees.

## Release

Version|Type|Date
---|---|---
v1.0|Internal|2005

## Source

Compile with Microsoft Visual Studio 6.

## History

Win32 binaries have a DOS executable (stub) embedded in the EXE header which is executed if you try and run the program under DOS. Usually it displays the message ``This program cannot be run in DOS mode.`` then quits. Using a custom stub reduces the binary file size but unfortunately Visual Studio 6 pads out the header with garbage. At the end of the binary there's usually many null bytes which don't seem to be used by anything. Let's replace the garbage and null bytes with text for any one who happens to open our binary in an editor.

## Usage

1. Place desired text into the config files
    1. ``stubtext.conf``: Text near the binary's stub
    1. ``message.conf``: Text at the end of the binary
        * If the text is too long it will silently overflow into the binary creating mayhem
        * Hex codes can be used with ``\xNN``
        * See the ``Example`` directory for examples
1. Run ``garb.exe file-to-edit.exe``
1. Test ``file-to-edit.exe``
    * You've messed with the internals of a binary file, better make sure it still works
