# Tomb4Plus
## Extended open source Tomb Raider Level Editor reimplementation
Tomb4Plus is a fork of Troye's [Tomb4](https://github.com/Trxyebeep/TOMB4)  project, with the intention of extending the project to support the functionality of the Tomb Raider Level Editor and its associated binary-patched derivatives.

The intention is to provide backwards compatibility with as many custom Tomb Raider levels built on the Tomb Raider 4 engine as possible, as well as provide bug fixes and compatibility improvements for modern operating systems.

### Goals
Reimplement the functionality of Tomb Raider Level Editor with support for extended functionality from Tomb Raider Next Generation (TRNG), Tomb Raider Engine Patcher (TREP), FLEP (FLExible Patcher), and associated TRNG plugins.

### Instructions
* Before you run the program, you need a manifest file to be placed in the directory of the game you are attempting to run. You can download existing manifest files from [here](https://github.com/SaracenOne/tomb4_manifest_directory) or if you believe the engine has the features capable of running a particular custom level, you can attempt to generate your own manifest file with [this](https://github.com/SaracenOne/tomb4_feature_extractor) tool.
* You can then run the custom level be either playing the `tomb4plus.exe` file in the game's directory, or running it with the `PATH="path/to/directory/"` command line argument.
* When you first launch, you will see a setup screen. Choose your preferred setting click the `okay` button.
* If you wish to see the setup screen again, run `tomb4plus.exe` with the `-SETUP` command line argument.
* Unlike the original engine which stored configuration settings in the system registry, the new engine stores configuration options in an .ini file stored at `C:\Users\<YourName>\AppData\Roaming\Tomb4Plus\config.ini`. Likewise, savegames and screenshots are no longer stored in the game directory, but are instead stored in `C:\Users\<YourName>\AppData\Roaming\Tomb4Plus\game_data\<LevelName>\`. You can also run the executable in a directory with a file named `portable.txt` which will instead store configurations and saves in the exe's local directory if you would prefer.
* Enjoy exploring the fascinating world of unofficial Tomb Raider content.

### Contributing
Feel free to submit PRs for new features, bug fixes, and increased compatibility for TRLE levels. You can also submit issue reports, but due to the sheer scope of custom levels this engine meant to support, please only submit issues reporting bugs found in either retail games, custom levels which use the vanilla tomb4.exe file or levels which are included in [this](https://github.com/SaracenOne/tomb4_manifest_directory) repository.

The exception to this if you find an existing  implementation of a TRNG, FLEP, or TREP feature in this repository and you believe it to be implemented incorrectly. In such a case, please provide detailed instructions on how you believe it be inaccurate, and if possible, a minimum reproduction project (MRP) demonstrating exactly how the implementation is incorrect. MRPs will be highly valuable it testing the accuracy of this engine.

### License:
This project is licensed under the GNU General Public License - see the [LICENSE](https://github.com/SaracenOne/TOMB4/blob/level_editor_v2/LICENSE) file for details
