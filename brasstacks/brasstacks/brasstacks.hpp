#ifndef BRASSTACKS_BRASSTACKS_HPP
#define BRASSTACKS_BRASSTACKS_HPP

#include "brasstacks/version.hpp"
#include "brasstacks/tools/MemTracker.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"
#include "brasstacks/tools/TimeKeeper.hpp"
#include "brasstacks/core/Application.hpp"
#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"
#include "brasstacks/assets/libraries/TextureLibrary.hpp"

#include "brasstacks/platform/vulkan/vulkan_formatters.hpp"

#ifdef BTX_DEBUG
    static std::filesystem::path const BTX_ASSET_PATH("assets/");
    static std::string           const BTX_SHADER_EXT("-debug.spv");
#else
    static std::filesystem::path const BTX_ASSET_PATH("assets/");
    static std::string           const BTX_SHADER_EXT(".spv");
#endif // BTX build config

#endif // BRASSTACKS_BRASSTACKS_HPP