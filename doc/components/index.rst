Components
==========

{% for component_info in build_config.components_info %}
{% if component_info.has_docs %}

{{ component_info.long_name }}
--------------------

.. toctree::
    :maxdepth: 2

    /{{ component_info.path }}/doc/index
{% if component_info.has_tests %}
    /{{ component_info.output_dir }}/unit_test_spec
    /{{ component_info.output_dir }}/unit_test_results
    /{{ component_info.output_dir }}/doxygen/html/index
    /{{ component_info.output_dir }}/coverage
{% endif %}


{% endif %}
{% endfor %}
