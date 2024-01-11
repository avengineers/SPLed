{% if build_config.component_info %}

Software Component Report
#########################

| **Variant:** {{ build_config.variant }}
| **Component:** {{ build_config.component_info.long_name }}

.. toctree::
    :maxdepth: 2

    {{ build_config.component_info.path }}/doc/index
{% if build_config.component_info.has_reports %}
    {{ build_config.component_info.reports_output_dir }}/unit_test_spec
    {{ build_config.component_info.reports_output_dir }}/unit_test_results
    {{ build_config.component_info.reports_output_dir }}/doxygen/html/index
    {{ build_config.component_info.reports_output_dir }}/coverage
{% endif %}

{% else %}

Variant Report
##############

**Variant:** {{ build_config.variant }}

.. toctree::
    :maxdepth: 1
    :caption: Contents

    doc/customer_requirements/index
    doc/software_requirements/index
    doc/software_architecture/index
    doc/ai_example/index
    doc/components/index
{% if build_config.target == 'reports' %}
    {{ build_config.reports_output_dir }}/coverage
{% endif %}
    doc/results/index

{% endif %}
