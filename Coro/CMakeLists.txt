set(LLVM_LINK_COMPONENTS
  Core
  ExecutionEngine
  Object
  Support
  native
  )

add_kaleidoscope_chapter(Kaleidoscope-Coro
  coro.cpp
  )

export_executable_symbols(Kaleidoscope-Coro)