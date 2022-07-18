#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

namespace btx {

std::vector<RenderQueue::ShaderIndex> RenderQueue::_shaders;
std::vector<RenderQueue::Queue>       RenderQueue::_meshes;

std::mutex              RenderQueue::_queue_lock;
std::mutex              RenderQueue::_submission_lock;

std::condition_variable RenderQueue::_queue_ready;
bool RenderQueue::_queue_empty = true;

void RenderQueue::begin_scene() {
    {
        std::unique_lock<std::mutex> lock(_submission_lock);
        _queue_ready.wait(lock, []{ return _queue_empty; });
    }
    
    _queue_lock.lock();
    _queue_empty = false;
}

void RenderQueue::end_scene() {
    _queue_lock.unlock();
}

void RenderQueue::begin_draw() {
    _queue_lock.lock();
}

void RenderQueue::end_draw() {
    _clear_queue();
    _queue_lock.unlock();
    _queue_ready.notify_one();
}

void RenderQueue::submit(const Shader *shader, const Mesh *mesh) {
    assert(shader != nullptr && mesh != nullptr);

    auto iter = std::find_if(
        _shaders.begin(), _shaders.end(),
        [&shader](const ShaderIndex &kv_pair) {
            return kv_pair.first == shader;
        }
    );

    if(iter == _shaders.end()) {
        _meshes.emplace_back(Queue());
        _meshes.back().reserve(25);
        _meshes.back().emplace_back(mesh);
        _shaders.emplace_back(std::make_pair(shader, _meshes.size() - 1));
    }
    else {
        _meshes[iter->second].emplace_back(mesh);
    }
}

RenderQueue::Queue RenderQueue::get_queue(std::size_t index) {
    return _meshes[index];
}

void RenderQueue::init() {
    _shaders.reserve(25);
    _meshes.reserve(25);
}

void RenderQueue::shutdown() {
    _clear_queue();
    _queue_ready.notify_all();
}

void RenderQueue::_clear_queue() {
    _shaders.clear();
    _meshes.clear();
    _queue_empty = true;
}

} // namespace btx