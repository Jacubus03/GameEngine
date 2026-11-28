#pragma once

#include <filesystem>
#include <string>
#include <vector>

class AssetBrowserPanel
{
public:
    // sourceRoot is the Assets folder of the project, scripts are copied there too
    explicit AssetBrowserPanel(const std::filesystem::path& root = "Assets", const std::filesystem::path& sourceRoot = "");

    void OnImGuiRender();

    // Copies files (e.g. dropped on the window) into the current folder
    void ImportFiles(const std::vector<std::string>& paths);

private:
    std::filesystem::path m_Root;
    std::filesystem::path m_SourceRoot;
    std::filesystem::path m_CurrentDirectory;
};
