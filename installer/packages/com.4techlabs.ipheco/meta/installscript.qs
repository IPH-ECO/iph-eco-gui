function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/iph-eco.exe",
                               "@StartMenuDir@/iph-eco.lnk",
                               "workingDirectory=@TargetDir@");
    }
}
