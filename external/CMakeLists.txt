
# enable FetchContent
include(FetchContent)

##########Pull in event library ####################
#FetchContent_Declare(
#	cee_events
#	GIT_REPOSITORY https://github.com/linre-90/cee_events.git
#	GIT_TAG master
#)
#FetchContent_MakeAvailable(cee_events)

##########Pull in raylib ####################
FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG 5.0
)
# make available
FetchContent_MakeAvailable(raylib)