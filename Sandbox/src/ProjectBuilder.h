#pragma once

#include <filesystem>
#include <string>

class ProjectBuilder
{
public:
    // Copies the program, its libraries and Assets to outputDirectory and sets the scene to start with.
    // Returns false and a reason in message when something fails.
    static bool Build(
        const std::filesystem::path& outputDirectory,
        const std::filesystem::path& runtimeDirectory,
        const std::string& executableName,
        const std::string& sceneText,
        std::string& message);
};
