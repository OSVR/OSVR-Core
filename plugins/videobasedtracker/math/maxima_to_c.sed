#!/bin/sed -f
# Intermediate result declarations
s/^%/auto v/
s/:/ = /

# End of lines - assumes you've split ,% to ,\n% already
s/,$/;/

# variable references
s/%/v/g

# quaternion members
s/q\[1]/q.w()/g
s/q\[2]/q.x()/g
s/q\[3]/q.y()/g
s/q\[4]/q.z()/g

# subscripts start at 0, not 1
s/\]/ - 1]/g
