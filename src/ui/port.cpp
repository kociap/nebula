#include <ui/port.hpp>

namespace nebula::ui {
    Port::Port(Vec2 const coordinates, port_t const type) {
        this->coordinates = coordinates;
        this->type = type;
    }

    void Port::move(nebula::Vec2 const offset) {
        coordinates.x += offset.x;
        coordinates.y += offset.y;
    }

    void Port::render() const {
        // TODO: circle shaped port render function
        // Draw dot

        if(type == OUT){
            // Draw connections
        }
    }

    void Port::remove_connection(Port *old_port) {
        List_Iterator<Port *, Port *> it;
        for(it = connections.begin(); it != connections.end(); it++)
        {
            if(*it == old_port) {
                connections.erase(it);
                break;
            }
        }
    }

    void Port::add_connection(Port* new_port) {
        // For IN type ports only one connection is allowed
        if(type == IN && connections.size() > 0)
        {
            // Remove old connection
            Port* old_port = *(connections.begin());
            old_port->remove_connection(this);
            connections.erase_front();
        }
        connections.emplace_front(new_port);
    }

    void Port::remove_all_connections() {
        for(Port* port : connections)
        {
            port->remove_connection(this);
        }
        connections = {};
    }
}