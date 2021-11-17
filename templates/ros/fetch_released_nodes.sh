#! /usr/bin/sh

# This script clone all the SPRING ROS nodes that are marked as released in the
# architecture design.

{% for node in nodes %}
{% if not node.generate %}
if cd src/{{ node.id }}; 
then
    echo "{{ node.id }}: pulling latest changes from {{ node.version }}...";
    git pull origin {{ node.version }}; 
else 

    echo "Cloning {{ node.name }} (ROS node: {{ node.id }}), branch: {{ node.version }} to src/ ..."
    git clone --depth 1 --branch {{ node.version }} {{ node.repo }} src/{{ node.id }};
fi
{% endif %}
{% endfor %}
