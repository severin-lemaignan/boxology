.. _{{ name }}:

``{{ label }}``
===============================================


{{ short_description }}

- Package version in PAL-SDK {{ sdk_version }}: ``{{ name }}-{{ version }}``

- Repository: {{ repo }}

- License: {{ license }}

.. graphviz::


   digraph deps {
   node [style="rounded"];
   "{{ name }}" [color = blue, style="bold"];

   "image_republisher" [URL="n___image_republisher.html"];
   "image_republisher" -> "{{ name }}" [label="head_front_camera/color/image_raw"];
   }

Description
-----------

{{ description }}

Node management
---------------


How to check the status of the node?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: sh

   # this  a copy-pastable snippet to check the status


.. code:: sh

   # this  a copy-pastable snippet using rosnode/rostopic to check the node is working



How to access the node's logs?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: sh

   # this  a copy-pastable snippet


How to start/stop/restart the node
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: sh

   # this  a copy-pastable snippet



Using in your code/application
------------------------------



Access via the robot's GUI
~~~~~~~~~~~~~~~~~~~~~~~~~~

[insert screenshots here]

Access via ROS standard tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: sh

   # this  a copy-pastable snippet



[insert screenshots here]

Using in Python
~~~~~~~~~~~~~~~

.. code:: python

   # this  a copy-pastable snippet


Using in C++
~~~~~~~~~~~~

.. code:: c++

   // this  a copy-pastable snippet



Reference
---------

Subscribed topics
~~~~~~~~~~~~~~~~~

{% for io in inputs %}
{% if (io.type == "topic") %}
- :ref:`{{ io.topic }}` ``[{{ join(io.datatype, "/") }}]``
{% endif %}
{% endfor %}

Published topics
~~~~~~~~~~~~~~~~
{% for io in outputs %}
{% if (io.type == "topic") %}
- :ref:`{{ io.topic }}` ``[{{ join(io.datatype,"/") }}]``
{% endif %}
{% endfor %}

Actions server
~~~~~~~~~~~~~~


Actions client
~~~~~~~~~~~~~~


Services
~~~~~~~~

Other inputs
~~~~~~~~~~~~

{% for io in inputs %}
{% if !(io.type == "topic") %}
 - {% if (io.type != "undefined") %} `{{ io.name }}` ({{ io.type }}) {% else %} {{ io.name }} {% endif %}
{% endif %}
{% endfor %}

Other outputs
~~~~~~~~~~~~~

{% for io in inputs %}
{% if !(io.type == "topic") %}
 - {% if (io.type != "undefined") %} `{{ io.name }}` ({{ io.type }}) {% else %} {{ io.name }} {% endif %}
{% endif %}
{% endfor %}

