#defaultVariance = 3e-6
#sideVariance = 5e-6
#edgeVariance = 8e-6

defaultVariance = 3.0
sideVariance = 5.0
edgeVariance = 8.0

def getVariance(oneBasedIndex):
    # side beacons
    if oneBasedIndex in [1,2,10,5,6,7,9,3,4,8,9]:
        return sideVariance
    elif oneBasedIndex in [12,13,14,15,18,21,22,23,24,25,27,28,29,30]:
        return edgeVariance
    else:
        return defaultVariance


print(
    ",".join([str(getVariance(i)) for i in range(1, 35)]))
