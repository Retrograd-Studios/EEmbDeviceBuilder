

#include "EEmbDevice.h"

#include <iostream>
#include <fstream>
#include <sstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace EEmbSimulator
{

	void to_json(nlohmann::json &j, const VEC4 &info)
	{
		j = nlohmann::json{
			{"x", info.x},
			{"y", info.y},
			{"w", info.w},
			{"h", info.h}};
	}

	void from_json(const nlohmann::json &j, VEC4 &info)
	{
		j.at("x").get_to(info.x);
		j.at("y").get_to(info.y);
		j.at("w").get_to(info.w);
		j.at("h").get_to(info.h);
	}

	void to_json(nlohmann::json &j, const EEmbPeriph &info)
	{

		j = nlohmann::json{
			{"typeId", info.typeId},
			{"label", info.label},
			{"labelRect", info.labelRect},
			{"hudRect", info.hudRect},
			{"targetRect", info.targetRect},
			{"targetHoverType", info.targetHoverType},
			{"isEditLabel", info.isEditLabel},
			{"isShowMenu", info.isShowMenu}};
	}

	void from_json(const nlohmann::json &j, EEmbPeriph &info)
	{
		j.at("typeId").get_to(info.typeId);
		j.at("label").get_to(info.label);
		j.at("labelRect").get_to(info.labelRect);
		j.at("hudRect").get_to(info.hudRect);
		j.at("targetRect").get_to(info.targetRect);
		j.at("targetHoverType").get_to(info.targetHoverType);
		j.at("isEditLabel").get_to(info.isEditLabel);
		j.at("isShowMenu").get_to(info.isShowMenu);
	}

	void to_json(nlohmann::json &j, const EEmbImg &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["imgPath"] = info.imgPath;
	}

	void from_json(const nlohmann::json &j, EEmbImg &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("imgPath").get_to(info.imgPath);
	}

	void to_json(nlohmann::json &j, const EEmbUI &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["rawVal"] = info.rawVal;
		j["max"] = info.max;
		j["min"] = info.min;
		j["val"] = info.val;
		j["offset"] = info.offset;
		j["mode"] = info.mode;
	}

	void from_json(const nlohmann::json &j, EEmbUI &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("rawVal").get_to(info.rawVal);
		j.at("max").get_to(info.max);
		j.at("min").get_to(info.min);
		j.at("val").get_to(info.val);
		j.at("offset").get_to(info.offset);
		j.at("mode").get_to(info.mode);
	}

	void to_json(nlohmann::json &j, const MB_modbus_t &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["periphPortNum"] = info.periphPortNum;
	}

	void from_json(const nlohmann::json &j, MB_modbus_t &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("periphPortNum").get_to(info.periphPortNum);
	}

	void to_json(nlohmann::json &j, const EEmbAO &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["output"] = info.output;
		j["value"] = info.value;
	}

	void from_json(const nlohmann::json &j, EEmbAO &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("output").get_to(info.output);
		j.at("value").get_to(info.value);
	}

	void to_json(nlohmann::json &j, const EEmbDO &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["value"] = info.value;
	}

	void from_json(const nlohmann::json &j, EEmbDO &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("value").get_to(info.value);
	}

	void to_json(nlohmann::json &j, const EEmbDisplay &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["width"] = info.width;
		j["height"] = info.height;
	}

	void from_json(const nlohmann::json &j, EEmbDisplay &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("width").get_to(info.width);
		j.at("height").get_to(info.height);
	}

	void to_json(nlohmann::json &j, const EEmbLED &info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["value"] = info.value;
		j["colorOn"] = info.colorOn;
		j["colorOff"] = info.colorOff;
	}

	void from_json(const nlohmann::json &j, EEmbLED &info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("value").get_to(info.value);
		j.at("colorOn").get_to(info.colorOn);
		j.at("colorOff").get_to(info.colorOff);
	}

	void to_json(nlohmann::json& j, const EEmbButton& info)
	{
		j = *static_cast<const EEmbPeriph *>(&info);
		j["buttonId"] = info.buttonId;
	}

	void from_json(const nlohmann::json& j, EEmbButton& info)
	{
		*static_cast<EEmbPeriph *>(&info) = j;
		j.at("buttonId").get_to(info.buttonId);
	}




	void to_json(nlohmann::json &j, const EEmbDevice &info)
	{
		j = nlohmann::json{
			{"deviceName", info.deviceName},
			{"images", info.images},
			{"UIs", info.UIs},
			{"AOs", info.AOs},
			{"DOs", info.DOs},
			{"COMs", info.COMs},
			{"displays", info.displays},
			{"LEDs", info.LEDs},
			{"buttons", info.buttons},

			{"globVarsFavs", info.globVarsFavs},
			{"mbRegsFavs", info.mbRegsFavs},

			{"mbRegsWin", info.mbRegsWin},
			{"uiWin", info.uiWin},
			{"globVarsWin", info.globVarsWin},
			{"remoteModulesWin", info.remoteModulesWin},

			{"simVisableDbgWins", info.simVisableDbgWins},
			{"isShowUiWin", info.isShowUiWin}
		};
	}

	void from_json(const nlohmann::json &j, EEmbDevice &info)
	{
		j.at("deviceName").get_to(info.deviceName);
		j.at("images").get_to(info.images);
		j.at("UIs").get_to(info.UIs);
		j.at("AOs").get_to(info.AOs);
		j.at("DOs").get_to(info.DOs);
		j.at("COMs").get_to(info.COMs);
		j.at("displays").get_to(info.displays);
		j.at("LEDs").get_to(info.LEDs);
		j.at("buttons").get_to(info.buttons);

		j.at("globVarsFavs").get_to(info.globVarsFavs);
		j.at("mbRegsFavs").get_to(info.mbRegsFavs);

		j.at("mbRegsWin").get_to(info.mbRegsWin);
		j.at("uiWin").get_to(info.uiWin);
		j.at("globVarsWin").get_to(info.globVarsWin);
		j.at("remoteModulesWin").get_to(info.remoteModulesWin);

		j.at("simVisableDbgWins").get_to(info.simVisableDbgWins);
		j.at("isShowUiWin").get_to(info.isShowUiWin);
	}

	std::string GetNormalizedPath(const std::string &path)
	{

		std::list<std::string> paths;

		std::stringstream ss("");

		for (auto c : path)
		{
			if (c == '\\' || c == '/')
			{
				paths.push_back(ss.str());
				ss.str(std::string());
				ss.clear();
				continue;
			}
			ss << c;
		}

		paths.push_back(ss.str());
		ss.str(std::string());
		ss.clear();

		std::list<std::string> paths2;

		for (auto s : paths)
		{
			if (s == ".." && !paths2.empty())
			{
				paths2.pop_back();
				continue;
			}
			else if (s == "." && !paths2.empty())
			{
				continue;
			}
			paths2.push_back(s);
		}

		if (paths2.front().empty())
		{
			ss << "/";
			paths2.pop_front();
		}

		for (auto s : paths2)
		{
			ss << s << "/";
		}

		// ss.seekp(-1, std::ios_base::cur);
		auto result = ss.str();
		return result.substr(0, result.size() - 1);
	}

	std::string getFileDir(const std::string &path)
	{

		size_t eIdx = path.find_last_of('/');
		if (eIdx == std::string::npos)
		{
			eIdx = path.find_last_of('\\');
		}
		if (eIdx == std::string::npos)
		{
			return path;
		}
		std::string dir = path.substr(0, eIdx);

		return dir;
	}

	std::string extractFileName(const std::string &path)
	{
		size_t eIdx = path.find_last_of('/');
		if (eIdx == std::string::npos)
		{
			eIdx = path.find_last_of('\\');
		}
		if (eIdx == std::string::npos)
		{
			return path;
		}
		std::string dir = path.substr(eIdx + 1);

		return dir;
	}

	bool EEmbDevice::BuildToJson(const std::string &path, const std::string &deviceName, std::list<std::shared_ptr<EEmbPeriph>> &periphs)
	{

		EEmbDevice device;
		device.deviceName = deviceName;
		device.jsonPath = GetNormalizedPath(path + "../artifacts/" + deviceName + "/Simulator");

		if (!fs::exists(device.jsonPath))
		{
			fs::create_directories(device.jsonPath);
		}

		for (auto &periph : periphs)
		{
			periph->isEditLabel = false;
			if (periph->typeId == PERIPH_TYPE_NONE)
			{
			}
			else if (periph->typeId == PERIPH_TYPE_IMG)
			{
				auto pImg = std::static_pointer_cast<EEmbImg>(periph);

				if (!fs::exists(pImg->imgPath))
				{
					std::cerr << "Error: File '" << pImg->imgPath << "' is not found!\n";
					return false;
				}

				auto artifactPath = device.jsonPath + "/" + extractFileName(pImg->imgPath);
				if (artifactPath != pImg->imgPath)
				{
					if (fs::exists(artifactPath))
					{
						fs::remove(artifactPath);
					}

					fs::copy_file(pImg->imgPath, artifactPath);
				}
				device.images.push_back(*pImg);
				device.images.back().imgPath = extractFileName(pImg->imgPath);
			}
			else if (periph->typeId == PERIPH_TYPE_UI)
			{
				device.UIs.push_back(*std::static_pointer_cast<EEmbUI>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_RS_485)
			{
				device.COMs.push_back(*std::static_pointer_cast<MB_modbus_t>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_AO)
			{
				device.AOs.push_back(*std::static_pointer_cast<EEmbAO>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_DO)
			{
				device.DOs.push_back(*std::static_pointer_cast<EEmbDO>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_DISPLAY)
			{
				device.displays.push_back(*std::static_pointer_cast<EEmbDisplay>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_LED)
			{
				device.LEDs.push_back(*std::static_pointer_cast<EEmbLED>(periph));
			}
			else if (periph->typeId == PERIPH_TYPE_BUTTON)
			{
				device.buttons.push_back(*std::static_pointer_cast<EEmbButton>(periph));
			}
		}

		device.jsonPath += "/SimulatorInfo.json";

		nlohmann::json j0 = device;
		std::ofstream ofile(device.jsonPath);
		ofile << std::setw(4) << j0;
		ofile.flush();
		ofile.close();

		return true;
	}

	bool EEmbDevice::LoadFromJson(EEmbDevice &device, const std::string &path, bool isResetToDefault)
	{

		if (!fs::exists(path))
		{
			std::cerr << "Error: File '" << path << "' is not found!\n";
			return false;
		}

		auto userFilePath = GetNormalizedPath(path + "/../UserSimulatorInfo.json");
		auto jsonPath = path;
		if (!isResetToDefault && fs::exists(userFilePath))
		{
			jsonPath = userFilePath;
		}

		std::ifstream inputFile(jsonPath);
		nlohmann::json j0 = nlohmann::json::parse(inputFile);
		inputFile.close();

		device = j0;
		device.jsonPath = jsonPath;

		for (auto &periph : device.images)
		{
			periph.imgPath = GetNormalizedPath(device.jsonPath + "/../" + extractFileName(periph.imgPath));
		}

		for (auto &periph : device.COMs)
		{
			periph.selectedPort = 0;
		}

		for (auto &periph : device.UIs)
		{
			periph.max = 0xFFF;
			periph.min = 0;
			periph.offset = 0;
			periph.mode = (EEmbUIMode) 0;
		}

		for (auto &periph : device.DOs)
		{
			periph.value = 0;
		}

		for (auto &periph : device.AOs)
		{
			periph.value = 0;
		}

		for (auto &periph : device.LEDs)
		{
			periph.value = 0;
		}

		for (auto &periph : device.buttons)
		{
			periph.state = 0;
		}

		return true;
	}

	bool EEmbDevice::saveToJson()
	{

		jsonPath = GetNormalizedPath(jsonPath + "/../UserSimulatorInfo.json");

		nlohmann::json j0 = *this;
		std::ofstream ofile(jsonPath);
		ofile << j0;
		ofile.flush();
		ofile.close();

		return true;
	}

	bool EEmbDevice::resetToDefault()
	{
		return LoadFromJson(*this, GetNormalizedPath(jsonPath + "/../SimulatorInfo.json"), true);
	}

	bool EEmbDevice::getPeriphs(std::list<std::shared_ptr<EEmbPeriph>> &periphs)
	{

		periphs.clear();

		for (auto &periph : images)
		{
			periphs.push_back(std::make_shared<EEmbImg>(periph));
		}

		for (auto &periph : COMs)
		{
			periphs.push_back(std::make_shared<MB_modbus_t>(periph));
		}

		for (auto &periph : UIs)
		{
			periphs.push_back(std::make_shared<EEmbUI>(periph));
		}

		for (auto &periph : AOs)
		{
			periphs.push_back(std::make_shared<EEmbAO>(periph));
		}

		for (auto &periph : DOs)
		{
			periphs.push_back(std::make_shared<EEmbDO>(periph));
		}

		for (auto &periph : LEDs)
		{
			periphs.push_back(std::make_shared<EEmbLED>(periph));
		}

		for (auto &periph : displays)
		{
			periphs.push_back(std::make_shared<EEmbDisplay>(periph));
		}

		for (auto &periph : buttons)
		{
			periphs.push_back(std::make_shared<EEmbButton>(periph));
		}

		return true;
	}

	bool EEmbDevice::getPeriphs(std::list<EEmbPeriph *> &periphs)
	{

		periphs.clear();

		for (auto &periph : images)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : COMs)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : UIs)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : AOs)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : DOs)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : LEDs)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : displays)
		{
			periphs.push_back(&periph);
		}

		for (auto &periph : buttons)
		{
			periphs.push_back(&periph);
		}

		return true;
	}
}
