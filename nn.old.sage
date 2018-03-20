import subprocess

def shell(acommand):
    '''
    Uses subprocess module to return the output of a shell command
    '''
    return subprocess.Popen(acommand,shell=True,stdout=subprocess.PIPE).communicate()[0]

def print_weights():
   try:
      infile = open("weights.ssv")
      for line in infile:
         print line
      infile.close()
   except IOError:
        print "weight file empty"
pw = print_weights

def get_weights():
   try:
      infile = open("weights.ssv")
      l = infile.readline()
      return stringToFloats(l)
      infile.close()
   except IOError:
      return [0, 0, 0, 0, 0, 0, 0, 0, 0]
gw = get_weights

def set_weights(l):
    """
    usage: sw(l)
    set weights in "weight.ssv" to values in the list l
    """
    outfile = open("weights.ssv", "w")
    for w in l:
        outfile.write("%f " % w)
    return l
sw = set_weights

load "somePlottingTools.py"

def f(x1, x2, w):
    return 3.0*x1 - 2.0*x2 + 0.1
#     return w[6] + w[7]*tanh(w[0] + w[2]*x1 + w[4]*x2) + w[8]*tanh(w[1] + w[3]*x1 + w[5]*x2)
#    return w[0] + w[1]*x1 + w[2]*x2

def weights():
    return [0.1, 3.0, -2.0]
 #   return [0.2, -0.3, 0.5]
w = weights

w = weights() 

def standard_deviation(x):
    return sqrt(variance(x))
sd=standard_deviation

def generate_patterns(n):
   '''
   randomly generate n patterns (x1, x2, f(x1, x2)) as list of
   tuples. the output values (third value in the tuples) get
   normalized over all patterns. then the patterns are plotted and
   written to the file "training.ssv" - one pattern per line
   '''
   outfile = open("training.ssv", 'w')
   outfile2 = open("validation.ssv", 'w')
   x1=[]
   x2=[]
   y=[]
   for i in range(n):   
       x1.append(gauss(0, 1))
       x2.append(gauss(0, 1))
       y.append(f(x1[i], x2[i], weights()))
   m = mean(y)
   s = sd(y)
   ret=[]
   for i in range(n):
#      y[i]=(y[i]-m)/s
      ret.append( (x1[i], x2[i], y[i]) )
      outfile.write( "%.13f %.13f %.13f\n" % (x1[i], x2[i], y[i]) ) 
      outfile2.write( "%.13f %.13f %.13f\n" % (x1[i], x2[i], y[i]) ) 
   outfile.close()
   return point3d(ret, size=7)
gp=generate_patterns

def nn(n,resetQ):
   wt = []
   wt_const = []
   for i in weights():
      wt.append([])
      wt_const.append([])
   if (resetQ==1):
       shell("nn 1 1 0.1 0.0")
       i = 0
       for j in weights():
          wt[i].append(gw()[i])
          wt_const[i].append(weights()[i])
          i = i + 1
   for k in range(n):
      shell("nn 1 0 0.1 0.0")
      i = 0
      for j in weights():
         wt[i].append(gw()[i])
         wt_const[i].append(weights()[i])
         i = i + 1
   print "Final weights:"
   print_weights()
   infile = open("error.ssv")
   g = Graphics()
   i = 0
   for j in weights():
      g = g + finance.TimeSeries(wt[i]).plot(color="blue")
      g = g + finance.TimeSeries(wt_const[i]).plot(color="red")
      i = i + 1
   for line in infile:
       errors = finance.TimeSeries(stringToFloats(line))
   print "Final error:"
   print errors[-1]
   infile.close()
   return g







