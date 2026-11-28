#pragma once

// For use by GameEngine applications
#include "GameEngine/Core/Core.h"

#include "GameEngine/Core/Application.h"
#include "GameEngine/Core/Layer.h"
#include "GameEngine/Core/Log.h"

#include "GameEngine/Core/Timestep.h"

#include "GameEngine/Core/Input.h"
#include "GameEngine/Core/KeyCodes.h"
#include "GameEngine/Core/MouseButtonCodes.h"

#include "GameEngine/ImGui/ImGuiLayer.h"

// Renderer
#include "GameEngine/Renderer/Buffer.h"
#include "GameEngine/Renderer/Framebuffer.h"
#include "GameEngine/Renderer/MeshData.h"
#include "GameEngine/Renderer/Mesh.h"
#include "GameEngine/Renderer/Material.h"
#include "GameEngine/Renderer/Shader.h"
#include "GameEngine/Renderer/Texture.h"
#include "GameEngine/Renderer/Renderer.h"
#include "GameEngine/Renderer/VertexArray.h"

// Scene
#include "GameEngine/Scripting/Script.h"
#include "GameEngine/Scripting/ScriptRegistry.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Scene/SceneSerializer.h"
#include "GameEngine/Scene/MeshRenderer.h"
#include "GameEngine/Scene/Object.h"
#include "GameEngine/Scene/Transform.h"
#include "GameEngine/Scene/Camera/Camera.h"
#include "GameEngine/Scene/Camera/OrthographicCamera.h"
#include "GameEngine/Scene/Camera/PerspectiveCamera.h"
#include "GameEngine/Scene/Camera/SceneCameraController.h"
