#!/bin/sed -f

# unwrap string
s/^"//
s/"$//

# Intermediate result declarations
s/^%/auto v/
s/:/ = /

# End of lines
s/$/;/

# variable references
s/%/v/g

# quaternion members
s/q\[1]/q.w()/g
s/q\[2]/q.x()/g
s/q\[3]/q.y()/g
s/q\[4]/q.z()/g

# subscripts start at 0, not 1
s/\]/ - 1]/g
s/\[1 - 1\]/[0]/g
s/\[2 - 1\]/[1]/g
s/\[3 - 1\]/[2]/g
s/\[4 - 1\]/[3]/g
