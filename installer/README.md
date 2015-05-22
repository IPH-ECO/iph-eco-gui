First you need to place all the binaries within
`packages/com.4techlabs.ipheco/data`.

A link from `packages/com.4techlabs.ipheco/data/iph-eco.exe` will be created and
placed on the start menu.

You must run binarycreator within this folder with the following command:

    binarycreator -c config\config.xml -p packages installer.exe

To generate an installer.exe file.
