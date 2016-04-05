# BlendManager
BlendManager is a program to manage various Blender version. It's able to open the version in which your .blend was made.

It uses Qt5 to show the user interface.

This is the result of 3 days of codes scavenging in Stack Overflow.

You may fork and modify the code, but please give credit to the original author (and the people in Stack Overflow). :)

# Usage

`BlendManager ... [.blend] ...`

BlendManager didn't accept any arguments (except maybe the Qt5 part) but pass all the arguments to the Blender, with an exception of detecting the version of the .blend supplied.

# Known Issues

- BlendManager may crashes when downloading Blender version without network connection.
- BlendManager may not show console output properly.
- BlendManager has not implemented autonomous update for versions.
- BlendManager has not implemented user-friendly versions (and settings) editing.
- BlendManager may not properly implement some features it has offered.
- BlendManager isn't written properly according to the safe practice, so memory leak and such problem may exist.

# Configuring

BlendManager relies on two .json file: `settings.json` and `versions.json`.
Both are read on startup.

Ps. all values regarding path must use `/` instead of `\`. (especially in Windows)

####settings.json

	"versionAutoSelect": [true|false]

Automagically detect your .blend version, if one is supplied through command-line argument.

	"forceLatest": [true|false]

Force BlendManager to use latest Blender version available on your current settings.

	"useLocalOnly": [true|false]

Prohibit downloading newer Blender version.
	
	"forceArguments" : [...]

Add additional arguments to later Blender execution.

	"ignoreCommandLineArguments" : [true|false]

Ignore any command-line given. (though this may not be implemented well)

	"defaultDownloadDir": "..."

Set temporary download directory. This directory must exist.

	"defaultBlenderRoot": "..."

Set default extraction path of Blender (to store downloaded versions).

	"extractCommandExecutable" : "..."

Set the command to extract downloaded Blender version archive.

	"extractCommandArguments" : [...]

Set the arguments to extract archive. (using `%{FILE}` and `%{OUTPUT_DIR}`)

####versions.json

	"...": {

Set the name for the version. (default is the archive name)

		"OS": "...",

Set the target operating system.
>These are the options:
  BLENDER_OS_WIN32,
  BLENDER_OS_WIN64,
  BLENDER_OS_LINUX32,
  BLENDER_OS_LINUX64,
  BLENDER_OS_MACOSX64,
  BLENDER_OS_FREEBSD64

		"branch": "...",

Set branch name to the version. `vanilla` being the default branch.

		"downloadHash": "...",

Set the md5 hash of the version. (Although checking mechanism is not implemented yet.

		"downloadURL": "..."

Set the download URL for the version.

		"localArguments": [...]

Set additional argument to pass.

		"localAvailable": [true|false]

Set the availability of version. (is it downloaded yet?)

		"localExecutable": "..."

Set the executable path. (must be full absolute path)

		"localPath": "..."

Set the working path. Usually where `localExecutable` exists.

		"major": [int]

Set the major version. 

		"minor": [int]

Set the minot version.

	}

