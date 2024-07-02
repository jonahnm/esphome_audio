#include "esp_adf_speaker.h"

#ifdef USE_ESP_IDF

#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace esp_adf {

static const char *const TAG = "esp_adf.mixer";

void ADFMixer::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ESP ADF Speaker...");
  pipeline.set_finish_timeout_ms(10000);
  
}

void ADFSpeaker::dump_config() {
  ESP_LOGCONFIG(TAG, "ESP ADF Speaker Configs:");
}


void ADFSpeaker::start() { pipeline.start(); }

void ADFSpeaker::start_() {
   pipeline.start();
}

void ADFSpeaker::stop() {
  if (this->state_ == speaker::STATE_STOPPED)
    return;
  this->state_ = speaker::STATE_STOPPING;
  pipeline.stop();
}

void ADFSpeaker::on_pipeline_state_change(PipelineState state) {
   switch (state) {
      case PipelineState::PREPARING:
        this->request_pipeline_settings_();
        break;
      case PipelineState::STARTING:
        break;
      case PipelineState::ABORTING:
        this->state_ = speaker::STATE_STOPPING;
        break;
      case PipelineState::RUNNING:
        this->state_ = speaker::STATE_RUNNING;
        break;
      case PipelineState::UNINITIALIZED:
      case PipelineState::STOPPED:
        this->state_ = speaker::STATE_STOPPED;
        break;
      case PipelineState::PAUSED:
        ESP_LOGI(TAG, "pipeline paused");
        break;
      default:
        break;
   }
}

void ADFSpeaker::loop() {
  this->pipeline.loop();
}

void ADFSpeaker::request_pipeline_settings_(){
    AudioPipelineSettingsRequest request{&this->pcm_stream_};
    request.sampling_rate = 16000;
    request.bit_depth = 16;
    request.number_of_channels = 1;
    request.finish_on_timeout = 1000; //ms
    request.target_volume = 1.;
    if (!this->pipeline.request_settings(request)) {
      esph_log_e(TAG, "Requested audio settings, didn't get accepted");
      this->pipeline.on_settings_request_failed(request);
    }
}


}  // namespace esp_adf
}  // namespace esphome

#endif  // USE_ESP_IDF
