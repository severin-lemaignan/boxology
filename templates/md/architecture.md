---
title: "{{ name }} -- version {{ version }}"
subtitle: "{{ description }}"
---


# Overview of modules

| **Node** | **Category** | **Status** |
|----------|-------------|------------|
{% for node in nodes %}
| [{{ node.name }}](#{{ node.id }}) | {{ node.label }} | {% if (node.generate) %} not yet implemented {% else %} released (version {{ node.version }}) {% endif %} {% if (node.bin == "") %} (dependency) {% endif %} |
{% endfor %}

# Detailed description


{% for category in sort(labels) %}


## {{ category }}

{% for node in nodes %}
{% if (node.label == category and node.bin != "") %}


### {{ node.name }} {#{{ node.id }}}

{{ node.short_description }}

#### Overview

{{ node.description }}

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
{% endfor %} {## end loop category ##}

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


