#ifndef SIMULATIONS_HPP
#define SIMULATIONS_HPP

#include <iostream>
#include <bits/stdc++.h>
#include "Exception.hpp"
#include "PetscException.hpp"
#include "OutputFileHandler.hpp"
#include "FHNOdeSystem.hpp"
#include "myFHNOdeSystem.hpp"
#include "UterineSimpleCellFactory.hpp"
#include "UterineRegularCellFactory.hpp"
#include "UterineZeroCellFactory.hpp"
#include "UterineSimpleCellFactory3d.hpp"
#include "UterineSimpleCellFactory3d_2type.hpp"
#include "UterineRegularCellFactory3d.hpp"
#include "UterineZeroCellFactory3d.hpp"
#include "UterineZeroCellFactory3d_2type.hpp"
#include "ChastePoint.hpp"
#include "ChasteCuboid.hpp"
#include "ChasteParameters_2017_1.hpp"
#include "DistributedTetrahedralMesh.hpp"
#include "UterineConductivityModifier.hpp"
#include "VtkMeshWriter.hpp"

void simulation_0d();
void simulation_2d();
void simulation_2d_2type();
void simulation_3d();
void simulation_3d_2type();

#endif // SIMULATIONS_HPP

