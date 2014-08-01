#include "configparser.h"

bool Configparser::parseFile(std::string filename) {
  weights_.clear();
  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename.data());

  assert(doc.ErrorID() == 0);

  tinyxml2::XMLElement* root =
      doc.FirstChildElement("config") ?
          doc.FirstChildElement("config") : nullptr;
  if (root == nullptr)
    return Error("has to start with <config>");

  int rounds = 3;

  if (root->FirstChildElement("parameters") != nullptr) {
    tinyxml2::XMLElement* parameters = root->FirstChildElement("parameters");
    if (parameters->FirstChildElement("rounds") != nullptr)
      rounds = parameters->FirstChildElement("rounds")->IntAttribute("value");
    else
      rounds = 1;

//    std::cout << rounds << std::endl;
    //FIXME: NO
    switch (rounds) {
      case 1:
        perm_.reset(new AsconPermutation<1>);
        break;
      case 2:
        perm_.reset(new AsconPermutation<2>);
        break;
      case 3:
        perm_.reset(new AsconPermutation<3>);
        break;
      case 4:
        perm_.reset(new AsconPermutation<4>);
        break;
      case 5:
        perm_.reset(new AsconPermutation<5>);
        break;
      case 6:
        perm_.reset(new AsconPermutation<6>);
        break;
      default:
        perm_.reset(new AsconPermutation<3>);
        break;
    }

  } else
    perm_.reset(new AsconPermutation<3>);

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
//    perm_->print(std::cout);
//    std::cout << characteristic << std::endl;
  }

  if (root->FirstChildElement("search")->FirstChildElement("phase")
      != nullptr) {

    credits_ =
        root->FirstChildElement("search")->IntAttribute("credits") ?
            root->FirstChildElement("search")->IntAttribute("credits") : 0;

    std::cout << credits_ << std::endl;

    tinyxml2::XMLElement* phase = root->FirstChildElement("search")
        ->FirstChildElement("phase");
    tinyxml2::XMLElement* setting = phase->FirstChildElement("setting");
    while (setting != nullptr) {
      std::vector<std::array<unsigned int, 2>> set;
      set.resize(rounds);
      for (auto& entry : set)
        entry[0] = entry[1] = 0;

      tinyxml2::XMLElement* guess = setting->FirstChildElement("guess");

      while (guess != nullptr) {
        assert(guess->IntAttribute("sbox_layer") < rounds);
        set[guess->IntAttribute("sbox_layer")][0] = guess->UnsignedAttribute(
            "inactive_weight");
        set[guess->IntAttribute("sbox_layer")][1] = guess->UnsignedAttribute(
            "active_weight");
        guess = guess->NextSiblingElement("guess");
      }
      weights_.push_back(set);
      setting = setting->NextSiblingElement("setting");
    }
  }

  for(auto& weight : weights_){
    for(auto& entry : weight){
      std::cout << "(" << entry[0] << ", " << entry[1] << "), ";
    }
    std::cout << std::endl;
  }

  return true;
}
PermutationBase* Configparser::getPermutation() {
  assert(perm_.get() != nullptr);
  return perm_->clone();
}
GuessWeights Configparser::getWeights() {
  return weights_;
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
