/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include "configparser.h"

Configparser::Configparser()
    : credits_(1000),
      print_active_(false) {
  settings_.clear();
}

bool Configparser::parseFile(std::string filename) {
  settings_.clear();
  tinyxml2::XMLDocument doc;
  doc.LoadFile(filename.data());

  if (doc.ErrorID() != 0) {
    doc.PrintError();
    return Error( { "Problem with ", filename.data(), ". "});
  }

  doc.Print();

  tinyxml2::XMLElement* root =
      doc.FirstChildElement("config") ?
          doc.FirstChildElement("config") : nullptr;
  if (root == nullptr)
    return Error( { "has to start with <config>" });

  int rounds = 3;



  if (root->FirstChildElement("parameters") != nullptr) {
    tinyxml2::XMLElement* parameters = root->FirstChildElement("parameters");
    if (parameters->FirstChildElement("rounds") != nullptr)
      rounds = parameters->FirstChildElement("rounds")->IntAttribute("value");
    else
      rounds = 1;

    std::string instance { parameters->FirstChildElement("permutation")
        ->Attribute("value") };

    perm_.reset(permutation_list(instance, rounds));
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
//  for (auto& box : active) {
//    std::cout << "(" << (int) box.layer_ << ", " << (int) box.pos_ << ") ";
//  }
//  std::cout << std::endl;

  if (root->FirstChildElement("search")->FirstChildElement("phase")
      != nullptr) {

    credits_ =
        root->FirstChildElement("search")->UnsignedAttribute("credits") ?
            root->FirstChildElement("search")->UnsignedAttribute("credits") :
            1000;

//    std::cout << "Credits:" << credits_ << std::endl;

    print_active_ =
        root->FirstChildElement("search")->BoolAttribute("print_active") ?
            root->FirstChildElement("search")->BoolAttribute("print_active") :
            false;
//    std::cout << "print_active:" << print_active_ << std::endl;

    tinyxml2::XMLElement* phase = root->FirstChildElement("search")
        ->FirstChildElement("phase");
    tinyxml2::XMLElement* setting = phase->FirstChildElement("setting");
    while (setting != nullptr) {
      Setting set;
      set.guess_weights_.resize(rounds);
      for (auto& entry : set.guess_weights_)
        entry[0] = entry[1] = 0;

      set.push_stack_probability_ =
          setting->FloatAttribute("push_stack") ?
              setting->FloatAttribute("push_stack") : 0.05;
//      std::cout << "Setting pushstack prob: " << set.push_stack_probability_
//                << std::endl;

      set.alternative_sbox_guesses_ =
          setting->UnsignedAttribute("alternative_sbox_guesses") ?
              setting->UnsignedAttribute("alternative_sbox_guesses") : 3;
//      std::cout << "Setting alternative sboxguesses: "
//                << set.alternative_sbox_guesses_ << std::endl;

      set.sbox_weight_hamming_ =
          setting->UnsignedAttribute("sbox_weight_hamming") ?
              setting->UnsignedAttribute("sbox_weight_hamming") : 1;
//      std::cout
//          << "Setting weight for sbox selection considering hammingweight: "
//          << set.sbox_weight_hamming_ << std::endl;

      set.sbox_weight_probability_ =
          setting->UnsignedAttribute("sbox_weight_probability") ?
              setting->UnsignedAttribute("sbox_weight_probability") : 2;
//      std::cout << "Setting weight for sbox selection considering ldt entries: "
//                << set.sbox_weight_probability_ << std::endl;

      tinyxml2::XMLElement* guess = setting->FirstChildElement("guess");

      while (guess != nullptr) {
        assert(guess->IntAttribute("sbox_layer") < rounds);
        set.guess_weights_[guess->IntAttribute("sbox_layer")][0] = guess
            ->FloatAttribute("inactive_weight");
        set.guess_weights_[guess->IntAttribute("sbox_layer")][1] = guess
            ->FloatAttribute("active_weight");
        guess = guess->NextSiblingElement("guess");
      }
      settings_.push_back(set);
      setting = setting->NextSiblingElement("setting");
    }
  }

//  for (auto& set : settings_) {
//    for (auto& entry : set.guess_weights_) {
//      std::cout << "(" << entry[0] << ", " << entry[1] << "), ";
//    }
//    std::cout << std::endl;
//  }

  return true;
}
std::unique_ptr<Permutation> Configparser::getPermutation() {
  assert(perm_.get() != nullptr);
  return perm_->clone();
}
Settings Configparser::getSettings() {
  return settings_;
}

unsigned int Configparser::getCredits() {
  return credits_;
}

bool Configparser::Error(std::initializer_list<std::string> msg) {
  std::cerr << "Config Error: ";
  for (const auto& m : msg)
    std::cerr << m;
  std::cerr << std::endl;
  return false;
}

bool Configparser::Warning(std::initializer_list<std::string> msg) {
  std::cerr << "Config Warning: ";
  for (const auto& m : msg)
    std::cerr << m;
  std::cerr << std::endl;
  return false;
}

bool Configparser::printActive() {
  return print_active_;
}
