PROJECT_NAME := frequency

# This line has to be included into the make file
# to include components that are located somewhere 
# but not in "component" directory

EXTRA_COMPONENT_DIRS := $(realpath ../..)
EXCLUDE_COMPONENTS := test

include $(IDF_PATH)/make/project.mk