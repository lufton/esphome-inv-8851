#include "inv_8851_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace inv_8851 {

void Inv8851Select::control(const std::string &value) {
    auto index = this->index_of(value);
    if (index.has_value()) this->parent_->set_select_value(this->type_, index.value());
    else ESP_LOGE(TAG, "There is no %s option for %s select.", value, this->type_);
}

}  // namespace inv_8851
}  // namespace esphome
