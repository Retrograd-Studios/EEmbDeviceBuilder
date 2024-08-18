

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include <nlohmann/json.hpp>


namespace EEmbSimulator {

	#define WINDOW_WIDTH 1280
    #define WINDOW_HEIGHT 800

	
	enum BTNS_POS {
		BUTTON_LEFT = 0,
		BUTTON_RIGHT,
		BUTTON_DOWN,
		BUTTON_UP,
		BUTTON_ESC,
		BUTTON_ENTER,
		BOTTON_COUNT
	};

	enum EEmbUIMode {
		EEMB_UI_MODE_DI = 0,
		EEMB_UI_MODE_4_20,
		EEMB_UI_MODE_TEMP
	};


	enum PeriphTypeId {
		PERIPH_TYPE_NONE = 0,
		PERIPH_TYPE_IMG,
		PERIPH_TYPE_BUTTON,
		PERIPH_TYPE_UI,
		PERIPH_TYPE_RS_485,
		PERIPH_TYPE_AO,
		PERIPH_TYPE_DO,
		PERIPH_TYPE_LED,
		PERIPH_TYPE_DISPLAY
	};

	enum TargetHoverType {
		TARGET_HOVER_TYPE_NONE = 0,
		TARGET_HOVER_TYPE_RECT,
		TARGET_HOVER_TYPE_CIRCLE
	};

	enum EEmbDisplayType{
		EEMB_DISPLAY_TYPE_ST7735 = 0,
		EEMB_DISPLAY_TYPE_COUNT
	};



	struct VEC4 {
		float x;
		float y;
		float w;
		float h;
	};

	void to_json(nlohmann::json& j, const VEC4& info);
	void from_json(const nlohmann::json& j, VEC4& info);


	struct MBL_modbus_t
	{
		//uint8_t pageBuf[256];
		// uint8_t txBuf[MBL_BUF_RX_SIZE];
		// uint8_t rxBuf[MBL_BUF_TX_SIZE];

		// const ModbusRemoteGrpReg_t * reqRemoteModuleGrp;
		// //const EEmb_modbus_remote_reg_t* reqRemoteReg;
		// //uint8_t* currentPage;
		// const ModbusHRegGroup_t* holdingsGrps;
		// const ModbusIRegGroup_t* inputsGrps;
		//const ModbusRemoteGrpReg_t* remoteRegGrps;
		uint32_t errorCode;
		//uint32_t remoteRegAddrReq;
		//uint32_t remoteRegCount;
		uint32_t baudRate;
		uint16_t holdingGrpCount;
		uint16_t inputGrpCount;
		volatile uint32_t rxTimeout;
		uint32_t rxTimeoutInit;
		uint32_t frameTimeout;
		volatile uint16_t txSize;
		volatile uint16_t rxSize;
		uint16_t rxPrevSize;
		uint16_t updateRegsTimeout;
		volatile uint16_t txTimeout;
		uint32_t sentCount;
        uint32_t sucCount;
        uint32_t errorCount;
		uint8_t modbusAddr;
		uint8_t parity;
		uint8_t stopBits;
		uint8_t isMaster;
		uint8_t isTimeout;
		uint8_t uartId;
        uint8_t isInited;
        uint8_t isReinit;


		//uint32_t waiting;
		//uint32_t timeout;
		uint32_t tryCount;
		uint32_t maxTryCount;
		//uint32_t errorsFlags;

		MBL_modbus_t() :  isReady(0), parity(0), stopBits(0), modbusAddr(0), baudRate(9600) {}

		volatile uint8_t isReady;
        volatile uint8_t isBusy;
	};



	struct EEmbPeriph
	{
		PeriphTypeId typeId;
		std::string label = "";
		VEC4 labelRect = {.x = 0, .y = 0, .w = 0, .h = 0 };
		VEC4 hudRect = {.x = 0, .y = 0, .w = 0, .h = 0 };
		VEC4 targetRect = {.x = 30, .y = 30, .w = 40, .h = 40 };
		TargetHoverType targetHoverType = TARGET_HOVER_TYPE_NONE;
		bool isEditLabel = false;
		bool isShowMenu = false;
		EEmbPeriph(PeriphTypeId typeId = PERIPH_TYPE_NONE) : typeId(typeId) {}
		virtual void drawGUI() {}
	};

	void to_json(nlohmann::json& j, const EEmbPeriph& info);
	void from_json(const nlohmann::json& j, EEmbPeriph& info);



	struct EEmbImg : EEmbPeriph {
		
		std::string imgPath;

		EEmbImg() : EEmbPeriph(PERIPH_TYPE_IMG) {
			this->targetHoverType = TARGET_HOVER_TYPE_NONE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbImg& info);
	void from_json(const nlohmann::json& j, EEmbImg& info);

	struct EEmbUI : EEmbPeriph {
		float rawVal;
		uint32_t max = 1000;
		uint32_t min = 0;
		uint32_t val = 0;
		uint32_t offset = 0;
		EEmbUIMode mode = EEmbUIMode::EEMB_UI_MODE_DI;
		EEmbUI() : EEmbPeriph(PERIPH_TYPE_UI) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbUI& info);
	void from_json(const nlohmann::json& j, EEmbUI& info);


	struct MB_modbus_t : EEmbPeriph {
		std::vector<std::string> portList;
		uint32_t selectedPort = 0;
		uint32_t periphPortNum = 0;
		void refresh();
		bool isOpened();
		void tryOpen();
		void close();
		std::unique_ptr<MBL_modbus_t> modbusInstance;
		MB_modbus_t() : EEmbPeriph(PERIPH_TYPE_RS_485) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
			modbusInstance = std::make_unique<MBL_modbus_t>();
		}

		MB_modbus_t(const MB_modbus_t& com) : EEmbPeriph(com),  
			portList(com.portList), selectedPort(com.selectedPort), 
			modbusInstance(std::make_unique<MBL_modbus_t>()) {}

		MB_modbus_t& operator=(const MB_modbus_t& com)
		{
			*static_cast<EEmbPeriph*>(this) = com;
			this->portList = com.portList;
			this->selectedPort = com.selectedPort;
			this->modbusInstance = std::make_unique<MBL_modbus_t>();
			return *this;
		}

		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const MB_modbus_t& info);
	void from_json(const nlohmann::json& j, MB_modbus_t& info);


	struct EEmbAO : EEmbPeriph {
		float output;
		uint16_t value;
		EEmbAO() : EEmbPeriph(PERIPH_TYPE_AO), value(0) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbAO& info);
	void from_json(const nlohmann::json& j, EEmbAO& info);



	struct EEmbDO : EEmbPeriph {
		uint8_t value;
		EEmbDO() : EEmbPeriph(PERIPH_TYPE_DO), value(0) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbDO& info);
	void from_json(const nlohmann::json& j, EEmbDO& info);

	
	struct EEmbDisplay : EEmbPeriph {
		uint32_t width;
		uint32_t height;
		uint32_t screenDataWidth;
		uint32_t screenDataHeight;
		std::unique_ptr<uint8_t[]> screenData = nullptr;
		std::atomic<bool> isRedraw;
		std::atomic<float> currentDisplayBrights;
		uint32_t screenTexture;
		EEmbDisplayType displayType = EEmbDisplayType::EEMB_DISPLAY_TYPE_ST7735;
		EEmbDisplay() : EEmbPeriph(PERIPH_TYPE_DISPLAY), isRedraw(false), currentDisplayBrights(1.f) {
			this->targetHoverType = TARGET_HOVER_TYPE_RECT;
			resetSize();
			rebuildScreen();
		}

		EEmbDisplay(const EEmbDisplay& disp) : EEmbPeriph(disp),
			width(disp.width), height(disp.height),
			//screenDataWidth(disp.screenDataWidth), screenDataHeight(disp.screenDataHeight),
			screenData(nullptr),
			//isRedraw(disp.isRedraw.load()),
			currentDisplayBrights(disp.currentDisplayBrights.load()),
			screenTexture(disp.screenTexture) 
		{
			//resetSize();
			rebuildScreen();
		} 

		EEmbDisplay(const EEmbDisplay&& disp) : EEmbPeriph(disp),
			width(disp.width), height(disp.height),
			//screenDataWidth(disp.screenDataWidth), screenDataHeight(disp.screenDataHeight),
			screenData(nullptr),
			//isRedraw(disp.isRedraw.load()),
			currentDisplayBrights(disp.currentDisplayBrights.load()),
			screenTexture(disp.screenTexture) 
		{
			//resetSize();
			rebuildScreen();
		} 

		EEmbDisplay& operator=(const EEmbDisplay& disp)
		{
			*static_cast<EEmbPeriph*>(this) = disp;

			this->width = disp.width;
			this->height = disp.height;
			this->currentDisplayBrights = disp.currentDisplayBrights.load();
			this->screenTexture = disp.screenTexture;
			//this->resetSize();
			this->rebuildScreen();

			return *this;
		}

		void rebuildScreen() {

			screenDataWidth = 1;
			screenDataHeight = 1;
			while (screenDataWidth < width)
			{
				screenDataWidth *= 2;
			}
			while (screenDataHeight < height)
			{
				screenDataHeight *= 2;
			}
			screenData = std::make_unique<uint8_t[]>(screenDataWidth*screenDataHeight*3);
			memset(screenData.get(), 0xFF, screenDataWidth*screenDataHeight*3);
			isRedraw = true;
		}

		void resetSize() {

			if (displayType == EEMB_DISPLAY_TYPE_ST7735)
			{
				width = 160;
				height = 128;
			}

		}


		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbDisplay& info);
	void from_json(const nlohmann::json& j, EEmbDisplay& info);


	struct EEmbLED : EEmbPeriph {
		uint8_t value;
		VEC4 colorOn = { 1.0f, 0.0f, 0.0f, 0.8f };
		VEC4 colorOff = { 0.25f, 0.0f, 0.0f, 0.8f };
		EEmbLED() : EEmbPeriph(PERIPH_TYPE_LED) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbLED& info);
	void from_json(const nlohmann::json& j, EEmbLED& info);


	struct EEmbButton : EEmbPeriph {
		uint8_t buttonId = BUTTON_ENTER;
		uint8_t state;
		EEmbButton() : EEmbPeriph(PERIPH_TYPE_BUTTON) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	void to_json(nlohmann::json& j, const EEmbButton& info);
	void from_json(const nlohmann::json& j, EEmbButton& info);


	enum EEmbSimulatorVisableDbgWinsId {
		SIM_DBG_WIN_MB_REGS_ID = 0,
		SIM_DBG_WIN_GLOB_VARS_ID,
		SIM_DBG_WIN_REM_MODULES_ID
	};

	#define EEMB_SIM_ALL_DBG_WINS_MASK ( 7 )


	struct EEmbDevice {
		
		std::string jsonPath;
		std::string deviceName;

		std::vector<EEmbImg> images;
		std::vector<EEmbUI> UIs;
		std::vector<EEmbAO> AOs;
		std::vector<EEmbDO> DOs;
		std::vector<MB_modbus_t> COMs;
		std::vector<EEmbDisplay> displays;
		std::vector<EEmbLED> LEDs;
		std::vector<EEmbButton> buttons;

		std::atomic<uint32_t> btnsPressed;
    	std::atomic<uint32_t> btnsReleased;

		std::list<std::string> globVarsFavs;
		std::list<std::string> mbRegsFavs;

		VEC4 mbRegsWin = {.x = 5, .y = 5, .w = 320, .h = 480};
		//VEC4 globVarsWin = {.x = WINDOW_WIDTH-255, .y = 100 + 315, .w = 250, .h = 310};
		VEC4 globVarsWin = {.x = WINDOW_WIDTH-255, .y = 100, .w = 250, .h = 310};
		VEC4 remoteModulesWin = {.x = WINDOW_WIDTH-255, .y = 100 + 315 + 310, .w = 250, .h = 310};

		uint32_t simVisableDbgWins;


		EEmbDevice() : btnsPressed(0), btnsReleased(0) {}


		EEmbDevice(const EEmbDevice& dev) : btnsPressed(0), btnsReleased(0)
		{
			this->jsonPath = dev.jsonPath;
			this->deviceName = dev.deviceName;

			this->images = dev.images;
			this->UIs = dev.UIs;
			this->AOs = dev.AOs;
			this->DOs = dev.DOs;
			this->COMs = dev.COMs;
			this->displays = dev.displays;
			this->LEDs = dev.LEDs;
			this->buttons = dev.buttons;

			globVarsFavs = dev.globVarsFavs;
			mbRegsFavs = dev.mbRegsFavs;

			globVarsWin = dev.globVarsWin;
			mbRegsWin = dev.mbRegsWin;
			remoteModulesWin = dev.remoteModulesWin;

			simVisableDbgWins = dev.simVisableDbgWins;

		}

		EEmbDevice& operator=(const EEmbDevice& dev)
		{
			this->jsonPath = dev.jsonPath;
			this->deviceName = dev.deviceName;

			this->images = dev.images;
			this->UIs = dev.UIs;
			this->AOs = dev.AOs;
			this->DOs = dev.DOs;
			this->COMs = dev.COMs;
			this->displays = dev.displays;
			this->LEDs = dev.LEDs;
			this->buttons = dev.buttons;

			this->btnsPressed.store(dev.btnsPressed);
			this->btnsReleased.store(dev.btnsReleased);

			globVarsFavs = dev.globVarsFavs;
			mbRegsFavs = dev.mbRegsFavs;

			globVarsWin = dev.globVarsWin;
			mbRegsWin = dev.mbRegsWin;
			remoteModulesWin = dev.remoteModulesWin;

			simVisableDbgWins = dev.simVisableDbgWins;

			return *this;
		}

		static bool BuildToJson(const std::string& path, const std::string& deviceName, std::list<std::shared_ptr<EEmbPeriph>>& periphs);
		static bool LoadFromJson(EEmbDevice& device, const std::string& path, bool isResetToDefault);
		bool saveToJson();
		bool resetToDefault();

		bool getPeriphs(std::list<std::shared_ptr<EEmbPeriph>>& periphs);
		bool getPeriphs(std::list<EEmbPeriph*>& periphs);

	};

	void to_json(nlohmann::json& j, const EEmbDevice& info);
	void from_json(const nlohmann::json& j, EEmbDevice& info);


}


