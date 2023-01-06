# Build Binaries

We assume you want to build variant **alpha**.
**alpha** is a theoretical variant, the name of your variants might be different.

## Visual Studio Code

In the blue ribbon at the bottom (1) select a variant like _alpha_, (2) and the Kit _prod_ ,(3) choose the build target _elf_ and (4) click on build:
![vscode-build](img/vscode-build.png)

![vscode-build](img/build-binaries.gif)

## Command Line

The following shows the steps/commands when you use Windows PowerShell to do so.

```powershell
cd <Your root directory of repository>

.\modules\spl\powershell\spl.ps1 -build -target link
```

## Binary Location

After the build all binaries can be found under `/build/{variant}`.