set ( LIBMIXED_SOURCES
  coroutine.cpp
  factory.cpp
  specialize.cpp
  fiber.cpp
  manager.cpp
  scheduler_tools.cpp
  scheduler.cpp
  userspace_scheduler.cpp
  )

add_library ( libmixed SHARED ${LIBMIXED_SOURCES} )
