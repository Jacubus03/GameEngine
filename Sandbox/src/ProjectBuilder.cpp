#include "ProjectBuilder.h"

#include <fstream>
#include <system_error>

namespace fs = std::filesystem;

bool ProjectBuilder::Build(
    const fs::path& outputDirectory,
    const fs::path& runtimeDirectory,
    const std::string& executableName,
    const std::string& sceneText,
    std::string& message)
{
    std::error_code error;

    if (!fs::exists(runtimeDirectory / executableName))
    {
        message = "Could not find " + executableName;
        return false;
    }

    if (!fs::exists(runtimeDirectory / "Assets"))
    {
        message = "Could not find the Assets folder";
        return false;
    }

    fs::create_directories(outputDirectory, error);

    if (error)
    {
        message = "Could not create the output folder: " + error.message();
        return false;
    }

    // Program and its libraries
    for (const auto& entry : fs::directory_iterator(runtimeDirectory, error))
    {
        const std::string name = entry.path().filename().string();

        if (!entry.is_regular_file() || (name != executableName && entry.path().extension() != ".dll"))
            continue;

        std::error_code copyError;
        fs::copy_file(entry.path(), outputDirectory / name, fs::copy_options::overwrite_existing, copyError);

        if (copyError)
        {
            message = "Could not copy " + name + ": " + copyError.message();
            return false;
        }
    }

    // Assets
    fs::copy(runtimeDirectory / "Assets", outputDirectory / "Assets", fs::copy_options::recursive | fs::copy_options::overwrite_existing, error);

    if (error)
    {
        message = "Could not copy Assets: " + error.message();
        return false;
    }

    // Scene to start with
    const fs::path scene = outputDirectory / "Assets" / "Scenes" / "Main.scene";
    fs::create_directories(scene.parent_path(), error);

    std::ofstream sceneFile(scene, std::ios::out | std::ios::binary);
    sceneFile << sceneText;

    std::ofstream startupFile(outputDirectory / "startup.txt", std::ios::out | std::ios::binary);
    startupFile << "Assets/Scenes/Main.scene\n";

    if (!sceneFile || !startupFile)
    {
        message = "Could not write the scene";
        return false;
    }

    message = "Build finished: " + fs::absolute(outputDirectory).string();

    return true;
}
