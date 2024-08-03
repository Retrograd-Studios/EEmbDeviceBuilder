

#include "../EEmbSimulator/EEmbDevice.h"


#include "Render.h"

#undef NK_IMPLEMENTATION
#include <nuklear.h>


#include <vector>
#include <string>
#include <list>
#include <memory>
#include <unordered_map>






namespace EEmbSimulator {


	extern std::shared_ptr<EEmbPeriph> selectedElement;
	extern std::shared_ptr<EEmbPeriph> hoverElement;
	extern std::shared_ptr<EEmbPeriph> movedElement;

	extern std::vector<std::string> typeList;

	extern std::unordered_map<std::string, uint32_t> texturesMap;


	extern struct nk_context* ctx;



	// void EEmbPeriph::drawGUI(uint32_t id) 
	// {

	// }

	void EEmbImg::drawGUI() 
	{

		if (selectedElement.get() != this)
        {
			return;
		}


		if (nk_begin(ctx, typeList[selectedElement->typeId].c_str(),
                             nk_rect(WINDOW_WIDTH-300, 320, 250, 250), NK_WINDOW_BORDER | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
        {
            //nk_layout_row_dynamic(ctx, 25, 2);
            // nk_property_float(ctx, "x: ", 0, &selectedElement->targetRect.x, WINDOW_WIDTH, 1, .1f);
            // nk_property_float(ctx, "y: ", 0, &selectedElement->targetRect.y, WINDOW_HEIGHT, 1, .1f);
            // nk_property_float(ctx, "w: ", 0, &selectedElement->targetRect.w, WINDOW_WIDTH, 1, .1f);
        	// nk_property_float(ctx, "h: ", 0, &selectedElement->targetRect.h, WINDOW_HEIGHT, 1, .1f);

			uint32_t imageId = 0;

			const auto& cTex = texturesMap.find(this->imgPath);
			if (cTex != texturesMap.end())
			{
				imageId = cTex->second;
			}

			nk_layout_row_dynamic(ctx, 150, 1);

			if ( nk_group_begin(ctx, "Content", 0) )
			{
				nk_layout_row_dynamic(ctx, 55, 4);
				for (auto& tex : texturesMap)
				{
					if (nk_button_image(ctx, nk_image_id(tex.second)))
					{
						this->imgPath = tex.first;
					}
				}

				if (nk_button_symbol(ctx, NK_SYMBOL_PLUS))
				{
					
				}
				nk_group_end(ctx);
			}


			nk_layout_row_dynamic(ctx, 25, 2);

			if ( nk_button_label(ctx, "reset size") )
			{
				int width, height, nrComponents;
        		unsigned char *data = SOIL_load_image(this->imgPath.c_str(), &width, &height, &nrComponents, SOIL_LOAD_RGBA);
				SOIL_free_image_data(data);

				this->targetRect.w = width;
				this->targetRect.h = height;

				// this->targetRect.x = WINDOW_WIDTH / 2 - width / 2;
				// this->targetRect.y = WINDOW_HEIGHT / 2 - height / 2;
			}

			if ( nk_button_label(ctx, "reset pos") )
			{
				int width, height, nrComponents;
        		unsigned char *data = SOIL_load_image(this->imgPath.c_str(), &width, &height, &nrComponents, SOIL_LOAD_RGBA);
				SOIL_free_image_data(data);

				// this->targetRect.w = width;
				// this->targetRect.h = height;

				this->targetRect.x = WINDOW_WIDTH / 2 - this->targetRect.w / 2;
				this->targetRect.y = WINDOW_HEIGHT / 2 - this->targetRect.h / 2;
			}
        }
        nk_end(ctx);

	}


	void EEmbUI::drawGUI() 
	{



		static const std::string modeLabels[] = {
            "Mode: discrete",
            "Mode: 4-20 mA",
            "Mode: NTC10K"
        };


		if (selectedElement.get() == this)
        {
                static uint32_t wX =  WINDOW_WIDTH-300;
                static uint32_t wY =  320;

				if (nk_begin(ctx, typeList[selectedElement->typeId].c_str(),
                             nk_rect(wX, wY, 250, 250), NK_WINDOW_BORDER | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
                {

					nk_layout_row_dynamic(ctx, 25, 1);
                    if (nk_combo_begin_label(ctx, modeLabels[this->mode].c_str(), nk_vec2(nk_widget_width(ctx), 80)))
					{
						nk_layout_row_dynamic(ctx, 25, 1);
						for (auto i = 0; i < 3; ++i)
						{
							if (nk_combo_item_label(ctx, modeLabels[i].c_str(), NK_TEXT_ALIGN_CENTERED))
							{
								this->mode = (EEmbUIMode)i;
							}
						}
						nk_combo_end(ctx);
					}

                }
                auto bounds = nk_window_get_bounds(ctx);
                wX = bounds.x;
                wY = bounds.y;
                nk_end(ctx);
        }


		if (!this->isShowMenu)
        {
            return;
        }

        float w = this->hudRect.w != 0 ?  this->hudRect.w : 160;
        float h = this->hudRect.h != 0 ?  this->hudRect.h : 130;

        auto x = this->hudRect.x == 0 ?  this->targetRect.x + 40 : this->hudRect.x;
        auto y = this->hudRect.y == 0 ?  this->targetRect.y + 40 : this->hudRect.y;

        if ((x + w) > WINDOW_WIDTH)
        {
            x = WINDOW_WIDTH - w;
        }

        if ((y + h) > WINDOW_HEIGHT)
        {
            y = WINDOW_HEIGHT - h;
        }
        

        static const std::string inputLabels[] = {
            "Input (0/1): ",
            "Input (mA): ",
            "Input (C): "
        };
		
		if (this->label.empty() || this->label == "UI_")
		{
			this->label = "UI_";
			this->isShowMenu = false;
			return;
		}

        if (nk_begin_titled(ctx, std::to_string((uint64_t)this->label.c_str()).c_str(), this->label.c_str(),
            nk_rect(x, y, w, h),
            /*NK_WINDOW_NO_SCROLLBAR |*/ NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_PRIVATE))
        {
            
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, modeLabels[this->mode].c_str(), NK_TEXT_LEFT);

            if (this->mode == 0)
            {
                bool isInput = this->rawVal == 0.0f ? false : true;
               
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, inputLabels[this->mode].c_str(), NK_TEXT_LEFT);

                nk_layout_row_dynamic(ctx, 25, 2);
                isInput = nk_option_label(ctx, "on", isInput);
                isInput = !nk_option_label(ctx, "off", !isInput);
                this->rawVal = isInput ? 20.0 : 0.0;
            }
            else if (this->mode == 1)
            {
                nk_layout_row_dynamic(ctx, 25, 1);
                this->rawVal = nk_propertyf(ctx, inputLabels[this->mode].c_str(), 
                    3.5f, this->rawVal, 20.5f, 0.1f, 1.0f);
            }
            else if (this->mode == 2)
            {
                nk_layout_row_dynamic(ctx, 25, 1);
                this->rawVal = nk_propertyf(ctx, inputLabels[this->mode].c_str(),
                    -50.0f, this->rawVal, 100.0f, 0.1f, 1.0f);
            }

        }
        else
        {
            this->isShowMenu = false;
        }
        auto bounds = nk_window_get_bounds(ctx);
		this->hudRect.x = bounds.x;
		this->hudRect.y = bounds.y;
		this->hudRect.w = bounds.w;
		this->hudRect.h = bounds.h;

        nk_end(ctx);

	}



    void MB_modbus_t::refresh()
    {
        if (portList.size())
        {
            portList.pop_back();
        }
    }


		bool MB_modbus_t::isOpened()
        {
            return this->modbusInstance->isReady;
        }

		void MB_modbus_t::tryOpen()
        {
            this->modbusInstance->isReady = 1;
        }

		void MB_modbus_t::close()
        {
            this->modbusInstance->isReady = 0;
        }


    void MB_modbus_t::drawGUI() {



        auto& uart = this->modbusInstance; 

            static std::string parities[] = {
                "no parity",
                "even",
                "odd"
            };

            static std::string stopBits[] = {
                "one",
                "two"
            };



		if (selectedElement.get() == this)
        {
                static uint32_t wX =  WINDOW_WIDTH-300;
                static uint32_t wY =  320;

				if (nk_begin(ctx, typeList[selectedElement->typeId].c_str(),
                             nk_rect(wX, wY, 250, 250), NK_WINDOW_BORDER | NK_WINDOW_SCALABLE | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
                {

                    nk_layout_row_dynamic(ctx, 25, 1);

                    //nk_label(ctx, "PORT NUM: ", NK_TEXT_LEFT);
                    nk_property_int(ctx, "Physical port num: ", 0, (int32_t*)&this->periphPortNum, 0xFFFFFF, 1, 1.f);


                    //nk_label(ctx, "baudrate: ", NK_TEXT_LEFT);
                    nk_property_int(ctx, "baudrate: ", 0, (int32_t*)&uart->baudRate, 0xFFFFFF, 1, 1.f);


                    nk_label(ctx, "parities: ", NK_TEXT_LEFT);
                    if (nk_combo_begin_label(ctx, parities[uart->parity].c_str(), nk_vec2(nk_widget_width(ctx), 80)))
					{
						nk_layout_row_dynamic(ctx, 25, 1);
						for (auto i = 0; i < parities->size(); ++i)
						{
							if (nk_combo_item_label(ctx, parities[i].c_str(), NK_TEXT_ALIGN_CENTERED))
							{
								uart->parity = i;
							}
						}
						nk_combo_end(ctx);
					}

                    nk_label(ctx, "stopBits: ", NK_TEXT_LEFT);
                    if (nk_combo_begin_label(ctx, stopBits[uart->stopBits].c_str(), nk_vec2(nk_widget_width(ctx), 80)))
					{
						nk_layout_row_dynamic(ctx, 25, 1);
						for (auto i = 0; i < parities->size(); ++i)
						{
							if (nk_combo_item_label(ctx, stopBits[i].c_str(), NK_TEXT_ALIGN_CENTERED))
							{
								uart->stopBits = i;
							}
						}
						nk_combo_end(ctx);
					}


                    nk_layout_row_dynamic(ctx, 25, 2);
                    if (nk_button_label(ctx, "Add to List"))
                    {
                        for (auto i = 0; i < 5; ++i)
                        {
                            portList.push_back("COM"+std::to_string(i+1));
                        }
                        portList.push_back("Bluetooth-RS-{0x00ff0023-0x13446622-0x56778899-0x00223311}");
                    }

                    if (nk_button_label(ctx, "Clear List"))
                    {
                        this->portList.clear();
                        this->selectedPort = 0;
                    }

                }
                auto bounds = nk_window_get_bounds(ctx);
                wX = bounds.x;
                wY = bounds.y;
                nk_end(ctx);
        }


		if (!this->isShowMenu)
        {
            return;
        }

        float w = this->hudRect.w != 0 ?  this->hudRect.w : 160;
        float h = this->hudRect.h != 0 ?  this->hudRect.h : 130;

        auto x = this->hudRect.x == 0 ?  this->targetRect.x + 40 : this->hudRect.x;
        auto y = this->hudRect.y == 0 ?  this->targetRect.y + 40 : this->hudRect.y;

        if ((x + w) > WINDOW_WIDTH)
        {
            x = WINDOW_WIDTH - w;
        }

        if ((y + h) > WINDOW_HEIGHT)
        {
            y = WINDOW_HEIGHT - h;
        }
        
		
		if (this->label.empty() || this->label == "RS-485_COM")
		{
			this->label = "RS-485_COM";
			this->isShowMenu = false;
			return;
		}

        if (nk_begin_titled(ctx, std::to_string((uint64_t)this->label.c_str()).c_str(), this->label.c_str(),
            nk_rect(x, y, w, h),
            /*NK_WINDOW_NO_SCROLLBAR |*/ NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_PRIVATE))
        {
            
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, "Port: ", NK_TEXT_LEFT);
            if (this->portList.size())
            {
                    nk_layout_row_dynamic(ctx, 25, 1);
                    if (nk_combo_begin_label(ctx, this->portList[this->selectedPort].c_str(), nk_vec2(nk_widget_width(ctx), 80)))
					{
						nk_layout_row_dynamic(ctx, 25, 1);
						for (auto i = 0; i < this->portList.size(); ++i)
						{
							if (nk_combo_item_label(ctx, this->portList[i].c_str(), NK_TEXT_ALIGN_CENTERED))
							{
								this->selectedPort = i;
							}
						}
						nk_combo_end(ctx);
					}
            }
            else
            {
                nk_label(ctx, "no ports", NK_TEXT_LEFT);
            }

            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_button_label(ctx, "refresh"))
            {
                refresh();
            }

            
            nk_layout_row_dynamic(ctx, 25, 2);
            nk_label(ctx, "baudrate: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(uart->baudRate).c_str(), NK_TEXT_LEFT);
            nk_label(ctx, "parity: ", NK_TEXT_LEFT);
            nk_label(ctx, parities[uart->parity].c_str(), NK_TEXT_LEFT);
            nk_label(ctx, "stop bits: ", NK_TEXT_LEFT);
            nk_label(ctx, stopBits[uart->stopBits].c_str(), NK_TEXT_LEFT);

            if (isOpened())
            {
                //nk_layout_row_dynamic(ctx, 25, 3);
                nk_layout_space_begin(ctx, NK_STATIC, 25, 3);
                nk_layout_space_push(ctx, { 0, 0, 55, 25 });
                nk_label(ctx, "status: ", NK_TEXT_LEFT);
                nk_layout_space_push(ctx, { 65, 0, 25, 25 });
                nk_button_color(ctx, { 120, 255, 120, 255 });
                nk_layout_space_push(ctx, { 95, 0, 55, 25 });
                nk_label(ctx, "opened", NK_TEXT_LEFT);
                nk_layout_space_end(ctx);
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_button_label(ctx, "close"))
                {
                    close();
                }
            }
            else
            {
                nk_layout_space_begin(ctx, NK_STATIC, 25, 3);
                nk_layout_space_push(ctx, { 0, 0, 55, 25 });
                nk_label(ctx, "status: ", NK_TEXT_LEFT);
                nk_layout_space_push(ctx, { 65, 0, 25, 25 });
                nk_button_color(ctx, { 255, 120, 120, 255 });
                nk_layout_space_push(ctx, { 95, 0, 55, 25 });
                nk_label(ctx, "closed", NK_TEXT_LEFT);
                nk_layout_space_end(ctx);
                nk_layout_row_dynamic(ctx, 25, 1);
                if (portList.size())
                if (nk_button_label(ctx, "open"))
                {
                    tryOpen();
                }
            }

        }
        else
        {
            this->isShowMenu = false;
        }
        auto bounds = nk_window_get_bounds(ctx);
		this->hudRect.x = bounds.x;
		this->hudRect.y = bounds.y;
		this->hudRect.w = bounds.w;
		this->hudRect.h = bounds.h;

        nk_end(ctx);

    }



    // void MB_aoutput_t::drawGUI(uint32_t id) {
    //     if (!this->isShowMenu)
    //     {
    //         return;
    //     }

    //     Targets& target = targets[id];

    //     std::string title = labels[id] + " info: ";

    //     float w = 150;
    //     float h = 75;

    //     float x = target.pos.x;
    //     float y = target.pos.y + 40;

    //     if ((x + w) > SCR_WIDTH)
    //     {
    //         x = SCR_WIDTH - w;
    //     }

    //     if ((y + h) > SCR_HEIGHT)
    //     {
    //         y = SCR_HEIGHT - h;
    //     }

    //     this->output = (float) this->value / 1000.f;

    //     if (nk_begin(ctx, title.c_str(),
    //         nk_rect(x, y, w, h),
    //         NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_CLOSABLE))
    //     {
    //         nk_layout_row_dynamic(ctx, 25, 1);
    //         nk_labelf(ctx, NK_TEXT_LEFT, "Output: %.3f V", this->output);
    //     }
    //     else
    //     {
    //         this->isShowMenu = false;
    //     }
    //     nk_end(ctx);
    // }
    // void MB_doutput_t::drawGUI(uint32_t id) {

    //     if (!this->isShowMenu)
    //     {
    //         return;
    //     }

    //     Targets& target = targets[id];

    //     std::string title = labels[id] + " info: ";

    //     float w = 150;
    //     float h = 75;

    //     float x = target.pos.x;
    //     float y = target.pos.y + 40;

    //     if ((x + w) > SCR_WIDTH)
    //     {
    //         x = SCR_WIDTH - w;
    //     }

    //     if ((y + h) > SCR_HEIGHT)
    //     {
    //         y = SCR_HEIGHT - h;
    //     }


    //     if (nk_begin(ctx, title.c_str(),
    //         nk_rect(x, y, w, h),
    //         NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_CLOSABLE))
    //     {
    //         nk_layout_row_dynamic(ctx, 25, 2);
    //         nk_label(ctx, "Output: ", NK_TEXT_LEFT);
    //         nk_label(ctx, value ? "HIGH" : "LOW", NK_TEXT_LEFT);

    //     }
    //     else
    //     {
    //         this->isShowMenu = false;
    //     }
    //     nk_end(ctx);
    // }






}



