#pragma once

#include "inv_8851.h"
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace inv_8851 {

class Inv8851Select : public select::Select {
 public:
  void set_parent(Inv8851 *parent) { this->parent_ = parent; }
  void set_type(const std::string type) { this->type_ = type; }

 protected:
  void control(const std::string &value) override;

  Inv8851 *parent_;
  std::string type_;
};

}  // namespace inv_8851
}  // namespace esphome
