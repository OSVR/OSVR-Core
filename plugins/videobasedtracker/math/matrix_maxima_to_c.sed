#!/bin/sed -f
# Start the output with a matrix declaration and comma initializer.
s/matrix[(]\[/Eigen::MatrixXd result;\nresult <</

# replace the row separators
s_],\[_,/* end of row */_g

# variable references
s/%/v/g

# quaternion members
s/q\[1]/q.w()/g
s/q\[2]/q.x()/g
s/q\[3]/q.y()/g
s/q\[4]/q.z()/g


# Finish the output cleanly
s/])$/;/

# subscripts start at 0, not 1
s/\]/ - 1]/g
