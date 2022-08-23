# Import new Dimension Projects

The following steps must be performed in order to import a Dimensions Make project into a SPL Variant that can be pushed to Git.

1. Cloning EPES/epes (this repository)
2. for each project to import
   1. Download Dimensions repository
   2. Run `build.bat --import` to import a Dimensions repository as a new variant in SPL
   3. optional: small adaptions to variant.cmake, toolchain.cmake and LDFile might be required
   4. Git commit
3. Git Push

![](doc/img/workflow-sequence.png)

## Get Dimensions Repository

Open 'Dimensions Desktop Client' and login. Then press `ctrl + w` to open a new project and type in your id, like M337103_HV:IMPLEMENTATION_PYRO_X200_TIGER_REV-C or M337103_HV:IMPLEMENTATION_ISO_X200_TIGER_REV-C.

You can choose any location for your working area. If the directory does not exist, it will be created automatically. An example location could be: 'C:\Repos\Dimensions\ISO_X200_TIGER'

> **⚠ _NOTE:_** Do not use the 'top-of-stack' of a Dimensions repository. Only use baselines as reference. Otherwise traceability will not work.

In order to import the Dimensions repository, it is not required to download it completely. We are usually only interested in the 'Impl' directory (thirdparty might be relevant in some cases, when there are additional customer libraries, above examples do not have those libs). The directories `Impl/Bld`, `Impl/Cfg` and `Impl/Src` must exist with those names, rename might be required. To download only 'Impl', you must select the 'Impl' directory and then `right-click` and select `Get (recursive)` as shown below:

![](doc/img/get-dimensions-repo.png)

> **⚠ _IMPORTANT:_** You must remove the read-only flag on all files that were downloaded from Dimensions. Otherwise import will fail.

Repeat this step for every Dimensions project that you want to import.

## Get EPES Repository

Clone from `git-shell` or using a client of your choice.

```shell
cd C:\Repos
git clone ssh://git@git.marquardt.de:7999/epes/epes.git
```

## Import Instruction

> **⚠ _NOTE:_** On the very first attempt, an intallation error might happen (if python was not yet available on your system). In that case, please just re-run it another time inside a new command line window or by double-clicking it again. ![](doc/img/installation-issue.png)

The import mode of `build.bat` requires 2 commandline parameters:

`build.bat --import --source <path to dimensions repository> --variant <name of variant>`

* source: Location of Dimensions project containing an 'Impl' directory, e.g. C:\Repos\Dimensions\ISO_X200_TIGER
* variant: Variant name (<platform>/<subsystem>, e.g. VW_PPE_Porsche_983/BMS_HV_PYRO)

`build.bat --import --source C:\Repos\Dimensions\ISO_X200_TIGER --variant VW_PPE_Porsche_983/BMS_HV_PYRO`

This call will import variant and sources under 'C:\Repos\epes' (epes root folder). If it is the first import into this directory, it will create the root files and add all variant and source files, based on the input of 'C:\Repos\Dimensions\ISO_X200_TIGER'. The new variant will be called 'VW_PPE_Porsche_983/BMS_HV_PYRO'. You will find the variant files under `/variants/VW_PPE_Porsche_983/BMS_HV_PYRO/` and the source files under `/legacy/VW_PPE_Porsche_983/BMS_HV_PYRO/`.

> **_NOTE:_** The source files are imported as legacy. After the import they are still in the same structure as they used to be under Dimensions. From CMake perspective they are considered as one SW-component. Modularization must be done manually afterwards. But they are already build with the new toolchain.

If you have an imported project already, it will add a new variant and its sources, keeping the root files. You must make sure to use a new variant name in such a case, otherwise you will overwrite the existing imported project.

Now both the imported new sources + the older sources from EPES/epes repositories are available on your disk. Either copy and overwrite the files in EPES/epes or use a diff and merge tool of your choice (e.g. Beyond Compare or Araxis Merge) to resolve conflicts if necessary.

## Manual Adaptions

It might occur that you will see changes on Git.

Changes in `toolchain.cmake` file: There might be project specific compiler flags that are used in Dimensions which are not part of the generic compile flags. In such case the line

```cmake
set(COMPILE_C_FLAGS -vle -DC_ENABLE_INSTRUCTION_SET_VLE -D__vle --prototype_warnings -Ospeed -Wundef -Wimplicit-int -Wshadow -Wtrigraphs --prototype_errors -g -dual_debug  -cpu=core_ppce200z425 -noobj -inline_prologue -pragma_asm_inline -noSPE --no_exceptions --no_commons -nokeeptempfiles -pnone --incorrect_pragma_warnings --slash_comment --long_long -nostdlib -preprocess_assembly_files -D__ghs__ -DCPP_VERSIONCHECK -DGHS -list  -passsource -DBRS_DERIVATIVE_SPC58NG84 -DBRS_OSC_CLK=32 -DBRS_TIMEBASE_CLOCK=180 -DBRS_OS_USECASE_OSGEN7 -DBRS_EVA_BOARD_VEBN01620 -DBRS_PLATFORM_SPC58xx -DBRS_COMP_GHS -DBRSHW_INSTRUCTION_SET_VLE -DBRSHW_CPU_NO_OF_CORES=3 -DUSE_FBLB003=1 -DSFDC_NO_EB_STDLIB -Ospeed -c99 -G -DSOME_FEATURE)
```

or similar could have additional entries like `-DSOME_FEATURE`. If that's the case, discard the change of this line and instead add

```cmake
set(VARIANT_ADDITIONAL_COMPILE_C_FLAGS -DSOME_FEATURE)
```

To the new created `config.cmake`.

Changes in linker script files like `PPE.ld`: The Greenhills compiler, and other compilers might too, uses an unwanted object file extension `.o`, but we expect `.c.o` to see the origin of the created object. This must be changed inside the linker script file. BUT be careful to only change the extension for objects that are coming from the SPL. If there are objects of external linked libraries, those referenced objects must use their original extensions.

Further manual steps are necessary:
* Removal of BSW/Bsw from legacy/\*\*/parts.cmake and remove directory from legacy/\*\*/BSW.
* Removal of BSW/MCAL from legacy/\*\*/parts.cmake and remove directory from legacy/\*\*/MCAL.
* Add Autosar configuration (activate or deactivate) in variants/\*\*/config.cmake.
* Add CBD package to variants/\*\*/parts.cmake.
* Adapt imported Vector DaVinci files by changing relative pathes and replacing "CBD123456" with real CBD package.

## CI

Add the imported project into selftests under /test/test_*.py

## Push Imported Changes

Last thing you need to do, is committing and pushing your changes to Git and Bitbucket. You can do this again from command line or in the Git client of your choice.

```batch
C:
cd C:\Repos\epes
git add .
git checkout -B feature/<branchname>
git commit -m"EPES-123: Transformer Version = <commit id>, Dimensions baseline = <project-id:branch:baseline>"
git push
```

## Traceability

In order to be able to trace all imports back from Git to Dimensions. We should consider some rules:

  1. Only use Dimensions baselines, do not use 'top-of-stack'
  2. Branchname in EPES/epes should contain a valid Jira issue id, e.g. `feature/EPES-123-import-ISO-X200-TIGER`
  3. Commit message should start with a valid Jira issue id, like `EPES-123:`
  4. The commit message should furthermore contain the commit id of the currently used Transformer (the backend tool that is used for importing). During import the Transformer should print something like this `Transformer Version = e40830c` which you can use.
  5. Commit message should also contain the Dimensions source baseline, like `Dimensions baseline = M337103_HV:IMPLEMENTATION_ISO_X200_TIGER_REV-C`

[//]: <> ( Next lines shall always be at the end of the file )

[//]: <> ( Get-History -count 15  | Select-Object -ExpandProperty CommandLine | clip )
