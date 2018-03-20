
   training_file = open("training.ssv", 'w')
   validation_file = open("validation.ssv", 'w')

   for i in range(N - K):
      training_file.write( "%f %f %f\n" % (x1, x2, y) ) 
   for i in range(K):
      x1 = patterns[N - K + i][0]
      x2 = patterns[N - K + i][1]
      y =  patterns[N - K + i][2]

   training_file.close()
   validation_file.close()
