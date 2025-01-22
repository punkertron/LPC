#include "ResourceManager.hpp"

#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Checkers.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

constexpr std::string_view QUEEN_FILENAME = "queen.png";
constexpr std::string_view YOULOST_FILENAME = "you_lost.png";
constexpr std::string_view YOUWIN_FILENAME = "you_win.png";
constexpr std::string_view WHITEWINS_FILENAME = "white_wins.png";
constexpr std::string_view BLACKWINS_FILENAME = "black_wins.png";
constexpr std::string_view FONT_FILENAME = "Sixtyfour-Regular.ttf";

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
        std::perror("readlink");
        return "";
    }
#endif

    return std::filesystem::path(buffer).parent_path();
}

template <typename Resource>
static inline void loadResource(Resource& resource, const std::filesystem::path& resourceDir, std::string_view fileName)
{
    auto texturePath = resourceDir / fileName;
    if (!resource.loadFromFile(texturePath.string())) {
        throw std::runtime_error(std::string("Unable to load resource: ") + std::string(fileName));
    }
}

// we load everything in ctor, even if it is possible, we will never need it.. Is it okay?
ResourceManager::ResourceManager()
{
    auto exeDir = getExecutablePath();

    loadResource(queen_, exeDir, QUEEN_FILENAME);
    loadResource(youLost_, exeDir, YOULOST_FILENAME);
    loadResource(youWin_, exeDir, YOUWIN_FILENAME);
    loadResource(whiteWins_, exeDir, WHITEWINS_FILENAME);
    loadResource(blackWins_, exeDir, BLACKWINS_FILENAME);
    loadResource(font_, exeDir, FONT_FILENAME);
}

const sf::Texture& ResourceManager::getQueenTexture() const
{
    return queen_;
}

const sf::Texture& ResourceManager::getYouLostTexture() const
{
    return youLost_;
}

const sf::Texture& ResourceManager::getYouWinTexture() const
{
    return youWin_;
}

const sf::Texture& ResourceManager::getColourWinsTexture(COLOUR c) const
{
    if (c == COLOUR::WHITE) {
        return whiteWins_;
    }
    return blackWins_;
}

const sf::Font& ResourceManager::getFont() const
{
    return font_;
}
