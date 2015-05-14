import bpy

#============================================================================
# Edit below here

# Edit the two file names here to change them to match the files you want to
# open.

locationFile = open('HDK_LED_locations_3rmt.csv')
patternFile = open('HDK_LED_patterns.txt')

# Edit the Y offset to by applied to move the group of beacons
# associated with different sensors to avoid overlap.  This is
# in cm.

sensorYOffset = 8.5

# Edit the number of repetitions of flashings done here.
# This will not change the camera behavior, which is set to
# move to the right over one set of flashes and then to the
# left over a second set.

numRepeats = 2

# Probably don't edit below here.
#============================================================================

locations = locationFile.readlines()
patterns = patternFile.readlines()

def setMaterial(ob, mat):
    me = ob.data
    me.materials.append(mat)

def makeMaterial(name, diffuse, specular, alpha):
    mat = bpy.data.materials.new(name)
    mat.diffuse_color = diffuse
    mat.diffuse_shader = 'LAMBERT' 
    mat.diffuse_intensity = 1.0 
    mat.specular_color = specular
    mat.specular_shader = 'COOKTORR'
    mat.specular_intensity = 0.0
    mat.alpha = alpha
    mat.ambient = 1
    return mat

# Replaces select_name() function that was removed in
# version 2.62.  The select_pattern() function does not quite
# do the same thing.
def selectNamed(name):
    bpy.ops.object.select_all(action='DESELECT')    
    myobject = bpy.data.objects[name]
    myobject.select = True
    bpy.context.scene.objects.active = myobject
    
# Faster version of the above that pulls the entry from the
# specified entry in a list
def selectObject(myObject):
    bpy.ops.object.select_all(action='DESELECT')
    myObject.select = True
    bpy.context.scene.objects.active = myObject


# Create some materials
bright = makeMaterial('Bright', (1,1,1), (1,1,1), 1)

# Make a single sphere that will be duplicated to make the beacons.
# Units are in centimeters throughout the model.  Put this one very
# far away so it will be clipped and we'll never see it.
bpy.ops.mesh.primitive_uv_sphere_add(size = 0.25, location=(1000.0,1000.0,1000.0))

# Make a sphere for each beacon with the bright material.
sphereNum = 0
listIndex = 0
sphereList = list()
for location in locations:

    # Skip the first line of the locations file because it is a header.
    # If this beacon is from a sensor number larger than 0, shift it up
    # in Y by the offset amount so that it will not overlap other sensors.
    if sphereNum > 0:
        # Pull the X, Y, Z coordinates out as the last three entries
        # in the line, skipping
        currentLocation = location.split(",")

        # Construct a new sphere        
        selectNamed("Sphere")
        bpy.ops.object.duplicate(linked=False)
        setMaterial(bpy.context.object, bright)
        bpy.ops.object.shade_smooth()
        sphereList.append(bpy.context.scene.objects.active)

        # Move the sphere to its correct location
        # Remember to convert from mm to cm
        # Name it after the 0-indexed beacon number (0-39 for 40 beacons)
        selectObject(sphereList[listIndex])
        myname = 'OSVR_beacon'+str(sphereNum-1).zfill(3)
        bpy.context.active_object.name = myname
        bpy.context.active_object.location = (float(currentLocation[2])/10, float(currentLocation[3])/10 + sensorYOffset * float(currentLocation[5]), float(currentLocation[4])/10)
        listIndex += 1
    
    # Always increment the sphere number even if we're skipping this sphere
    sphereNum += 1

# Keep track of which frame we're in.  There will be one for every
# character in a single line of the patterns file.  Each line in that
# file should have the same number of frames.

frameNum = 1
rep = 0
while rep < numRepeats:

    count = 0;
    while count < len(patterns[0])-1:
                
        bpy.context.scene.frame_set(frameNum)
        print("Handling time = ", frameNum)

        # Set each beacon's size based on the frame we're in
        # and its entry in the string.  It gets smaller for a
        # dim beacon and larger for a bright one.
        # TODO: we'd like to maybe adjust its diffuse color as
        # well, to make it dimmer, but we're probably going to
        # be overblowing the sensor so size is the main effect.
        beaconId = 0;
        for sphere in sphereList:
            selectObject(sphereList[beaconId])
            if patterns[beaconId][count] == '*':
                bpy.context.object.scale = [1.00, 1.00, 1.00]
            else:
                bpy.context.object.scale = [0.75, 0.75, 0.75]
            bpy.ops.anim.keyframe_insert_menu(type='Scaling')
            beaconId += 1

        count += 1

        # Next frame we will set.
        bpy.data.scenes["Scene"].frame_end = frameNum
        frameNum += 1
        
    rep += 1

