Components
==========

{% for component_info in build_config.components_info %}
{% if component_info.has_docs %}

{{ component_info.long_name or component_info.name }}
-----------------------------------------------------

.. toctree::
    :maxdepth: 2

    /{{ component_info.path }}/doc/index
{% if (build_config.target == 'reports') and component_info.has_reports %}
    /{{ component_info.reports_output_dir }}/unit_test_results
    /{{ component_info.reports_output_dir }}/doxygen/html/index
    /{{ component_info.reports_output_dir }}/coverage
{% endif %}


{% endif %}
{% endfor %}
