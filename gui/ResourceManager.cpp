#include "ResourceManager.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

constexpr std::string QUEEN_TEXTURE_FILENAME = "queen.png";

static std::filesystem::path getExecutablePath()
{
    char buffer[2048];

#if defined(_WIN32)
    GetModuleFileName(NULL, buffer, sizeof(buffer));
#elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    _NSGetExecutablePath(buffer, &size);
#else
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    } else {
        perror("readlink");
        return "";
    }
#endif

    return std::filesystem::path(buffer).parent_path();
}

// we load everything in ctor, even if it is possible, we will never need it.. Is it okay?
ResourceManager::ResourceManager()
{
    auto exeDir = getExecutablePath();
    auto texturePath = exeDir / QUEEN_TEXTURE_FILENAME;
    if (!queen_.loadFromFile(texturePath.string())) {
        throw std::runtime_error("Unable to load queen texture");
    }
}

sf::Texture& ResourceManager::getQueenTexture()
{
    return queen_;
}
