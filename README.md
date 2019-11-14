# gridtools_verification

gridtools_verification is part of the GridTools Framework.

## THIS REPOSITORY IS LACKING TEST COVERAGE. IT IS NOT RECOMMENDED TO USE THIS CODE OUTSIDE OF THE COSMO DYNAMICAL CORE

## Installation instructions
- Install serialbox2 with gridtools support: cmake .. -DCMAKE_INSTALL_PREFIX=../install -DGRIDTOOLS_ROOT=<PATH_TO_GRIDTOOLS> -DSERIALBOX_TESTING_GRIDTOOLS=ON
- Install gridtools_verificaiton: cmake .. -DGRIDTOOLS_ROOT=<PATH_TO_GRIDTOOLS> -DSERIALBOX_ROOT=<PATH_TO_SERIALBOX_INSTALLED_ROOT>
