#ifndef BRASSTACKS_ENGINE_LAYER_HPP
#define BRASSTACKS_ENGINE_LAYER_HPP

namespace btx {

class Event;

class Layer {
public:
    virtual void on_attach() { };
    virtual void on_detach() { };
    virtual void update()    { };
    virtual void on_event([[maybe_unused]] Event &event) { };

    virtual ~Layer() = default;

    Layer(const Layer &&) = delete;
    Layer(Layer &)        = delete;

    Layer & operator=(const Layer &&) = delete;
    Layer & operator=(Layer &)        = delete;

protected:
    Layer() = default;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_LAYER_HPP