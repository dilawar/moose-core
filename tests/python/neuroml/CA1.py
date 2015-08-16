## Aditya Gilra, NCBS, Bangalore, 2012
## Dilawar Singh, NCBS, 2015

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python/')
import moose
from moose.utils import *
import moose.utils as mu
import count

from moose.neuroml.NeuroML import NeuroML

import unittest

simdt = 10e-6 # s
plotdt = 10e-6 # s
runtime = 0.19 # s

def loadGran98NeuroML_L123(filename):
    neuromlR = NeuroML()
    populationDict, projectionDict = \
        neuromlR.readNeuroMLFromFile(filename)
    soma_path = populationDict['CA1group'][1][0].path+'/Seg0_soma_0_0'
    somaVm = setupTable('somaVm',moose.Compartment(soma_path),'Vm')
    soma = moose.Compartment(soma_path)
    moose.reinit()
    moose.start(runtime)
    tvec = arange(0.0,runtime,simdt)
    res =  count.spike_train_simple_stat( somaVm.vector )
    return res['number of spikes']

if __name__ == "__main__":
    if len(sys.argv)<2:
        filename = "CA1soma.net.xml"
    else:
        filename = sys.argv[1]
    loadGran98NeuroML_L123(filename)


