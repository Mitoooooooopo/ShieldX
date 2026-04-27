#pragma once
#include <string>
#include <vector>

struct GameEntry {
    std::string name;
    std::string packageName;
    std::string apkPath;
    std::string iconPath;
    bool patched;
};

class GameLibrary {
public:
    GameLibrary();
    void scanInstalledPackages();
    const std::vector<GameEntry>& getGames() const { return m_games; }
    bool isShieldGame(const std::string& packageName);

private:
    std::vector<GameEntry> m_games;
    
    // Known Shield game packages
    const std::vector<std::string> SHIELD_PACKAGES = {
        "com.nvidia.residentevil5",
        "com.nvidia.borderlands2",
        "com.nvidia.portal",
        "com.nvidia.hl2",
        "com.nvidia.tombraider"
    };
}; 
