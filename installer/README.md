First you need to place all the i686 binaries within
`packages/com.4techlabs.ipheco.i686/data` and the x86_64 binaries within
`packages/com.4techlabs.ipheco.x86_64/data`.

A link from `/iph-eco.exe` will be created and placed on the start menu.

You must run binarycreator within this folder with the following command:

    binarycreator -c config\config.xml -p packages installer.exe

To generate an installer.exe file.
