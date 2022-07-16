#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

namespace btx {

std::vector<RenderQueue::ShaderIndex> RenderQueue::_shaders;
std::vector<RenderQueue::Queue>       RenderQueue::_meshes;

std::mutex              RenderQueue::_queue_lock;
std::mutex              RenderQueue::_submission_lock;
std::condition_variable RenderQueue::_queue_ready;

void RenderQueue::begin_scene() {
    {
        std::unique_lock<std::mutex> lock(_submission_lock);
        _queue_ready.wait(
            lock,
            [] {
                for(const auto &queue : _meshes) {
                    if(queue.size() >= 2) {
                        return false;
                    }
                }
                return true;
            }
        );
    }
    
    _queue_lock.lock();
}

void RenderQueue::end_scene() {
    _queue_lock.unlock();
}

void RenderQueue::begin_draw() {
    _queue_lock.lock();
    assert(!_shaders.empty() && !_meshes.empty());
}

void RenderQueue::end_draw() {
    _meshes.clear();
    _shaders.clear();

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
        // BTX_ENGINE_TRACE(
        //     "Shader {} has {} submissions",
        //     iter->second,
        //     _meshes[iter->second].size()
        // );
    }
}

RenderQueue::Queue RenderQueue::get_queue(size_t index) {
    return _meshes[index];
}

void RenderQueue::init() {
    _shaders.reserve(25);
    _meshes.reserve(25);
}

void RenderQueue::shutdown() {
    _shaders.clear();
    _meshes.clear();
    _queue_ready.notify_all();
}

} // namespace btx