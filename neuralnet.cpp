// copyright Anthony D. Blaom, 2012

#include <cmath>
#include <cstdlib> // needed for RAND_MAX definition
#include <iostream>
#include "neuralnet.h"



using namespace std;

// auxilliary functions

/* The following generates a pseudorandom number according to an
			   approximately normal distribution with zero
			   mean and unit standard deviation. This it
			   does by sampling a uniform distribution N
			   times, adding the results and dividing by
			   sqrt(N).*/
double rnorm() {
  static int N = 100; // sample size
  static double a = sqrt(3.0); /* then a uniform distribution on [-a, a] has
			 unit standard deviation */
  double sum = 0;
  double X;
    for (int i = 0; i != N; ++i){
      X = (2.0*rand() - RAND_MAX)/RAND_MAX; // X is in [-1,1]
      sum = sum + a*X;
    }
    return sum/sqrt(N);
}

// Node public members

void::Node::update_output() {
  switch(type_of_activation) {
  case 'h' : // hyperbolic tangent
    output = tanh(signal);               
    break;
  case 'l' : // logistic function
    output = 1.0 / (1.0 + exp(-signal)); 
    break;
  case 'i' : // identity
    output = signal;
    break;
  case 'c' : // constant
    output = 1.0;
    break;
  default:
    output = tanh(signal);
    break;
  }
}

void::Node::update_delta() {
  double derivative;
  switch(type_of_activation) {
  case 'h' : // hyperbolic tangent
    derivative = pow(1.0/cosh(signal), 2);
    break;
  case 'l' : // logistic function
    derivative = 1.0 / pow(1.0 + exp(-signal), 2); 
    break;
  case 'i' : // identity
    derivative = 1.0;
    break;
  case 'c' : // constant
    derivative = 0;
    break;
  default:
    derivative = pow(1.0/cosh(signal), 2);
    break;
  }
  delta = error*derivative;
}

Node::Node() {
  output = 0;
  delta = 0;
  signal = 0;
  error = 0;
  type_of_activation = 'h';
  num_input_edges = 0;
}

Node::Node(char t) {
  if (t == 'c') {
    output = 1;
  } else {
  output = 0;
  }
  delta = 0;
  signal = 0;
  error = 0;
  type_of_activation = t;
  num_input_edges = 0;
}

void Node::dump() {
  cout << " type=" << type_of_activation << " signal=" << signal << " delta=" << delta << " output=" << output << " error=" << error;
} 
// Edge public members 

void::Edge::initialize_weight() {
  weight = weight_sigma*rnorm(); 
}

void::Edge::update_weight(const double eta, const double lambda) {
  static double W = 1.0; 
  double left_output = (this->left_node)->get_output();
  double right_delta = (this->right_node)->get_delta();
  //   cout << endl << " leftout=" << left_output << " rightdelta=" <<
   right_delta;
  double q = weight/W;
  weight =  weight - 
    eta*(
	 left_output*right_delta /* gradient descent correction */
	 + (2*lambda/W) * q/(1 + pow(q, 2)) /* regularization penalty */
		   );
}

void::Edge::set_weight_sigma(){
  static double C = 1.47;
  int n = (this->right_node)->get_num_input_edges();
  weight_sigma = C/sqrt(n);
}

Edge::Edge(Node *l, Node *r, bool d) { 
  weight = 0;
  weight_sigma = 0.5; 
  this->left_node = l;
  this->right_node = r;
  this->last_forward = true;
  this->last_backward = true;
  this->lazy = false;
  this->dead = d;
}

// Pattern public members

// constructor
Pattern::Pattern() {
  std::list<double>::iterator it;
  for (it = (this->x).begin(); it != (this->x).end(); ++it) {
    *it = 0;
  }
  for (it = (this->y).begin(); it != (this->y).end(); ++it) {
    *it = 0;
  }
}

void Pattern::print_to_console() {
  std::list<double>::const_iterator it;
  cout << endl;
  cout << "Inputs: ";
  for (it = (this->x).begin(); it != (this->x).end(); ++it) {
    cout << *it << " ";
  }
  cout << "Outputs: ";
  for (it = (this->y).begin(); it != (this->y).end(); ++it) {
    cout << *it << " ";
  }
}

// Neuralnet public members

void Neuralnet::initialize_weights() {
  list<Edge>::iterator it;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    it->initialize_weight();
  }
}

void Neuralnet::input_weights_from_file(ifstream& infile){
  list<Edge>::iterator it;
  double w;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    infile >> w;
    it->set_weight(w);
  }
}

void Neuralnet::output_weights_to_file(ofstream& outfile){
  list<Edge>::iterator it;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    outfile << (it->get_weight()) << " ";
  }
  outfile << endl;
}

void Neuralnet::stochastic_update(const Pattern pattern, const double eta, const double lambda) {
  // 1. reset all the node signals and all node errors to zero
  // (except the input node signals and the output node errors)
  list<Edge>::iterator it;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    // currently not efficient but thorough:
    // (it->left_node)->reset_error();
    // (it->right_node)->reset_error();
    // (it->left_node)->set_signal(0);
    // (it->right_node)->set_signal(0);

    if (it->last_backward) {
      (it->left_node)->reset_error();
    }
    if (it->last_forward) {
      (it->right_node)->set_signal(0);
    }
  }

  // 2. load the pattern into the input nodes and apply the activation
  // function (which ought to be the identity)
  list<Node *>::const_iterator node_it = input_list.begin();
  list<double>::const_iterator double_it;
  for (double_it = (pattern.x).begin(); double_it != (pattern.x).end(); ++double_it) {
    (*node_it)->set_signal(*double_it);
    (*node_it)->update_output();
    ++node_it;
  }

  // 3. compute the outputs in a forward pass
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    (it->right_node)->add_to_signal( ((it->left_node)->get_output())*(it->get_weight()) );
    if (it->last_forward) {
      (it->right_node)->update_output();
      //      cout << "Output at node " << (it->right_node) << " is " << (it->right_node)->get_output() << endl;
    }
  }

  // 4. evaluate the "error" to be fed into the ouput nodes for
  // backpropagation and evaluate the delta's on the left of the
  // output nodes (which depend on these errors).
  /* to evaluate the output "error" we are assuming an error function
     E(z,y) = frac{1}{2}\sum_j (z_j - y_j)^2 and the index j runs over
     all outputs (not over patterns - we are doing stochastic
     update!). Here z_j is the network-predicted output, based on the
     pattern inputs; and y_j is the pattern output value */
  node_it = output_list.begin();
  for (double_it = (pattern.y).begin(); double_it != (pattern.y).end(); ++double_it) {
    double zj = (*node_it)->get_output();
    double yj =  *double_it;
    double e =  zj - yj; /* The derivative of the error function
			  E(z,y) w.r.t z_j */
    (*node_it)->reset_error(); /* output node errors were not reset in
				  step 1 above */
    (*node_it)->add_to_error(e);
    (*node_it)->update_delta();
    //    cout << "Error at node " << (*node_it) << " is " << e << endl;
    //    cout << "Delta at node " << (*node_it) << " is " << (*node_it)->get_delta() << endl;
    ++node_it;
  }

  // 5. compute all deltas and errors in a backward pass, except at
  // left nodes of lazy edges, and simultaneously update weights at an
  // edge if it is not dead. (See the documentation of Neuralnet in
  // neuralnet.h.)
  /* These weight updates depend only on the right delta and left
     output, and the current weight if regularization term is
     included */
  /* Note for future improvement: i am computing errors and deltas for the input
     nodes, which are never needed!! */
  list<Edge>::reverse_iterator rit;
  for (rit = edge_list.rbegin(); rit != edge_list.rend(); ++rit) {
    if (!(rit->lazy)) {
      (rit->left_node)->add_to_error( ((rit->right_node)->get_delta())*(rit->get_weight()) );
      if (rit->last_backward) {
	(rit->left_node)->update_delta();
	//    cout << "Delta @ Node" <<  (rit->left_node) << " is " << (rit->left_node)->get_delta() << endl;
      }
    }
    if (!rit->dead) {
      rit->update_weight(eta, lambda);
    }
  }
}

void Neuralnet::output_weights_to_console() {
  cout << endl;
  list<Edge>::const_iterator it;
  int i = 0;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    cout << endl << "weight number " << i << ": "<< "weight=" << it->get_weight();
    // cout << endl << "Left node=" << it->left_node << ": ";
    // (it->left_node)->dump();
    // cout << endl << "Right node=" << it->right_node << ": ";
    // (it->right_node)->dump();
    ++i;
  }
}

double Neuralnet::compute_error(const Pattern& pattern) {
  // 1. reset all the node signals to zero
  // (except the input node signals)
  list<Edge>::iterator it;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    if (it->last_forward) {
      (it->right_node)->set_signal(0);
    }
  }

  // 2. load the pattern into the input nodes and apply the activation
  // function (understood to be the identity)
  list<Node *>::const_iterator node_it = input_list.begin();
  list<double>::const_iterator double_it;
  for (double_it = (pattern.x).begin(); double_it != (pattern.x).end(); ++double_it) {
    (*node_it)->set_signal(*double_it);
    (*node_it)->update_output();
    ++node_it;
  }

  // 3. compute all node outputs in a forward pass
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    (it->right_node)->add_to_signal( ((it->left_node)->get_output())*(it->get_weight()) );
    if (it->last_forward) {
      (it->right_node)->update_output();
    }
  }

  // 4. evaluate the error
  double error = 0.0;
  int n = 0;
  node_it = output_list.begin();
  for (double_it = (pattern.y).begin(); double_it != (pattern.y).end(); ++double_it) {
    double zj = (*node_it)->get_output();
    double yj =  *double_it;
    error =  error + pow(zj - yj, 2);
    ++node_it;
    ++n;
  }
  error = sqrt(error/n);
  return error;
}

void Neuralnet::test() {
  cout << endl;
  int i = 0;
  list<Edge>::const_iterator it;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    cout << "edge number = " << i << endl;
    cout << "from " << it->left_node << " to " << it->right_node << endl;
    cout << "is last edge into a node on forward pass? " 
	 << it->last_forward << endl;
    cout << "is last edge into a node on backward pass? " 
	 << it->last_backward << endl;
    cout << "number of edges into the right node = " 
	 << (it->right_node)->get_num_input_edges() << endl;
    ++i;
  }
}

// constructor:
 Neuralnet::Neuralnet(list<Node *> inputs, list<Node *> outputs, list<Edge> ed) {
  input_list = inputs;
  output_list = outputs;
  edge_list = ed;

  // initialize the <last_forward>, <last_backward> flags (for edges)
  // and the num_input_edges (for nodes), and the <lazy> flag for
  // edges 1. set all the flags to true and num_input_edges to zero
  list<Edge>::iterator it, jt;
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    it->last_forward = true;
    it->last_forward = true;
    (it->right_node)->reset_num_input_edges();
  }
  // 2. in a forward pass set all the appropriate last_forward flags to false.
  //    Simultaneously set (incrementally) num_input_edges to correct value,
  //    for each node appearing as a right_node (ie, non-input nodes) 
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    (it->right_node)->increment_num_input_edges();  
    for (jt = edge_list.begin(); jt != it; ++jt) {
      if (jt->right_node == it->right_node) {
	jt->last_forward = false;
      }
    }
  }
  // 3. in a backward pass set all appropriate last_backward flags to false: 
  list<Edge>::reverse_iterator rit, rjt;
  for (rit = edge_list.rbegin(); rit != edge_list.rend(); ++rit) { 
    for (rjt = edge_list.rbegin(); rjt != rit; ++rjt) {
      if (rjt->left_node == rit->left_node) {
	rjt->last_backward = false;
      }
    }
  }

  // 4. set the lazy flags
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    if ((it->left_node)->get_num_input_edges() == 0) {
      it->lazy == true;
    }
  }

  // initialize the weight_sigmas
  for (it = edge_list.begin(); it != edge_list.end(); ++it) {
    it->set_weight_sigma();
  }

 }





