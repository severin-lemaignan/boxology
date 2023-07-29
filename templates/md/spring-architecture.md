---
title: "{{ name }} -- version {{ version }}"
subtitle: "{{ description }}"
author: Séverin Lemaignan
---


# Overview of modules

| **Node** | **Partner** | **Status** | **Description** |
|----------|-------------|------------|-----------------|
{% for partner in sort(labels) %}
{% for node in sort(nodes) %}
{% if (node.label == partner) %}
| [{{ node.name }}](#{{ node.id }}) | {{ node.label }} | {% if (node.generate) %} not yet implemented {% else %} released {% endif %} {% if (node.bin == "") %} (dependency) {% endif %} | {{ node.short_description }}{% if (node.repo != "") %}. Code: [{{ node.repo }}]({{ node.repo }}){% endif %} |
{% endif %}
{% endfor %}
{% endfor %}

# Detailed description


{% for partner in sort(labels) %}


## {{ partner }}

{% for node in nodes %}
{% if (node.label == partner and node.bin != "") %}


### {{ node.name }} {#{{ node.id }}}

{{ node.description }}

*The node {{ node.name }} (id: `{{ node.id }}`) is maintained by {{ node.label }}.*

#### Status

{% if (node.generate) %} This node is not yet implemented.
{% else %}
Implemented. Current release/branch: {{ node.version }}

{% if (node.repo) %}
Source code repository: [{{ node.repo }}]({{ node.repo }})
{% endif %}
{% endif %}


#### Inputs

{% for io in node.inputs %}
{% if (io.type == "topic") %}
 - Topic subscription: `{{ io.topic }} [{{ join(io.datatype, "/") }}]`

{% else %}
 - Input: {% if (io.type != "undefined") %} `{{ io.name }}` ({{ io.type }}) {% else %} {{ io.name }} {% endif %}

{% endif %}
{% endfor %}

#### Outputs

{% for io in node.outputs %}
{% if (io.type == "topic") %}
 - Topic publication: `{{ io.topic }} [{{ join(io.datatype,"/") }}]`

{% else %}
 - Output: {% if (io.type != "undefined") %} `{{ io.name }}` ({{ io.type }}) {% else %} {{ io.name }} {% endif %}

{% endif %}
{% endfor %}

{% if node.dependencies %}
#### Dependencies

{% for dep in node.dependencies %}
- `{{ dep.0 }}/{{ dep.1 }}`
{% endfor %}

{% endif %}

{% endif %}
{% endfor %} {## end loop nodes ##}
{% endfor %} {## end loop partners ##}

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


