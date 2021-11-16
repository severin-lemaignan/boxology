cmake_minimum_required(VERSION 3.0.2)
project({{ id }})


find_package(catkin REQUIRED COMPONENTS
{% for node in nodes %}
    {{ node.id }}
{% endfor %}
)

catkin_package()

install(DIRECTORY launch
   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

