#include "GameLibrary.h"
#include <android/log.h>
#include <cstdio>
#include <algorithm>

#define TAG "ShieldCompat"
#define LOG(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

GameLibrary::GameLibrary() {}

void GameLibrary::scanInstalledPackages() {
    m_games.clear();
    
    // Use pm list packages to find installed Shield games
    FILE* pipe = popen("pm list packages -f", "r");
    if (!pipe) {
        LOG("Failed to run pm list packages");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), pipe)) {
        std::string s(line);
        // Line format: package:/path/to/apk=com.package.name
        for (const auto& pkg : SHIELD_PACKAGES) {
            if (s.find(pkg) != std::string::npos) {
                GameEntry entry;
                entry.packageName = pkg;
                entry.patched = false;

                // Extract APK path
                size_t start = s.find(':') + 1;
                size_t end = s.find('=');
                if (start != std::string::npos && end != std::string::npos)
                    entry.apkPath = s.substr(start, end - start);

                // Clean name from package
                entry.name = pkg.substr(pkg.rfind('.') + 1);

                LOG("Found Shield game: %s at %s",
                    entry.name.c_str(), entry.apkPath.c_str());
                m_games.push_back(entry);
            }
        }
    }
    pclose(pipe);
}

bool GameLibrary::isShieldGame(const std::string& packageName) {
    return std::find(SHIELD_PACKAGES.begin(),
                     SHIELD_PACKAGES.end(),
                     packageName) != SHIELD_PACKAGES.end();
}
