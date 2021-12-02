# {{ name }}

**Version:** {{ version }}

{{ description }}

## Overview of modules

| **Node** | **id** | **Label** | **Version** | **Inputs** | **Outputs** |
|----------|--------|-----------|------------|------------|-------------|
{% for node in nodes %}
| {{ node.name }} | {{ node.id }} | {{ node.label }} | {{ node.version }} | | |
{% endfor %}


