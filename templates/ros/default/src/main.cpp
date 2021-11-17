#include <sstream>

#include "ros/ros.h"
#include "std_msgs/String.h"
{% for dep in dependencies %}
#include "{{ dep.0 }}/{{ dep.1 }}.h"
{% endfor %}

{% for sub in inputs %}
{% if not sub.type == "tf" %}
void {{ sub.short }}Callback(const {{ sub.datatype.0 }}::{{ sub.datatype.1 }}::ConstPtr& msg)
{
  ROS_INFO_STREAM("{{ id }}: received message: " << msg);
}
{% endif %}
{% endfor %}

int main(int argc, char **argv) {
    ros::init(argc, argv, "{{ id }}");

    ros::NodeHandle n;

    {% for sub in inputs %}
    {% if not (sub.type == "tf") %}
    {% if sub.type == "undefined" %}
    // ATTENTION: this topic is not defined in the architecture design
    {% endif %}
    ros::Subscriber {{ sub.short }}_sub = n.subscribe("{{ sub.short }}", 1, {{ sub.short }}Callback);
    {% endif %}
    {% endfor %}

    {% for pub in outputs %}
    {% if not (pub.type == "tf") %}
    {% if pub.type == "undefined" %}
    // ATTENTION: this topic is not defined in the architecture design
    {% endif %}
    ros::Publisher {{ pub.short }}_pub = n.advertise<{{ pub.datatype.0 }}::{{ pub.datatype.1 }}>("{{ pub.short }}", 1);
    {% endif %}
    {% endfor %}

    ros::Rate loop_rate(10);

    int count = 0;
    while (ros::ok()) {
        std_msgs::String msg;

        std::stringstream ss;
        ss << "hello world " << count;
        msg.data = ss.str();

        ROS_INFO("%s", msg.data.c_str());

        chatter_pub.publish(msg);

        ros::spinOnce();

        loop_rate.sleep();
        ++count;
    }

    return 0;
}
