# -*- coding: utf-8 -*-

"""test_muparser.py:
Modified from https://elifesciences.org/articles/25827 , Fig4.py
"""
import sys
import numpy as np
import moose
import abstrModelEqns9 as ame
import rdesigneur as rd
print( "[INFO ] Using moose from %s" % moose.__file__ )

def singleCompt( name, params ):
    print('=============')
    print('[INFO] Making compartment %s' % name)
    mod = moose.copy( '/library/' + name + '/' + name, '/model' )
    A = moose.element( mod.path + '/A' )
    Z = moose.element( mod.path + '/Z' )
    Z.nInit = 1
    Ca = moose.element( mod.path + '/Ca' )
    CaStim = moose.element( Ca.path + '/CaStim' )

    print( '\n\n[INFO] CaStim %s' % CaStim.path )
    runtime = params['preStimTime'] + params['postStimTime']
    steptime = 50
    CaStim.expr += '+x2*(t>100+'+str(runtime)+')*(t<100+'+str(runtime+steptime)+ ')'
    print("[INFO] CaStim.expr = %s" % CaStim.expr)
    tab = moose.Table2( '/model/' + name + '/Atab' )
    ampl = moose.element( mod.path + '/ampl' )
    phase = moose.element( mod.path + '/phase' )
    moose.connect( tab, 'requestOut', A, 'getN' )
    ampl.nInit = params['stimAmplitude'] * 1
    phase.nInit = params['preStimTime']

    ksolve = moose.Ksolve( mod.path + '/ksolve' )
    stoich = moose.Stoich( mod.path + '/stoich' )
    stoich.compartment = mod
    stoich.ksolve = ksolve
    stoich.path = mod.path + '/##'

    print( 'REINIT AND START' )
    moose.reinit()
    runtime += 100 + steptime*2
    moose.start( runtime )
    t = np.arange( 0, runtime + 1e-9, tab.dt )
    return name, t, tab.vector


def run():
    panelC = []
    panelCticks = []
    panelC.append( singleCompt( 'negFB', ame.makeNegFB( [] ) ) )
    panelC.append( singleCompt( 'negFF', ame.makeNegFF( [] ) ) )
    panelC.append( singleCompt( 'fhn', ame.makeFHN( [] ) ) )
    panelC.append( singleCompt( 'bis', ame.makeBis( [] ) ) )

    panelCticks.append( np.arange( 0, 15.00001, 5 ) )
    panelCticks.append( np.arange( 0, 1.50001, 0.5 ) )
    panelCticks.append( np.arange( 0, 5.00002, 1 ) )
    panelCticks.append( np.arange( 0, 5.00002, 1 ) )
    moose.delete( '/model' )
    for i in zip( panelC, panelCticks, list(range( len( panelC ))) ):
        plotPos = i[2] + 5
        doty = i[1][-1] * 0.95
        print('doty',  doty )

def runPanelDEFG( name, dist, seqDt, numSpine, seq, stimAmpl ):
    preStim = 10.0
    blanks = 20
    rdes = rd.rdesigneur(
        useGssa = False,
        turnOffElec = True,
        chemPlotDt = 0.1,
        diffusionLength = 1e-6,
        cellProto = [['cell', 'soma']],
        chemProto = [['dend', name]],
        chemDistrib = [['dend', 'soma', 'install', '1' ]],
        plotList = [['soma', '1', 'dend' + '/A', 'n', '# of A']],
    )
    rdes.buildModel()
    A = moose.vec( '/model/chem/dend/A' )
    Z = moose.vec( '/model/chem/dend/Z' )
    print(moose.element( '/model/chem/dend/A/Adot' ).expr)
    print(moose.element( '/model/chem/dend/B/Bdot' ).expr)
    print(moose.element( '/model/chem/dend/Ca/CaStim' ).expr)
    phase = moose.vec( '/model/chem/dend/phase' )
    ampl = moose.vec( '/model/chem/dend/ampl' )
    vel = moose.vec( '/model/chem/dend/vel' )
    vel.nInit = 1e-6 * seqDt
    ampl.nInit = stimAmpl
    stride = int( dist ) / numSpine
    phase.nInit = 10000
    Z.nInit = 0
    for j in range( numSpine ):
        k = int( blanks + j * stride )
        Z[k].nInit = 1
        phase[k].nInit = preStim + seq[j] * seqDt

    moose.reinit()
    runtime = 50
    snapshot = preStim + seqDt * (numSpine - 0.8)
    print(snapshot)
    moose.start( snapshot )
    avec = moose.vec( '/model/chem/dend/A' ).n
    moose.start( runtime - snapshot )
    tvec = []
    for i in range( 5 ):
        tab = moose.element( '/model/graphs/plot0[' + str( blanks + i * stride ) + ']' )
        dt = tab.dt
        tvec.append( tab.vector )
    moose.delete( '/model' )
    return dt, tvec, avec

def makePassiveSoma( name, length, diameter ):
    elecid = moose.Neuron( '/library/' + name )
    dend = moose.Compartment( elecid.path + '/soma' )
    dend.diameter = diameter
    dend.length = length
    dend.x = length
    return elecid

if __name__ == '__main__':
    moose.Neutral( '/library' )
    moose.Neutral( '/model' )
    run()
    print( 'All done' )
