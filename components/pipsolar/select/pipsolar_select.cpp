#include "pipsolar_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pipsolar {

static const char *const TAG = "pipsolar.select";

void PipsolarSelect::dump_config() { LOG_SELECT(TAG, "Pipsolar Controller Select", this); }

void PipsolarSelect::control(const std::string &value) {
  ESP_LOGD(TAG, "got option: %s", value.c_str());

  if (this->is_bulk_voltage_ || this->is_float_voltage_) {
    float float_val = std::stof(value);
    uint16_t raw_val = (uint16_t)(float_val * 10.0f);

    if (this->is_bulk_voltage_) {
      this->parent_->last_bulk_voltage_ = raw_val;
    } else if (this->is_float_voltage_) {
      this->parent_->last_float_voltage_ = raw_val;
    }

    // Ruft die kombinierte Sende-Funktion aus der pipsolar.cpp auf
    this->parent_->send_mchgv_command();

    if (this->optimistic_) {
      this->publish_state(value);
    }
    return;
  }

  if (this->mapping_.find(value) != this->mapping_.end()) {
    ESP_LOGD(TAG, "found mapped option %s for option %s", this->mapping_[value].c_str(), value.c_str());
    this->parent_->switch_command(this->mapping_[value]);
  } else {
    ESP_LOGD(TAG, "could not find option %s in mapping", value.c_str());
    return;
  }
  // auto options = this->traits.get_options();
  // auto opt_it = std::find(options.cbegin(), options.cend(), value);
  // size_t idx = std::distance(options.cbegin(), opt_it);
  // std::string mapval = this->mapping_[idx];
  // ESP_LOGD(TAG, "Found value %s for option '%s'", *mapval.c_str(), value.c_str());

  if (this->optimistic_)
    this->publish_state(value);
}

void PipsolarSelect::map_and_publish(std::string &value) {
  ESP_LOGD(TAG, "got value: %s", value.c_str());
  if (this->status_mapping_.find(value) != this->status_mapping_.end()) {
    ESP_LOGD(TAG, "found mapped option %s for option %s", this->status_mapping_[value].c_str(), value.c_str());
    this->publish_state(this->status_mapping_[value]);
  } else {
    ESP_LOGD(TAG, "could not find option %s in mapping", value.c_str());
    return;
  }
}

}  // namespace pipsolar
}  // namespace esphome
