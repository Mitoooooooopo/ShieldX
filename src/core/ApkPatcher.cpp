#include "ApkPatcher.h"
#include <android/log.h>
#include <cstdio>
#include <string>

#define TAG "ShieldCompat"
#define LOG(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

ApkPatcher::ApkPatcher(const std::string& workDir)
    : m_workDir(workDir) {}

bool ApkPatcher::runCmd(const std::string& cmd) {
    LOG("Running: %s", cmd.c_str());
    int ret = system(cmd.c_str());
    if (ret != 0) {
        m_lastError = "Command failed: " + cmd;
        LOG("Failed: %s", cmd.c_str());
        return false;
    }
    return true;
}

bool ApkPatcher::extractApk(const std::string& apkPath,
                             const std::string& outDir) {
    // APK is just a zip
    std::string cmd = "unzip -o \"" + apkPath + "\" -d \"" + outDir + "\"";
    return runCmd(cmd);
}

bool ApkPatcher::injectCompatSo(const std::string& extractDir) {
    // Copy our compat .so into the extracted APK's lib folder
    std::string libDir = extractDir + "/lib/armeabi-v7a/";

    // Create dir if missing
    runCmd("mkdir -p \"" + libDir + "\"");

    // Copy our prebuilt compat so
    std::string compatSo = m_workDir + "/libshieldcompat.so";
    std::string cmd = "cp \"" + compatSo + "\" \"" + libDir + "\"";
    if (!runCmd(cmd)) return false;

    // Replace libkillswitch.so with our stub
    std::string killswitchStub = m_workDir + "/libkillswitch_stub.so";
    cmd = "cp \"" + killswitchStub + "\" \""
        + libDir + "libkillswitch.so\"";
    return runCmd(cmd);
}

bool ApkPatcher::repackApk(const std::string& extractDir,
                            const std::string& outApk) {
    // Repack directory back into APK
    std::string cmd = "cd \"" + extractDir + "\" && "
                    + "zip -r \"" + outApk + "\" .";
    return runCmd(cmd);
}

bool ApkPatcher::signApk(const std::string& apkPath) {
    // Sign with debug keystore
    std::string keystore = m_workDir + "/debug.keystore";

    // Generate keystore if missing
    std::string genKey = "keytool -genkey -v"
        " -keystore \"" + keystore + "\""
        " -alias androiddebugkey"
        " -keyalg RSA -keysize 2048"
        " -validity 10000"
        " -storepass android -keypass android"
        " -dname \"CN=Android Debug,O=Android,C=US\""
        " 2>/dev/null || true";
    runCmd(genKey);

    // Sign APK
    std::string cmd = "apksigner sign"
        " --ks \"" + keystore + "\""
        " --ks-pass pass:android"
        " --key-pass pass:android"
        " \"" + apkPath + "\"";
    return runCmd(cmd);
}

PatchStatus ApkPatcher::patch(const GameEntry& game) {
    std::string extractDir = m_workDir + "/" + game.packageName;
    std::string outApk = m_workDir + "/" + game.packageName + "_patched.apk";

    LOG("Patching: %s", game.packageName.c_str());

    if (!extractApk(game.apkPath, extractDir))
        return PatchStatus::FAILED_EXTRACT;

    if (!injectCompatSo(extractDir))
        return PatchStatus::FAILED_INJECT;

    if (!repackApk(extractDir, outApk))
        return PatchStatus::FAILED_REPACK;

    if (!signApk(outApk))
        return PatchStatus::FAILED_SIGN;

    LOG("Patch complete: %s", outApk.c_str());
    return PatchStatus::SUCCESS;
}
