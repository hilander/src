set ( LIBMIXED_SOURCES
  coroutine.cpp
  factory.cpp
  specialize.cpp
  fiber.cpp
  manager.cpp
  scheduler_tools.cpp
  scheduler.cpp
  userspace_scheduler.cpp
  scheduler_tools.cpp
  message_queue.cpp
	socket.cpp
	mutex_trylock.cpp
  )

add_library ( libmixed SHARED ${LIBMIXED_SOURCES} )
