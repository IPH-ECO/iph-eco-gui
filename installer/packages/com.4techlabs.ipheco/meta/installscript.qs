function Component() {
    if (systemInfo.kernelType === "winnt") {
        installer.componentByName("com.4techlabs.ipheco.i686")
            .setValue("Virtual", "true");
        installer.componentByName("com.4techlabs.ipheco.x86_64")
            .setValue("Virtual", "true");

        if ( systemInfo.currentCpuArchitecture === "i386") {
            installer.componentByName("com.4techlabs.ipheco.i686")
                .setValue("Virtual", "false");
            installer.componentByName("com.4techlabs.ipheco.i686")
                .setValue("Default", "true");
        }
        if ( systemInfo.currentCpuArchitecture === "x86_64") {
            installer.componentByName("com.4techlabs.ipheco.x86_64")
                .setValue("Virtual", "false");
            installer.componentByName("com.4techlabs.ipheco.x86_64")
                .setValue("Default", "true");
        }
    }
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/iph-eco.exe",
                               "@StartMenuDir@/iph-eco.lnk",
                               "workingDirectory=@TargetDir@");
    }
}
