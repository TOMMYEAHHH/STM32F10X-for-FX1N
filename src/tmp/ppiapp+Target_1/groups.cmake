# groups.cmake

# group src
add_library(Group_src OBJECT
  "${SOLUTION_ROOT}/src/port_a.asm"
  "${SOLUTION_ROOT}/src/main.c"
  "${SOLUTION_ROOT}/src/stm32f10x_it.c"
  "${SOLUTION_ROOT}/src/system_stm32f10x.c"
  "${SOLUTION_ROOT}/src/SysTick.c"
  "${SOLUTION_ROOT}/src/sio.c"
)
target_include_directories(Group_src PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_src PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_src_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_src_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_src PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_src PUBLIC
  Group_src_ABSTRACTIONS
)
set(COMPILE_DEFINITIONS
  STM32F10X_MD
)
cbuild_set_defines(AS_ARM COMPILE_DEFINITIONS)
set_source_files_properties("${SOLUTION_ROOT}/src/port_a.asm" PROPERTIES
  COMPILE_FLAGS "${COMPILE_DEFINITIONS}"
)

# group fwlib
add_library(Group_fwlib OBJECT
  "${SOLUTION_ROOT}/fwlib/src/misc.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_dma.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_exti.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_gpio.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_pwr.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_rcc.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_sdio.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_tim.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_usart.c"
  "${SOLUTION_ROOT}/fwlib/src/stm32f10x_flash.c"
)
target_include_directories(Group_fwlib PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_fwlib PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_fwlib_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_fwlib_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_fwlib PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_fwlib PUBLIC
  Group_fwlib_ABSTRACTIONS
)

# group core
add_library(Group_core OBJECT
  "${SOLUTION_ROOT}/core/startup_stm32f10x_md.s"
  "${SOLUTION_ROOT}/core/core_cm3.c"
)
target_include_directories(Group_core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_core_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_core_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_core PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_core PUBLIC
  Group_core_ABSTRACTIONS
)
set(COMPILE_DEFINITIONS
  STM32F10X_MD
)
cbuild_set_defines(AS_ARM COMPILE_DEFINITIONS)
set_source_files_properties("${SOLUTION_ROOT}/core/startup_stm32f10x_md.s" PROPERTIES
  COMPILE_FLAGS "${COMPILE_DEFINITIONS}"
)

# group fx1n
add_library(Group_fx1n OBJECT
  "${SOLUTION_ROOT}/fx1n/fx1nhelper.c"
  "${SOLUTION_ROOT}/fx1n/fx1n.c"
  "${SOLUTION_ROOT}/fx1n/fx1nfnc.c"
  "${SOLUTION_ROOT}/fx1n/fx1niap.c"
  "${SOLUTION_ROOT}/fx1n/fx1nuart.c"
)
target_include_directories(Group_fx1n PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
)
target_compile_definitions(Group_fx1n PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
add_library(Group_fx1n_ABSTRACTIONS INTERFACE)
target_link_libraries(Group_fx1n_ABSTRACTIONS INTERFACE
  ${CONTEXT}_ABSTRACTIONS
)
target_compile_options(Group_fx1n PUBLIC
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_OPTIONS>
)
target_link_libraries(Group_fx1n PUBLIC
  Group_fx1n_ABSTRACTIONS
)
