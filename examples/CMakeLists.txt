IF(ENABLE_GTOP_DATABASE)

ADD_EXECUTABLE(hm_2_asteroids hm_2_asteroids.cpp)
TARGET_LINK_LIBRARIES(hm_2_asteroids ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(migrate_or_not migrate_or_not.cpp)
TARGET_LINK_LIBRARIES(migrate_or_not ${MANDATORY_LIBRARIES} pagmo_static)

IF(ENABLE_SNOPT)
	ADD_EXECUTABLE(gtoc_2_turin gtoc_2_turin.cpp)
        TARGET_LINK_LIBRARIES(gtoc_2_turin ${MANDATORY_LIBRARIES} pagmo_static)
ENDIF(ENABLE_SNOPT)

ENDIF(ENABLE_GTOP_DATABASE)

IF(ENABLE_GSL)
	ADD_EXECUTABLE(evolve_spheres evolve_spheres.cpp)
        TARGET_LINK_LIBRARIES(evolve_spheres ${MANDATORY_LIBRARIES} pagmo_static)
	ADD_EXECUTABLE(evolve_spheres_racing evolve_spheres_racing.cpp)
        TARGET_LINK_LIBRARIES(evolve_spheres_racing ${MANDATORY_LIBRARIES} pagmo_static)
ENDIF(ENABLE_GSL)

ADD_EXECUTABLE(constraints_handling constraints_handling.cpp)
TARGET_LINK_LIBRARIES(constraints_handling ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_death_penalty cstrs_death_penalty.cpp)
TARGET_LINK_LIBRARIES(cstrs_death_penalty ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_co_evolution cstrs_co_evolution.cpp)
TARGET_LINK_LIBRARIES(cstrs_co_evolution ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_self_adaptive cstrs_self_adaptive.cpp)
TARGET_LINK_LIBRARIES(cstrs_self_adaptive ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_self_adaptive_island cstrs_self_adaptive_island.cpp)
TARGET_LINK_LIBRARIES(cstrs_self_adaptive_island ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_immune_system cstrs_immune_system.cpp)
TARGET_LINK_LIBRARIES(cstrs_immune_system ${MANDATORY_LIBRARIES} pagmo_static)

ADD_EXECUTABLE(cstrs_core cstrs_core.cpp)
TARGET_LINK_LIBRARIES(cstrs_core ${MANDATORY_LIBRARIES} pagmo_static)
