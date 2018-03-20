import subprocess
def make_it_so(acommand):
    '''
    Uses subprocess module to return the output of a shell command
    '''
    return subprocess.Popen(acommand,shell=True,stdout=subprocess.PIPE).communicate()[0]

a
