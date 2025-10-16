# ImGuiExamples
Sample project showcasing how to integrate [ImGuiPlugin](https://github.com/amuTBKT/ImGuiPlugin) in Unreal projects.

# Build Setup
* __ImGuiExamples.uproject__ : Add ImGuiPlugin to the Plugins list.
* __ImGuiExamples.Build.cs__ : Include "ImGui" and "ImGuiRuntime" module dependencies.
* __ImGuiExamplesGameModule.cpp__ : Add call to `SETUP_DEFAULT_IMGUI_ALLOCATOR()` in module startup function.
* __ImGuiTestWidget.cpp__ : A simple widget showcasing how to use different fonts and display slate icons in a standalone widget.
* __ImGuiTestActor.cpp__ : Shows how to use ImGui widget with Actors.

# Code Setup
* All ImGui code should be wrapped inside `#ifdef WITH_IMGUI` macros. This is needed to ensure code continues to compile even when the ImGuiPlugin is disabled (manually or depending on build configuration)
* `SETUP_DEFAULT_IMGUI_ALLOCATOR()` needs to be called inside every module startup function which uses ImGui functions. This is needed to ensure memory allocation doesn't fail when making ImGui calls.
