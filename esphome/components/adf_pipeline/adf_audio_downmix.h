#pragma once

#ifdef USE_ESP_IDF

#include "adf_audio_element.h"
namespace esphome {
namespace esp_adf {

class ADFPipelineProcessElement : public ADFPipelineElement {
 public:
  AudioPipelineElementType get_element_type() const { return AudioPipelineElementType::AUDIO_PIPELINE_PROCESS; }
};

class ADFDownmix : public ADFPipelineProcessElement {
 public:
  const std::string get_name() override { return "Downmix"; }

 protected:
  bool init_adf_elements_() override;
  void on_settings_request(AudioPipelineSettingsRequest &request) override;

  audio_element_handle_t sdk_downmix_;
};

}  // namespace esp_adf
}  // namespace esphome
#endif
