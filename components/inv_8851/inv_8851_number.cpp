#include "inv_8851_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace inv_8851 {

void Inv8851Number::control(float value) {
    this->parent_->set_number_value(this->type_, value);
}

}  // namespace inv_8851
}  // namespace esphome
