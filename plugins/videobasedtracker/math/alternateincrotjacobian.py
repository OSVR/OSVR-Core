import re
raw = """
auto v1 = 1/2-incrotOP[2,3]/24;
auto v2 = 2*m_incRot[1]*v1;
auto v3 = -incrotOP[2,3]/6;
auto v4 = incrotOP[2,3]/6;
auto v5 = -(incrotOP[2,2]*m_incRot[3])/24;
auto v6 = m_objExtRot[1]*(v5+v4)+m_objExtRot[2]*(v3+v2+1);
auto v7 = -incrotOP[1,3]/6;
auto v8 = v7+1;
auto v9 = 1/2-incrotOP[1,3]/24;
auto v10 = incrotOP[2,2]*v9;
auto v11 = v3+1;
auto v12 = incrotOP[1,1]*v1;
auto v13 = m_objExtRot[1]*(v10-m_incRot[2]*v8)+m_objExtRot[2]*(v12+m_incRot[1]*v11)+m_objExtRot[3]+m_xlate[3];
auto v14 = 1/v13^2;
auto v15 = -incrotOP[1,2]/6;
auto v16 = v15+1;
auto v17 = 1/2-incrotOP[1,2]/24;
auto v18 = incrotOP[3,3]*v17;
auto v19 = m_objExtRot[3]*(m_incRot[2]*v8+v10)+m_objExtRot[2]*(v18-m_incRot[3]*v16)+m_objExtRot[1]+m_xlate[1];
auto v20 = 1/v13;
auto v21 = -(m_incRot[2]*incrotOP[3,3])/24;
auto v22 = 2*m_incRot[2]*v9;
auto v23 = incrotOP[1,3]/6;
auto v24 = -(m_incRot[1]*incrotOP[3,3])/24;
auto v25 = -(incrotOP[1,1]*m_incRot[3])/24;
auto v26 = m_objExtRot[2]*(v7+v25)+m_objExtRot[1]*(v23+v22-1);
auto v27 = incrotOP[1,2]/6;
auto v28 = 2*m_incRot[3]*v17;
auto v29 = -(m_incRot[1]*incrotOP[2,2])/24;
auto v30 = -(incrotOP[1,1]*m_incRot[2])/24;
auto v31 = m_objExtRot[2]*(v30+v15)+m_objExtRot[1]*(v29+v27);
auto v32 = m_objExtRot[1]*(v18+m_incRot[3]*v16)+m_objExtRot[3]*(v12-m_incRot[1]*v11)+m_objExtRot[2]+m_xlate[2];
Eigen::Matrix<double,2,3> ret;
ret <<
 v20*(m_objExtRot[2]*(v21+v4)+(v5+v3)*m_objExtRot[3])*fl-v6*v14*v19*fl,
 v20*(m_objExtRot[2]*(v24+v23)+(v22+v7+1)*m_objExtRot[3])*fl-v26*v14*v19*fl,
 v20*((v29+v15)*m_objExtRot[3]+m_objExtRot[2]*(v28+v27-1))*fl-v31*v14*v19*fl,
 v20*(m_objExtRot[1]*(v21+v3)+(v4+v2-1)*m_objExtRot[3])*fl-v6*v14*v32*fl,
 v20*(m_objExtRot[1]*(v24+v7)+(v25+v23)*m_objExtRot[3])*fl-v26*v14*v32*fl,
 v20*((v30+v27)*m_objExtRot[3]+m_objExtRot[1]*(v28+v15+1))*fl-v31*v14*v32*fl;
return ret;
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
                ).replace('fl', 'm_cam.focalLength'
                ).replace('1/2', '0.5'
                ).replace('v13^2', '(v13*v13)'
                         ).string()
print(out)
