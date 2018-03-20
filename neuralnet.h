#ifndef GUARD_neuralnet
#define GUARD_neuralnet

// header file for neural network objects
// copyright Anthony D. Blaom, 2012

// Please read the comments preceding the definition of Neuralnet
// class for documentation.

#include <list>
#include <vector>
#include <fstream>

double rnorm(); /* Generates a pseudorandom number according to an
			   approximately normal distribution with zero
			   mean and unit standard deviation */

class Node {
private:
  double output, delta;
  double signal, error;
  //  double derivative; now a local variable in update_delta()
  char type_of_activation;
  int num_input_edges;
public:
  // output:
  void update_output(); /* output is activation function applied to signal */
  double get_output() const {return output;} 
  // delta:
  void update_delta(); /* evaluates activation derivative at current signal value
			  and multiplies this by error to get delta */
  double get_delta() const {return delta;}
  // signal:
  void set_signal(double a) {signal = a;} 
  void add_to_signal(double a) {signal = signal + a;}
  // error: 
  void reset_error() {error = 0;}
  void add_to_error(double a) {error = error + a;}
  // type_of_activation:
  void set_type_of_activation(char t) {type_of_activation = t;}
  // num_input_edges:
  void reset_num_input_edges() {num_input_edges = 0;}
  void increment_num_input_edges() {++num_input_edges;}
  int get_num_input_edges() const {return num_input_edges;}
  // testing:
  void dump();
  // constructors:
  Node();
  Node(char t);
};

class Edge {
 private:
  double weight;
  double weight_sigma;   /* this is the standard deviation for the
			      randomly generated (and normally
			      distributed) initial weight value, set
			      by a call to <initialize_weight>.*/
 public:
  // weight:
  void initialize_weight();
  void set_weight(double w) {weight = w;}
  void update_weight(const double eta, const double lambda);   
     /* eta is the learning rate, lamda the regularizing parameter */
  double get_weight() const {return weight;}
  // weight_sigma:
  void set_weight_sigma();
  // variables:
  Node *left_node;
  Node *right_node;
  bool last_forward; /* initialized to true in Neuralnet constructor
			if this is the last edge into *right_node on
			forward pass through edge_list */ 
  bool last_backward; /* initialized to true in Neuralnet constructor
			 if this is the last edge into *left_node on
			 backward pass through edge_list */ 
  bool lazy; /* true for edges that need not do work in the backward
		pass of a stochastic update because their left nodes
		have no incoming edges */
  bool dead; /* true if the weight is not to be changed in stochastic
		updates */
  // constructors:
  Edge(Node *l, Node *r, bool d);
};

class Pattern {
public:
  std::list<double> x;
  std::list<double> y;
  void print_to_console();
  // constructors:
  Pattern();
  Pattern(std::list<double> xx, std::list<double> yy) {this->x = xx; this->y = yy;}
};

/* a Neuralnet object contains no Nodes - only POINTERS to nodes: a
     list of pointers to the input nodes, called <input_list>; a list
     of pointers to the output nodes, called <output_list>; and a list
     of edges, named <edge_list>, whose elements point to their
     respective left and right nodes. (These lists are private members
     of the Neuralnet object.) Thus all nodes reside OUTSIDE the
     Neuralnet object. To create a Neuralnet object one must:

     1. Create nodes for the network of the appropriate activation
     type, individually, or in any convenient container (whose details
     are irrelevant after construction of the Neuralnet object because
     all access will be through pointers stored in the object).

     2. Create two lists of pointers of type Node that will point to the
     input nodes and output nodes respectively, say <inputs> and <outputs>.

     3. Create a list of Edges (with a valid forward pass ordering)
     which point to the appropriate nodes, say <ed>, through the Edge
     member variables <left_node> and <right_node>.

     4. Create the Neuralnet object by calling the Neuralnet
     constructor with, eg, 

     Neuralnet nn(inputs, outputs, ed);

     Copies of the lists <inputs>, <outputs> and <ed> become private
     data in the Neuralnet object nn (for purposes of data
     protection), called <input_list>, <output_list> and <edge_list>
     resp., which can be modified only by calling Neuralnet member
     functions. The Neuralnet constructor additionally passes forwards
     and backwards through the newly created network object, informing
     each node how many edges are fanning into it (through their
     private member <num_input_edges>), and informing each edge
     whether or not it is last into or first out of a node (through
     the private member variables <last_forward> and
     <last_backward>). The first bit of information is needed for to
     set the weight sigmas (standard deviations) needed for weight
     initializations, a random assignment (member function
     <initialize_weights>); the second so that edges know when the
     activation function can be applied to the node inputs during the
     forward pass, and when the derivative can be applied to the node
     errors in the backward pass ie, when the relevant incremental
     summations are finished (when member function
     <stochastic_update()> is applied). The final job of the
     constructor is to set all the weight sigmas. 

     Note that the lists <in>, <out> and <ed> themselves play no
     further role as calls to these member functions do not affect
     them. The nodes themselves are global objects and so not
     protected.

     Added September 2012: I have added the concept of a "dead weight"
     which is never updated during stochastic updates. (This was so
     normalization of inputs and outputs can be implemented by adding
     a layer of nodes at the beginning and end of the network; see
     main.cpp.) A weight is dead if its Edge has its public member
     <dead> equal to true. We have also added the Edge public member
     <lazy> which the Neuralnet constructor sets to true if its left
     node has no input edge. In that case there is no need for the
     Edge to increment the error in its left node - or to update the
     delta at its left node - during the backward propogation step of
     stochastic updates. */
class Neuralnet { 
 private: 
  std::list<Node *> input_list; /* list of pointers to the input
				   nodes */

  std::list<Node *> output_list; /* list of pointers to the output
				   nodes */
  std::list<Edge> edge_list;  /* list of edges, each of these pointing
				 to left and right nodes */
 public:
  void initialize_weights();
  void input_weights_from_file(std::ifstream& infile);
  void output_weights_to_file(std::ofstream& outfile);
  void stochastic_update(const Pattern pattern, const double eta, const double lambda);
  void output_weights_to_console();
  /* the following member's parameter holds both the member's input
     (as pattern.x) and output (as pattern.y) */
  double compute_error(const Pattern& pattern);
  void test();
  // constructor:
  Neuralnet(std::list<Node *> inputs, std::list<Node *> outputs, std::list<Edge> ed);
};


#endif
