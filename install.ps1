# Copyright (c) 2021, Maksym Motorny. All rights reserved.
# Use of this source code is governed by a 3-clause BSD license that can be
# found in the LICENSE file.

Param([string]$Configuration = "RelWithDebInfo")

$PathToXPlane11 = cat $Env:LOCALAPPDATA\x-plane_install_11.txt
echo "X-Plane 11 is found at $PathToXPlane11"

$CurrentConfig = cat build\CurrentConfig.txt
echo "Build configuration is $CurrentConfig"

cmake --install build --prefix $PathToXPlane11\Resources\plugins\DeviceDatarefs --config $CurrentConfig --component DeviceDatarefs
