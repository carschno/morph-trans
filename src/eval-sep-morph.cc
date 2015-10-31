#include "cnn/nodes.h"
#include "cnn/cnn.h"
#include "cnn/rnn.h"
#include "cnn/gru.h"
#include "cnn/lstm.h"
#include "cnn/training.h"
#include "cnn/gpu-ops.h"
#include "cnn/expr.h"

#include "utils.h"
#include "sep-morph.h"
#include "decode.h"
#include "read-write.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace cnn;
using namespace cnn::expr;

int main(int argc, char** argv) {
  cnn::Initialize(argc, argv);

  string vocab_filename = argv[1];  // vocabulary of words/characters
  string morph_filename = argv[2];
  string test_filename = argv[3];
  string model_filename = argv[4];

  unordered_map<string, unsigned> char_to_id, morph_to_id;
  unordered_map<unsigned, string> id_to_char, id_to_morph;

  ReadVocab(vocab_filename, &char_to_id, &id_to_char);
  unsigned vocab_size = char_to_id.size();
  ReadVocab(morph_filename, &morph_to_id, &id_to_morph);
  unsigned morph_size = morph_to_id.size();

  vector<string> test_data;  // Read the dev file in a vector
  ReadData(test_filename, &test_data);

  Model m;
  SepMorph nn;
  Read(model_filename, &nn, &m);

  // Read the test file and output predictions for the words.
  string line;
  double correct = 0, total = 0;
  for (string& line : test_data) {
    vector<string> items = split_line(line, '|');
    vector<unsigned> input_ids, target_ids, pred_target_ids;
    input_ids.clear(); target_ids.clear(); pred_target_ids.clear();
    for (const string& ch : split_line(items[0], ' ')) {
      input_ids.push_back(char_to_id[ch]);
    }
    for (const string& ch : split_line(items[1], ' ')) {
      target_ids.push_back(char_to_id[ch]);
    }
    unsigned morph_id = morph_to_id[items[2]];
    Decode(morph_id, char_to_id, input_ids, &pred_target_ids, &nn);

    string prediction = "";
    for (unsigned i = 0; i < pred_target_ids.size(); ++i) {
      prediction += id_to_char[pred_target_ids[i]];
      if (i != pred_target_ids.size() - 1) {
        prediction += " ";
      }
    }
    if (prediction == items[1]) {
      correct += 1;     
    }
    total += 1;
  }
  cerr << "Prediction Accuracy: " << correct / total << endl;
  return 1;
}
