if (USE_TASK_RW)
    add_subdirectory(running_wave)
endif()

if (USE_TASK_SW)
    add_subdirectory(spherical_wave)
endif()

if (USE_TASK_TF)
    add_subdirectory(tight_focusing)
endif()

