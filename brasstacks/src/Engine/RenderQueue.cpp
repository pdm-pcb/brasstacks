#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

#include "brasstacks/ECS/ECS.hpp"

namespace btx {

using Shaders = std::vector<RenderQueue::ShaderIndex>;
using Meshes  = std::vector<RenderQueue::Queue>;

Shaders RenderQueue::_shaders_a;
Meshes  RenderQueue::_meshes_a;
Shaders RenderQueue::_shaders_b;
Meshes  RenderQueue::_meshes_b;

Shaders *RenderQueue::_shaders_front = &RenderQueue::_shaders_a;
Meshes  *RenderQueue::_meshes_front  = &RenderQueue::_meshes_a;
Shaders *RenderQueue::_shaders_back  = &RenderQueue::_shaders_b;
Meshes  *RenderQueue::_meshes_back   = &RenderQueue::_meshes_b;

std::mutex RenderQueue::_front_mutex;
std::mutex RenderQueue::_back_mutex;

std::condition_variable RenderQueue::_back_ready;
bool RenderQueue::_back_empty = true;

bool RenderQueue::_running = false;

void RenderQueue::begin_scene() {
    {
        std::unique_lock<std::mutex> lock(_back_mutex);
        _back_ready.wait(lock, []{ return _back_empty || !_running; });
    }
    _back_mutex.lock();
}

void RenderQueue::end_scene() {
    _back_empty = false;
    _back_mutex.unlock();
    _back_ready.notify_one();
}

void RenderQueue::begin_draw() {
    {
        std::unique_lock<std::mutex> lock(_back_mutex);
        _back_ready.wait(lock, []{ return !_back_empty || !_running; });
    }
    _back_mutex.lock();
    _front_mutex.lock();

    _swap_queues();
    
    _back_mutex.unlock();
    _back_ready.notify_one();
}

void RenderQueue::end_draw() {
    _clear_front();
    _front_mutex.unlock();
}

void RenderQueue::submit(const Shader *shader, const Entity::ID id) {
    assert(shader != nullptr);

    auto iter = std::find_if(
        (*_shaders_back).begin(), (*_shaders_back).end(),
        [&shader](const ShaderIndex &kv_pair) {
            return kv_pair.first == shader;
        }
    );

    if(iter == (*_shaders_back).end()) {
        (*_meshes_back).emplace_back(Queue());
        (*_meshes_back).back().reserve(RENDERQUEUE_PREALLOC);
        (*_meshes_back).back().emplace_back(id);
        (*_shaders_back).emplace_back(
            std::make_pair(shader, (*_meshes_back).size() - 1)
        );
    }
    else {
        (*_meshes_back)[iter->second].emplace_back(id);
    }
}

RenderQueue::Queue & RenderQueue::get_queue(std::size_t index) {
    return (*_meshes_front)[index];
}

void RenderQueue::init() {
    _shaders_a.reserve(RENDERQUEUE_PREALLOC);
    _meshes_a.reserve(RENDERQUEUE_PREALLOC);
    _shaders_b.reserve(RENDERQUEUE_PREALLOC);
    _meshes_b.reserve(RENDERQUEUE_PREALLOC);

    _running = true;
}

void RenderQueue::shutdown() {
    _running = false;
    _back_ready.notify_all();

    _front_mutex.lock();
    _back_mutex.lock();

    _clear_front();
    _clear_back();

    _front_mutex.unlock();
    _back_mutex.unlock();
}

void RenderQueue::_clear_front() {
    (*_shaders_front).clear();
    (*_meshes_front).clear();
}

void RenderQueue::_clear_back() {
    (*_shaders_back).clear();
    (*_meshes_back).clear();
}

void RenderQueue::_swap_queues() {
    auto shaders_temp = _shaders_front;
    auto meshes_temp  = _meshes_front;

    _shaders_front = _shaders_back;
    _meshes_front  = _meshes_back;

    _shaders_back = shaders_temp;
    _meshes_back  = meshes_temp;

    _back_empty  = true;
}

} // namespace btx