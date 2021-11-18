/*
 Copyright 2021-2025, SPRING Consortium

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/


#include "ros/ros.h"
#include "std_msgs/String.h"
{% for dep in dependencies %}
#include "{{ dep.0 }}/{{ dep.1 }}.h"
{% endfor %}

class {{ id_capitalized }} {
   public:
    {{ id_capitalized }}(ros::NodeHandle* nh) {

{% for sub in inputs %}
{% if not (sub.type == "tf") %}
{% if sub.type == "undefined" %}
        // ATTENTION: this topic is not defined in the architecture design
{% endif %}
        {{ sub.short }}_sub_ = nh->subscribe("{{ sub.short }}", 1, &{{ id_capitalized }}::{{ sub.short }}Callback, this);
{% endif %}
{% endfor %}

{% for pub in outputs %}
{% if not (pub.type == "tf") %}
{% if pub.type == "undefined" %}
        // ATTENTION: this topic is not defined in the architecture design
{% endif %}
        {{ pub.short }}_pub_ = nh->advertise<{{ pub.datatype.0 }}::{{ pub.datatype.1 }}>("{{ pub.short }}", 1);
{% endif %}
{% endfor %}
    }

    ~{{ id_capitalized }}() {}

   private:
{% for sub in inputs %}
{% if not (sub.type == "tf") %}
    void {{ sub.short }}Callback(const {{ sub.datatype.0 }}::{{ sub.datatype.1 }}::ConstPtr& msg)
    {
        ROS_INFO_STREAM("{{ id }}: received message: " << msg);
    }
{% endif %}
{% endfor %}

{% for sub in inputs %}
{% if not (sub.type == "tf") %}
    ros::Subscriber {{ sub.short }}_sub_;
{% endif %}
{% endfor %}

{% for pub in outputs %}
{% if not (pub.type == "tf") %}
    ros::Publisher {{ pub.short }}_pub_;
{% endif %}
{% endfor %}

};

int main(int argc, char** argv) {
    ros::init(argc, argv, "{{ id }}");

    // Initialize node parameters from launch file or command line.
    // Use a private node handle so that multiple instances of the node can be
    // run simultaneously while using different parameters.
    ros::NodeHandle private_node_handle_("~");

    auto {{ id }} = {{ id_capitalized }}(&private_node_handle_);

    ROS_INFO("Node {{ id }} launched and ready.");
    ros::spin();
    return 0;
}
