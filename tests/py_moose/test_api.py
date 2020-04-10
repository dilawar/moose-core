__author__     = "Dilawar Singh"
__copyright__  = "Copyright 2019-, Dilawar Singh"
__maintainer__ = "Dilawar Singh"
__email__      = "dilawars@ncbs.res.in"


import moose
import random
import numpy as np


def test_children():
    a1 = moose.Neutral('/a')
    a2 = moose.Neutral('/a/b')
    a3 = moose.Neutral('/a/b/c1')
    moose.Neutral('/a/b/c2')
    assert len(a1.children) == 1
    assert len(a2.children) == 2
    moose.le(a1)
    moose.le(a2)
    moose.le(a3)
    moose.setCwe(a3)
    s = moose.getCwe()
    assert s == a3, (s, a3)
    a11 = a1.children[0]
    ax = moose.element(a1)
    ax1 = ax.children[0]
    assert ax == a1
    assert ax1 == a11
    assert a11.isA['Neutral'], a11.isA
    assert ax1.isA['Neutral'], a11.isA
    print("test_children is done")


def test_other():
    a1 = moose.Pool('/ada')
    assert a1.className == 'Pool', a1.className
    finfo = moose.getFieldDict(a1.className)
    s = moose.Streamer('/asdada')
    p = moose.PulseGen('pg1')
    assert p.delay[0] == 0.0
    p.delay[1] = 0.99
    assert p.delay[1] == 0.99, p.delay[1]


def test_vec():
    a = moose.Pool('/p111', 100)
    v = moose.vec(a)
    assert len(v) == 100, len(v)
    assert v == v.vec
    assert v[0] == v.vec[0], (v[0], v.vec[0])
    x = [random.random() for i in range(100)]
    v.conc = x
    assert sum(v.conc) == sum(x)
    assert np.allclose(v.conc, x), (v.conc, x)

    # assign bool to double.
    y = [float(x < 5) for x in range(100) ]
    v.concInit = y
    assert (v.concInit[:5] == 1.0).all(), v.concInit[:5]
    assert (v.concInit[5:] == 0.0).all(), v.concInit[5:]

def test_finfos():
    s = moose.SimpleSynHandler('synh')

    s.numSynapses = 10
    assert s.numSynapses == 10

    syns = s.synapse.vec
    print(s.synapse, '111')
    s8a = s.synapse[8]
    s8b = s.synapse[-2]
    assert s8a == s8b, (s8a, s8b)

    # negative indexing.
    assert syns[-2] == syns[len(syns)-2]
    assert len(syns) == 10
    for i, s in enumerate(syns):
        s.weight = 9.0
    for s in syns:
        assert s.weight == 9.0

    # this is a shorthand for above for loop.
    syns.weight = 11.121
    assert np.allclose(syns.weight, 11.121), syns.weight

     # try:
     #     print(syns[11])
     # except Exception as e:
     #     print(e, "Great. We must got an exception here")
     # else:
     #     print(syns[11])
     #     raise Exception("This should have failed")

    a = moose.Pool('x13213')
    a.concInit = 0.1
    assert 0.1 == moose.getField(a, 'concInit')

    # Now get some finfos.
    a = moose.element('/classes/Compartment')


def test_inheritance():
    ta = moose.Table2('/tab2', 10)
    tb = moose.wildcardFind('/##[TYPE=Table2]')
    assert len(tb) == len(ta.vec)
    for i, (t1, t2) in enumerate(zip(tb, ta.vec)):
        assert t1 == t2, (t1, t2)
        assert t1.id == t2.id
        assert t1.dataIndex == t2.dataIndex
        assert t1.path == t2.path

    a = moose.CubeMesh('/dadada')
    isinstance(a, moose.CubeMesh)
    assert isinstance(a, moose.CubeMesh)
    aa = moose.wildcardFind('/##[TYPE=CubeMesh]')[0]
    assert a == aa
    # This must be true for isinstance to work.
    assert isinstance(aa, moose.CubeMesh), (a.__class__, aa.__class__)

    a = moose.CubeMesh('yapf')
    assert a.isA['CubeMesh']
    assert a.isA['ChemCompt']


def test_delete():
    a = moose.Neutral('/xxx')
    b = moose.Neutral('/xxx/1')
    c = moose.Neutral('/xxx/1/2')
    d = moose.Neutral('/xxx/2')
    e = moose.Neutral('/xxx/2/2')
    f = moose.Neutral('/xxx/2/2/3')
    x = moose.wildcardFind('/xxx/##')
    assert len(x) == 5
    moose.delete(e)
    x = moose.wildcardFind('/xxx/##')
    assert len(x) == 3
    moose.delete(a)
    x = moose.wildcardFind('/xxx/##')
    assert len(x) == 0


def test_wrapper():
    a = moose.Pool('/dadadada', concInit=9.99, nInit=10)
    assert a.nInit == 10
    f = moose.Function('/fun1', expr='x0+x1+A+B')
    assert f.expr == 'x0+x1+A+B'
    assert f.numVars == 4, f.numVars


def test_access():
    a1 = moose.Pool('ac1')
    try:
        a2 = moose.Compartment('ac1')
    except Exception:
        pass
    else:
        raise RuntimeError("Should have failed.")
    a2 = moose.element(a1)
    a3 = moose.element(a1.path)
    assert a2 == a3, (a2, a3)

def test_element():
    a = moose.Pool('xxxx', 2)
    ae = moose.element(a)
    assert ae.parent == a.parent, (ae.parent, a.parent)

def test_typing():
    a = moose.Pool('x123y', 100)

    a.concInit = True
    assert a.concInit == 1.0, a.concInit
    a.concInit = False
    assert a.concInit == 0.0, a.concInit

    av = moose.vec(a)
    av.concInit = 1.0
    assert np.allclose(av.concInit, 1.0), av.concInit
    av.concInit = 0.012
    assert np.allclose(av.concInit, 0.012), av.concInit

    av.concInit = True
    assert np.allclose(av.concInit, 1.0), av.concInit

def main():
    test_children()
    test_finfos()
    test_other()
    test_delete()
    test_wrapper()
    test_inheritance()
    test_access()
    test_element()
    test_vec()
    test_typing()

if __name__ == '__main__':
    main()
