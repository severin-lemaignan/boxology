/* See LICENSE file for copyright and license details. */

#include "connection.hpp"
#include <QDebug>

using namespace std;

const string Connection::ANONYMOUS = "anonymous";

Connection::~Connection() {

    //qWarning() << "Connection " << QString::fromStdString(name) << " deleted!!";
}
