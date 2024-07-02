#include "adf_audio_process.h"


#ifdef USE_ESP_IDF
#include "adf_pipeline.h"

#include <downmix.h>
#include "sdk_ext.h"
#define NUMBER_SOURCE_FILE 2
namespace esphome {
namespace esp_adf {

static const char *const TAG = "esp_audio_processors";


bool ADFResampler::init_adf_elements_(){
  downmix_cfg_t downmix_cfg = DEFAULT_DOWNMIX_CONFIG();
  downmix_cfg.downmix_info.source_num = NUMBER_SOURCE_FILE;
  this->sdk_downmixer_  = downmix_init(&downmix_cfg);
  esp_downmix_input_info_t source_info = {
        .samplerate = this->src_rate_,
        .channel = this->src_num_channels_,
        .gain = {0, 0},
        .transit_time = 10,
  };
    esp_downmix_input_info_t source_information[NUMBER_SOURCE_FILE] = {0};
    for (int i = 0; i < NUMBER_SOURCE_FILE; i++) {
        source_information[i] = source_info;
    }
  source_info_init(this->sdk_downmixer_, source_information);
  sdk_audio_elements_.push_back(this->sdk_downmixer_);
  sdk_element_tags_.push_back("downmixer");
  return true;
}

}  // namespace esp_adf
}  // namespace esphome

#endif
