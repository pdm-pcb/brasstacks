#!/bin/bash

cd third_party/include \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h -O imgui.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h -O imconfig.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h -O imgui_internal.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imstb_rectpack.h -O imstb_rectpack.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imstb_textedit.h -O imstb_textedit.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imstb_truetype.h -O imstb_truetype.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_glfw.h -O imgui_impl_glfw.h \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_vulkan.h -O imgui_impl_vulkan.h \
&& cd ../src \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp -O imgui.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp -O imgui_draw.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui_tables.cpp -O imgui_tables.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui_widgets.cpp -O imgui_widgets.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/imgui_demo.cpp -O imgui_demo.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_glfw.cpp -O imgui_impl_glfw.cpp \
&& wget -q https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_vulkan.cpp -O imgui_impl_vulkan.cpp
