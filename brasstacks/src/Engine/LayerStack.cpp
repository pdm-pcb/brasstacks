#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/LayerStack.hpp"

#include "brasstacks/Engine/Layer.hpp"

namespace btx {

std::vector<Layer *> LayerStack::_layers;

void LayerStack::update_layers() {
    for(auto layer = _layers.rbegin(); layer != _layers.rend(); ++layer) {
        (*layer)->update();
    }
}

void LayerStack::on_event(Event &event) {
    for(auto layer = _layers.rbegin(); layer != _layers.rend(); ++layer) {
        (*layer)->on_event(event);
    }
}

void LayerStack::push_layer(Layer *layer) {
    _layers.emplace_back(layer);
    layer->on_attach();
}

void LayerStack::pop_layer() {
    _layers.back()->on_detach();
    delete _layers.back();
    _layers.erase(--_layers.end());
}

void LayerStack::init() {
    _layers.reserve(5);
}

void LayerStack::shutdown() {
    for(auto layer = _layers.rbegin(); layer != _layers.rend(); ++layer) {
        (*layer)->on_detach();
        delete *layer;
    }

    _layers.clear();
}

} // namespace btx