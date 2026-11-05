# Components

{% if build_config.target == 'reports' %}
```{toctree}
:maxdepth: 2

{% for component_info in build_config.components_info %}
{% if component_info.has_docs %}
/{{ build_config.reports_output_dir }}/{{ component_info.name }}_index
{% endif %}
{% endfor %}
```
{% else %}
```{toctree}
:maxdepth: 2

{% for component_info in build_config.components_info %}
{% if component_info.has_docs %}
{{ component_info.long_name or component_info.name }} </{{ component_info.path }}/doc/index>
{% endif %}
{% endfor %}
```
{% endif %}
