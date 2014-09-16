#include "configparser.h"

Configparser::Configparser() : credits_(1000), print_active_(false){
  settings_.clear();
}

bool Configparser::parseFile(std::string filename) {
  settings_.clear();
  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename.data());

  assert(doc.ErrorID() == 0);

  tinyxml2::XMLElement* root =
      doc.FirstChildElement("config") ?
          doc.FirstChildElement("config") : nullptr;
  if (root == nullptr)
    return Error("has to start with <config>");

  int rounds = 3;

  //FIXME: Better solution
//  perm_.reset(new AsconPermutation(3));

  if (root->FirstChildElement("parameters") != nullptr) {
    tinyxml2::XMLElement* parameters = root->FirstChildElement("parameters");
    if (parameters->FirstChildElement("rounds") != nullptr)
      rounds = parameters->FirstChildElement("rounds")->IntAttribute("value");
    else
      rounds = 1;

//    std::cout << rounds << std::endl;
    std::string instance { parameters->FirstChildElement("permutation")->Attribute("value")};

    perm_.reset(permutation_list(instance,rounds));
  }

  if (root->FirstChildElement("char") != nullptr) {
    std::string characteristic { root->FirstChildElement("char")->Attribute(
        "value") };
    BitMask bm;
    unsigned int bit_pos = 0;
    for (auto& val : characteristic) {
      switch (val) {
        case '?':
          bm = BM_DUNNO;
          break;
        case '0':
          bm = BM_0;
          break;
        case '1':
          bm = BM_1;
          break;
        default:
          bm = BM_CONTRA;
          break;
      }
      if (bm != BM_CONTRA) {
        if (perm_->setBit(bm, bit_pos) == false)
          break;
        ++bit_pos;
      }
    }
  }
    if (root->FirstChildElement("active") != nullptr) {
      std::string characteristic { root->FirstChildElement("active")->Attribute(
          "value") };
      bool active;
      int bit_pos = -1;
      for (auto& val : characteristic) {
        switch (val) {
          case '0':
            active = false;
            ++bit_pos;
            break;
          case '1':
            active = true;
            ++bit_pos;
            break;
          default:
            active = false;
            break;
        }
        if (active == true) {
          if (perm_->setBox(active, bit_pos) == false)
            break;
        }
      }
//    perm_->print(std::cout);
//    std::cout << characteristic << std::endl;
  }
    std::vector<SboxPos> active, inactive;
    perm_->SboxStatus(active, inactive);
    for(auto& box : active){
      std::cout << "(" << (int) box.layer_ << ", " << (int) box.pos_ << ") ";
    }
    std::cout << std::endl;

  if (root->FirstChildElement("search")->FirstChildElement("phase")
      != nullptr) {

    credits_ =
        root->FirstChildElement("search")->UnsignedAttribute("credits") ?
            root->FirstChildElement("search")->UnsignedAttribute("credits") : 1000;

    std::cout << "Credits:" << credits_ << std::endl;

    print_active_ =
            root->FirstChildElement("search")->BoolAttribute("print_active") ?
                root->FirstChildElement("search")->BoolAttribute("print_active") : false;
    std::cout << "print_active:" << print_active_ << std::endl;

    tinyxml2::XMLElement* phase = root->FirstChildElement("search")
        ->FirstChildElement("phase");
    tinyxml2::XMLElement* setting = phase->FirstChildElement("setting");
    while (setting != nullptr) {
      Setting set;
      set.guess_weights_.resize(rounds);
      for (auto& entry : set.guess_weights_)
        entry[0] = entry[1] = 0;

      set.push_stack_probability_ = setting->FloatAttribute("push_stack") ?
          setting->FloatAttribute("push_stack") : 0.05;
      std::cout << "Setting pushstack prob: " << set.push_stack_probability_ << std::endl;


      tinyxml2::XMLElement* guess = setting->FirstChildElement("guess");

      while (guess != nullptr) {
        assert(guess->IntAttribute("sbox_layer") < rounds);
        set.guess_weights_[guess->IntAttribute("sbox_layer")][0] = guess->FloatAttribute(
            "inactive_weight");
        set.guess_weights_[guess->IntAttribute("sbox_layer")][1] = guess->FloatAttribute(
            "active_weight");
        guess = guess->NextSiblingElement("guess");
      }
      settings_.push_back(set);
      setting = setting->NextSiblingElement("setting");
    }
  }

  for(auto& set : settings_){
    for(auto& entry : set.guess_weights_){
      std::cout << "(" << entry[0] << ", " << entry[1] << "), ";
    }
    std::cout << std::endl;
  }

  return true;
}
Permutation* Configparser::getPermutation() {
  assert(perm_.get() != nullptr);
  return perm_->clone();
}
Settings Configparser::getSettings() {
  return settings_;
}

unsigned int Configparser::getCredits() {
  return credits_;
}

bool Configparser::Error(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  printf("invalid config file - ");
  vprintf(format, argptr);
  printf("\n");
  va_end(argptr);
  return false;
}

bool Configparser::Warning(const char *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  printf("config file - warning: ");
  vprintf(format, argptr);
  printf("\n");
  va_end(argptr);
  return false;
}

bool Configparser::printActive(){
  return print_active_;
}
