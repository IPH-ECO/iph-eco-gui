function Component() {
    if (systemInfo.kernelType === "winnt") {
        if (systemInfo.currentCpuArchitecture === "i386") {
            QMessageBox.critical("cancel.wrong_architecture", "IPH-ECO Setup", "This application works only on 64 bit operating systems.");
            installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
            installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
            installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
            installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
            installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
            installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
            gui.clickButton(buttons.NextButton);
        }
        if (systemInfo.currentCpuArchitecture === "x86_64") {
            installer.setValue("TargetDir", "@RootDir@/Program Files/IPH-ECO");
        }
    }
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("Execute",
            "@TargetDir@/vcredist_x64.exe",
            "/install", "/passive", "/quiet");

        component.addOperation("CreateShortcut",
            "@TargetDir@/iph-eco.exe",
            "@StartMenuDir@/iph-eco.lnk",
            "workingDirectory=@TargetDir@");
        component.addOperation("CreateShortcut",
            "@TargetDir@/uninstall.exe",
            "@StartMenuDir@/Uninstall.lnk");
        component.addOperation("CreateShortcut",
            "@TargetDir@/iph-eco.exe",
            "@DesktopDir@/IPH-ECO.lnk");
    }
}