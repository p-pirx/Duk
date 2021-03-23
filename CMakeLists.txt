add_executable(
  duck
  duck.cpp
)

add_resource(duck mallard.tga)

# Pull in pico libraries that we need
target_link_libraries(duck pico_stdlib pico_display)

# create map/bin/hex file etc.
pico_add_extra_outputs(duck)


