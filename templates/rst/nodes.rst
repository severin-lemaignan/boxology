
.. _nodes:

ROS Nodes
#########

This page list all the nodes available in the SDK {{ sdk_version }}. They are
installed and running by default on the robot.


{% for category in sort(labels) %}

{{ category }}
--------------------------------------------------

.. toctree::
   :maxdepth: 2

{% for node in nodes %}
{% if (node.label == category) %}
   {{ node.name }}
{% endif %}
{% endfor %}
{% endfor %}



