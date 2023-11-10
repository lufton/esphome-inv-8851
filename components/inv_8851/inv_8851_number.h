#pragma once

#include "inv_8851.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace inv_8851 {

class Inv8851Number : public number::Number {
 public:
  void set_parent(Inv8851 *parent) { this->parent_ = parent; }
  void set_type(const std::string type) { this->type_ = type; }

 protected:
  void control(float value) override;

  Inv8851 *parent_;
  std::string type_;
};

}  // namespace inv_8851
}  // namespace esphome
