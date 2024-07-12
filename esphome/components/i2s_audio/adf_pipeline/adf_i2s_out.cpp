#include "adf_i2s_out.h"
#ifdef USE_ESP_IDF

#include "../../adf_pipeline/sdk_ext.h"
#include "i2s_stream_mod.h"
#include "../../adf_pipeline/adf_pipeline.h"
#include <pwm_stream.h>
#ifdef I2S_EXTERNAL_DAC
#include "../external_dac.h"
#endif
namespace esphome {
using namespace esp_adf;
namespace i2s_audio {

static const char *const TAG = "adf_i2s_out";


void ADFElementI2SOut::setup() {
}

bool ADFElementI2SOut::init_adf_elements_() {
  if (this->sdk_audio_elements_.size() > 0)
    return true;

 pwm_stream_cfg_t pwm_cfg = PWM_STREAM_CFG_DEFAULT();
    pwm_cfg.pwm_config.gpio_num_left = 22;
    pwm_cfg.pwm_config.gpio_num_right = 4;
    this->adf_i2s_stream_writer_ = pwm_stream_init(&pwm_cfg);
  sdk_audio_elements_.push_back(this->adf_i2s_stream_writer_);
  sdk_element_tags_.push_back("i2s_out");

  return true;
}

void ADFElementI2SOut::clear_adf_elements_(){
  this->sdk_audio_elements_.clear();
  this->sdk_element_tags_.clear();
}

bool ADFElementI2SOut::is_ready(){
  return true;
}

void ADFElementI2SOut::on_settings_request(AudioPipelineSettingsRequest &request) {
  if ( !this->adf_i2s_stream_writer_ ){
    return;
  }

  if (this->is_adjustable()){
    bool rate_bits_channels_updated = false;
    if (request.sampling_rate > 0 && (uint32_t) request.sampling_rate != this->sample_rate_) {
      this->sample_rate_ = request.sampling_rate;
      rate_bits_channels_updated = true;
    }

    if(request.number_of_channels > 0 && (uint8_t) request.number_of_channels != this->num_of_channels())
    {
      this->channel_fmt_ = request.number_of_channels == 1 ? I2S_CHANNEL_FMT_ONLY_RIGHT : I2S_CHANNEL_FMT_RIGHT_LEFT;
      rate_bits_channels_updated = true;
    }

    if (request.bit_depth > 0 && (uint8_t) request.bit_depth != this->bits_per_sample_) {
      bool supported = request.bit_depth == 16;
      if (!supported) {
        request.failed = true;
        request.failed_by = this;
        return;
      }
      this->bits_per_sample_ = (i2s_bits_per_sample_t) request.bit_depth;
      rate_bits_channels_updated = true;
    }

    if (rate_bits_channels_updated) {

      audio_element_set_music_info(this->adf_i2s_stream_writer_,this->sample_rate_, this->num_of_channels(), this->bits_per_sample_ );

      esph_log_d(TAG, "update i2s clk settings: rate:%d bits:%d ch:%d",this->sample_rate_, this->bits_per_sample_, this->num_of_channels());
     // pwm_stream_t *i2s = (pwm_stream_t *)audio_element_getdata(this->adf_i2s_stream_writer_);
      //i2s->config.pwm_config.bits_per_sample = this->bits_per_sample_;
      if (pwm_stream_set_clk(this->adf_i2s_stream_writer_, this->sample_rate_, this->bits_per_sample_,
                            this->num_of_channels()) != ESP_OK) {
        esph_log_e(TAG, "error while setting sample rate and bit depth,");
        request.failed = true;
        request.failed_by = this;
        return;
      }
    }
  }
/*
  if ( request.finish_on_timeout != this->finish_on_timeout_ms_ ){
    esph_log_d(TAG, "Setting finish_on_timout to (ms): %d", request.finish_on_timeout);
    this->finish_on_timeout_ms_ = request.finish_on_timeout;
    i2s_stream_t *i2s = (i2s_stream_t *) audio_element_getdata(this->adf_i2s_stream_writer_);
    esph_log_d(TAG, "finish on timeout was: %s",i2s->finish_on_timeout ? "true":"false");
    i2s->finish_on_timeout = this->finish_on_timeout_ms_ > 0;
    audio_element_set_input_timeout(this->adf_i2s_stream_writer_, this->finish_on_timeout_ms_ / portTICK_PERIOD_MS);
  }
  */
  // final pipeline settings are unset
  if (request.final_sampling_rate == -1) {
    esph_log_d(TAG, "Set final i2s settings: %d", this->sample_rate_);
    request.final_sampling_rate = this->sample_rate_;
    request.final_bit_depth = 16;
    request.final_number_of_channels = this->num_of_channels();
  } else if (
       request.final_sampling_rate != this->sample_rate_
    || request.final_bit_depth != 16
    || request.final_number_of_channels != this->num_of_channels()
  )
  {
    request.failed = true;
    request.failed_by = this;
  }
/*
  if (this->use_adf_alc_ && request.target_volume > -1) {
    int target_volume = (int) (request.target_volume * 128. * this->max_alc_val_) - 64;
    if (i2s_alc_volume_set(this->adf_i2s_stream_writer_, target_volume) != ESP_OK) {
      esph_log_e(TAG, "error setting volume to %d", target_volume);
      request.failed = true;
      request.failed_by = this;
      return;
    }
  }
#ifdef I2S_EXTERNAL_DAC
  if( this->external_dac_ != nullptr && request.target_volume > -1){
    this->external_dac_->set_volume( request.target_volume);
  }
#endif
*/
}

}  // namespace i2s_audio
}  // namespace esphome
#endif
