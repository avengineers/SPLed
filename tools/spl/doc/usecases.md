# Usecases

This document shows how spl can be used from the view of the project which uses it.

This serves as aid when designing the API of SPL.

## Usecase A: Use spl w/o any special things!

CMakelists.txt:
```
cmake_minimum_required(VERSION 3.22.0)

project(epes)

include(spl)
```


## Usecase B: Use spl in project EPES (short-term solution)

Project epes needs the standard epes stuff and also the extension "epes_extension" (it will get obsolete when we have extensions "a2l", "odx", etc.)

CMakelists.txt:
```
cmake_minimum_required(VERSION 3.22.0)

project(epes)

include(spl)

spl_install_extension(epes_extension)
```

## Usecase C: Use spl in project EPES (long-term solution)

Project epes needs the standard epes stuff and also the extension "epes_extension" (it will get obsolete when we have extensions "spl_a2l", "spl_odx", etc.)

CMakelists.txt:
```
cmake_minimum_required(VERSION 3.22.0)

project(epes)

include(spl)

spl_install_extension(a2l)
spl_install_extension(odx)
```
