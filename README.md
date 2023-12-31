# Screensavers pack

Are you tired of the boring default screensavers that come with your computer? Do you want to add some personality and style to your desktop?

![sellergif](Documents/seller.gif)

Look no further than our Windows screensavers pack, featuring three stunning and unique screensavers that will transform your screen into a captivating visual experience!

First up, we have the bouncing circles screensaver. Watch as colorful circles bounce and move across your screen in a mesmerizing dance. This dynamic and energetic screensaver is perfect for adding a pop of lively and vibrant movement to your desktop.

Next, we have the moiré screensaver. This abstract and hypnotic screensaver features intricate and ever-shifting geometric patterns that create a stunning visual effect. With its mesmerizing and meditative qualities, the moiré screensaver is perfect for adding a sense of calm and transcendence to your workspace.

Last but not least, we have the screen melt effect screensaver. Watch as your screen becomes a liquid, melting and shifting in an otherworldly display of movement and transformation. This surreal and captivating screensaver is sure to amaze and delight anyone who views it.

Whether you want to add a touch of playfulness, elegance, or surrealism to your desktop, our Windows screensavers pack has something for everyone. Say goodbye to dull and outdated screensavers and elevate your screen with these mesmerizing and engaging options. Download our screensavers pack now and take your desktop to the next level.


## Command-line arguments

```
ScreenSaver           - Show the Settings dialog box.
ScreenSaver /c        - Show the Settings dialog box, modal to the foreground window.
ScreenSaver /p <HWND> - Preview Screen Saver as child of window <HWND>.
ScreenSaver /s        - Run the Screen Saver.
```
HWND is an HWND presented on the command line as an unsigned decimal number.


## Building

Screensavers pack uses the Microsoft Visual Studio 2022 for its builds.

To build Screensavers pack from source files with Microsoft Visual Studio, you can use either the graphical mode or the command-line mode. Here are the instructions for both methods:

### Graphical mode
1. Open Microsoft Visual Studio and select Open a project or solution from the start page or the File menu.
2. Browse to the folder where the .sln file is located and select it. This will load the project in Microsoft Visual Studio.
3. Select the configuration and platform for the project by using the drop-down menus on the toolbar. For example, you can choose Debug or Release for the configuration, and x86 or x64 for the platform.
4. Build the project by clicking on the Build menu and selecting Build Solution. You can also use the keyboard shortcut Ctrl+Shift+B.
5. Run the project by clicking on the Debug menu and selecting Start Debugging. You can also use the keyboard shortcut F5

### Command-line mode
1. Open a Developer Command Prompt for Microsoft Visual Studio. You can find it in the Start menu under Microsoft Visual Studio Tools.
2. Navigate to the folder where the .sln file is located by using the cd command.
3. Invoke the MSBuild tool to build the project. You can specify various options and flags for the tool. For example, the following command builds the project with the Release configuration and the x64 platform:
```
msbuild Screensavers.sln /p:Configuration=Release /p:Platform=x64
```
4. Run your executable by typing its name in the command prompt. For example:
```
Moire-exe-x86-64\moire.scr /S
```

## Authors

This program was written and is maintained by SV Foster.


## License

This program is available under EULA, see [EULA text file](EULA.txt) for the complete text of the license. This program is free for personal, educational and/or non-profit usage.
