#! /usr/bin/sh

# This script clone all the SPRING ROS nodes that are marked as released in the
# architecture design.

{% for node in nodes %}
{% if not node.generate %}
echo "Cloning {{ node.name }} (ROS node: {{ node.id }}), branch: {{ node.version }} to src/ ..."
git clone --depth 1 --branch {{ node.version }} {{ node.repo }} src/{{ node.id }}
{% endif %}
{% endfor %}
