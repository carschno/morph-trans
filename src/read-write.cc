#include "read-write.h"

using namespace std;
using namespace cnn;
using namespace cnn::expr;

void Serialize(vector<SepMorph>& models, vector<Model*>& cnn_models,
               string& filename) {
  ofstream outfile(filename);
  if (!outfile.is_open()) {
    cerr << "File opening failed" << endl;
  }

  boost::archive::text_oarchive oa(outfile);
  unsigned morph_size = models.size();
  oa & morph_size;
  for (unsigned i = 0; i < models.size(); ++i) {
    oa & models[i];
    oa & *cnn_models[i];
  }
  outfile.close();
}

void Read(string& filename, vector<SepMorph>* models,
          vector<Model*>* cnn_models) {
  ifstream infile(filename);
  if (!infile.is_open()) {
    cerr << "File opening failed" << endl;
  }

  boost::archive::text_iarchive ia(infile);
  unsigned morph_size;
  ia & morph_size;
  for (unsigned i = 0; i < morph_size; ++i) {
    SepMorph model;
    Model *cnn_model = new Model();
   
    ia & model;
    model.InitParams(cnn_model);
    ia & *cnn_model;
    
    models->push_back(model);
    cnn_models->push_back(cnn_model);
  }
  infile.close();
}

void Serialize(string& filename, SepMorph& model, Model* cnn_model) {
  ofstream outfile(filename);
  if (!outfile.is_open()) {
    cerr << "File opening failed" << endl;
  }

  boost::archive::text_oarchive oa(outfile);
  oa & model;
  oa & *cnn_model;
  outfile.close();
}

void Read(string& filename, SepMorph* model,
          Model* cnn_model) {
  ifstream infile(filename);
  if (!infile.is_open()) {
    cerr << "File opening failed" << endl;
  }

  boost::archive::text_iarchive ia(infile);
  ia & *model;
  model->InitParams(cnn_model);
  ia & *cnn_model;

  infile.close();
}

