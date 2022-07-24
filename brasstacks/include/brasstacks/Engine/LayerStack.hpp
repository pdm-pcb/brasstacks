#ifndef BRASSTACKS_ENGINE_LAYERSTACK_HPP
#define BRASSTACKS_ENGINE_LAYERSTACK_HPP

#include <vector>

namespace btx {

class Layer;
class Event;

class LayerStack final {
public:
    static void on_event(Event &event);
    static void update_layers();

    static void push_layer(Layer *layer);
    static void pop_layer();
    
    static void init();
    static void shutdown();

    ~LayerStack() { shutdown(); }

    LayerStack(const Layer &&) = delete;
    LayerStack(Layer &)        = delete;

    LayerStack & operator=(const Layer &&) = delete;
    LayerStack & operator=(Layer &)        = delete;

private:
    static std::vector<Layer *> _layers;

    LayerStack() = default;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_LAYERSTACK_HPP