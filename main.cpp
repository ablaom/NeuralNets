// copyright Anthony D. Blaom, 2012
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <vector>
#include <ctime>
#include <algorithm>
// #include "instant_input.h"
#include "consoleinput.h"
#include "neuralnet.h"

using namespace std;

/* function to read the layer architecture from file or console */
void get_architecture(vector<int>& layer_vec, vector<char>& layer_activation_vec);

/* function to view the layer architecture on the console */
void output_architecture_to_console(const vector<int>& layer_vec, 
				    const vector<char>& layer_activation_vec);

/* function to read in pattern data from <filename>. Format as in:
   0.549061479464299 -0.324085197138906 0.128441385584874
   0.843946784956205 0.399941611710293 2.88684416665979
   0.0932513889397143 0.427451887967431 1.47468703195263x...  */
vector<Pattern> read_in_pattern_data(string pattern_file, int num_inputs, int num_outputs);


/* the following function is the main function (for which main() is
   just a front end). It does the following:

   1. builds a layered neural network with an architecture specified in "architecture.ssv" 
   2. randomly resets the edge weights if <weight_to_be_reset> is true
   3. otherwise reads in weights from "weights.ssv"
   3. performs <num_eons> stochastic updates to the network using the patterns in "training.ssv"
   4. writes the final weights to "weights.ssv"
   5. writes the mean error over all patterns in "validation.ssv" for the updated network to "error.ssv" 

   This function maintains two data files:

   "weights.ssv" - this file is overwritten with the final weight values
   "weights_cummulative.ssv" - this file is APPENDED to if
   <weights_to_be_reset> is false, and overwritten otherwise. */

void update(int num_eons, bool weights_to_be_reset, double eta, double lambda);

int main(int argc, char** argv) {

  // process arguments of main 
  if (argc != 5) {
    cout << "nn needs 4 arguments: num_eons weights_to_be_reset eta lambda." << endl;
    return 1;
  }
  stringstream ss;
  int num_eons;
  bool weights_to_be_reset;
  double eta;
  double lambda;
  for (int i = 1; i != 5; ++i) { 
    ss << argv[i] << " ";
  }
  ss >> num_eons >> weights_to_be_reset >> eta >> lambda;

  // call <update> function on arguments of <main>
  update(num_eons, weights_to_be_reset, eta, lambda);

  return 0;
}

void update(int num_eons, bool weights_to_be_reset, double eta, double lambda){

  // plant seed for random number generator (used in rnorm() in "neuralnet.cpp")
  srand(time(NULL));

  // Get the architecture from file "architecture.ssv"
  vector<int> layer_vec;
  vector<char> layer_activation_vec;
  get_architecture(layer_vec, layer_activation_vec);
  /* Note that layer_vec[i] is the number of nodes in the ith layer,
     EXCLUDING the bias node (which is in every layer) */
  //  output_architecture_to_console(layer_vec, layer_activation_vec);
  int num_inputs = layer_vec[0];
  int num_outputs = layer_vec[layer_vec.size()-1];

  // cout << num_inputs << " " << num_outputs;

  // Construct a layered neural network based on the architecture
  // above
  // 1. construct a double vector of nodes, <node_array>
  vector< vector<Node> > node_array;
  for (vector<int>::size_type i = 0; i != layer_vec.size(); ++i) {
    Node bias('c'), proto_node(layer_activation_vec[i]);
    vector<Node> this_vector; /* a temporary vector to hold the nodes
				 for current (ith) layer */
    this_vector.push_back(bias); 
    /* Note: a bias is created even for the output layer but no edge
       is ever connected to it and so it is "dead". This is necessary
       to keep the code in "3" below uncomplicated. */

    for (int j = 0; j != layer_vec[i]; ++j) {
      this_vector.push_back(proto_node);
    }
    node_array.push_back(this_vector);
  }

  // 2. construct the list of pointers to the input nodes
  list<Node *> input_list;
  for (vector<Node>::iterator it = 
	 node_array[0].begin() + 1; it != node_array[0].end(); ++it) {
    input_list.push_back(&(*it));
  }

  // 2'. construct the list of pointers to the output nodes
  list<Node *> output_list;
  vector<int>::size_type L = layer_vec.size()-1;
  for (vector<Node>::iterator it = 
	 node_array[L].begin() + 1; it != node_array[L].end(); ++it) {
    output_list.push_back(&(*it));
  }

  // 3. construct a list of edges with a layer-consistent ordering
  list<Edge> edge_list;
  /* in the three nested loops below, l indexes the layer, i a node
     within the lth layer (the 'left' node), and j a node within the
     (l+1)th layer (the 'right' node). */
  for (vector< vector<Node> >::iterator l 
	 = node_array.begin(); l != node_array.end() - 1; ++l) {
    vector<Node>::iterator i, j;
    for (i = l->begin(); i != l->end(); ++i) {
      for (j = (l + 1)->begin() + 1; j!=  (l + 1)->end(); ++j) {
	// there's no edge TO a bias; hence "(l + 1)->.begin() + 1"
	Edge this_edge(&(*i), &(*j), 0);
	edge_list.push_back(this_edge);
      }
    }
  }

  // 4. construct the neural network object <nn> based on above data
  Neuralnet nn(input_list, output_list,  edge_list);
  //  nn.test();

  // Read in old weights from file or reset to random values
  // according to <weights_to_be_reset> parameter flag
  if (weights_to_be_reset) {
    nn.initialize_weights();
  } else {
    ifstream infile("weights.ssv");
    nn.input_weights_from_file(infile);
    infile.close();
  }

  // Read in the training data and scramble
  vector<Pattern> pattern_data = read_in_pattern_data("training.ssv", num_inputs, num_outputs);
  random_shuffle(pattern_data.begin(), pattern_data.end());

  // Read in the validation data
  vector<Pattern> validation_data = read_in_pattern_data("validation.ssv", num_inputs, num_outputs);

  // Do stochastic updates for <num_eons> eons
  //  nn.output_weights_to_console();
  vector<Pattern>::const_iterator pit;
  for (int i = 0; i < num_eons; ++i) {
    for (pit = pattern_data.begin(); pit != pattern_data.end(); ++pit) {
      nn.stochastic_update(*pit, eta, lambda);
      //      nn.output_weights_to_console();
    }
    //    nn.output_weights_to_console();
  }
  //  cout << endl;

  // Write the weights to file
  {
    ofstream outfile("weights.ssv");
    nn.output_weights_to_file(outfile);
    outfile.close();
  }

  // Compute the error and write to file
  ofstream outfile;
  outfile.open("error.ssv"); 
  double error = 0.0;
  int N = 0;  
  for (pit = validation_data.begin(); pit != validation_data.end(); ++pit) {
    error = error + nn.compute_error(*pit);
    ++N;
  }
  error = error/N;
  outfile << error << " ";
  outfile.close();
}

// end of update 

void get_architecture(vector<int>& layer_vec, vector<char>& layer_activation_vec) {
  // 1. read in the data in two strings <s> and <s2>
  char action = 'j';
  string s;
  string s2;
      ifstream infile("architecture.ssv");
      getline(infile, s);
      getline(infile, s2);
      infile.close();

  // 2. build <layer_vec> (vector of number of nodes in each layer) from string <s>
  stringstream ss;
  ss << s;
  int input;
  while (ss >> input) {
    layer_vec.push_back(input);
  }
  // 3. build <layer_activation_vec> (vector of activation functions in each layer) from <s2>
  stringstream ss2;
  ss2 << s2;
  char input2;
  while (ss2 >> input2) {
    layer_activation_vec.push_back(input2);
  }
  if (layer_activation_vec[0] != 'i') {
    cerr << "Warning: Input nodes have non-identity activation functions.";
  }
}



void output_architecture_to_console(const vector<int>& layer_vec, 
				      const vector<char>& layer_activation_vec)  {
    vector<int>::const_iterator it;
    vector<char>::const_iterator it2;
    int i = 0;
    cout << endl << endl;
    for (it = layer_vec.begin(); it != layer_vec.end(); ++it) {
      cout << "Layer " << i << " has " << *it << " nodes plus a bias." << endl;
      ++i;
    }
    i = 0;
    for (it2 = layer_activation_vec.begin(); it2 != layer_activation_vec.end(); ++it2) {
      cout << "Layer " << i << " has activation type \'" << *it2 << "\'" << endl;
      ++i;
    }
  }

vector<Pattern> read_in_pattern_data(string filename, int num_inputs, int num_outputs) {
  vector<Pattern> ret;
  ifstream infile(filename.c_str());
  string s;

  //  cout << num_inputs << " " << num_outputs;

  int num_patterns = 0;
  double r;
  while(getline(infile, s)) {
    bool end_of_file = 0;
    Pattern pattern;
    stringstream ss(s);
    for (int i = 0; i != num_inputs; ++i) {
      if (ss >> r) {
	(pattern.x).push_back(r);
      } else {
	end_of_file = 1;
      }
    }
    for (int i = 0; i != num_outputs; ++i) {
      if (ss >> r) {
	(pattern.y).push_back(r);
      } else {
	end_of_file = 1;
      }
    }
    if (!end_of_file) {
      ret.push_back(pattern);
      ++num_patterns;
      //    cout << endl << "Pattern number " << num_patterns << " is " << endl;
      //    pattern.print_to_console();
    }
  }
  infile.close();
  return ret;
}
