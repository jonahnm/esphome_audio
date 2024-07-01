#include "adf_audio_process.h"


#ifdef USE_ESP_IDF
#include "adf_pipeline.h"
#include <downmix.h>
#include <filter_resample.h>
#include "sdk_ext.h"

namespace esphome {
namespace esp_adf {

static const char *const TAG = "esp_audio_processors";

bool ADFResampler::init_adf_elements_(){
  downmix_cfg_t downmix_cfg = DEFAULT_DOWNMIX_CONFIG();
  downmix_cfg.downmix_info.source_num = 2;
  this->sdk_downmix_= downmix_init(&downmix_cfg);
  downmix_set_input_rb_timeout(this->sdk_downmix_, 0, 0);
  downmix_set_input_rb_timeout(this->sdk_downmix_, 0, 1);
  sdk_audio_elements_.push_back(this->sdk_downmix_);
  sdk_element_tags_.push_back("downmix");
  return true;
}

void ADFResampler::on_settings_request(AudioPipelineSettingsRequest &request){

  bool settings_changed = false;
  if( request.sampling_rate > -1 ){
    if( request.sampling_rate != this->src_rate_ )
    {
      this->src_rate_ = request.sampling_rate;
      settings_changed = true;
    }
    uint32_t dst_rate = request.final_sampling_rate > -1 ? request.final_sampling_rate : this->src_rate_;
    if( dst_rate != this->dst_rate_ )
    {
      this->dst_rate_ = dst_rate;
      settings_changed = true;
    }
  }
  if( request.number_of_channels > -1 ){
    if( request.number_of_channels != this->src_num_channels_ )
    {
      this->src_num_channels_ = request.number_of_channels;
      settings_changed = true;
    }
    uint32_t dst_num_channels = request.final_number_of_channels > -1 ? request.final_number_of_channels : this->src_num_channels_;
    if( dst_num_channels != this->dst_num_channels_ )
    {
      this->dst_num_channels_ = dst_num_channels;
      settings_changed = true;
    }
  }

  if( request.bit_depth > -1 ){
    if( request.bit_depth != this->src_bit_depth_ )
    {
      this->src_bit_depth_ = request.bit_depth;
      settings_changed = true;
    }
  }

  if ( request.final_bit_depth > -1 && request.final_bit_depth != 16 ){
    request.bit_depth = 16;
    request.final_bit_depth = 16;
    request.requested_by = this;
    this->pipeline_->request_settings(request);
  }



  if( this->sdk_resampler_ && settings_changed)
  {
    if( audio_element_get_state(this->sdk_resampler_) == AEL_STATE_RUNNING)
    {
      audio_element_stop(this->sdk_resampler_);
      audio_element_wait_for_stop(this->sdk_resampler_);
    }
    ADFPipelineElement* el = request.requested_by;
    if( el != nullptr ){
     esph_log_d(TAG, "Received request from: %s", el->get_name().c_str());
    }
    else {
    esph_log_d(TAG, "Called from invalid caller");
    }
    esph_log_d(TAG, "New settings: SRC: rate: %d, ch: %d bits: %d, DST: rate: %d, ch: %d, bits %d", this->src_rate_, this->src_num_channels_, this->src_bit_depth_, this->dst_rate_, this->dst_num_channels_, 16);
    rsp_filter_t *filter = (rsp_filter_t *)audio_element_getdata(this->sdk_resampler_);
    resample_info_t &resample_info = *(filter->resample_info);
    resample_info.src_rate = this->src_rate_;
    resample_info.dest_rate = this->dst_rate_;
    resample_info.src_ch = this->src_num_channels_;
    resample_info.dest_ch = this->dst_num_channels_;
    resample_info.src_bits = this->src_bit_depth_;
    resample_info.dest_bits = 16;
  }

  esph_log_d(TAG, "Current settings: SRC: rate: %d, ch: %d bits: %d, DST: rate: %d, ch: %d, bits %d", this->src_rate_, this->src_num_channels_, this->src_bit_depth_, this->dst_rate_, this->dst_num_channels_, 16);
}

}  // namespace esp_adf
}  // namespace esphome

#endif
