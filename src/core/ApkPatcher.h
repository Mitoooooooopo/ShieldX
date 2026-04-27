#pragma once
#include <string>
#include "GameLibrary.h"

enum class PatchStatus {
    SUCCESS,
    FAILED_EXTRACT,
    FAILED_INJECT,
    FAILED_REPACK,
    FAILED_SIGN
};

class ApkPatcher {
public:
    ApkPatcher(const std::string& workDir);
    
    PatchStatus patch(const GameEntry& game);
    std::string getLastError() const { return m_lastError; }

private:
    std::string m_workDir;
    std::string m_lastError;

    bool extractApk(const std::string& apkPath, const std::string& outDir);
    bool injectCompatSo(const std::string& extractDir);
    bool repackApk(const std::string& extractDir, const std::string& outApk);
    bool signApk(const std::string& apkPath);
    bool runCmd(const std::string& cmd);
}; 
