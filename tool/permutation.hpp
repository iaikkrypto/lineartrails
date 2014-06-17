template <unsigned rounds>
void Permutation<rounds>::SboxStatus(std::vector<SboxPos>& active,
                                  std::vector<SboxPos>& inactive) {
  active.clear();
  inactive.clear();

  for (size_t layer = 0; layer < rounds; ++layer)
    //FIXME: get rid of 64
    for (int pos = 0; pos < 64; ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active.emplace_back(layer, pos);
        else
          inactive.emplace_back(layer, pos);
      }
}
template <unsigned rounds>
void Permutation<rounds>::SboxStatus(std::vector<std::vector<SboxPos>>& active, std::vector<std::vector<SboxPos>>& inactive){
  active.clear();
  inactive.clear();
  active.resize(rounds);
  inactive.resize(rounds);

  for (size_t layer = 0; layer < this->sbox_layers_.size(); ++layer)
    //FIXME: get rid of 64
    for (int pos = 0; pos < 64; ++pos)
      if (this->sbox_layers_[layer]->SboxGuessable(pos)) {
        if (this->sbox_layers_[layer]->SboxActive(pos))
          active[layer].emplace_back(layer, pos);
        else
          inactive[layer].emplace_back(layer, pos);
      }
}
