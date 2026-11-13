# Components

::::{if} var.build_config.target == "reports"
```{toctree}
:maxdepth: 2
:glob:

/build/**/reports/*_index
```
::::

::::{if} var.build_config.target != "reports"
```{toctree}
:maxdepth: 2
:glob:

/**/doc/index
```
::::
