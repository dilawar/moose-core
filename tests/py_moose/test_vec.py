# -*- coding: utf-8 -*-
import moose

def test_vec():
    foo = moose.Pool('/foo1', 500)
    bar = moose.vec('/foo1')
    assert len(bar) == 500, len(bar)
    for i in range(len(bar)):
        print(bar[i])

def test_vec2():
    iaf = moose.vec('/iaf', n=10, dtype='IntFire')
    v1 = iaf.vec
    v2 = iaf.vec
    assert v1 == v2
    assert id(v1) == id(v2)
    iaf.Vm = range(10)
    print(iaf.Vm)
    print("All done")
    assert iaf[5].Vm == 5, iaf[5].Vm
    print(iaf.Vm)


if __name__ == '__main__':
    test_vec()
    test_vec2()
