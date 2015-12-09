import re
raw = """
auto v1 = m_rot[1,3]*m_beacon[3]+m_rot[1,2]*m_beacon[2]+m_beacon[1]*m_rot[1,1]+xlate[1];
auto v2 = m_beacon[3]*m_rot[3,3]+m_beacon[2]*m_rot[3,2]+m_beacon[1]*m_rot[3,1]+xlate[3];
auto v3 = 1/v2^2;
auto v4 = 1/v2;
auto v5 = m_rot[2,3]*m_beacon[3]+m_beacon[2]*m_rot[2,2]+m_beacon[1]*m_rot[2,1]+xlate[2];
Eigen::Matrix<double,2,3> ret;
ret <<
 m_rot[1,1]*v4*fl-v1*m_rot[3,1]*v3*fl,
 m_rot[1,2]*v4*fl-v1*m_rot[3,2]*v3*fl,
 m_rot[1,3]*v4*fl-v1*m_rot[3,3]*v3*fl,
 m_rot[2,1]*v4*fl-v5*m_rot[3,1]*v3*fl,
 m_rot[2,2]*v4*fl-v5*m_rot[3,2]*v3*fl,
 m_rot[2,3]*v4*fl-v5*m_rot[3,3]*v3*fl;
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
                ).replace('principalPoint', 'm_cam.principalPoint'
                ).replace('v2^2', '(v2*v2)'
                ).replace('fl', 'm_cam.focalLength'
                ).replace('xlate', 'm_xlate'
                         ).string()
print(out)
#print(.sub(r'(\1 - 1, \2 - 1)', input))
