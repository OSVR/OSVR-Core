
# Add a "custom target" for just putting the headers in the IDE.
add_custom_target(SerializationExamples
    SOURCES
    SerializationTraitExample_Simple.h
    SerializationTraitExample_Complicated.h)

set_target_properties(SerializationExamples PROPERTIES
    FOLDER "OSVR Core Internal Examples")
