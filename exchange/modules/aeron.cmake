if(NOT TARGET aeron)

  # Module flag
  set(AERON_MODULE Y)

  # Module subdirectory
  add_subdirectory("aeron")

endif()
