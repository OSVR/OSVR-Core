import re
raw = """
auto v1 = m_rotatedObjPoint[1]+pos[1];
auto v2 = m_rotatedObjPoint[3]+pos[3];
auto v3 = 1/v2^2;
auto v4 = 1/v2;
auto v5 = m_rotatedObjPoint[2]+pos[2];
Eigen::Matrix<double,2,3> ret;
ret <<
 -v1*m_rotatedObjPoint[2]*v3*fl,
 m_rotatedObjPoint[3]*v4*fl+m_rotatedObjPoint[1]*v1*v3*fl,
 -m_rotatedObjPoint[2]*v4*fl,
 (-m_rotatedObjPoint[3]*v4*fl)-m_rotatedObjPoint[2]*v5*v3*fl,
 m_rotatedObjPoint[1]*v5*v3*fl,
 m_rotatedObjPoint[1]*v4*fl;
"""

matrixIndexPattern = re.compile(r'\[([0-9]),([0-9])]', re.VERBOSE)
def replaceMatrixIndices(inString):
    def matchfunc(match):
        return '({}, {})'.format(int(match.group(1)) - 1, int(match.group(2)) - 1)
        
    return matrixIndexPattern.sub(matchfunc, inString)

vectorIndexPattern = re.compile(r'\[([0-9])]', re.VERBOSE)
def replaceVectorIndices(inString):
    def matchfunc(match):
        return '[{}]'.format(int(match.group(1)) - 1)
    return vectorIndexPattern.sub(matchfunc, inString)

class Chainable:
    def __init__(self, inString):
        self._string = replaceVectorIndices(replaceMatrixIndices(inString))

    def replace(self, old, new):
        self._string = self._string.replace(old, new)
        return self
    def string(self):
        return self._string
out = Chainable(raw
                ).replace('v2^2', '(v2*v2)'
                ).replace('fl', 'm_cam.focalLength'
                ).replace('pos', 'm_xlate'
                         ).string()
print(out)
#print(.sub(r'(\1 - 1, \2 - 1)', input))
