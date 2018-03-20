# NeuralNets

> Some C++ code for training feed-forward artificial
> neural-networks, in supervised machine learning. 

To install the program, clone the repository, and from within the clone directory
run "make".

Documentation is in the file [main.cpp](main.cpp) and
[neuralnet.h](neuralnet.h). A neural network is trained at the command
line with:

````
nn num_eons weights_to_be_reset eta lambda
````

Here, `num_eons` is the number of eons (training is by stochastic
descent) `weight_to_be_reset` should be `0` unless restarting training
from where previously left off, `eta` is the learning rate, and
`lambda` the regularization parameter.

A file "architecture.ssv" specifies the network architecture, the
training data must appear in "training.ssv" and the validation data in
"validation.ssv". The RMS validation error is written to "error.css"

This is a fairly bare bones implementation. Any serious user will want
to wrap the code is a more user-friendly interface than provided here.

