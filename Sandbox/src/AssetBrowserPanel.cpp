#include "AssetBrowserPanel.h"

#include <GameEngine.h>

#include <imgui.h>

#include <algorithm>
#include <system_error>

namespace
{
    std::string ToString(const std::filesystem::path& path)
    {
        const std::u8string text = path.generic_u8string();
        return std::string(text.begin(), text.end());
    }

    std::filesystem::path ToPath(const std::string& text)
    {
        return std::filesystem::path(std::u8string(text.begin(), text.end()));
    }

    // Adds (1), (2)... when the file already exists
    std::filesystem::path UniquePath(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
            return path;

        for (int i = 1; ; i++)
        {
            std::filesystem::path name = path.stem();
            name += " (" + std::to_string(i) + ")";
            name += path.extension();

            const std::filesystem::path candidate = path.parent_path() / name;

            if (!std::filesystem::exists(candidate))
                return candidate;
        }
    }
}

AssetBrowserPanel::AssetBrowserPanel(const std::filesystem::path& root, const std::filesystem::path& sourceRoot)
    : m_Root(root), m_SourceRoot(sourceRoot), m_CurrentDirectory(root)
{
}

void AssetBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Assets");

    if (m_CurrentDirectory != m_Root)
    {
        if (ImGui::Button("Back"))
            m_CurrentDirectory = m_CurrentDirectory.parent_path();

        ImGui::SameLine();
    }

    ImGui::TextDisabled("%s", ToString(m_CurrentDirectory).c_str());
    ImGui::Separator();

    std::vector<std::filesystem::directory_entry> entries;
    std::error_code error;

    for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory, error))
        entries.push_back(entry);

    // Folders first
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b)
    {
        if (a.is_directory() != b.is_directory())
            return a.is_directory();

        return a.path().filename() < b.path().filename();
    });

    for (const auto& entry : entries)
    {
        const std::string name = ToString(entry.path().filename());

        ImGui::PushID(name.c_str());

        if (entry.is_directory())
        {
            const std::string label = "[" + name + "]";

            ImGui::Selectable(label.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                m_CurrentDirectory = entry.path();
        }
        else
        {
            ImGui::Selectable(name.c_str());

            if (ImGui::BeginDragDropSource())
            {
                const std::string path = ToString(entry.path());

                ImGui::SetDragDropPayload("ASSET_PATH", path.c_str(), path.size() + 1);
                ImGui::TextUnformatted(name.c_str());
                ImGui::EndDragDropSource();
            }
        }

        ImGui::PopID();
    }

    if (entries.empty())
        ImGui::TextDisabled("Empty folder");

    ImGui::Separator();
    ImGui::TextDisabled("Drop files on the window to import them here");

    ImGui::End();
}

void AssetBrowserPanel::ImportFiles(const std::vector<std::string>& paths)
{
    for (const std::string& file : paths)
    {
        const std::filesystem::path source = ToPath(file);
        std::error_code error;

        // Scripts always go to the Scripts folder
        if (source.extension() == ".cpp")
        {
            if (!m_SourceRoot.empty())
            {
                std::filesystem::create_directories(m_SourceRoot / "Scripts", error);
                std::filesystem::copy_file(source, m_SourceRoot / "Scripts" / source.filename(), std::filesystem::copy_options::overwrite_existing, error);
            }

            const std::filesystem::path scriptsDirectory = m_Root / "Scripts";
            std::filesystem::create_directories(scriptsDirectory, error);
            std::filesystem::copy_file(source, scriptsDirectory / source.filename(), std::filesystem::copy_options::overwrite_existing, error);

            if (error)
                GE_LOG_ERROR("Could not import {0}: {1}", file, error.message());
            else
                GE_LOG_INFO("Imported script {0}, rebuild the project to use it", ToString(source.filename()));

            m_CurrentDirectory = scriptsDirectory;
            continue;
        }

        const std::filesystem::path destination = UniquePath(m_CurrentDirectory / source.filename());
        std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive, error);

        if (error)
            GE_LOG_ERROR("Could not import {0}: {1}", file, error.message());
        else
            GE_LOG_INFO("Imported {0}", ToString(destination));
    }
}
