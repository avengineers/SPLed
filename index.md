# Variant Report

:::{if} var.build_config.variant == "Disco"
**Variant:** Disco
:::

:::{if} var.build_config.variant == "Sleep"
**Variant:** Sleep
:::

:::{if} var.build_config.variant == "Spa"
**Variant:** Spa
:::

:::{if} var.build_config.variant == "Base/Dev"
**Variant:** Base/Dev
:::

:::{if} var.build_config.variant == "IDEA/Sloemada"
**Variant:** IDEA/Sloemada
:::

::::{if} var.build_config.target == "reports"
```{toctree}
:maxdepth: 1
:caption: Contents
:glob:

doc/software_architecture/index
doc/sw_requirements/index
doc/components/index
doc/results/index
/build/**/test/Debug/reports/coverage
```
::::

::::{if} var.build_config.target != "reports"
```{toctree}
:maxdepth: 1
:caption: Contents

doc/software_architecture/index
doc/sw_requirements/index
doc/components/index
doc/results/index
```
::::
