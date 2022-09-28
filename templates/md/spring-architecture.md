# {{ name }}

**Version:** {{ version }}

{{ description }}

## Overview of modules

| **Node** | **Partner** | **Status** |
|----------|-------------|------------|
{% for node in nodes %}
| [{{ node.name }}](#{{ node.id }}) | {{ node.label }} | {% if (node.generate) %} mock-up {% else %} released (version {{ node.version }}) {% endif %} {% if (node.bin == "") %} (dependency) {% endif %} |
{% endfor %}

## Detailed description


{% for node in nodes %}
{% if (node.bin != "") %}


### {{ node.name }} {#{{ node.id }}}

{{ node.description }}

*The node {{ node.name }} (id: `{{ node.id }}`) is maintained by {{ node.label }}.*

#### Status

{% if (node.generate) %} **This node is currently auto-generated (mock-up)** {% else %} **Current release: {{ node.version }}** {% endif %}


#### Inputs/outputs

{% for io in node.inputs %}
 - Input: `{{ io.name }}` ({{ io.type }})
{% endfor %}

{% for io in node.outputs %}
 - Output: `{{ io.name }}` ({{ io.type }})
{% endfor %}

#### Dependencies

{% for dep in node.dependencies %}
- `{{ dep.0 }}/{{ dep.1 }}`
{% endfor %}

{% endif %}
{% endfor %}

{% for node in nodes %}
{% if (node.bin == "") %}

### Non-executable dependency: {{ node.name }}

Module {{ node.name }} (id: `{{ node.id }}`) is overseen by {{ node.label }}.

{{ node.description }}


#### Status

{% if (node.generate) %} **This code is currently auto-generated (mock-up)** {% else %} **Current release: {{ node.version }}** {% endif %}


#### Dependencies

{% for dep in node.dependencies %}
- `dep.0/dep.1`
{% endfor %}



{% endif %}
{% endfor %}

