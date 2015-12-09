defaultVariance = 3.0
highVariance = 6.0
turnedOffVariance = 15.0
def getVariance(oneBasedIndex):
    # side beacons
    if oneBasedIndex in [1,2,10,5,6,7]:
        return turnedOffVariance
    elif oneBasedIndex in [3,26,33,9,8]:
        return highVariance
    else:
        return defaultVariance


print(
    ",".join([str(getVariance(i)) for i in range(1, 35)]))
