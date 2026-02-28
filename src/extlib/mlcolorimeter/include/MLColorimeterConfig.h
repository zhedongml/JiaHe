// Copyright 2025 MLOptic

#ifndef SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERCONFIG_H_
#define SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERCONFIG_H_

#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qvector.h>

#include <map>
#include <vector>

#include "MLColorimeterCommon.h"
#include "Result.h"
#include "json.hpp"
#include "mlcolorimetercommon_global.h"

namespace ML {
namespace MLColorimeter {
using json = nlohmann::ordered_json;

class MLCOLORIMETERCOMMON_EXPORT MLColorimeterConfig {
 public:
    MLColorimeterConfig();
    ~MLColorimeterConfig();

    bool LoadConfigPath(const char* path, std::vector<std::string> nameList = {".json", ".csv"});

    bool ReadModuleInfo();

    ML::MLColorimeter::ModuleConfig GetModuleInfo();

    bool LoadFocusScanConfig(const char* path);

    bool ReadFocusScanConfig();

    ML::MLColorimeter::FocusScanConfig GetFocusScanConfig();

    bool LoadDiopterScanConfig(const char* path);

    bool ReadDiopterScanConfig();

    ML::MLColorimeter::DiopterScanConfig GetDiopterScanConfig();

    std::map<std::string, QVector<QStringList>> GetModuleCSVFile();

    bool LoadFolderRule(std::string path,
                        std::vector<std::string> sessionList = {"Movement", "Dark", "DarkList", "ColorShift", "FFC", "Distortion",
                                                                "Offset", "FourColor", "FourColorCalibration", "Luminance",
                                                                "Radiance", "FOVCrop", "Flip_Rotate"});

    std::string ProcessPath(std::string session, ML::MLColorimeter::ProcessPathConfig config, std::string RXCombination = "");

    std::string ProcessPath(std::string session, ML::MLColorimeter::ProcessPathConfig config, FolderRule rule,
                            std::string RXCombination = "");

    std::string CombinateKey(std::string session, ML::MLColorimeter::ProcessPathConfig config);

    std::map<std::string, ML::MLColorimeter::FolderRule> GetFolderRule();

 private:
    std::map<std::string, json> m_JsonMap;
    std::map<std::string, QVector<QStringList>> m_CsvMap;
    ML::MLColorimeter::ModuleConfig m_ModuleConfig;
    ML::MLColorimeter::FocusScanConfig m_FocusScanConfig;
    ML::MLColorimeter::DiopterScanConfig m_DiopterScanConfig;
    std::map<std::string, ML::MLColorimeter::FolderRule> m_allFolderRule;
};

}  // namespace MLColorimeter
}  // namespace ML

#endif  // SRC_MLCOLORIMETERCOMMON_MLCOLORIMETERCONFIG_H_
