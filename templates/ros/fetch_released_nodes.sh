#! /usr/bin/sh

# This script clone all the SPRING ROS nodes that are marked as released in the
# architecture design.

{% for node in nodes %}
{% if not node.generate %}

NODE_PATH=src/{{ node.id }}
if [ -d "$NODE_PATH" ]; then
    rm -rf $NODE_PATH;
fi
cd $NODE_PATH
{% if existsIn(node, "repo_subfolder") %}
echo "Cloning {{ node.name }} (ROS node: {{ node.id }}), branch: {{ node.version }} to /tmp ..."
rm -rf /tmp/{{ node.id }}
git clone --depth 1 --branch {{ node.version }} {{ node.repo }} /tmp/{{ node.id }}
echo "Copying subfolder {{ node.repo_subfolder }} to src/ ..."
cp -R /tmp/{{ node.id }}/{{ node.repo_subfolder }} src/{{ node.id }}
{% else %}
echo "Cloning {{ node.name }} (ROS node: {{ node.id }}), branch: {{ node.version }} to src/ ..."
git clone --depth 1 --branch {{ node.version }} {{ node.repo }} src/{{ node.id }}
{% endif %}
{% endif %}
{% endfor %}

