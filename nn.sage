num_weights = 25
num_inputs = 2
num_outputs = 1
if 'means' not in globals():
    means = range(num_inputs + num_outputs)
if 'sds' not in globals():
    sds= range(num_inputs + num_outputs)
if 'rms_sd_out' not in globals():
    rms_sd_out = [1]
if 'weights' not in globals():
   weights = range(num_weights)
if 'errors' not in globals():
   errors = []
if 'patterns' not in globals():
   patterns=[]
if 'training_patterns' not in globals():
   training_patterns=[]
if 'validation_patterns' not in globals():
   validation_patterns=[]

w=weights
e=errors
p=patterns
tp=training_patterns
vp=validation_patterns

import subprocess

def shell(acommand):
    '''
    Uses subprocess module to return the output of a shell command
    '''
    return subprocess.Popen(acommand,shell=True,stdout=subprocess.PIPE).communicate()[0]

def string_to_floats(str):
    '''
    converts a string of space separated floats into a list of floats
    '''
    ret = []
    lst = str.split()
    for i in lst:
        ret.append(float(i))
    return ret

def ssv_to_list_of_lists(filename):
    '''
    A function to convert a space separated data file <filename> into
    a list (row) of lists (cols)
    '''
    infile = open(filename)
    ret = []
    for line in infile:
        ret.append(tuple(string_to_floats(line)))
    return ret

def set_weights():
    outfile = open("weights.ssv", "w")
    for r in weights:
        outfile.write("%.15e " % r)
        print "%.15e " % r
sw = set_weights

def get_weights():
   try:
      infile = open("weights.ssv")
      l = infile.readline()
      alist = string_to_floats(l)
      infile.close()
   except IOError:
      print "No weights found in \"weights.ssv\"" 
      return weights
   del weights[:]
   weights.extend(alist)
   return weights
gw = get_weights

def f(x1, x2):
    return 3.0*x1 + 0.1*(x1-x2)^2 - 2.0*x2 + 0.01*x1^3 + 1.0
#     return w[6] + w[7]*tanh(w[0] + w[2]*x1 + w[4]*x2) + w[8]*tanh(w[1] + w[3]*x1 + w[5]*x2)
#    return w[0] + w[1]*x1 + w[2]*x2

def generate_patterns(n):
   '''
   randomly generates n patterns x1, x2, f(x1, x2) as list of lists
   and puts them into the list <patterns>, erasing previous contents
   '''
   del patterns[:]
   for i in range(n):   
       ran = [gauss(0,1) for j in range(3)]
       x1 = 1.0 + 2.0*ran[0]
       x2 = 3.0 - 4.0*ran[1]
       y = f(x1, x2) + ran[2]
       patterns.append( [x1, x2, y] )
   return point3d(patterns, size=7)
gp=generate_patterns

def normalize(p):
   q=[]
   for i in range(num_inputs):
      q.append( (p[i]-means[i])/sds[i] )
   # normalize the outputs uniformly!
   for i in range(num_inputs, num_inputs + num_outputs):
      q.append( (p[i]-means[i])/rms_sd_out[0] )
   return q

def unnormalize(p):
   q=[]
   for i in range(num_inputs):
      q.append( sds[i]*p[i] + means[i] )
   for i in range(num_inputs, num_inputs + num_outputs):
      q.append( rms_sd_out[0]*p[i] + means[i] )
   return q

def split_patterns(J):
   '''
   copy the (J-1)th tenth of the patterns to validation_patterns, and
   the rest into training_patterns; compute the norms and sds of
   training data, write the training-based normalized patterns
   (training and validation) to files "training.ssv" and
   "validation.ssv"
   '''
   # split the patterns into training_patterns and validation_patterns
   N = len(patterns)
   K = floor(N/10)
   del training_patterns[:]
   del validation_patterns[:]
   for i in range(N):
      if (floor(i/K)==J or (J==9 and floor(i/K)>=9)):
         validation_patterns.append(patterns[i])
      else:
         training_patterns.append(patterns[i])

   # compute the means and standard deviations
   del means[:]
   del sds[:]
   for i in range(num_inputs + num_outputs):
      m = 0
      for p in training_patterns:
         m = m + p[i]
      m = m/len(training_patterns)
      means.append(m)
      sd = 0
      for p in training_patterns:
         sd = sd + (p[i]-m)^2
      sd = sqrt(sd/(len(training_patterns) - 1))
      sds.append(sd)   

   # in the special case of outputs, compute the rms standard
   # deviation (outputs are to be scaled uniformly)
   rms_sd_out[0]=0
   for i in range(num_inputs, num_inputs + num_outputs):
      rms_sd_out[0] += sds[i]^2
   rms_sd_out[0] = sqrt(rms_sd_out[0]/num_outputs)

   # write the normalized patterns to file
   training_file = open("training.ssv", 'w')
   validation_file = open("validation.ssv", 'w')
   for pat in training_patterns:
      q = normalize(pat)
      for i in range(num_inputs + num_outputs):
         if i==0:
            training_file.write("%.13f" % q[0])
         else:
            training_file.write(" %.13f" % q[i]) 
      training_file.write("\n")
   for pat in validation_patterns:
      q = normalize(pat)
      for i in range(num_inputs + num_outputs):
         if i==0:
            validation_file.write("%.13f" % q[0])
         else:
            validation_file.write(" %.13f" % q[i])
      validation_file.write("\n")
   training_file.close()
   validation_file.close()

sp=split_patterns

def nn(n, resetQ):
   wt = []
   for i in weights:
      wt.append([])
   if (resetQ==1):
       shell("nn 1 1 0.1 0")
       for i in range(len(weights)):
          wt[i].append(get_weights()[i])
   del errors[:]
   for k in range(n):
      shell("nn 20 0 0.1 0")
      error_file = open("error.ssv")
      for j in range(len(weights)):
         wt[j].append(get_weights()[j])
      line = error_file.readline()
      temp = string_to_floats(line)
      e=temp[0]*(rms_sd_out[0]^2) # unnormalize the error for reporting
      errors.append(e)
      error_file.close()
   g = Graphics()
   for i in range(len(weights)):
      g = g + finance.TimeSeries(wt[i]).plot()
   print "Final error:", errors[-1]
   print "Weights: "
   for j in range(num_weights):
      weights[j]=get_weights()[j]
   print weights
   return g
#   return finance.TimeSeries(errors).plot()








