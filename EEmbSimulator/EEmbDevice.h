

#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <atomic>


namespace EEmbSimulator {

	
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


	struct EEmbImg : EEmbPeriph {
		
		std::string imgPath;

		EEmbImg() : EEmbPeriph(PERIPH_TYPE_IMG) {
			this->targetHoverType = TARGET_HOVER_TYPE_NONE;
		}
		virtual void drawGUI() override;
	};

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
		}
		virtual void drawGUI() override;
	};


	struct EEmbAO : EEmbPeriph {
		float output;
		uint16_t value;
		EEmbAO() : EEmbPeriph(PERIPH_TYPE_AO), value(0) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	struct EEmbDO : EEmbPeriph {
		uint8_t value;
		EEmbDO() : EEmbPeriph(PERIPH_TYPE_DO), value(0) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};

	
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

	struct EEmbLED : EEmbPeriph {
		uint8_t value;
		VEC4 colorOn = { 1.0f, 0.0f, 0.0f, 0.8f };
		VEC4 colorOff = { 0.25f, 0.0f, 0.0f, 0.8f };
		EEmbLED() : EEmbPeriph(PERIPH_TYPE_LED) {
			this->targetHoverType = TARGET_HOVER_TYPE_CIRCLE;
		}
		virtual void drawGUI() override;
	};
}