
.. _topics:

ROS Topics
##########

This page list all the topics exposed by the SDK {{ sdk_version }}.

.. toctree::
   :maxdepth: 2

{% for node in nodes %}

{% for io in node.inputs %}
{% if (io.type == "topic") %}
   {{ io.topic }}
{% endif %}
{% endfor %}
{% for io in node.inputs %}
{% if (io.type == "topic") %}
   {{ io.topic }}
{% endif %}
{% endfor %}

{% endfor %}


