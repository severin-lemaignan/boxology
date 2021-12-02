# {{ name }}

**Version:** {{ version }}

{{ description }}

## Overview of modules

| **Node** | **id** | **Partner** | **Status** | **Inputs** | **Outputs** |
|----------|--------|-------------|------------|------------|-------------|
{% for node in nodes %}
| {{ node.name }} | {{ node.id }} | {{ node.label }} | {% if (node.generate) %} mock-up {% else %} released (version {{ node.version }}) {% endif %} {% if (node.bin == "") %} (dependency) {% endif %} | |
{% endfor %}


